---
title: "GSHA old compact-timer download callback fix"
tags: ["gsha", "arm-ext", "compact-timer", "download", "reverse-engineering"]
created: 2026-07-22T13:32:55.735Z
updated: 2026-07-22T13:32:55.735Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# GSHA old compact-timer download callback fix

# GSHA old compact-timer download callback fix

## Symptom
After confirming resource download, GSHA created socket 2 but never called `my_getSocketState(2)` or sent `/simpleDownload`. The screen remained in the connecting state.

## Root cause
Extracted `verdload.ext` (18404 bytes, SHA-256 `ac754c00919100bc1556ce180381f90b45676c4a25899b0ee87c52bf0fba6472`) schedules a 500 ms callback, polls the socket through `mr_plat(1001, socket)`, then sends through table[89]. Its compact child scheduler uses queued/current/timestamp at `+4/+8/+12`; the host scanner only recognized the newer `+8/+12/+16` layout. The child queue was therefore invisible during timer-owner routing.

## Generic fix
`arm_ext_find_compact_timer_scheduler()` now decodes ordered Thumb word load/store behavior in the bounded walker body: timestamp load/clear, queued-head load, and current-head store/load/clear. It verifies adjacent fields and supports the two disassembly-proven child ABI starts, `+4` and `+8`; wrapper `+12` remains owned by the separate wrapper scanner. No application/package/address branch, fallback, exact instruction word, or fixed queue-operation instruction position is used. The normalized scheduler is `literal + queued_offset - 8`.

## Evidence
Pre-fix baseline `/tmp/skyengine-e2e-2E7E7r` had socket 2 but no poll or request. Post-fix `/tmp/skyengine-e2e-jwwaAK` had `my_getSocketState(2): 0`, a 590/590-byte `POST /simpleDownload`, and Host `spd.skymobiapp.com:6009`. A bounded network syscall capture showed the server previously returned TLV value 404 for resource 5000. After the user restored that resource, `/tmp/skyengine-e2e-tYOWJu` showed sustained 2048-byte receives, a 97881-byte temporary resource after 5 seconds, and a changed progress frame.

## Compatibility
Across 84 fixture prefix candidates, the final scanner matches 8 old child walkers and the same 61 new child walkers as before; all 15 wrappers remain excluded, with zero false positives. Old layouts occur in gghjt, gsha, and gwkdl. Native build, TypeScript, GSHA target, and the 5-file/6-test focused compatibility matrix pass without xvfb or large instruction/table traces. Full retry=0 E2E is 30/31 files and 53/54 tests: only the unchanged `gjxwsmn` latest-frame assertion fails, identically with an unmodified HEAD Debug binary, so no non-GSHA test change is retained.

See `docs/修复记录/gsha/gsha-download-res-progress.md` for the complete disassembly and runtime record.
