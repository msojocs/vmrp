---
title: "DOTA Browser Plugin Record Pack Slot Fix"
tags: ["dota", "browser", "arm-ext", "debugging"]
created: 2026-06-27T12:58:15.667Z
updated: 2026-06-27T15:07:39.467Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# DOTA Browser Plugin Record Pack Slot Fix

On 2026-06-27 the DOTA browser plugin crash was fixed by keeping ARM-visible package names within fixed-buffer ABI limits. Deep disassembly evidence: brwmain.ext startup at 0x2D1E0C clears/copies into a 32-byte R9-relative buffer at R9+0xBD8; the following list head at R9+0xBF8 was overwritten by bytes 32..35 of the absolute host path ("ixtu"), leading to crash PC 0x2CF5CA and unmapped read 0x75747879. frame.ext at 0x2C9810 uses a 32-byte sp+4 package buffer before a table[125] wrapper and later writes through an output pointer at 0x2C9844. frame.ext's getter at 0x2C916C reads child module record[100], not just shared EXT_TABLE[100].

The final fix has two generic pieces. First, root table[100] now exposes the already-computed cwd-relative pack alias from arm_ext_init_pack_names() instead of the absolute host path, and direct file probes table[40]/[42]/[46] resolve that alias through arm_ext_pack_to_host_path(), matching table[125]. Second, private-loader children whose own Thumb code proves a 32-byte package-copy hazard get both shared table[100] and child record[100] rewritten to a bounded alias. If a mythroad-relative spelling or basename is still too long, the executor registers a synthetic alias like ~pN and resolves it back to the host package for I/O. record[100] strings live in executor metadata memory, not adjacent low heap/P storage. This remains generic: no DOTA, app, filename, or payload-length branch, and no table[125] bad-pointer suppression.

Final verification: cmake --build build --target vmrp -j2 passed; VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose passed in 83.429s with kept dir /tmp/vmrp-e2e-GSqFvj. The kept stdout/stderr have no invalid memory, UC_MEM, UC_ERR, 2CF5CA, 757478, 2C9844, or pc-bytes signatures. PPM evidence: start-browser.ppm is 240x320 with 27 colors; browser-running.ppm is 240x320 with 24 colors, nonzero=223287, pixel(152,146)=(240,240,240); final screen.ppm has 2954 colors. The E2E regression records drawCount before the final LEFT_SOFT and requires it to grow after the 60s browser window while checking browser-running.ppm.
