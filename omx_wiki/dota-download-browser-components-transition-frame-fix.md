---
title: "DOTA download-browser-components transition frame fix"
tags: ["dota", "browser-plugin", "e2e", "ppm", "debugging"]
created: 2026-06-27T15:46:20.555Z
updated: 2026-06-27T23:30:09Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# DOTA download-browser-components transition frame fix

2026-06-27 target test `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"` failed because `download-browser-components.ppm` was captured at a legitimate low-color browser component startup frame: black background plus two blue progress-line rows (`unique=2`, y=190..191). This was not PPM parsing, ANSI/terminal rendering, resource absence, or a crash. `frame.ext` extracted from `mythroad/plugins/embrw.mrp` contains `logo.bmp`, `progress.bmp`, `font.uni`, CMWAP/CMNET, and browser config strings; Thumb disassembly around adjusted `0x2c947c` confirms guest startup/UI helper flow. Fix: keep the `download-browser-components` screenshot label but poll through the transition frame until `uniqueColorCount() > 4` and `(152,146)` is non-black. Verification: `cmake --build build --target vmrp -j2` passed; `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed in 51.681s with kept dir `/tmp/vmrp-e2e-dbtcUH`; `download-browser-components.ppm` was 240x320, `unique=29`, `nonzero=74300`, rows 3..319, samples `(80,80)`, `(152,146)`, `(95,88)` all `(240,244,240)`. No UC_ERR/UC_MEM/crash signatures.

Superseded on 2026-06-28. Frame sampling proved polling moved past the target browser component page into a later browser stage; the real target page stayed line-only until resource package resolution was fixed. The final fix is in `src/arm_ext_executor.c`: `table[125]` uses LR to scope blank/root-package child resource reads to the registered child package owner. See [[dota-browser-plugin-dsm-package-provenance]] for the final evidence.
