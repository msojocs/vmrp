---
title: "DOTA download-browser-components waiting hypothesis invalidated"
tags: ["dota", "browser-plugin", "e2e", "ppm", "correction"]
created: 2026-06-27T15:51:29.852Z
updated: 2026-06-27T15:51:29.852Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# DOTA download-browser-components waiting hypothesis invalidated

Correction: the earlier 2026-06-27 memory claiming that waiting until the `download-browser-components` screenshot becomes 27/29 colors fixes the issue is invalid. Frame sampling in `/tmp/skyengine-e2e-sample-xUy7bc` shows that after confirming the download result, captures from 250ms through 12000ms remain the real target page with only black plus two blue rows (`#2844a0`, bbox x=52..187 y=190..191). The 27-color frame at 14000ms is a later browser stage, not the expected download-browser-components page. Future debugging should inspect `frame.ext` resource reads and table[120]/[123]/[29]/[118] draw/present bridging for logo/text loss, not add longer E2E waits.
