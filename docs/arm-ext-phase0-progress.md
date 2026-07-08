# Phase 0 安全网 —— 执行记录

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第三节

## P0.1a 编译加固 ✅

- CMake 新增 `VMRP_STRICT_WARNINGS`(默认 ON,`-Wextra -Wshadow -Wstrict-prototypes`,
  只作用于 src/ 外层第一方源码;mythroad 子项目与 third_party 不受影响)和
  `VMRP_WERROR`(默认 OFF,冒烟验证可零错误通过)。
- 存量 63 条告警全部清零(均为行为不变修复):
  - `()`→`(void)` 原型补全:vmrp.h/bridge.h/dsm.h/main.c/vmrp.c/utils.c/network.c;
  - 未用参数 `(void)` 标注(SDL/bridge/Unicorn ABI 固定签名);
  - shadow 变量重命名(executor split_lo/hi、tail_lo/hi);
  - 符号比较显式转换(fileLib、executor 7197 位模式等价比较);
  - main.c 编辑框 fallthrough 加规范注释;
  - 死代码删除:utils.c `vmrp_wstrlen`(未引用)、executor case 46 的
    "ret<0 查 MRP 缓存"回退块(`ret` 为 uint32,-Wtype-limits 证实恒不成立,
    删除即保持现行为,且符合"禁止兜底"规范);
  - network.c guest 地址截断显式化 `(uint32_t)(uintptr_t)`。
- 未修(超出本阶段范围,已知噪声):mythroad/ 的 pointer↔int cast 告警
  (64 位下 32 位 guest ABI 固有,独立子项目)。

## P0.1b Sanitizer 基线 ✅(修复 3 类真问题)

构建:`cmake -B build-san -DSANITIZE=address,undefined`;运行时
`ASAN_OPTIONS=detect_leaks=0:log_path=...`(先聚焦内存错误,泄漏检测另行开启)。

首轮全量 e2e 暴露并修复:

1. **table[3] memcpy 桥重叠区间 UB**(多应用触发,dst<src 重叠 2 字节):
   真机 SDK memcpy 是朴素前向循环;新增 `arm_ext_guest_memcpy`,重叠时前向
   逐字节(dst>src 时产生与真机一致的周期"涂抹",与 memmove 不同),不重叠
   走宿主快速路径。
2. **table[6] strncpy 桥 dst==src**(talkcat):同理 `arm_ext_guest_strncpy`。
3. **`arm_str` 空串退路越界**:退路是 1 字节 `""` 字面量,`_DrawText` 按
   UCS2 双字节读首字符 → global-buffer-overflow;改为 4 字节全零静态缓冲。

另修 issues doc **B3**:`arm_alloc` 加 `len > EXT_MEM_SIZE` 上限守卫,
杜绝 align4/heap_top+len 回绕绕过容量检查。

复跑结果:**ASan/UBSan 报告 0 条**。3 并发下 6 个用例失败均为像素时序类
(sanitizer 减速使固定延时采样踩到不同帧;普通构建 27/27 全绿)——串行复跑
见下文"遗留"。

## P0.2 单元测试基座 ✅

- `test/unit/test_arm_ext.c`:直接 `#include` executor 翻译单元测 static 函数;
  `unit_stubs.c` 提供"一触即 abort"的 mythroad/bridge 桩(意外触达=大声失败)。
- CMake `vmrp-unit` 目标 + ctest 注册。
- 覆盖:`arm_alloc`(对齐/保留区跳过/容量/B3 溢出回归)、
  `arm_ext_compact_heap_cut_range`(中部拆分/裁头/裁尾/整段摘除/无重叠 5 拓扑,
  含 free_bytes 记账)、`arm_ext_guest_memcpy/strncpy`(重叠语义)、
  不变量检查器正负样本。当前 47 checks 全过。
- 待补:find_wrapper_* 的字节资产正负样本测试(需从 fixtures 提取 wrapper
  片段,Phase 4 指纹治理时一并做)。

## P0.3 不变量检查器 ✅

- `arm_ext_verify_invariants(m, where)`,`VMRP_ARM_EXT_INVARIANTS=1` 门控,
  在 `arm_ext_call` / `arm_ext_call_dispatch` 入口调用(高频入口检查等价于
  上次调用的出口检查)。只读,违反打印 `INVARIANT ...` 不中断。
- 检查项:heap_top 界内;nested 注册表容量与映像可映射;active/timer owner
  必须指向 wrapper 或已注册模块的 P;**compact free-list 不得覆盖模块映像/
  ER_RW**(gzwdzjs 无 netpay 冻结的根因类,状态级提前报警)。
- 验证:负样本单测复刻 gzwdzjs 根因场景能报出 violation;
  gzwdzjs+gxdzc e2e 开启检查零报警、结果不变。

## P0.4 ABI 知识文档 ✅

- `docs/arm-ext-abi.md`:地址空间全景、P 结构、extChunk 布局、compact 堆
  控制块、timer 节点变体与调度器偏移矩阵、模块生命周期、三层分配器;
  每个偏移标注证据强度(SDK 定义/wrapper 反汇编/运行观测),9 处"待核实"
  诚实标注。

## 回归验证

- 普通构建全量 e2e:16 文件 / 27 用例全绿(含全部今日改动)。
- `VMRP_WERROR=ON` 冒烟构建零错误。
- vmrp-unit 47 checks 全过。
- sanitizer 报告 0 条;时序性失败的串行复跑结果见附注。

## 遗留 / 移交 Phase 1

- `bridge_dsm_network_cb` 把 host 函数指针截断为 32 位再还原调用
  (native 路径 host 指针超 4GB 即崩)——新发现,待记入 issues doc 处理。
- sanitizer 下像素时序失败无法完全消除(减速本质),验证纪律定为:
  **sanitizer 运行以"报告零容忍"为准,像素断言以普通构建为准**。
- mythroad/ 指针截断告警群(需系统性 uintptr_t 化,择机独立处理)。
