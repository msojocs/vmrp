# arm_ext_executor.c 问题分析

日期：2026-07-06
分析对象：`src/arm_ext_executor.c`（10002 行，约 448KB，约 303 个函数）
分析方式：全文件分段人工审查 + 横切面统计，关键结论已回读源码核实。

> 说明：本项目是单机 MRP 模拟器，运行用户自己选择的 MRP 应用，**guest（被模拟的 ARM EXT 代码）不视为攻击面**。因此本文档不把"构造恶意 guest 越界读写宿主内存"当作安全漏洞来评估。列出的内存相关问题，均是**在真实应用正常/异常运行时会实际触发崩溃、花屏、内存破坏或泄漏**的健壮性缺陷。

---

## 一、总体判断

这个文件把过多职责压进了单个翻译单元，并且积累了大量针对具体应用二进制的特化代码。两个最突出的结构性问题：

1. **单文件职责混杂**：至少 8 类互不相关的职责挤在一起（MRP 包解析、gzip、虚拟 FD、嵌套模块注册、屏幕脏区跟踪、定时器队列扫描、诊断 watch、Thumb 字节码模式匹配、Unicorn 执行核心 + table 分发）。其中 `hook_table`（约 6315–7796 行，约 1480 行）和 `arm_ext_call`（约 8903–9456 行，约 550 行）是两个超大函数。

2. **应用特定字节码指纹泛滥**：约 30+ 个"识别特定二进制指令模式"的函数，很多本质是对单个 `.ext` 编译产物的特征码，与项目规范"禁止应用特定代码"正面冲突（详见第三节）。

---

## 二、真实运行中会触发的 Bug（按影响排序）

### B1. 逻辑画布放大时回拷宿主屏幕缓冲越界写（高）

- 位置：`leave_screen_context`（约 5577–5594）、`arm_ext_call`（约 9384–9399）、`arm_ext_call_dispatch`（约 9859–9873）三处重复逻辑。
- 现象：这些路径按 `m->screen_w * m->screen_h` 逐像素把 ARM 侧屏幕拷回宿主 `mr_screenBuf`。宿主 `mr_screenBuf` 按 `vmrp_config` / `mr_screen_w * mr_screen_h` 分配，而 `m->screen_w/h` 可被 `hook_screen_dim_write` / `arm_ext_push_draw_screen_context`（约 5697）采纳为 guest 设定的逻辑画布尺寸。校验只有 `≤2048×2048` 和 `w*h*2 ≤ EXT_SCREEN_RESERVE(1MB)`，**从未与宿主缓冲实际容量比较**。
- 触发场景：游戏把逻辑画布设得比宿主物理窗口大（如宿主 240×320、guest 480×320）时，回拷会越过宿主堆缓冲末尾 —— 这是真实游戏会遇到的正常行为，不是恶意构造。
- 修复方向：回拷前用宿主缓冲真实容量（`mr_screen_w/mr_screen_h`）夹断拷贝行数/列数；或在采纳尺寸处同时校验宿主容量。

### B2. 写屏/写定时器头时 `arm_ptr` 返回 NULL 未判空，直接段错误（中高）

同一个反模式在多处出现：拿到 `arm_ptr(m, addr)` 后不判空就交给 `memcpy`/宿主 API。当 `addr` 落在未映射区间（真实运行中状态损坏、字段未初始化都可能发生）时 `arm_ptr` 返回 NULL，宿主立即崩溃。

- `write_game_timer_head`（约 4311–4322）：两个候选偏移都未映射时，走 `memcpy(arm_ptr(m, grw + 主偏移), ...)` 而该指针为 NULL。对照 `read_game_timer_head` 每次读都判了空，写侧漏判。
- `hook_table` case 3/4/5/6/7/8/9（约 6420–6442）：`memcpy`/`strcpy`/`strcat` 前不判空（对照 case 16/113/114 有判空）。
- `hook_table` case 34/35/80（约 6729–7196）：`mr_getDatetime`/`mr_getUserInfo`/`mr_getScreenInfo` 直接收 `arm_ptr(m, r0)`，NULL 时宿主向 NULL 写结构体。
- `hook_table` case 2 realloc（约 6414）：`memmove(dst, arm_ptr(m, r0), n)`，old 指针非法时 `arm_ptr` 为 NULL。
- `hook_table` case 44 真实读分支（约 6976）：虚拟 fd 分支有 `if (n && hp)`，真实 `mr_read((int32)r0, hp, r2)` 分支没有判空。
- `patch_wrapper_stack_size`（约 8378–8383）：读写 code 区 `arm_ptr` 结果不判空（此处通常有效，风险偏低）。

