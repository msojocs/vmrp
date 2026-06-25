---
title: "gghjt modal timer and cold extraction regressions"
tags: ["vmrp", "gghjt", "gwkdl", "mrp-cache", "timer", "arm-ext"]
created: 2026-06-25T14:46:53.066Z
updated: 2026-06-25T14:46:53.066Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# gghjt modal timer and cold extraction regressions

2026-06-25 regression follow-up for ce382f4488948934fcdaff4811289ac1377a9aec. Fixes: removed unproven compact-sdk private-child RW bridge mirror, kept proven verdload rw+0x16C..0x1B0 layout; guarded modal close timer-head restore so saved game timer head is written only when current head is still zero; changed MRP cache lookup so exact path matches still work and direct basename resources still work, but dir/file requests no longer basename-fallback to package entries because those are filesystem-state probes for first-run extraction. Evidence: gghjt direct cold socket timeout was a pre-socket crash; gdb showed bad memset len 0xFFFFFFFC from game.ext parser reading count=-1 after missing gghjt.pak. gghjt.mrp res.list is entry 17, so dynamic cache from gwkdl made gghjt\res.list appear present before extraction. Verification: cmake --build build --target vmrp -j2; pnpm vitest run test/e2e/gghjt/download-plugin.test.ts; pnpm vitest run test/e2e/gghjt/game-start.test.ts; pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts; pnpm vitest run test/e2e all passed; git diff --check clean.
