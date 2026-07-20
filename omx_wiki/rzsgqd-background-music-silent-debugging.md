---
title: "rzsgqd background music silent debugging"
tags: ["rzsgqd", "audio", "bgm", "arm-ext", "reverse-engineering", "ppm"]
created: 2026-07-20T12:44:26.959Z
updated: 2026-07-20T14:47:47Z
sources: []
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# RZSGQD background music silent debugging

## Objective and constraints

Reproduce and fix the silent default BGM in `build/mythroad/2937_240320_rzsgqd.mrp`. The production fix must be generic, commented, and based on the guest/runtime contract; no application detection, hard-coded package/address behavior, fallback logic, xvfb, or unbounded trace. Final proof must include actual non-zero audio output plus focused and broad compatibility regression.

## 2026-07-20 baseline recovery

- Worktree begins with an unrelated user edit in `docs/prompt.md`; preserve it.
- Existing runtime already implements MIDI, WAV, PCM, and MP3, so unsupported MP3 is not yet established as this sample root cause.
- Three read-only parallel investigations are active: guest disassembly, host audio/media chain, and verification/test coverage.
- Planned evidence: bounded runtime logs, extracted guest disassembly, SDL disk or FFI-rendered PCM statistics, and a PPM proving the application reaches the expected startup state.

## 2026-07-20 failing baseline and bounded runtime evidence

- Package SHA-256 is `f344197f594ab7c71179963e6c8efb03cff79b7eb1c5856d021bbd0e31bf6a02`.
- A 20-second no-xvfb run used `SDL_VIDEODRIVER=dummy`, `SDL_AUDIODRIVER=disk`, and `VMRP_PPM=1`. It produced no WAV/audio capture at all, proving that the SDL audio device was never opened rather than opened with an all-zero primary stream.
- The baseline PPM is `320x240`, SHA-256 `2afe32230944bd164f7a198655a20bc32d9ebf7f067486ae9161f17f1e11b4d0`, and visibly shows the Fruit main-menu background plus enabled music-note icon. The application is initialized and drawing; this is not a startup crash or blank-screen issue.
- A bounded GDB probe, with no instruction trace, hit `mr_platEx(2222)` with the 12-byte input `{src=0x2BC984,len=0xF3C,src_type=1}`, then `mr_platEx(2232)` with the 4-byte input `{handle=1}`. Neither `mr_playSound` nor `native_playSound` was reached.
- Narrow `SKYENGINE_ARM_EXT_DIAG=1` corroborated both calls from primary `game.ext`: runtime file `0x226308`, LR `0x23667D` (`game.ext+0x10375`). Both calls returned `MR_IGNORE=1` because `dsm.c` dispatches media commands 201..216 but omits the declared multichannel commands 222..225.

## Required decode and channel semantics

- The package contains `gamestart.wav`, `combo.wav`, `miss.wav`, `right.wav`, and `mute.wav`. All five are RIFF WAVE with format tag `0x11` (IMA ADPCM); the current WAV decoder accepts only PCM tag 1 and IEEE float tag 3.
- `mute.wav` is 3,900 bytes and decodes to all-zero samples; it is the first multichannel clip opened. `gamestart.wav` is 27,708 bytes, 22.05 kHz mono, about 2.519 seconds, and has strong non-zero PCM. Therefore an acceptance check must reject the incomplete result where only the silent clip is opened/played.
- A complete generic repair needs both parts: implement the declared multichannel OPEN/PLAY/STOP/CLOSE handle lifecycle and decode IMA ADPCM into the existing 44.1 kHz stereo render domain. The final mixer must preserve concurrent clips rather than turning the multichannel API into a single-stream alias.
- Positive compatibility baselines already captured: TalkCat MP3 produces 593,920 bytes with 216,212 nonzero samples; gzwdzjs MIDI produces 884,736 bytes with 440,212 nonzero samples. These will be re-run after the implementation.

## Guest disassembly contract

- `game.ext+0x385C` is `play_record(index)`. Each 24-byte record contains data at `+4`, device at `+8`, length at `+12`, handle at `+16`, and a loop/flag word at `+20`; handles start at `-1`.
- OPEN uses `2220 + device`, a 12-byte input `{guest_data_addr, data_len, loop}`, and stores R0 directly as the handle. The caller retries only for `-1`; `0` is a sticky non-playable result, while every other value is passed to PLAY. A successful OPEN must therefore return a distinct value other than `0` or `-1`, not `MR_SUCCESS`.
- PLAY, STOP, and CLOSE use `2230 + device`, `2240 + device`, and `2250 + device` with the same four-byte `{handle}` input. CLOSE first issues STOP, then CLOSE, then unconditionally stores `-1` in the record.
- Startup opens and plays record 4 (`mute.wav`, loop=1), then scene setup can independently open record 2 (`gamestart.wav`) while retaining record 4. The false `MR_IGNORE=1` result currently aliases both records to handle 1; the repaired runtime must allocate generation-safe independent handles and mix their decoded voices.
- The nested address in OPEN is a 32-bit ARM guest address. `aex_table.c` table 38 must validate and translate it before calling a DSM host-pointer entry; writing a 64-bit host pointer back into the 12-byte guest payload would corrupt the ABI.

## 2026-07-20 worktree necessity audit and incomplete-fix proof

- The current worktree changes 11 source-facing files and adds about 728 lines. The `src/e2e_control.c` SWIPE command is unrelated to the media dispatch/decode path and is not needed to reproduce or verify default startup audio.
- The two large production changes cannot be substituted for one another: implementing command 222..225 dispatch only reaches the decoder, while adding IMA ADPCM only helps calls that already reach the legacy sound API. RZSGQD uses neither legacy path, so both a multichannel bridge/mixer and IMA ADPCM decode are required.
- The ARM bridge's special OPEN handling is necessary on 64-bit hosts. Its 12-byte guest input contains a nested 32-bit address; the bridge validates/translates that address and passes a host pointer through a separate function rather than corrupting the guest payload.
- The rebuilt `build/skyengine` (newer than all changed audio sources) was run for 20 seconds with an isolated work directory, `SDL_VIDEODRIVER=dummy`, `SDL_AUDIODRIVER=disk`, and PPM output. It opened the audio device and wrote 3,465,216 bytes (866,304 stereo frames, about 19.644 seconds), but all 1,732,608 S16 samples were zero.
- The fixed-run PPM remains `320x240` with SHA-256 `2afe32230944bd164f7a198655a20bc32d9ebf7f067486ae9161f17f1e11b4d0`, matching the initialized Fruit menu and enabled music icon. This proves the current implementation has advanced from "no audio device" to "only a silent voice", but has not met the audible-BGM acceptance criterion.
- Next discriminating probe: bounded breakpoints on `dsm_media_open_channel_host`, `native_playSoundChannel`, and the guest `play_record` caller to determine why the non-zero `gamestart.wav` record is not entering the mixer. Do not accept device-open, positive handle, or non-empty disk output without a non-zero-sample assertion.
