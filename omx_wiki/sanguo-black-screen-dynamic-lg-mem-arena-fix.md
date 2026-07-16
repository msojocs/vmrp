---
title: "Sanguo black-screen dynamic LG_mem arena fix"
tags: ["vmrp", "sanguo", "arm-ext", "allocator", "lg-mem", "disassembly", "ppm"]
created: 2026-07-16T13:58:36.204Z
updated: 2026-07-16T13:59:05.540Z
sources: ["src/arm_ext/aex_mem.c", "docs/修复记录/sanguo-black-screen-disassembly.md", "docs/修复记录/sanguo-black-screen-runtime.md"]
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# Sanguo black-screen dynamic LG_mem arena fix

## Symptom and bounded evidence

`build/vmrp build/mythroad/490111_240x320_sanguo.mrp` stayed inside the first synchronous cfunction.ext code-0 call at about 100% of one CPU. The 64-entry guest PC ring converged on Thumb `0xE82DE2..0xE82E3C`; the baseline P6 240x320 frame was entirely black. No Xvfb or unbounded trace was used.

## Root cause

The wrapper function at cfunction.ext `+0x2C80` legitimately attaches external arena `0x40018800..0x4001CDA0` to LG_mem. It changes the ARM-visible `table[110]` end and links nodes by offsets relative to `table[108]` base; `table[146]` exposes the shared free-list head. The host allocator incorrectly retained the initialization-time `origin_mem_addr + origin_mem_len` end, rejected the external node, and used bump allocation. Guest teardown then traversed a different list and formed a permanent four-node loop. Historical R9 synchronization affected whether the loop persisted but disabling it only produced a black frame and is not the fix.

## Generic repair

`src/arm_ext/aex_mem.c` now reloads current base/end/head from `table[108]/[110]/[146]` for every table allocation/free. It validates arena endpoints independently because the numeric interval may contain unmapped gaps, and validates every visited offset, metadata pair, and full node span. The traversal limit is derived from mapped band capacity. ARM-visible left/min/top statistics are read before update, and wrapper-owned top is preserved while a dynamic end is live. Host bump ownership is checked before the numeric arena test; dynamic frees must also be app-visible, so executor-private or registered executable storage cannot enter the guest list. First-fit, address reuse, bump fallback, and executable-range protection remain intact. No package checks, fixed target addresses, timeout exits, or corrupted-chain skipping were added.

## Verification

The final SDL dummy run reached network initialization and attempted `211.155.236.226:20000`. `/tmp/sanguo-final.ppm` is P6 240x320 with SHA-256 `c4ca6f078322e28d3969479da7b437f55458d6843eea96d9f188b7dd2e3db0e5`; it shows the game connection-timeout dialog instead of a black hang. The rebuilt focused allocator suite passed 3 files/6 tests in 307.26s. The rebuilt default parallel E2E suite passed 19 files/32 tests in 289.89s. Build, TypeScript, and diff checks passed. Stale build-tree CTest artifacts are not valid current-source unit evidence.

Detailed retained evidence is in `docs/修复记录/sanguo-black-screen-disassembly.md`, `sanguo-black-screen-runtime.md`, and `sanguo-black-screen-progress.md`.
