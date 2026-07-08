# gzwdzjs 花屏调试记录

## 现象

- 用例:`pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 花屏`
- 流程:启动(--memory 2M) → 关音乐 → 主菜单 → 4 下 ENTER → 教程对话场景
- 预期:屏幕底部(对话框以下)应为"部分黑屏",门板图案延伸到 x≈128 (pixel(128,276)=rgb(96,60,0))
- 实际:门板只画到 x≈76,右侧黑色区域有零星彩色噪点(花屏),pixel(128,276)=(0,0,0)

## 环境信息

- gzwdzjs.mrp 含 game.ext / cfunction.ext / verdload.ext(ARM EXT 应用)
- 顶部对话区域渲染正常,仅底部场景区域异常
- stdout/stderr 无 arm_ext 报错;启动时有 payOneAsTlv 联网(正常流程)

## 进行中

- [ ] 手动复现(绕过 vitest,socket 直接驱动)
- [ ] 对比不同 --memory 档位的表现
- [ ] 反汇编 game.ext 定位绘制该区域的代码

## 2026-07-05 本轮接手记录

- 约束:不使用 xvfb;trace 日志量大,优先使用 PPM、窄口径环境变量和反汇编;修复必须是通用语义修复,不能写 gzwdzjs 专用分支;代码修改需要说明原因。
- 测试入口已确认:`test/e2e/gzwdzjs/game-start.test.ts` 的“花屏检查”启动 `gzwdzjs.mrp --memory 2M`,关音乐后进入主菜单,再按 4 次 ENTER,最后断言 `render-check.ppm` 的 `(128,276)` 应为 `rgb(96,60,0)`。
- 初步判断:该点位处于底部场景背景/门板区域,不是 E2E PPM 解析问题;需要把 PPM 像素形态、ARM EXT 绘制入口和 game.ext 反汇编对应起来。

## 2026-07-05 PPM/trace 进展

- `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 花屏 --reporter=verbose` 稳定失败,保留目录 `/tmp/vmrp-e2e-Al8lM6`,失败值为 `(128,276)=(0,0,0)`。
- PPM 视觉检查:`render-check.ppm` 的底部 `y>=230` 左侧门板只到 `x≈73`,右侧黑区散布短线噪点;`y=276` 上 `x=96/100/101/103/218` 等点为非黑,符合“部分花屏”。
- 已提取 `test/fixtures/gzwdzjs.mrp` 到 `/tmp/gzwdzjs-unpack`: `game.ext` 解压后 116492 字节,`MRPGCMAP` 后 `0xE80008` 为 ARM 入口;运行时 table[125] 从 RAM 源 `$` 解压 `abc` 到 `0x226118`,table[131] 注册为 primary game,`P=0x2BC3DC`,`H=0x23B38D`。
- 按步骤采样 `/tmp/gzwdzjs-steps`:第 3 次 ENTER 后 `(128,276)=(96,60,0)`;第 4 次 ENTER 后变为 `(0,0,0)`,且 `x=96` 出现 `(48,48,48)` 噪点。问题在第 4 次 ENTER 的场景/对话框切换期间产生。
- 过滤 trace `/tmp/gzwdzjs-trace-filter/stdout.filtered.log`:最后显示路径主要是 `table[120]` 在 `lr=0x23A45F` 对屏幕缓存画大量 6px 竖条,随后 `table[29](0x2001BC,0,100,240)` 局部 present 对话区域。`y=276` 没有被最后的局部 present 覆盖,来自更早的全屏 screen cache 内容。
- `VMRP_WATCH_WRITE=0x220720,0x220760 test/e2e/gzwdzjs/repro.sh /tmp/gzwdzjs-watch-pixel` 未捕获目标像素附近的 guest store,但最终 PPM 仍是 `(96,276)=(48,48,48)`,`(128,276)=(0,0,0)`,`(218,276)=(48,48,48)`。这把嫌疑从普通 ARM 写指令收窄到桥表内宿主 blitter/镜像/提交语义,尤其是 `table[120]` 的 `_DrawBitmap` 写屏缓存和 `table[29]` 的 present/mirror。
- 现有 trace 不打印 `table[120]` 的栈上传参(`h/rop/trans/sx/sy/mw`),下一步需要加窄口径临时诊断,只在目标像素可能被覆盖的 `table[120]`/`table[29]` 调用前后采样 screen cache 与实参。

## 2026-07-05 接手后的中间结论

