# geyaxz 游玩失败菜单后卡死修复进度

## 目标与约束

- 目标用例:`pnpm vitest run test/e2e/geyaxz/play.test.ts`。
- 失败菜单(您失败了!重玩本关/选择关卡)按 DOWN+ENTER 后必须离开当前画面。
- SDL dummy、无 xvfb、无全量 trace;修复不得含应用/按键/像素分支与兜底逻辑。

## 2026-07-18 复现与表象

- 复现:选择菜单项后画面回到关卡(剩余步数 0)且整机失去响应;
  最后一个 waitFor 超时,断言 `(32,229)==[96,84,144]`(仍是关卡画面)。
- DIAG 运行:事件全程正常进入 game(+0x1367C/+0x12A0C 命中),失败菜单
  导航正常;最后一次 wrapper 活动是 `0x1504`(调度 10ms 后执行 fnC60
  延迟关闭)与 `table31_pre t=10`,此后**整个进程再无输出**;期间正在
  反复读 `plugins\netpay.mrp`(计费插件加载轮询)。

## 2026-07-18 定位:guest 死循环

- WSL 下 core 被吞、yama 禁 attach → 用 `VMRP_BIN` 包装脚本让 vmrp 生为
  gdb 子进程,卡死时 SIGINT 采样。
- 主线程栈:卡在 unicorn 内执行 wrapper 代码;12 次 PC/LR 采样全部落在
  `0xE80D28`(延迟命令队列排空循环)、`0xE81254`(命令入口)、
  `0xE80DE4`(按 id 查订阅者)、`0xE818F2`(首参≠1 的消息转发)。
- gdb 直读 guest 内存:队列 {head,tail}=RW+0x2C/0x30 = {0x270D, 0x0}——
  head 是小整数而非堆指针,排空循环在低内存垃圾链上永久循环,且首次入队
  时 `*tail` 已把节点指针写进 EXT_TABLE 槽(tail 曾为 0x10068=table[26])。

## 2026-07-18 根因:staging 窗口 R9 语义分歧(与 boot bug 同源)

- 内存写观察:{head,tail} 在启动期被写成 {0x270D, 0x10068},写入 PC 在
  0x40000000 arena(graphics.ext file+0x396/+0x33A),r9=0x225E60(wrapper
  的 RW)。boot bug 的 selected=1(RW+0x1C)是同一次 init 的另一处写入。
- graphics.ext 先装入主内存 0x25B95C,后被私有 loader 迁入 arena
  0x40018800(record 0x2BD04C 复用,P=0x2BD0AC);其 helper veneer
  (file+0x17A8)语义是 r9=[P][0],且 P[0] 已被 arena entry 正确写为
  0x25DA90。
- 逐块追踪 hook_restore_r9:fn 0x2E8 入口 r9=0x25DA90(正确)→ init 内
  `blx r5` 进 wrapper 命令(cmd 1,0x64)→ 宿主在 wrapper 代码入口把 r9
  切为 wrapper rw(因旧实例已注册且 rw 相同)→ 返回 arena 代码时新基址
  **尚未注册**(mr_cacheSync 已过、extChunk[8] 未确认),registry 查不到、
  无法恢复 → init 余下部分以 wrapper R9 运行,按固定偏移把自己的初始化
  数据写进 wrapper RW。
- **完整修复已实现并验证可消除腐蚀**(staging 窗口按 child P[0] 恢复 R9 /
  位移记账两种变体):但 graphics/SKY 推广计费框架随即真正跑通启动流程
  (POST rop.skymobiapp.com/payOneAsTlv → 200 OK → 写 system/ntp/termsync、
  gbreg.sys),game 每帧 present 空帧等待一个 vmrp 未模拟的完成信号,
  boot 黑屏 >10s。当前全部通过的用例都建立在该框架瘫痪的基线上,完整
  R9 修复需先补齐该链路,回退。

## 2026-07-18 落地方案:结构性不变量修复(与第一轮同类)

- `arm_ext_restore_wrapper_command_queue`:head/tail **同时**不是合法堆
  指针(空态 {0,&head} 或可映射堆指针)时恢复 canonical empty;在每次
  wrapper arm_ext_call 入口执行,先于首次入队,避免 table 槽被写坏。
- `arm_ext_restore_drained_event_layer` 恢复(boot 修复),注释重写为
  已证实的腐蚀机制;两者共用同一入口点,无应用分支。
- `mythroad.c` FULL runtime Lua dealevent 单入口改动不受影响,保留。
- R9 完整修复方向、staging 窗口机制与推广框架链路缺口已记入本文档,
  作为后续工作。

## 验证

- geyaxz boot 5.43s 通过;play 92.49s 通过(失败菜单 → 选择关卡正常)。
- 全量 e2e 回归见下节。

## 2026-07-18 最终验证矩阵(SDL dummy,无 xvfb,无全量 trace)

- `cmake --build build --target vmrp`:通过;
- `ctest --test-dir build`:1/1 通过;`pnpm exec tsc --noEmit`:通过;
- geyaxz boot 5.43s、play 92.49s:通过;
- `pnpm vitest run test/e2e --retry=0`:**27 文件 / 49 测试全部通过**,
  278.80s(含新增 play 用例);
- 全部临时实验代码(RW 写观察、arena 块追踪、R9 追踪、gdb 包装)已删除;
  保留的通用诊断增强:`VMRP_ARM_EXT_WATCH_PC` 覆盖 wrapper 本体、
  观察点输出含 r9。
