---
title: "gzwdzjs play MR_UCS2GB caller-owned output pointer fix"
tags: ["gzwdzjs", "arm-ext", "mr-platex", "ucs2gb", "abi", "debugging"]
created: 2026-07-12T04:04:54.545Z
updated: 2026-07-12T04:04:54.545Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# gzwdzjs play MR_UCS2GB caller-owned output pointer fix

2026-07-12: gzwdzjs play froze because game.ext text layout called mr_platEx(1207/MR_UCS2GB) with a caller-owned guest buffer. dsm.c writes GB text in place and intentionally leaves output_len at 0. aex_t038 incorrectly treated length 0 as no output and overwrote the valid guest output pointer with 0. game.ext then called strlen(NULL), failed its nonzero check, and reached ARM semihost SYS_EXIT every callback. Generic fix: when host_output is the same mapping as the original ARM output, retain the original ARM address regardless of output_len; only copy newly returned host buffers when a positive size is provided. Disassembly chain: 0x227FE6 -> 0x228458 -> 0x23D538 -> 0x23BCCC(MR_UCS2GB) -> table[38] -> 0x23D58E strlen -> 0x23D5BC check -> 0x226320 SYS_EXIT. Verified play.test passes and new-plant PPM appears about 1.5s after confirmation, with no UC errors.
