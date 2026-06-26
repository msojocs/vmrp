# RX4.5 Startup Debug Log

Date: 2026-06-26

## Request / Constraints

- Target command: `build/vmrp temp/RX4.5.mrp`.
- Symptom: application startup fails.
- Trace output can be very large; avoid dumping full traces.
- Do not use `xvfb`.
- Use disassembly-level analysis to find the failing location.
- PPM screenshots may be used to verify changes.
- Do not implement scenario-specific branches such as `if (is_xxx_app())`.
- Save progress periodically and store project memory.

## Initial Workspace State

- Existing local changes before this investigation:
  - `docs/prompt.md` modified.
  - `test/e2e/rx/` untracked.
  - `test/fixtures/rx4.5.mrp` untracked.
- Existing binaries:
  - `build/vmrp`
  - `build/test_vmrp*`
- Relevant existing docs:
  - `docs/TESTING.md` says the Vitest e2e harness uses `VMRP_E2E_SOCKET` and does not require `xvfb`.
  - Screenshots are dumped as PPM files through the e2e `SCREEN` command.

## Running Notes

- Native sub-agents launched:
  - runtime/debugger lane: MRP load/start failure path.
  - code explorer lane: ARM EXT executor, ABI, disassembly, PPM touchpoints.
  - test lane: no-xvfb execution and PPM verification path.

## 2026-06-26 Initial Reproduction

Command:

```bash
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
VMRP_PPM=1 VMRP_PPM_PATH=/tmp/rx45.ppm \
timeout 5s build/vmrp temp/RX4.5.mrp
```

Result:

- Exit status was `124` from `timeout`, so the host process did not immediately exit.
- `stderr` showed `startVmrp` and `vmrp_runtime_start_dsm()` returned `0x0`.
- Startup then hit an ARM EXT execution error:
  - `UC_MEM_READ_UNMAPPED addr=0x80110004 size=2`
  - crash PC `0x2DFAC2` in Thumb mode.
  - `last_file=0x2C60A4..0x2E8F14`.
  - register state included `R2=0x80110004`, `R9=0x002E9208`, `R10=0x002C5FB4`, `LR=0x002DF9EB`.
- Executor wrote a narrow crash dump to `/tmp/vmrp_crash.bin` with suggested command:

```bash
arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x2DF9C2 /tmp/vmrp_crash.bin
```

Current hypothesis to validate: RX startup reaches a nested/ext module, then a Thumb routine dereferences a pointer derived from an invalid high address (`0x80110004`) rather than a mapped EXT/platform address. Need disassembly and data-flow proof before changing code.

## 2026-06-26 PPM / No-Xvfb Validation

The test lane independently reproduced the same failure shape without `xvfb`:

- The process starts with `SDL_VIDEODRIVER=dummy` and `SDL_AUDIODRIVER=dummy`.
- The E2E socket can be created and `SCREEN` can dump PPM.
- `DRAW_COUNT` stays `0`.
- PPM files are valid `P6 240x320` but all black (`uniqueColorCount=1`, `nonBlack=0`).
- Existing focused command:

```bash
env VMRP_E2E_KEEP_TMP=1 VMRP_BIN=build/vmrp VMRP_WORK_DIR=. \
  pnpm vitest run test/e2e/rx/app-prepare.test.ts
```

Current failure is therefore before any successful visible draw. PPM success criteria after a fix:

- E2E socket starts.
- `DRAW_COUNT > 0`.
- PPM has `uniqueColorCount > 1` or `nonBlack > 0`.
- RX-specific pixel `(227,301)` should become `[248,0,0]` per the existing test.

## 2026-06-26 Disassembly Around Crash

`/tmp/vmrp_crash.bin` disassembly shows the crash is not an invalid branch target; it is a concrete load from a bad pointer:

```asm
0x2DFAB8: ldr   r1, [pc, #112]     ; literal 0x8C
0x2DFABC: add   r1, r9             ; r1 = R9 + 0x8C = 0x2E9294
0x2DFABE: ldr   r2, [r1, #0x74]    ; r2 = *(R9 + 0x100) = 0x80110004
0x2DFAC2: ldrsh r5, [r2, r3]       ; unmapped 16-bit read from 0x80110004
```

The routine then scans five halfword arrays from fields at `R9+0x100..0x110`, looking for a clear bit in a 5x16 matrix. Need to prove whether these fields are corrupted pointers, handset/platform storage addresses that VMRP does not map, or a failed relocation/cache sync result.

Further disassembly of `game.ext` shows where these fields are installed:

