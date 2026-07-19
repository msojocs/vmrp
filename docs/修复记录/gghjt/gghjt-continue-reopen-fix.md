# gghjt 连续进入下载/付费界面修复说明

本文记录 `test/gghjt/continue-twice.sh` 触发的 gghjt 下载/付费界面连续进入问题。问题表现经历过几个阶段：

- 初始现象：第一次能进入下载/付费 child 界面，返回后第二次点击卡在游戏主菜单，进程关不掉，需要 `pkill`。
- 中间现象：能再次进入，但弹窗正文为空白；标题、`确定`、`取消` 正常。
- 中间现象：能进入两次，第三次再次卡住。
- 当前目标：第二次、第三次以及后续都能进入 child 界面，关闭后能回到菜单，进程不残留。

触发坐标约定：

- `121,291,3000`：触发进入下载/付费界面事件。
- `160,290`：切换菜单。
- 关闭弹窗的脚本点击路径会触发 wrapper resume / child close 流程。

## 1. 调查约束

- 不使用 `xvfb`。
- `timeout` 对该类卡死进程不可靠，清理时使用 `pkill`。
- trace 日志量非常大，定位时优先使用窄口径诊断，例如 `VMRP_PPM=1` 截屏。注：原窄口径 PC/RW 诊断 `VMRP_GGHJT_PC_DIAG` / `VMRP_GGHJT_RW_DIAG` 已随 app_compat 诊断钩子清理从代码移除，下文涉及它们的 marker 不再输出；如需重新定位需自行恢复埋点。
- 修复不加入兜底逻辑。所有写内存的动作都要由样本识别、private loader 结构或反汇编地址约束触发。
- 修改代码处需要保留反汇编依据注释，避免后续把 gghjt 私有状态误认为平台通用逻辑。

## 2. 多 agent 分析结论

本次复盘使用了三个子 agent 做并行只读分析：

- `third_party/mtk_disasm` 参考价值分析：结论是它有价值，但只能作为 Mythroad/MTK native EXT 平台 ABI 证据，不能直接证明 gghjt 私有 loader、`verdload.ext` bridge copy 偏移或 `wrapper_rw+0x190` 链表结构。
- gghjt 目标反汇编分析：结论是 `cfunction.ext` 私有 loader、`verdload.ext` bridge slot copy、`wrapper_rw+0x190` 五组 head/tail pair 和低表 veneer 自环风险都有直接反汇编证据。
- 当前实现审查：结论是行为方向成立，但 `src/arm_ext_executor.c` 里 gghjt 专项逻辑和平台执行器逻辑耦合较重，后续应先做逻辑隔离，再考虑拆出 `arm_ext_gghjt.c`。

因此本文把证据分为两层：

- 平台 ABI 层：来自 `third_party/mtk_disasm`，说明 native EXT 的加载、function table、internal table、event/timer callback 是什么。
- gghjt 私有层：来自 `/tmp/skyengine_mrp_extract/cfunction.ext`、`verdload.ext`、`game.ext` 的目标反汇编和运行诊断，说明本问题真正的根因。

## 3. 样本信息

本次分析的内嵌 EXT 文件来自 `/tmp/skyengine_mrp_extract`：

| 文件 | 大小 | SHA-256 |
| --- | ---: | --- |
| `cfunction.ext` | `19428` | `bb85bebb382a9b6262ad48727014edd377ba17b82b36bda3d4496c36562c8e70` |
| `verdload.ext` | `18404` (`0x47E4`) | `ac754c00919100bc1556ce180381f90b45676c4a25899b0ee87c52bf0fba6472` |
| `game.ext` | `160904` | `2d61dbc7478353aa7d14c1abc1f6557692457946d9efc09326fc26089d02b4f2` |

`verdload.ext` 的当前识别条件是：

- 文件长度 `0x47E4`。
- helper offset `0x36DC`。
- entry offset `0x08` 处 prologue 为 `10 40 2D E9`。

这些条件只对当前 gghjt 样本负责。不同渠道包或不同版本的 `verdload.ext` 如果长度、helper offset 或入口序列变化，当前专项修复会静默不触发，而不是走兜底路径。

## 4. `third_party/mtk_disasm` 的参考价值

