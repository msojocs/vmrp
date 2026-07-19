# OPBZQE 顶部黑区问题排查记录

日期：2026-06-22

## 目标

- 修复 `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts` 自动进入游戏界面后顶部出现黑色区域的问题。
- 预期启动后完整显示界面，不能只针对特定应用写特殊分支。
- 结合反汇编确认 advbar 插件相关行为，避免加入兜底逻辑。

## 初始状态

- 工作区已有用户改动：`docs/prompt.md`、新增 `test/e2e/opbzqe/game-prepare.test.ts`、新增 `test/fixtures/opbzqe.mrp`、以及若干删除的 integration 脚本。
- 本次排查会避免回滚这些现有改动。
- 初步假设：启动阶段曾短暂显示正确画面，随后顶部变黑，说明基础解码/绘制路径可用，后续插件加载或刷新区域处理更可能破坏了顶部帧缓冲。

## 进度

- 已阅读目标测试：进入主菜单后断言 `y<40` 顶部像素，测试注释说明问题帧只显式提交 `y=40..319`，因此黑区很可能来自局部刷新/前台画面交接。
- 已定位宿主刷新入口：
  - `src/main.c::guiDrawBitmap()` 把 MRP 的 `mr_drawBitmap` 输出复制到 SDL surface。
  - `src/mythroad/mythroad.c::_DispUpEx()` 调用 `mr_drawBitmap(mr_screenBuf, x, y, w, h)`，这是从全屏 stride 的 `mr_screenBuf` 提交局部矩形。
  - `src/arm_ext_executor.c` 内有前台插件 screen owner 逻辑，会接受/拒绝不同 nested EXT 的绘制，并在 callback 退出时同步屏幕快照。
- 初步可疑点：
  - `guiDrawBitmap()` 按 `xx + yy * screen_width` 从 `bmp` 取源像素，这只适合 full-screen stride 源；但 Lua/MRP 层的 `MRF_DispUp()` 存在 `mr_drawBitmap(mr_bitmap[i].p + y * mr_bitmap[i].h + x, x, y, w, h)` 这类“源指针已经偏移”的调用，需确认目标场景是否命中。
  - ARM ext 前台归属逻辑可能在 advbar 插件加载后保留了错误的顶部快照，导致后续主菜单只刷新 `y>=40` 时顶部仍来自黑色插件/空缓冲。

## 复现记录

- 先运行目标测试时，旧 `build/skyengine` 通过；执行 `make -C build -j$(nproc)` 重建后，目标测试稳定失败。
- 失败断言：`wake.pixel(110, 27)` 实际 `[0, 0, 0]`，预期 `[232, 144, 176]`。
- 手动 E2E 复现产物保存在 `/tmp/skyengine-opbzqe-debug/run-xsAKYr`：
  - `menu.ppm` 中 `y=0..39` 全部为 `[0,0,0]`。
  - `menu.ppm` 中 `y=40..319` 有正常菜单画面，断言点 `(113,124)` 为 `[184,192,48]`。
  - 诊断日志显示进入菜单后反复调用 `drawBitmap bmp=0x2001BC x=0 y=40 w=240 h=280`，说明本帧只显式提交了 `y=40..319`。
- `opbzqe.mrp` 解包到 `/tmp/skyengine-opbzqe-extract`，关键 EXT：
  - `game.ext`：65512 bytes
  - `verdload.ext`：18984 bytes
  - `cfunction.ext`：23492 bytes

## 当前判断

- 黑区范围与本帧局部提交矩形完全吻合：`y<40` 没有被提交到 SDL surface。
- ARM ext 的 `leave_screen_context()` 会把 ARM framebuffer 完整复制回宿主 `mr_screenBuf`，但只要回调中发生过任意显式 present，就不会再合成 full-screen present。该策略能避免“清屏后只提交脏区”的应用暴露未提交区域，但在本样本中会让已经写入宿主 shadow buffer 的顶部留在 SDL surface 的旧黑色内容上。
- 临时诊断确认：点击进入菜单后，`leave_screen_context()` 时宿主 shadow buffer 已经有正确顶部像素，`(110,27)=[232,144,176]`、`(120,20)=[64,208,184]`；黑区只存在于 SDL surface。

