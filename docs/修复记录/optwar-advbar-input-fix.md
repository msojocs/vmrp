# optwar 顶部广告条输入修复记录

## 目标

- 目标用例：`pnpm vitest run test/e2e/optwar/game-prepare.test.ts`
- 现象：自动进入游戏主菜单后，顶部 advbar 广告条仍停留；按 `LEFT`/`RIGHT` 后预期广告条消失并且菜单切换，但当前未消失。
- 约束：不使用 xvfb；trace 日志很大，避免全量 trace；修复不能写应用特定判断；需要结合 PPM 像素验证。

## 当前进度

1. 已确认目标测试会准备 `netpay.mrp` 与 `advbar.mrp`，启动 `test/fixtures/optwar.mrp`，通过 E2E socket 注入点击和方向键，并用 PPM 读取具体像素断言。
2. `test/e2e/optwar/game-prepare.test.ts` 与既有 `opbzqe` 用例结构一致，失败点集中在按方向键后的前台广告条消隐/主菜单输入接收。
3. `src/e2e_control.c` 的 `KEY` 命令会按下 500ms 后释放；`src/main.c` 将 SDL 方向键映射为 `MR_KEY_LEFT`/`MR_KEY_RIGHT`，再进入 `event()`。
4. 下一步检查 Mythroad `mr_event` 和 ARM EXT wrapper 的前台/后台分发、定时器和屏幕 owner 逻辑，并对 optwar/advbar 相关 EXT 做定点反汇编。

## 复现记录

- 执行 `pnpm vitest run test/e2e/optwar/game-prepare.test.ts` 后失败：
  - 失败点不是最终像素断言，而是 `await skyengine.key('RIGHT', 1_000)` 内部的 `WAIT_DRAW` 超时。
  - E2E 返回：`ERR wait_draw_timeout current=22 target>22`。
- 这说明 `RIGHT` 按键注入后没有产生任何新的 `guiDrawBitmap()`，菜单未切换，广告条也没有触发消隐重绘。
- 下一步用手动 E2E socket 驱动保留 PPM 与日志，避免 Vitest `afterEach` 清理临时目录。

## 手动 E2E/PPM 记录

- 诊断目录：`/tmp/skyengine-optwar-diag-Tmz6cA`。
- 环境只启用 `VMRP_ARM_EXT_DIAG=1`，未启用全量 trace。
- 按键前 `DRAW_COUNT=22`，按 `KEY RIGHT` 并等待 1500ms 后仍为 `DRAW_COUNT=22`。
- PPM 像素：
  - 按键前 `menu.ppm`：`(110,27)=[128,48,40]`，`(120,20)=[176,120,120]`，`(83,267)=[24,24,24]`，`(10,41)=[176,192,200]`。
  - 按键后 `after-right.ppm`：上述四个像素完全相同。
- 结论：`optwar` 的问题比 `opbzqe` 更早；方向键没有导致 game 显式 present，因此不是单纯 foreground cover 未释放。

## ARM EXT 分层诊断

- `optwar` 的 primary game 注册为 `P=0x6F4B2C, helper=0x65605D`，后续前台子模块 advbar 为 `P=0x703F9C, helper=0x669089`。
- wrapper 定时器仍在 `P=0x625C20, helper=0xE800B0`，并会代理触发 primary game 的全屏 `present29 x=0 y=0 w=240 h=320`。
- 诊断日志里同一个 `present29` 调用点从 `accept=1` 变成 `accept=0`：
  - 拒绝时 `cover27=65535..0`，表示 foreground cover 的行跨度已经为空；
  - 但 `fgOwnerP=0x703F9C, fgOwnerH=0x669089` 仍然指向 advbar；
  - `arm_ext_should_accept_screen_write()` 因 owner 仍为 active child 而拒绝 primary present，`arm_ext_should_accept_visible_present()` 又因为没有 cover 行跨度而无法进入“裁剪遮挡后提交”的分支。
- 初步结论：`foreground_screen_owner_*` 被当成了比 `foreground_cover` 更强的可见遮挡依据。对于 `extChunk[0x34] == 0` 的非模态 advbar，这会在 cover 已经被下层显式 present 清空后留下 stale owner，导致后续下层菜单重绘无法显示。

## 第一阶段修复验证

