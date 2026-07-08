# arm_ext 启发式指纹分级清单

日期:2026-07-08
对应计划:docs/arm-ext-executor-refactor-plan.md §七 P4.2
依据材料:docs/arm_ext_executor-issues.md §四、docs/arm-ext-abi.md

---

## 一、分级标准

| 分级 | 判定规则 |
|---|---|
| **SDK形状** | 最终动作由运行时数据结构校验把关(magic 0x79ABBCCF 验证 / 合法地址范围过滤),误匹配即静默跳过;字节匹配仅用于定位,不直接决定行为。处置:保留,建议改造为"宽松定位+结构确认"两段式。 |
| **灰区** | 字节模式含通配或多特征锚定,结合运行时偏移提取;无独立 magic 校验但有地址范围约束;无法单凭字节形状判断是 SDK 形状还是单应用指纹。处置:两段式改造(宽松定位+结构确认),补充告警日志。 |
| **单应用指纹** | 零通配或极少通配的整段字节匹配;注释点名具体应用/映像地址;匹配失败直接丧失某项保护,无降级机制。处置:迁往 app_compat 机制或改写为运行时数据结构解析。 |

---

## 二、函数分级清单

共 28 条记录。

| 函数名(当前文件) | 匹配对象 | 最终判据 | 失败行为 | 分级 | 处置建议 |
|---|---|---|---|---|---|
| `find_wrapper_timer_dispatch`<br>(arm_ext_executor.c:4045) | SDK wrapper 定时器队列消费函数(code=2 分发器);注释点名 gxdzc cfunction.ext 0xE83A80、gghjt/gwkdl 19428 字节 wrapper | 32 字节模式(i==0 和 i==16 通配);命中后再解码 LDR literal 提取 `compact_sched_off`,要求值在 `[0x80, 0x1000)` 且 4 字节对齐——有地址范围约束,但无 magic 校验 | 静默跳过;`wrapper_timer_dispatch_addr=0`,wrapper 定时器通路整体失效 | **灰区** | 两段式改造:定位阶段保持现有通配模式,提取阶段补充对 scheduler_off 所指运行时节点的 magic 校验(0x79ABBCCF);提取结果写入 WrapperProfile;失败时输出告警日志 |
| `find_wrapper_timer_dispatch` — chain_thunk_pat 子段<br>(arm_ext_executor.c:4045) | gghjt/gwkdl 特定 cfunction.ext 的链式递减 thunk(注释点名 0xE83B50/0xE8261C/0xE83590,19428 字节) | 10 字节零通配精确匹配 | 静默跳过;`chain_thunk_addr` 不填,相关挂钩不注册 | **单应用指纹** | 迁往 app_compat 机制;或改为检测 extChunk[0x34] 递减行为的运行时判据 |
| `find_wrapper_timer_dispatch` — pat2 子段<br>(arm_ext_executor.c:4045) | 单节点 resume 回调(诊断用);结构确认依赖 EXT_CHUNK_MAGIC(0x7FD854EB) | 20 字节模式(1 字节通配);命中后读 literal pool 验 EXT_CHUNK_MAGIC——有 magic 校验 | 静默跳过;仅诊断路径,不影响运行时行为 | **SDK形状** | 保留;失败时补充 DIAG 级日志 |
| `find_wrapper_compact_heap_free_return`<br>(arm_ext_executor.c:1730) | SDK compact mr_free() pop/return 指令;提取 R9 相对 ctrl_off | 17 个固定半字精确校验(`off+0x02` 至 `off+0x8E`)+ LDR literal 解码 ctrl_off(要求 `[0x80,0x1000)` 且对齐);无 magic 校验 | 静默跳过;`wrapper_compact_free_return_addr=0`,compact 堆 mr_free hook 整体不挂 | **单应用指纹** | 17 半字整段匹配脆弱性极高;建议改写:通过 compact heap ctrl 运行时结构(free-list 链表头有效性)定位返回点,消除逐字节比对;迁移前至少补 P0.2 单元测试资产 |
| `patch_wrapper_stack_size`<br>(arm_ext_executor.c:4150) | Wrapper 启动代码中 `add rN,r0,#0x2000/0x3000`(ARM32 字,非 Thumb);4 个精确匹配词表 | 4 个 32 位 ARM 字零通配精确匹配,扫描前 min(code_len,0x200) 字节 | 静默跳过;stack size 不修正,潜在栈溢出 | **单应用指纹** | 4 个词表覆盖当前已知变体;建议在 P4.1 WrapperProfile 中记录"已找到 / 未找到"并补告警;长期可改为解析 ARM 立即数编码动态提取栈大小 |
| `arm_ext_child_has_compact_timer_walker`<br>(arm_ext_executor.c:1701) | 子模块 compact 定时器 walker 函数形状;issues doc 明确评估为"单应用指纹" | 32 字节模式(i==0 和 i==16 通配),余 30 字节零通配;无运行时结构校验 | 静默返回 0;compact 定时器保护对该子模块全部失效 | **单应用指纹** | issues doc 已明确;迁往 app_compat;或改写:以运行时 compact 调度器偏移(已由 find_wrapper_timer_dispatch 提取)直接定位,跳过代码形状探测 |
| `arm_ext_child_has_compact_r9_state_list`<br>(arm_ext_executor.c:1648) | compact 私有子模块 0x4C 字节节点状态链表解析器;注释点名 verdload.ext 0x2CBCFC、brwmain 0x2CF5CA | 0xB5F0 锚定 + 768 字节窗口内 5 项特征(含 `0x204C` 分配大小、`node+0x48` next 引用计数 ≥5);无 magic 校验 | 静默返回 0;short_pack_alias 第 4 级跳过 | **单应用指纹** | 注释已点名两个具体映像地址;建议迁往 app_compat,记录 verdload/brwmain 指纹;同时评估是否可通过 R9 状态列表运行时结构(节点 next 字段有效性)替代代码扫描【待核实:能否从运行时确认 0x4C 节点布局】 |
| `arm_ext_child_reads_record100_to_compact_r9_buffer`<br>(arm_ext_executor.c:1782) | 子模块清零 7 字 R9 相对缓冲区后以 r1=100 调用 record-read shim;注释点名 verdload.ext 0x2CA58A/0x2CD810 | 双锚定(`0xB530 0xB083`)+ 96 字节窗口内两个固定指令序列,含 `0x2164`(movs r1,#100 即 record[100] 的直接证据) | 静默返回 0;short_pack_alias 第 3 级跳过 | **单应用指纹** | 注释直接引用两个具体地址;迁往 app_compat;或改写:在运行时拦截 record[100] 读调用时动态检测目标缓冲区大小 |
| `arm_ext_child_needs_short_pack_alias` — Level 1(栈帧模式)<br>(arm_ext_executor.c:1827) | 含 32 字节 pack 名缓冲区的子模块;注释点名 frame.ext、"older DOTA frame.ext A801 spelling" | `0xB5FF 0xB089` 锚定 + 96 字节窗口内 4 个子变体 + STR 输出校验;子变体间为 OR 关系 | 静默返回 0;short_pack_alias 不安装 | **灰区** | 多变体 OR 覆盖提升容错,但无运行时结构校验;建议在 Level 1 命中后补运行时检查(如验证目标栈地址在有效 RW 范围内);补告警日志 |
| `arm_ext_child_needs_short_pack_alias` — Level 2(R9 全局拷贝模式)<br>(arm_ext_executor.c:1882) | 私有子模块将 table[100] 拷贝到 32 字节 R9 相对全局缓冲区 | `0xB5B0` 锚定 + 72 字节窗口内 3 项特征序列(clear32/pack_len_call/copy_len_call) | 静默返回 0;short_pack_alias 不安装 | **灰区** | 同 Level 1;建议补运行时 R9 缓冲区地址校验;补告警日志 |
| `arm_ext_child_needs_short_pack_alias` — Level 3<br>(arm_ext_executor.c:1922) | 代理 `arm_ext_child_reads_record100_to_compact_r9_buffer` | 同 #6 | 静默返回 0 | **单应用指纹** | 同 #6 处置 |
| `arm_ext_child_needs_short_pack_alias` — Level 4<br>(arm_ext_executor.c:1925) | 代理 `arm_ext_child_has_compact_r9_state_list` | 同 #5 | 静默返回 0 | **单应用指纹** | 同 #5 处置 |
| `arm_ext_frame_timer_node_is_valid`<br>(arm_ext_executor.c:2573) | frame.ext 定时器节点;注释点名 frame.ext 0x2C9486/0x2C9538,R9+0x94 链 | 运行时:node+0x1C 已映射 AND `*(node+0x00)==0x79ABBCCF` | 静默返回 0 | **SDK形状** | 保留;magic 校验是充分的结构确认;可将常量 `0x79ABBCCF` 提取为具名宏(已有 ARM_EXT_COMPACT_TIMER_MAGIC) |
| `arm_ext_foreground_child_has_frame_timer_queue`<br>(arm_ext_executor.c:2586) | DOTA frame.ext 定时器队列,R9+0x94/0x9C/0xA0;注释点名 DOTA 0x2C96A0 | 纯运行时:foreground 标志 + 偏移映射 + magic 节点校验(0x79ABBCCF @+0) | 静默返回 0 | **SDK形状** | 保留;固定偏移 0x94/0x9C/0xA0 建议提升为具名常量 |
| `arm_ext_compact_timer_node_is_valid`<br>(arm_ext_executor.c:2605) | compact 子模块定时器节点(仅映射检查,无 magic) | 运行时:node+0x00 和 +0x1C 已映射;**无 magic 校验** | 静默返回 0 | **灰区** | 与 aex_mem.c 的 `arm_ext_compact_timer_magic_node_is_valid` 不一致:后者要求 magic;建议统一为带 magic 校验的版本,评估是否存在无 magic 的合法节点【待核实】 |
| `arm_ext_compact_timer_queue_at`<br>(arm_ext_executor.c:2610) | compact 子模块调度器(R9+0xC0 或 R9+0x248) | 运行时:rw+off+0x10 映射 + 节点映射校验(通过 #10 的无 magic 版本) | 静默返回 0 | **灰区** | 同 #10;升级节点校验为带 magic 版本后本函数随之升级为 SDK形状 |
| `arm_ext_foreground_child_has_compact_timer_queue`<br>(arm_ext_executor.c:2630) | 前台子模块是否有活跃 compact 队列 | 两段式:先调用 `arm_ext_child_has_compact_timer_walker`(单应用指纹代码探测) + 再探运行时 {0xC0, 0x248} | 静默返回 0;代码探测失败则整体跳过 | **灰区** | 两段式架构正确;但第一段是单应用指纹(见 #4);建议将第一段替换为运行时 compact_sched_off 有效性检查(已由 find_wrapper_timer_dispatch 提取),去掉代码扫描依赖 |
| `arm_ext_wrapper_timer_node_is_valid`<br>(arm_ext_executor.c:2678) | 旧式 wrapper 定时器节点;注释点名 optwar cfunction.ext 0xE84920 | 运行时:node+0x20 映射 AND `*(node+0x08)==0x79ABBCCF` | 静默返回 0 | **SDK形状** | 保留;magic 偏移 @+8 与 compact 版本 @+0 不同,建议文档化两种 timer node ABI 布局差异 |
| `arm_ext_wrapper_compact_timer_node_is_valid`<br>(arm_ext_executor.c:2690) | compact wrapper 定时器节点 | 运行时:node+0x1C 映射 AND `*(node+0x00)==0x79ABBCCF` | 静默返回 0 | **SDK形状** | 保留 |
| `arm_ext_wrapper_compact_timer_queue_at`<br>(arm_ext_executor.c:2701) | compact wrapper 调度器,偏移来自 find_wrapper_timer_dispatch 提取 | 运行时:rw+off+0x14 映射 + magic 节点校验(@+0) | 静默返回 0 | **SDK形状** | 保留;调度器偏移为动态提取值,不硬编码 |
| `arm_ext_wrapper_has_timer_queue`<br>(arm_ext_executor.c:2722) | Wrapper 是否有任意形式的定时器队列 | 两段式:先 compact sched(magic @+0)+ fallback 旧式固定偏移 0x3C8/0x3D8(magic @+8) | 静默返回 0 | **SDK形状** | 保留;fallback 偏移 0x3C8/0x3D8 建议提升为具名常量并在 WrapperProfile 中记录来源 |
| `arm_ext_wrapper_dispatch_is_busy`<br>(arm_ext_executor.c:2650) | 旧式 wrapper busy 位(wrapper_rw+4) | 运行时:仅在 `wrapper_timer_dispatch_addr!=0` 且 `compact_sched_off==0` 时检查 rw+4 | 静默返回 0 | **灰区** | 注释已说明该偏移被 19KB chain-walker wrapper 的无关数据复用;保护逻辑已有 compact sched 前置门控;建议将 busy-bit 语义写入 WrapperProfile 说明,明确何时有效 |
| `arm_ext_compact_timer_magic_node_is_valid`<br>(aex_mem.c:491) | compact 节点保护用校验(含 magic) | 运行时:0x20 字节已映射 AND `*(node+0x00)==0x79ABBCCF` | 静默返回 0 | **SDK形状** | 保留;与 executor.c 中无 magic 版本(#10)存在不一致,建议合并为单一函数 |
| `arm_ext_collect_primary_compact_timer_nodes`<br>(aex_mem.c:568) | 主 game.ext compact 定时器节点(R9+0x84/0x80);注释点名 gzwdzjs | 运行时:固定偏移 {0x84, 0x80} + `read_game_timer_head` 地址范围过滤 + magic 校验 | 静默跳过,无保护 | **灰区** | 固定偏移 0x84/0x80 无代码形状支撑;建议补 WrapperProfile 字段记录 game 调度器偏移(类比 compact_sched_off),运行期动态填入;注释已点名 gzwdzjs 作为观测依据 |
| `arm_ext_collect_wrapper_compact_timer_nodes`<br>(aex_mem.c:596) | Wrapper compact 定时器节点,偏移来自 profile | 运行时:使用 `wrapper_compact_timer_scheduler_off`(动态提取) + magic | 静默跳过 | **SDK形状** | 保留;依赖 find_wrapper_timer_dispatch 的动态提取结果,已是两段式 |
| `arm_ext_collect_active_compact_timer_nodes`<br>(aex_mem.c:612) | 活跃前台子模块 compact 节点(固定偏移 {0xC0, 0x248}) | 两段式:代码探测(#4 单应用指纹) + 运行时 magic | 静默跳过 | **灰区** | 同 #16;代码探测段替换为运行时 compact_sched_off 检查后可升级为 SDK形状 |
| `read_game_timer_head`<br>(arm_ext_executor.c:3199) | game.ext 定时器队列头(R9+0x8C,fallback 0x88) | 运行时:非自指针(非 EXT_TABLE_ADDR 范围内)地址过滤;无 magic 校验 | 静默返回 0 | **灰区** | 地址范围过滤有一定防误判作用;建议在返回值上追加 magic 校验【待核实:game.ext 定时器节点是否总有 0x79ABBCCF】 |
| `arm_ext_discover_private_child_rw_shift`<br>(arm_ext_executor.c:1444) | 私有子模块 bridge-copy RW 布局(record→rw 偏移对);注释关联 gghjt cfunction.ext | LDR/STR 配对序列匹配 `std_bridge_copy_rw_runs[]` 表(2 条 run,17+1 对);提取 shift 值;无 magic | 静默返回 0;bridge repair 不执行 | **单应用指纹** | run 表硬编码了具体的 record 偏移(0x68, 0x0C..0x4C)和 RW 偏移(0x16C, 0x170..0x1B0);建议迁往 app_compat 并由运行时 RW 布局解析替代【待核实:是否有第二个 wrapper 共享该布局】 |
| `arm_ext_private_child_has_safe_rw_bridge_layout`<br>(arm_ext_executor.c:1562) | 私有子模块 bridge layout 安全性(目标在高 RW 区) | 调用 #23 + shift 合法性(dst >= 0x80) | 静默返回 0 | **单应用指纹** | 依赖 #23;同处置 |
| `arm_ext_mirror_read_file_to_adjacent_slot`<br>(arm_ext_executor.c:2341) | 特定 wrapper 解析器的栈 `{fileLen,dataPtr}` ABI;注释点名 wrapper 调用点 0xE80B82 | R9==wrapper_rw + len 匹配 + `*(len_slot+8)==0x00000B82`(返回地址精确校验) | 静默返回 0;readFile 数据指针不镜像 | **单应用指纹** | 0x00000B82 是具体映像地址;建议迁往 app_compat;或改写为通用的"wrapper 调用 readFile 时栈帧 dataPtr 提取"逻辑,消除具体地址依赖 |

---

## 三、分级统计

| 分级 | 数量 | 函数/条目 |
|---|---|---|
| **SDK形状** | 10 | `find_wrapper_timer_dispatch`(pat2段)、`arm_ext_frame_timer_node_is_valid`、`arm_ext_foreground_child_has_frame_timer_queue`、`arm_ext_wrapper_timer_node_is_valid`、`arm_ext_wrapper_compact_timer_node_is_valid`、`arm_ext_wrapper_compact_timer_queue_at`、`arm_ext_wrapper_has_timer_queue`、`arm_ext_compact_timer_magic_node_is_valid`(aex_mem)、`arm_ext_collect_wrapper_compact_timer_nodes` |
| **灰区** | 9 | `find_wrapper_timer_dispatch`(主段)、`arm_ext_child_needs_short_pack_alias`(Level 1)、`arm_ext_child_needs_short_pack_alias`(Level 2)、`arm_ext_compact_timer_node_is_valid`、`arm_ext_compact_timer_queue_at`、`arm_ext_foreground_child_has_compact_timer_queue`、`arm_ext_wrapper_dispatch_is_busy`、`arm_ext_collect_primary_compact_timer_nodes`、`arm_ext_collect_active_compact_timer_nodes`、`read_game_timer_head` |
| **单应用指纹** | 9 | `find_wrapper_timer_dispatch`(chain_thunk_pat 段)、`find_wrapper_compact_heap_free_return`、`patch_wrapper_stack_size`、`arm_ext_child_has_compact_timer_walker`、`arm_ext_child_has_compact_r9_state_list`、`arm_ext_child_reads_record100_to_compact_r9_buffer`、`arm_ext_child_needs_short_pack_alias`(Level 3/4)、`arm_ext_discover_private_child_rw_shift`、`arm_ext_private_child_has_safe_rw_bridge_layout`、`arm_ext_mirror_read_file_to_adjacent_slot` |

> 注:部分函数(如 `find_wrapper_timer_dispatch`)内含多个独立子段,各子段分级不同,拆开计算后实际条目数为 28。

---

## 四、显式 fallback 链评审

### 4.1 `short_pack_alias` 四级回退
**所在文件:** arm_ext_executor.c:606  
**调用路径:** `arm_ext_apply_short_pack_alias_for_private_child` → `arm_ext_child_needs_short_pack_alias`(Level 1~4 见上节) → `arm_ext_short_pack_alias_for_host_path`(5 级路径)

**各级依据:**

| 级别 | 逻辑 | 依据 |
|---|---|---|
| 级 1 | `host_path == m->pack_host_path` 且 alias 长度 < 32:返回已注册 alias | **有真机语义依据**:已缓存的 pack 路径直接命中,避免重复注册 |
| 级 2 | 路径非绝对路径且长度 < 32:原样返回 | **有真机语义依据**:相对短路径本身即为 pack 名,不需缩短 |
| 级 3 | `strstr(path, "mythroad/")` 命中且后缀 < 32:返回 `"mythroad/..."` 后缀 | **有真机语义依据**:DSM 层将 `mythroad/...` 视为根路径,注释明确说明 |
| 级 4 | `arm_ext_basename(path)` 长度 < 32:返回文件名 | **依据待补**:basename 可能截断有区分意义的路径前缀,导致不同 pack 映射到相同 alias;建议补充说明何时 basename 唯一性成立 |
| 级 5(兜底) | 生成合成 alias(`~p0`, `~p1`, ...):注册映射 | **有真机语义依据**:合成 alias 保证唯一性,为规范兜底而非脆弱启发式 |

**整体评价:** 前 3 级均有明确语义依据,第 4 级(basename)存在歧义风险,建议在注释中补充"basename 唯一性假设成立的条件"(如:一个游戏包内不会有两个同名但路径不同的子 ext)。

---

### 4.2 `hook_low_zero` return-to-LR
**所在文件:** arm_ext_executor.c:3285  
**注册方式:** `arm_ext_load` 经 `uc_hook_add` 挂载,覆盖地址 `[0, EXT_LOW_TABLE_SIZE-1]`

**两个场景:**

| 场景 | 触发条件 | 行为 | 依据 |
|---|---|---|---|
| a | `address < EXT_TABLE_COUNT * 4`(未解析的低表槽) | 返回 r0=0(NULL/FAILED)并 return-to-LR;有 diag 日志 | **有真机语义依据**:未注册的 API slot 应返回失败而非崩溃,return-to-LR 是正确的 Thumb 调用约定 |
| b | `address >= EXT_TABLE_COUNT * 4`(越界跳转) | 不修改 r0;LR 非零则 return-to-LR,否则 uc_emu_stop | **依据待补**:场景 b 的"跳转到 LR"在 LR 值可能已损坏时存在掩盖真实错误的风险;建议在 DIAG 模式下改为强制 stop 并输出完整寄存器快照 |

**整体评价:** 场景 a 有明确语义;场景 b 的静默 return-to-LR 可能将越界跳转错误转化为后续逻辑错误,建议区分处理。

---

### 4.3 case 40/42 disk→MRP 缓存回退
**所在文件:** src/arm_ext/aex_table.c:703(case 40)、765(case 42)

| 接口 | 回退条件 | 行为 | 依据 |
|---|---|---|---|
| case 40 (`mr_open`) | `ret==0`(磁盘未找到) 且 `mrp_cache_count>0` | 在 MRP 缓存中查找虚拟 fd | **有真机语义依据**:MRP 包内嵌资源不落盘,cache 作为虚拟文件系统兜底语义明确 |
| case 42 (`mr_info`) | `ret!=MRP_IS_FILE` 且 `mrp_cache_count>0` | 在缓存中确认文件存在返回 `MRP_IS_FILE` | **有真机语义依据**:同上 |
| case 46 (`mr_getLen`) | ~~曾有 ret<0 时查 MRP 缓存~~ | **已删除**:原逻辑因 `ret` 为 `uint32_t` 恒不成立(死代码,-Wtype-limits 证实),已按"禁止兜底逻辑"规范移除 | — |

**整体评价:** case 40/42 回退有真机语义支撑且行为可预期;case 46 死代码已正确清理。

---

### 4.4 case 125 (readFile) 多级包上下文解析
**所在文件:** arm_ext_executor.c:3666  
**性质:** 包上下文分层解析,不完全等同于 disk→MRP 回退

| 层级 | 条件 | 行为 |
|---|---|---|
| 子模块作用域 | 调用方为已注册私有子模块 | 使用子模块 pack alias 解析路径 |
| RAM pack(`$` 前缀) | 路径以 `$` 开头,ramp/raml 有效 | 从内存 RAM pack 读取 |
| 空 pack | pack 路径为空 | 直接读取裸文件 |
| 宿主 pack | 其余情况 | 使用宿主 pack 路径解析 |

最终对 wrapper 解析器调用 `arm_ext_mirror_read_file_to_adjacent_slot`(见 #25,单应用指纹)。

**整体评价:** 分层解析有明确语义依据;`arm_ext_mirror_read_file_to_adjacent_slot` 子步骤是唯一的单应用指纹依赖,处置建议见上节。

---

## 五、待核实项汇总

共 **4** 处标注【待核实】:

1. **`arm_ext_child_has_compact_r9_state_list`(#5)**:能否通过 R9 状态列表运行时结构(节点 next 字段有效性)替代代码扫描——需要对 verdload/brwmain 实际运行时内存布局进行确认。

2. **`arm_ext_compact_timer_node_is_valid`(#10) vs `arm_ext_compact_timer_magic_node_is_valid`(#18)**:是否存在合法的无 magic 的 compact 定时器节点——如不存在则两函数应统一,如存在则需记录对应 SDK 版本。

3. **`read_game_timer_head`(#22)**:game.ext 定时器节点是否总携带 0x79ABBCCF magic——若是则返回值可追加 magic 校验;若存在不带 magic 的合法 game timer 节点则需保留现状。

4. **`arm_ext_discover_private_child_rw_shift`(#23)**:是否有第二个 wrapper 共享 `std_bridge_copy_rw_runs[]` 所描述的 record→RW 偏移布局——若仅 gghjt cfunction.ext 一个则可直接迁 app_compat,若有多个则需先补充测试覆盖再评估。
