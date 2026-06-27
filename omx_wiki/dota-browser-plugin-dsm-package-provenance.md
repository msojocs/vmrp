---
title: "DOTA browser plugin DSM package provenance"
tags: ["dota", "arm-ext", "dsm", "mrp", "browser-plugin"]
created: 2026-06-27T06:49:59.956Z
updated: 2026-06-27T23:30:09Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# DOTA browser plugin DSM package provenance

Date: 2026-06-27

Focused test: `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`.

Root cause: after the browser plugin download installs `mythroad/plugins/embrw.mrp`, private loader children such as `frame.ext` can clear or bypass `table[100]` before calling table[125] for sibling resources. The child had only a RAM package owner, and that RAM package held decoded executable payloads rather than sibling assets like `logo.bmp`, `progress.bmp`, and `font.uni`; resource reads stayed in the wrong package context and the browser launch remained a low-color/near-black transition frame.

Fix: DSM owns write provenance. It tracks MRPG writes by returned Mythroad fd, tracks the write offset, supports append-mode `SEEK_END,0` only when host file length equals the proven stream position, publishes a complete package snapshot only after the header total length has arrived, updates the normalized path on successful `mr_rename()`, and clears stale provenance on matching `mr_remove()`. ARM EXT records the package context that produced `last_file_copy`; nested module registration proves ownership by staged executable bytes, including immutable-tail matching because private loaders overwrite the first two words. For installed DSM MRPs, the executor visits both indexed and sequential payload layouts and gunzips compressed entries before comparing to the staged child. RAM-package auxiliary proof must also show that the same encoded payload exists in the installed MRP and decodes to the staged child; shared non-executable assets alone do not promote ownership. On proof, the child owner is promoted to the host-openable installed MRP, so blank `table[100]` resource reads resolve through `mythroad/plugins/embrw.mrp`.

Evidence: diagnostic run in `/tmp/vmrp-e2e-IRIipF/stdout.log` showed `package_host candidate ... lastPath="mythroad/plugins/embrw.mrp" lastLen=21538 direct=1 ... match=1`, after DSM published `len=21538` and renamed the temp path to `mythroad/plugins/embrw.mrp`. Earlier diagnostic evidence showed `logo.bmp`, `progress.bmp`, `focus.bmp`, `no_focus.bmp`, and `font.uni` reading with `ok=1` once the child owner was promoted.

Verification: `cmake --build build --target vmrp -j2`, `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`, and non-diagnostic `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"` all passed.

2026-06-28 refinement for `download-browser-components.ppm`: the remaining line-only browser component screen was caused by the same package-provenance class at the `table[125]` bridge. `frame.ext` was registered with a child package owner, but when shared `table[100]` was blank or still named the outer DOTA package, reads for `logo.bmp`, `progress.bmp`, `focus.bmp`, `no_focus.bmp`, and `font.uni` were attempted against the root app package instead of `mythroad/plugins/embrw.mrp`. The final generic fix resolves the resource caller from LR, detects blank/root `table[100]`, and scopes the read to the child module's recorded RAM or host package owner; explicit non-root package switches remain authoritative. Verification: `cmake --build build --target vmrp -j2`; `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed with kept dir `/tmp/vmrp-e2e-hPO7o8`. The target PPM was `240x320`, `unique=1395`, `nonzero=7905`, bbox `52,71..187,234`, samples `(95,88)=(248,248,120)`, `(120,190)=(40,68,160)`, `(152,146)=(0,16,32)`, and the PNG shows the browser logo/text/progress. Tracked regression `pnpm vitest run test/e2e/optwar/game-prepare.test.ts --reporter=verbose` also passed.
