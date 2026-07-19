---
title: "WBRW Go html2sky rendering final verification 2026-07-12"
tags: ["wbrw", "html2sky", "sky-format", "ppm", "verification"]
created: 2026-07-12T11:32:16.280Z
updated: 2026-07-12T11:32:16.280Z
sources: []
links: []
category: session-log
confidence: medium
schemaVersion: 1
---

# WBRW Go html2sky rendering final verification 2026-07-12

Final state for the WBRW blank-page fix. Production changes are in /home/msojocs/github/mrp-store/backend/internal/html2sky/html2sky.go: the display list now always prepends the WBRW style/glyph prelude before generated 0x0f records; no includeStylePrelude/fallback branch remains. Link display operands keep the observed SKY quirk where 0x38 anchor records use the raw caption pool index while plain 0x54 text records use +1. Extracted form controls are prioritized after the page/logo title so small WBRW screens show k input, type select, search, and jump controls before long navigation menus. Verification on 2026-07-12: go test ./internal/html2sky ./internal/service ./internal/handler passed from /home/msojocs/github/mrp-store/backend. Live E2E without xvfb passed from /home/msojocs/github/skyengine with VMRP_E2E_KEEP_TMP=1 VMRP_PPM=1 pnpm vitest run test/e2e/wbrw/temp.test.ts --reporter=verbose. Retained evidence: /tmp/skyengine-e2e-jACagN/screen.ppm and /tmp/skyengine-e2e-jACagN/screen.png. The screenshot visibly renders 网盘搜索引擎, k: [2009], type: [不限 v], [搜索], and [跳页]. PPM stats: 240x320, 115 unique colors, green-near-#d2ff8d count 52798 with bbox [0,30,230,295]. E2E stdout confirms POST /page2 to Host proxy2.51mrp.com on 127.0.0.1:80 and HTTP/1.1 200 OK from nginx.
