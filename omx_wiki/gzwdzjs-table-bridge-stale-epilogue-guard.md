---
title: "gzwdzjs table bridge stale epilogue guard"
tags: ["gzwdzjs", "arm-ext", "table-bridge", "thumb", "bad-sp"]
created: 2026-07-06T13:15:59.658Z
updated: 2026-07-06T13:15:59.658Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# gzwdzjs table bridge stale epilogue guard

2026-07-06 fixed gzwdzjs repro.sh severe stutter/error-spam by guarding stale returns from ARM EXT native table bridges. Disassembly evidence: game.ext loaded at 0x226118; 0x23A880 starts with Thumb push {r4-r7,lr}; sub sp,#12, so old SP=0x9 caused writes near 0xFFFFFFF5. Bridge callsite 0x23B5AE is blx r7 followed by 0x23B5B0 pop {r3,r4,r5,r6,r7,pc}. hook_table returns native table calls by writing PC=LR. If that epilogue is re-entered after its pop consumed the stack frame, the stack is double-popped and later callbacks run with a bad SP. Generic fix in src/arm_ext_executor.c records table returns whose LR points at a Thumb blx-reg/pop-pc epilogue, consumes the first legitimate entry, computes the post-pop SP, and stops only a duplicate entry with the same PC/post-pop-SP/self LR. No app-name or fixed-address branch remains. Verification: target repro with SDL dummy and same auto-click sequence had no UC_ERR/UC_MEM/invalid memory signatures, stdout+stderr 1796 bytes, draw counts 98/176/283/283/283 at 5/15/30/55/85s with 0.1-0.2ms socket responses, latest PPM 240x320 sha prefix 66d382b4f7105779 unique=17 nonzero=88954. Full pnpm vitest run test/e2e --reporter=verbose passed: 16 files, 27 tests.
