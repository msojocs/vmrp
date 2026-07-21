---
title: "gwyaz installed-file probe and wrapper RW owner fix"
tags: ["arm-ext", "mr-info", "mrp-cache", "installer", "restart", "journal", "event-routing", "gwyaz", "debugging"]
created: 2026-07-21T04:03:19.000Z
updated: 2026-07-21T04:14:46.000Z
sources: ["src/arm_ext/aex_table.c", "src/arm_ext_executor.c", "src/include/arm_ext_internal.h", "docs/修复记录/gwyaz/gwyaz-startup-hang-progress.md"]
links: ["private-loader-wrapper-package-context-repair.md", "cookie-mrp-restart-and-full-runtime-version-contract.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# gwyaz installed-file probe and wrapper RW owner fix

Two independent host semantic bugs hid behind the gwyaz startup test.

First, ARM EXT table[42]/`mr_info` must report the real EFS installation state. The old implementation called `mr_info()` and, on failure, reported `MRP_IS_FILE` when the same name existed in the current MRP cache. Disassembly at outer-wrapper PC `0xE8168F` proves this query is the first-run installer's "already installed?" probe. A cache entry is a source payload, not an installed file. The false positive made the guest skip its manifest extraction, publish `RESTART` to `gwy.mrp`, and then lose the source when the old ArmExtModule was destroyed. Removing only that table[42] fallback lets the guest use the existing table[40] read-only cache fd to extract its own manifest. The resulting physical layout contains `gwy.mrp` plus the guest-declared `gwy/{dload,gamelist,gui,pmsg,reglogin,resmng,rollscr,svrctrl}.mrp`; no host manifest parser, package-name rule, or cross-restart fallback is needed.

Second, the wrapper RW journal must distinguish unowned staging code from confirmed child code. The input path was complete: RIGHT_SOFT became Mythroad key 18, wrapper listeners delivered it to reglogin and GUI, GUI translated press/release to `0x3004/0x3005`, and its dispatcher reached the current-page lookup. The lookup returned zero because the host journal had reverted wrapper RW offset `0x8F4`. Exact address proof: wrapper RW `0x225E60`; GUI root current-page slot `0x226754`; difference `0x8F4`. GUI PC `0x2433CC` had legitimately stored widget `0x24C118` there and no detach/destructor ran. The old "all non-wrapper PCs are foreign" rule erased that shared-object write before the event.

The journal now records a write only when the PC belongs to neither the wrapper nor a confirmed nested module (`arm_ext_find_nested_module`). This remains effective for the geyaxz R9-divergence case: pending staging code has no confirmed owner, and its post-registration stale staging PC is outside the confirmed runtime image. It also permits registered children to mutate wrapper-allocated objects intentionally passed to them. The rule depends on executable ownership, not application identity, offsets, pointer values, input codes, or UI state.

Verification evidence: the target gwyaz test passes and produces different before/after PPM hashes (`4639525...` and `70f2dcc...`); focused DIAG no longer reports an `off=0x8F4` revert. The three geyaxz tests, including the missing-plugin download path and the approximately 97-second play flow, also pass, preserving the journal's original protection contract. Final no-retry regression passed all 28 E2E files / 51 tests in 308.63 seconds; ctest 1/1, TypeScript, and diff checks also passed.

Durable rules:

1. `mr_info` answers installed filesystem state; a readable package source is not proof of installation.
2. Source access and installed-state queries are separate contracts: table[40] may expose a container payload while table[42] still reports it absent from EFS.
3. Wrapper ER_RW can contain shared allocated objects. Registered child writes are not corruption merely because the destination lies in wrapper RW.
4. Journal staging-R9 corruption by code ownership: protect against unowned executable PCs, including stale staging addresses, while preserving confirmed module writes.