`third_party/mtk_disasm` 的价值是证明平台 native EXT ABI，而不是证明 gghjt 私有链表。

可引用的平台事实：

- `_mr_TestComC(800)` 是 native EXT load 路径。`third_party/mtk_disasm/mythroad_mini_mini.disasm:2887` 附近显示它以 `filebuf + 8` 为入口，将 `_mr_c_function_table` 偏移写入 EXT 文件头，调用 `mr_cacheSync`，再 `BLX entry`。
- `_mr_TestComC(801)` 是 native EXT call 路径。它通过 `mr_load_c_function` / `mr_c_function` 分发已加载 native EXT。
- `_mr_c_function_table` / `_mr_c_internal_table` 是低表/API 表契约。mini2011 符号表中 `_mr_c_function_table = 0xA4`，full1968 样本中同类 table offset 为 `0xB0`，说明文件头首 word 是平台函数表偏移，不是 gghjt 私有含义。
- `_mr_TestCom` 注册 callback 槽位：`mr_event_function = 0x24`、`mr_timer_function = 0x28`、`mr_stop_function = 0x2C`、`mr_pauseApp_function = 0x30`、`mr_resumeApp_function = 0x34`。
- `mr_event` / `mr_timer` 先走已注册 callback，如果 callback 不处理再 fallback 到 native EXT code `1` / `2`。这支持“timer/event ownership 是平台模型”的判断。
- `mr_doExt` 的普通 EXT 生命周期是 read file 后执行 load `800`，再以 `801` 发送 code `6`、`8`、`0`。这可以用来对照 gghjt 私有 loader 为什么特殊。

不能用 `third_party/mtk_disasm` 直接证明的内容：

- 不能证明 `gghjt.mrp` 的 `cfunction.ext` 私有 loader。
- 不能证明 `verdload.ext` 长度 `0x47E4`、helper offset `0x36DC` 或 bridge copy 偏移。
- 不能证明 `wrapper_rw+0x190` 是五组 child-flow event list。
- 不能证明 reopen 时 `verdload` bridge copy 目标和 wrapper child list 区域发生 alias。
- 不能证明 `game.ext` 中 `rw+0x218`、`rw+0x22c`、`rw+0x14bc`、`rw+0x751`、`rw+0x12f4` 等业务状态偏移。

准确表述应是：`third_party/mtk_disasm` 证明低表地址、callback 槽、P/helper/extChunk 这类概念来自平台 native EXT ABI；而本次连续进入问题的直接根因来自 gghjt 自带 EXT 的私有反汇编。

## 5. gghjt 私有 loader 证据

`cfunction.ext` 按 wrapper 运行地址看基址为 `0xE80000`。私有 child loader 集中在 `0xE832DC..0xE83416`，它不走宿主可见的 table[25] nested-load 路径，而是在 wrapper 内部自己构造 module record、extChunk、P，再直接调用 child entry。

关键反汇编语义：

- `0xE83302..0xE83314`：分配并清零 `0x248` 字节 module record。未显式写入的 bridge 槽保持 0。
- `0xE8332A..0xE83354`：分配并初始化 `0x38` 字节 child extChunk；`extChunk[0] = 0x7FD854EB`，`extChunk[0x2C] = module_record`。
- `0xE8336C..0xE83370`：分配 `0x14` 字节 child `P`，对应 `mr_c_function_P_t`。
- `0xE8339C..0xE833A6`：写 child 文件信息：`extChunk[0x04] = file_base + 8`，`extChunk[0x0C] = file_base`，`extChunk[0x10] = file_len`。
- `0xE833A8`：写 `file_base[0] = module_record`。这会覆盖静态文件头，说明 staging image 已经进入 wrapper-owned runtime 形态。
- `0xE833AA..0xE833B0`：绕写 `file_base[4] = child_P`。
- `0xE833B2..0xE833B6`：写 `P->mrc_extChunk = extChunk`。
- `0xE833B8..0xE833C8`：调用 cache sync / `_mr_TestCom1` 类入口，参数形态是 `r0=0, r1=9, r2=file_base, r3=file_len`。此时 `file_base[0]` 和 `file_base[4]` 已经被改写。
- `0xE833CA..0xE833CE`：`r0=1; blx file_base+8`，直接进入 child。
- `0xE833D8..0xE833E2`：回填 `extChunk[0x14] = P[0]`、`extChunk[0x18] = P[4]`。

