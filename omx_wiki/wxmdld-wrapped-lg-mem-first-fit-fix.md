---
title: "WXMDLD wrapped LG_mem first-fit and nested resource-owner fix"
tags: ["vmrp", "wxmdld", "arm-ext", "allocator", "lg-mem", "table125", "disassembly", "ppm"]
created: 2026-07-17T05:27:48Z
updated: 2026-07-17T05:53:01Z
sources: ["src/arm_ext_executor.c", "src/arm_ext/aex_mem.c", "src/arm_ext/aex_table.c", "src/mythroad/mem.c", "docs/修复记录/wxmdld-99-percent-crash-debug.md"]
links: ["sanguo-black-screen-dynamic-lg-mem-arena-fix.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# WXMDLD wrapped LG_mem first-fit and nested resource-owner fix

## Symptom

`build/vmrp build/mythroad/wxmdld.mrp` stopped on its Flash loading page at 99%
and raised `UC_MEM_READ_UNMAPPED` at `PC=0x23E128`, address `0x7801023E`.
After correcting that path, a fresh work directory exposed a second failure at
`PC=0x239644`, address `0x38383838`; an existing-cache restart could succeed.
Changing the configured memory from 1M through 16M only moved the fault.

The MRP is 505794 bytes with SHA-256
`8a0d57d6952412cb9bf7ca21766b0a7831ccf8de90aded3cebef2bcb5b32c0e8`.
The extracted `flaengine.ext` is 129312 bytes with SHA-256
`da95583347450358a093fc0c95687cdff52438728e63dca9309f59066da6ca8b`.
Investigation used bounded table diagnostics, exact PC/field watches and SDL dummy
PPMs, without Xvfb or an unbounded instruction trace.

## Hidden nested resource owner

The first fault came from `table[125]` reading `abc` from the outer package. Its
direct LR belonged to a shared wrapper thunk, so the old direct-LR lookup could not
see the nested caller. The thunk had saved the real Thumb return in its stack frame.

`arm_ext_resource_owner_for_lr()` now scans at most 16 stack words only when direct
LR is in wrapper code and direct ownership failed. A candidate must have the Thumb
bit, resolve into a registered child and be immediately preceded by a `BLX Rm`
halfword. This recovers a proven call frame without using active/foreground state as
an ownership guess. The corrected owner supplies the four important `abc` lengths
27791, 76016, 94289 and 434627.

## Why `pet` was missing only on cold start

The second bad address was a downstream NULL-derived value. `hash31("pet")` is
`0x1B11F`, and both the pair table and lookup owner were correct. On the failed cold
run, parser objects `0x2B4F74` and `0x2AA4B4` both had definition count zero at
`+0x48`, so `0x23AA86` legitimately skipped the complete definition loop. On a
correct run, definition index `r7=0x106` calls `0x2395B6 -> 0x239840` and inserts
that key with value `0x3C7E58`, attr 0, into owner `0x348B24`. The lookup chain
`0x233E2C -> 0x244148(index 0) -> 0x229C8C -> 0x2328E0 -> 0x2396F8`
returns zero on the failed cold process and `0x3C7E58` on a correct process.

Three isolated states established provenance: a clean first process generated a new
SKY/child but lacked `pet` in that same process; a warm cached process succeeded; and
retaining the newly generated SKY while deleting only `avm_cache` let the next
process insert `pet` before regenerating the correct avm. Thus the parent
`flaengine.ext` registers the property while consuming the SKY definition blob. The
generated child does not register it. After `table[131]`, its real initialization
entry is `BLX file_base+8`; the previously suspected `+0x439D` is a helper/callback.

This ruled out hash changes, lookup fallbacks, adding the `pet` key and forcing the
generated child's initialization entry.

## Disassembly and allocator contract

The function at `flaengine.ext` runtime `0x2431B8` predicts an output address,
directly edits the Mythroad free-list through `0x23FDB8`, calls `table[125]`, discards
the returned pointer and consumes the predicted address. Correct first-fit behavior
is therefore an ABI contract, not an optimization.

During cold sky conversion the guest installs this link:

```text
LG_mem_base = 0x225E5C
head.next   = 0xFFFDA360
target      = (base + head.next) modulo 2^32 = 0x2001BC
```

The target header bytes `60 A3 FD FF` confirm the value at runtime. Native
`src/mythroad/mem.c` evaluates `LG_mem_base + previous->next` on a 32-bit target, so
the pointer addition wraps. `mr_malloc` accepts the mapped node below base while
`mr_free` separately rejects a caller-supplied `p < base`.

The host port instead rejected `next_off > end - base` before adding base. It treated
the wrapped relative offset as a forward length, returned zero from guest first-fit
and fell back to bump memory. Later 76016/94289/434627-byte reads returned
`0xEF4D8C`, `0xF0767C` and `0xF1E6D0`, while guest code consumed its predicted
buffer. Parser definition counts then remained zero.

## Generic repair

`arm_ext_origin_node_read()` now performs the addition as `uint32_t` first and
validates the decoded address. A normal node may be below base only when its header
and complete nonzero range are mapped, do not overflow and remain before end.
`next == end` stays the sentinel and `next > end` remains invalid. Registered code
overlap protection and the traversal cap remain active; the cap now counts every
mapped band below end so wrapped lower nodes are represented. Guest free retains its
original `p < base` rejection.

The file bridge also validates the complete guest span before `table[43]/mr_write`.
That closes a host out-of-bounds read seen in the corrupted downstream state, but is
not the allocator root cause. There are no package names, content fingerprints,
fixed target branches, automatic `table[125]` reclamation or swallowed errors.

## Verification

A clean copied Mythroad tree generated:

- `avm_cache`, 219848 bytes, SHA-256
  `d9d91908442aad3e766ba357bfa5f1a430a5bb9baf691a2db5b054d1f75d572f`;
- `cache/wxmdld/sky.bmp`, 1196230 bytes, SHA-256
  `2d2708d82c46ef5a830b9f9635bd60b4e21e795fa6cf9be9e8dc6850615e9565`.

All three large definition reads returned first-fit address `0x24A404`, while the
sky preparation trace recorded repeated origin allocations at wrapped address
`0x2001BC`. Cold and same-directory warm logs contain no `UC_MEM`, `UC_ERR`,
`uc_emu_start failed` or fatal marker.

The cold run reached the illustrated 240x320 title at draw count 56. Its PPM has 196
colors and SHA-256
`098e2b474feb2f9217d5bbf87b6fbf50e8bf088c13e2b82d15ce39a4fd5536cb`.
On warm start, E2E `DOWN` changed draw count 48 to 49 and exactly 1138 pixels within
the menu bounds `x=84..154,y=239..284`; the resulting hash is
`2e34259c05bbc975e8baef15749afd68b3c32f7fa4d8dad9ef2047d1ffbb3b88`.
`UP` changed draw count to 50 and restored the original PPM hash byte-for-byte, then
`QUIT` exited with status zero.

`ENTER` also reached the game's own "please wait" branch before attempting its
external server at `122.224.212.135:18080`. A failed connection there is application
network behavior and is not recurrence of the 99% emulator crash.

The exact default-work-directory command initially remained almost black after the
code fix because `build/mythroad/cache/wxmdld/sky.bmp` was an old, incomplete output
from a pre-fix crash. It had the expected 1196230-byte length and four-byte header,
but SHA-256 `34664c5a...e8792`, followed by a mostly zero image region. The frame had
only 8 colors and 33 nonblack pixels (`548abcce...e877`). `avm_cache` and
`exdet.dat` were already correct.

An isolated copy proved that removing only this generated SKY, while retaining avm,
metadata and all other state, invokes the application's normal rebuild path. It
recreated SHA-256 `2d2708d8...9565` and the exact 196-color title frame. Removing the
same known-bad debugging artifact from the default build work directory made the
original command reach draw count 48 and the complete title as well. No filename,
package or hash based runtime invalidation was added, and no user cache tree was
globally cleared.

Final checks after temporary probes were removed passed the native build,
TypeScript, diff check, clean/warm/default target runs, and the full parallel E2E
suite: 22 files and 43 tests in 300.94 seconds.

Detailed addresses, rejected hypotheses and retained log paths are in
`docs/修复记录/wxmdld-99-percent-crash-debug.md`.
