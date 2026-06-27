---
title: "DOTA browser plugin DSM package provenance"
tags: ["dota", "arm-ext", "dsm", "mrp", "browser-plugin"]
created: 2026-06-27T06:49:59.956Z
updated: 2026-06-27T07:05:51.287Z
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