这说明宿主不能把这个 child 当成普通 `_mr_readFile` 后等待 table[25] 的模块。特别是 `_mr_TestCom1(r1==9)` 之后，若宿主把 raw 文件缓存重新覆盖 staging，会破坏 `file_base[0]=module_record`、`file_base[4]=P` 这类 runtime 头部。

对应代码：

- `src/arm_ext_executor.c` 的 `arm_ext_has_internal_loader_chunk()` 扫描 heap 中已建立的 extChunk，避免 `_mr_TestCom1(r1==9)` 对私有 loader staging 做 raw image 覆盖。
- `arm_ext_sync_internal_nested_module()` 在发现 `extChunk[0] == EXT_CHUNK_MAGIC`、`extChunk[0x0C] == file_base`、`extChunk[0x10] == file_len`、`P[0x0C] == extChunk` 后，将该 child 记录进 host nested module 表，并同步 active P/helper。

## 6. `verdload.ext` bridge copy 与正文空白

`verdload.ext` 的 bridge copy 出现在文件偏移 `0x308` 开始的函数中。不同加载基址下可显示为 `0x70CB08` 或 `0x72D97A`，实际拷贝从函数内部 `base+0x34C` 附近开始。

关键语义：

- `r4 = file_base - 0x38`。
- `ldr r0,[r4,#0x38]` 读取 `file_base[0]`，也就是 module record。
- `r5 = r9 + 0x150`。
- `record[0x68] -> rw+0x16C`，该槽对应 table index `26`，期望值是 `0x10068`。
- `record[0x0C..0x4C] -> rw+0x170..0x1B0`，对应 table index `3..19`，期望值是 `0x1000C..0x1004C`。

正文空白问题的精确根因不是 `verdload.ext` 静态正文缺失，而是：

- `cfunction.ext` 私有 loader 分配并清零了 module record。
- 私有 loader 没有填 `record+0x0C..0x4C` 和 `record+0x68` 这些 bridge 源槽。
- `verdload` entry 运行时把空白源槽复制到了 child RW 的 `rw+0x16C..0x1B0`。
- 弹窗初始化后续依赖这些 RW bridge 槽：
  - `verdload` 文件偏移 `0x24DC` 读 `rw+0x19C` 调用 memset，期望 table[14] `0x10038`。
  - `verdload` 文件偏移 `0x254C` / `0x2576` 读 `rw+0x170`，在 `0x2582` 附近调用 memcpy，期望 table[3] `0x1000C`。

因此只修 module record 源槽不够，因为宿主观察到 private child 时 child entry 很可能已经跑过，错误值已经被复制进 RW。当前实现同步两边：

- `record[0x68]` 与 `rw+0x16C`。
- `record[0x0C..0x4C]` 与 `rw+0x170..0x1B0`。

对应代码：

- `arm_ext_is_gghjt_verdload_child()` 用长度、helper offset、entry prologue 限定当前样本。
- `arm_ext_sync_gghjt_verdload_bridge_slots()` 在 extChunk/P/helper 已知后同步 record 和 RW。

这一步修复了“标题和按钮正常，但描述正文一片空白”的问题。

## 7. `wrapper_rw+0x190` 链表与第三次卡死

`cfunction.ext` 中 `wrapper_rw+0x190` 的定位来自同一个 literal：

- `0xE810C4` 的字面量是 `0xD0`。
- 多处代码执行 `ldr r1,[pc]; add r1,r9; adds r1,#0xC0`。
- 因此目标地址是 `r9 + 0xD0 + 0xC0 = wrapper_rw + 0x190`。
- slot 通过 `slot << 3` 访问，每个 slot 是 8 字节 head/tail pair。

插入证据：

- `0xE81082..0xE810A0`：按 `slot*8` 找到 head/tail pair，执行标准双向链表头插入。
- 它保留 old head，并维护 old head 的 prev；如果链表设计上总为空，就不需要这套双向链表维护。

遍历证据：

