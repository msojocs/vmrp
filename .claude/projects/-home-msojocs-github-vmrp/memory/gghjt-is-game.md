---
name: gghjt-is-game
description: gghjt.mrp is the game "蛊惑街头", not a banking app
metadata:
  type: project
---

`gghjt.mrp`（src/app_compat_gghjt.c 针对的应用）是游戏「蛊惑街头」，不是银行类应用。其 `cfunction.ext` 用私有加载器加载子模块 `verdload`，绕过标准 `_mr_c_function_new`(函数表 table[25]) 路径，导致 vmrp 通用 GOT 重定位漏掉它——这才是 app_compat_gghjt 那些硬编码 hook 存在的根因。详见 [[app-compat-generalization]]。