修复方向：为 `arm_ptr` 增加"带长度、返回可用区间"的变体（如 `arm_ptr_span(m, addr, need)`），或在这些调用点统一判空 + 长度夹断。

### B3. `arm_alloc` 长度整数回绕（中）

- 位置：`arm_alloc`（约 686–711）。
- 问题：`align4(len)` 在 `len` 接近 `UINT32_MAX` 时 `(len+3)&~3` 回绕为很小的值；`m->heap_top + len >= mem_end` 在 `len` 巨大时也会回绕绕过检查，返回"成功"地址但实际无容量。
- 触发：`case 132`（`c2u` 结果 `out_size` 为负转 uint32）、`case 25`（`p_len` 直接来自 guest）等把外部长度传入 `arm_alloc` 的路径。目前多数调用点有 `if (!addr)` 兜住，但依赖回绕后恰好返回 0 并不可靠。
- 修复：入口拒绝 `len > EXT_MEM_SIZE`（等价地 `heap_top + len < heap_top` 判溢出）。

### B4. `arm_ext_host_cache_sync` 短文件同步长度不一致（中）

- 位置：约 9962–9967。
- 问题：`copy_len = min(len, flen)` 只拷贝了 `copy_len` 字节，但随后 `arm_ext_sync_internal_nested_module(m, addr, flen)` 按完整 `flen` 解析。当宿主数据比声明的 pending 长度短时，`[addr+copy_len, addr+flen)` 是陈旧/未初始化数据，nested module 解析会越过有效数据。
- 修复：同步也用 `copy_len`。

### B5. `char_bitmap` 固定 32 字节缓冲，依赖字体上限（中）

- 位置：`hook_table` case 30（约 6638–6644）。
- 问题：缓冲固定 `arm_alloc(m, 32)`，`memcpy` 长度 `bitmap_size = ((width*height)+7)>>3` 来自字体尺寸。当前 sky12/sky16 最大恰为 32 字节，但这是隐式依赖，字体返回更大尺寸就越界写 ARM 堆。缺 `bitmap_size <= 32` 断言。

### B6. 资源/分配失败处理不一致（中）

- `arm_ext_call`（约 8928–8929）：`outp_addr`/`outl_addr = arm_alloc(m, 4)` 未判 0，而同函数 `input_addr` 有 `if (!input_addr) return MR_FAILED;`。返回 0 时 helper 会向地址 0 写输出。
- `init_table`（约 8462–8465）：`m->screen_addr = arm_alloc(...)` 可能返回 0，后续把 0 当有效屏幕地址写入 table / bitmap descriptor。
- `arm_ext_record_nested_module`（约 1259–1265）：注册表满时仅在 `VMRP_ARM_EXT_TRACE` 下打印一行后 `return`，默认构建无任何提示。该模块此后在 PC→owner 归属查找中查不到，事件被派发给错误 owner。这是功能被破坏的状态，应无条件告警。
- `arm_ext_bump_reuse`（约 4178–4197）：命中空闲块后先摘除空闲表再 `arm_ext_bump_track`，track 失败（宿主 OOM）时块既不在空闲表也不在 live 表，永久泄漏。应先 track 成功再改空闲表。

---

## 三、死代码 / 常驻的临时诊断（违反规范）

项目规范要求"不要有兜底代码逻辑"，且注释标"临时/修复后删除"的代码不应常驻。以下均已确认：

### D1. 恒不执行的死代码

- `arm_ext_call_dispatch` 的 `depth_patched`（约 9698 声明）：全函数从未被赋非 0 值，导致约 9742–9744 的 suspend depth 恢复块永远不执行。注释自称"旧版兼容路径，正常保持 no-op"。要么删除，要么恢复其逻辑。
- `arm_ext_guest_mem_malloc`（约 3932–3933）：`while` 循环末尾的 `previous = nextfree; nextfree = base + node_next;` 不可达 —— 每个分支都以 `return` 或显式 `continue` 结束。残留死代码误导读者以为存在正常链表推进路径。
- `leave_screen_context`（约 5584–5593）：`arm_ext_screen_stride(m)` 恒返回 `m->screen_w`（约 888 行），`stride == m->screen_w` 恒真，逐行拷贝的 `else if` 分支不可达。
- `arm_ext_dispup_rect`（约 5392–5420）：`... ? MR_SUCCESS : 0` 两分支同为 0，`w<=0` 也返回 0，失败与成功不可区分，是自欺欺人的三目表达式。

### D2. 标注"修复后删除"却常驻在关键路径的诊断 watch 子系统