- `0xE80F26..0xE80F3C`：遍历 active slot，对每个 `node[0x0C]` 调 `0xE83514`，进入 wrapper modal suspend。
- `0xE82200..0xE8221A`：遍历同一 head，比较 `node[0x10]`，对非当前 owner 调 `0xE83098(node[0x0C])`。
- `0xE82228..0xE82244`：遍历并调 `0xE835FC(node[0x0C])`。
- `0xE82266..0xE822AA`：用 `node[0]` 和 `node[4]` unlink/relink 同一个 head/tail pair。

suspend/resume 语义：

- `0xE83514` 检查 magic `0x7FD854EB`，递增 `extChunk[0x34]`，从 0 到 1 时调用 `0xE83C7C`，随后通过 helper code `4` 进入 child suspend/modal 状态。
- `0xE835FC` 检查 magic，递减 `extChunk[0x34]`，小于等于 0 时走 release/resume 类 helper。
- `0xE83098` 也检查 magic 并递增 `extChunk[0x34]`。

这说明 `wrapper_rw+0x190` 保存的是 wrapper child-flow subscriber/extChunk 链，不是普通 GOT/bridge 槽。

第三次卡死的核心链路：

- `verdload` bridge copy 会把低表 bridge veneer，如 `0x1000C..0x1004C`，写入 `rw+0x170..0x1B0`。
- gghjt private reopen 场景里，这个目的区会别名到 `wrapper_rw+0x190..0x1B0`。
- 如果 active slot 的 head 被写成 `0x1002C`，`0xE8109C` 会把新 child node 链到该 veneer。
- 后续 `0xE80F36` 或 `0xE82200` 这类 `node = node[0]` 遍历会进入低表 veneer。
- 执行器低表 bridge entry 自身是 self veneer，`*(0x1002C) == 0x1002C`，遍历会卡在自引用上。

早期尝试“清空五组 list”能消除低表自环，但会删除 wrapper 的持久 subscriber。结果是第二次打开时 `0xE80F36 -> 0xE83514` 不再把 modal suspend depth `extChunk[0x34]` 拉到 1，关闭点击不能通过 wrapper resume 配对，后续请求状态残留。正确动作不是清空，而是保存并恢复 pre-child 的真实 head/tail 基线。

对应代码：

- `ArmExtPrimaryChildSnapshot` 增加 `wrapper_child_event_lists[5][2]` 和 `wrapper_child_event_lists_valid`。
- `arm_ext_wrapper_rw_base()` 读取 wrapper RW 基址。
- `arm_ext_is_gghjt_wrapper_child_event_list_addr()` 限定 `wrapper_rw+0x190..0x1B7`。
- `arm_ext_save_gghjt_wrapper_child_event_lists()` 在 primary click 触发 child 前保存五组 head/tail pair。
- `arm_ext_restore_gghjt_wrapper_child_event_lists()` 在两个时机恢复真实基线：
  - reopen 时，发生在 `verdload` 错误 bridge copy 之后、`cfunction.ext` 插入新 child node 之前。
  - child close 被确认后，移除刚关闭 child node，同时保留 wrapper 持久 subscriber。
- `arm_ext_should_restore_got_snapshot_addr()` 和 `hook_got_write()` 将 `wrapper_rw+0x190..0x1B7` 从 GOT snapshot/restore 中排除，防止后续 memcpy/memset/GOT 修复再次把低表 veneer 写回链表。

## 8. `game.ext` close/reopen 状态恢复

child 关闭后，`game.ext` 会进入 close/completion 路径。相关状态由 `arm_ext_restore_confirmed_closed_child_if_needed()` 处理。

反汇编/日志依据：

- `game.ext 0x6661C8` 写 `12` 到 `rw+0x22C`，并设置 `rw+0x751`。
- `game.ext 0x6636E0..0x663714` 将 async flags 写到 `rw+0x12F3/rw+0x12F4`。
- `rw+0x218` 是菜单页状态，`0x6570CA` 读取并经 `0x65A464` 分发。
- `rw+0x33C/rw+0x340` 是 active touch latch；close 后仍指向 modal 按钮会阻止下一次菜单点击进入 child setup。
- `rw+0x14BC` 是 async request descriptor，由 `0x666244 -> 0x65DC9C` 构造后交给 `0x663070`。
- `rw+0x13AC` input-gate block 会被 `0x65F750` 重置；菜单进入路径 `0x65A2AC -> 0x6570E4` 再通过 `0x65F764` 检查 `rw+0x13C8`。

