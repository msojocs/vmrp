# DOTA 浏览器插件启动崩溃调试记录

## 2026-06-27 初始记录

- 目标命令：`pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`。
- 现象：自动下载浏览器插件后，确认下载结果并启动浏览器插件时崩溃。
- 约束：不使用 `xvfb`；避免全量 trace；修复不能写应用特定判断；代码修改必须带原因注释；需要结合反汇编定位 BUG；可用 PPM 验证；调查进度需要持续保存。
- 当前工作区已有用户改动：
  - `docs/prompt.md` 更新为本轮 DOTA 浏览器插件崩溃任务。
  - `test/e2e/dota/download-plugin.test.ts` 新增 `"下载浏览器插件 - 成功"` 用例，末尾仍有 TODO throw，等待崩溃修复后补断言。
- 并行调查：
  - 子 Agent `Leibniz`：只读调查 ARM EXT 崩溃、寄存器和反汇编证据。
  - 子 Agent `Harvey`：只读梳理 DOTA 下载服务器、插件文件名和测试链路。

## 待验证

- 目标用例的真实崩溃 PC、寄存器、最近文件访问和 `/tmp/vmrp_crash.bin` 内容。
- 下载后实际落盘的插件名与启动路径是否一致。
- 崩溃是否来自通用 ARM EXT wrapper/桥接布局、文件映射或插件生命周期，而不是 DOTA 特定逻辑。

## 2026-06-27 复现与初步定位

- `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` 当前失败点是测试内 TODO throw，但 stdout 已记录 ARM EXT 异常，说明进程未直接退出不等于浏览器插件启动成功。
- 保留目录：`/tmp/vmrp-e2e-j6Fok9`、`/tmp/vmrp-e2e-6y04fc`。
- 下载结果画面有效：`download-result.ppm` 为 `240x320`，`unique=6`，断言点 `(152,146)=[0,252,0]`。
- 启动后截图异常：`start-browser.ppm` 基本黑屏，`unique=2`，`nonblack=253`；`screen.ppm` 后续虽有非黑像素，但目标启动阶段已经记录异常。
- `Harvey` 子 Agent 只读结论：
  - 成功路径下载目标是 `mythroad/plugins/embrw.mrp`。
  - 目标 Vitest 不启动 `tool/download-server.js`，当前复现走默认 DNS/远端响应。
  - `browser.mrp`、`embrw.mrp`、`plugins/ose/brwcore.mrp` 三个名字并存，需要避免按文件名写特殊逻辑。
- 窄口径 DIAG/PC 复现命令：
  `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 VMRP_ARM_EXT_TRACE_PC=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`
- 崩溃证据：
  - `UC_MEM_WRITE_UNMAPPED addr=0x202C7831 size=4 value=0x0`
  - `crash PC=0x2C97FC (thumb)`
  - `R4=0x202C7831`，`R9=0x002C9FAC`，`LR=0x002C93F1`
  - 反汇编命令：`arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x2C96FC /tmp/vmrp_crash.bin`
  - PC 附近指令：
    - `0x2C97F8: bl 0x2C93E0`
    - `0x2C97FC: str r0, [r4, #0]`
  - 该函数把第三个参数当作输出指针写回结果；第三个参数 `0x202C7831` 的低位落在当前插件代码附近，疑似 ARM 侧带标志位/Thumb 位的代码地址被错误当成数据输出指针传入。
- 启动阶段模块同步：
  - `DIAG synced file=0x2C60F0 len=5084 ... H=0x2C6825`
  - `DIAG synced file=0x2C7738 len=10352 ... H=0x2C925D`
  - 随后第二个子模块在 `0x2C97FC` 写 `*out` 时崩溃。

## 2026-06-27 最终定位与修复

- 崩溃阶段修复后，目标用例继续失败在浏览器启动画面低色数断言：
  `expected 3 to be greater than 4`。
