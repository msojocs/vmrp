# 490111 三国黑屏反汇编记录

日期：2026-07-16

## 1. 范围与取证方法

- 目标包：`build/mythroad/490111_240x320_sanguo.mrp`，SHA-256
  `bcbac32ad7567e1112f0c15db6c8aae9752bfe4faf2f2492f5665e7332c51702`。
- 本文使用 MRP 索引解析、`tool/analyze-mr.py`、Capstone、
  `arm-none-eabi-objdump`和 GDB 暂停后的 Unicorn 寄存器读取。
- 运行只开启有界 `VMRP_ARM_EXT_DIAG` 诊断，没有运行全量 table/PC
  trace，也没有使用 Xvfb。
- 本文不修改生产逻辑；文中的修复方向必须以通用内存 ABI
  为判据，不能以包名或场景分支规避。

## 2. MRP 目录与启动资源

包头是新版索引格式：

| 字段 | 值 |
|---|---:|
| 文件长度 | `0x7B598` (505240) |
| `header_len` (`+0x04`) | `0x1C8B` |
| `index_start` (`+0x0C`) | `0x00F0` |
| 索引结束 | `header_len + 8 = 0x1C93` |
| 条目数 | 263 |

索引记录格式与 `arm_ext_visit_mrp_payloads()` 相同：
`name_len, name, data_off, packed_len, reserved`。关键条目如下（`raw`
是 gzip 解压后长度）：

| 序号 | 名称 | 包内偏移 | packed | raw | SHA-256 前 16 位 |
|---:|---|---:|---:|---:|---|
| 0 | `#807022#*` | `0x001CA5` | `0x20` | `0x20` | `21f502050c5666ef` |
| 1 | `start.mr` | `0x001CD6` | `0x3FA` | `0x9BA` | `ff67eea7e6eed10e` |
| 251 | `graph.ext` | `0x04B00F` | `0x31D5` | `0x459C` | `a7327be292deccf6` |
| 252 | `dpnoui.mrp` | `0x04E1F7` | `0x114A` | `0x11B4` | `051ada2b7e06696b` |
| 253 | `skyfont.ext` | `0x04F355` | `0x124D` | `0x1970` | `deb43062b156b130` |
| 255 | `download.ext` | `0x050D81` | `0x20AE` | `0x2B9C` | `69fe245286fd34d2` |
| 256 | `game.ext` | `0x052E40` | `0x17E80` | `0x2836C` | `6bc53814289a39de` |
| 257 | `cfunction.ext` | `0x06ACD6` | `0x3396` | `0x4BE4` | `9f1bb5ba0cc3d870` |
| 258 | `verdload.ext` | `0x06E081` | `0x355C` | `0x4A7C` | `13b396f50da0a2f7` |
| 259 | `msgim.mrp` | `0x0715EF` | `0x5082` | `0x5122` | `f83f65169a4d7e95` |
| 260 | `fedd.mrp` | `0x076682` | `0x36A7` | `0x37EA` | `cc84924dee289140` |
| 261 | `xsh.mrp` | `0x079D39` | `0x17F0` | `0x184A` | `0c7fa57211497205` |
| 262 | `plugins.lst` | `0x07B53D` | `0x5B` | `0xE0` | `1590fccd08ef962e` |

`plugins.lst` 明确列出 `plugins\\dpnoui.mrp`、`plugins\\msgim.mrp`、
`plugins\\fedd.mrp`和 `plugins\\xsh.mrp`。它们的枚举发生在主
`game.ext` 完成私有加载之后，不是 Lua 入口。

## 3. `start.mr` 的精确启动协议

`start.mr` 是 MRP bytecode `version=0x80, endian=1`。根 proto 在
`0x0006`，含 61 条指令和 5 个子函数。

1. 根 proto PC 1..4 执行 `_com(3629, 2913)`，置
   `MR_FLAGS_BI`。
2. 子函数 `0.0` (`off=0x259`) 读取 `cfunction.ext`，然后调用
   `_strCom(800, ext_bytes, 0)`。
3. 根 proto PC 20..23 要求装载返回 0；否则 PC 59..60
   直接 `Exit()`。
4. 装载成功后，PC 24..32 调用
   `_strCom(801, {1, sysinfo.vmver}, 6)`。这个定制 Lua ABI 将
   `input=1`、`input_len=vmver=1968` 作为两个标量传入 wrapper。
5. PC 35..39 紧接着调用 `_strCom(801, "", 0)` 启动主游戏。
6. `dealevent`、`dealtimer`、`suspend`、`resume` 分别转发 EXT
   code `1/2/4/5`。事件字符串参数会先包成
   `iiiii`，普通参数包成 `iii`。

