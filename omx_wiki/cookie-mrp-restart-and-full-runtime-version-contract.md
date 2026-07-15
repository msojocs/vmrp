---
title: "Cookie MRP restart and FULL runtime version contract"
tags: ["cookie", "mrp", "arm-ext", "restart", "mythroad", "e2e", "reverse-engineering"]
created: 2026-07-15T05:09:32Z
updated: 2026-07-15T05:09:32Z
sources: ["docs/修复记录/cookie/cookie-run-mrp-handoff-debug.md"]
links: ["cookie-e2e-key-timer-generation-boundary.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# Cookie MRP restart and FULL runtime version contract

## Symptom and acceptance boundary

The final action in `test/e2e/cookie/run-mrp.test.ts` selects DSM's application
list from Cookie's file manager. The old test ended with a fixed ten-second delay,
so it passed even when the host exited. Process survival alone was also
insufficient: after the first lifecycle repair the host survived and every EXT
bootstrap call returned success, but the screen remained Cookie's stale
"starting" frame.

The completed test waits for DSM's body pixel `(120,180)=(24,160,200)` and then
checks its title pixel `(120,10)=(24,104,136)`. The stale frame has
`(200,224,248)` and `(0,24,112)` at those locations, so the assertion proves that
the second application submitted a distinct stable frame.

## Shared-array and restart defects

Cookie's `game.ext` writes all four standard Mythroad filename arrays and then
publishes `MR_STATE_RESTART`:

```text
pack      = dsm_gm.mrp
start     = cfunction.ext
old_pack  = cookie_v6110.mrp
old_start = logo.ext
state     = 3
```

Native Mythroad exposes table[100..103] as writable 128-byte arrays. The Unicorn
table had allocated entries 101..103 as exact-length strings; empty old-app names
therefore had only one byte, while the guest legitimately cleared and copied 32
bytes. The executor also treated every guest state greater than or equal to 3 as
an exit, calling `mr_exit()` before the guest's already-armed restart timer could
reach Mythroad's standard `mr_timer() -> mr_stop() -> _mr_intra_start()` path.

The fix gives all four table entries fixed 128-byte guest slots. At a published
RESTART or STOP transition it copies the four NUL-bounded values back to native
Mythroad, recreating the original shared-memory ABI. RESTART only publishes state
3 and lets the timer perform stop/start; STOP alone follows `mr_exit()` and its
old-application/host-exit contract. The ARM form of table[131] cmd 3 has no Lua
stack, so it now uses the SDK default `start.mr` instead of dereferencing `L`.

## Why a surviving host still did not draw

After that repair the restart correctly targeted `dsm_gm.mrp:cfunction.ext`.
The 368-byte file has a valid `MRPGCMAP` header, and direct code 6/8/0 calls all
return zero. Narrow disassembly proves that it only handles code 2 by polling the
NES key bitmap at `0x80110000`; it has no drawing calls, timer registration, or UI
callbacks. It is a helper, not the application-list UI. Two captures one and
three seconds after handoff were byte-identical stale frames, with SHA-256
`e4f29b52626d22591eddfdb27758c90aa9e18cf44933f83e955c3984fa597c41`.

Package-index diagnostics proved that `dsm_gm.mrp` also contains a 37,230-byte
`start.mr`. Its strings include `c_load`, `_strCom`, and `cfunction.ext`, matching
the architecture in which the Lua UI loads the tiny native key helper itself.

## FULL versus MINI version contract

Cookie's entry selector compares the runtime version with 2000. Below 2000 it
uses the Lua entry (`start.mr`, or an existing shell/startqq variant). At or above
2000 it probes and selects `cfunction.ext`. The structured probe succeeded; this
was not a package-read failure and did not pass through table[125].

Git history identifies the regression. Before commit `87fa2d3`, FULL
`mythroad.c` declared version 1968, while MINI `mythroad_mini.c` declared 2011.
That refactor added Unicorn EXT support to FULL and also changed its public
version to 2011, without changing FULL's Lua startup architecture into MINI's
native-entry architecture. It conflated EXT execution capability with startup
ABI flavor.

The generic correction restores FULL's public version to 1968 and leaves MINI at
2011. Cookie now requests `dsm_gm.mrp:start.mr`; that Lua program loads its own
`cfunction.ext` helper. No package name, entry filename, pixel, or failed-start
fallback exists in production code, and the rejected direct-native bootstrap was
removed.

## Evidence and regression

The final no-xvfb artifact `/tmp/vmrp-e2e-aHIAVT/second-app.ppm` is a 240x320 P6
image with SHA-256
`d3d014208fb1fac6862c4c2ab2a6479aa582a09b3a6df8908f74e9301d29c3ba`.
Runtime logs show the second `_mr_intra_start` executing `dofile('start.mr')`.
All graphical runs used SDL's dummy video/audio drivers.

Verification completed on 2026-07-15:

```text
cmake --build build --target vmrp -j2                  passed
ctest --test-dir build --output-on-failure             1/1 passed
pnpm exec tsc --noEmit                                 passed
focused Cookie E2E                                    1/1 passed
complete test/e2e matrix                               19 files, 31 tests passed
```

The reusable rule is that an emulator must preserve both halves of a platform
contract: mutable shared arrays and lifecycle states at the memory boundary, and
the runtime-flavor version that guest launchers use to choose their startup ABI.
An EXT returning success is not evidence that an application initialized; the
next application's own rendered state is the acceptance boundary.