- 关键运行时证据来自 `/tmp/vmrp-e2e-BHFHIz/stdout.log`：
  - DSM 下载流先发布完整 MRPG：`len=21538 path="mythroad/vld/7530410/753041.t"`。
  - 随后重命名到安装路径：`mythroad/plugins/embrw.mrp`。
  - 安装包内包含 `brw.ext`、`frame.ext`、`logo.bmp`、`progress.bmp`、`font.uni` 等条目；磁盘条目为 gzip 压缩，loader 暂存的子模块是解压后的 payload。
- 失败根因：
  - `frame.ext` 作为私有 loader 子模块启动后会清空/绕过 `table[100]` 包名，再通过 table[125] 读取同包资源。
  - 旧逻辑只能把子模块归属到运行时 RAM 包；该 RAM 包只含解码出的子模块 payload，缺少 `logo.bmp`、`progress.bmp`、`font.uni` 等兄弟资源。
  - 因此 `readFile("logo.bmp")` 等资源读仍落在错误包上下文，浏览器只提交低色数/近黑过渡帧。
- 修复方式：
  - DSM 层按 Mythroad fd 跟踪写入的 MRPG 流，只在 header 声明的总长度完整到达后发布快照，并在 `mr_rename()` 成功后更新规范化 host 路径。下载器每块写入前会 `SEEK_END,0` 追加，tracker 只有在 host 文件长度等于已证明的流位置时才继续跟踪；其它 seek 会重置流证明，`mr_remove()` 会清除同路径快照。
  - ARM EXT executor 在 table[125] 读取时保存产出 `last_file_copy` 的 host/RAM 包上下文；登记嵌套模块时用不可变 payload tail 证明 staged child 确实来自该包。
  - 对已安装 DSM MRP，遍历索引式和顺序式 payload；若条目为 gzip，则解压后再与 staged child 比较。匹配成功后把子模块 owner 提升为 host-openable `mythroad/plugins/embrw.mrp`，后续空 `table[100]` 的资源读取通过该 host 包解析。RAM-package 辅助证明也必须同时满足“同一 encoded payload 存在于已安装 MRP”且“该 payload 解码后匹配 staged child”，不能只靠共享资源命中。
  - 修复没有按应用名或文件名分支；边界是 DSM 文件写入 provenance 和通用 MRP payload 字节证明。
- 修复后诊断验证：
  - `/tmp/vmrp-e2e-IRIipF/stdout.log` 中 `package_host candidate ... lastPath="mythroad/plugins/embrw.mrp" lastLen=21538 direct=1 ... match=1`。
  - `logo.bmp`、`progress.bmp`、`focus.bmp`、`no_focus.bmp`、`font.uni` 资源读均为 `ok=1`。
- 最终验证命令：
  - `cmake --build build --target vmrp -j2`
  - `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`

## 2026-06-27 simpleDownload 最后 60 秒复查