## 反汇编证据

- `opbzqe.mrp` 内嵌 `game.ext` 的 Thumb 反汇编显示，`0x9FAE..0x9FB4` 明确以 `r0=0, r1=40, r2=240, r3=280` 调用 `0x9118` 局部刷新函数：
  - `0x9FAE: movs r2, #240`
  - `0x9FB0: movs r1, #40`
  - `0x9FB2: movs r0, #0`
  - `0x9FB4: bl 0x9118`
- `0x9118` 再经函数表调用宿主 draw/present 桥，运行日志中的 `drawBitmap ... x=0 y=40 w=240 h=280` 与反汇编一致。
- 结论：`y=40,h=280` 是样本真实局部提交语义，不是 E2E 截图或宿主传参误读。

## 修复策略

- 在 ARM ext 执行器中增加行级 screen damage/present 追踪。
- 对宿主代执行的 screen-cache 绘制 API（点、矩形、位图、文本、平台字符绘制）记录实际改动区域；文本/位图使用前后 framebuffer 差分，避免透明/可变宽绘制把未改动像素误标为脏。
- 对 `mr_drawBitmap` / `DispUpEx` 记录本回调显式提交到 SDL 的区域。
- 回调退出时仍保持原有“有显式 present 时不做全屏兜底”的语义，只额外提交“宿主绘制 API 已改动、但本回调显式 present 未覆盖”的行段。这样不是 `opbzqe` 特判，也不是固定 `y=40` 修复。

## 验证结果

- `make -C build -j$(nproc)`：通过。
- `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
- 手动 PPM 复验目录：`/tmp/skyengine-opbzqe-debug/verify-eaPVDS`。
  - `(110,27)=[232,144,176]`
  - `(120,20)=[64,208,184]`
  - `(113,124)=[184,192,48]`
  - `y=0..39` 无全黑行。
- 相关 e2e：
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts`：通过。
  - `pnpm vitest run test/e2e/gxdzc-pixel.test.ts`：通过。
- `ctest --test-dir build --output-on-failure`：
  - 前 4 个 C/单元测试通过。
  - 失败项来自当前工作区已删除的 integration shell 脚本：`test/integration/gxdzc/test_exit.sh`、`test/integration/gxdzc/test_pay_cancel.sh`、`test/integration/mpc/test_exit.sh`。
  - `e2e_vitest` 在 CTest 的 60s 限制下超时，改为上面的直接 vitest 分文件验证。
- `test/e2e/gghjt/download-plugin.test.ts` 单文件运行超过约 2 分钟无输出，已中断，未作为通过项记录。

## 追加进度：参考图校正后重新排查

- 用户提供的参考 PNG 是顶部 advbar 横幅，说明正确结果不是单纯消除黑色区域，也不是显示游戏菜单背景。
- 当前 `test/e2e/opbzqe/game-prepare.test.ts` 仍含临时 TODO throw，目标用例不能直接作为最终验证；先改用手动 E2E/PPM 与窄口径日志定位 advbar 绘制链路。

## 追加进度：advbar 文件存在后的真实失败

- 将已有 `build/mythroad/plugins/advbar.mrp` 临时复制到运行目录后复现，顶部 `y=0..39` 稳定全黑，符合用户反馈。
- 窄口径诊断目录：`/tmp/skyengine-opbzqe-debug/diag-advbar-lGekUQ`。
- 日志显示 advbar 被加载为 active child：`activeP=0x7021D8 activeH=0x68D1F1`，primary game 绘制多次被 `screen_reject primary` 拒绝。
- 被接受的 active child present 多为 `drawBitmap x=0 y=40 w=240 h=280`，说明 advbar 插件负责把下方游戏区域提交出来，但顶部广告区域尚未正确进入 SDL surface。

