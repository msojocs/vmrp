# Phase 4 启发式治理 —— 执行记录(第 1 批)

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第七节

## P4.3 保护集收敛 ✅(第一步)

- **统一收集入口** `arm_ext_collect_protected_ranges`(aex_mem.c):四个收集器
  (primary/wrapper/active compact timer 节点 + 注册模块映像/ER_RW)一次收集,
  经新增 `arm_ext_merge_protect_ranges` 排序合并(重叠/相邻/重复区间并段)。
  语义不变(cut_range 对重复区间本就是空跑),遍历次数确定、便于诊断。
  单测覆盖 5 种拓扑(乱序/重叠/相邻/重复/吞并),vmrp-unit 升至 56 checks。
- **生命周期断言**:`drop_stale`/`retire-data-read`/`restore-dump0` 三个模块
  状态迁移函数出口接入 `arm_ext_verify_invariants`(env 门控)——
  retire/drop/restore 改状态机前先有一致性监督。

## P4.1 探测可观测化 ✅(第一步)

- wrapper 二进制模式探测结果从 TRACE 门控改为**默认可见的单行摘要**
  (每次 arm_ext_load 一行):
  `wrapper probes len=… timer_dispatch=… compact_sched_off=… compact_heap_ctrl=… compact_free_ret=… chain_walker=…`
  值为 0 即未命中。此前失配=静默降级,新 SDK 变体表现为"莫名其妙的花屏";
  现在一行日志即可定位哪个探测没命中(实测 gzwdzjs:timer_dispatch/compact
  堆命中、chain_walker 未命中,与 wrapper 类型一致)。

## P4.2 指纹分级清单 ✅

- `docs/arm-ext-fingerprint-classification.md`:28 条记录逐一分级
  (SDK 形状 10 / 灰区 10 / 单应用指纹 10,待核实 4 处),含匹配对象、
  最终判据、失败行为与处置建议;另列显式 fallback 链评估
  (short_pack_alias 五级、hook_low_zero、case 40/42 缓存回退等)。
- 清单中的重要发现:`arm_ext_compact_timer_node_is_valid`(无 magic)与
  `arm_ext_compact_timer_magic_node_is_valid`(有 magic)语义不一致,
  统一前需核实"无 magic 的合法节点"是否存在(涉 DOTA frame timer,勿盲改)。

## 验证

- vmrp-unit 56 checks ✅;全量 e2e 27/27 ✅;第一方告警 0 ✅;
  VMRP_ARM_EXT_INVARIANTS=1 下 gzwdzjs 双用例零报警 ✅。

## 待续(第 2 批起,按分级清单执行)

1. **单应用指纹迁移**(10 个):优先 `find_wrapper_compact_heap_free_return`
   (17 半字)与 `chain_thunk_pat`——迁 app_compat 或改运行时结构判据;
   每迁一个配 P0.2 字节资产单测(正样本从 fixtures 解出 wrapper 片段)。
2. **灰区两段式改造**(10 个):代码探测段替换为运行时 compact_sched_off /
   magic 校验(清单 #16/#26 已给出具体路径)。
3. **WrapperProfile 结构体**:把 wrapper_timer_dispatch_addr/
   compact_sched_off/ctrl_off/chain_walker 等散字段收进 profile 子结构
   (字段改名波及面大,单独一批做)。
4. **magic/无 magic 节点校验器统一**(待核实后)。
5. **retire/drop/restore 显式状态机**(断言已就位,重写在后)。
