# GSHA 资源下载无进度排查记录

## 目标与约束

- 目标用例：`pnpm vitest run test/e2e/gsha/download-res.test.ts`
- 目标行为：确认下载资源后，客户端实际发起网络请求并开始下载。
- 不使用 `xvfb`；使用 E2E 控制 socket 与 PPM 验证画面。
- 不增加应用名、包名或固定 guest 地址判断，不增加失败兜底路径。
- 修改必须由真实 ARM 调用链和平台 ABI 共同证明，并为非显然代码添加注释。
- 先修复目标用例，最后运行相关及完整测试检查兼容性。

## 2026-07-22 第一轮基线审计

### 已确认事实

1. 当前工作树中的 `src/network.c` 仅删除两段已注释代码，没有网络行为改动，不能视为本问题的修复。
2. 新增目标用例能够自动走到“确定下载资源”并用 PPM 像素检查下载界面；用例当前没有断言服务端收到请求或下载进度变化。
3. 标准 Mythroad DSM 网络 ABI（`src/mythroad/dsm.c`）定义：
   - `mr_initNetwork()` 返回 `MR_WAITING` 时，平台随后调用传入回调；同步成功/失败时不调用回调。
   - `mr_getHostByName()` 返回 `MR_WAITING` 时，平台随后用解析结果调用传入回调；直接返回 IP 时不调用回调。
4. ARM EXT C 函数表当前实现（`src/arm_ext/aex_table.c`）与标准 ABI 有明显差异：
   - `table[81]` 忽略 guest 传入的 `r0` 回调，调用 `mr_initNetwork(NULL, mode)`。
   - `table[83]` 忽略 guest 传入的 `r1` 回调，调用同步 `my_getHostByName(..., NULL, NULL)` 并直接返回 IP。
5. 上述差异目前只是嫌疑。是否构成 GSHA 的实际停滞原因，必须由 GSHA 反汇编证明它检查了 `MR_WAITING` 并依赖回调，再由受控运行日志证明确认下载后停在对应状态。

### 并行证据任务

- 运行目标用例并保存精简网络日志、PPM 与进程临时目录。
- 解包/反汇编 `gsha.mrp`，定位确认下载后的 `table[81]`/`table[83]` 调用者及返回值分支。
- 审查相似下载修复与当前网络层的兼容风险，确定需要补充的定向和回归验证。

### 下一步判定标准

- 若反汇编证明 guest 接受同步返回值，则排除回调差异，继续追踪 socket/connect 参数。
- 若反汇编证明 guest 只在异步回调中推进，则设计执行器线程内的通用 guest callback 投递机制；不得从宿主 DNS worker 直接重入 Unicorn。
- 修复后同时以服务端连接/请求证据和下载中 PPM 的时间变化证明结果，不能只依赖“进入下载界面”。

## 2026-07-22 ARM 下载器与定时器根因

### 下载调用链

解包得到的实际下载器为 `verdload.ext`（18404 字节）。受限网络日志、网络
syscall 跟踪和 ARM 反汇编共同确认：

1. 下载器在 `+0x1178` 创建 TCP socket，并在 `+0x11A0` 通过 `table[85]`
   发起非阻塞连接；传入的连接类型是 `MR_SOCKET_NONBLOCK`。
2. CMWAP 模式先连接 `10.0.0.172:80`。宿主 CMWAP fast path 会在常规
   `my_connect()` 日志之前返回，因此“没有 connect 日志”不能证明 guest 没有调用
   connect。
3. connect 返回后，`+0x11D2..+0x11DC` 创建 500 ms 定时器。回调
   `+0x3CA4` 通过 `mr_plat(1001, socket)` 查询连接状态；只有状态变成
   `MR_SUCCESS` 才越过 `+0x3D0A` 并在 `+0x3D9E` 调用 `table[89]` 发送请求。
4. GSHA 接受 `mr_initNetwork` 和 DNS 的同步成功返回，不依赖当前被忽略的网络或
   DNS callback。阻塞 connect 状态不变量虽另有缺口，但不是本次非阻塞路径的停滞
   原因。

运行日志中第二个 socket 创建成功后从未出现 `my_getSocketState(2)`，证明停滞发生
在 500 ms 回调被执行以前；宿主网络层尚未得到发送请求的机会。

### 旧版 compact timer ABI

下载器的 timer walker 位于 `verdload.ext+0x3B18`，其 R9 相对布局为：

```text
scheduler literal: +0x1D0
queued head:       +0x1D4  (scheduler +4)
current head:      +0x1D8  (scheduler +8)
last tick:         +0x1DC  (scheduler +12)
node magic:        0x79ABBCCF at node+0
queue links:       node+0x18 / node+0x1C
```