## 追加进度：时序修正

- 用户澄清：不是一直全黑，而是 advbar 广告条先正确出现，随后顶部被黑色覆盖。
- 后续定位按“正确广告帧 -> 黑色覆盖帧”的时序展开，重点找覆盖 top 40px 的后续绘制/恢复动作。

## 追加进度：点击后的关键刷新顺序

- 用户澄清后重新读窄口径日志，点击后关键顺序为：
  - `drawBitmap x=0 y=0 w=240 h=40`：advbar active child 首次提交顶部广告区域。
  - 随后 `drawBitmap x=0 y=40 w=240 h=280`：advbar active child 反复提交下方游戏区域。
  - 大量 primary 绘制被 `screen_reject primary` 拒绝，说明现有前景归属逻辑把 advbar 作为活动层。
- 下一步需要抓逐帧 PPM，确认顶部 `y=0,h=40` 这次提交的源像素是否为广告，以及后续哪一次提交/上下文恢复把它覆盖为黑色。

## 追加进度：逐帧 PPM 证据

- 临时 `VMRP_DRAW_SEQ_DIR` 诊断目录：`/tmp/skyengine-opbzqe-debug/drawseq-1782124008-74415`。
- `draw-00088-x0-y0-w240-h40.ppm` 顶部为红色 advbar 广告，顶部平均色约 `(167,63,52)`，`(110,27)=(128,48,40)`。
- `draw-00089..00368` 为下方区域逐行提交，顶部广告保持不变。
- `draw-00369-x0-y0-w240-h320.ppm` 开始出现全屏提交，顶部被改成黑色，说明黑条不是 advbar 从未绘制，而是后续全屏 present 覆盖。

## 追加进度：覆盖来源确认

- 时间：2026-06-22 18:33:42 CST。
- 复读 `/tmp/skyengine-opbzqe-debug/drawseq-log-1782124164-74967/stdout.log`：
  - `draw-00088` 前有 `DIAG drawBitmap ... x=0 y=0 w=240 h=40`，源像素为 advbar 红色横幅。
  - `draw-00089..00368` 没有对应 `DIAG drawBitmap` 单行日志，它们来自 `leave_screen_context()` 的 `arm_ext_present_uncovered_screen_damage()`：advbar 显式提交 `y=40,h=280` 后，执行器补交了未覆盖脏行。
  - `draw-00369` 紧跟 `DIAG drawBitmap ... x=0 y=40 w=240 h=280` 和 `timer_owner lr=0xE848FE ownerP=wrapper`，没有对应 `DIAG drawBitmap x=0 y=0 w=240 h=320`。因此黑顶全屏帧来自 `leave_screen_context()` 的合成全屏提交，而非 advbar 主动提交黑色全屏。
- 语义判断：
  - 同一轮 wrapper timer 调度中，active child 已经提交过屏幕；但外层 wrapper 回调退出时 `screen_presented_in_callback` 仍被当成本层局部状态，且共享 `screen_dirty`，导致外层回调用主 ARM framebuffer 合成全屏。
  - 需要让显式 present 的事实按实际屏幕所有者/嵌套执行传播，不能让外层 wrapper 把 active child 的已提交脏屏再全屏提交一次。

## 追加进度：第一轮修正未通过

- 已移除 `src/main.c` 的 `VMRP_DRAW_SEQ_DIR` 临时逐帧 dump。
- 已尝试把显式 present 深度化，并让 foreground snapshot 只合并 accepted rect/diff；`make -C build -j$(nproc)` 通过。
- `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts` 仍失败：`wake.pixel(110,27)` 仍为 `[0,0,0]`。
- 下一步改用窄口径临时诊断，仅记录 `leave_screen_context()` 是否触发全屏合成、触发时的 `screen_dirty`/present depth/foreground owner，避免开启大 trace。

## 追加进度：第二轮修正仍未通过

