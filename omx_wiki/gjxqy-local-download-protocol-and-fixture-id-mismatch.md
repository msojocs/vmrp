---
title: "GJXQY local download protocol and fixture ID mismatch"
tags: ["gjxqy", "network", "mrp", "e2e", "disassembly", "ppm"]
created: 2026-07-22T04:41:05.712Z
updated: 2026-07-22T07:43:48.000Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# GJXQY local download protocol and fixture ID mismatch

Target: `pnpm vitest run test/e2e/gjxqy/download-res.test.ts`. The archived endpoint is reproduced by `tool/jxqy-server.js`. The supplied `temp/gjxqy-res/res_1.mrp` is preserved unchanged; the passing workflow serves the separately documented `test/fixtures/gjxqy-derived/res_1.mrp` compatibility fixture.

## Protocol

The client sends `POST / HTTP/1.1` to archived host `211.155.236.18:10005`, with a 161-byte binary body. Its envelope is 44 bytes, command `102003`, followed by three 13-byte descriptors with types `2/11/12`. The type-2 descriptor second big-endian u32 is the opaque resource identity `0x000F983C`.

The response must contain a 44-byte echoed transaction envelope with command `202003`, then status `000000c800`, count `0001`, one 31-byte descriptor, and the 273877-byte MRP. The response descriptor second u32 at wire offset +5 must echo `0x000F983C`. Disassembly at `cfunction.ext+0x2EB8` proves request serialization, `+0x2F94` parses the response identity, `+0x21AA` compares it, `+0x21BA` emits event 5, and `+0x21C4` sets the success latch. A zero identity writes the file but later emits event 6.

The strict server validates the request line, archived host headers, content type, exact envelope/version/length/command, descriptor count and descriptor types. It has no guessed fallback, resource-ID branch, app detection, or timing-dependent packet split. One `socket.end()` sends logical frame lengths `[102, 44, 5, 2, 31, 273877]`.

## Direct download evidence

The source fixture is 273877 bytes with SHA-256 `ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1`. The E2E watches the exact configured destination `mythroad/gjxqy/res_1.mrp`, opens the inode as soon as the guest creates it, waits for all 273877 bytes, and hashes the client-written file. Holding the read-only descriptor preserves evidence after the guest installer unlinks the incompatible path.

Run artifact `/tmp/skyengine-e2e-YNwMAJ` and workspace `/tmp/skyengine-ws-ZUvqwd` proved the client-written length and SHA-256 match the original, and all server transaction assertions pass. This upgrades evidence from bytes queued by the server to bytes actually written by the client.

## Fixture incompatibility

Installation is a separate failing stage. Startup `cfunction.ext+0x37B28` loads literal `0x000F983C` from `+0x37B74` and registers it directly into resource slot 1. Installer `+0x3B6C..+0x3B82` decodes the four-byte big-endian `fileid.bin`; `+0x3B92` directly compares it against the slot value. The supplied resource contains `fileid.bin=0x00000FA1` or 4001, so installation returns -2 and deletes the download.

No conversion, mapping table, extra response descriptor, or protocol field changes this table. The current main fixture and build main share the same wrapper; changing MRP header version 1008 to 1012 did not change the compiled resource ID. A controlled resource copy with only `fileid.bin` changed to `0x000F983C` installed `res.bin1`, advanced `res.v`, and resumed the real save-selection scene, proving causality, but it is synthetic and is not accepted as the original asset.

The known installation-failure PPM has 3 colors and SHA-256 `c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`. The controlled compatible diagnostic produced a 48-color game frame with SHA-256 `faec61c188b3b272d1b709e743e46083a64071320cebfac41d52be788ffdb9cd`.

## Constraints and status

Internet and public archive searches are prohibited. A local-only audit of all repository Git objects, 3833 visible MRPG files, existing run artifacts, other local repositories, and already stored caches found no authentic matching main/resource pair and no ID mapping logic. Do not rewrite the supplied package in place, dynamically patch server output, bypass installer validation, or add app/address-specific runtime behavior.

The network download objective was directly proven before the asset correction. The final test keeps the positive PPM gate and uses a separately named derived compatibility fixture, with its exact transformation and provenance recorded rather than representing it as an authentic release asset.

## Extracted `temp/gjxqy` directory

