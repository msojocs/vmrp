---
title: "Optwar exit plugin component update stall"
tags: ["optwar", "plugin", "promote", "browser-components", "e2e", "reverse-engineering"]
created: 2026-06-29T11:05:31+08:00
updated: 2026-06-29T11:05:31+08:00
sources: []
links: ["optwar-exit-plugin-no-network-investigation.md"]
category: debugging
confidence: medium
schemaVersion: 1
---

# Optwar exit plugin component update stall

## 2026-06-29 11:05 current repro

Target command: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/optwar/exit-plugin.test.ts --reporter=verbose`.

Current tree builds with `cmake --build build --target skyengine -j2`. The target test now passes the earlier no-network/red-failure stage and fails at `test/e2e/optwar/exit-plugin.test.ts:101`: `promote-start.ppm` pixel `(104,296)` is still `[0,0,0]` after pressing `LEFT_SOFT` on the downloaded plugin result and waiting 20s. Kept artifact: `/tmp/skyengine-e2e-koss3D`.

Stdout evidence from `/tmp/skyengine-e2e-koss3D/stdout.log`:
- Startup ad request still uses `POST /adsystem/Ads/ClientRequest` and gets HTTP 404; this is not the target failure.
- The exit plugin download sends `POST /simpleDownload HTTP/1.1`, receives HTTP 200, then sends `POST /dl_confirm HTTP/1.1`.
- After launching the downloaded promote plugin, the browser/component update path sends another `POST /simpleDownload HTTP/1.1`, receives HTTP 200, then sends another `POST /dl_confirm HTTP/1.1`.

Current inference: the new failure is past network availability and past initial plugin download. The stuck 100% stage is likely in component install/start/render scheduling after the second simpleDownload/dl_confirm, not in the server response itself. Next work: inspect PPM color statistics, runtime `mythroad/promote` and browser component files, then run a narrow ARM EXT diagnostic to align the active child module and disassembly around the component launch path. Avoid full trace and do not add app-name-specific branches or fallback behavior.

## 2026-06-29 11:14 long observation

Manual E2E script kept artifact `/tmp/skyengine-e2e-DeGIIr` and sampled after launching the downloaded promote plugin. Draw count moved from `164` to `207` after 5s and then to `213` by 15s, but stayed `213` through 40s and 60s. PPM evidence:
- `promote-5000.ppm`: 3 colors, nonzero bbox `56,128..182,175`.
- `promote-15000.ppm`, `promote-40000.ppm`, `promote-60000.ppm`: 3 colors, nonzero bbox `52,128..186,175`, top colors black plus green `#00fc00` and white `#f8fcf8`.

Runtime files after the run: `mythroad/plugins/promote.mrp` is `27443` bytes, `mythroad/plugins/brwcore.mrp` is `99357` bytes, and `mythroad/promote/{config,font12,brw/error}` exists. This proves both the first promote plugin and the second browser core/component payload are written to disk. The failure is stable after the second component update reaches 100%; no further network request is visible after the second `POST /dl_confirm HTTP/1.1`.