- 已把直接写屏 damage 细化到行级，并将“无显式 present”路径改为优先提交 damage 行而非整屏；`make -C build -j$(nproc)` 通过。
- 手动 E2E 目录：`/tmp/skyengine-opbzqe-debug/manual-fix-1782125565`。
  - `(110,27)=(0,0,0)`、`(120,20)=(0,0,0)`、`top_black=9600`。
  - 下方菜单仍正常：`(113,124)=(184,192,48)`。
- 这说明顶部黑行仍被某条后续提交路径刷到 SDL；下一步只记录合成/脏行提交的矩形范围和顶部直接写屏 owner，判断是不是 active child 自己未提交的 top damage 被合成出来。

## 追加进度：第三轮直接写屏可见性修正仍未通过

- 时间：2026-06-22 18:57:06 CST。
- 已将 `hook_screen_write()` 改为：存在独立 foreground child 时，ARM screen cache 的直接写入不再标记 `screen_dirty`/damage，避免共享 framebuffer 内存写入被模拟器自动提交到 SDL。
- `make -C build -j$(nproc)`：通过。
- `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：仍失败，`wake.pixel(110,27)` 仍为 `[0,0,0]`。
- 结论：黑色覆盖不只来自 `hook_screen_write()` 自动 present，或者前台快照/host shadow 已经在红色 advbar 提交后被黑色污染。
- 下一步增加窄口径临时诊断：记录 accepted `mr_drawBitmap`/`DispUpEx` 的矩形和顶部像素、`leave_screen_context()` 前后 foreground snapshot/ARM screen/host shadow 的关键像素，不开启全量 trace。

## 追加进度：顶部修复后下方区域变黑

- 时间：2026-06-22 19:07 CST。
- 将 `screen_dirty` 收窄为“需要执行器合成提交的 screen-cache 脏区”后，顶部 advbar 红色区域已能保留：
  - 手动 PPM 目录：`/tmp/skyengine-opbzqe-debug/manual-diag4-1782126304`。
  - `(110,27)=(128,48,40)`、`(120,20)=(176,120,120)`、`(200,20)=(232,200,168)`。
- 新问题：下方游戏区域变黑，`(113,124)=(0,0,0)`，说明之前的下方正常画面依赖 screen-cache damage 自动提交/或 advbar 从共享 screen cache 代理刷新。
- 语义修正方向：
  - `mr_drawBitmap` / `DispUpEx` 是显式可见 present，必须按 foreground owner 接受/拒绝。
  - `_DrawBitmap`、`DrawRect`、`_DrawText`、直接写 ARM screen cache 只是 backing cache 写入，不能因为前台插件存在就完全忽略；否则 advbar 提交 `y=40..319` 时读不到底层游戏画面。
  - 需要持久追踪 foreground child 已显式 present 的可见覆盖区域；缓存写入只在未被 foreground 覆盖的行段自动提交。这样既保留顶部 advbar，又不阻断下方游戏区域。

## 追加进度：继续排查下方黑屏

- 时间：2026-06-22 19:19:46 CST。
- 当前代码已能保留顶部 advbar 红色广告条，但目标测试仍失败：`(113,124)` 下方游戏区域为黑色。
- 手动诊断显示 `drawBitmap bmp=0x2001BC x=0 y=40 w=240 h=280` 的源像素已经正确，`srcMid=B8,C0,30` 对应 `[184,192,48]`；失败点是这次显式 present 没有进入 `guiDrawBitmap()`。
- 代码复读发现 `arm_ext_should_accept_screen_write()` 主要通过 LR 所属代码段判定调用者。advbar 代理提交下方游戏区域时，运行日志中 LR 可能落在 primary/helper veneer，导致 foreground child 的可见提交被误判成 primary 并拒绝。
- 下一步用窄口径诊断打印 PC/LR/current/R9 推断出的 owner，确认是否应改用“当前执行代码归属”来判定 `mr_drawBitmap`/`DispUpEx` 的前景所有权。

## 最终修复

- 时间：2026-06-22 19:28:45 CST。
- 窄口径诊断确认：
  - advbar 顶部广告条先通过 foreground child 显式提交 `x=0,y=0,w=240,h=40`，源像素为红色广告条。
  - 下方游戏区域通过 `x=0,y=40,w=240,h=280` 显式提交，源像素已经正确，`(113,124)` 源值为 `[184,192,48]`。
  - 但这次下方提交的 LR/R9 指向 primary veneer/RW，旧 `arm_ext_should_accept_screen_write()` 将其拒绝，因此 SDL 没收到下方游戏画面。
- 修复点：
  - 将“screen-cache 写入是否可见”和“显式 present 是否可见”分离。
  - `_DrawBitmap`、`DrawRect`、`_DrawText`、直接 ARM screen buffer 写入仍只作为 backing cache 变更处理；foreground child 存在时不自动提交到 SDL。
  - `mr_drawBitmap` / `DispUpEx` 作为显式 present 处理：如果已有 foreground child 覆盖区域，则允许 covered layer 的显式 present 进入裁剪路径，只提交未被 foreground cover 占用的行段。这样顶部 advbar 不会被 primary 覆盖，下方游戏区域也能正常显示。
  - foreground child 显式 present 时记录持久 cover 行段；active child 切换或前台 owner 清理时同步清空。
  - callback 退出时删除“无 damage 则整屏提交”的兜底路径，只提交已经记录到行级 damage 的区域。
- 反汇编证据保持不变：
  - `advbar.ext` 在约 `0x608..0x620` 的循环直接写 RGB565 到共享 screen cache，可解释“广告条出现后 top cache 可能被黑色写入”，但直接写 cache 不应自动覆盖 foreground 可见层。
  - `opbzqe game.ext` 在 `0x9FAE..0x9FB4` 明确提交 `r0=0,r1=40,r2=240,r3=280`，说明下方区域局部 present 是程序真实语义，不是测试特判。
- 最终 PPM：`/tmp/skyengine-opbzqe-debug/final-ppm-1782127669/menu.ppm`
  - `(110,27)=[128,48,40]`
  - `(120,20)=[176,120,120]`
  - `(200,20)=[232,200,168]`
  - `(113,124)=[184,192,48]`
  - 顶部仅 14 个黑像素（广告图像自身边缘/内容），不是整条黑区。
- 验证：
  - `make -C build -j$(nproc)`：通过，无编译 warning；清理注释和移除整屏兜底后再次通过。
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过；清理注释和移除整屏兜底后再次通过。
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts`：通过。
  - `pnpm vitest run test/e2e/gxdzc-pixel.test.ts`：通过。

