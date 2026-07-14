# cookie 首次左软键调试记录

## 2026-07-14 当前复现

- 目标用例：`test/e2e/cookie/run-mrp.test.ts`。
- 用户现象：第一次左键不通过，失败最终落在 `test/e2e/vmrp-e2e.ts` 的 `else reject(new Error(line || ...))`。
- 本地命令：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose`。
- 当前保留目录：`/tmp/vmrp-e2e-uydw68`，工作区：`/tmp/vmrp-ws-Mr8BVZ`。
- 本地失败响应是 `ERR wait_draw_timeout current=8 target>8`，不是空响应。`menu-first.ppm` 已生成，说明第一次 `LEFT_SOFT` 已打开菜单，当前卡住点更接近下一次 `UP` 后等待绘制。

## 已确认链路

- `vmrp.key('LEFT_SOFT', 1_000, 250)` 会先发 `DRAW_COUNT`，再发 `KEY LEFT_SOFT 250`，最后发 `WAIT_DRAW <previous> 1000`。
- `src/e2e_control.c` 将 `LEFT_SOFT` 解析为 `SDLK_EQUALS`，注入 `SDL_KEYDOWN`，等待 hold 时间后注入 `SDL_KEYUP`，然后写回 `OK key ...`。
- `src/main.c` 将 `SDLK_EQUALS` 转成 `MR_KEY_SOFTLEFT`，通过 `event(MR_KEY_PRESS/RELEASE, MR_KEY_SOFTLEFT, 0)` 进入 Mythroad/ARM EXT。
- `draw_count` 只在 `guiDrawBitmapWithStride()` 提交 host bitmap 时递增；它不是“guest 已消费输入”的直接证明。

## PPM 证据

- `/tmp/vmrp-e2e-uydw68/home.ppm` 与 `/tmp/vmrp-e2e-uydw68/screen.ppm` 的 SHA-256 相同：`8ed68d8d917ddf1a9ef3235552e2a5e1a4ff100943e5fdc1cbe0c606a2ba00c8`。
- `/tmp/vmrp-e2e-uydw68/menu-first.ppm` 的 SHA-256：`3221f0a9f9249f65566093e42e92628f8b7ffba5bf8f6043521d066eb2044577`。
- 手动 socket 步进目录 `/tmp/vmrp-cookie-manual-230914` 中，`after-left.ppm` 显示菜单第一项高亮，`after-up.ppm` 显示菜单末项高亮，说明 `UP` 输入本身也会改变画面。

## 下一步

- 用更精确的手动 socket 复现记录每个 `DRAW_COUNT`、`KEY`、`WAIT_DRAW` 的响应，确认失败是否由 e2e 的 `waitForDraw` 语义过窄触发。
- 反汇编 `cookie_v6110.mrp` 内的 EXT/资源，定位事件处理与绘制提交路径，避免用测试特化逻辑绕过问题。
- 如果修复测试助手等待语义，必须保持非场景化，并验证其它 e2e 用例不被放宽成“兜底通过”。
