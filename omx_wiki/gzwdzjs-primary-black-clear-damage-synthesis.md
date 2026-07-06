---
title: "gzwdzjs primary black clear damage synthesis"
tags: ["gzwdzjs", "arm-ext", "rendering", "screen-damage", "drawrect"]
created: 2026-07-06T14:59:48.257Z
updated: 2026-07-06T14:59:48.257Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# gzwdzjs primary black clear damage synthesis

2026-07-06 fixed `pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程` where `game-start.ppm` had `y=0..199` black after the second ENTER. Evidence: final table[29] full-screen present source and ARM screen cache already had correct nonblack pixels, but callback-exit synthesized row presents later copied black rows from the host shadow buffer. Damage sampling traced the stale visible rows to `table[122] DrawRect(0,0,240,320,0,0,0)` at `lr=0x23A62F`: a primary full-screen black cache reset before partial repaint/present was being marked as visible screen damage. Generic fix in `src/arm_ext_executor.c` keeps the DrawRect backing write but, only for primary full-screen black clears, skips screen-damage tracking/foreground claim/cache-write finish so callback-exit damage synthesis cannot expose pixels the app never explicitly presented. This is not an app-name branch and does not suppress explicit black presents. Verification: build passed; focused tutorial test passed with `/tmp/vmrp-e2e-afKb8y/game-start.ppm` `unique=84`, top nonblack `48000`, rows `0..319`, `(75,75)=[232,184,40]`; full gzwdzjs game-start file passed; opbzqe prepare, dota download-plugin, and `test/e2e/gxdzc/gxdzc-pixel.test.ts` passed.
