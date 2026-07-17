---
title: "OP6120 independent SCRRAM 4M startup fix"
tags: ["vmrp", "op6120", "arm-ext", "scrram", "allocator", "table38", "disassembly", "ppm"]
created: 2026-07-17T07:08:48Z
updated: 2026-07-17T07:08:48Z
sources: ["src/arm_ext/aex_table.c", "src/arm_ext_executor.c", "src/include/arm_ext_internal.h", "src/mythroad/dsm.c", "docs/修复记录/op6120-4m-startup-hang-debug.md", "test/e2e/op6120/boot-4m.test.ts"]
links: ["wxmdld-wrapped-lg-mem-first-fit-fix.md", "sanguo-black-screen-dynamic-lg-mem-arena-fix.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# OP6120 independent SCRRAM 4M startup fix

## Symptom and bounded reproduction

`build/vmrp build/mythroad/op6120.mrp --memory 4M` remained on its progress
artwork while the same package started with 1M. The 525104-byte MRP has SHA-256
`611b4cd737dcf458370ff215bc73636cf65bdc6dfc36907c2ce8aa8f00b7c8e2`.

Runs used SDL dummy, the E2E socket, bounded table/PC diagnostics and PPMs, without
Xvfb or an instruction-wide trace. Both 2M and 4M stopped at draw count 13 with the
same PPM SHA-256 `56071684cf261086bd89fed8614138df21aeaff996b7455455b5a7b64f054003`.
That frame has 1787 colors but only 10207 nonblack pixels. A successful 1M run
advanced beyond draw 6000 and produced SHA-256
`c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`,
with 1697 colors and 75128 nonblack pixels. Distinct-color count alone therefore
cannot distinguish success from this richer loading artwork.

All memory sizes generated the same 3822820-byte `cache/op6120/sky.bmp`, SHA-256
`6db8e39e624a3606ce40e4cc5cc4c42f06a30055f625c4a28e6f51df66f56def`,
ruling out cache corruption as the branch cause.

## Disassembly and allocation boundary

The package chain is `start.mr -> mrc_loader.ext -> cfunction.ext -> game.ext`.
`game.ext` then loads `mythroad/plugins/flaengine.mrp:flaengine.ext`. The extracted
engine is 129312 bytes with SHA-256
`da95583347450358a093fc0c95687cdff52438728e63dca9309f59066da6ca8b`.
Its ARM entry is `+0x8`, Thumb loader `+0x18734`, SCRRAM request entry `+0x6C4`
and exRam detection entry `+0x7C0`.

At runtime `flaengine.ext+0x6C4` requests roughly 10 MiB. The first table[38]
`MR_MALLOC_SCRRAM` (code 1014) call asks for `0xA00400` bytes and returns to
`flaengine.ext+0x733`.

With 1M, the old main bump is about `0x345EF4`, so the allocation ends before
`EXT_STACK_ADDR=0xE00000` and succeeds once. With 4M, the origin pool has advanced
the bump by about 3 MiB. `arm_alloc()` crosses the wrapper stack, skips to wrapper
code end around `0xE85BC4`, then cannot fit before the 16 MiB main-map end.
It returns failure. Guest detection reduces the request by `0x400` per attempt;
the gap from `0xA00400` to the first potentially fitting `0x18A400` is 8664 steps,
and the eventual roughly 1.5 MiB result is too small for Flash preparation.

## Ownership correction

The host had modeled platform SCRRAM as main bump memory. Native porting semantics
treat SCRRAM as extension memory outside `LG_mem`; the reserved native code in
`src/mythroad/dsm.c` also expresses it as an independent `malloc` (the currently
compiled native branch itself returns `MR_IGNORE`). It must not change the app heap,
table[108..111] statistics, wrapper stack or wrapper code capacity.

The executor now reserves guest band `0x50000000..0x50FFFFFF`. On the first valid
1014 request it lazily allocates zeroed host backing and maps the complete band with
`uc_mem_map_ptr(..., UC_PROT_READ | UC_PROT_WRITE)`. It has symmetric
`arm_ptr()`/`arm_addr()` translation but is intentionally absent from LG_mem
first-fit mapped-band traversal. A request over 16 MiB explicitly fails; there is no
main-heap fallback.

Repeated requests preserve one active stable allocation. Growth initializes only
the newly exposed `[old_len,want)` tail so existing guest data survives. Code 1015
treats NULL as a no-op, accepts only the current allocation base, unmaps before
freeing host backing and rejects foreign/stale/double non-NULL frees. Module unload
closes Unicorn before freeing backing that remains active.

A bounded final diagnostic recorded exactly one request:

```text
code=1014 inputLen=10486784 ret=0 out=0x50000000 outLen=10486784
```

No decrement loop occurred. A separate minimal ARM Unicorn diagnostic executed
`map_ptr -> unmap/free -> same-address remap -> read/write` from a code hook and
returned `hook unmap/remap/read/write: ok`, validating the lifecycle operation used
by 1015 without adding a product test API.

## Regression and compatibility

`test/e2e/op6120/boot-4m.test.ts` pins the MRP hash, creates an isolated work tree,
installs the local advbar fixture, deletes SKY to force the failing conversion path,
and maps all relevant external domains to `127.0.0.1`. It starts with 4M and waits
for the exact successful PPM. It also asserts pixel `(184,128)=rgb(240,240,240)`,
1697 colors, the SKY size/hash, draw count above 13 followed by further growth, and
no Unicorn, unmapped, invariant or fatal log markers.

The final 4M result is byte-identical to the known successful 1M PPM and reaches the
application's DNS/connect/send/receive phase. TypeScript, the native build, diff
checks, focused `wxmdld`/`sanguo`/`istore`/`gwkdl` regressions, and the default
parallel full suite all pass. The full result is 24 test files and 45 tests in
295.00 seconds, without Xvfb.

Detailed rejected hypotheses, addresses and phase-by-phase evidence are in
`docs/修复记录/op6120-4m-startup-hang-debug.md`.
