---
title: "gfhcq game stutter performance investigation"
tags: ["vmrp", "gfhcq", "performance", "arm-ext", "e2e", "ppm"]
created: 2026-07-15T00:00:00+08:00
updated: 2026-07-15T19:21:26+08:00
sources: ["test/e2e/gfhcq/temp.test.ts", "test/fixtures/gfhcq.mrp"]
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# gfhcq game stutter performance investigation

## Goal and constraints

The focused repro is `test/e2e/gfhcq/temp.test.ts`: start the `480x800`
runtime, decline BGM, press ENTER on the menu, and observe the game phase.  The
required outcome is materially faster game execution, followed by compatibility
verification against the other E2E cases.  The investigation must not use
`xvfb`, application-name branches, fallback behavior, or unbounded trace logs.
Production changes require comments that explain the corrected contract.

## Reproduction and valid baseline

The launcher defaults to SDL dummy drivers when the caller does not supply a
driver. Because the login shell exports `SDL_VIDEODRIVER=x11`, authoritative
baseline/final commands explicitly unset `DISPLAY`, `WAYLAND_DISPLAY`, and
`VMRP_PPM`, then set `SDL_VIDEODRIVER=dummy` and `SDL_AUDIODRIVER=dummy`; no
display server or `xvfb` was used for those measurements. The focused test now
states the actual `480x800`, `--memory 8M` reproduction contract. RGB565 screen
storage alone is 768,000 bytes, so the former default 1 MiB setting mixed
resource exhaustion into the performance result. BGM/menu waits are bounded at
30 seconds, the existing three-second delay before ENTER is preserved, and game
progress is sampled once every ten seconds without broad trace or per-frame PPM
dumping.

Artifacts `/tmp/vmrp-e2e-akjST9` and `/tmp/vmrp-e2e-lPLMIs` predate that
three-second delay and sampled a menu/transition phase. Their apparently high
submission rates are not game baselines and are deliberately excluded.

The valid pre-fix RelWithDebInfo artifact is `/tmp/vmrp-e2e-k0DfVb`. Draw
counts were `122,235,400,510,541,570,600`; the final three ten-second deltas
were `31,29,30`, or about 3.0 submissions/s after the game reached its slow
steady phase. `/usr/bin/time` reported 44.51s user, 0.80s system and 69.43s
wall time (about 65% CPU). The final PPM had 749 colors and 344,083 non-black
pixels, with SHA-256
`7b0862f073b6cb9d14ac84616c4b4e28a24ebb8c17fcb1454ec3dc9d36a409ed`.
A valid long Debug run settled near 1.03 submissions/s.

## Disassembly and bounded sampling

The package directory is `[0xF0,0x17C2)` with 241 entries. `game.ext` starts at
MRP offset `0x32973` and decodes to `0x2CEEC` bytes; `cfunction.ext` starts at
`0x51533` and decodes to `0x422C` bytes. The game helper is at
`game.ext+0x1DD81`.

The timer walker at `game.ext+0x1E9BC` uses scheduler state at `R9+0x8C` and
requests a next interval with a 10 ms minimum. The busy-wait helper at
`game.ext+0x8488` is reached by present paths that intentionally wait 30 ms;
that guest timing contract was left unchanged. The direct RGB565 loop at
`game.ext+0x25F16..+0x25F66` writes pixels with the `strh` at `+0x25F60`.

A bounded low-volume sampler over the first ten million guest blocks recorded:

- table `[119]` DrawPoint: 803,744 calls;
- table `[120]` DrawBitmap: 117,214 calls;
- table `[29]` present: 552 calls;
- timer/time tables `[31..33]`: only thousands of calls.

The diagnostic artifact is `/tmp/vmrp-e2e-SEy3zA/stdout.log`; its hot game
runtime base was `0x2BC290`, with hotspots at `+0x1CE3A..+0x1CE78` and
`+0x25F16`. This frequency split ruled out timer starvation, SDL event-loop
spin, framebuffer/GOT hooks, the R9 guard path, and SDL presentation as the
primary target bottleneck.

## Root cause and fix

`aex_t120` handled each bitmap call by allocating and copying a full
480x800 framebuffer snapshot, drawing even a small tile, comparing all 384,000
pixels, then freeing the snapshot. At 768,000 bytes per snapshot, the bounded
117,214-call sample alone implies about 90 GB (84 GiB) of snapshot copies,
before counting the full comparison reads and allocator overhead.

