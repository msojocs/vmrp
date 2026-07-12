---
title: "WBRW html2sky native elements and PPM verification 2026-07-12"
tags: ["wbrw", "html2sky", "sky-format", "native-controls", "ppm"]
created: 2026-07-12T12:38:25.071Z
updated: 2026-07-12T13:06:43.621Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# WBRW html2sky native elements and PPM verification 2026-07-12

Final converter rule memory. html2sky.go must emit the sample style/glyph prelude before section 0x0f; otherwise WBRW accepts the packet but renders blank. Record framing is parent:u16, idFlags:u16, optional e1 for 0x8000, optional e2 for 0x4000, op:u8, len:u8, payload. Linked images use href/src/UTF-8-alt pool order and 0x4e->0x38->0x45->0x38. Forms use 0x61, inputs/submits use 0x62 type 8/0/7, select uses 0x41 with all option value/UCS-2-label pairs. 0x38 uses a raw caption index and activates the preceding href; 0x45/0x54/0x61/0x62 references are one-based. The shared content node uses neutral style 3; green style 7 belongs on bounded descendants. Live path is game.ext -> nginx:80 -> mrp-store:8089 -> html2sky.Generate. Final E2E without xvfb passed and wrote /tmp/vmrp-e2e-tqv2Vy/loaded.ppm: 240x320, 122 colors, 34204 exact [208,252,136] pixels bbox [5,57,230,295], SHA256 11bb0823ee54fc54fc048790cd8e7dc4c39e1a1fa3a9e20b53991587bb50321e. PPM visibly contains logo caption, native input/select, 搜索/跳页 buttons, and local green bands. The logo 0x45 causes /image HTTP 200, but this WBRW build displays UTF-8 alt text rather than PNG pixels. temp.test.ts now captures loaded.ppm and asserts the final visual pixels so blank pages fail.

---

## Update (2026-07-12T12:49:59.988Z)

Final converter rule memory. html2sky.go must emit the sample style/glyph prelude before section 0x0f; otherwise WBRW accepts the packet but renders blank. Record framing is parent:u16, idFlags:u16, optional e1 for 0x8000, optional e2 for 0x4000, op:u8, len:u8, payload. Linked images use href/src/UTF-8-alt pool order and 0x4e->0x38->0x45->0x38. Forms use 0x61, inputs/submits use 0x62 type 8/0/7, select uses 0x41 with option value/UCS-2-label pairs. 0x38 uses a raw caption index and activates the preceding href; 0x45/0x54/0x61/0x62 references are one-based. The shared content node uses neutral style 3; green style 7 belongs on bounded descendants. Production code has no target URL/app/fixture checks and no page-specific c1/title/cpy/c3 selectors. Live path is game.ext -> nginx:80 -> mrp-store:8089 -> html2sky.Generate. Final E2E without xvfb passed after all edits and wrote /tmp/vmrp-e2e-sh49lG/loaded.ppm: 240x320, 122 colors, 34204 exact [208,252,136] pixels bbox [5,57,230,295], SHA256 8f21bebd1fef5203c5a2883f22e1d82ad9625ce51bc0d2cc93473e4a924f5233. PPM visibly contains logo/title, native input/select, 搜索/跳页 buttons, and local green bands. The logo 0x45 causes /image HTTP 200 with a byte-identical 5291-byte PNG. game.ext+0x22760 uses alt when decoded width or height is non-positive; current emulator displays UTF-8 alt because its decoder/completion bridge does not publish dimensions, not because of SKY or proxy fields. temp.test.ts captures loaded.ppm and asserts final visual pixels so blank pages fail.

---

## Update (2026-07-12T13:06:43.621Z)

Final converter rule memory, including ordering correction. html2sky.go emits the sample style/glyph prelude before section 0x0f. Record framing is parent:u16, idFlags:u16, optional e1/e2, op:u8, len:u8, payload. Images use href/src/UTF-8-alt and 0x4e->0x38->0x45->0x38. Forms use 0x61, inputs/submits 0x62 type 8/0/7, select 0x41. The shared content node uses neutral style 3. Production code has no target URL/app/fixture checks or page-specific CSS class selectors. Ordering rule: never prioritize controls ahead of source content. Items retain DOM order. All links/plain text/separators inside the same paragraph receive one FlowID and share one 0x4e row; sibling anchor/text nodes therefore flow inline and wrap. The form follows the navigation row, and result links/descriptions follow the form. Unit tests assert A/B navigation before input before Result and one row per inline flow. The remote task.jysafe so1 endpoint currently returns zero result nodes, so its default screenshot correctly proceeds from form to pagination. Default E2E passed at /tmp/vmrp-e2e-YWbvWe/loaded.ppm, SHA256 b42968c115e9b50ed7805bade1c1fedbd034d6d74878cd3bf300f285ee3dd2df. Controlled HTML with two result nodes was fetched through the same Go proxy using VMRP_WBRW_TEST_URL; /tmp/vmrp-e2e-9QAIjR/loaded.ppm visibly shows net-disk navigation, native input/select/搜索, then qq2009.jar result and description, SHA256 11fc5ae9280348fa11d8bcb34c3d6e498f976f42a7a15ed61b1a07574ce170a3. No xvfb or broad trace was used.
