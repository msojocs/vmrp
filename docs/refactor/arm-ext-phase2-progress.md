# Phase 2 文件拆分 —— 执行记录(第 1 批)

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第五节

## 基础设施 ✅

- `src/arm_ext/` 拆分目录;`src/include/arm_ext_priv.h` 跨单元内部 API
  (对外公开接口仍只有 arm_ext_executor.h;函数抽出时若被其它单元调用,
  在 priv 头登记并去 static,单元私有的保持 static 不进头)。
- CMake:`VMRP_ARM_EXT_SOURCES`(vmrp/wasm/shared 三目标统一引用)+
  `VMRP_ARM_EXT_SPLIT_SOURCES`(vmrp-unit 链接已抽出单元)。
- 微工具上收 priv 头 static inline:`align4`/`reg_read32`/`arm_ext_addr_range_mapped`。

## 已抽出单元(每单元后过 vmrp-unit + 全量 e2e,均 27/27)

| 单元 | 行数 | 内容 |
|---|---|---|
| `aex_support.c` | 114 | bytes_contain、guest memcpy/strncpy 重叠语义、路径比较、diag 字节预览、5 个诊断开关缓存访问器 |
| `aex_pack.c` | 182 | parse_mrp_cache、mrp_cache_find/free、mrp_vfd_open/get、路径 basename 助手 |
| `aex_mem.c` | 988 | LG_mem 池 first-fit/bump 后备/app_mem 统一入口、compact 堆 cut_range/sanitize/保护集收集、timer 节点收集器、不变量检查器 |
| `aex_screen.c` | 1618 | row-span 脏区/覆盖区、present 分段提交、DispUp、draw-bitmap 镜像、screen context(enter/leave/push/pop)、stride/copy_to_host;**diag dump 系列随物理区间迁入,待后续归位 aex_diag** |

executor 主体:10261 → **7324 行**。

## 第 2 批(同日续)

| 单元 | 行数 | 内容 |
|---|---|---|
| `aex_guard.c` | 175 | Thumb 解码助手(popcount/read_thumb16/blx/pop 判定)、table return 守卫(note/consume/is_stale/block) |
| `aex_exec.c` | 361 | set_arm_mode/arg_read、trace_pc/dump_pc_ring/hook_intr、run_arm(_with_sp)、restore_ext_r9、arm_str/format_arm |

executor 主体:7324 → **6808 行**;`reg_write32` 上收 priv 内联。
第 2 批全量 e2e 27/27 ✓,vmrp-unit 47 checks ✓。

## 事故与教训(已恢复,无损失)

屏幕区第一次切割把 `capture_timer_dispatches` 的**前向声明**误当定义锚点
(z1<z0),导致区间被复制而非切出。凭借几何推导(行数守恒 8956 + 重复段
定位)精确恢复,后以"锚点必须带 `") {"`/`" {"` 后缀 + 唯一性断言 + 区间
内容抽查"的脚本规范重做。**拆分脚本规范:所有切割锚点用定义形态并
assert count==1,切割后 assert 行数/内容。**

## 收尾批(同日续):timer/diag/module 三单元 ✅

| 单元 | 行数 | 内容 |
|---|---|---|
| `aex_timer.c` | 336 | frame/compact/wrapper timer 节点与队列校验(foreground_child_has_*_timer_queue、wrapper_has_timer_queue、dispatch_is_busy 等) |
| `aex_diag.c` | 334 | diag_dump_* 从 aex_screen 归位 + diag_fd_* + diag_owner_for_lr |
| `aex_module.c` | 2356 | nested 模块注册/retire/drop/restore、pack alias、find_wrapper_*/child_has_* 模式扫描、Thumb LDR literal 解码 |

- `arm_alloc`/`arm_ext_meta_alloc` 归位 aex_mem.c;screen 维度 hook
  (hook_screen_dim_read/write)经 priv 头跨单元登记。
- executor 主体:5736 → **2406 行**(生命周期 load/call/dispatch/unload、
  hook_table 序言、hook_invalid/low_zero 等 Unicorn hook 留守,
  aex_lifecycle 单独成文件的必要性消失,不再拆)。
- 布局终态:executor 2406 行 + src/arm_ext/ 十单元 8981 行。

### 验证(收尾批,与 Phase 3 收尾同一提交)

构建零告警(非 mythroad)→ vmrp-unit 69 checks → 全量 e2e **29/29**
(golden 帧逐字节断言含在用例内)。

## 验证纪律(沿用)

每单元抽取 = 一个提交粒度:构建零告警(非 mythroad)→ vmrp-unit 47 checks
→ 全量 e2e 27/27 逐字节一致。本批四轮全量 e2e 全绿;纯移动改动未跑
sanitizer(无语义变化),下批结束后补一轮。