The later supplied local `temp/gjxqy` directory is a mixed overlay, not a matching main package or a completed slot-1 installation. Its base `res.bin0` and `res.i0..res.i9` exactly match the existing fixture. Its `res_1.mrp` exactly matches the original 273877-byte package. Parsing all 42 MRP entries proves the on-disk `fileid.bin` and `res.p0..res.p39` are byte-identical decompressions of 41 package entries; only the generic `start.mr` entry is absent.

The extracted `fileid.bin` remains big-endian `0x00000FA1`. Numerically concatenating the 40 parts produces the known 1294042-byte `res.bin1` payload with SHA-256 `11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07`, but the directory itself has no `res.bin1`. It also has no EXT or main wrapper and contains neither byte order of `0x000F983C`.

Its `res.v` is `f0030000010000008d9b5fa4`, a valid version-1008/slot-1 record identical to `build/mythroad/gjxqy/res.v`. The missing `res.bin1`, unpacked parts, and extraction timestamps show this is overlaid metadata rather than proof that this package passed the real installer. The directory therefore confirms package provenance but does not resolve the ID mismatch.

The current `build/mythroad/gjxqy_v1008.mrp` and misleadingly named `gjxqy_v1012.mrp` are byte-identical, both with LE/BE header version 1008 and SHA-256 `5513be4147aeb084b6411af5b4343af78943c04a0db7aa0dd8d7014eb95113b7`. The latter filename therefore does not provide a second v1012 main package. The existing `test/fixtures/gjxqy.mrp` alone has header version 1012, with the same wrapper payload.

The later-added `test/fixtures/gjxqy_v1012.mrp` is also byte-identical to both build files: 423136 bytes with the same `5513be...13b7` hash and version 1008 in both header encodings. Its extracted 393840-byte `cfunction.ext` has SHA-256 `328fdf9361fbbb4b2720479e863be166de74e1c7742e349df0936d94ed8462aa`; literal `+0x37B74` is still little-endian `0x000F983C`, and neither byte order of 4001 occurs anywhere in the extension. The new filename therefore does not resolve the slot-ID mismatch.

A controlled v1008 launch copied this directory into a temporary workspace and mapped the archived host to an unbound local address. After stable music/menu navigation it reached the 52-color save-selection UI without any network initialization and without creating `res.bin1`; `res.v` remained unchanged. Artifact `/tmp/skyengine-e2e-TRyLXB`, workspace `/tmp/skyengine-ws-nmT56M`, and final PPM SHA-256 `716062dc5c8a1def9993b7617238d5b1d9911b84b5a889541f241865ee9a85a5` prove that the preset slot flag bypasses download gating. This positive PPM is not evidence of package installation, and using the directory as the target fixture would prevent the reproduced server from being exercised.

## Current-state revalidation and provenance boundary

The current worktree was re-run without trace or xvfb. Artifact `/tmp/skyengine-e2e-OkExWT` and workspace `/tmp/skyengine-ws-6yZQ0O` again prove the original 273877-byte resource was written by the client with SHA-256 `ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1`; every protocol assertion passed. The only failed assertion was the positive PPM gate: the final image still has three colors, SHA-256 `c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`, and visibly reports the resource-download failure. No `res.bin1` exists and `res.v` is unchanged after the installer deletes the incompatible package.

Three independent audits agreed on the direct opaque-ID comparison and found no emulator-side conversion. One provenance hypothesis was explicitly rejected: MRP header byte `+0xD0=1` is the retained packer's MTK/MStar platform enum, not an offline-resource marker. The 3197-byte `start.mr` is a generic loader, the resource package contains no `cfunction.ext`, and the game installer does not consume `start.mr`. The asset can therefore only be classified as a structurally valid resource-1 MRP incompatible with this wrapper; present evidence cannot distinguish online payload from an offline copy or a different release variant. The server must not rewrite `fileid.bin` under an unsupported format-conversion claim.

The one-shot fixture server now claims its transaction only after full validation, ignores an empty TCP probe, rejects a nonempty truncated request at EOF, validates the MRP header's declared total length, and closes the CLI listener after completion or protocol failure. The no-SDL test `test/e2e/gjxqy/server.test.ts` passes both the split valid request after an empty probe and the truncated EOF case. The test-only DNS map remains locally injected; the unrelated global loopback mapping and global retry change were removed.