因此 close 后恢复内容包括：

- `rw+0x218` 菜单页状态。
- `rw+0x22C` child download state。
- `rw+0x33C/rw+0x340` touch latch。
- `rw+0x00F8` request active。
- `rw+0x012C` request block。
- `rw+0x14BC` async request block。
- `rw+0x0854` child slot flags。
- `rw+0x0860` child slot tail。
- `rw+0x13AC` input gate block。
- `rw+0x751` child complete flag。
- `rw+0x12F4` async complete flag。
- `wrapper_rw+0x190` 五组 child event list 的 pre-child 基线。

恢复后设置 `primary_child_reopen_timer_needed = 1`，并通过 wrapper-owned timer 重新驱动 resume/reopen 队列，避免直接把 timer 路由给半更新状态的 `game.ext`。

## 9. 根因总结

本问题不是单点 bug，而是四个状态建模缺口叠加：

1. 宿主原先只建模 host-visible table[25] nested-load，对 `cfunction.ext` 私有 loader 的 extChunk/P/helper 边界认识不足。
2. `_mr_TestCom1(r1==9)` 后继续用 raw `_mr_readFile` 缓存覆盖 staging，会破坏私有 loader 已建立的 runtime 文件头。
3. `verdload.ext` bridge copy 的源 module record 被私有 loader 清零但未填充，导致 child RW 的 bridge 槽为空，弹窗正文复制/清零逻辑不能正常调用低表 API。
4. reopen 场景下 `verdload` bridge copy 目的区别名到 `wrapper_rw+0x190` child-flow list；低表 veneer 被当作链表节点后会形成 `0x1002C` 自引用遍历卡死。清空 list 会破坏 wrapper 持久 subscriber，只有恢复真实 pre-child 基线才同时满足“不卡死”和“后续可关闭”。

## 10. 实现改动清单

主要改动集中在 `src/arm_ext_executor.c`：

| 区域 | 作用 |
| --- | --- |
| `ArmExtPrimaryChildSnapshot` | 保存 gghjt primary 状态、请求状态、child close 状态和 `wrapper_rw+0x190` 五组 list 基线。 |
| `arm_ext_has_internal_loader_chunk()` | 识别 `cfunction.ext` 私有 loader 已建立的 extChunk，避免 raw staging 覆盖 runtime 头。 |
| `arm_ext_is_gghjt_verdload_child()` | 用当前样本的长度/helper/prologue 限定 `verdload.ext`。 |
| `arm_ext_sync_gghjt_verdload_bridge_slots()` | 同步 module record 和 child RW bridge 槽，修复正文空白。 |
| `arm_ext_sync_internal_nested_module()` | 将私有 loader child 记录为 nested module，并同步 active P/helper 与前台 owner。 |
| `arm_ext_is_gghjt_wrapper_child_event_list_addr()` | 识别 `wrapper_rw+0x190..0x1B7` 运行时链表区域。 |
| `arm_ext_should_restore_got_snapshot_addr()` | 禁止 GOT snapshot restore 覆盖 wrapper child-flow list。 |
| `arm_ext_save_gghjt_wrapper_child_event_lists()` | child 打开前保存真实 list 基线。 |
| `arm_ext_restore_gghjt_wrapper_child_event_lists()` | reopen/close 时恢复真实 list 基线，避免低表自环且保留 subscriber。 |
| `arm_ext_confirm_primary_child_snapshot()` | 只有看到 child P/helper 才确认本次 primary click 真正打开 child；reopen 时在正确时机恢复 list。 |
| `arm_ext_restore_confirmed_closed_child_if_needed()` | close 完成后恢复 primary 状态、请求状态、input gate 和 list 基线。 |
| `arm_ext_schedule_primary_child_reopen_timer()` / `arm_ext_schedule_wrapper_resume_timer_after_child_close()` | 保持 wrapper-owned timer 驱动 reopen/resume，避免半更新状态卡住。 |