运行时 `verdload RW+0x1D4 = 0x25DA68`，且该节点带有正确 magic，说明 500 ms
队列确实存活。现有 `arm_ext_find_compact_timer_scheduler()` 只接受较新版 walker 的
`scheduler+8/+12/+16` 编码，因此没有给该模块登记调度器偏移。

实际错误序列是：child 创建 500 ms timer，wrapper 创建 125 ms timer；首次 wrapper
tick 后又创建剩余 370 ms timer。随后 `arm_ext_call()` 的 stale-wrapper 清理因为无法
识别 child 的活队列而清除 timer owner，后续 tick 被派发给 primary，下载器的 socket
轮询回调永远不再运行。不能把 wrapper veneer 下的所有 timer 都强制归给 active child，
因为同一事件内确有合法的 wrapper 125 ms timer；修复点必须是通用的 child 队列检测。

### 修复方向与验收地址

- 保留 walker 的稳定控制流形状，但解码队列、current、timestamp 的 Thumb
  load/store 立即数，并验证它们之间的结构关系。
- 将旧版 `+4/+8/+12` 和新版 `+8/+12/+16` 统一规范化为宿主扫描器所用的
  `+8/+12` 视图。旧版 literal `0x1D0` 因此登记为 `0x1CC`。
- 所有偏移必须四字节对齐、落在可表示的 RW 相对范围；timestamp 的读取与清零必须
  使用相同 base、寄存器和偏移，避免把相似普通函数误识别为 timer walker。
- 按用户要求，联网验收直接把 `spd.skymobiapp.com` 映射到
  `159.75.119.124`，不使用 `127.0.0.1` 或本地回环服务。最终证据应同时包含
  `my_getSocketState`、发往 `spd.skymobiapp.com:6009` 的请求日志和 PPM 进度变化。

## 2026-07-22 修复前复核与兼容面扫描

### 可复现基线

- 无 `xvfb`、无 PC/table trace 运行目标用例，稳定失败于下载进度像素：
  `(100,174)` 实际为 `rgb(0,4,0)`，预期为 `rgb(0,200,0)`。
- 保留目录 `/tmp/skyengine-e2e-2E7E7r` 的日志显示：注册请求正常完成，随后
  第二个下载 socket 创建成功；此后没有 `my_getSocketState(2)`，也没有
  `POST /simpleDownload`。这与“500 ms guest timer 回调没有执行”一致。
- 失败帧保存在 `/tmp/skyengine-e2e-2E7E7r/downloading.ppm`，工作目录没有
  新的资源包文件。画面证据与网络日志相互独立，均不支持“已经开始下载”。

### 反汇编复核

`verdload.ext` SHA-256 为
`ac754c00919100bc1556ce180381f90b45676c4a25899b0ee87c52bf0fba6472`。
`+0x3B18` walker 的关键指令为：

```text
+0x10  ldr r6, =0x1D0
+0x12  add r6, r9
+0x14  ldr r1, [r6, #12]   ; timestamp
+0x16  str r5, [r6, #12]   ; clear same timestamp
+0x1A  ldr r0, [r6, #4]    ; queued head
+0xAA  str r0, [r6, #8]    ; current head
+0xC8  ldr r4, [r6, #8]
+0xD4  str r5, [r6, #8]
```

因此探测器对 `+0x14/+0x16/+0x1A` 的新版固定编码无法命中。规范化公式是
`scheduler = literal + queued_offset - 8`，GSHA 对应 `0x1D0 + 4 - 8 =
0x1CC`；现有扫描器随后仍可统一从 normalized `+8/+12/+16` 读取
queued/current/timestamp。

### 兼容面

- 离线扫描全部 `test/fixtures/*.mrp` 找到三种布局：新版 child
  `queued/current/timestamp=+8/+12/+16`、旧版 child `+4/+8/+12`、独立的
  wrapper direct `+12/+16/+20`。wrapper 由另一探测器处理，本修复不能混入。
- 旧版 child 出现在 gghjt、gsha、gwkdl；其余当前 fixture 的同形 child walker
  使用新版布局。对新版应用规范化公式保持原 literal 不变。
- 修复必须同时验证 timestamp 的 load/clear 使用同一 base 和 offset、
  `current=queued+4`、`timestamp=current+4`，并验证后续 current 的 store/load/clear，
  不能只放宽函数前 32 字节的 byte pattern。

## 2026-07-22 修复后运行证据

### 请求链恢复

修复后的受限诊断运行保存在 `/tmp/skyengine-e2e-jwwaAK`。与基线使用相同应用
路径和公网地址，确认下载后出现：

```text
my_socket(): s=2 fd=11
my_getSocketState(2): 0
my_send(s:2, fd:11, len:590): sent=590, errno=115
[my_send] data: POST /simpleDownload HTTP/1.1
Host: spd.skymobiapp.com:6009
```