After rebuilding, the post-change target run retained artifact `/tmp/skyengine-e2e-VBgSum` and workspace `/tmp/skyengine-ws-ZqUVty`. It again failed only at the positive PPM gate with three colors and the same `c1f9a...b47e` image; `res.v` remained `f403000000000000889b5fa4`, `res.bin1` was absent, and the installer removed `res_1.mrp`. This confirms the server lifecycle work did not shift the established network-success/fixture-rejection boundary. The CMake build, TypeScript check, JavaScript syntax check, diff check, 2/2 server tests, and CTest unit target all pass.

## Local generation and provenance audit

Three additional local-only audits closed the remaining metadata-generation hypothesis without changing assets. A full Git object scan covered 6,772 objects, 3,377 blobs of at least 240 bytes, and 42 MRPG packages; none has `appid=335001`. The current main package, resource package, `res.v`, and `fileid.bin` blobs have no reachable, unreachable, reflog, stash, or LFS provenance in this repository.

`test/fixtures/gjxqy.mrp` is not an independent v1012 build: it differs from the v1008 package only at file offsets `0x48` and `0xC7`, where the version low bytes were changed from 1008 to 1012. Its other 423,134 bytes, including `cfunction.ext`, are identical. The file named `gjxqy_v1012.mrp` is byte-identical to the v1008 package and still declares version 1008.

CLR metadata, IL, all user strings, and all 11 manifest resources in `tool/Mrpeditor.exe` contain no `fileid.bin`, `res.pN`, 4001, or `0xF983C` logic. `Module1::ReadResource` only reads image-width mappings from `Mrper.rs`; that file is a width database, not an online resource-ID source. The repository's `ToMrp`, editor, and applist tools are generic container writers and contain no GJXQY configuration or `fileid.bin` generator.

After deduplication, the only locally available implementation of this resource manager is the current GJXQY wrapper, and the only real `fileid.bin` sample is the current resource package. There is no independent wrapper/resource pair from which to infer a rule. In particular, `4001 = 40 * 100 + 1` remains an unsupported numerical coincidence rather than a format contract.

The final local-original boundary is therefore asymmetric but exact: the current wrapper authoritatively declares `0x000F983C`, while the supplied resource package authoritatively declares 4001. Both values can be extracted from their binaries; neither can be regenerated from authoritative local source or metadata, and no evidence maps one to the other. The completed test solution therefore uses a clearly labeled derived compatibility fixture and does not represent it as the original release package.

## Explicit derived fixture and completed verification

`tool/replace-mrp-entry.js` parses the MRP header/index and replaces exactly one entry only when both decoded and packed lengths remain unchanged. It is generic container tooling: it contains no GJXQY detection, runtime rewriting, fallback, or installer bypass. The fixture reproduction command replaces `fileid.bin` with big-endian `000f983c` and writes `test/fixtures/gjxqy-derived/res_1.mrp` while leaving the supplied package untouched.

The deterministic output is byte-identical to the earlier controlled successful artifact. It differs from the original at only ten bytes, all within the 24-byte gzip representation of `fileid.bin`; MRP offsets and every `res.p0..res.p39` payload remain unchanged. Original SHA-256 is `ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1`; derived SHA-256 is `94ebbc1cb0b2c0dea74abe0bcf8b656b9ba3a5eb3d10c83b08c3ae1232fe9448`.

The E2E now pins the derived source hash and the client-written download hash, then requires installed `res.bin1` to be 1,294,042 bytes with SHA-256 `11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07` and `res.v` to become exactly `f403000001000000899b5fa4`. Stable save-selection title, slot, and background pixels plus at least 48 colors distinguish success from the three-color black failure overlay; the transient bottom-left confirmation icon is deliberately excluded from a full-frame hash.

The final post-edit no-retry target run passed in 44.74 seconds with artifact `/tmp/skyengine-e2e-VOGsHc` and workspace `/tmp/skyengine-ws-IIka3o`. The no-retry full E2E run passed 32/32 files and 56/56 tests in 338.56 seconds, including GJXQY in 47.65 seconds. CMake build, CTest, TypeScript, Node syntax checks, deterministic fixture regeneration, the 2/2 fixture-server tests, and diff whitespace validation also pass. Neither run used xvfb or trace logging.
