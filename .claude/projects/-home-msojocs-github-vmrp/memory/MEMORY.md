# Memory Index

- [gghjt is game](gghjt-is-game.md) — gghjt.mrp 是游戏「蛊惑街头」，不是银行应用
- [app_compat generalization](app-compat-generalization.md) — 通用化 app_compat 硬编码 hook；mtk_disasm 是加载器权威参考
- [dota splash corner garbage](dota-splash-corner-garbage.md) — dota 开机角落花屏根因：DrawBitmap 源指针指向资源清单而非位图
- [dota y80 black band](dota-y80-black-band.md) — SOLVED 主界面 y80 黑带：sync_r9 覆盖未登记新实例(menu)的 R9→memset 清掉共享 cg_1；修复=歧义共享代码且 R9 非任何已知 rw 时信任 game R9
