# 490111 三国黑屏运行时取证

日期：2026-07-16

## 调查范围

- 原始复现命令：`build/skyengine build/mythroad/490111_240x320_sanguo.mrp`。
- 使用现有真实图形会话 `DISPLAY=:0`，没有使用 Xvfb。
- 不打开无界 ARM/table trace。PC 取证只使用
  `VMRP_ARM_EXT_TRACE_PC=1` 的 64 项环形缓冲，并在 GDB 中读取。
- 本文只记录运行时证据和修复约束，本调查没有修改生产代码。

## 基线身份与保留产物

黑屏基线二进制的 ELF build-id 是
`1e743dc82be987e54254c3cb5da1921f0e56fbad`。调查期间共享工作树另有并行修复
和重建，因此后续 build-id `6841cf21afa6b3f37fbaad88b02139243aa24169`
的行为不当作原始基线证据。

基线产物：

| 产物 | 用途 |
| --- | --- |
| `/tmp/skyengine-sanguo-baseline.out` | 有界启动标准输出，144 行/8192 字节 |
| `/tmp/skyengine-sanguo-baseline.err` | `SKYENGINE_LOG=1` 启动阶段日志，32 行/1687 字节 |
| `/tmp/skyengine-sanguo-proc.txt` | 7 秒时的进程/线程状态 |
| `/tmp/skyengine-sanguo-baseline.ppm` | `SIGUSR1` 抓取的 240x320 基线画面 |
| `/tmp/sanguo-cfunction.ext` | 解压后 wrapper，19428 (`0x4BE4`) 字节 |
| `/tmp/sanguo-game.ext` | 解压后主 EXT，164716 (`0x2836C`) 字节 |

`cfunction.ext` SHA-256 是
`9f1bb5ba0cc3d870389de1138064225aaaddd3e3225fa2123e6459bae707de73`；
`game.ext` SHA-256 是
`6bc53814289a39de809dc4a8b50e1717095c08961fd97a563937630a37e11fc3`。

## 进程状态和宿主调用链

6 秒后的 `ps` 证据：

```text
PID    STAT  %CPU  NLWP  WCHAN
16952  Rl    99.8     4  -
```

主线程唯一占满一个 CPU；`SDLTimer` 等待 futex，PulseAudio 线程分别等待
`poll`/futex。程序不是阻塞在图形服务器、音频或网络系统调用中。

最后一条 Mythroad 阶段日志是：

```text
[_mr_intra_start] all registered, dofile('start.mr')...
```

宿主启动链为：

```text
startVmrp
  -> skyengine_runtime_start_dsm
  -> mr_start_dsm / _mr_intra_start
  -> mrp_dofile("start.mr")
  -> mrp_call
  -> _strCom(800/801)
  -> arm_ext_load / arm_ext_call
  -> Unicorn cfunction.ext code-0 启动回调
```

同步 `dofile` 尚未返回，SDL 主循环也尚未进入。因此黑屏不是 timer
未触发、present 丢失或窗口事件处理问题。

## PPM 黑屏证据

`/tmp/skyengine-sanguo-baseline.ppm` 是合法 P6 Netpbm，尺寸 240x320，总长
230415 字节，SHA-256 是
`1edcc2a5a6f7f596acf98d8fbd248c4175886dc8759e6d75ec652b339c2a53b6`。

跳过 15 字节头后，像素负载恰好是 `240 * 320 * 3 = 230400` 字节；删除
NUL 后的字节数为 **0**。即 RGB 负载全部是 `00 00 00`，画面确实为
全黑，不是视觉误判或截图工具丢帧。

## GDB 和 guest PC 环

不开 PC 环时，GDB 中断落在
`hook_restore_r9 -> arm_ext_sync_r9_for_code_addr`，当时 guest 基本块地址为
`0xE82DE6`。wrapper 代码基址是 `0xE80000`，因而对应
`cfunction.ext + 0x2DE6`。

打开只保留 64 项的 PC 环后，8 秒采样时
`pc_ring_pos = 244132059`。最后 64 项全部在：

```text
0xE82DE2..0xE82E3C (Thumb)
```

该段反汇编是空闲链表节点遍历/拆分，不是正常定时器忙等。在每次
`0xE82E30` 循环头读 guest `R4`，得到稳定四节点环：

