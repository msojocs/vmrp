---
title: "WBRW proxy2 white image matte progress 2026-07-16"
tags: ["wbrw", "proxy2", "image", "gif", "alpha", "ppm"]
created: 2026-07-16T19:10:00+08:00
updated: 2026-07-16T19:15:00+08:00
sources: []
links: ["wbrw-proxy2-bitmap-logo-gif-adaptation-2026-07-13.md"]
category: session-log
confidence: high
schemaVersion: 1
---

# WBRW proxy2 white image matte progress 2026-07-16

## Goal and baseline

- Goal: the transparent canvas of the task.jysafe logo must render white instead of black after proxy2 converts it for WBRW.
- Current loaded frame: `/tmp/skyengine-e2e-1UrKUI/loaded.ppm`; pixel `(100,43)` is `[0,0,0]`, while the expected framebuffer white is `[248,252,248]`.
- Upstream `http://task.jysafe.cn/so1/logo.png` is a 273x54 RGBA PNG, SHA-256 `1f57029ad31e0f5cd9815f96a9f28c46c82af9573eb37d24242b0e6ad69e3f78`.
- Among fully transparent source pixels, hidden RGB is white for 6,786 pixels and black for 3,818 pixels. The dominant intended canvas color is therefore white.
- Current cached conversion `/tmp/skyengine-ws-qR5gkb/mythroad/brw/http/cache3/6788.gif` is an opaque 220x43 GIF, SHA-256 `3b3961cd665aa286b939a882043e689bdf1226f8c558a6f92dc35367a84b10d9`; its former transparent canvas pixels are opaque black.

## Root cause and fix direction

- `proxy2/adaptImage` decodes the RGBA source, scales it into a zero-initialized transparent-black destination, then quantizes directly into the opaque Plan9 GIF palette. Transparent pixels therefore select opaque black.
- Flattening only after resize is insufficient because translucent edge samples have already blended with transparent black.
- The explicit output contract is to flatten any alpha onto an opaque white browser canvas before resizing, then quantize to GIF. This also handles PNG encoders that discard hidden RGB and store transparent pixels as black.
- The behavior depends only on image alpha, not the URL or page identity. Opaque images retain their existing conversion path.

## Implementation and final evidence

- `proxy2/adaptImage` now scans the decoded raster for alpha. When any pixel is not opaque, it paints an opaque white RGBA canvas and composites the source with `draw.Over` before resizing and GIF quantization.
- Regression input uses a transparent-black PNG canvas, an opaque colored subject, a half-transparent edge, and a width reduction. The decoded output must be 6x3, have four exact opaque-white corners, contain only opaque pixels, and retain the subject.
- Air rebuilt and restarted the proxy2 server after the change; the live image request returned HTTP 200.
- Final loaded frame: `/tmp/skyengine-e2e-0crfJT/loaded.ppm`, SHA-256 `5d55df316c04789ffe490662444ab8af4d276608e3eac6bf8416448d133d0806`.
- PPM proof: `(100,43)=[248,252,248]`, blue logo `(50,72)=[72,144,216]`, red logo `(42,68)=[248,0,0]`, and rightmost glyph `(216,53)=[152,152,152]`.
- Final cached image: `/tmp/skyengine-ws-51gaPf/mythroad/brw/http/cache3/6620.gif`, 220x43, SHA-256 `1d45130453b4b9b0ffa6067cb3fead589d7cc7e63544600e831f703d4abf70da`. All four corners are `[255,255,255,255]`; it has zero transparent pixels and zero opaque-black pixels.
- The original white-matte assertion now passes. The selected repro proceeds farther and currently stops at the unrelated fixed coordinate `(180,192)`, where a remote layout shift moved the old select-edge pixel and left body white.

## Completed checks

- [x] Added white alpha composition before resize and GIF quantization.
- [x] Added transparent-black PNG, translucent-edge, resize, opaque-output, and retained-subject coverage.
- [x] Retained and inspected the final GIF and loaded PPM.
- [x] Verified the original white pixel and all logo identity pixels.