因此黑屏不是 `start.mr` 中的 Lua 循环。实测卡死在第 5
步的同步 code-0 调用内，Lua 尚未返回。

## 4. `cfunction.ext` 入口与私有加载

以文件基址 `0xE80000` 记录运行地址：

- `0xE80008` 是 ARM `mr_c_function_load`，通过 `blx 0xE82EE0`
  进入 Thumb loader。
- `0xE82EE0..0xE82F2A` 调用 `table[25]`，登记 helper
  `0xE800B0`，请求 P 长度 `0x2014`；随后求得 ER_RW 长度
  `0x3B0`，分配 RW 并设 R9。
- `0xE800B0` 是 ARM 桥，最终进入 Thumb helper
  `0xE82F8C`。
- `0xE82F8C..0xE8306E` 按 code 0..9 跳转分派。code 6 分支
  `0xE83030..0xE83034` 只把标量地址保存到 wrapper RW；
  code 0 从 `0xE82FB6` 开始，经 `0xE83BE0 -> 0xE821F4`
  进入包枚举与主游戏私有加载。

有界运行诊断证明主游戏已经装载，不应将问题归因于
MRP 目录失败或 gzip 失败：

| 对象 | 运行值 |
|---|---:|
| `game.ext` file image | `0x226318`, len `0x2836C` (164716) |
| extChunk | `0x2BC57C` |
| P | `0x2BC5DC` |
| helper | `0x238F8D` (`file + 0x12C75`, Thumb) |
| ER_RW | `0x24E68C`, len `0x4074` (16500) |

wrapper 已读到 `plugins.lst`并完成多个插件路径探测，随后在
首次 code-0 回调内持续占用 100% 单核。

## 5. `0xE82C80` setup 的反汇编语义

`0xE80B5E` 调用 `0xE82C80(arena, arena_len)`。该函数不是
普通 malloc，而是把一段外部 arena 临时并入 Mythroad LG_mem
相对偏移链表：

1. `0xE82C92..0xE82CAA` 连续调用两次 `table[0](1)`，
   保存两个探测块。
2. `0xE82CAC..0xE82CB2` 立即调用 `table[1](first, 1)`，
   使第一个探测块成为空闲链表中的可编辑节点。
3. `0xE82CB4..0xE82CD0` 保存 `table[136]` (`LG_mem_top`)
   和该节点的 8 字节 `{next,len}`。
4. `0xE82CD2..0xE82CE2` 对齐 `arena/arena_len`，保存对齐后
   的起点和长度。
5. `0xE82CEE..0xE82D06` 读 `table[110]` (`LG_mem_end`)，
   并将它扩展为 `max(old_end, arena + arena_len)`。
6. `0xE82D08..0xE82D2A` 以 `table[108]` (`LG_mem_base`) 为基址，
   把 arena 和旧链表用 32 位相对偏移连起来，并更新
   `LG_mem_top`。
7. `0xE82D30..0xE82D8E` 遍历、拆分或合并旧链表尾部，
   使新 arena 成为完整 first-fit 地址空间的一部分。

这段代码直接读写 `table[108]`、`table[110]`、`table[136]`
及 `table[146]`所指数据，明确假定 table[0]/[1] 与这些变量
是同一个分配器的共享状态。

## 6. `0xE82DA0` teardown 的反汇编语义

`0xE82E4E` 在临时 arena 使用完后调用 `0xE82DA0`：

1. `0xE82DA6..0xE82DBC` 执行一次 `table[0](1)` 和
   `table[1](p, 1)`，获取并释放当前 first-fit 头节点。
2. `0xE82DC2..0xE82DCC` 若返回地址恰好是 arena 起点，
   直接重新分配整段 arena 并返回。
3. 否则 `0xE82DCE..0xE82E3E` 以 `table[108]` 为 base、
   `table[110]` 为 end 遍历空闲节点，找到 arena 相交节点后
   拆分或修复 `{next,len}`。主循环是
   `0xE82DE2..0xE82E3C`。
4. 返回 `0xE82E48` 后，函数恢复 `LG_mem_top`、释放两个
   探测块，并恢复旧 `LG_mem_end` 及先前保存的链表尾部。

这是有配对 setup/teardown 的 allocator 协议，不能把循环本身判为
游戏忙等后用超时强行跳过。

## 7. 实测链表和活锁路径

未修复运行时：

