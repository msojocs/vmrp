# TalkCat startup crash debug

Date: 2026-07-02

## Request / constraints

- Target command: `build/vmrp build/mythroad/talkcat.mrp`.
- Expected: successfully enter the game main screen.
- Actual: crashes during startup data extraction with
  `arm_ext_executor: uc_emu_start(0xE83A55) failed: 10 (Invalid instruction (UC_ERR_INSN_INVALID))`.
- Constraints: no `xvfb`; avoid broad trace because logs are huge; use
  disassembly evidence; no app-specific branches; no fallback-style fix; keep
  progress in docs; PPM may be used for verification.

## 2026-07-02 09:23 checkpoint

Clean reproduction used a separate work directory so package extraction was not
masked by existing `build/mythroad/talkcat/*` files:

```bash
timeout 120s env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_PPM=1 VMRP_PPM_PATH=/tmp/talkcat-clean.ppm \
  build/vmrp --work-dir /tmp/vmrp-talkcat-work \
  /home/msojocs/github/vmrp/build/mythroad/talkcat.mrp \
  > /tmp/talkcat-clean.out 2> /tmp/talkcat-clean.err
```

Result:

- The process remained alive until timeout, but the ARM callback failed and
  printed the reported invalid instruction error.
- Failure line:
  `arm_ext_executor: uc_emu_start(0xE83A55) failed: 10 (Invalid instruction (UC_ERR_INSN_INVALID))`.
- Register dump:
  - `PC=0x00E7F5F8`, which is inside the wrapper stack page below
    `EXT_CODE_ADDR=0xE80000`, not inside executable wrapper code.
  - `SP=0x00E7FE90`.
  - `LR=0x002D1753`.
  - `R9=0x10E40062`, not a valid wrapper/game RW base.
- PC bytes at the invalid target:
  `E0 FC E7 00 01 00 00 00 90 05 00 00 8C 76 2F 00`.
- PPM was valid 240x320 and nonblack, so rendering begins before this crash.

Focused rerun:

```bash
timeout 70s env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_ARM_EXT_TRACE_PC=1 VMRP_ARM_EXT_DIAG=1 \
  VMRP_PPM=1 VMRP_PPM_PATH=/tmp/talkcat-diag.ppm \
  build/vmrp --work-dir /tmp/vmrp-talkcat-work-diag \
  /home/msojocs/github/vmrp/build/mythroad/talkcat.mrp \
  > /tmp/talkcat-diag.out 2> /tmp/talkcat-diag.err
```

Disassembly anchors from extracted `/tmp/talkcat-mrp/cfunction.ext`:

- `cfunction.ext` is 16940 bytes, mapped at `0xE80000`.
- `0xE83A55` is the wrapper timer dispatcher entry (`0xE83A54` Thumb).
- Recent PC ring ends in `0xE82168..0xE8219E`.
- `0xE82168`:
  - `push {r4,r5,r6,lr}`;
  - calls `0xE831B8` to swap/select wrapper R9;
  - calls `0xE82594`, a wrapper heap allocator using `*(R9+0x14C)`;
  - on failure calls `0xE835AC`, which calls a bridge allocator and writes a
    4-byte header;
  - restores R9 through `0xE831B4`;
  - `pop {r4,r5,r6,pc}` at `0xE8219E`.
- The bad PC is therefore a popped/smashed return address, not an instruction
  at `0xE83A55`.

Most important runtime clue before the bad pop:

```text
DIAG table44 fd=1 name='talkcat/start/1'
  want=4294966297 ret=0x3910 dst=0x3561A4 lr=0x2D239B
  activeP=0x356504 primaryP=0x356504
```

`want=4294966297` is `0xFFFFFC19` (`-999` as signed). The host bridge reads
`0x3910` bytes into `0x3561A4`, which overlaps the primary `P` block at
`0x356504`. That explains the later corrupted R9 and stack return.

The missing native behavior was `mr_plat(MR_GET_FILE_POS, fd)`. `MR_IGNORE` is
`1`, and `-999 == MR_IGNORE - 1000`, matching the bad read length.

## 2026-07-02 fix

Implemented the generic file-position query instead of adding a TalkCat-specific
branch:

