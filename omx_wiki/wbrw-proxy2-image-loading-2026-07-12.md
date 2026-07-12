---
title: "WBRW proxy2 image loading 2026-07-12"
tags: ["wbrw", "proxy2", "image", "reverse-engineering", "ppm"]
created: 2026-07-12T00:00:00Z
updated: 2026-07-12T00:00:00Z
category: debugging
confidence: medium
schemaVersion: 1
---

# WBRW proxy2 image loading 2026-07-12

Task: fix `test/e2e/wbrw/temp.test.ts` so WBRW loads the page logo as an actual image, not only alt text. User constraints: modify Go directly in `/home/msojocs/github/mrp-store/backend/internal/service/mrp_proxy.go`, do not use xvfb, avoid broad trace logging, do not add app/URL-specific branches, add explanatory comments to code edits, and verify with screenshot/PPM.

Initial evidence:

- Current Go `/image` handler fetches the upstream target and returns `s.makePacket(image, nil)`: proxy2 status 200, field count 0, payload = raw upstream image bytes.
- Current `/res` and `/mrp` handlers are different: they require range requests and return fields `tag7`, `tag8`, and `tag16` around partial content.
- Existing wiki memory says a previous E2E reached `/image` HTTP 200 with a byte-identical 5291-byte PNG, but `game.ext+0x22760` drew UTF-8 alt text because decoded width/height remained non-positive.
- Local reference cache at `temp/brw/http/cache3/*.gif` contains many `.gif` image cache files. The sampled files are GIF89a raw image bytes. Reference `.sky` pool entries point to `.gif` image paths.
- Reverse-engineering window around `game.ext+0x22398..0x22818` shows WBRW image layout first tries supplied dimensions, then tries cache/decode paths, and finally draws alt if the computed width or height at the stack descriptor remains non-positive.

Next step: identify whether the fieldless proxy2 image packet prevents the image response path from creating the cache/decode descriptor, then patch the generic Go image response shape without converting bytes or checking a specific host/app.

## Update 2026-07-12T23:01:00+0800

- Baseline no-xvfb command passed but still exposes the image defect: `VMRP_E2E_KEEP_TMP=1 VMRP_PPM=1 pnpm vitest run test/e2e/wbrw/temp.test.ts --reporter=verbose` wrote `/tmp/vmrp-e2e-Z0iEO6/loaded.ppm`; workspace `/tmp/vmrp-ws-YsDudE` has `cache3/5235.dat` page SKY and `cache3/5920.dat` 5291-byte PNG.
- Current cache index after the Go run is only 16 bytes (`64 00 00 00 ...`) and does not contain per-image records. The image file exists as `.dat`, not `.gif`.
- Reference cache `temp/brw/http/cache3/index.dat` is made of 100-byte entries and stores image paths like `brw/http/cache3/6652994.gif`; sampled `*.gif` files are raw GIF89a bytes. This confirms the real browser cache names image entries with `.gif` even though the stored payload is just the response bytes.
- Disassembly evidence: `game.ext+0x22398..0x22818` image layout checks decoded/supplied width and height and reaches the alt branch at `+0x22760` when dimensions remain non-positive. `game.ext+0x27020` parses proxy2 packets and supports zero response fields, so field count zero by itself is not an envelope parse failure. `game.ext+0x21640` maps proxy2 type 1 to `/image`, while `+0x211cc` maps request type 1 to packet code 2; current image request captures show packet code 2 and many telemetry fields but no range fields.
- Working hypothesis: the generic Go image packet must carry enough response metadata for WBRW to classify/cache the payload under an image extension. Returning raw bytes with no response fields lets the cache body appear as `.dat`, which leaves the later decode descriptor empty even though the PNG bytes are byte-identical.

## Update 2026-07-12T23:20:00+0800

- Re-read repo constraints and current code. `handleProxy2Image` in `mrp_proxy.go` still discards upstream response headers and emits a fieldless proxy2 success packet with `Content-Type` set to the generic configured page content type. The handler also ignores the third return value from `fetch`, so any upstream MIME or validator data is unavailable to WBRW.
- Existing backend unit test only proves the raw image payload survives the proxy2 envelope; it does not assert any image response metadata, cache classification, or rendered pixels.
- E2E PPM helpers are already available through `readPpm`, while `temp.test.ts` currently verifies page background/input/footer pixels but has no assertion that the logo region contains decoded image pixels.
- Continuing with a bounded reverse-engineering pass around `/image` response consumption and the cache-index format before touching Go, because the server change must remain protocol-generic and cannot be keyed to the current logo URL/app.

## Update 2026-07-12T23:37:00+0800

- External `omx ask claude` advisor failed with a 502 upstream error and produced no usable protocol-field advice; artifact is `.omx/artifacts/claude-focused-subagent-task-in-home-msojocs-github-vmrp-inspect-wb-2026-07-12T15-20-59-818Z.md`.
- Disassembly of `game.ext+0x27020` confirms proxy2 packet parsing accepts zero response fields; `game.ext+0x22398..0x22818` then reaches the image alt branch only after decoded/supplied dimensions at the image layout descriptor remain non-positive.
- The image layout path does not call public `mr_platEx(3001/3002)` directly. It loads a function pointer from R9-relative slot `0x4634` and calls it with private codes `12`, `13`, and `15`; the call sequence creates/opens an image descriptor, queries dimensions, and frees the descriptor on failure. This makes missing host/private image decode plumbing a live competing root cause.
- Current `src/mythroad/dsm.c` implements `mr_platEx(4033)` as a plain success return for WBRW browser-renderer initialization, but it does not install or expose an image helper. Historic platform headers and source show public image info/decode ABI (`MR_GET_IMG_INFO=3001`, `MR_DECODE_IMG=3002`) but the observed WBRW path uses the private 12/13/15 browser image hook instead.