- 新任务：分析 `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"` 最后 `60s` 内发起 `simpleDownload` 的触发条件；不跑 `xvfb`，避免全量 trace，继续用 stdout/PPM/反汇编证据。
- 当前工作树已有前序修复形态，不能直接采信旧 wiki 结论；本轮以新保留目录 `/tmp/vmrp-e2e-GLjzVe` 为准。
- 目标用例通过：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`，耗时约 `83s`。
- 低噪声 stdout 证据：
  - 启动早期先有一次 `cmwap` 请求 `POST /payOneAsTlv` 到 `rop.skymobiapp.com:80`。
  - 下载界面确认下载后有第一次 `POST /simpleDownload`：`my_initNetwork(..., 'cmwap')`，`Host: spd.skymobiapp.com:6009`，真实连接 `159.75.119.124:6009`，收到 `HTTP/1.1 200 OK`，随后安装 `mythroad/plugins/embrw.mrp`。
  - 浏览器插件启动后，测试第 180 行再次按 `LEFT_SOFT`，最后 `60s` 内出现第二次 `POST /simpleDownload`：`my_initNetwork(..., 'CMNET')`，同步 DNS 解析 `spd.skymobiapp.com`，`my_connect()` 异步连接 `159.75.119.124:80` 成功，`my_getSocketState(3): 0`，`[my_send] cmwap off.`，请求头仍写 `Host: spd.skymobiapp.com:6009`，收到 `HTTP/1.1 404 Not Found`。
- 当前关键疑点：
  - 第二次 `simpleDownload` 确实发生在最后 `60s` 观察窗口内，不是只在插件下载阶段发生。
  - 第二次请求的 HTTP Host 指向 `:6009`，但 socket 实际连到 `:80`；这可能是浏览器插件业务使用 `CMNET` 后显式传入 `80`，也可能是平台网络桥/反汇编路径把请求端口处理错。
  - 需要继续用 MRP 内容和 ARM EXT/DSM 网络槽反汇编区分“插件业务状态机触发”与“平台 BUG 导致错误目标端口”。
- 运行时文件状态：
  - `mythroad` 是 `wasm/dist/fs/mythroad` 的符号链接。
  - 本轮测试后运行时 `mythroad/plugins` 只有 `embrw.mrp` 与 `netpay.mrp`；`brwmain.mrp`、`brwcore.mrp`、`brwshell.mrp` 等浏览器下级插件只存在于 `build/mythroad/plugins`，没有被测试工作目录预置。
  - `embrw.mrp` 内部只含 `brw.ext`、`frame.ext`、`logo.bmp`、`progress.bmp`、`font.uni`、焦点位图；`frame.ext` 字符串包含 `brw/vercfg.bin`、`brw/upd/vercfg.bin`、`brw/brw2.cfg`、`CMWAP`、`CMNET`、`plugins/embrw.mrp`。这支持一个候选解释：最后 `60s` 的左软键是在已启动的 `frame.ext` 浏览器壳中确认继续/更新，因下级浏览器包或版本配置缺失而再次进入下载服务。
- 当前 x86_64 反汇编核验：
  - `build/vmrp` 中 `my_initNetwork` 位于 `0x22010`，对 `mode` 做 `strncasecmp("cmwap", mode, 5)`，每次覆盖全局 `isCMWAP`。
  - `my_socket` 位于 `0x21df0`，创建 socket 时把当前 `isCMWAP` 复制到 `mSocket+0x10`。
  - `my_connectAsync` 位于 `0x21c50`，真实连接结果写到 `realState`，再按 `mSocket+0x10` 决定是否同步 guest 可见 `state`。
  - `my_getSocketState` 位于 `0x21da0`，返回 `mSocket+0x0c`；本轮日志的 `my_getSocketState(3): 0` 证明 CMNET 阶段没有复现旧的 `MR_WAITING` 状态污染。

## 2026-06-27 simpleDownload 触发条件结论

- 复现稳定：
  - `/tmp/vmrp-e2e-GLjzVe/stdout.log` 和 `/tmp/vmrp-e2e-aj82ck/stdout.log` 均显示最后 `60s` 窗口内有第二次 `POST /simpleDownload`。
  - `strace -f -s 4096 -e trace=connect,sendto,recvfrom -o /tmp/vmrp-dota-simpledownload-sendto.strace ...` 再次复现通过，系统调用级证据显示第二次请求在测试第 180 行 `LEFT_SOFT` 后出现。
- 触发条件：
  - 前提一：测试开头删除运行时 `mythroad/plugins/embrw.mrp`，DOTA 进入“下载浏览器插件”界面。
  - 前提二：第 158 行 `LEFT_SOFT` 下载并安装 `embrw.mrp`，第一次 `/simpleDownload` 的 TLV `0x29CE=480004 (0x75304)`，与 `build/mythroad/plugins/embrw.mrp` 的 appid 一致，返回 `200 OK`。
  - 前提三：第 166 行确认下载结果后启动 `embrw.mrp`，`frame.ext` 作为浏览器壳进入可交互界面。
  - 直接触发：第 180 行再次按 `LEFT_SOFT`。此时已经在浏览器插件 `frame.ext` 路径内；`frame.ext` 选择 `CMNET`，解析 `spd.skymobiapp.com`，调用 `mr_connect(..., port=80, type=async)`，socket 状态成功后发送第二次 `/simpleDownload`。
  - 运行时文件条件：当前测试工作目录只有 `embrw.mrp` 和 `netpay.mrp`，没有 `brwmain.mrp`、`brwcore.mrp`、`brwshell.mrp` 等下级浏览器插件；第二次 `/simpleDownload` 的 TLV `0x29CE=700015 (0xAAE6F)`，不是 `embrw.mrp` 的 appid。现有仓库内未找到 appid `700015` 对应 MRP，说明它更像浏览器壳请求的下一级组件/配置/更新对象，而不是重复下载 `embrw.mrp`。
- 端口判断：
  - `src/arm_ext_executor.c` table[85] 直接把 ARM `r2` 作为 `mr_connect` 端口传入；`src/network.c` 的 `my_connect()` 对 CMNET 不解析 HTTP Host。
  - 第二次日志有 `[my_send] cmwap off.`，因此 `Host: spd.skymobiapp.com:6009` 不会被平台用于重连。
  - `connect('159.75.119.124', 80)` 是 guest/plugin 传入的平台参数；`Host: ...:6009` 是 guest/plugin 随后构造的 HTTP 头。当前证据不支持“VMRP 把 6009 改成 80”的平台 BUG。
- 反汇编证据：
  - `/tmp/vmrp-embrw-extract/frame.ext` 中 `CMWAP` 位于文件偏移 `0x24f0`，`CMNET` 位于 `0x24f8`。
  - `arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x2c8000 /tmp/vmrp-embrw-extract/frame.ext` 显示 `0x2c9200` 函数根据 `r0 == 1` 选择 `CMWAP`，否则选择 `CMNET`，再进入网络初始化/连接路径。
  - `frame.ext` 字符串还包含 `brw/vercfg.bin`、`brw/upd/vercfg.bin`、`brw/wvercfg.bin`、`brw/brw2.cfg`、`plugins/embrw.mrp`；这些配置/组件在运行时目录缺失，支持“浏览器壳启动后进入更新/下载检查”的解释。
- 更具体的按键触发反汇编：
  - `src/e2e_control.c` 把 `LEFT_SOFT` 映射为 `SDLK_EQUALS`；`src/main.c` 再把 `SDLK_EQUALS` 送成 `MR_KEY_SOFTLEFT`。`src/include/types.h` 中 `MR_KEY_SOFTLEFT` 的枚举值为 `17`，`MR_KEY_SELECT` 为 `20`。
  - `frame.ext` 是带 `MRPGCMAP` 头的混合 ARM/Thumb 子模块，不能按全文件单一 Thumb 流解释；只对已知 Thumb 函数区切片。
  - 在 `frame.ext` Thumb 切片里，`0x2c8658` 一段是输入命中/确认处理函数。它先要求全局 UI 状态字节 `[state+4] == 2`；`r0 == 0` 的按下事件会在 `0x2c8722..0x2c8732` 处理 `r1 == 12/13`（上/下键）并更新选择字节 `[r4+7]`，这只是改变当前选择。
  - 真正触发网络路径的是确认/释放分支：`r0 == 1` 时，`0x2c8736..0x2c873c` 明确比较 `r1 == 20` 或 `r1 == 17`，两者任一命中都会落到 `0x2c873e`；其中 `17` 正是左软键。
  - `0x2c873e..0x2c8752` 读取选择字节 `[r4+7]`，先调用 `0x2c93c0` 保存选择状态、`0x2c9044` 刷新界面，然后把该状态作为 `r0` 调用 `0x2c9200`。`0x2c9200` 对 `r0 == 1` 选择 `CMWAP`，否则选择 `CMNET`；当前日志中这一分支实际选择了 `CMNET`。
  - `0x2c9200` 之后调用疑似 `initNetwork(APN)` 包装的 `0x2c9a48`。若返回 `2`，`0x2c9232..0x2c9242` 使用 `0x4e20`（`20000ms`）注册重试回调；该回调落在 `0x2c84d0` 附近，会再次调用 `0x2c9260` 和 `0x2c9200`。这解释了左软键后观察窗口内出现 CMNET 初始化和后续下载请求。
  - 因此“第 180 行左软键触发”不只是测试时序判断：`frame.ext` 内部确有左软键/确认键分支直接调用网络模式选择函数。尚未静态定位到 `simpleDownload` 字符串或 appid `700015` 的构造函数，因为这些字符串不在 `frame.ext`/`brw.ext` 明文中；`700015` 的具体资源身份仍按运行时 TLV 证据归为中置信推断。
- PPM/可见状态：
  - `/tmp/vmrp-e2e-7SRio9/start-browser.ppm` 为 `240x320`，不是黑屏；前序断言 `uniqueColorCount() > 4` 已通过。
  - 最终 `screen.ppm` 也有大量非黑像素；当前问题是网络业务触发条件，不是浏览器插件启动崩溃或空画面。
- 结论：
  - 最后 `60s` 发起 `simpleDownload` 的直接触发条件是“浏览器插件启动后在 `frame.ext` 界面再次按左软键”；更完整的条件是运行时只安装了外层 `embrw.mrp`，下级浏览器组件/配置缺失，`frame.ext` 在左软键确认后走 `CMNET` 更新/下载路径，请求 appid `700015`。
  - 程序现有网络代码在此路径上已经把 CMNET socket 状态正确暴露为 `0`，没有复现旧的 CMWAP/CMNET 状态污染。当前若要让请求成功，修复方向应是提供/模拟浏览器壳请求的 `700015` 资源或补齐它期望的组件/配置；不应在平台层按 HTTP Host 对 CMNET 已连接 socket 做重连兜底。

### 2026-06-27 17:16 当前状态复验

- 复验命令：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`，结果 `1 passed | 1 skipped`，目标用例耗时 `83299ms`。
- 新保留目录：`/tmp/vmrp-e2e-i2ymhU`。
- 新 stdout 与前述结论一致：
  - 第一次 `/simpleDownload`：`my_initNetwork(..., 'cmwap')`，`my_connect('159.75.119.124', 6009)`，`POST /simpleDownload`，返回 `HTTP/1.1 200 OK`。
  - 最后 `60s` 窗口内第二次 `/simpleDownload`：`my_initNetwork(..., 'CMNET')`，`my_getHostByName('spd.skymobiapp.com')`，`my_connect('159.75.119.124', 80)`，`my_getSocketState(3): 0`，`[my_send] cmwap off.`，`POST /simpleDownload`，`Host: spd.skymobiapp.com:6009`，返回 `HTTP/1.1 404 Not Found`。
