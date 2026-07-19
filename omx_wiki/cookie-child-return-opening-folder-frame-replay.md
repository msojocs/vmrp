---
title: "Cookie child return reload protocol (table[138] binary _RL record)"
tags: ["cookie", "wbrw", "handoff", "e2e", "reload", "start-parameter"]
created: 2026-07-15T14:13:48.924Z
updated: 2026-07-16T14:20:00+08:00
sources: []
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# Cookie child return reload protocol (table[138] binary _RL record)

Date: 2026-07-16 (supersedes the 2026-07-15 "opening-folder frame replay" attempt)

## Problem

Cookie's file manager launches a child MRP (wbrw). When the child exits, the
platform restarts Cookie. On a real device Cookie shows `正在打开`, actually
re-opens the directory it was in, and restores the pre-launch selection. In the
emulator it instead landed on the home icon grid (or, with the earlier hack,
showed a replayed `正在启动` frame) — the file list was never truly reloaded.

## Rejected approach (removed)

The first fix added `present_screen_history` (per-present ring pairing whole ARM
memory + allocator ledger with pixels, rolled back at RESTART by a hardcoded
`snapshot_age=1`), `MrPausedAppSnapshot` (keeping the parent VM alive across the
child), `mr_note_directory_open` (screenshotting `mr_screenBuf` at
`mr_findStart` and replaying it through `_DispUpEx`), and a `mr_state`-poking
`mr_present_restored_screen`. This has no real-machine analogue: it rolled back
ARM memory without registers / host timer / file side effects (mixed-time VM),
the age constant only fit Cookie's draw sequence, and the replayed `正在打开`
meant the list was never reloaded. Passing tests were false positives (return
frames were byte-identical to old screenshots). All of it was deleted; the
RESTART path is now a plain cold restart, matching
`child exit → mr_restart_old_app → fresh VM`.

## Root cause (disassembly)

Cookie's continuation state is a **128-byte binary record inside table[138]**
(the host `start_fileparameter[128]`, shared memory on real hardware), not a C
string. `save_reload_state@0x338D0` (game.ext file offsets, base 0xE80000)
writes `"_RL\0"` + zero fill, a big-endian `1` at +0x6C ("fm state saved"), and
four big-endian view words at +0x70/+0x74/+0x78/+0x7C (w0 low byte = view id, at
the buffer's last byte +0x7F), plus root-dir `fm.sav`
(`fm_save@0xC36C`: BE32 magic1 | u8 dirlen | dir | BE32 depth | BE32 selection).
On cold restart, init/code=0 → `0x337BC` matches `"_RL"` → skips splash;
`0x4354`/`read_reload_state@0x33680` read the view words, and when +0x6C==1 call
`fm_load@0xC098` to read `fm.sav`, restore dir + selection index, and delete it.
suspend(4)/resume(5) are unrelated (close-handle / redraw only) — reload is a
pure cold-start path.

The emulator handled table[138] as a C string in two places
(`alloc_filename_table_slot` snprintf seeding; `arm_ext_sync_filename_slot` +
`mr_set_start_fileparameter`→STRNCPY plus a NUL at value[127] clobbering the
+0x7F view id), truncating the record to `"_RL"`. Cookie matched `"_RL"` (splash
skipped) but read the view words as 0 → home grid, `fm.sav` never opened.

## Fix

Treat the table[138] / start_fileparameter slot as a fixed 128-byte binary
buffer, everywhere in the handoff round trip:

- `src/mythroad/mythroad.c` `mr_set_start_fileparameter()` → `MEMCPY(128)`.
- `src/arm_ext_executor.c` new `alloc_start_parameter_table_slot()`
  (memcpy-seed the guest slot) and `arm_ext_sync_start_parameter_slot()`
  (memcpy 128 bytes back to host, no NUL termination).
- table[100..103] keep their string semantics; other retained host fixes
  (short pack alias, DSM work-path reset on start, Lua suspend/resume priority,
  e2e `SCREEN_DRAW` ring, `VMRP_LIFECYCLE_DIAG`) are unchanged.

## Verification (no xvfb)

`mr_open(mythroad/fm.sav,1)` read → `mr_remove(fm.sav)` → `mr_findStart(mythroad/)`
in the return trace prove the list is really reloaded and Cookie draws
`正在打开` itself. Both `run-mrp.test.ts` cases pass (`方式二` /tmp/skyengine-e2e-tZHWPH,
`方式一` /tmp/skyengine-e2e-ZLESpM). The tests pre-create `mythroad/brw` so wbrw's
own new root directory doesn't shift the index-based selection; the 8-item list
is stable across launch/return, so the restored frame matches the pre-launch
frame byte-for-byte and the last-item highlight assertion moved to (100,248).
`tsc --noEmit`, `ctest` (skyengine-unit), and the full parallel `test/e2e`
(19 files, 32 tests) all pass in ~308s.

Related files: `src/arm_ext_executor.c`, `src/mythroad/mythroad.c`,
`src/mythroad/dsm.c`, `src/main.c`, `src/e2e_control.c`,
`test/e2e/cookie/run-mrp.test.ts`, `test/e2e/skyengine-e2e.ts`,
`docs/修复记录/cookie/cookie-run-mrp-handoff-debug.md`.
