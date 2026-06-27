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
