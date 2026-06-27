---
title: "opglqa font download repeat investigation"
tags: ["opglqa", "font", "download", "e2e", "arm-ext", "mrp"]
created: 2026-06-27T02:03:08.981Z
updated: 2026-06-27T02:28:27.247Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# opglqa font download repeat investigation

# opglqa font download repeat investigation

## 2026-06-27 initial evidence
- Goal: fix `pnpm vitest run test/e2e/opglqa/font.test.ts` where first run downloads font resources but the second launch still asks to download.
- Constraints: no xvfb; avoid full trace; no app-specific branches; add explanatory comments to code changes; use disassembly and PPM evidence; keep progress in docs/wiki.
- Current worktree has unrelated changes in `docs/prompt.md` and `test/e2e/optwar/game-prepare.test.ts`; opglqa test and fixture are untracked. Do not revert unrelated work.
- `test/e2e/opglqa/font.test.ts` first expects a download-font screen, confirms font download, enters main screen, then restarts and expects direct main screen.
- Code path under review: `src/arm_ext_executor.c` MRP package cache falls back for `mr_open`, `mr_info`, and `mr_getLen`; `src/mythroad/dsm.c` maps MRP paths into `mythroad/` and logs file opens.
- Next evidence to collect: preserved e2e logs, files created under `mythroad/`, MRP entry list, and disassembly/string references for skyfont/font persistence checks.

---

## Update (2026-06-27T02:28:27.247Z)

## 2026-06-27 final resolution
- Root cause: ARM EXT table[125] used the host VM global `pack_filename` for `_mr_readFile`, ignoring the ARM-visible package pointer in table[100]. During font install, the ARM wrapper had switched table[100] to `system/___sftmp.mrp`, but host `_mr_readFile("gb121.uc2")` still opened `test/fixtures/opglqa_v4450.mrp`; that package lacks the `gb12*.uc2` chunks, so install failed and removed `system/gb12v2.uc2`.
- Disassembly/string anchors: `rx.ext` contains `system/___sftmp.mrp` at adjusted `0x00E9D8F0`, `gb12%d.uc2` at `0x00E9D904`, and `verdload.ext` at `0x00E9D918`; `skyfont.ext` contains `system/gb12v2.uc2` at adjusted `0x00E81384` and `sfc.bin` at `0x00E81398`. The downloaded package is MRPG and contains `gb121.uc2` plus the other chunks.
- Fix: added generic `mr_readFile_from_pack(packname, filename, filelen, lookfor)` in `src/mythroad/mythroad.c` and made `src/arm_ext_executor.c` table[125] bind `_mr_readFile` to the ARM table[100] package context for one bridge call. This is not app-specific and has no fallback to the outer package on invalid pack names.
- Test adjustment: `test/e2e/opglqa/font.test.ts` now resets only generated `gb12v2`/temp font artifacts at start, polls for the stable main-screen pixel after install because successful extraction briefly shows a black transition frame, and then relaunches to verify direct main-screen entry.
- Verification evidence: `pnpm vitest run test/e2e/opglqa/font.test.ts --reporter=verbose` passes. First-launch logs show `system/___sftmp.mrp` opened repeatedly for extracted chunks, then `gb12v2.uc2` and `gb12v2.adl` persisted and `___sftmp.mrp` removed. Second-launch logs show `gb12v2.adl` and `gb12v2.uc2` opened directly, with no `vld/77ae410` download path. PPM pixels: first `main-screen1` and second `main-screen2` both have pixel `(9,188)=[144,212,248]`.
- Regression verification: `pnpm vitest run test/e2e/dota/download-plugin.test.ts test/e2e/gghjt/download-plugin.test.ts --reporter=verbose` passes, 2 files / 6 tests.

