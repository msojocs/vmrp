# Phase 5 长期项 —— 执行记录(第 1 批)

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第八节

## P5.2 性能 ✅

- **pack 归属判定缓存**:`arm_ext_current_pack_matches_staged_file` 此前每次
  调用整包读入 + 逐条目 gunzip 比对(issues doc M5,大 MRP 注册路径秒级
  卡顿)。新增 8 槽正向结果缓存(arm_ext_internal.h `ArmExtPackMatchCache`),
  命中键 = staged 内容 FNV-1a + 包路径 + 包长 + 包头 64B FNV——包被重新
  下载/替换(dota/talkcat 流程)时包长或包头必变,不会拿旧包结论冒充新包;
  只缓存"匹配成功",负结果始终走慢路径。
- **字节扫描加速**:`arm_ext_bytes_contain` 从逐字节推进改为 memchr 跳到
  首字节候选再整段比对,结果与朴素实现严格等价(单测以朴素实现为
  oracle 对拍,含首字节高频重复构造 + 边界)。
- vmrp-unit 升至 **69 checks**。

## P5.3 golden 关键帧回归 ✅

- 方法论:候选帧间隔 2s 双采样逐字节一致(排除动画)且**跨进程重启可
  复现**才入选 golden。
- 入选:`gzwdzjs-bgm-dialog`、`gxdzc-boot`(test/fixtures/golden/*.ppm);
  落选:opbzqe-menu(双采样即不同,动画),wbrw-home(首轮入选,复验
  发现主页含时钟且内容跨运行可变——一轮差 20 像素,另一轮差 2404 像素,
  已剔除并在用例注释记录)。
- 新用例 `test/e2e/golden/golden-frames.test.ts`:全图 `diffPixelCount==0`,
  waitFor 只吸收启动抖动、不放宽判据。渲染有意变更时用同一方法重新生成
  资产并在提交说明写明。
- e2e 总量:16→**17 文件,27→29 用例**,全绿。

## P5.1 多实例化 —— 设计定稿,实施择机

- `docs/arm-ext-multi-instance-design.md`:阻碍多实例的 12 个宿主可变全局
  清单、`ArmExtHostIface` 目标形态、五步迁移(每步零行为变化脚手架起步)、
  热路径解引用风险与回退策略、mythroad 层边界说明。
- 未实施原因:收益方(wasm 多实例/并行单测)当前无阻塞需求,且步骤 3
  (gzip 工作区去共享)需 mythroad 层配合,应独立排期。

## 验证

- vmrp-unit 69 checks ✅;全量 e2e 17 文件/29 用例 ✅(含新 golden 用例);
  第一方告警 0 ✅。
- 错误路径用例说明:方案提到的"删 netpay"错误路径已由
  test/e2e/gzwdzjs/game-start.test.ts 覆盖(本轮工作起点即该用例);
  "断网"类用例涉及本地 mock 服务开关设计,列入后续。