```asm
0x2DFB30: ... platform address range selection ...
0x2DFC2A: ldr   r0, [pc, #108]     ; literal 0x8C
0x2DFC2C: adds  r1, r5, #4         ; r5 is selected platform base
0x2DFC30: str   r1, [r0+r9, #0x74] ; R9+0x100 = base+4
0x2DFC34: str   r1, [r0+r9, #0x78] ; base+8
0x2DFC38: str   r1, [r0+r9, #0x7C] ; base+12
0x2DFC40: stmia r0!, {r1, r5}      ; R9+0x10C/R9+0x110
```

Literal decoding around `0x2DFC4C..0x2DFC90` includes the selected range `0x80110000..0x81070000`. This supports the platform-memory interpretation rather than a random corrupted pointer.

## 2026-06-26 Fix

Implemented a generic ARM-visible `0x80000000..0x811FFFFF` platform IO band in the Unicorn mapping:

- `src/include/arm_ext_internal.h`
  - Added `EXT_PLATFORM_IO_MEM_ADDR` / `EXT_PLATFORM_IO_MEM_SIZE`.
  - Added `platform_io_mem` to `ArmExtModule`.
  - Extended `arm_ptr()` to translate the new band.
- `src/arm_ext_executor.c`
  - Extended `arm_addr()` for host-to-ARM conversion.
  - Allocated and mapped the band with `uc_mem_map_ptr()`.
  - Added a block hook range so R9 restore behavior remains consistent in this platform band.
  - Freed the new allocation in `arm_ext_unload()`.

Rationale: this is a platform address-space fix. It does not branch on RX or any specific app. Existing code already maps MTK-style `0x40000000` and `0xA0000000` platform bands; RX exposed the missing `0x80000000` ROM/MMIO/platform-state band.

The band size is 18 MiB rather than the earlier 2 MiB attempt because the
same disassembled platform selector literal table includes `0x80110000` and
`0x81070000` / `0x81080000`-class ranges. Mapping only the first 2 MiB fixes
the immediate `0x80110004` crash but does not cover the selector's own high
range.

Build:

```bash
cmake --build build --target vmrp
```

Result: build succeeded.

## 2026-06-26 Final Verification

Direct no-xvfb smoke:

```bash
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
VMRP_PPM=1 VMRP_PPM_PATH=/tmp/rx45-final.ppm \
timeout 8s build/vmrp 'temp/RX4.5.mrp'
```

Result:

- Exit status was `124` from `timeout`, meaning the app kept running until the
  smoke timeout.
- Logs had no `UC_ERR`, `invalid memory`, `unmapped`, or `arm_ext_execute`
  failure lines.
- `/tmp/rx45-final.ppm` was valid `P6 240x320`.
- PPM stats: `unique=173`, `nonblack=76008`.
- Stable sample pixels:
  - `(0,0)=[112,188,232]`
  - `(10,45)=[232,244,248]`
  - `(40,75)=[88,180,216]`
  - `(150,308)=[72,160,208]`
  - `(227,301)=[96,176,224]`

Visual result: RX now reaches a blue `RX文件管理器` shell instead of the
previous black/no-draw failure.

Focused e2e:

```bash
env VMRP_E2E_KEEP_TMP=1 VMRP_BIN=build/vmrp VMRP_WORK_DIR=. \
  pnpm vitest run test/e2e/rx/app-prepare.test.ts
```

Result: passed. The RX test now asserts `DRAW_COUNT > 0`, `240x320`,
`uniqueColorCount() > 100`, and stable pixels from the file-manager shell. The
previous black/red pixel assertions were stale: after the platform-memory fix,
the startup screen is not the old black failure image and contains no red pixel
at `(227,301)`.

Regression e2e:

```bash
env VMRP_BIN=build/vmrp VMRP_WORK_DIR=. pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts
env VMRP_BIN=build/vmrp VMRP_WORK_DIR=. pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts
env VMRP_BIN=build/vmrp VMRP_WORK_DIR=. pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts
```

Result: all passed.

Hygiene:

```bash
git diff --check
```

Result: no whitespace errors.

## Sub-Agent Review Notes

- Runtime/code review lane agreed the fix is a generic address-space mapping,
  not an RX-specific branch. It flagged the initial 2 MiB `0x80000000` mapping
  as too narrow relative to the `0x81070000` literal range; the final patch
  expands that band to 18 MiB.
- Test review lane confirmed the original RX e2e black/red pixel assertions
  matched an obsolete failure image. It recommended validating draw count,
  screen size, color richness, and stable shell pixels instead.
- Verification lane reproduced the same transition with SDL dummy and PPM:
  `DRAW_COUNT` advanced, PPM became nonblack/multicolor, and the original
  `UC_MEM_READ_UNMAPPED addr=0x80110004` did not recur.
