# cookie 首次左软键调试记录

## 2026-07-14 当前复现

- 目标用例：`test/e2e/cookie/run-mrp.test.ts`。
- 用户现象：第一次左键不通过，失败最终落在 `test/e2e/skyengine-e2e.ts` 的 `else reject(new Error(line || ...))`。
- 本地命令：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose`。
- 当前保留目录：`/tmp/skyengine-e2e-uydw68`，工作区：`/tmp/skyengine-ws-Mr8BVZ`。
- 本地失败响应是 `ERR wait_draw_timeout current=8 target>8`，不是空响应。`menu-first.ppm` 已生成，说明第一次 `LEFT_SOFT` 已打开菜单，当前卡住点更接近下一次 `UP` 后等待绘制。

## 已确认链路

- `skyengine.key('LEFT_SOFT', 1_000, 250)` 会先发 `DRAW_COUNT`，再发 `KEY LEFT_SOFT 250`，最后发 `WAIT_DRAW <previous> 1000`。
- `src/e2e_control.c` 将 `LEFT_SOFT` 解析为 `SDLK_EQUALS`，注入 `SDL_KEYDOWN`，等待 hold 时间后注入 `SDL_KEYUP`，然后写回 `OK key ...`。
- `src/main.c` 将 `SDLK_EQUALS` 转成 `MR_KEY_SOFTLEFT`，通过 `event(MR_KEY_PRESS/RELEASE, MR_KEY_SOFTLEFT, 0)` 进入 Mythroad/ARM EXT。
- `draw_count` 只在 `guiDrawBitmapWithStride()` 提交 host bitmap 时递增；它不是“guest 已消费输入”的直接证明。

## PPM 证据

- `/tmp/skyengine-e2e-uydw68/home.ppm` 与 `/tmp/skyengine-e2e-uydw68/screen.ppm` 的 SHA-256 相同：`8ed68d8d917ddf1a9ef3235552e2a5e1a4ff100943e5fdc1cbe0c606a2ba00c8`。
- `/tmp/skyengine-e2e-uydw68/menu-first.ppm` 的 SHA-256：`3221f0a9f9249f65566093e42e92628f8b7ffba5bf8f6043521d066eb2044577`。
- 早期手动目录 `/tmp/skyengine-cookie-protocol-B5FPfX` 的 `after-left.ppm` 与 `after-up.ppm` 实际相同；只有前一 release 跨过 guest timer 调度边界后，后续 `UP` 才会改变画面。

## 下一步

- 用更精确的手动 socket 复现记录每个 `DRAW_COUNT`、`KEY`、`WAIT_DRAW` 的响应，确认失败是否由 e2e 的 `waitForDraw` 语义过窄触发。
- 反汇编 `cookie_v6110.mrp` 内的 EXT/资源，定位事件处理与绘制提交路径，避免用测试特化逻辑绕过问题。
- 如果修复测试助手等待语义，必须保持非场景化，并验证其它 e2e 用例不被放宽成“兜底通过”。

## 2026-07-15 复现校正

- 显式使用 `SDL_VIDEODRIVER=dummy VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose` 复现；产物目录为 `/tmp/skyengine-e2e-rIfKIh`。
- 本次仍失败于 `ERR wait_draw_timeout current=8 target>8`。`home.ppm` 在 09:14:39.296 生成，`menu-first.ppm` 在 09:14:39.620 生成，且两张图 SHA-256 不同，证明第一次 `LEFT_SOFT` 的 `KEY`、`WAIT_DRAW` 和随后菜单像素断言均已完成。
- 因此当前可重复失败点不是第一次 `LEFT_SOFT` 本身，而是它后面的 `UP`：助手在按键前读到 `draw_count=8`，按键后 1000ms 内计数没有大于 8。异常统一落在 `command()` 的非 `OK` 分支，容易从堆栈误判为第一条左软键失败。
- 协议步进现场 `/tmp/skyengine-cookie-protocol-B5FPfX` 同时保留了 `home.ppm`、`after-left.ppm` 和 `after-up.ppm`。后两张 SHA-256 均为 `8c8cfc0db7171ac19584cf9bee6df2df386f14840abdf396b4a33e1439efa2c`，说明原始连续 `UP` 没有改变可见状态。

### 当前约束

- 不修改用例去设置 `waitForDraw: false`，因为这会取消输入完成的同步语义，属于绕过而非修复。
- 不添加 cookie/MRP 名称判断；修复判据必须来自输入事件、guest 调度或真实绘制提交的通用状态。
- 后续运行继续显式使用 `SDL_VIDEODRIVER=dummy`，不使用 `xvfb`；ARM trace 仅针对候选地址做窄范围采样，避免生成不可控日志。

## 2026-07-15 根因与反汇编

- ARM 客户程序没有在首次左软键后退出、阻塞或异常。`game.ext` 运行基址为 `0x226110`、长度 `0x40B24`，P 为 `0x2BC4EC`，Thumb helper 为 `0x25D189`（文件偏移 `0x37078`）。
- helper 的 code=1 分支 `0x25D1CC` 读取事件三元组后进入 `0x25BFAC`。首次软左键 PRESS 以 `(type=0,key=17)` 经过 `0x25C148 -> 0x244498`，RELEASE 以 `(type=1,key=17)` 经过 `0x25C198 -> 0x244498`；`0x244498` 再把 `0x3004/0x3005` UI 事件分发到 widget。两条路径均正常返回，没有非法访存或 Unicorn 执行错误。
- 窄 DIAG 证明 `LEFT release` 后紧接着就是下一条 `UP press`，两者之间没有 code=2 timer dispatch。宿主 `isKeyDown` 已在 release 后清零，说明不是 SDL latch 丢键，而是 guest 状态机尚未跨调度周期。
- 间隔量化显示 1..40ms 都不改变 PPM，跨过下一次真实 timer dispatch 后 `UP` 才产生 5192 个差异像素。固定 500ms 默认值还会制造方向键重复，因此不能用加大 hold 或固定 sleep 解决。

## 2026-07-15 通用修复

- 单纯等待“任意一次 timer epoch”在压力下仍会失败：`/tmp/skyengine-e2e-SU4h7C` 在 `draw_count=14` 后不再绘制，`/tmp/skyengine-e2e-yubNpH` 在 `draw_count=8` 后失败。原因是刚消费的 timer 事件可能早在按键前就已排队，epoch 增加不能证明它属于按键后的调度周期。
- `src/main.c` 现在为每次 `timerStart()` 分配递增 generation，并把 generation 随 SDL timer 事件带回主线程。`timerDispatchedGeneration` 只在 `timer()` 完整返回后发布；`timerPendingGeneration` 和 `timerDispatchInProgress` 区分真实停止与 `timer()` 内部短暂的 stop/start。
- 默认短按的 KEYDOWN 由 SDL 队列进入主线程；首个后续 guest timer 完整返回后，主线程在处理下一条 SDL 事件前同步执行 KEYUP。这样 guest 恰好观察一个按下 tick，同时避免控制线程唤醒延迟把方向键变成长按重复。
- release 回执记录当时的 arm/pending generation，KEY 响应要等到严格晚于该快照的 generation 完整 dispatch，或观察到真实 timer 停止/runtime 退出。显式 `holdMs` 仍表示物理长按时长，并保留主线程回执和 release 后边界。
- 删除所有候选固定 60ms/250ms gap。等待循环中的 `SDL_Delay(1)` 只让出控制线程 CPU，完成条件完全来自 generation、timer 状态和 runtime 终态。
- 按键若成功触发 `skyengine_request_exit()`，服务端返回 `OK key <code> exited`。TypeScript 助手仅对这个显式终态跳过 `WAIT_DRAW`；其它 ERR、空响应、超时和无绘制行为仍按原契约失败。

## 2026-07-15 范围校正与验证

- 用户明确 `test/e2e/cookie/run-mrp.test.ts` 尚未写完，本次不定义或修正场景预期。调试期间临时加入过像素/区域断言，现已全部撤销；该文件相对 `HEAD` 无差异。
- 调试运行始终显式使用 `SDL_VIDEODRIVER=dummy`，未使用 `xvfb`。保留 PPM 证明首页和 `menu-first` 不同，且第一次左软键后 `(94,166)=[248,192,32]`，所以输入链和第一次菜单绘制成立；后续未完成场景不作为验收结论。
- 最终生产代码通过 `SDL_VIDEODRIVER=dummy cmake --build build --target skyengine -j2`、`SDL_VIDEODRIVER=dummy ctest --test-dir build --output-on-failure`（1/1）和 `SDL_VIDEODRIVER=dummy pnpm exec tsc --noEmit`。
- 没有把 cookie/MRP 名称、像素坐标或场景步骤写入生产同步逻辑，也没有捕获 `WAIT_DRAW` 错误、关闭 `waitForDraw` 或添加固定 sleep 作为兜底。