- PPM 检查：`/tmp/vmrp-e2e-i2ymhU/start-browser.ppm` 与 `screen.ppm` 均为 `240x320` raw pixmap；去除 PPM 头后分别有 `34708` 与 `39880` 个非零字节，证明不是黑屏路径。

### 2026-06-27 19:52 当前状态重新复现

- 复现命令：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`。
- 当前失败点不是测试内 TODO throw，而是第 180 行再次按 `LEFT_SOFT` 后 `VmrpE2e.key()` 等待新绘制超时：`ERR wait_draw_timeout current=160 target>160`。
- 保留目录：`/tmp/vmrp-e2e-w6QYxK`。
- stdout 证据：
  - 第一次 `/simpleDownload` 仍是下载外层 `embrw.mrp`，返回 `HTTP/1.1 200 OK`。
  - 浏览器启动后第二次 `/simpleDownload` 改为返回 `HTTP/1.1 200 OK`，不再是旧记录里的 404。
  - 第二次返回后立即出现 ARM EXT 异常：`UC_MEM_READ_UNMAPPED addr=0x75747879 size=4`，`crash PC=0x2CF5CA (thumb)`，`last_file=0x4134E0..0x4136C0`。
  - 寄存器：`R4=0x75747869`，`R9=0x002C7784`，`LR=0x002CF5CB`，`PC=0x002CF5CA`。
- 反汇编证据：
  - 命令：`arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x2CF4CA /tmp/vmrp_crash.bin`。
  - `0x2CF5A4` 函数是 12 字节节点的链表插入/注册逻辑；`0x2CF5CA: ldr r0, [r4, #16]` 读取链表头字段。
  - `R4=0x75747869` 不是有效 ARM 内存指针，`R4+0x10=0x75747879` 导致崩溃；该值像文本/数据字节被当成结构指针传入。
- 新运行时文件状态：
  - `wasm/dist/fs/mythroad/brw/vercfg.bin` 已在本轮运行中写入，首个资源 ID 为 `0x000AAE6F`，与文档中的 `700015` 对应。
  - 当前 `wasm/dist/fs/mythroad/plugins` 已包含 `brwshell.mrp`、`brwgui.mrp`、`brwmain.mrp`、`brwcore.mrp`、`embrw.mrp`、`netpay.mrp` 和新写入的 `dump0`；这和旧记录“只有 embrw/netpay”的条件不同。
- 当前根因方向：
  - 旧的外层 `frame.ext` 资源 owner/低色数启动问题已不是本轮首要失败。
  - 需要定位第二阶段 `700015` 配置/组件处理后，哪个浏览器组件或回调把结构指针污染成 `0x75747869`。下一步用低噪声 ARM EXT DIAG 记录 table[125]/table[127]/table[25] 的文件名、长度、LR 和 nested owner，再按栈返回地址反汇编调用方。

### 2026-06-27 19:55 DIAG 复现

- 复现命令：`VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`。
- 结果同样失败于 `ERR wait_draw_timeout current=160 target>160`；保留目录：`/tmp/vmrp-e2e-8mM4I4`。
- 关键差异：
  - 第二次 `/simpleDownload` 的 700015 配置返回 `HTTP/1.1 200 OK` 后，连续同步四个浏览器组件：
    - `file=0x2CF110 len=22884 ... rw=0x2C7784`
    - `file=0x2D4A78 len=19708 ... rw=0x2C85CC`
    - `file=0x2D9778 len=56528 ... rw=0x2C8B3C`
    - `file=0x2E7448 len=148224 ... rw=0x30B74C`
  - `wasm/dist/fs/mythroad/brw/vercfg.bin` 的组件顺序是 `brwshell.mrp`、`brwgui.mrp`、`embrw.mrp`、`brwmain.mrp`、`brwcore.mrp`；最后一个 `len=148224` 与 `brwcore.ext` 解压长度一致。
  - 崩溃时 `activeP=0x412C00 activeH=0x309181 activeRW=0x30B74C`，说明前台 active 子模块是 `brwcore.ext`。
  - 但 CPU 寄存器为 `PC=0x2CF5CA`、`R9=0x2C7784`；该 PC/RW 属于第一个浏览器组件 `file=0x2CF110 len=22884`，不是 active 的 `brwcore.ext`。
  - `call-post-active rw=0x30B74C` 中可以看到 brwcore RW 的多个槽指向字符串数据，如 `0x309DE8` 处为 ASCII `.librunapp`，而崩溃指令期望的是链表头/结构指针。
- 反汇编解释：
  - `0x2CF5A4` 函数按 `r0` 操作链表头：分配 12 字节节点，调用 memset 后读写 `[r4+16]`、`[r4+20]`。
  - 崩溃时传入 `r0/r4=0x75747869`，`ldr r0,[r4,#16]` 访问 `0x75747879` unmapped。
- 当前更强根因方向：
  - 不是 700015 配置缺失，也不是 CMWAP/CMNET 端口重写。
  - 这是多浏览器组件同步后，ARM EXT 在跨子模块/回调执行时选择了错误的 R9/P 上下文：active 已经是 `brwcore.ext`，但执行回到第一个组件的代码/RW，并把 brwcore 字符串/全局表中的内容当作第一个组件的链表结构参数使用。

### 2026-06-27 23:02 最终修复复验

- 本轮最终失败不是 R9 选择错误，而是 ARM-visible `table[100]`/`record[100]` 暴露宿主绝对路径后触发 fixed-buffer 溢出：
  - `brwmain.ext` 启动代码在 `0x2D1E0C` 清空 `R9+0xBD8` 的 32 字节缓冲，随后按 `strlen(pack)`/`memcpy` 复制包名。`R9+0xBF8` 正好是后续 `0x2D1D64` 读取的链表头，长宿主路径第 32..35 字节为 `ixtu`，最终在 `0x2CF5CA: ldr r0,[r4,#16]` 访问 `0x75747879`。
  - `frame.ext` 在 `0x2C9810` 使用 `sp+4` 的 32 字节包名缓冲，随后调用 table[125] 包装并在 `0x2C9844` 写回输出指针；长包名会先破坏该输出指针，表现为 host table[125] 收到损坏的 `r1=0x656c2c88` 或 ARM 写 `0x656c2c84`。
- 关键修正点：
  - `arm_ext_init_pack_names()` 已经能为根包 `/home/.../test/fixtures/dota.mrp` 生成 cwd-relative alias `test/fixtures/dota.mrp`，但 `init_table()` 仍把宿主绝对路径写入共享 `table[100]`。最终改为从启动时就暴露 `m->pack_alias`，同时让 table[40]/[42]/[46] 和 table[125] 一样通过 `arm_ext_pack_to_host_path()` 解析 alias 到宿主路径。
  - `frame.ext` 的包名 getter 是 `0x2C916C`，它读取私有 loader 写入的 child module record 的 `record[100]`，不是共享 `EXT_TABLE[100]`。因此还必须在私有 child 同步/确认时更新 child `record[100]`。
  - `record[100]` 的短字符串放在 executor metadata 映射区，不再放在低 heap/P 邻近区域，避免后续把包名指针误当 wrapper callback 或 P 邻接数据。
- 修复边界：
  - 用子模块自身代码形态检测风险：32 字节栈包名复制 + 输出指针写回、32 字节 R9-relative 全局包名复制、或 record[100] 读入紧凑 R9 状态区。没有应用名、文件名、长度特判。
  - 对需要短包名 ABI 的 child，根据已记录的 package provenance 生成 ARM 可见短 alias，例如 `mythroad/plugins/embrw.mrp`；如果 root-relative 或 basename 仍超过 32 字节 fixed-buffer 上限，则分配 `~pN` 合成 alias，并在 executor 内维护 alias->host package 映射供 I/O 解析。
  - host 侧 table[125] 仍通过已证明的 package owner 做 I/O 解析；没有在 table[125] 做坏指针兜底或吞异常。
- 复验：
  - `cmake --build build --target vmrp -j2` 通过。
  - `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` 通过：`1 passed | 1 skipped`，目标用例耗时 `83429ms`。
  - 成功保留目录 `/tmp/vmrp-e2e-GSqFvj`：stdout/stderr 无 `invalid memory`、`UC_MEM`、`UC_ERR`、`2CF5CA`、`757478`、`2C9844` 或 `pc bytes`。
  - PPM：`start-browser.ppm` 为 `240x320`、`27` 色、`nonzero=222900`；`browser-running.ppm` 为 `240x320`、`24` 色、`nonzero=223287`、`pixel(152,146)=(240,240,240)`；最终 `screen.ppm` 为 `2954` 色。
  - 目标用例最终断言记录第 180 行再次 `LEFT_SOFT` 前的 `drawCount`，60 秒后要求 `drawCount` 增长，并保存/检查 `browser-running.ppm`，避免只用旧帧证明存活。