## 追加进度：方向键后广告条未消失

- 时间：2026-06-22 19:56:07 CST。
- 用户追加用例：进入主菜单后按 LEFT/RIGHT，顶部广告条应消失。
- 先发现测试本身还在检查按键前的 `wake` 截图对象，需要按键后重新 `screen()` 才能验证新画面；但手动 PPM 重新截图确认真实画面也仍保留广告条，不只是测试问题。
- 手动 PPM 目录：`/tmp/skyengine-opbzqe-debug/right-key-1782129300`。
  - 按键前 `(110,27)=[128,48,40]`、`(113,124)=[184,192,48]`。
  - 按 RIGHT 后重新截图 `(110,27)=[128,48,40]` 仍是广告条；下方 `(10,41)` 已变化，说明游戏菜单收到了方向键并重绘，但顶部 foreground cover 仍在裁剪 primary 的顶部刷新。
- 窄口径日志显示方向键事件后 `activeP=0x7021D8 activeH=0x68D1F1` 仍保持为 advbar child，`modalDepth=0` 没有普通 suspend/resume close edge。下一步需要找出 wrapper/child 的通用“前景覆盖结束”信号，而不是按键或应用名特判。

## 追加进度：继续定位前景覆盖生命周期

- 时间：2026-06-22 20:20 CST。
- 代码复读确认当前 foreground cover 的生命周期只有两个入口：
  - foreground child 的显式 `mr_drawBitmap` / `DispUpEx` 会把对应行段累加到 `foreground_cover_*`。
  - active child 切回 primary、模态关闭、或 reset child modules 时才清理 cover。
