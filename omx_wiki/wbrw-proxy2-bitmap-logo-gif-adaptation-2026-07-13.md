---
title: "WBRW proxy2 bitmap logo GIF adaptation 2026-07-13"
tags: ["wbrw", "proxy2", "image", "gif", "ppm"]
created: 2026-07-13T03:47:00Z
updated: 2026-07-13T06:01:24Z
sources: []
links: ["wbrw-go-html2sky-rendering-progress-2026-07-12.md", "wbrw-proxy2-image-loading-2026-07-12.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# WBRW proxy2 bitmap logo GIF adaptation 2026-07-13

The WBRW logo defect was not a missing network field. With raw PNG plus tag 7=102, WBRW accepted the full response, published a `.png` cache record, and invoked every completion callback. The decode callback then used private selector 12; `game.ext+0x46070` has no selector-12 implementation and returned `-1`, so decoded dimensions were never published and the image node stayed on its alt-text path.

No extra TLV can fix that path. Tag 8 is only an optional payload-length override. Tag 21 means framed/compressed payload. Nonzero tag 33 and the presence of tags 29/30 prevent normal cache publication. Tag 7, tag 20 (`Last-Modified` Unix seconds), and positive tag 15 freshness are the complete positive image-cache metadata used here.

The generic server-side compatibility rule is therefore byte-format adaptation, not metadata fabrication. Valid GIF and BMP streams pass through byte-for-byte after complete decode validation. PNG and JPEG are decoded and encoded as GIF. TIFF, truncated or otherwise malformed images, and unknown formats return proxy2 status 415. Tag 7 describes the emitted bytes, and no URL, host, application, or fixture condition exists.

The first working GIF retained a standards-valid transparent palette, but WBRW rendered that palette entry as ordinary RGB and exposed its black value across the canvas. The source PNG itself preserves hidden RGB under alpha: its least-opaque samples are predominantly white. The converter now finds the dominant RGB565 color among the source's minimum-alpha pixels, composites all alpha against that source-derived matte, and adds the exact matte to the GIF palette. This preserves partially transparent edges without hard-coding white, a page color, URL, or application. The resulting GIF is intentionally opaque because this WBRW path does not honor GIF transparency.

The final completion audit closed a generic cache-publication gap. WBRW discards a record when unsigned tag 20 is zero and signed tag 15 does not move expiry past current time. The proxy now maps `Cache-Control: max-age` first, then `Expires`, then Last-Modified heuristic freshness into positive signed-int32 tag 15. Remaining lifetime subtracts corrected current age: the greater of apparent `Date` age and upstream `Age` plus response delay and resident/conversion time. Quote-aware Cache-Control parsing honors field-qualified `no-cache`; `no-store`, invalid metadata, and unrepresentable values fail explicitly. A response with neither nonzero tag 20 nor positive tag 15 returns proxy2 502. This matches `game.ext+0x1c92a..+0x1c934`; no default TTL is fabricated.

Image request parsing is strict: real code-2, zero-payload requests require one ASCII/NUL-terminated path and host, exact packet/header lengths, and no duplicate target fields. Malformed requests return proxy2 400. Fetches advertise only GIF/PNG/JPEG/BMP with identity content encoding and cap encoded bodies at 4 MiB. Decoded still images are limited to 4,194,304 pixels; a structural GIF preflight enforces 256 frames and 4,194,304 aggregate frame pixels before `DecodeAll` allocates them. Resource-limit oversize input returns 413 rather than decoding or continuing. Unit regressions cover JPEG/PNG conversion, GIF/BMP pass-through, cache directives/timing, Unix-zero Last-Modified, quoted directives, malformed formats, oversize animation, and ambiguous envelopes.

Final proof after the matte correction: `/tmp/vmrp-e2e-XLLZsd/loaded.ppm`, SHA-256 `1239a429c4d1689ce0b3c55d0daf64b32ae62f40b6b864cd2067554dab1c993e`. The 240x320 PPM has 150 colors, 1113 blue logo pixels in bbox `[8,41,93,91]`, 74 red-family pixels in bbox `[43,79,57,84]`, 26717 exact green `[208,252,136]` pixels in bbox `[5,100,230,275]`, and 8628 exact framebuffer-white `[248,252,248]` pixels in the logo rectangle. E2E explicitly asserts the white logo background at `(100,43)`, blue cloud at `(50,43)`, exact red at `(50,82)`, shifted native controls, and the footer row. The cached adapted image is `/tmp/vmrp-ws-B6b9e6/mythroad/brw/http/cache3/5898.gif`, opaque GIF89a 273x54 with white logical background, SHA-256 `a5a2cc0f0896adb619c451d69cde206be3422e22d0bc3371d8eb25c30e0630e8`.

Verification passed without xvfb: the final E2E completed in 24.72 seconds with `VMRP_E2E_KEEP_TMP=1 VMRP_PPM=1 pnpm vitest run test/e2e/wbrw/temp.test.ts --reporter=verbose`; focused Go tests, vet, race, module tidy diff, and repository diff checks passed. Full `go test ./...` remains red only because three pre-existing html2sky tests require the absent untracked `temp/html2sky/网盘搜索引擎.html` fixture; all image-owning service/handler packages pass.

## Layout-bound fitting

The decoded bitmap exposed a second independent defect: its natural `273x54` size exceeded WBRW's content area and clipped the final logo text. Historical raw requests establish the generic sizing contract. Across 518 retained code-2 image packets, tag 4 is the `236x266` browser viewport, while tag 31 is a per-layout `220x0` or `236x0` image bound. The 499 icon requests additionally carry tag 5=`35x35`, so neither the viewport nor an HTML requested size can be substituted for tag 31. The live logo request supplies tag 31=`220x0`.

The image handler now validates tag 31 as exactly two big-endian u16 values, rejects duplicate, malformed, and all-zero bounds with proxy2 400, and passes the values to the format adapter. Images are only scaled down, never enlarged or cropped. Width and height retain their aspect ratio using `golang.org/x/image/draw` Catmull-Rom resampling; a zero axis remains unconstrained. PNG/JPEG still emit GIF, oversized static GIF and BMP retain their working transport format, and GIF/BMP that already fit remain byte-identical. A multi-frame GIF that would require resizing fails explicitly with 415 rather than silently dropping animation.

The `273x54` source now becomes `220x44`. Final no-xvfb proof passed in 24.73 seconds: `/tmp/vmrp-e2e-nQzH6Y/loaded.ppm`, 240x320 with 156 colors, SHA-256 `77b7e55d4af75b13f8f506b6a654e9f261781bf2beb15fff149e151b371394e5`. Manual inspection of `/tmp/vmrp-e2e-nQzH6Y/loaded.png` shows the complete final glyph inside the viewport. E2E asserts blue `(50,72)`, red `(42,68)`, white matte `(100,43)`, the far-right glyph `(216,53)`, and the reflowed form/footer. Exact green `[208,252,136]` occupies 26,717 pixels in bbox `[5,85,230,260]`; red-family pixels occupy bbox `[37,66,47,70]`.

The final cache artifact is `/tmp/vmrp-ws-0UsxRf/mythroad/brw/http/cache3/6131.gif`, an opaque single-frame GIF89a with white logical background, `220x44`, SHA-256 `0d5c2e3e36801a82b31612cd2a5e2f49486a4eeee1bba281d15464d73048a3c8`. Focused service/handler tests, focused vet, and race tests pass; `go mod tidy` leaves `go.mod` and `go.sum` unchanged. Full `go test ./...` retains only the three absent-fixture html2sky failures above. Full `go vet ./...` also exposes a pre-existing module-version mismatch: `go.mod` declares Go 1.23 while `config/config_test.go` uses Go 1.24's `testing.Chdir`.