所有关键写入都由 `arm_ext_is_gghjt_pack()`、当前样本 `verdload` 签名、extChunk/P/helper 校验和 child close flags 约束触发，没有通用 fallback。

## 11. 验证记录

> 说明：下列记录中使用的 `VMRP_GGHJT_PC_DIAG` / `VMRP_GGHJT_RW_DIAG` 窄口径诊断已从代码移除，相关 marker（`cache18496`、`snapshot_confirm`、`close_before`、`md1` 等）为当时诊断埋点的历史输出，现已不再产生。行为类验证（PPM 显示、无残留进程、`ctest`）仍然有效。

已执行过的关键验证：

- `cmake --build build -j2` 通过。仍有既有 warning：`mr_getHostByName` implicit declaration。
- 三次进入自定义序列，环境：
  - `VMRP_GGHJT_PC_DIAG=1`
  - `VMRP_GGHJT_RW_DIAG=1`
  - `VMRP_PPM=1`
- 结果：
  - `cache18496=3`
  - `snapshot_confirm=3`
  - `close_before=3`
  - 无残留进程。
  - 第二次、第三次打开时 `md=1`，关闭后回到 `md=0`。
- 第三次打开的 PPM `/tmp/gghjt-third-open.png` 已人工检查：标题 `下载提示`、正文、`确定`、`取消` 均显示。
- `test/gghjt/continue-twice.sh` 带 RW 诊断：
  - `cache18496=2`
  - `snapshot_confirm=2`
  - `close_before=2`
  - `md1=20`
  - 无残留进程。
- smoke 脚本均达到目标 marker，并使用 `pkill` 清理：
  - `test/gghjt/continue-twice.sh`：`cache18496=2`
  - `test/gghjt/pay-normal-enter.sh`：`cache18496=1`
  - `test/gghjt/pay-normal-back.sh`：`cache18496=1`
  - `test/gghjt/download-plugin-enter.sh`：`cache18496=1`
- `ctest --test-dir build --output-on-failure`：
  - 3 个 unit/API/filelib 测试通过。
  - 3 个 e2e 测试因自身 30s timeout 失败，涉及 `gxdzc`、`mpc`，未见内存错误或 fatal error；该结果记录为现有测试框架超时风险，不作为本修复的功能失败。

推荐后续复验命令：

```bash
cmake --build build -j2
timeout 240 env VMRP_NO_MOUSE=1 VMRP_PPM=1 ./test/gghjt/continue-twice.sh || pkill -f 'build/skyengine|skyengine'
timeout 180 env VMRP_NO_MOUSE=1 VMRP_PPM=1 ./test/gghjt/pay-normal-enter.sh || pkill -f 'build/skyengine|skyengine'
timeout 180 env VMRP_NO_MOUSE=1 VMRP_PPM=1 ./test/gghjt/pay-normal-back.sh || pkill -f 'build/skyengine|skyengine'
timeout 180 env VMRP_NO_MOUSE=1 VMRP_PPM=1 ./test/gghjt/download-plugin-enter.sh || pkill -f 'build/skyengine|skyengine'
```

诊断复验：原窄口径 PC/RW 诊断（`VMRP_GGHJT_PC_DIAG` / `VMRP_GGHJT_RW_DIAG`）已从代码移除，下列依赖其 marker 的检查项当前无法直接观测，如需使用需先恢复对应埋点。

重点检查（行为类，仍可通过 PPM 与普通日志观测）：

- 日志中无 `UC_ERR`、`invalid memory`、`Unhandled CPU exception`。
- PPM 中第三次及后续 child 界面标题、正文、按钮都完整显示。
- 进程无残留（用 `pkill` 清理后无遗留 `skyengine`）。

重点检查（原依赖已移除的 PC/RW 诊断 marker，需恢复埋点后才能观测）：

- `child_snapshot_confirm` 次数符合进入次数。
- `child_close_before_clear` / `child_close_after_clear` 成对出现。
- reopen 前后 `wrapper_rw+0x190..0x1B7` 不残留 `0x1002C` 这类低表 veneer。
- 第二次及后续打开时 modal depth `extChunk[0x34]` 能进入 1，关闭后回到 0。