- 这与新现象吻合：RIGHT/LEFT 事件后游戏下方重绘成功，但 active child 仍登记为 advbar，因此旧 cover 继续裁剪 primary 的顶部刷新。
- 下一步增加仅在 `VMRP_ARM_EXT_DIAG=1` 下启用的窄口径状态日志，记录按键前后 `active/primary/timer`、active/primary 的 `P->mrc_extChunk` 字段、cover 行段和 visible present 的 owner/rect；用这些日志找通用清理信号，修完后移除临时诊断。

## 追加进度：本轮复现入口

- 时间：2026-06-22 20:43 CST。
- 重新运行 `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`，失败点仍是按 RIGHT 后顶部像素保持 `[128,48,40]`。
- 同时发现测试代码当前直接检查按键前的 `wake` 截图对象，测试断言本身不会读取新画面；但上一轮手动 PPM 已提示真实 SDL 画面也可能保留广告条，所以本轮先用 E2E socket 在按键后重新导出 PPM，再判断是测试问题还是前景层释放问题。

## 追加进度：按键后真实 PPM 与状态日志

- 时间：2026-06-22 20:52 CST。
- 手动 E2E 重新导出按键后的新 PPM，目录 `/tmp/skyengine-opbzqe-manual-NQXAi1`：
  - 按键前：`(110,27)=[128,48,40]`、`(113,124)=[184,192,48]`。
  - 按 RIGHT 后：`(110,27)=[128,48,40]`、`(113,124)=[184,192,48]`、`(10,41)=[88,104,40]`。
  - 结论：真实 SDL 画面仍保留顶部广告条；测试未重取截图是额外问题，但不是唯一问题。
- 窄口径诊断目录 `/tmp/skyengine-opbzqe-diag-V6b55L`，只启用 `VMRP_ARM_EXT_DIAG=1`，未开启大 trace。
- 日志要点：
  - advbar 仍登记为 active child：`activeP=0x7021D8 activeH=0x68D1F1`，`active[34]=0`，不是普通 suspend/resume 模态关闭路径。
  - 按键事件进入 `arm_ext_call code=1 input_len=20` 后，primary/game 后续确实做了全屏提交：`present29 x=0 y=0 w=240 h=320`。
  - 此时旧前景覆盖仍为 `cover27=0..240`，`covered=1`，所以 primary 的全屏提交被裁剪掉顶部 40px，SDL surface 保留旧 advbar 像素。
- 下一步：反汇编确认按键后的全屏 present 是 game.ext 的真实行为，并寻找通用规则释放非模态前景 cover；不能写方向键或 opbzqe 特判。

## 追加进度：修复与初验

- 时间：2026-06-22 20:54 CST。
- 反汇编修正：`.ext` 文件是 gzip 压缩流，已先解压到 `/tmp/skyengine-opbzqe-disasm/*.raw` 后再反汇编；先前直接对压缩流反汇编的垃圾指令作废。
- 解压后 `game.ext.raw` 中日志 LR `0x64F301` 落在 `0x64F2D0..0x64F300` 的 Thumb wrapper，最终 `blx r7` 调用函数表 present；运行日志中的 `present29 x=0 y=0 w=240 h=320` 是 game 层真实显式提交。
- 修复点：在 `src/arm_ext_executor.c` 中按 `P->mrc_extChunk[0x34]` suspend depth 区分前景覆盖：
  - `extChunk[0x34] > 0`：模态/暂停子层，继续保持遮挡，直到 wrapper resume 清理。
  - `extChunk[0x34] == 0`：非模态 overlay。若下层随后显式 present 一个与 overlay cover 相交的矩形，则相交 cover 行段退场，让下层画面重新可见。