- `hook_watch_write`（约 8017–8113）、`arm_ext_watch_sentinel_check`（约 8115–8163，末尾明写 `/* ---- 临时诊断结束 ---- */`）。
- `arm_ext_watch_module_event` 调用点标 `(修复后删除)`：register（约 1269）、drop_stale（约 2532）、reset_child_modules（约 9581）、load（约 8754）—— 全在模块生命周期关键路径上。
- 约 8058–8110 针对具体 guest bitmap blitter 栈偏移的取证 dump。
- watch 子系统的 `static ArmExtWatchState slots[8]`（约 7956–7965）`used` 只增不减，`arm_ext_unload` 不回收，同进程反复 load/unload 超 8 次后 watch 静默失效，且 slot 持有已释放 `m` 的悬空指针（ABA 风险）。

这些虽由环境变量门控、运行时开销小，但属明确的代码债，应随对应修复移除或收敛到统一 diag 开关。

---

## 四、应用特定字节码指纹（违反规范核心条款）

项目规范第 1 条明确禁止"应用特定代码，包括伪装成通用启发式实际只匹配某特定应用二进制特征的代码"。这是全文件最集中的规范问题：约 30+ 个函数对具体 `.ext` 反汇编得到的固定 Thumb 指令序列做精确匹配。注释里直接点名了应用与镜像地址，恰好证明其匹配对象是单一二进制：

| 函数（约行号） | 硬编码特征 | 点名应用/地址 |
|---|---|---|
| `arm_ext_child_has_compact_r9_state_list`（1755） | 0xB5F0 起始 + 窗口内 4448/204C 等 | verdload.ext 0x2CBCFC… / brwmain 0x2CF5CA |
| `arm_ext_child_has_compact_timer_walker`（1808） | 32 字节 Thumb 机器码，仅通配 2 字节 | — |
| `find_wrapper_compact_heap_free_return`（1837） | 17 个固定偏移精确半字（整函数字节匹配） | Compact SDK mr_free |
| `arm_ext_child_reads_record100_to_compact_r9_buffer`（1889） | 7 半字精确序列（含 movs r1,#100） | verdload.ext 0x2CA58A / 0x2CD810 |
| `arm_ext_child_needs_short_pack_alias`（1934） | 4 组不同应用变体的精确指令序列 | frame.ext / DOTA A801 拼写 |
| `arm_ext_mirror_read_file_to_adjacent_slot`（2474） | 返回地址 `0x00000B82` 门槛 | wrapper 0xE80B82 call site |
| `find_wrapper_timer_dispatch`（8266） | pat/chain_thunk_pat/pat2 三组字节序列 | gxdzc/gghjt/gwkdl cfunction.ext（解压 19428 字节） |
| `patch_wrapper_stack_size`（8371） | PatchWord 表（具体指令编码） | 特定 wrapper |
| private child RW 布局族（1300–1753） | `std_bridge_copy_rw_runs` 0x16C/0x170/17 槽 | gghjt cfunction.ext |
| timer node magic `0x79ABBCCF`（2700/2804/2815/4327） | 结构标记 | frame.ext 0x2C9486 / optwar cfunction.ext |
| 调度器偏移探测（4416 `{0x84,0x80}`、4464 `{0x0C0,0x248}`） | 固定偏移数组 | gzwdzjs / DOTA / verdload |

**评估**：这些代码里，凡"最终判据是运行时数据结构校验（如 `0x79ABBCCF` magic 验证后才动作，误匹配即静默跳过）"的，属规范灰区、注释也给了证据来源，可辩护为 SDK 变体适配；但"零通配的整函数字节匹配"（如 `find_wrapper_compact_heap_free_return`、`arm_ext_child_has_compact_timer_walker`）对编译器版本/优化级别极其脆弱，本质是单应用指纹，每出现一个新变体就要往数组里加一组分支。

**建议**：逐个函数标注"这匹配的是 SDK 稳定形状，还是某个 `.ext` 指纹"。指纹类应迁移到已存在的 `app_compat_*.c`（`app_compat_select`/`profile->init`）机制，或改为解析运行时结构（magic + 布局校验），而不是从字节形状猜身份。

此外还有多处显式 fallback 兜底链（规范禁止）：`arm_ext_short_pack_alias_for_host_path`（约 621–656，四级回退含 `strstr "mythroad/"` 路径启发式）、`arm_ext_child_package_context`（约 1170–1227，三级归属判定）、`hook_table` case 40/42/46/125（磁盘找不到→查 MRP 缓存虚拟 fd）、`hook_low_zero`（约 6303，"return-to-LR 兜底"）、`arm_ext_call` 约 9262 的 "broad wrapper busy bit is only a fallback"。

---