- 已在 `arm_ext_should_accept_screen_write()` 进入 primary/wrapper 屏幕写入拒绝判断前，增加非模态空 cover 的 stale owner 清理。
- 修复后 `cmake --build build --target skyengine` 通过。
- 重新执行 `pnpm vitest run test/e2e/optwar/game-prepare.test.ts` 后，失败点从 `WAIT_DRAW` 超时推进到最终像素断言：
  - `(110,27)` 已经变化，说明顶部 advbar 区域可以被下层重绘覆盖；
  - `(10,41)` 未变化，说明方向键事件之后菜单切换仍未达到测试预期，或该采样点不能代表菜单切换。
- 手动保留 PPM 的目录：`/tmp/skyengine-optwar-afterfix-5foXHm`。
  - `menu.ppm`：`(110,27)=[128,48,40]`，`(120,20)=[176,120,120]`，`(83,267)=[24,24,24]`，`(10,41)=[176,192,200]`。
  - `after-right.ppm`：`(110,27)=[128,152,168]`，`(120,20)=[128,152,168]`，`(83,267)=[24,24,24]`，`(10,41)=[176,192,200]`。
- 下一步聚焦事件路由：确认 `code=1` 输入的 `MR_KEY_RIGHT/MR_KEY_LEFT` 是否经 wrapper 正确分发到 primary game，或是否仍被非模态 advbar 的前台分发元数据截留。

## 第二阶段事件路由诊断

- 新增窄口径诊断点，只在 `VMRP_ARM_EXT_DIAG=1` 下打印 `code=1` 事件元组和 ARM helper 原始返回值，避免开启全量 trace。
- 手动复现目录：`/tmp/skyengine-optwar-diag2-IyRthX`。
- 输入确认：
  - RIGHT press/release：`event=0,param0=15` 与 `event=1,param0=15`。
  - LEFT press/release：`event=0,param0=14` 与 `event=1,param0=14`。
- PPM 差异：
  - `menu.ppm` 到 `after-right.ppm` 只改变顶部 `y=0..39` 共 9600 个像素；
  - `after-right.ppm` 到 `after-left.ppm` 完全没有像素变化。
- 结论：菜单确实没有切换，不是测试采样点误判。顶部广告条的可见遮挡已能被清除，但方向键仍没有进入主游戏菜单状态机。
- 关键日志：
  - 按键时 `activeP=0x703F9C, activeH=0x669089, active[34]=0`，仍是非模态 advbar；
  - wrapper helper 原始返回 `r0=0x1`，即 `MR_IGNORE`；
  - 当前宿主逻辑只要 `code=1 && has_separate_wrapper` 就设置 `wrapper_modal_event_routed=1`，最终把 wrapper 的 `MR_IGNORE` 强制改成 `MR_SUCCESS`。
- 反汇编核对：
  - optwar 的 `cfunction.ext` wrapper 入口是 ARM `0xE800B0`，跳到 Thumb `0xE83464`；
  - `0xE834B6` 是 code=1 分支：读取事件三元组后调用 wrapper 事件/控件分发；
  - 当前非模态 advbar 状态下 wrapper 返回 `MR_IGNORE`，说明前台 wrapper 没有消费方向键，宿主不应把它改成已处理。
- 下一步修正：只在模态深度 `extChunk[0x34] > 0` 时保留“wrapper 已处理原始事件”的成功返回；非模态 wrapper 返回 `MR_IGNORE` 时必须原样返回，让 `mr_event()` 继续走后续事件路径。

## 用户澄清后的两次按键验证

- 语义澄清：进入主菜单后第一次 `LEFT`/`RIGHT` 只关闭顶部 advbar，不切换菜单；第二次方向键才切换菜单。
- 保留 PPM 目录：`/tmp/skyengine-optwar-tworight-HEaPUH`。
- 手动 E2E 结果：
  - `menu` draw count：36。
  - 第一次 `RIGHT` 后 draw count：51。
  - 第二次 `RIGHT` 后 draw count：66。