- 这不是方向键或样本特判；触发条件只依赖通用 EXT 模态深度与显式 present 行段。
- 同步修正测试：按 RIGHT 后重新抓 `after-right` PPM，避免继续检查按键前的 `wake` 截图对象。
- 构建说明：`make -C build` 走到旧 Makefile，仍引用已不存在的 `src/app_compat_gxdzc.c`；当前 `build` 目录是 Ninja 配置，使用 `cmake --build build --target skyengine` 成功。
- `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
- 手动 PPM 复验目录 `/tmp/skyengine-opbzqe-verify-m2hUwd`：
  - 按键前：`(110,27)=[128,48,40]`、`(113,124)=[184,192,48]`、`(10,41)=[64,96,24]`。
  - 按 RIGHT 后：`(110,27)=[232,144,176]`、`(113,124)=[184,192,48]`、`(10,41)=[96,104,40]`。
  - 结论：广告条消失，菜单区域保持可见且方向键触发了菜单画面变化。

## 追加进度：最终校验

- 时间：2026-06-22 21:02 CST。
- 代码收紧：只有在 active child 的 `P->mrc_extChunk[0x34]` 可读且值为 0 时，才把 foreground cover 视为非模态 overlay 并允许下层显式 present 释放相交行段；字段不可读或 suspend depth 非 0 时继续保持 cover，避免把未知状态当作可释放路径。
- 测试补强：`test/e2e/opbzqe/game-prepare.test.ts` 按 RIGHT 后重新抓 `after-right` PPM，并同时检查 `(110,27)` 不再是广告条颜色、`(10,41)` 相对按键前发生变化，覆盖“广告条消失”和“菜单能切换”两个预期。
- 验证结果：
  - `cmake --build build --target skyengine`：通过。
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过；补强菜单像素断言后再次通过。
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts test/e2e/gxdzc-pixel.test.ts`：通过。
  - `ctest --test-dir build --output-on-failure`：前 4 个本地测试通过；第 5 项 `e2e_vitest` 被 CTest 60.06 秒超时截断，因此 CTest 总体返回失败。这是整套 `pnpm vitest run test/e2e` 的 CTest 超时边界，目标 opbzqe 用例和本次相关 e2e 已单独通过。

## 追加进度：优化重构入口

- 时间：2026-06-22 21:11 CST。
- 目标：在不改变 advbar/top-cover 行为的前提下，压缩 `src/arm_ext_executor.c` 中繁琐的屏幕层变更。
- 只读复查后的优化点：
  - `foreground_screen_snapshot` 当前只承担 owner metadata 语义，像素缓冲不再回写，可移除整屏分配和 merge 复制路径。
  - damage、present、foreground cover 三套 `min_x/max_x/rows` 是同一种行区间模型，可合并为 `ArmExtRowSpans`，减少重复 ensure/clear/mark/has 代码。
  - `mr_drawBitmap` 与 `_DispUpEx` 的 cover 裁剪循环后续可继续合并；第一步先做低风险的数据结构收束。

## 追加进度：第一轮结构收束

- 时间：2026-06-22 21:15 CST。
- 已完成：
  - 在 `src/include/arm_ext_internal.h` 中新增 `ArmExtRowSpans`，把 damage、present、foreground cover 三组行区间字段收束为结构体。
  - 在 `src/arm_ext_executor.c` 中新增通用 `arm_ext_ensure_row_spans` / `arm_ext_clear_row_spans` / `arm_ext_has_row_spans` / `arm_ext_mark_row_spans`，替代重复的 min/max 数组管理。
  - 将 `foreground_screen_snapshot` 的整屏像素缓冲移除，保留 `foreground_screen_owner_*` 和 valid 标记作为可见层 owner metadata；实际可见保护仍由 foreground cover 行区间承担。
- 兼容性说明：本轮不改变显式 present、cache-only write、非模态 cover 释放的触发条件，只减少重复状态和不再使用的整屏复制路径。
- `cmake --build build --target skyengine`：通过。

## 追加进度：第二轮去重与验证