The final fix passes the active `ArmExtScreenContext` into
`arm_ext_draw_bitmap_from_guest`. In its existing per-pixel loop, the helper
compares each valid destination before and after the unchanged ROP operation.
Only a changed RGB565 value whose guest physical address overlaps the primary
framebuffer is merged into the existing per-row damage spans. The destination
address is mapped with the persistent primary stride, preserving the old diff
semantics when table `[91]` selects an off-screen buffer, a primary-screen
subview, or a temporary table `[92]` stride.

Source-owner bounds, ROP, rotation, and transparency remain unchanged.
Transparent pixels, same-color writes, unsupported ROPs, and rejected source
addresses produce no damage, just as with the old snapshot comparison.
Foreground-owner claim and dirty-state ordering are also unchanged. The work is
now O(blit pixels) inside a loop that already visits those pixels, with no
per-call allocation or second framebuffer scan. No application/scene check or
fallback path was added.

An independent completion audit rejected the first rectangle-only version: it
could mark an off-screen table `[91]` draw as primary damage and could expose
unpresented backing-cache colors after a transparent/no-op blit. A second audit
also required partial overlap with a primary-screen subview, not only exact
base-address equality. Both cases are handled by the final physical-address
intersection and before/after comparison. The final implementation audit found
no remaining correctness blocker for ABI-valid, two-byte-aligned screen
buffers.

Investigative R9/guard caches, hook narrowing, deferred framebuffer diff,
sampler counters, and SDL conversion changes were removed before regression
testing. They did not improve optimized steady submission cadence beyond the
table `[120]` correction and would have expanded the compatibility surface.

## Final performance and PPM evidence

The final RelWithDebInfo dummy artifact is `/tmp/vmrp-e2e-sfwzrO`. Counts were
`121,284,449,613,778,942,1107`; the final three deltas were `165,164,165`, or
16.47 submissions/s. This is 5.49x the valid dummy pre-fix steady rate of
3.0/s. Time was 22.92s user, 0.85s system and 68.60s wall (34% CPU), reducing
CPU time from 45.31s to 23.77s, or 47.5%, while respecting guest waits.

The final Debug dummy artifact `/tmp/vmrp-e2e-tdK9Mf` produced
`118,273,437,572,685,797,905`; its final deltas `113,112,108` are about
11.1 submissions/s. Time was 48.48s user, 0.76s system and 69.91s wall. The
earlier valid long Debug run's approximately 1.03/s severe slow phase used a
different elapsed window, so it is retained as qualitative evidence rather
than reported as a strict paired speedup ratio.

For the authoritative same-driver correctness check, pre-fix dummy artifact
`/tmp/vmrp-e2e-k0DfVb`, immediate post-fix `/tmp/vmrp-e2e-qtrd5r`, and final
overlap-aware dummy artifact `/tmp/vmrp-e2e-sfwzrO` have byte-identical final
PPMs at SHA-256
`7b0862f073b6cb9d14ac84616c4b4e28a24ebb8c17fcb1454ec3dc9d36a409ed`.
The final Debug frame is a different timing-dependent sprite phase at SHA-256
`395ef893f53a5f857d637e4201ae303d6f78e0e07bb1b55f7e219ad02dfda03b`.
All final frames are 480x800 with 749 colors and 344,083 non-black pixels and
were visually inspected after lossless PNG conversion. The dummy final logs
total only 2,046 bytes and contain no PERF or broad trace output.

## Verification

- `cmake --build build --target vmrp -j4`: passed.
- `cmake --build build-perf --target vmrp -j4`: passed; only pre-existing
  string truncation warnings were emitted.
- Focused test in final RelWithDebInfo and Debug: both passed with explicitly
  forced SDL dummy drivers, no trace, and no `VMRP_PPM=1`.
- Final render-sensitive run covering gxdzc, gghjt, opbzqe, dota, gwkdl, and
  gzwdzjs: 15/15 passed; this supplemental run inherited X11 but used no xvfb.
- Final full non-temporary E2E with explicitly forced SDL dummy drivers: 19
  files and 31/31 tests passed in 309.66s, including golden full-frame and
  off-screen composition comparisons.
- Three independent read-only agents audited implementation equivalence,
  measurement/PPM arithmetic, and requirement coverage; their findings drove
  the off-screen/no-op and explicit-driver corrections above.
- The current checkout no longer defines the historical `vmrp-unit` target;
  cached build-directory ctest output was excluded from final evidence.
- `git diff --check`: passed after cleanup.
