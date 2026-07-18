# geyaxz 背景音乐选择事件路由修复进度

## 目标与约束

- 目标用例：`pnpm vitest run test/e2e/geyaxz/boot-to-home.test.ts`。
- 关闭背景音乐后必须离开声音选择页。
- 使用 SDL dummy 驱动，不使用 xvfb；只做定点 PC 观测，不开启全量 trace。
- 修复依据必须是通用 runtime/ARM EXT 契约，不能按应用、按键或像素分支。
- 最终以 PPM 证明画面切换，并回归现有 ARM EXT 输入用例。

## 2026-07-18 复现与 PPM

- fixture：`test/fixtures/geyaxz.mrp`，309017 字节，SHA-256
  `63253ca7b377f043fc941013eba6c018718d36a3f4f60def56d76d9a5c8023b8`。
- 失败现场：`/tmp/vmrp-e2e-veYLLu/`。
- `bgm-select.ppm` 实际为 240x320，SHA-256
  `634877a83bee1b664fb68ef8b423de181c5f1f895fda645912c3d44e938dd62b`。
- `RIGHT_SOFT` 后有新 draw，但 PPM 仍是“是否开启声音”；像素
  `(112,125)` 保持 `[128,220,232]`。默认重试和无重试运行均稳定失败。

## 2026-07-18 反汇编与运行时证据

- 宿主正确发送 PRESS/RELEASE `(type=0/1,param0=18)`；18 是
  `MR_KEY_SOFTRIGHT`。改变物理 hold 时间不能修复问题。
- wrapper 调用链：ARM helper `0xE800B0` -> Thumb `0xE82A44` -> code=1
  分支 `0xE82A96` -> 回调链 `0xE819CC..0xE81A74`。
- 回调链从 wrapper `RW+0xEC` 的选中链表取节点，以节点中的 P/helper 调用
  子模块。链表迭代把 `MR_IGNORE=1` 当作“继续下一个节点”，wrapper 对外仍可
  返回 1，因此不能仅从 wrapper 的最终 R0 推断 primary 是否收到事件。
- primary `game.ext` 运行时 file=`0x226308`，helper=`0x23A549`。
  事件入口为 file+`0x1367C`，键盘核心分发为 file+`0x12A0C`。
  UI 状态门命中软右键后调用 file+`0x13748`，直接证明该键会关闭/推进当前页。
- 失败时 wrapper 对两个 20-byte 原始事件都返回 `MR_IGNORE`，modal depth
  保持 0，active 仍是非模态辅助 child `P=0x2D3AAC/H=0x260C31`。
- `src/mythroad/mythroad.c` 当前先发送 20-byte `native_ext_event`，只有它返回
  `MR_IGNORE` 才执行 Lua `dealevent`；但 `src/arm_ext_executor.c` 又把所有
  20-byte wrapper 事件强制改成 `MR_SUCCESS`，所以 Lua 路径被无条件截断。
- 包内 `start.mr` 的 `dealevent` 会把事件打包为 12-byte/20-byte结构并调用
  `_strCom(801,...,code=1)`。FULL 应用因此同时存在 native 与 Lua 两个入口。

## 根因与修复方向

20-byte 强制消费是为修复 DOTA/optwar 的一次按键双处理而加入，但它是在
双入口之后抑制第二次调用，无法区分 wrapper 已处理与只返回忽略。通用修复应
消除重复入口：保留显式注册的 `native_event_function` 最高优先级；若 Lua
`dealevent` 存在，由 Lua 独占普通事件；只有没有该函数的 EXT-only runtime 才
发送 20-byte `native_ext_event`。这与当前 pause/resume 的 Lua-owner、native
fallback 契约一致，也使 DOTA/optwar 每个事件只进入 wrapper 一次。

下一检查点：修改 `mr_event()` 与删除执行器中的 20-byte 强制成功规则后，先跑
geyaxz 目标用例并比较前后 PPM，再回归 optwar、DOTA 和其它 E2E。

## 2026-07-18 空事件层根因与首次通过

- 单入口实验排除了“双入口就是全部根因”：改为只调用 Lua `dealevent` 后，
  每次 PRESS/RELEASE 确实只产生一个 12-byte code=1，但目标仍失败。
- `VMRP_ARM_EXT_WATCH_PC` 对 primary file+`0x14240`、`0x1367C`、
  `0x12A0C`、`0x13748` 的观察点全部零命中，证明事件没有进入 game。
- wrapper `0xE81A3C..0xE81A74` 的反汇编和运行时内存一致：
  `RW+0x1C` 是 selected event layer，`RW+0xF4` 是八组 `{head,tail}`；
  list node 的 `+0x10` 指向 extChunk record，record 的 `+0x08/+0x1C/+0x34`
  分别是 helper、P 和 suspend depth。