- 时间：2026-06-22 21:20 CST。
- 继续优化：
  - 将 `foreground_screen_snapshot_valid` 重命名为 `foreground_screen_owner_valid`，避免继续暗示存在整屏像素快照。
  - 将 `mr_drawBitmap` 和 `_DispUpEx` 的 foreground-cover 裁剪循环合并到 `arm_ext_submit_uncovered_present_segments()`；两个 present API 只保留各自的 segment submit 小函数。
  - 将旧 `arm_ext_merge_foreground_screen_*` helper 改为 owner claim 语义，避免“merge 像素”与实际行为不一致。
- 兼容性说明：裁剪仍按 foreground cover 行区间执行；非模态 overlay 仍只在 active child 的 `extChunk[0x34]` 可读且为 0 时，允许下层显式 present 释放相交 cover。没有加入样本、方向键或应用名特判。
- 验证结果：
  - `cmake --build build --target skyengine`：通过。
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts test/e2e/gxdzc-pixel.test.ts`：通过。
  - `ctest --test-dir build --output-on-failure`：前 4 个本地测试通过；第 5 项 `e2e_vitest` 仍被 CTest 60.07 秒超时截断，和优化前观察一致。目标 opbzqe 用例与本次相关 e2e 已单独通过。

## 追加进度：最终复验

- 时间：2026-06-22 21:25 CST。
- 命名和状态清理后复验：
  - 删除冗余的 `foreground_screen_owner_valid`，诊断中的 valid 直接由 `foreground_screen_owner_p_addr != 0` 推导，减少 owner 地址与 valid 标志不一致的风险。
  - `cmake --build build --target skyengine`：通过。
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts test/e2e/gxdzc-pixel.test.ts`：通过。
  - `ctest --test-dir build --output-on-failure`：前 4 个本地测试通过；第 5 项 `e2e_vitest` 仍被 CTest 60.06 秒超时截断。
- 额外检查：确认 `foreground_screen_snapshot`、旧 `screen_region_rows`、旧 `*_min_x/max_x` 字段引用已从 `src/arm_ext_executor.c` 和 `src/include/arm_ext_internal.h` 清除；保留的 `snapshot` 命名仅属于 GOT/modal 现有机制。

## 追加进度：移除 CTest 入口

- 时间：2026-06-22 22:17 CST。
- 按“只保留 Vitest”要求清理：
  - `CMakeLists.txt` 删除 `VMRP_BUILD_TESTS`、`enable_testing()`、C 单测可执行文件、shell 集成脚本注册和 `e2e_vitest` 包装注册；CMake 现在只负责构建 emulator/library 目标。
  - `docs/TESTING.md` 改为 Vitest-only 测试说明。
  - `AGENTS.MD` 的构建与验证要求改为 `cmake --build build --target skyengine` 加相关 Vitest e2e。
  - 清理当前 `build/` 目录里旧 `CTestTestfile.cmake` 和旧 `test_skyengine*` 二进制后重新配置，避免本地验证继续读取历史生成物。
- 当前入口检查：
  - `rg -n "SKYENGINE_BUILD_TESTS|enable_testing|add_test|test_skyengine|skyengine_.*tests|e2e_vitest|ctest|CTest" CMakeLists.txt src docs/TESTING.md package.json test/e2e AGENTS.MD`：无命中。
  - `ctest --test-dir build -N`：`Total Tests: 0`，确认当前 build 目录不再注册测试。
- 验证结果：
  - `cmake -S . -B build`：通过。
  - `cmake --build build --target skyengine`：通过。
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts`：通过。
  - `pnpm vitest run test/e2e/dota/download-plugin.test.ts test/e2e/gxdzc-pixel.test.ts`：通过。
- 剩余风险：
  - `pnpm test:e2e` 直接 Vitest 全量运行时卡在 `build/skyengine --work-dir . mythroad/gghjt.mrp`，超过 3 分钟无输出后已终止并清理残留进程。这是 gghjt 全量场景自身挂起问题，不再是 CTest 的 60 秒包装超时。
