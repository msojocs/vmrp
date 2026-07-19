# arm_ext_executor.c 代码优化方案

日期:2026-07-08
前置文档:`docs/arm_ext_executor-issues.md`(2026-07-06 问题清单,B1-B6/D1-D2/指纹清单仍然有效)
本方案基于三路并行分析:子系统边界分析、启发式/脆弱点盘点、重构安全网评估。

---

## 一、现状诊断(量化)

| 指标 | 数值 | 说明 |
|---|---|---|
| 文件行数 | 10261 | 单翻译单元,约 16850 行 src/*.c 总量的 61% |
| 函数数 | ~319 | 其中 `hook_table` 1496 行(100 个 case)、`arm_ext_dispatch_pending_sms_result` 567 行、`arm_ext_call` ~550 行 |
| ArmExtModule 字段 | ~146 | god struct,所有子系统状态混在一起 |
| 近 3 个月提交 | 77 次 | 全项目的修复热点集中在这一个文件 |
| 应用字节码指纹函数 | 30+ | 与"禁止应用特定代码"规范冲突(见 issues doc 第四节) |
| 裸魔数偏移 | 60+ 处 | extChunk/P/RW/timer 节点偏移散落重复,无结构体定义 |
| getenv 调用 | 72 处 | 9 个环境变量开关,热路径未缓存,无文档 |
| C 层单元测试 | 0 | 安全网只有 27 条像素断言 e2e(13 个 MRP 应用) |
| 编译安全网 | 仅 -Wall | 无 -Wextra/-Werror;ASan/UBSan 有开关但从未在 e2e 下跑过 |

**核心病灶**(按因果排序):

1. **知识没有沉淀为结构**:wrapper ABI(extChunk 布局、P 结构、compact 堆 ctrl、timer 节点)散落为几十处裸偏移和逐字节模式匹配,每修一个 bug 就新增一处,谁也不敢动旧的。
2. **"先污染后清理"模式**:sanitize/retire/drop/restore/guard 这类运行时修正是对分配器/生命周期源头问题的补丁摞补丁,4 处独立收集 timer 节点、3 个纠缠的模块生命周期函数,互相之间无不变量约束,修 A 坏 B 的结构性风险就来自这里。
3. **单文件+超大函数**让上述两点无法被局部化审查:hook_table 100 个 case 之间有隐式状态依赖,arm_ext_call 与 arm_ext_call_dispatch 存在逐字重复块。
4. **模式匹配失败即静默降级**:find_wrapper_* 匹配不上时相关保护整体不生效,无任何日志,新 SDK 变体上表现为"莫名其妙的另一个 bug"。

---

## 二、总体策略

> 原则:**每一步都是行为不变的机械重构,或是有独立验证的 bug 修复,二者绝不混在同一提交**。模拟器是确定性的——纯重构后 27 条 e2e 的像素输出应逐字节一致,任何像素差异都视为回归而非"容差范围内的等价"。

分五个阶段,前两阶段是后三阶段的前置条件。每阶段结束跑全量 `pnpm test:e2e` + ASan 构建验证。

---

## 三、Phase 0 —— 安全网(先于一切重构)

**P0.1 编译加固**
- CMake 增加 `SKYENGINE_STRICT_WARNINGS` 选项:`-Wextra -Wshadow -Wstrict-prototypes`(先修掉存量告警,再对 src/*.c 开 `-Werror`;`-Wconversion` 存量太大可缓)。
- 建立 ASan+UBSan 构建目录(`-DSANITIZE=address,undefined` 已存在,从未使用),全量 e2e 在 sanitizer 下跑通一次并记录基线——**这一步本身就可能直接抓到 issues doc 里 B1(屏幕回拷越界)**。

**P0.2 C 层单元测试基座**
- 新建 `test/unit/`(CTest 或单文件 main),优先覆盖纯函数:
  - `arm_alloc` 溢出边界(B3 的回归测试);
  - `arm_ext_compact_heap_cut_range` 在合成 free-list 上的裁剪(左/右/两侧/整段吞并四种拓扑);
  - `find_wrapper_*` 系列:把已知 wrapper(gxdzc/gghjt/verdload cfunction.ext)的相关字节段截取为测试资产,断言"能识别";再用扰动字节断言"不误报"。
- 这些函数当前是 static——拆分文件(Phase 2)前先通过 `#include` 整文件的 test harness 或临时导出宏解决,拆分后自然变成可链接单元。

**P0.3 不变量检查器**
- 新增 `arm_ext_verify_invariants(m)`(仅 debug/env 门控):nested_module 区间互不重叠且不与 free-list 相交、active/timer owner 必须指向已注册模块、heap_top 界内、screen 尺寸与宿主容量一致。
- 在 `arm_ext_call`/`arm_ext_call_dispatch` 出入口调用,e2e 期间任何违反直接打印并可选 abort。**这是把"修 A 坏 B"从像素级症状提前到状态级报警的关键装置。**

**P0.4 ABI 知识文档化**(与 P0.3 同步产出)
- `docs/arm-ext-abi.md`:内存映射全景(EXT_BASE/TABLE/HEAP/STACK/CODE/PLATFORM 区)、extChunk 布局表、P 结构、compact 堆 ctrl 布局、timer 节点两种格式(magic 0x79ABBCCF 的 +0/+8 变体)、RW 调度器偏移矩阵(0xC0/0x248/0x3C8...)及各自的证据来源(SDK ABI / 某 wrapper 反汇编 / 纯观测)。
- 素材已存在于代码注释和 omx_wiki/docs 的 40+ 篇 case 记录里,做的是汇总而非新逆向。

验收:strict 构建零告警;sanitizer e2e 全绿(或修完暴露的 bug 后全绿);单元测试框架可运行;不变量检查器在全量 e2e 下零报警。

---

## 四、Phase 1 —— Bug 修复与低风险清理(可与 Phase 0 并行推进)

按 issues doc 优先级执行,每项独立提交+独立验证:

1. **B1** 屏幕回拷越界:三处重复逻辑合并为一个"按宿主容量夹断"的辅助函数(gtcm 480x320 逻辑画布用例是现成回归测试)。
2. **B2** `arm_ptr` NULL 解引用:引入 `arm_ptr_span(m, addr, need)`(判空+长度校验一次完成),替换 hook_table case 2/3-9/34/35/44/80、`write_game_timer_head` 等裸用点。
3. **B3/B4/B5/B6**:arm_alloc 溢出拒绝、cache_sync 用 copy_len、char_bitmap 尺寸断言、分配失败路径统一(bump_reuse 先 track 后摘链)。
4. **D1 死代码删除**:depth_patched 恒零块、guest_mem_malloc 不可达推进、leave_screen_context 恒真分支、dispup_rect 无意义三目。
5. **D2 临时诊断收敛**:watch 子系统(hook_watch_write/sentinel,注释自称"修复后删除")与 `arm_ext_watch_module_event` 调用点——对应 bug 已修复的直接删除;仍有诊断价值的收编进统一 diag 层(见下)。
6. **诊断层统一**:`arm_ext_load` 时把 9 个 getenv 一次性解析进 `m->diag_flags` 位域;新增 `AEX_DIAG(m, flag, fmt, ...)` 宏替换 358 处裸 `if(getenv(...)) printf`。顺带在 docs 里给出环境变量清单。
7. **魔数常量化**:在 `arm_ext_internal.h` 定义 `struct` 或偏移宏族:`EXTCHUNK_OFF_HELPER(0x08)/RW(0x14)/SUSPEND(0x34)...`、`P_OFF_ER_RW(0)/ER_RW_LEN(4)...`、`COMPACT_CTRL_OFF_FREE_BYTES(0x0C)/FREELIST_HEAD(0x18)`,全文替换裸偏移。这是纯文本替换级重构,但让后续所有阶段的代码可读。

验收:全量 e2e 逐字节一致(B1/B2 等行为修复除外,它们需单独说明预期变化);sanitizer 报警清零。

---

## 五、Phase 2 —— 文件拆分(机械,不改行为)

目标布局(`src/arm_ext/` 子目录,预估行数):

| 文件 | 职责 | ~行数 |
|---|---|---|
| `aex_support.c` | align/路径/字节扫描等纯工具 | 300 |
| `aex_pack.c` | MRP 缓存解析、虚拟 fd(parse_mrp_cache/mrp_vfd_*) | 300 |
| `aex_mem.c` | LG_mem pool/bump/compact 堆 + cut_range/sanitize | 1300 |
| `aex_module.c` | 嵌套模块注册/retire/drop/restore、pack alias | 1000 |
| `aex_screen.c` | 屏幕快照/脏区/present/modal 快照 | 1200 |
| `aex_timer.c` | timer 队列扫描、dispatch 捕获、owner 记录 | 700 |
| `aex_guard.c` | GOT 守卫、table return 守卫、watch(若保留) | 500 |
| `aex_exec.c` | run_arm_with_sp、trace、hook_invalid | 500 |
| `aex_table.c` | hook_table 分发框架(Phase 3 再细拆) | 1600 |
| `aex_lifecycle.c` | arm_ext_load/unload/call/call_dispatch/invoke | 1300 |
| `aex_diag.c` | 诊断 dump 函数集中地 | 500 |

拆分次序(依赖由浅入深,每拆一个文件一个提交):
1. support → pack → guard(近乎无依赖);
2. exec → diag;
3. mem → screen → module(三者之间有单向依赖,按此序);
4. timer(依赖 mem/module 的查询接口);
5. table + lifecycle(最后,此时它们只剩"调用各子系统"的编排职责)。

配套规则:
- ArmExtModule 暂不拆,但按子系统加段注释(`/* ---- mem ---- */`),单一子系统字段在各文件稳定后逐步下沉为该文件的私有 struct(module 持指针)。
- 16 处前向声明的函数簇是天然的切割提示,拆分时顺带消除。
- 26 个 `extern` 全局(mr_gzInBuf、LG_mem_base 等 mythroad 符号)集中收进一个 `aex_host_iface.h`,为将来多实例化留口,不在本阶段改语义。
- 拆分期间**禁止顺手改逻辑**;diff 应当能用 "moved code" 方式审查。

验收:每步全量 e2e 像素逐字节一致;无循环 include;单文件 ≤1600 行。

---

## 六、Phase 3 —— hook_table 与 arm_ext_call 解体

1. `hook_table` 改造为分发框架:
   - 提取公共前导(参数读取 r0-r3、diag 入口日志)为 `AexTableCtx`;
   - 每个 case 变成 `static void table_NN_xxx(ArmExtModule*, AexTableCtx*)`,以函数指针表 `table_handlers[150]` 分发;
   - 按域分组落文件:`aex_table_mem.c`(0/1/2/14…)、`aex_table_file.c`(40-46/125/131…)、`aex_table_screen.c`、`aex_table_timer.c`、`aex_table_misc.c`;
   - **case 间隐式依赖**(如 case 0 写 last_alloc、后续 case 读)借此显式化为 ctx/module 字段,并在 ABI 文档中记录。
2. `arm_ext_call` 的 code==2 分支簇:把与 `arm_ext_call_dispatch` 逐字重复的"模态屏幕恢复"、"timerStart(50)+state=1"四连段提取为共享函数——issues doc M1 点名的"改一处漏一处"直接消除。
3. `arm_ext_dispatch_pending_sms_result`(567 行)按结果类型切成 3-4 个子函数。

验收:同 Phase 2(逐字节一致);函数 ≤200 行成为该目录守则(诊断 dump 类除外)。

---

## 七、Phase 4 —— 启发式治理(风险最高,收益最大)

这一阶段解决"补丁摞补丁"的根子,节奏放最慢、每项独立验证:

**P4.1 探测结果集中化:WrapperProfile**
- 新增 `struct AexWrapperProfile`:compact_heap_ctrl_off、timer_dispatch_addr、timer 节点格式(magic 偏移/next 偏移)、调度器偏移(0xC0/0x248/0x3C8...)、已命中的 pattern 清单。
- 所有 find_wrapper_*/child_has_* 探测在 load/sync 时执行一次,结果写入 profile;运行期代码只读 profile,不再各自扫描。
- **探测失败从静默降级改为一次性告警日志**(默认构建可见):"wrapper compact-heap free pattern not matched; heap sanitize disabled"。新 SDK 变体从"莫名其妙的花屏"变成一行可搜索的日志。

**P4.2 指纹分级与迁移**
- 按 issues doc 第四节的评估,给 30+ 个匹配函数逐个标注:
  - **SDK 形状**(最终判据是运行时结构校验,如 magic 0x79ABBCCF + 布局验证):保留,改造成"宽松定位+结构确认"两段式,收进 aex_module/aex_timer;
  - **单应用指纹**(零通配整段字节匹配,如 `find_wrapper_compact_heap_free_return` 17 半字、`arm_ext_child_has_compact_timer_walker` 32 字节):迁往既有 `app_compat_*.c` 的 profile 机制,或改写为对运行时数据结构的解析。每迁一个,配一条 P0.2 的单元测试(正样本字节资产 + 扰动负样本)。
- 显式 fallback 链(short_pack_alias 四级回退、hook_low_zero return-to-LR 等)逐个评审:有真机语义依据的写明依据,没有的删除并跑回归定位真正需求。

**P4.3 sanitize/生命周期收敛**
- 4 处 timer 节点收集函数合并为单一 `aex_collect_protected_ranges()`(去重,输出有序区间集),cut_range 消费者不变——2026-07-07 gzwdzjs 修复(模块映像+ER_RW 保护)已验证这套语义,合并只是消除重复。
- retire/drop/restore 三个纠缠函数重构为显式模块状态机:状态 = {pending, registered, active, retired},迁移函数唯一入口,P0.3 的不变量检查器在每次迁移后断言一致性。
- 长期方向(本方案不强制):compact 堆保护从"free 后事后裁剪"前移到"分配决策时查询保护区间",消灭"先污染后清理"——需要先有状态机与单元测试兜底再做。

验收:全量 e2e 全绿;人为破坏 pattern(hex 编辑测试 wrapper 副本)时有明确告警而非静默;`grep -c "0x79ABBCCF\|0x248\|0xC0" src/arm_ext/` 收敛到定义处。

---

## 八、Phase 5 —— 长期项(择机)

- **多实例化**:mr_gzInBuf/LG_mem_base 等 26 个全局收进 host 接口结构体,ArmExtModule 持引用(wasm 多实例、并行单测都受益)。
- **性能**:`arm_ext_current_pack_matches_staged_file` 整包读入+解压比对(M5)改为摘要缓存;模块注册路径 O(n·m) 字节扫描加索引。
- **e2e 增强**:golden PPM 资产化(当前断言是抽点像素,升级为关键帧全图 diff==0);为"删 netpay""断网"等错误路径补用例(近期多个 bug 都出自错误路径)。

---

## 九、风险控制与节奏

| 规则 | 说明 |
|---|---|
| 一步一提交 | 每个拆分/修复独立提交,提交信息写明"行为不变"或预期行为变化 |
| 全量回归 | 每提交后 `pnpm test:e2e`(~5 分钟,16 文件/27 用例);Phase 边界加跑 sanitizer 构建 |
| 逐字节像素基线 | 纯重构提交要求 27 用例像素输出与基线完全一致 |
| 禁止混合提交 | 重构与 bug 修复分离;发现新 bug 时先记 issues doc,不顺手修 |
| 回滚友好 | Phase 2/3 的移动类改动保持函数签名不变,单提交可 revert |

**工作量粗估**:Phase 0 ≈ 3-5 天;Phase 1 ≈ 3-4 天;Phase 2 ≈ 4-6 天;Phase 3 ≈ 3-4 天;Phase 4 ≈ 1-2 周(最不可压缩);合计约 4-6 周弹性推进。Phase 0/1 本身就有独立价值(修真 bug+可观测性),即使后续阶段暂停也不白做。

## 十、开始点建议

第一批三个提交(风险最低、立刻见效):
1. CMake strict-warnings 选项 + 修存量告警;
2. sanitizer 构建跑全量 e2e,处理暴露的问题(预期命中 B1/B2 一类);
3. 诊断层统一(getenv 缓存 + AEX_DIAG 宏)——它会触碰全文件但纯机械,是检验"逐字节一致"回归流程的理想演练。
