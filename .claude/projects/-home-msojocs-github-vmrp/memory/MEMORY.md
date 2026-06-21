# Memory Index

- [gghjt is game](gghjt-is-game.md) — gghjt.mrp 是游戏「蛊惑街头」，不是银行应用
- [app_compat generalization](app-compat-generalization.md) — 通用化 app_compat 硬编码 hook；mtk_disasm 是加载器权威参考
- [dota splash corner garbage](dota-splash-corner-garbage.md) — dota 开机角落花屏根因：DrawBitmap 源指针指向资源清单而非位图
- [dota y80 black band](dota-y80-black-band.md) — SOLVED 主界面 y80 黑带：sync_r9 覆盖未登记新实例(menu)的 R9→memset 清掉共享 cg_1；修复=歧义共享代码且 R9 非任何已知 rw 时信任 game R9
- [gghjt scene band garbage](gghjt-scene-band-garbage.md) — PARTIAL 楼房 band 花屏真因=platEx(1014 拓展内存)被 stub→背景合成无内存(Mem-S:no memory)；已实现 exRam,错误清零、楼墙部分合成,但 band 仍残留花屏(合成不完整)。前七轮"4004"方向是未进可玩场景的错误状态
