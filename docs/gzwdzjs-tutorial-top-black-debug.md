# gzwdzjs 教程进入后上半屏黑屏调试记录

## 现象

- 用例:`pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程`
- 流程:启动(`--memory 2M`) -> 关音乐 -> 主菜单 -> 教程确认 -> `LEFT_SOFT` -> 进入介绍画面 -> 再按 2 次 `ENTER`
- 预期:`game-start.ppm` 上半部分正常显示图像,`pixel(75,75)` 非黑。
- 实际:`game-start.ppm` 的 `y=0..199` 全黑,`y=200..319` 有底部场景/状态栏图像。

## 2026-07-06 22:24 当前工作树复现

- 当前 `git status --short` 仅显示:
  - `docs/prompt.md`
  - `test/e2e/gzwdzjs/game-start.test.ts`
- `src/` 无工作树 diff;当前运行时行为来自已构建源码,不是未落盘 src 改动。
- 命令:
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
- 结果:失败于 `test/e2e/gzwdzjs/game-start.test.ts:87`,
  `screen.pixel(75,75) == [0,0,0]`。
- 保留目录:`/tmp/vmrp-e2e-EUtFlT`。
- PPM 证据:
  - `start-confirm.ppm`: `unique=240`,上半屏非黑 `38400`,下半屏非黑 `36904`,
    `(75,75)=[232,184,40]`。
  - `introduce.ppm`: 同样全屏正常,`(75,75)=[232,184,40]`,
    `(94,145)=[208,244,200]`。
  - `game-start.ppm`: `unique=36`,上半屏非黑 `0`,下半屏非黑 `27318`,
    非黑行范围 `200..319`;`(75,75)=[0,0,0]`,
    `(120,200)=[136,92,8]`,`(75,250)=[96,60,0]`。
- stdout/stderr 未出现 `UC_MEM_READ_UNMAPPED`、`PC=0x23BACE` 或
  `uc_emu_start(...) failed`;这不是旧的定时器链表崩溃/读取中卡死。

## 2026-07-06 22:31 分步复现

- 手动脚本在 `LEFT_SOFT + 5s` 后继续按两次 `ENTER`,分别保存
  `post-enter-1.ppm` 和 `post-enter-2.ppm`。
- 输出目录:`/tmp/gzwdzjs-post-enter-diag`。
- PPM 结果:
  - `start.ppm`:全屏正常,`top=38400`,`bottom=36904`,
    `(75,75)=[232,184,40]`。
  - `post-enter-1.ppm`:全屏仍正常,`top=38400`,`bottom=36904`,
    `(75,75)=[232,184,40]`。
  - `post-enter-2.ppm`:复现黑屏,`top=0`,`bottom=27318`,
    非黑行范围 `200..319`,`(75,75)=[0,0,0]`。
- 结论:黑屏由第二次 `ENTER` 后的教程/游戏场景切换触发,不是
  `LEFT_SOFT` 后加载阶段造成。

## 2026-07-06 22:35 窄口径屏幕诊断

- 使用 `VMRP_ARM_EXT_SCREEN_DIAG=1 VMRP_ARM_EXT_DIAG=1`,未开全量 trace。
- 诊断显示最后阶段的显式 present 仍由 primary `game.ext`
  (`ownerP=0x2BC3DC`,`ownerH=0x23B38D`)发起:
  - `present29 x=0 y=0 w=240 h=320 accept=1 covered=0`
  - `present29 x=0 y=100 w=240 h=130 accept=1 covered=0`
- `foreground_cover` 为空(`cover27=65535..0`),`fgOwnerP=0`,因此当前证据
  不支持 foreground child cover 裁剪导致 top 行丢失。
- 下一步:反汇编 `game.ext` 中 `lr=0x23B5B1` 的 table[29] present wrapper
  和第二次 `ENTER` 后的场景绘制路径,证明黑屏是 guest 侧只提交/只绘制
  下半屏,还是模拟器对 full-screen source/stride/屏幕缓存的解释错误。

## 2026-07-06 22:55 根因与修复

- table[29] present 入口/出口采样显示,最终全屏 present 源和 ARM screen cache
  在 `(75,75)`,`(120,160)`,`(120,200)` 都已经是正确非黑像素;黑屏不是
  guest 未绘制,也不是 present source/stride 取错。
- `src/main.c::guiDrawBitmapWithStride()` 临时采样显示,最终截图前 SDL surface
  又收到来自宿主 shadow buffer 的行级回放:
  - `rect=0,75 240x1` 和 `rect=0,160 240x1` 的源行为黑;
  - `rect=0,200 240x1` 的源行为非黑。
  这与 `game-start.ppm` 上半屏黑、`y>=200` 正常完全吻合。
- damage 采样定位到第二次 `ENTER` 后的 `table[122] DrawRect(0,0,240,320,0,0,0)`,
  `lr=0x23A62F`。该调用把 primary screen cache 整屏清黑,后续应用只局部重绘并
  显式 present。旧逻辑把整屏黑 clear 记为可见 damage,callback 退出时又按行
  合成 present,于是暴露了应用没有显式提交的 cache reset 像素。
- 修复点:`src/arm_ext_executor.c` 的 table[122] 仍执行 `DrawRect`,保持 backing
  screen cache 语义;但当目标是 primary 且矩形覆盖全屏且颜色为黑时,不把该
  clear 记为 screen damage/foreground claim,也不触发 callback-exit 可见合成。
  这不是 app 专用分支,也不是忽略黑色绘制;如果应用显式 present 该黑屏,仍会
  通过正常 present 路径显示。

## 2026-07-06 22:58 验证

- 构建:
  `cmake --build build --target vmrp -j2`
- focused:
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
  通过,保留目录 `/tmp/vmrp-e2e-afKb8y`。
- focused PPM:
  - `game-start.ppm`: `240x320`,`unique=84`,
    上半屏非黑 `48000`,下半屏非黑 `27318`,非黑行范围 `0..319`;
  - `(75,75)=[232,184,40]`,`(120,160)=[208,244,200]`,
    `(120,200)=[136,92,8]`,`(75,250)=[96,60,0]`。
- 同文件回归:
  `pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts --reporter=verbose`
  通过,`游戏教程开始` 和 `花屏检查` 均通过。
- 相关渲染回归:
  - `pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts test/e2e/dota/download-plugin.test.ts test/e2e/gxdzc-pixel.test.ts --reporter=verbose`
    中实际存在的 `opbzqe` 与 `dota` 两个文件通过;
  - `pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts --reporter=verbose`
    通过。`gxdzc` 的正确路径包含 `gxdzc/` 子目录。
