---
title: "MPS selector 100 is not record 100"
tags: ["vmrp", "arm-ext", "mps", "selector", "record100", "alias", "disassembly", "op6120"]
created: 2026-07-17T14:12:39.386Z
updated: 2026-07-17T14:12:39.386Z
sources: []
links: ["dota-browser-plugin-record-pack-slot-fix.md"]
category: debugging
confidence: medium
schemaVersion: 1
---

# MPS selector 100 is not record 100

For OP6120 launched as `愤怒的小鸟VS僵尸2_v1002.mrp`, the black screen came from a false short-package-alias classification. `game.ext+0x452..+0x474` clears seven words and sends `app=1, code=100, param3=R9+0x104, extCode=2` through the `sendAppEvent` veneer at `+0x818`. The parent `cfunction.ext+0x18B0` selector-100 branch writes runtime API/state pointers at destination offsets +8..+24; it does not copy private `record[100]`. The real package lookup is the direct `record[100]` pointer read at `game.ext+0x9CC`, followed by `.mrp`/`.skf` checks at +0xBE8 and +0xC1C. The obsolete scanner installed a short alias anyway: the 28-byte basename survived, while the 34-byte basename became `~p0` and failed the guest suffix gate before file I/O. The generic fix is to delete that selector-based classifier while retaining independent fixed-copy and compact state-list classifiers. Appending `.mrp` to every synthetic alias was rejected because it changes `.skf` identity and creates ordinary-filename collision risk. Verification: the original command produces PPM SHA-256 `c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`; focused OP6120, DOTA browser, and Cookie handoff tests pass; TypeScript and CTest pass; the SDL-dummy full E2E suite passes 24/24 files and 46/46 tests without Xvfb. Related: [[dota-browser-plugin-record-pack-slot-fix]]. Evidence: `docs/修复记录/bird-long-mrp-name-black-screen-debug.md`.
