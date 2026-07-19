---
title: "GTXZJ date guard and default runtime assets"
tags: ["skyengine", "gtxzj", "arm-ext", "disassembly", "datetime", "rtc", "fonts", "black-screen", "ppm"]
created: 2026-07-17T02:37:08Z
updated: 2026-07-17T03:10:52Z
sources: ["src/utils.c", "src/skyengine_args.c", "src/skyengine.c", "src/skyengine_api.c", "src/include/skyengine_api.h", "CMakeLists.txt", "test/e2e/gtxzj/boot-to-title.test.ts", "docs/修复记录/gtxzj-black-screen-progress.md"]
links: ["sanguo-black-screen-dynamic-lg-mem-arena-fix.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# GTXZJ date guard and default runtime assets

## Symptom and identity

`build/skyengine build/mythroad/gtxzj.mrp` opened a live 240x320 SDL window but remained
entirely black. The process was idle in the SDL event loop rather than spinning, and
15 seconds of bounded observation produced no timer or draw. A live `SIGUSR1` dump
was P6 240x320 with an all-zero RGB payload. The investigated package is 361950 bytes
with SHA-256
`65f41d75b54b66d9fe747c7df0b50b5d2808ff71c08bdbf4a860989d976773da`.

The tracked fixture provides durable EXT identities. `cfunction.ext` is 16940 bytes
with SHA-256
`5d4a25bb9aca85d4638a43acaefd1aa80267654494170b66c0b024441bbbc69c`;
`game.ext` is 101136 bytes with SHA-256
`4582afaf47b2fed745b2c75647c55dd72ff4994529b3a979ab1e34f7b273d3ab`.
They can be reproduced without any retained diagnostic worktree:

```bash
dd if=test/fixtures/gtxzj.mrp bs=1 skip=336656 count=11678 status=none \
  | gzip -dc | sha256sum
dd if=test/fixtures/gtxzj.mrp bs=1 skip=273132 count=63502 status=none \
  | gzip -dc | sha256sum
```

This is distinct from the Sanguo dynamic `LG_mem` arena bug. Memory settings from
1M through 8M and injected keys did not change the result. Investigation used the
real `DISPLAY=:0` WSLg session for final evidence, no Xvfb, and bounded PC/bridge
probes instead of an unbounded instruction trace.

## Disassembly root cause

The child `game.ext` helper is runtime `0x238099`, file offset `0x11D91`.
`mrc_init` at `0x22C124` first calls the guard at `0x23A5B8` (`game.ext` file offset
`0x142B0`) and immediately returns at `0x22C16E` unless the guard returns 1. That
return occurs before timer registration or any UI draw.

The guard obtains `mr_datetime` through `0x23A1E0 -> 0x237C0C -> function-table byte
offset 0x88`. Offset 0x88 is table slot 34, `mr_getDatetime`, so the observed host
year is application-visible and the source field layout is correct. Its complete
policy is:

- years before 2012 pass and years after 2012 fail;
- in 2012, dates before June pass and dates after June fail;
- 2012-06-20 and earlier pass deterministically;
- 2012-06-21 through 2012-06-24 use `abs(rand()) % 100 > 20 * (day - 20)`, giving
  approximately 79%, 59%, 39%, and 19% pass probability;
- 2012-06-25 and later fail.

The 2026 host date therefore makes initialization return without any timer or draw.
An isolated 2012-06-20 proof hit `mrc_init`, the successful guard branch at
`0x22C130`, resource loads, an 80 ms timer registration, and repeated full-frame
presents.

## Rejected initialization hypothesis

The wrapper private loader itself sends child helper codes 6, 8, and 10, which first
suggested that code 0 was missing. Its caller must also be disassembled: at
`cfunction.ext+0x2EA4` it loads child P, at `+0x2EA6` it loads the helper, at
`+0x2EAA` it executes `movs r1,#0`, and at `+0x2EAC` it calls the helper. The committed
path already invokes child code 0 once.

Changing `primary_host_init_pending` to 1 called the same child initializer twice
and still did not create a timer or draw. That experiment was removed. It is not a
valid repair and must not be reintroduced.

## Second black-screen boundary

Passing the date gate displays two bitmap logos, then reaches the text-only
"是否开启音效？" menu after about six seconds. The CLI defaults its work directory
to the executable directory. Before the repair, `build/mythroad/system` lacked
`gb12.uc2` and `gb16.uc2`; both `mr_open` calls returned zero, so this valid text
screen also appeared black. Fixing only the date gate was insufficient for the
user's original command.

## Generic repair contract

The runtime now emulates a deterministic handset RTC date of 2011-01-01 by default,
while hour, minute, and second still come from the live host clock. CLI
`--device-date YYYY-MM-DD|host` and `SKYENGINE_DEVICE_DATE` select a validated calendar
date or restore the host wall-clock date. Validation covers the fixed format,
month lengths, and Gregorian leap years before the application starts.

`getDatetime` reads only the public runtime configuration. There are no package
names, hashes, deadline constants, post-failure retries, timeout callbacks, or
scene-specific branches. Shared-library starts inherit the deterministic default
through `skyengine_args_default()`. Embedding hosts can call
`skyengine_api_set_device_date("host"|"YYYY-MM-DD")` before start; it reuses the CLI
calendar validator and leaves the previous valid mode unchanged on invalid input.
Calls during an active VM are rejected so the worker cannot observe a mixed date.

The native `skyengine-system-assets` dependency treats tracked `gb12.uc2` and `gb16.uc2`
as required desktop assets and copies them on every requested `skyengine` build to
`$<TARGET_FILE_DIR:skyengine>/mythroad/system`. Their deployed SHA-256 values are
`f8e9a443e28eecce3a99f0ebf26a197b1ef5e65bab5406054ff7e985d48274b3` and
`6a6d819025765b4b967aa9dd5c7efc5c86b06265b73a14db91869b22bf3d2dd5`.
The dependency restores a deleted font without relinking and follows an overridden
`CMAKE_RUNTIME_OUTPUT_DIRECTORY`, so assets remain beside the actual executable.

## Verification

On real `DISPLAY=:0`, the original command reached the sound menu. The 6-second PPM
has SHA-256
`3286614596ea0f61c9d59a119e8ffec1ba754fc1678c8e81192111b7e48aa1b0` and a
nonzero text payload. A real-display controlled run selected sound-off with
`RIGHT_SOFT`, reached the illustrated title, changed the bottom selection with
`DOWN`, and opened the no-save dialog with `ENTER`; it recorded 218 draws and quit
cleanly. The title-to-selection and selection-to-dialog PPMs changed by 1506 and
57656 RGB bytes respectively.

The GTXZJ E2E reproduces the same state-machine boundaries with stable pixel and
regional-diff assertions. Final checks passed:

- `cmake --build build --target skyengine -j2`;
- `pnpm exec tsc --noEmit`;
- `SKYENGINE_BUILD_SHARED_ONLY=ON` shared-library build and direct fixed/host/invalid/
  active-run setter probes;
- `pnpm exec vitest run test/e2e --reporter=verbose`: 22 files and 43 tests in
  304.05 seconds, including nine focused date-contract tests;
- the GTXZJ case took about 7.18 seconds in that full parallel run;
- build ID `fe38a6d32e0a1c4ba78310c0ae2adce903a51d6a`;
- retained full-suite log SHA-256
  `fb21f3191c2b9edf44bbc1f930d4b39026469a3b2d733644819ab075b06e0dc0`.
- the separately configured GFHCQ performance regression passed with seven rising
  draw samples and 347189 changed pixels; retained log SHA-256
  `1b60cbba0819a36193a67010cb31b4041b362ccbff9ed368ef4095cf04406967`.

The final build ID was also replayed on real WSLg `DISPLAY=:0`. Its retained
transcript includes `RIGHT_SOFT`, `DOWN`, `ENTER`, `DRAW_COUNT=1289`, `OK quit`, and
the WSLg environment. Sound/title/load/dialog frames have SHA-256 values
`32866145...`, `000e9612...`, `9a56df14...`, and `2610ebf7...`; stable pixel probes
and 301 changed bottom-menu pixels prove the selection moved before the no-save
dialog opened. The transcript SHA-256 is
`ca153d758d4af83538709d21eee1ae8a58c3eb66a32fc998dc5b09c891c82a8e`.

One initial full run copied an ignored, diagnosis-generated
`wasm/dist/fs/mythroad/gtxzj/` into every isolated E2E workspace. That extra visible
root directory invalidated Cookie's fixed last-row coordinate. Explicit host-date
mode failed identically, proving it was not an RTC regression. Removing only that
09:56 runtime artifact restored the clean template; the final full run passed both
Cookie return routes. Do not run diagnostics with the source-tree Mythroad directory
as a mutable work directory when its contents will later seed isolated regressions.