```text
B = table[108] = 0x225E5C
fixed host end = B + origin_mem_len = 0x325E5C
H = table[146] = 0x225E54

old table[110] = 0x325E5C
arena            = 0x40018800
arena len        = 0x000045A0
new table[110]   = 0x4001CDA0
```

setup 完成后的关键偏移在 **guest 动态 end** 下是自洽的：

```text
H.next = 0x3FDF29A4
B + 0x3FDF29A4 = 0x40018800

node 0x40018800 = { next=0x00096888, len=0x000045A0 }
B + 0x00096888  = 0x2BC6E4

node 0x2BC6E4   = { next=0x3FDF6F44, len=0x00069778 }
B + 0x3FDF6F44  = 0x4001CDA0 = new table[110] sentinel
```

但宿主 `arm_ext_guest_mem_malloc()` 和 free 路径只使用
`m->origin_mem_addr + m->origin_mem_len = 0x325E5C`作为固定 end。
它看到 `B + H.next = 0x40018800 > 0x325E5C` 时，将 wrapper
刚构造的合法动态链判为 corrupted，然后退到 bump 分配。

teardown 仍按已改写的 `table[110]=0x4001CDA0` 遍历。它以为
table[0]/[1] 已经操作同一条动态链，实际却收到了无关 bump
块；零 `next` 把遍历带回 B，最终进入下列环：

```text
0x225E5C: next=0x000003B0  -> 0x22620C
0x22620C: next=0x00000001  -> 0x225E5D
0x225E5D: unaligned next=3 -> 0x225E5F
0x225E5F: unaligned next=0 -> 0x225E5C
```

GDB 两次独立暂停读到 guest PC `0xE82DE6` 和 `0xE82E30`，
LR 都是 `0xE82DBD`，CPSR `0x200001F3` 表明 Thumb 状态，
R9 `0x225E60` 是 wrapper RW。另一持续运行实例 6 分钟仍保持
99.9% CPU，code 0 始终没有返回。

## 8. 根因判定

**高置信根因：同一个 Mythroad LG_mem ABI 被宿主拆成了两套
不同步的边界。**

- guest wrapper 依据 SDK 共享 ABI 改写 `table[110]` 指向的
  `LG_mem_end`，并直接编辑 `table[146]` 空闲链。
- host table[0]/[1]/[2] 虽然也读写这条链，边界判定却使用
  不随 slot 更新的 `origin_mem_len`。
- 真机 `mr_malloc/mr_free`和 `table[110]` 指向同一个全局
  `LG_mem_end`；不存在这个 split-brain。
- 因此 setup 构造的动态 arena 链本身不是坏数据；它是被宿主
  用过期固定 end 误判后，才在 teardown 中变成活锁。

恢复旧版 `table[0]=arm_alloc`、`table[1]=只记账` 可以用作诊断
对照，但它不是最终修复：这会破坏已知依赖 first-fit 地址复用
的应用（例如 talkcat）。

## 9. 通用修复必须满足的不变量

1. host table[0]/[1]/[2] 每次调用时必须与 guest 读到相同的
   `LG_mem_base/LG_mem_end/LG_mem_free`，不得在 guest 扩展 end 后仍用
   过期固定边界。
2. 动态地址空间可以跨 Unicorn 的多个已映射 band；遍历应验证
   **节点及其长度映射有效**，不应假设 `base..end` 在一块
   host 连续内存中。
3. 不能删除 first-fit、禁用 free、在循环中加超时或将错误链
   当空链继续。这些都是改变真机 ABI 的兜底行为。
4. 动态 arena 拆除后，`table[110]`、`table[136]` 和头节点必须
   恢复 setup 前的值，不得残留跨 band 偏移。
5. 修复不得依赖 `sanguo.mrp`、文件哈希、固定运行地址或
   包内特定资源名。

## 10. 验证方法

目标修复后需要同时证明：

1. 定向诊断中 `0xE82C80` 将 end 扩展到 `0x4001CDA0`后，
   host table[0]/[1] 能遍历同一条动态链，不再误报 corrupted
   或退到无关 bump 块。
2. `0xE82DA0` teardown 返回，首次 `_strCom(801, "", 0)` 结束，
   主线程进入正常 timer/SDL 调度，CPU 不再持续占满。
3. 通过 SDL dummy 抓取 PPM，验证不是全黑/单色帧；再结合按键后
   画面变化证明游戏不只是静态 splash。
4. 定向回归必须包含依赖 origin_mem first-fit 的 talkcat，以及使用
   wrapper/private arena 的现有 ARM EXT 用例。
5. 最后执行 unit/ctest 和完整 E2E 兼容性回归，且保留最终
   PPM 与无全量 trace 的小型运行日志作为证据。
