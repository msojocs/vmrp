---
title: "Wrapper RW foreign-write journal (staging R9 divergence containment)"
tags: ["arm-ext", "private-loader", "r9", "package-context", "journal", "geyaxz", "debugging"]
created: 2026-07-18T16:12:27.745Z
updated: 2026-07-18T17:30:00.000Z
sources: ["src/arm_ext_executor.c", "docs/修复记录/geyaxz/geyaxz-play-freeze-progress.md"]
links: ["arm-ext-canonical-empty-event-layer-recovery.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# Wrapper RW foreign-write journal (staging R9 divergence containment)

This page originally documented a package-context repair that rediscovered the wrapper's inline current-package buffer (R9+0x34) from the path-builder's exact Thumb instruction shape and republished table[100] bytes into it. That matcher hardcoded one compiler artifact (18 exact halfword compares) and was one of three per-field symptom repairs (event-layer selected, deferred-command queue, package context) for the same root cause. All three are now replaced by a single structural mechanism.

Forensic basis (geyaxz boot-to-home -t 2, temporary foreign-write watch): the staged child performs exactly 23 writes into wrapper ER_RW — 22 while pending (PC inside the mr_cacheSync image, R9 = borrowed wrapper rw): offsets 0x1C=1 (event layer selected), 0x30=table-slot pointer (queue tail), 0x34..0x74 = EXT_TABLE bridge pointers (the child initializing its own GOT block; the "0x0C first byte" corrupting the package buffer is the low byte of EXT_TABLE_ADDR+0xC), 0x8C..0x94 zeroing — plus 1 write after registration confirmation through a pointer computed during the staging window (0x2C = registration handle, queue head). The post-registration straggler proves a pure "pending-window PC" predicate is insufficient; the correct predicate is "foreign PC" (any PC outside the wrapper body).

Mechanism (`hook_wrapper_rw_foreign_write` / `arm_ext_wrapper_rw_journal_install` / `arm_ext_wrapper_rw_journal_flush` in src/arm_ext_executor.c): a ranged UC_HOOK_MEM_WRITE over wrapper ER_RW ([P[0], P[0]+P[4])) journals the pre-write byte value on first foreign-PC touch (bitmap + shadow buffer); a later wrapper-body write to a journaled byte clears its mark (legitimate state takes over, no revert); at every arm_ext_call entry, when no pending staging window is open, marked bytes are reverted to their journaled values. Flushing must not happen while the pending window is open, because the staged child still reads back its own GOT writes through the borrowed R9. On real hardware all of these writes land in the child's own RW, so reverting them reproduces the exact wrapper state a real handset would have — no instruction signatures, no layout offsets, no app fingerprints. The revert restores the wrapper's own package path at +0x34, so the private loader's NULL-prefix path builder reopens the correct package and the missing-plugin flow reaches the generic download prompt (proven by PPM: 5 unique colors, body pixels (80,80)/(112,125) = [0,4,0], vs the 801-color menu).

Kept caveats: a named PPM artifact is not proof of page identity — use stable body pixels plus unique-color count. The full R9 fix (restore-by-[P][0]/displacement accounting) remains deferred because it wakes the unemulated SKY billing chain. Related background: [[arm-ext-canonical-empty-event-layer-recovery]].
