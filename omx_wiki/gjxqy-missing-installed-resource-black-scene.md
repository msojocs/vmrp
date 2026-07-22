---
title: "GJXQY missing installed resource black scene"
tags: ["gjxqy", "rendering", "black-screen", "resource", "ppm", "disassembly"]
created: 2026-07-22T15:05:02.184Z
updated: 2026-07-22T15:23:57.000Z
sources: []
links: ["gjxqy-local-download-protocol-and-fixture-id-mismatch"]
category: debugging
confidence: high
schemaVersion: 1
---

# GJXQY missing installed resource black scene

Target: `pnpm vitest run test/e2e/gjxqy/temp.test.ts`, without xvfb or broad trace. Baseline failed 4/4: `scene-next.ppm` SHA-256 `0f661bb4556e81c44fb06692775ba68293c5d0bb5c29b330f4bef1b4bd019e3ab`, 240x320, 3 colors, 76040/76800 black pixels (99.010%), with only the 38x20 tavern name tile visible. SDL dummy uses CPU RGB565 presentation; screen diagnostics show black pixels already exist in the guest framebuffer, so GPU, shader, SDL pitch, and PPM conversion are outside the fault boundary.

The disassembled target is the 393840-byte `cfunction.ext` extracted from `test/fixtures/gjxqy.mrp`, SHA-256 `328fdf9361fbbb4b2720479e863be166de74e1c7742e349df0936d94ed8462aa`. It is a raw Thumb module; the evidence below was decoded with `arm-none-eabi-objdump -D -b binary -m arm -M force-thumb`.

## Installed-slot record

The loader begins at `cfunction.ext+0x460F0`. At `+0x4610A..+0x46116` it resolves literal `+0x596D4`, the string `\res.v`; `+0x46118..+0x46126` opens it and returns `-1` for a null handle. `+0x4613C..+0x46148` reads exactly 12 bytes and requires the read result to equal 12. `+0x4615E..+0x46172` copies three 32-bit words, and `+0x46174..+0x46182` verifies `(word0 | word1) XOR 0xA45F987C == word2`. On success, `+0x46184..+0x4618C` returns word0 and word1 to its caller.

The fixture bytes are `f0 03 00 00 01 00 00 00 8d 9b 5f a4`: version 1008, installed-slot value 1, and the valid checksum. The runtime therefore follows the slot-1 resource path.

## Runtime resource open and failure

The resource-open routine begins at `cfunction.ext+0x4F1DA`. At `+0x4F1F0..+0x4F1FC` it loads literal `+0x59704`, the string `\res.bin`, and appends it to the package path. `+0x4F1FE..+0x4F20C` gets the path length, reads the selected slot from `[sp,#20]`, adds ASCII `'0'`, and writes that byte at the end. Slot 1 therefore produces `\res.bin1`.

`+0x4F20E..+0x4F21C` passes that assembled path to the file-open wrapper at `+0x384F8`. At `+0x4F21E..+0x4F222`, a null handle sets `r0=-1` and exits. There is no branch from this failure to `res.p0..res.p39`; the missing `res.bin1` open is the exact runtime failure boundary.

## Installer assembly path

The separate installer routine begins at `cfunction.ext+0x3AC8`. `+0x3BB6..+0x3BCE` loads `\res.bin` and appends the resource slot digit, again yielding `\res.bin1`. `+0x3BD0..+0x3BDA` allocates a `0x8000`-byte transfer buffer, and `+0x3BE4..+0x3BEC` opens the output file. After `+0x3BFE` initializes part index zero, `+0x3C1E..+0x3CC0` loads `res.p` and appends the decimal part index. `+0x3CC2..+0x3CEC` reads that indexed package entry through `+0x38698`; `+0x3CFC` writes the extracted bytes through `+0x3AA34`; and `+0x3D00..+0x3D04` requires the written length to equal the extracted length. `+0x3D0C..+0x3D18` flushes, increments the part index, and loops only when the part length is the full `0x8000` bytes.

This proves that loose `res.p*` files are installer inputs concatenated into `res.bin1`, not runtime fallback assets. Numeric concatenation of `res.p0..res.p39` yields exactly 1294042 bytes and SHA-256 `11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07`.

## Repair and verification

The fixture had a valid slot-1 `res.v` and all 40 installer inputs but no installed `res.bin1`. Adding the exact concatenated file made the strengthened target pass twice with retry disabled:

- `/tmp/skyengine-e2e-qAGOva`: 22.16s; `scene-next.ppm` SHA-256 `c977b398c8ee23073750640c92c62b16155ada06ad3d740236a9a9f775c685a1`, 280 colors, 26383/76800 black pixels (34.353%).
- `/tmp/skyengine-e2e-oU0rfb`: 23.07s; `scene-next.ppm` SHA-256 `c2a5893cef4142c0deb8aa67011dd3938ffbe22a4963b80781f2b9adaf475b60`, 279 colors, 26318/76800 black pixels (34.268%).

Both retained 240x320 frames visibly contain the complete tavern interior, NPCs, furniture, floor, player, and UI. Their animation-dependent hashes differ, while all four exact structural pixels and the greater-than-200-color assertion remain stable. The regression test also pins the installed-resource size and hash.

Final compatibility checks passed: `pnpm exec tsc --noEmit`; `cmake --build build --target skyengine -j2`; `git diff --check`; and `pnpm vitest run test/e2e --retry=0` with 30 files and 53 tests in 280.27s. `ctest --test-dir build --output-on-failure` found no tests, consistent with `docs/TESTING.md`, which defines Vitest as the sole automated test entry point.

This is a fixture consistency repair, not an application, address, or pixel-specific runtime fallback. Related package-installation context is recorded in [[gjxqy-local-download-protocol-and-fixture-id-mismatch]].