## 五、可维护性问题（重大）

### M1. 两个超大函数

- `hook_table`（约 6315–7796，约 1480 行）：单个 switch，case 号乱序（…60,61,81,82…69,70…），case 内交织大段 `if (getenv("VMRP_ARM_EXT_DIAG"))` 寄存器 dump。
- `arm_ext_call`（约 8903–9456，约 550 行）：15+ 个 `code == 2 && …` 嵌套分支，多段几乎逐字重复（约 9232–9284 四段 `mr_timerStart(50); mr_timer_state=1; …`；模态屏幕恢复块与 `arm_ext_call_dispatch` 逐字节重复，含 `extern` 声明重复出现）。任一处修复容易漏改另一处。

### M2. getenv 未缓存、无文档

- 9 个 `VMRP_*` 开关（`VMRP_ARM_EXT_DIAG` 45 处、`VMRP_ARM_EXT_TRACE` 19 处、`VMRP_WATCH_*`、`VMRP_ARM_EXT_TRACE_PC`、`VMRP_ARM_EXT_TIMER_LIVENESS_DIAG`、`VMRP_ARM_EXT_SCREEN_DIAG`、`VMRP_MEMORY`），共 72 处 `getenv`，无集中文档。
- 热路径重复调用：`hook_table`（约 6329）、`hook_intr`（约 3003）每次进入都 `getenv`；单个 case 内多次调同一 env。watch 系列用了 `static int parsed = -1` 缓存，说明作者知道这个模式，只是未统一。可在 `arm_ext_load` 时一次性缓存进 `ArmExtModule`。

### M3. 魔法数字泛滥

- extChunk 偏移（`0x04/0x08/0x0C/0x14/0x18/0x1C/0x20/0x24/0x28/0x2C/0x34/0x37/0x38`）在 `arm_ext_read_internal_loader_tuple`、诊断块等多处重复裸写，无统一 struct 或宏。`0x34`（suspend 深度）语义裸写约 10 处。修改 extChunk 布局（公共 ABI）时需同步多处，极易漏改。
- RW/scheduler 偏移（`0x94/0x9C/0xA0/0xC0/0x248/0x3C8/0x3D8`）、timer 节点字段偏移（`0x08…0x1C`）、Thumb opcode 常量（83 处比较，无命名）同样裸写。
- 已有 `GAME_TIMER_HEAD_OFFSET` 这样的命名先例，应推广。建议至少把 extChunk 偏移收进 `arm_ext_internal.h` 的命名宏。

### M4. 重复实现

- `arm_ext_bytes_contain`（约 413）朴素扫描 vs `arm_ext_staged_file_ram_package`（约 1106）另一份手写扫描循环，未复用。
- `arm_ext_diag_dump_wrapper_compact_timer_nodes`（约 4781–4812）两段循环除 next 偏移（0x18 vs 0x1C）外逐字重复。
- 结构体 `ArmExtDecodedPayloadMatch`（约 301）与 `ArmExtRawPayloadMatch`（约 390）字段完全相同；`arm_exec_addr`（约 918）是恒等函数。

### M5. 性能隐患

- `arm_ext_current_pack_matches_staged_file`（约 1121–1168）每次调用把整个 MRP 读入内存再解 gzip 比对；`arm_ext_bytes_contain` 对 ≥1KB payload 在整个已安装 MRP 上 O(n·m) 扫描。这些发生在模块注册路径上，大 MRP 下会出现秒级卡顿。

---

## 六、建议的处理优先级

1. **B1（屏幕回拷越界）**：真实游戏放大逻辑画布即触发的内存破坏，优先修，用宿主缓冲真实容量夹断，三处重复逻辑合并成一个辅助函数一起改。
2. **B2（NULL 判空缺失）**：引入 `arm_ptr_span` 带长度校验的变体，统一替换裸 `arm_ptr` + memcpy/strcpy 模式；顺带把 B3（arm_alloc 溢出）在同一处加固。
3. **B4/B6（长度/失败处理一致性）**：小范围防御性修复。
4. **D1/D2（死代码 + 常驻诊断）**：删除或收敛到统一 diag 开关，降低噪声。
5. **第四节应用指纹**：逐函数标注 SDK 形状 vs 单应用指纹，指纹类迁往 `app_compat_*.c`。这是与项目规范冲突最深的债务，宜在触碰相关区域时逐步偿还。
6. **M1–M5（拆分/常量化）**：结合上述修复，把 watch 子系统（本就标"修复后删除"）、MRP 缓存、屏幕脏区、compact timer 探测、hook_table case 分发拆到独立 `.c`，`arm_ext_internal.h` 已存在，拆分成本可控。