- 失败时 selected=1，slot 1 为 canonical empty list：`head=0` 且
  `tail=&head=0x225F5C`。slot 0 有四个节点，末节点引用 primary extChunk
  `0x2BC56C`。wrapper 只遍历 selected slot，所以必然返回 `MR_IGNORE`。
- wrapper 层栈函数 `0xE80BA4(+1)` 会把新层初始化为 canonical empty，成功
  child load 随后在 `0xE811BC..0xE8120A` 插入节点。回调结束后顶层重新成为
  canonical empty、下层 primary 仍有效，说明 child 节点耗尽后漏掉了配对 pop。
- 通用修复在 code=1 的正常 wrapper 分发前检查该结构不变量：仅当 primary
  suspend depth 为 0、selected 顶层连续为 canonical empty，并且下层链能实际
  找到 primary extChunk 时，才把 selected 收缩到该下层。没有按应用、按键、
  helper 地址、像素或画面状态判断，也不绕过 wrapper。
- 首次修复运行：`/tmp/vmrp-e2e-xONfhU/`，目标用例 5.585 秒通过；窄日志为
  `event_layer_restore selected=1 target=0 primaryChunk=0x2BC56C`。

下一检查点：删除临时 event-chain dump，重新构建，用定点 PC 与 PPM 证明最终
二进制进入 primary 并切页，然后执行 optwar/DOTA 及完整 E2E 回归。

## 2026-07-18 最终验证

- 临时 wrapper event-chain dump 及声明已全部删除；最终代码只保留通用层栈
  修复和 `VMRP_ARM_EXT_DIAG` 下的一行修复记录。
- 最终定点运行：`/tmp/vmrp-e2e-cEkkIw/`，目标用例 5.561 秒通过。
- 事件修复前首先记录
  `event_layer_restore selected=1 target=0 primaryChunk=0x2BC56C`；随后
  PRESS 和 RELEASE 都命中 primary `game.ext+0x1367C` 与 `+0x12A0C`，
  参数 `r1=0x12`，证明软右键真实进入 game 事件状态机。
- PPM 前后均为 240x320：
  - 修复前声音选择页 SHA-256：
    `634877a83bee1b664fb68ef8b423de181c5f1f895fda645912c3d44e938dd62b`；
  - 修复后主菜单 SHA-256：
    `4f9cb8feff24aae16a6ca50b32752fa7ffa84228ddef6f3a69db004a68788e50`；
  - 76,629 个像素变化，bbox=`[0,0,239,319]`；
  - `(112,125)` 从 `[128,220,232]` 变为 `[160,132,32]`。
- PPM 可视检查确认修复前是“是否开启声音”，修复后是带“开始游戏、玩家社区、
  声音、游戏帮助、游戏关于、游戏退出”的主菜单，不是单点误判。

验证矩阵（全部显式使用 SDL dummy，未使用 xvfb，未开启全量 trace）：

- `cmake --build build --target vmrp --parallel 2`：通过；
- `ctest --test-dir build --output-on-failure`：1/1 通过；
- `pnpm exec tsc --noEmit`：通过；
- geyaxz 目标用例：普通、DIAG、定点 WATCH 三种运行均通过；
- optwar、DOTA、opbzqe、gxdzc 聚焦回归：5 文件、7 测试通过；
- `pnpm vitest run test/e2e --reporter=verbose --retry=0`：26 文件、48 测试
  全部通过，耗时 312.96 秒。

最终实现没有应用/包名、按键、helper 地址或像素分支；没有等待画面变化的兜底；
没有绕过 wrapper。FULL runtime 的普通事件由 Lua `dealevent` 单一拥有，缺少
该 hook 的 EXT-only runtime 仍使用 20-byte native 入口。wrapper 只在完整结构
证据成立时收缩耗尽的空事件层，模态深度非零、顶层非 canonical empty、下层不含
primary extChunk 或链损坏时均不修改 guest 状态。

## 2026-07-18 方案复审（第二轮）

- 重新构建后目标用例通过（5.647s，`/tmp/vmrp-e2e-0X45f7`），确认工作区
  修复有效，作为复审基线。
- 复审焦点拆成两条独立证据链：
  1. `mythroad.c` 事件路由：对照仓库根 `mythroad/` 原版平台源码与
     `temp/mythroadlib_disasm/` 固件反汇编，验证“FULL runtime 由 Lua
     `dealevent` 独占普通事件”是否就是真机语义（若真机从不直接把原始
     事件发给 ext，则 20-byte native 路径本是 vmrp 兼容层，改动成立）。
  2. `arm_ext_executor.c` 空层收缩：静态追 cfunction.ext 中 push(+0x111C)/
     pop(+0xCD2)/unlink 的全部调用点，判断空层残留是 (a) child 加载失败
     漏 pop、(b) child 注销只 unlink 不 pop、还是 (c) pop 依赖宿主未触发
     的调用路径——决定“宿主改 guest RW”是否为正确层级的修复。