- 已有补丁把 `table[120]` 从直接调用宿主 `_DrawBitmap(arm_ptr(...))` 改成按 guest bitmap allocation/table[95] 边界逐像素读取,可以阻止 `singlepea.bmp` 这类小 allocation 后续字节被误读成像素。
- 窄探针曾捕获异常调用:`lr=0x23A45F`,目标 rect=`74,123 26994x25972`,bitmap=`0x2BE7E4`,rop=`6`,sx=`30308`,sy=`28787`,mw=`78`;原实现会把 `(128,276)` 从 `0x61E0` 改成 `0x0000`。这证明噪点由 `table[120]` 的宿主 blitter 越界读写引入,不是普通 guest store。
- 但“越界源像素直接跳过”只让单点断言通过,会保留第 3 次 ENTER 的旧门板/按钮画面,使底部右侧本应黑掉的区域不黑。下一步必须继续追踪异常 `DrawBitmap` 参数上游,不能把跳过越界读当作最终视觉修复。
- 当前重点:反汇编 `0x23A420` `_DrawBitmap` wrapper 及其 `0x2370ac` 周边调用路径,用更窄探针打印异常调用时的寄存器、栈参数、bitmap allocation 边界和可疑对象字段。

## 2026-07-05 反汇编/窄探针进展

- `0x23A420` wrapper 的 ABI 已对齐:`table[120](p,x,y,w,h,rop,trans,sx,sy,mw)` 中 `w` 来自 wrapper 入栈参数 0,`h` 来自入栈参数 1,`rop` 来自入栈参数 2,`trans` 由 wrapper 从 `*p` 读取,`mw` 来自 wrapper 入参 r3。
- `0x2370AC` 的父帧中,`[sp,#28]` 是 bitmap 对象索引,`[sp,#32]` 是用于坐标表寻址的 incoming r3;ARM 指令按 32 位回绕计算 `scene_base + 0xA8C + r3*8`。
- 异常调用示例:`p=0x2BE7E4 rect=74,123 26994x25972 rop=6 sx=30308 sy=28787 mw=78`,对象 `0x24B5C4` 字段显示 bitmap 宽高为 `78x18`,allocation 正好是 `0x2BE7E4..0x2BF2DC`。
- 对应坐标表地址按回绕落到 `0x23EB68`,其字节为 `78 20 73 70 72 69 74 65 20 65 72 72 6F 72 00 00` (`"x sprite error"`),所以巨大 w/h/sx/sy 不是随机内存,而是上游把错误路径/字符串区当坐标半字表读了。
- 仅跳过越界源像素会消除噪点并让 `(128,276)` 保持门板色,但右侧应黑区域仍保留旧画面。下一步检查 `DrawRect/table[122]` 或其它清屏是否发生后又被 table[120] 重画。

## 2026-07-05 最终修复与验证

- 最终代码没有保留 `VMRP_ARM_EXT_PIXEL_PROBE`/`VMRP_ARM_EXT_BITMAP_PROBE` 临时探针,也没有加入 app 专用分支或黑色兜底填充。
- 通用修复:
  - 跟踪 table[0]/[2]/[125] 返回给应用的 live allocation,并在注册为 EXT 模块代码后从 live allocation 中剔除代码区。
  - 将 table[120] 从直接调用宿主 `_DrawBitmap(arm_ptr(...))` 改为从 guest 地址逐像素读取,并用 table[95] bitmap descriptor 或 live allocation 限定源读取范围。
  - 对越过 owner 边界的源像素不写目的像素,避免把其它 heap/code/string 字节当成颜色。
- 右侧区域复核:临时 table[122] 像素探针显示,最终阶段存在合法 `DrawRect(2,202,236,101,0x65,0x3D,0x01)` 和正常 `table[120] rect=186,274 49x13` 绘制。因此“右侧非黑”不是 stale screen cache,不能用黑填充作为通用修复。
- 聚焦验证:
  - `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t '花屏' --reporter=verbose` 通过。
  - 最新 PPM `/tmp/vmrp-e2e-Rn2k0Z/render-check.ppm`: `(128,276)=[96,60,0]`;检查区域内原花屏代表色 `(48,48,48)`/`(96,100,96)` 计数为 0。
- 兼容验证:
  - `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts test/e2e/opbzqe/game-prepare.test.ts test/e2e/gghjt/game-start.test.ts --reporter=verbose` 通过,3 个文件 4 个用例通过。
  - 批量加入 `talkcat/game-prepare.test.ts` 时,该用例断言已打印通过,但批量命令收尾阶段有一个 `talkcat.mrp` 进程未退出;已中断该批量命令并确认无残留进程。此项不计作干净通过。