```text
0x225E5C -> 0x22620C -> 0x225E5D -> 0x225E5F -> 0x225E5C
```

`0x225E5D` 和 `0x225E5F` 不按 4/8 字节对齐，且实际字节为：

```text
0x225E5C: b0 03 00 00 ...  -> next_off = 0x3B0
0x22620C: 01 00 00 00 ...  -> next_off = 1
0x225E5D: 03 00 00 00 ...  -> unaligned next_off = 3
0x225E5F: 00 00 00 00 ...  -> unaligned next_off = 0
```

以 `table[108]` 的 base `0x225E5C` 加上这些 offset，恰好生成上述环。

## 首个关键边界改写

GDB 硬件 watchpoint 证明，wrapper 在加载外部 arena 时，由
`cfunction.ext + 0x2D06` 的：

```asm
str r7, [r3]
```

将 `table[110]` 指向的 `LG_mem_end` slot `0x225E44` 从
`0x325E5C` 改成 `0x4001CDA0`。该次 setup 的输入是：

```text
table[108] LG_mem_base = 0x225E5C
old table[110] end     = 0x325E5C
external arena         = 0x40018800
arena length           = 0x000045A0
new table[110] end     = 0x4001CDA0
```

外部 arena 地址的运行时传递链也已用 watchpoint 逐级确认：

```text
record 0x3DE314 + 0x1C
  <- cfunction.ext + 0x1038 从调用参数 pair 复制 0x40018800
  -> cfunction.ext + 0x1434 复制到 local 0x345BE0
  -> cfunction.ext + 0x090A 复制到 local 0x345AF4
  -> cfunction.ext + 0x0B5E 调用 setup 0xE82C80
  -> cfunction.ext + 0x2D06 扩展 LG_mem_end
```

这是 wrapper 配对 setup/teardown 协议的正常行为：它会把分开映射的 arena
临时接入以 `LG_mem_base` 为基准的 offset 链，再在 `0xE82DA0` teardown
中摘除。不能把它当成应用自己写坏指针。

## 根因判定

高置信根因是 guest 和 host 对同一 Mythroad LG_mem ABI 使用了不同边界：

- guest wrapper 从 `table[108]`/`table[110]`/`table[146]` 读取动态
  `LG_mem_base`/`LG_mem_end`/free head，并把外部 arena 接入链表。
- 黑屏基线中的 host `arm_ext_guest_mem_malloc/free` 仍以固定
  `m->origin_mem_addr + m->origin_mem_len = 0x325E5C` 做 end。
- guest 已经合法把 end 扩展到 `0x4001CDA0`，host 却把链头指向的
  `0x40018800` 误判为 corrupted，改走无关的 bump 分配。
- teardown 继续按 guest 动态 end 遍历，但 table[0]/table[1] 已没有操作
  同一条链，最终生成四节点环并永久活锁。

详细的 MRP 索引、Lua 启动协议和 setup/teardown 逐指令语义见
`docs/修复记录/sanguo-black-screen-disassembly.md`。

## 修复建议与禁止项

1. table[0]/[1]/[2] 每次进入时必须从 guest ABI slot 取当前
   base/end/head，不能继续使用过期固定 end。
2. `base..end` 可以跨越多个 Unicorn 映射 band；应验证每个节点和
   实际读写范围可映射，不应强制整个数值区间宿主连续。
3. 保留 first-fit 语义和 guest 对 `LG_mem_end` 的协议级改写。禁止用
   超时跳出、强行设 PC 返回、禁用 free、包名/场景特判或失败后清链
   的兜底逻辑解决活锁。
4. 修复后要证明 setup 期 host table[0]/[1] 与 guest 看到同一动态链，
   teardown 能从 `0xE82DA0` 返回并恢复旧 end/head。

## 后续验证门槛

- 目标命令的首次 code-0 调用返回，主线程进入 SDL/timer 循环，不再持续
  100% 单核。
- PPM 的 230400 字节 RGB 负载不再全零，且后续帧/按键能产生画面变化。
- 定向回归覆盖同类 private arena wrapper，以及依赖 origin first-fit 地址
  复用的用例；最后再执行 unit/ctest 和全量 E2E。
- 最终证据必须来自修复后当前 build-id，不得混用本文的黑屏基线
  PPM 或调查期间的并行实验二进制。