- `src/include/fileLib.h`: declared `my_tell(int32_t f)`.
- `src/fileLib.c`: added `my_tell()` using `lseek(fd, 0, MR_SEEK_CUR)`.
- `src/mythroad/include/dsm.h`: added `tell` to `DSM_REQUIRE_FUNCS`.
- `src/native_dsm_funcs.c`: wired `.tell = my_tell`.
- `src/mythroad/dsm.c`: handled `MR_GET_FILE_POS` in `mr_plat()`.

The bridge case directly calls the required DSM `tell` callback. Returning
`MR_IGNORE` is unsafe here because guest EXT code uses the result in length
arithmetic before calling `mr_read()`.

## 2026-07-02 verification

Focused clean-workdir diagnostic:

```bash
rm -rf /tmp/vmrp-talkcat-work-fixed-diag
install -d /tmp/vmrp-talkcat-work-fixed-diag
timeout 90s env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_ARM_EXT_DIAG=1 VMRP_ARM_EXT_TRACE_PC=1 \
  VMRP_PPM=1 VMRP_PPM_PATH=/tmp/talkcat-fixed-diag.ppm \
  build/vmrp --work-dir /tmp/vmrp-talkcat-work-fixed-diag \
  /home/msojocs/github/vmrp/build/mythroad/talkcat.mrp \
  > /tmp/talkcat-fixed-diag.out 2> /tmp/talkcat-fixed-diag.err
```

Result:

- Exit status was `124` from `timeout`, meaning the emulator kept running until
  stopped.
- No matches for `FFFFFC19`, `want=4294966297`, `uc_emu_start`, `UC_ERR`,
  invalid memory/unmapped failures, or `1231` not-implemented logs.
- `MR_GET_FILE_POS` now returns real file offsets. For the old crash file:

```text
DIAG table37 code=1231 param=0x1 ret=0x3910 ...
DIAG table44 fd=1 name='talkcat/start/1' want=13608 ret=0x3528 dst=0x2F1E64 ...
```

The old `0xFFFFFC19` request is gone; `13608 == 0x3910 - 1000`, which matches
the guest's payload-length calculation.

- The clean work directory extracted 92 TalkCat files, including
  `talkcat/start/1`, `talkcat/sys.dat`, `talkcat/listen/1`, `talkcat/talk/1`,
  yawn resources, sneeze resources, icons, and mp3 assets.
- PPM capture was valid and nonblank:
  `P6 240x320`, `pixel_bytes=230400`, `unique=3541`,
  `nonblack_pixels=76446`.
- Converted `/tmp/talkcat-fixed-diag.ppm` to
  `/tmp/talkcat-fixed-diag.png` for visual inspection. It shows the TalkCat
  main scene in the brick alley. There is still horizontal visual noise, which
  appears separate from the startup extraction crash.

## 2026-07-02 completion audit

Normal clean-workdir startup without heavy ARM diagnostics:

```bash
rm -rf /tmp/vmrp-talkcat-goal-audit2 /tmp/talkcat-goal-audit2.ppm
install -d /tmp/vmrp-talkcat-goal-audit2
timeout 120s env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_PPM=1 VMRP_PPM_PATH=/tmp/talkcat-goal-audit2.ppm \
  build/vmrp --work-dir /tmp/vmrp-talkcat-goal-audit2 \
  build/mythroad/talkcat.mrp \
  > /tmp/talkcat-goal-audit2.out 2> /tmp/talkcat-goal-audit2.err
```

Result:

- Exit status was `124` from `timeout`, not a crash.
- No matches for `uc_emu_start`, `UC_ERR`, `Invalid instruction`, invalid
  memory/unmapped failures, `FFFFFC19`, `want=4294966297`, or `1231` missing
  implementation logs.
- Clean extraction produced 92 files under
  `/tmp/vmrp-talkcat-goal-audit2/mythroad/talkcat`.
- PPM capture was valid and nonblank:
  `P6 240x320`, `pixel_bytes=230400`, `unique=3397`,
  `nonblack_pixels=76451`.
- Converted `/tmp/talkcat-goal-audit2.ppm` to
  `/tmp/talkcat-goal-audit2.png`; visual inspection shows the TalkCat main scene
  in the brick alley.