这条证据把 timer callback、`mr_plat(1001, socket)`、请求构造和 `table[89]`
发送串在同一个 socket 上。修复前相同位置只有 `my_socket(): s=2`，没有状态查询
或请求，因此变化直接对应 compact timer scheduler 被正确登记和派发。

### 服务端状态不是宿主回归

修复后客户端还收到 HTTP 200，但画面从“正在连接下载服务器”变为“资源不存在”。
为了确认这不是 emulator 丢包或解析错误，使用仅捕获网络 syscall 的有界跟踪
`/tmp/gsha-network.strace`（启动包装脚本为
`/tmp/gsha-network-strace-wrapper.sh`）检查实际收发字节；未启用 ARM 指令或 table
全量 trace。请求 TLV `0x29CE` 的值是 `0x1388`（资源号 5000），响应 body 为：

```text
00 00 00 64 00 00 00 04 00 00 01 94
```

即 tag 100、length 4、value 404。公网服务已经明确报告该资源不存在，所以旧用例
固定要求进度条像素 `(100,174) == rgb(0,200,0)` 会把外部资源库存误当成 runtime
正确性。最终 E2E 改为验证同一 socket 成功连接、`sent == len`、请求路径为
`/simpleDownload` 且 Host 为 `spd.skymobiapp.com:6009`；不对公网响应内容作成功假设。
退出 emulator 后再读取 stdout，以确保 C 管道缓冲已经刷新。PPM
`download-request-result.ppm` 仍保留为失败时的画面诊断。

## 2026-07-22 验证结果

- `cmake --build build --target skyengine -j4`：通过。
- `pnpm exec tsc --noEmit`：通过。
- 无诊断、SDL dummy driver 的目标用例：1 文件 1 用例通过，约 17 秒。
- 定向兼容矩阵：5 文件 6 用例全部通过；覆盖 GSHA、旧布局 `gghjt`、新版
  `optwar`，以及 `gxdzc`、`wbrw` 的 nested-module 路径。
- 离线全 fixture 扫描：旧 child walker 8 个、新 child walker 61 个、wrapper
  false positive 0 个；旧布局分布于 gghjt、gsha、gwkdl。
- 所有运行均未使用 `xvfb`；未启用大体量 guest 指令/table trace。

## 2026-07-22 最终解码器与资源恢复复核

### 不依赖固定指令字

最终 scanner 不再比较 walker 内某个固定位置的完整 Thumb 指令字，也不要求
current-head 操作恰好位于某个 SDK 构建的 `+0xAA/+0xC8/+0xD4`。入口沿用既有的
compact walker 识别前缀；随后在有界函数体内解码 Thumb word load/store，并按顺序
验证以下语义：

1. timestamp 由 R6 相对字段读出并清零；
2. queued head 由同一 R6 基址读出；
3. current head 被写入、读回并清零；
4. `current=queued+4`、`timestamp=current+4`。

child ABI 已由 fixture 反汇编证明只有 queued `+4` 与 `+8` 两代；queued `+12`
属于 wrapper dispatcher，仍由独立 wrapper scanner 管理。规范化公式不变。全 fixture
复扫的共享 prefix 候选共 84 个：旧 child 8、新 child 61、wrapper 15；最终 scanner
命中旧 8、新 61、wrapper 0，没有漏掉原有新版 walker，也没有 false positive。

### 公网资源恢复后的行为

用户恢复资源 5000 后，无诊断运行 `/tmp/skyengine-e2e-tYOWJu` 显示同一 socket
完成请求并持续接收资源数据；5 秒时工作区已产生 97,881 字节临时资源文件。
`download-request-result.ppm` 的 SHA-256 为
`e908afde6d99057c478d4e17112a70bb3de78b7d4706d1bc143822708d20b9ec`，进度条已经
从暗色连接状态变为亮绿色填充。E2E 因此同时验证确认下载前后的画面确实变化，以及
同一 connected socket 的完整 `/simpleDownload` 请求；画面断言不绑定某个服务端版本
的固定进度百分比或固定颜色值。

### 回归处理

第一次完整无重试回归为 30/31 文件、53/54 用例；唯一失败是 `gjxwsmn` 在 Debug
构建中从 latest framebuffer 读到了 timer 已推进后的下一帧。用同配置的未修改 HEAD
二进制 A/B 后该失败完全相同，排除 scanner 匹配变化；离线复扫也证明其三个 child
walker 的地址和 scheduler offset 均未改变。按范围约束，最终工作树不保留对
`gjxwsmn` 或任何其它非 GSHA 测试的修改；因此当前完整回归的准确结果仍是上述
30/31 文件、53/54 用例，唯一失败是未修改 HEAD 也可复现的既有 latest-frame
时序断言。最终 scanner 下的目标用例和不含该用例的 5 文件/6 用例定向兼容矩阵通过。