- PPM 差异：
  - `menu -> after-right-1`：9896 像素变化，bbox `[0,0,239,273]`；顶部 `(110,27)` 从 `[128,48,40]` 变为 `[128,152,168]`，说明广告条已经被下层重绘覆盖。
  - `after-right-1 -> after-right-2`：293 像素变化，bbox `[96,263,142,273]`，变化集中在底部菜单提示/选中状态区域。
  - 代表性菜单像素：`(110,263)` 从第一次后的 `[24,24,24]` 变为第二次后的 `[0,252,0]`。
- 因此原测试里“第一次 `RIGHT` 后 `(10,41)` 必须变化”的断言不符合实际业务语义；`(10,41)` 在两次按键中都保持 `[176,192,200]`，不是菜单切换的有效采样点。

## 最终修复方案

1. 屏幕合成修复：
   - 在 primary/wrapper 屏幕写入拒绝判断前，检查非模态前台子模块是否已经没有 `foreground_cover` 行跨度。
   - 若 `foreground_screen_owner_*` 仍指向当前非模态子模块但 cover 为空，清理 stale owner。
   - 反汇编依据：`extChunk[0x34] == 0` 表示当前 advbar 不是 suspend 模态层；没有 cover 行跨度时，owner 元数据不能继续作为可见遮挡掩码。
2. 事件路由修复：
   - `code=1` 仍先进入 wrapper helper，这是 `mrc_extHelper` 的真实路径。
   - 宿主注入的原始事件是 20-byte `mr_c_event_st`，它已经先经过 wrapper；即使 wrapper 返回 `MR_IGNORE`，宿主也向 `mr_event()` 返回 `MR_SUCCESS`，避免同一个输入随后又经 Lua `dealevent -> _strCom(801)` 的 12-byte 路径重复处理。
   - Lua 内部 12-byte `_strCom(801)` 转发仍保留 helper 的真实返回值。这样非模态 advbar 在清掉可见层后可以返回 `MR_IGNORE`，让后续输入继续交给游戏菜单状态机。
   - 若一次事件导致 `extChunk[0x34]` 从 0 变为大于 0，也视为 wrapper 已消费，避免进入模态的同一输入落到下层游戏。
3. 测试修正：
   - 第一次 `RIGHT`：断言顶部广告条像素 `(110,27)` 已变化。
   - 第一次 `RIGHT`：断言底部菜单像素 `(98,264)` 保持不变，防止同一方向键被重复处理导致菜单也切换。
   - 第二次 `RIGHT`：断言底部菜单像素 `(98,264)` 相对第一次后的画面发生变化。

## dota 回归：一次方向键切两格

- 用户补充的新现象：`dota` 主菜单按一次方向键，实际切换了两个菜单。
- 关联风险点：`mr_event()` 会先调用 `native_ext_event(1, 20-byte event)`；若返回 `MR_IGNORE`，随后继续调用 Lua `dealevent`，部分 Lua 代码又会通过 `_strCom(801, ..., code=1)` 发起一次 12-byte EXT 事件。
- 因此如果 wrapper 的 20-byte 原始事件已经让非模态前台/菜单状态机移动一次，但仍返回 `MR_IGNORE`，宿主继续下发到 Lua 12-byte 路径就会形成重复处理。
- 窄诊断确认一次 `UP` 会出现 press/release 各一组 20-byte 原始事件与 12-byte Lua 转发事件；如果 20-byte 返回 `MR_IGNORE`，Lua 12-byte 路径会再次转发，形成双切。
- 最终处理：20-byte 原始事件由宿主消费，12-byte 内部转发不强制成功。该规则基于事件结构长度与 wrapper 调用链，不按 `dota` 或具体 MRP 名称判断。

## 当前验证

- `cmake --build build --target skyengine`：通过。
- `pnpm vitest run test/e2e/optwar/game-prepare.test.ts`：通过。
- `pnpm vitest run test/e2e/dota/download-plugin.test.ts`：通过。
- `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
- `pnpm vitest run test/e2e/gxdzc-pixel.test.ts`：通过。
- 当前 PPM 目录：`/tmp/skyengine-optwar-current-STwnT0`。
  - `menu -> right1`：只改变顶部广告条区域 `y=0..39` 共 9600 像素，菜单区 `y>=40` 无变化。
  - `right1 -> right2`：菜单区变化 296 像素，bbox `[96,263,142,273]`。
  - `(98,264)`：`menu=[0,252,0]`，`right1=[0,252,0]`，`right2=[128,128,128]`。
