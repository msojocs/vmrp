---
title: "gzwdzjs ARM EXT DrawBitmap source overrun"
tags: ["arm-ext", "rendering", "drawbitmap", "gzwdzjs"]
created: 2026-07-05T14:42:16.444Z
updated: 2026-07-05T14:42:16.444Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# gzwdzjs ARM EXT DrawBitmap source overrun

gzwdzjs 花屏根因定位在 ARM EXT table[120] 宿主 DrawBitmap 桥接: game.ext 上游 0x2370AC 会在异常路径把字符串区(如 "x sprite error") 当作坐标半字表读取,生成巨大 w/h/sx/sy。旧实现直接调用宿主 _DrawBitmap(arm_ptr(p),...) 并允许 C 指针跨越当前 guest bitmap allocation,把相邻 heap/code/string 字节当作像素,产生灰/彩噪点。通用修复是跟踪 table[0]/[2]/[125] 发给应用的 live allocations,从注册 EXT 模块代码区中剔除这些 live ranges,并让 table[120] 通过 guest 地址逐像素读取,用 table[95] bitmap descriptors 或 live allocation 限定源读取范围;越过 owner 边界的源像素不写目的像素。不要用 app 专用分支或黑色兜底填充。验证: gzwdzjs 花屏 focused test 通过,PPM (128,276)=[96,60,0],原噪点代表色 48,48,48 / 96,100,96 在检查区域为 0; gxdzc/opbzqe/gghjt e2e 子集通过。
