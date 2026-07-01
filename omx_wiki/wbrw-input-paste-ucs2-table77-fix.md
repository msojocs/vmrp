---
title: "wbrw input paste UCS2 table77 fix"
tags: ["wbrw", "input", "paste", "arm-ext", "ucs2", "e2e", "ppm"]
created: 2026-07-01T11:08:41.053Z
updated: 2026-07-01T11:08:41.053Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# wbrw input paste UCS2 table77 fix

2026-07-01: Fixed wbrw Ctrl+V paste blank input. Root cause: native DSM edit mode returns UCS2-BE text from mr_editGetText, but ARM EXT table[77] copied it with alloc_string/strlen, so ASCII UCS2-BE beginning with 0x00 was truncated to an empty string before browser ARM code saw it. Fix: table[77] now copies raw bytes using wstrlen(text)+2 via alloc_bytes. Added E2E PASTE command that sets SDL clipboard and injects Ctrl+V with Ctrl modifier, plus focused test/e2e/wbrw/input-text.test.ts assertion. Verification: cmake --build build --target vmrp -j2 passed; VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/input-text.test.ts --reporter=verbose passed. Final artifact /tmp/vmrp-e2e-4w5oYJ stderr contains editGetText URL; before/after PPM top address bar diff 1030 pixels in bbox x=10..233,y=3..26.