- 溯源：被删除的 20-byte 强制 MR_SUCCESS 规则来自 fe4d20c（雷电1941
  广告条），属于“下游抑制第二次进入”；新方案在源头消除双入口，层级
  更合理，但需回归 optwar/DOTA 证明等价。
- cfunction.ext/game.ext 已从 fixture 解出至 `temp/disasm/geyaxz/`
  （MRPG 索引起于 240，条目=4B名长+名+{off,packedLen}+12B，gzip 自解）。

## 2026-07-18 复审结论:机制证实与叙事修正

复审用 wrapper 基址 PC 观察点(新增:`VMRP_ARM_EXT_WATCH_PC` 现在同样覆盖
cfunction.ext 本体)加临时 RW+0x1C/层表内存写钩子(已删除),完整还原了
层栈的真实生命周期,修正了第一轮的错误推断:

- **push 不是来自 wrapper 的 adjust_layer(+0xBA4)**。失败运行中 +0xBA4
  从未执行。selected=1 由 **graphics.ext** 写入:它被加载进 0x40000000
  arena、持 wrapper R9 运行,先经 wrapper 命令 (1,0x64) 握手拿到 wrapper
  内部函数指针,随后在自身 init(file+0x332)直接 store selected=1,且
  **全模块无任何恢复 selected 的代码**——层1 是它永久声明的 overlay 层。
- wrapper 侧唯一的降层路径:命令 (1,0x2A) → 0xE81520 经 0xE83908 挂
  10ms 软定时器 → 回调 0xE80C80(teardown:清空当前层+adjust_layer(-1))。
  本次失败中 guest 从未发出 0x2A,宿主 tick(0x3A80)一直正常,不存在
  宿主漏驱动定时器的问题。
- 实测时序:层0 依次注册 4 个订阅者(含 primary,分别来自 lr=0xE8218F/
  0x24D16F/0x2506BF/0x256B9B/0x25D11B 等模块,含 graphics.ext 自己从
  arena 的注册)→ graphics.ext init 置 selected=1 → overlay child
  (lr=0x260C47)注册进层1 → 它用命令 (1,0x21)(经 1ms 延迟 fnC40,
  订阅者 id=0xB)自行注销 → 层1 canonical empty,分发器(+0x1A3C,无
  空层回退)从此丢弃所有事件。
- 分发器、插入(+0x11F8)、注销(+0xF4C)的结构与修复假设完全一致:
  节点 +0x10 指向 extChunk record;层表 RW+0xF4 八组 {head,tail}。
- 结论:**宿主侧结构性收缩(arm_ext_restore_drained_event_layer)是
  正确层级的修复**——没有可归责于 vmrp 的缺失调用或时序错误可以更根本地
  修;"修加载顺序"缺乏真机依据且风险更大。实现保留,仅重写了注释使其
  与证实的机制一致。
- **mythroad.c 改动获原版源码背书**:初始提交 d5ba4af 的原版 mr_event
  只有 mr_event_function → Lua dealevent 两级,从不直接把原始事件发给
  C ext;20-byte native_ext_event 是 vmrp 为 EXT-only runtime 加的兼容
  入口。改动使 FULL runtime 回归原生单入口语义,方向正确,保留。
- 复审后验证:目标用例通过(5.557s,`/tmp/vmrp-e2e-EQOspp`),最终 PPM
  像素 (112,125)=[160,132,32] 与已知主菜单一致(菜单含动画,整帧哈希
  随帧变化,不能作回归断言)。

复审最终验证矩阵(SDL dummy,无 xvfb,无全量 trace,临时实验代码已全部删除):

- `cmake --build build --target vmrp`:通过;
- `ctest --test-dir build`:1/1 通过;`pnpm exec tsc --noEmit`:通过;
- geyaxz 目标用例:通过(5.557s),最终 PPM (112,125)=[160,132,32]=主菜单;
- `pnpm vitest run test/e2e --retry=0`:26 文件 / 48 测试全部通过,314.91s。

复审保留原实现(结构性空层收缩 + FULL runtime Lua 单入口),仅:
1. 重写 `arm_ext_restore_drained_event_layer` 注释为证实的机制
   (graphics.ext overlay 层声明 / 0x2A→10ms teardown 唯一降层路径);
2. `mr_event` 注释补原版源码证据(d5ba4af);
3. `VMRP_ARM_EXT_WATCH_PC` 扩展覆盖 wrapper 本体(通用诊断基建)。