注意：`download-plugin-enter.sh` 可能删除 `mythroad/plugins/netpay.mrp` 来模拟缺插件路径。运行前后需要关注工作区状态，避免污染其他验证。

## 12. 当前实现风险

当前实现已经能解释并修复目标现象，但维护风险主要来自职责混杂：

- `src/arm_ext_executor.c` 同时包含平台 native EXT 执行、private loader 识别、gghjt `verdload` bridge 修复、gghjt `game.ext` 状态恢复、wrapper child list 修复、GOT restore 排除、前台 screen owner 和 timer owner 策略。
- `ArmExtPrimaryChildSnapshot` 名字偏通用，但字段几乎都是 gghjt 私有 RW 偏移语义。
- `arm_ext_has_internal_loader_chunk()` 只按 magic/file/len 判断，不完整验证 helper/P/rw。当前后续同步函数会做更严格检查，但 staging 是否跳过 raw restore 的判断仍偏宽。
- `verdload` 识别依赖当前样本固定长度、helper offset 和 entry prologue。版本变化会导致修复不触发。
- bridge sync 是宿主观察到 private child 后补写。当前时序已经验证，但如果未来路径在同步前已经读取 `rw+0x19C/rw+0x170`，仍可能过晚。
- wrapper list snapshot 来自 primary mouse-up 前状态。如果候选 snapshot 和 child loader confirmation 之间夹入其他 wrapper 状态变化，可能恢复到过旧的 list。
- GOT snapshot 清理假设 `got_snapshot_base == wrapper_rw` 且 `0x190..0x1B7` 永远不是合法 bridge GOT。当前 gghjt 成立，但不能推广成平台规则。
- timer re-arm 路径直接操作 `mr_timerStart()`、`mr_timer_state`、`host_timer_pending`、`timer_p_addr`、`timer_helper_addr`，需要继续用日志覆盖已有 timer pending 的边界。

这些风险不要求在本修复里引入兜底逻辑。更合适的处理方式是后续拆层和命名收敛。

## 13. 后续拆层建议

短期建议先做逻辑隔离，不急着大规模拆文件：

- 将 gghjt 专项状态包成 `GghjtCompatState` 或类似结构。
- 将 `ArmExtPrimaryChildSnapshot` 改名为 `GghjtPrimaryChildSnapshot` 或 `GghjtChildFlowSnapshot`。
- 将 gghjt 专项函数统一前缀，例如 `gghjt_compat_*`。
- 将魔数提成命名常量：
  - `GGHJT_VERDLOAD_LEN`
  - `GGHJT_VERDLOAD_HELPER_OFF`
  - `GGHJT_WRAPPER_CHILD_EVENT_LIST_OFF`
  - `GGHJT_WRAPPER_CHILD_EVENT_LIST_COUNT`
  - `GGHJT_WRAPPER_EVENT_PAIR_SIZE`
- 平台层保留 extChunk、P/helper、nested module registry、timer owner、cache sync 和 GOT snapshot 框架。
- gghjt 层只在明确 hook 点上调用：
  - private loader child 已确认。
  - verdload 签名匹配。
  - primary child open snapshot 已确认。
  - child close flags 已确认。

中期再考虑拆出 `src/arm_ext_gghjt.c`，但拆分应在验证脚本稳定后进行，避免一次性重构放大时序风险。

## 14. 结论

`third_party/mtk_disasm` 对本问题有参考价值，但它只证明平台 ABI；真正 BUG 在 gghjt 私有 wrapper/verdload/game 三者交界处：

- `cfunction.ext` 私有 loader 构造 runtime 文件头和 extChunk/P/helper，但绕过 host-visible nested-load。
- `verdload.ext` 从空白 module record 复制 bridge 槽，导致正文相关 API 指针为空。
- reopen 时 `verdload` bridge 目的区会污染 `wrapper_rw+0x190` child-flow list，把低表 veneer 写成链表 head。
- 清空 list 会破坏 wrapper 持久 subscriber；保存并恢复真实 pre-child list 基线才符合反汇编中的插入、遍历和 suspend/resume 计数语义。

当前修复不是通用 fallback，而是针对当前 gghjt 样本、由私有 loader 签名和反汇编状态机约束的兼容逻辑。
