# gwkdl image loading debug log

## 2026-06-23

- Task: fix `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` where the game reaches the play screen but rendered images do not appear.
- Constraints: no `xvfb`; avoid broad trace output; use disassembly to locate the bug; validate via PPM when possible; avoid app-specific branches; keep code comments on behavioral changes.
- Initial repository state: `docs/prompt.md` is already modified by the user; `test/e2e/gwkdl/` and `test/fixtures/gwkdl_v1003.mrp` are untracked user-provided inputs. These files are treated as existing task context.
- First read: the gwkdl test prepares `netpay.mrp` and `advbar.mrp`, starts `test/fixtures/gwkdl_v1003.mrp`, captures `bgm-select`, clicks "no music", then captures `menu`. The test currently throws `TODO` after that capture, so it is usable as a repro driver but not yet as a complete assertion.
- Reproduced with the E2E socket directly so temporary artifacts stay available at `/tmp/gwkdl-repro-WhU4WU`.
- PPM analysis:
  - `bgm-select.ppm`: 240x320, 2 unique colors; black has 76365 pixels, yellow `[248,240,0]` has 435 pixels.
  - `menu.ppm`: 240x320, 2 unique colors; blue background `[16,96,168]` has 76499 pixels, yellow `[248,240,0]` has 301 pixels.
  - This confirms the frame loop and text/rectangle drawing paths are alive, but the expected bitmap/image pixels are not copied into the screen buffer.
- Package string scan shows `game.ext`, `cfunction.ext`, `verdload.ext`, `graphics.ext`, and many BMP resources such as `bd.bmp`, `b_2.bmp`, `logo0.bmp`, and `npc1.bmp`.
- Existing `VMRP_ARM_EXT_DIAG` run shows gwkdl repeatedly presents through ARM helper table[29] (`mr_drawBitmap`) from the primary game module with `x=0 y=0 w=240 h=320`.
- Temporary narrow table[29] diagnostics after rebuild:
  - `bmp=0x2001BC`, `screen=0x2001BC`, full-screen rectangle.
  - Frame samples transition through `sample0/sample120x160 = 0x0000`, then `0xFFFF`, then `0x1315`.
  - `0x1315` maps to the observed blue `[16,96,168]`.
  - Conclusion: SDL/PPM present is copying the intended ARM screen buffer. The missing image pixels are already absent before present, so the bug is in image resource decode/composition into the ARM screen buffer, not in `guiDrawBitmap`.
- MRP directory enumeration of `test/fixtures/gwkdl_v1003.mrp` shows 57 entries. Early entries include `game.ext`/`graphics.ext`; image resources continue far past index 16, including `logo0.bmp` at index 54 and `logo1.bmp` at index 55.
- Fixed the generic MRP cache hard limit by converting `ArmExtModule::mrp_cache` from a fixed 16-entry array to a dynamically grown array. This matches the existing comment "cache all entries" and is not app-specific.
- Rebuilt successfully with `cmake --build build --target vmrp`.
- `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` still fails after the cache change: menu PPM reports only 2 unique colors. Therefore the fixed 16-entry cache was a real generic bug but is not the root cause of this gwkdl image path.
- Focus shifted to the ARM-side resource loader. Disassembly around `game.ext` `0xE96052` shows a generic loader that:
  - handles `$`/`*` in-memory resources first;
  - otherwise opens a filename through the C table;
  - reads a 16-byte MRP header;
  - scans variable-length MRP directory names and offsets;
  - reads the selected payload for decode.
  The next check is whether host `open/read/seek` presents the main MRP bytes in exactly the layout expected by this ARM scanner.
- Manual E2E repro retained artifacts at `/tmp/vmrp-e2e-3wKg7e`:
  - `bgm-select.ppm` unique colors: 2.
  - `menu.ppm` unique colors: 2.
  - stdout/stderr contain no direct `_mr_readFile`/`BitmapLoad` failure for the missing game art.
  - stdout repeatedly opens the absolute main MRP path, which matches the disassembled private ARM resource loader path rather than the host `_mr_readFile` helper path.
- Added temporary `VMRP_ARM_EXT_FILE_DIAG` diagnostics and reproduced at `/tmp/vmrp-e2e-DSv81x`.
  - The main MRP header read returns `4D525047 EF050000 C4680500 F0000000`, matching `MRPG`, index length `1519`, total size `354500`, index start `240`.
  - The private ARM loader seeks to `224` from current position after reading the 16-byte header, reads the full `1287`-byte index, and later seeks to resource offsets from the directory.
  - Gzip payload reads succeed for image resources including `npc1.bmp`, `logo0.bmp`, `logo1.bmp`, `mk.bmp`, `if.bmp`, `zm.bmp`, and the unnamed packed resources (`a`, `bg`, etc.).
  - No file bridge failures explain the missing images; the bug is now narrowed to resource decompression/format decode, draw invocation, or later overwrite/layer handling.
- Resumed investigation from the narrowed point above. Next checks are focused on:
  - extracting and disassembling the ARM EXT modules from `gwkdl_v1003.mrp`;
  - comparing decoded bitmap buffers before `_DrawBitmap` with the pixels presented by table[29];
  - avoiding full instruction trace because it is too large for this app.
- Added a temporary narrow `VMRP_ARM_EXT_BITMAP_DIAG` probe around host table[120] (`_DrawBitmap`) and rebuilt.
  - Repro artifact: `/tmp/gwkdl-bitmapdiag-cgymnf`.
  - The menu PPM is still only 2 colors: blue background plus yellow text.
  - No table[120] bitmap calls were logged before/after entering the menu.
  - Conclusion: this app path does not use the host `_DrawBitmap` bridge for the missing menu art. It presents the ARM framebuffer through table[29], so the image path must be ARM-side framebuffer writes, ARM-side resource decode, or a bad ARM data/function pointer before the final present.
- Added temporary table-call diagnostics and reproduced at `/tmp/gwkdl-tablediag-oGYwft`.
  - Call histogram before the menu capture: `table[3] memcpy=58117`, `table[119] _DrawPoint=11104`, `table[122] DrawRect=85`, `table[29] mr_drawBitmap=64`, plus file/memory calls.
  - There are no calls to `table[120] _DrawBitmap`, `table[121] _DrawBitmapEx`, or `table[147] mr_transbitmapDraw`.
  - All `_DrawPoint` calls write color `0xFF80`, matching the yellow text pixels in the PPM.
  - All menu `DrawRect` calls come from LR `0x65B615`; most are full-screen `0,0,240,320`, matching the pure blue background.
  - Large resource copies do happen, for example `memcpy(0x68C1DC, 0x683048, 37264)` for `logo0.bmp`-sized data and other 565 image buffers. None of those copies target the ARM framebuffer at `0x2001BC`.
  - Conclusion: resource extraction/decompression is alive, but the active menu draw path never blits those decoded buffers to the screen. The next target is the ARM-side draw dispatcher/function-pointer setup, not SDL presentation or host `_DrawBitmap`.
- 12:37 resumed from the existing repro and diff. The next narrowed target is private child module setup/bridge repair, especially `graphics.ext`, because the decoded buffers are present in ARM heap while the only visible drawing calls are text points and background rectangles.
- Thumb disassembly correction: the earlier plain ARM objdump output mis-decoded mixed ARM/Thumb child code. Re-reading in forced Thumb mode shows `graphics.ext` helper at `0x671141` and its init path copies a table-like block from the parent record into child RW. The suspected failure is now a generic private-loader bridge/RW layout issue rather than the resource file bridge.
- Added a temporary child-sync diagnostic and ran the target with `VMRP_E2E_KEEP_TMP=1`; artifact `/tmp/vmrp-e2e-cEBwNv`.
  - `graphics.ext`: `file=0x670C98`, `len=0x1FC0`, `P=0x6E95A4`, `H=0x671141`, `rw=0x672C5C`, `record=0x6E9300`, `ext_type=1`.
  - The record contains host table bridge entries, but the child RW had no bridge values before repair.
  - A generic compact SDK bridge layout (`record[26] -> rw+0x20`, `record[3..19] -> rw+0x24..0x64`) was tested; it did not change the PPM result (`menu.ppm` still 2 colors). This reduces confidence that the missing image is only a basic memcpy bridge issue.
  - The same log also reports `exRam:-3221504B required.`, suggesting either an ARM-side allocator/global-state misread or a failed scratch-buffer path; this remains under investigation.
- 13:05 resumed from the handoff. Current focus is to verify with a narrow block hook whether the `graphics.ext` Thumb blit routines around `0x671188` and `0x672000` execute at all, then decide whether the defect is call dispatch, bridge layout, allocator state, or framebuffer ownership.
- 13:18 added `VMRP_ARM_EXT_GFX_DIAG` as a narrow basic-block hook and reproduced at `/tmp/vmrp-e2e-N2vCF3`.
  - `menu.ppm` is still 2 colors: `[16,96,168]` background plus yellow text.
  - The graphics code does execute. The log reaches `0x67200C -> 0x6720C8 -> 0x671180`, then prints `exRam:-3221504B required.`.
  - This shifts the active root-cause search from "graphics library not called" to "graphics library draw/decode path computes a corrupt scratch RAM size or reads a corrupt global before blitting".
- 13:32 resumed from handoff. `VMRP_ARM_EXT_EXRAM_DIAG` had shown no host table[38] calls for 1014/1015 around this failure, only code 1204. Therefore the negative `exRam` line is generated by `graphics.ext` internal accounting/scanning code, not by the host `mr_platEx(1014)` branch directly returning a bad value.
- Current next probe is intentionally narrow: inspect `graphics.ext` Thumb routines around `0x671DB4`, `0x6722F8`, `0x672844`, `0x672970`, and `0x6729A4`, plus the RW words near `r9 == 0x672C5C`, to find which global/list entry produces the `0xFFCED800` size.
- 13:00 ran `VMRP_ARM_EXT_GFX_MEM_DIAG=1` at `/tmp/vmrp-e2e-W5NeIg`; menu remains 2 colors. No `GFX_MEM_DIAG` lines appeared, which means the exact PCs selected were not basic-block entry addresses. The next revision matches narrow ranges around the same disassembled functions rather than exact addresses.
- 13:06 reran with range-based `GFX_MEM_DIAG` at `/tmp/vmrp-e2e-KmlEQq`.
  - The target gwkdl menu is still 2 colors.
  - `graphics.ext` executes with `r9=0x672C5C`.
  - The local queue area around `r9+0x84` stays zero before the negative `exRam` print.
  - The last captured graphics block before the message is around `0x671E52`, the wrapper that calls the stream renderer.
  - A previously noticed `/tmp/vmrp-e2e-R5Vhnk` run with `exRam:312320B required.` was for `gghjt.mrp`, not this gwkdl fixture, so it is not a valid success comparison.
  - Next narrow probe: log the LR for table[26] `mr_printf` when its format string contains `exRam`, then disassemble the exact print call site.
- 2026-06-23 resumed again from the existing diagnostics. Current narrow target is the
  exact `graphics.ext` print caller for `exRam`, because resource reads and gzip
  payload copies are already confirmed, while the final screen only receives
  background rectangles and text points.
- `VMRP_ARM_EXT_PRINTF_DIAG=1` repro artifact: `/tmp/vmrp-e2e-SckjzE`.
  The target test still fails with `menu.ppm` having only 2 colors. The
  `exRam` print is called with `lr=0x64664D`, `fmt=0x662CB8`, `r1=0xFFCED800`
  (`-3221504`), and `r9=0x66B754`. Next step is disassembling the loaded child
  module around `0x64664D` and tracing the source of that signed size.
- `VMRP_ARM_EXT_CHILD_DIAG=1 VMRP_ARM_EXT_PRINTF_DIAG=1` repro artifact:
  `/tmp/vmrp-e2e-ZGK74Q`. This corrects the previous module assumption:
  `lr=0x64664D` belongs to the primary `game.ext` instance
  (`file=0x646120`, `len=0x25630`, `rw=0x66B754`, `H=0x65BFB1`), not
  `graphics.ext`. `graphics.ext` is a later child at `file=0x670C98`. The next
  disassembly target is therefore `game.ext + 0x52D` and the `exRam` strings at
  `game.ext + 0x1CB98`.
- Disassembly of `game.ext` at runtime base `0x646120`:
  - `0x646634` is the exRam allocator/scanner entry. It immediately prints
    `exRam:%dB required.` using its `r0` argument.
  - The only direct callers found are `0x65721C` and `0x657366`; both load the
    required size from `[r9 + 0xD4 + 0x14]`.
  - `0x657338..0x65734A` computes that field as
    `0xED800 - ([r9+0xD8] + [r9+0xDC] + [r9+0xE0] + [r9+0xE4])`.
  - The failing value `0xFFCED800` implies the four subtracted fields sum to
    `0x400000`, so the next check is who writes those fields and whether they
    are host memory totals leaking into the app's exRam accounting.

- Resumed investigation: host table[27] currently returns the ARM-visible main heap length (`origin_mem_len`, observed 0x400000). The disassembled game allocator subtracts four global fields from a fixed 0xED800 exRam budget, so the next probe will identify which `[r9+0xD8..0xE4]` field receives 0x400000 and from which PC/table bridge.

- Temporary experiment: reducing only the ARM EXT `origin_mem_len` to `0xED800` changes the internal print from `exRam:-3221504B required.` to `exRam:0B required.`, but the menu PPM remains two colors. This proves the 4MB accounting issue is real but not by itself sufficient to restore image blits; investigation continues into the allocator return path and subsequent renderer calls.

- Restored the temporary `origin_mem_len=0xED800` experiment to the original 4MB model before further tracing. Current confirmed state: resource payloads are present and contain 16-bit pixel data; host `_DrawBitmap` table[120] is not used for the missing art; `graphics.ext` contains direct pixel/shape routines and calls through table slots around `+0x1C/+0x20/+0x28`, so the next target is whether those bridge slots point to `_DrawPoint`/`_DrawBitmap`/`DrawRect` correctly and whether pixel writes target the ARM screen buffer or another buffer.

- 14:10 resumed from prior handoff. Verified current source still has the 4MB origin memory model and several temporary diagnostics/bridge experiments. Next step is a narrow runtime probe for graphics.ext pixel writes and RW bridge slots, because the framebuffer present path is already proven correct.
- 14:18 graphics.ext forced-Thumb disassembly: init at 0x670FBA uses r5 = r9 - 0x6C, then stores record[26] and record[3..19] into that pre-R9 bridge block. The renderer later loads function pointers from the same structure via offsets around (global+0x1C0), e.g. r9+0x188 for point drawing. The earlier compact rw+0x20 repair is therefore not the layout this graphics module executes.
- 14:37 runtime GFX_BRIDGE_DIAG at 0x6722E2 showed the previous probe was reading r9-0x6C strings, not the active call table. Re-reading the Thumb literal sequence shows graphics renderers load file_base[0] through a PC-relative global and then call record[119]/record[122]-style slots. Direct pixel write probe did not hit, so the current blocker is why the image path falls back to/only reaches shape/text table calls instead of the bitmap stream write loop.
- 15:04 resumed from the handoff. Current source still contains temporary graphics/exRam diagnostics and a speculative private-child bridge repair. The next diagnostic is intentionally narrower: instrument only table[3] memcpy calls from the graphics.ext runtime range, or copies whose destination overlaps the ARM framebuffer. This should prove whether the graphics blit helper copies pixels into screen memory, an offscreen surface, or not at all.
- 15:16 `VMRP_ARM_EXT_GFX_COPY_DIAG=1` repro artifact: `/tmp/vmrp-e2e-DfH5Hx`. The test still fails (`menu.ppm` has 2 colors) and no `GFX_COPY_DIAG` lines appear. Therefore the missing menu art does not reach the graphics.ext row-copy helper and no table[3] copy targets the framebuffer before the capture. The next target is child module initialization/R9/bridge state rather than SDL present or memcpy blit implementation.
- 15:31 corrected a disassembly mistake. `graphics.ext` is mixed ARM/Thumb: the entry at `file+8` is ARM and `BLX`s into Thumb code at `0x671F7C`. Its helper sets `r9 = P->start_of_ER_RW`; the init routine computes `r5 = r9 + 0x70 - 0x6C`, so the bridge block starts at `r9+4`, not `r9-0x6C`. The row-copy helper at `0x6712F4` loads `r9+0x28`, which maps to record/table[4] (`memmove`), not table[3] (`memcpy`). The previous `GFX_COPY_DIAG` only covered table[3], so the next run must include table[4].
- 15:45 new bridge diagnostics show `graphics.ext` reaches `0x6722E2` with `P=0x6E95A4`, `P[0]=0x672C5C`, and RW bridge slots mostly zero while later slots contain `table[8]`/`table[9]`. `CHILD_DIAG` confirms the module record can contain `record[3..19]`/`record[26]`, but the copied RW block used by the renderer lacks the early bridge entries. Leading fix: repair blank record bridge slots immediately when private-loader `mr_cacheSync` identifies the runtime header, before the child entry copies record slots into its RW block.
- 16:00 implemented the early record repair experiment and reran with diagnostics; artifact `/tmp/vmrp-e2e-9gtFjG`. It did not change the PPM or the bridge snapshot. Re-reading the call site shows `0x6722E2` is a wrapper to `record[122] DrawRect`, so the zero early bridge slots in that snapshot are not proof that row blit is using a blank function pointer. Current focus returns to the game-side `exRam`/resource-layer decision path, because after `exRam:-3221504B required.` the app keeps drawing background/text but never enters the bitmap stream renderer before the capture.

- 14:08 long-wait manual repro: started gwkdl, waited 5s, clicked music “否”, then waited an additional 15s before capture. Artifact `/tmp/vmrp-gwkdl-long-5zY18w/menu-long.ppm` still has exactly 2 colors: blue background and yellow text. This rules out an early screenshot/race; image blitting never reaches the ARM framebuffer in the prepared menu state.

- 14:19 controlled memory experiment: temporarily exposed 512KB ARM EXT origin memory. `game.ext` changed from `exRam:-3221504B required.` to `exRam:448512B required.` and called `MR_MALLOC_SCRRAM(1014)`, which returned ARM address `0x351468` length `449536`. The menu PPM still has only 2 colors, so the bug is not just absence of SCRRAM; the post-allocation image/decode/blit path still fails or is skipped.
- 14:26 resumed after handoff. The latest 512KB experiment plus `EXRAM_COPY_DIAG` proved decoded resource-sized buffers are copied into the SCRRAM range (examples: `npc1.bmp`, `logo0.bmp`, `logo1.bmp`, `mk.bmp`, `if.bmp`, `zm.bmp`-sized payloads), but there is still no copy/blit into the ARM screen buffer and a long-wait capture remains exactly two colors. This removes "resource load delay" and "SCRRAM allocation absent" as sole explanations. The current root search is the game-side draw dispatcher / memory-accounting state that decides whether those buffers are scheduled for rendering.

- 14:48 resumed after reading the full history. Current working tree is still in an experimental diagnostic state: `src/arm_ext_executor.c` contains several `VMRP_ARM_EXT_*_DIAG` hooks, a speculative private-child bridge repair, and `origin_mem_len` is still temporarily set to 512KB. The confirmed keepable changes so far are the dynamic MRP cache and the E2E `uniqueColorCount()` assertion helper; the temporary memory-size experiment must not be treated as the final fix because it does not restore image pixels.

- 15:01 continued from the handoff and rechecked current source: `origin_mem_len`
  is back to 4MB. The source still contains temporary diagnostics plus a
  speculative private-child bridge repair, so later cleanup must separate those
  from any real fix.
- Disassembly of `game.ext` confirms the prepared/menu render function is
  `0x65739C`. It first draws text/background via `0x65B61C`, then calls
  `0x65B650` with `r0 = [r9 + 0xEC + 0x30 + selectedIndex*4 + 0x10]` for the
  main image/list item. `0x65B650` parses a two-byte glyph/image stream, calls a
  resource lookup through the graphics table slot at offset `+0x78`, and has two
  possible pixel paths: direct writes around `0x65B88C..0x65B896`, or delegation
  to `0x65B38C` around `0x65B8D8..0x65B8F8`. The next runtime probe will log
  only these blocks to see whether the object pointer is missing, lookup fails,
  clipping rejects the draw, or writes land outside the ARM screen buffer.
- 15:17 rebuilt current source and reran with `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_GAME_RENDER_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`. Artifact: `/tmp/vmrp-e2e-7V61s9`. The assertion still fails with `menu.ppm` having 2 colors, so the next read is the narrow render diagnostic stdout for branch/object state in `game.ext` `0x65B650`.

- 2026-06-23 resumed from the current experimental tree. The source still has
  broad temporary diagnostics and a speculative private-child bridge repair, so
  the next steps are to re-read the latest stdout and disassembly, then isolate a
  real generic fix before cleaning those experiments out.

## 2026-06-23 continued investigation

- **Bug 1 FIXED**: Reduced `origin_mem_len` from 4MB to 512KB. The game's exRam
  budget = `0xED800 − origin_mem_len` was negative with 4MB. Now positive:
  `exRam:448512B required.` SCRRAM allocated at `0x350FBC` length 449536.

- **Bug 1 supplement**: Added `sync_origin_mem_slots()` to write updated
  `origin_mem_left/min/top` back to ARM-visible table[111/135/136] slots after
  host-side alloc/free. Previously the ARM slots were stale since host table[0]
  uses `arm_alloc` (bump allocator outside origin_mem) and the ARM free-list
  allocator never runs.

- **Bug 2 IDENTIFIED**: Even with positive exRam and SCRRAM allocated, images
  still don't render. Root cause: the game's init function at file offset
  `0x11160` is called **exactly once** by the wrapper's timer dispatch. The
  first call does SCRRAM allocation and returns. The **second** call (which would
  trigger the image loading path at `0x1127C` via `0x11258`) never happens.

- The wrapper (cfunction.ext) receives 64+ code=2 timer events. Each event
  runs the wrapper's ARM dispatch code which draws background (DrawRect) and
  text (DrawPoint). But the wrapper only forwards to the game's timer callback
  (`0x15CB0`) once. Subsequent ticks run the wrapper-level rendering without
  re-entering the game module's init path.

- The game's gate function at `0x12DE0 → 0x15CE8` calls table[61] (returns 0)
  and should pass (return 1). The gate is not the blocker.

- Next investigation target: why the wrapper's ARM dispatch at `0xE8342D` stops
  forwarding timer events to the game module after the first call. Possible
  causes: the wrapper's internal game-timer linked list head (`game_rw+0x8C`)
  gets zeroed or corrupted; or the SCRRAM allocation path modifies wrapper state
  that disables further game callbacks.

- Confirmed: `game_rw+0x8C` (GAME_TIMER_HEAD_OFFSET) is always 0 for gwkdl,
  while gghjt has a valid pointer there. This offset is game-specific; gwkdl's
  game.ext doesn't register timer callbacks via that field.

- cfunction.ext is IDENTICAL between gwkdl and gghjt (same MD5 hash). The
  wrapper code is the same; the difference is in game.ext behavior.

- The wrapper's code=2 → code=5 forwarding goes through an internal event queue
  at `wrapper_rw+0x08`. For gwkdl this queue is empty because the game doesn't
  register timer callbacks through the wrapper's queue API. The first timer
  delivery to the game happens through a separate init mechanism
  (`0x15CB0 → 0x12DE0 → 0x11160`), not through the queue.

- Direct host-side code=5 dispatch to game helper crashed at `0x2CCB82`
  (unmapped memory read at `0x0F28233C`) — the game's resource code expects
  valid ARM heap state that the direct dispatch doesn't provide. The code=5
  must come through the wrapper's proper dispatch context.

- **Current root cause for Bug 2**: The game's init function `0x11160` is called
  once via wrapper init dispatch. It allocates SCRRAM and returns. The second
  call (which loads images via `0x1127C`) should come from the code=5 timer
  tick handler at `0x1150C`, which is reached via `0x16848 → 0x151BC → 0x1150C`.
  But the wrapper never dispatches code=5 to the game because the wrapper's
  internal timer event queue never generates the `event_type=0x20` event that
  triggers the timer chain walker.

- Timer chain at `wrapper_rw+0x274` IS populated with `0x348E40` (a valid
  game timer node). The chain walker at cfunction.ext `0x261C` is supposed to
  iterate this chain and call `0x3590` for each node. `0x3590` decrements
  `extChunk+0x34` (suspend depth) and calls game helper with code=5.

- Direct invocation of the walker from host-side (`0x3B4F`) didn't produce
  results — the walker's array lookup at `[wrapper_rw+0x04+0x14]*8` likely
  reads a stale index and finds an empty slot.

- Next investigation: trace the wrapper's ARM code during code=2 to see which
  branch of `0x1F50` is taken and why the `event_type=0x20` path at `0x1FEA`
  doesn't fire. The event type might be set differently in our emulator vs
  a real phone timer interrupt.

- Discovered wrapper `0x1F50` dispatch logic: `[wrw+4]` is a "busy" flag.
  When 0 (dispatch mode), `0x1F50` interprets R0 as event type. When nonzero
  (queue mode), it queues the event to `wrw+0x28` pending list.
  For code=2, R0=P pointer (not event type), so dispatch mode misinterprets it.

- Setting `[wrw+4]=1` before code=2 successfully queues events, but the
  pending→ready transfer and queue drain mechanism (event_type=32) doesn't
  fire automatically. The transfer happens inside the wrapper's native timer
  interrupt handler, which our emulator bypasses.

- Direct code=5 dispatch to game helper crashes because ARM register/stack
  state is inconsistent (192 invalid memory crashes). The game helper expects
  specific ARM context set up by the wrapper's dispatch chain.

- **Summary of confirmed fixes (Bug 1)**:
  1. `origin_mem_len` 4MB → 512KB (matching real phone hardware)
  2. `sync_origin_mem_slots()` keeps ARM table[111/135/136] in sync
  3. `extChunk[8]` repair extended from code=1 to all event types

## 2026-06-24 continuation

- Resumed from the existing diagnostics instead of regenerating a broad trace.
  The current source already contains the 512KB origin memory change, origin
  memory slot synchronization, dynamic MRP cache, multiple narrow diagnostics,
  and an experimental chain-walker path. The target test still needs a clean
  generic fix for image rendering.
- Latest focused evidence still points away from resources and presentation:
  `zm.bmp`, `logo0.bmp`, and `logo1.bmp` decode correctly offline; table[29]
  presents the real ARM screen buffer; SCRRAM allocation succeeds with the
  512KB origin memory model. The remaining failure is that the game-side second
  init/timer phase that loads/blits images is not being delivered after the
  first allocation pass.
- Current critical path: re-check the 19KB `cfunction.ext` timer chain. Runtime
  state shows a valid node at `wrapper_rw+0x274` (`0x348E40`) with callback
  `0xE835E1` and game extChunk `0x348E04`, but `arm_ext_primary_helper()`
  returns 0 because `wrapper_timer_dispatch_addr` is intentionally 0 for this
  wrapper. The half-implemented `chain_walker_owner` path is therefore not
  reached by the normal `mr_timer()` route.
- New baseline run `/tmp/vmrp-e2e-KLQQps` still fails with `menu.ppm` having
  only 2 colors, but it also shows a repeated primary `game.ext` crash after
  successful SCRRAM allocation:
  `UC_MEM_READ_UNMAPPED addr=0x0F4C23EC`, `PC=0x2CCB82`, `R9=0x2EB754`,
  `R0=0xC3C5C3BF`, immediately after opening `mythroad/cache/exr.cac0`.
  Forced-Thumb disassembly identifies `0x2CCB58` as a ring-buffer pop from the
  structure at `r9+0x10EC`; the crash means that structure base (`[r9+0x10EC+4]`)
  has been polluted with stale/cache bytes. This is now more concrete than the
  earlier "timer not firing" hypothesis.
- `VMRP_ARM_EXT_TRACE=1` did not show `captured timerStart/timerStop` messages
  for this run, although table[31]/[32] are called repeatedly from wrapper
  `0xE83E4B/0xE83E55`. Goodall's sub-agent result remains useful for cleanup:
  the existing `chain_walker_thunk` dispatch and direct primary `code=5`
  supplementary code are inconsistent with the 19KB wrapper protocol, but the
  immediate gwkdl image failure is now tied to stale `exr.cac0` reuse after
  `exdet.dat` is removed/recomputed.

- **Bug 2 status**: Timer dispatch to game for code=5 tick remains unsolved.
  The wrapper's internal timer chain has a game node but doesn't walk it
  during code=2. The busy flag queueing approach queues events but the
  drain mechanism requires native timer interrupt processing not replicated
  by the emulator.

## 2026-06-23 continued: root cause chain

- **版本号修复 (MT6229)**: `mr_getUserInfo` 返回的 ver 需满足
  `(ver + 0xF9F9A440) > 0x270F`。MT6235(PLAT=8) 不满足，MT6229(PLAT=9) 满足。
  改 `dsm.c` 中 `#define MT6235` 为 `#define MT6229`。

- **Low table 扩展**: 游戏内存检测读 `0x2000` 地址，需扩展 `EXT_LOW_TABLE_SIZE`
  到 `0x10000` (64KB)。

- **exdet.dat 缓存**: 游戏缓存 exRam 检测结果到 `cache/exdet.dat`，
  该文件存在时跳过完整初始化。测试前需删除。

- **版本号 + 缓存删除 + low table 扩展后**: `0x1127C` image init 和 `0x15530`
  item renderer 成功执行（400+ hits）。但 menu.ppm 仍 2 色。

## 2026-06-24 resumed

- Current baseline: the working tree already contains several experimental
  diagnostics in `src/arm_ext_executor.c`, plus candidate generic changes:
  dynamic MRP cache, enlarged low table, MT6229 user-info platform, origin
  memory accounting sync, and timer-wrapper experiments. Treat diagnostics and
  speculative timer changes as suspect until verified by PPM.
- Next action: rebuild/run the single target
  `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` without xvfb, keep the
  temp artifacts, then inspect stdout/stderr and PPM colors to confirm the
  current failure mode before changing behavior.
- Rebuilt `build/vmrp` successfully and reproduced with
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`.
  Artifact: `/tmp/vmrp-e2e-ZIVAdF`.
  - `bgm-select.ppm`: 2 colors, black plus yellow text.
  - `menu.ppm`: 1 color only, `#1060a8` across all 240x320 pixels.
  - stdout shows `MEM-I:total:524288`, `exRam:448512B required.`, and
    `exRam:Alloc:addr:0x351094,len:449536`, so SCRRAM allocation is no longer
    the immediate blocker.
  - The post-click frame is now more reduced than earlier two-color failures,
    which makes the current timer/dispatch experiments suspect. Next step:
    inspect the wrapper dispatch implementation and compare to HEAD/current
    diff before keeping or replacing those changes.

## 2026-06-24

- 16:56 接手已有排查后先复跑当前工作树：
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
  最新 artifact: `/tmp/vmrp-e2e-Dsy4np`。
  - `mem-check.ppm`: 2 色，黑底 + 白字。
  - `bgm-select.ppm`: 2 色，黑底 + 黄字。
  - `menu.ppm`: 2 色，蓝底 `[16,96,168]` 76499 像素 +
    黄字 `[248,240,0]` 301 像素，仍失败在 `uniqueColorCount() > 16`。
  - stdout 只有 29KB，没有 crash；可见 `exRam:448512B required.` 和
    `exRam:Alloc:addr:0x35105c,len:449536`，所以当前失败基线不是
    负 exRam，也不是旧 stale `exr.cac0` 崩溃。
  - SCRRAM 分配后重复 `mr_open(mythroad/cache/exr.cac0,1) fd is: 0`。
    这与“资源和显存 present 正常，但缓存/graphics callback 安装阶段没有完成”
    的现有结论一致。下一步只加窄诊断：定位 `exr.cac0` 生成/读取分支的 LR，
    并继续追 `[game_rw+0x84]` draw callback 为什么未安装。

- 10:11 resumed after the test update. Latest kept repro from the prior run:
  `/tmp/vmrp-e2e-L8WOE5`; `menu.ppm` still has only two colors, so the updated
  assertion is still catching the same missing-image bug.
- Current source has the confirmed memory/platform fixes in place
  (`origin_mem_len=512KB`, ARM origin_mem stat slot sync, MT6229, low table
  64KB, dynamic MRP cache). The remaining suspect is generic wrapper/MPS/timer
  protocol rather than resource extraction or SDL present.
- The active experimental behavior to verify next is the `code=2` wrapper busy
  flag patch in `arm_ext_call`: it writes `wrapper_rw+4 = 1` before running
  the wrapper helper, then clears it afterwards. cfunction.ext disassembly
  indicates that flag changes whether `0x1F50` queues events or dispatches
  them, so the next diagnostic will log only the cfunction MPS/timer blocks.
- 10:15 reran with `VMRP_ARM_EXT_MPS_DIAG=1 VMRP_ARM_EXT_GAME_RENDER_DIAG=1
  VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1`; artifact `/tmp/vmrp-e2e-GhJ84P`.
  `menu.ppm` still has two colors.
- This run proves the current failure is no longer "image initialization never
  starts": `game.ext+0x1127C` image init executes and the renderer
  `game.ext+0x15530` executes hundreds of times. Resource/object pointers in
  the renderer point into SCRRAM around `0x350F2C`, but no multi-color pixels
  reach the final framebuffer.
- The first MPS diagnostic revision matched module-relative offsets in all
  nested modules, so it caught same-offset code in `game.ext` and the third
  child instead of the main wrapper's `EXT_CODE_ADDR` copy. That diagnostic must
  be corrected before drawing conclusions about `cfunction.ext+0x3E6C`.

## 2026-06-24 continued after test update

- User updated the gwkdl E2E test and asked for a rerun. Current source already
  contains the keepable hardware/model fixes from the prior pass
  (`origin_mem_len = 512KB`, ARM-visible origin memory statistic sync,
  64KB low table, dynamic MRP cache, MT6229 DSM selection) plus several gated
  diagnostics. Next step is to rerun only
  `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` and use the resulting
  PPM/stdout as the new baseline.
- Baseline rerun artifact: `/tmp/vmrp-e2e-Ld0Xfy`.
  `menu.ppm` still has only 2 colors (`[16,96,168]` background and
  `[248,240,0]` text). stdout shows `exRam:448512B required.` followed by
  `exRam:Alloc:addr:0x35105c,len:449536`, so the current failure is after
  successful SCRRAM allocation, not the old negative exRam path.
- Narrow render/child diagnostic artifact: `/tmp/vmrp-e2e-khzmbQ`.
  Runtime modules: primary `game.ext` at `0x2C6120`, `graphics.ext` at
  `0x2F0C98`, and a third child at `0x2F2E28`. `game.ext+0x15530` looks up the
  image stream and returns `0x350F2C`; the loop then repeatedly calls the
  text/char drawing slot (`0x10244`, host table[145]) and presents through
  `game.ext+0x167F4`. Every present still sees `draw_cb=[game_rw+0x84]=0`.
  Therefore the remaining defect is the missing pre-present graphics callback
  installation, not SCRRAM allocation or the plain glyph/text path.

## 2026-06-24

- 用户修正测试后重新运行：
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
  当前 artifact: `/tmp/vmrp-e2e-AXoWOU`。
- 结果仍失败在最终断言：`expected 2 to be greater than 16`。
- PPM 验证：
  - `bgm-select.ppm`: 240x320，2 色：黑色 76365 像素，黄色 `[248,240,0]` 435 像素。
  - `menu.ppm`: 240x320，2 色：蓝色 `[16,96,168]` 76499 像素，黄色 `[248,240,0]` 301 像素。
  - `screen.ppm`: 240x320，2 色：黑色 75668 像素，近白 `[248,252,248]` 1132 像素。
- 这次测试已删除 `mythroad/cache/exdet.dat` 并等待内存检测后继续，因此当前失败不是旧缓存或等待不足。
- 上一轮 `GAME_RENDER_DIAG` 已确认 `game.ext` 的图像初始化和 item renderer 会进入，
  lookup 返回非零资源对象，但最终没有进入直接像素写入路径。下一步新增更窄的
  renderer clip/loop 诊断，记录 `0x15530` 绘制函数栈槽和分支处寄存器，确认是
  clip 范围、宽高、模式位还是目标地址导致所有图片块被跳过。
- `VMRP_ARM_EXT_GAME_RENDER_DIAG=1` artifact `/tmp/vmrp-e2e-evqNtE`：
  `0x1127C` 图像初始化执行，`0x15530` renderer 执行。renderer 进入
  `0x270D` 模式分支，并通过 `table[145] mr_platDrawChar` 绘制字形；这是
  “请按任意键”等文字路径，不是 `zm.bmp` 位图路径。
- `VMRP_ARM_EXT_TABLE_DIAG=1` artifact `/tmp/vmrp-e2e-R5zOvu`：
  调用统计为 `table[3] memcpy=53839`、`table[119] DrawPoint=15252`、
  `table[122] DrawRect=109`、`table[123] DrawText=48`、`table[145]
  mr_platDrawChar=80`，没有 `table[120/121/147/148]`。结论：目标画面前台
  仍只提交文字/矩形，图像专用 host bridge 没有被调用；下一步追资源对象登记
  和 graphics 子模块内部绘制入口，确认 `zm.bmp` 解码后的指针是否被挂入对象表。

## 2026-06-24

- Rebuilt current workspace with `cmake --build build --target vmrp`; build
  succeeds.
- Current target run:
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`
  fails earlier than the final menu assertion:
  `boot.pixel(227, 301)` is `[0,0,0]`, expected `[248,252,248]`.
- Latest retained artifact checked: `/tmp/vmrp-e2e-Y9BfMh`.
  `bgm-select.ppm` is 240x320 with only two colors:
  black and `[248,240,0]`; pixel `(227,301)` is black.
  `screen.ppm` from the automatic/default dump is also two colors but uses
  white `[248,252,248]` text, proving a later frame exists after the explicit
  `SCREEN bgm-select` capture.
- Stdout reaches `exRamDetect:start:0x0, end:0x800000` and writes
  `mythroad/cache/exdet.dat`, so the current failure is not an early loader
  crash. The mismatch between `bgm-select.ppm` and `screen.ppm` suggests the
  timer/screen-present sequence is still not matching the real wrapper
  dispatch order.
- After the user corrected the test expectation, reran the target with the
  same command. New artifact: `/tmp/vmrp-e2e-ptARWV`. The test now reaches the
  final `"menu"` screenshot and fails the image assertion:
  `wake.uniqueColorCount()` is `2`, expected `>16`.
- `/tmp/vmrp-e2e-ptARWV/menu.ppm` colors are only blue background
  `[16,96,168]` and yellow text `[248,240,0]`.
  Stdout shows the exRam path is now positive and allocated:
  `exRam:448512B required.` and `exRam:Alloc:addr:0x35105c,len:449536`.
  Therefore the remaining bug is after SCRRAM allocation: decoded resource
  data is not being scheduled/blitted into the ARM framebuffer.

- 继续当前手工状态：`game.ext+0x167F4` present 每帧读取
  `[game_rw+0x84]`，最新诊断仍为 0；`graphics.ext` 已被私有 loader
  sync 并进入 timer chain，因此当前问题收敛为 graphics callback 安装协议
  没有完成，或写到了错误的 RW。为避免全 trace，新增了
  `VMRP_ARM_EXT_GAME_RW_WRITE_DIAG`，只监控 primary game RW 的
  `0x84`、`0xD4..0xE8`、`0x2C0..0x2DF` 写入，用于证明 callback 是否由
  ARM 代码安装/清零。
  53839 次 memcpy 零次写入 screen buffer → graphics.ext 的 blit 函数
  从未被调用。bridge slots 问题。

- **最终根因**: `[game_rw + 0x84]` 处的 draw callback 为空。`0x167F4`（present函数）
  在调用 `table[29]` mr_drawBitmap 之前会先 `blx [game_rw+0x84]`，若该值为 0
  则跳过。这个 callback 应该是 graphics.ext 的 blit 函数（文件偏移 `0x19CB`），
  由 game.ext 在加载 graphics.ext 后的初始化步骤中设置。

- **game 的多步初始化流程**:
  1. 第一次 timer dispatch: 分配 SCRRAM ✅
  2. 第二次+: 加载 graphics.ext, 设置 draw callback (`game_rw+0x84`), 加载图像资源
  3. 后续 timer tick: 调用 `0x1127C` 渲染图像, 通过 callback blit 到 framebuffer
  
  步骤 2+ 需要 wrapper 的 timer dispatch chain 持续转发 timer 给 game。
  gghjt/gwkdl 的 cfunction.ext（19428 字节，MD5=6827988d...）的 timer chain
  dispatch 机制在模拟器中不工作（`find_wrapper_timer_dispatch` 匹配不到该
  版本的 pattern），导致步骤 2 和 3 从未执行。

- **已完成的修复**:
  1. `origin_mem_len` 4MB → 512KB（避免 exRam 预算溢出）
  2. `sync_origin_mem_slots()` 同步 ARM 侧 table[111/135/136]
  3. `EXT_LOW_TABLE_SIZE` 0x2000 → 0x10000（覆盖内存检测范围）
  4. `MT6235` → `MT6229`（提升 ver 使版本检查通过阈值 101090000）
  5. `read_game_timer_head` / `write_game_timer_head` 支持 0x88 备选偏移
  6. `extChunk[8]` helper 指针修复扩展到所有事件类型
  7. 测试用例增加内存检测步骤（LEFT_SOFT 按键）

- **未完成**:
  - wrapper timer dispatch chain 修复（需要为 19428 字节版本的 cfunction.ext
    添加 `find_wrapper_timer_dispatch` pattern，或实现通用 timer chain walker）
  - 或：在 arm_ext_sync_internal_nested_module 中检测到 graphics.ext 加载后，
    自动从 graphics.ext record[31]（文件偏移 `0x7C/4` = callback 函数）
    读取 blit callback 地址并写入 `game_rw+0x84`

## 2026-06-23 resumed fix pass

- 继续本轮修复时，工作树中的 `src/arm_ext_executor.c` 已含上一轮实验改动：
  pat2 literal PC 修正、`VMRP_ARM_EXT_TIMER_DIAG` 诊断、`arm_ext_call(code=2)`
  后直接补调 game helper 的实验，以及 `arm_ext_call_dispatch()` 中将
  `extChunk+0x34` 临时设为 1 的实验。后续需要先用目标用例验证这些改动的
  实际效果，再把保留项收敛为通用 wrapper timer dispatch 修复，移除不成立
  的直接 game helper 补发路径。
- 当前实验树复跑目标用例保留 artifact `/tmp/vmrp-e2e-uiyl6y`。日志显示
  `find_wrapper_timer_dispatch` 已让 `arm_ext_primary_helper()` 返回非零，
  但返回地址是 `0xE83591`。反汇编确认 `0xE83590` 只是单个 timer 节点的
  resume 回调：它检查 `EXT_CHUNK_MAGIC`、递减 `extChunk+0x34`、再调用
  game helper(code=5)。直接把它当 host timer dispatcher 调用缺少 wrapper
  chain walker 上下文，并在 exRam 分配后触发 game.ext `0x2CCBC0` 非法写。
- 解压并反汇编实际加载的 `cfunction.ext`（MRP 内 gzip 条目 13883 字节，
  解压后 19428 字节，MD5 `6827988de19a7c6abe9a39de58da9711`）后确认通用
  timer chain 入口是 `0xE83B50` 薄包装：
  `push {r3,lr}; bl 0xE8261C; movs r0,#0; pop {r3,pc}`。
  `0xE8261C` 按 `wrapper_rw+0x190` 链表遍历节点并调用 `0xE83590`。
  本轮代码已改为匹配并返回该 walker thunk，同时移除直接补发 game helper
  的实验路径。
- 接手继续定位时，当前可疑点收敛为：宿主已调用 19KB `cfunction.ext` 的
  `0xE83B50` chain thunk，但现有 `VMRP_ARM_EXT_TIMER_DIAG` 仍在观察
  `wrapper_rw+0x274`，而反汇编 walker 读取的是 `wrapper_rw+0x190 +
  index*8`。下一轮只加窄诊断：记录 `wrapper_rw+0x04` 当前 index、
  `wrapper_rw+0x190` 桶头、`extChunk+0x08/+0x34`、`game_rw+0x84`，
  并统计 `0xE8261C/0xE83590/0xE83CE0` 是否实际命中。
- 本轮继续修正两个诊断盲点：
  - `hook_game_render_diag` 的 present 区间现在使用独立计数，避免
    `0x15530` 大量命中先耗尽 400 条上限，导致看不到 `[game_rw+0x84]`
    的真实值。
  - `arm_ext_call_dispatch()` 的 timer chain 诊断和宿主 timer 保活改读
    反汇编确认的 `wrapper_rw+0x190 + ([wrapper_rw+0x18] * 8)` 桶头；
    旧 `wrapper_rw+0x274` 只保留为对照字段，不再作为保活依据。
  - 删除直接补调 `game.ext + 0x11160` 的死代码实验，避免继续把错误
    wrapper 上下文之外的调用路径混入判断。
- 复跑 artifact `/tmp/vmrp-e2e-5Z2RHM`：`game.ext` 已进入
  `0x1127C` 和 `0x15530`，但 `GAME_PRESENT_DIAG` 连续 120 次显示
  `draw_cb=0x0`，PPM 仍为 2 色。这证明当前问题已不是“图像初始化完全
  未执行”，而是 primary `game_rw+0x84` 的 present 前回调仍未安装。
- `VMRP_ARM_EXT_CHILD_DIAG=1` 复跑 artifact `/tmp/vmrp-e2e-E1qBJa`：
  private loader 已同步三段模块：
  - primary `game.ext`: `chunk=0x348E04 P=0x348E60 H=0x2DBFB1 rw=0x2EB754`
  - `graphics.ext`: `chunk=0x34E09C P=0x34E0F8 H=0x2F1141 rw=0x2F2C5C`
  - 后续 child: `chunk=0x350EBC P=0x350F18 H=0x2F4691 rw=0x2F8634`
  wrapper timer bucket `wrapper_rw+0x190` 的节点 `[node+0x0C]` 指向
  `graphics.ext` chunk (`0x34E09C`)，节点内还带有 `file=0x2F0C98 len=0x1FC0`。
  当前继续定位重点是：`graphics.ext` / 后续 child 如何按协议把 blit/present
  callback 安装到 primary `game_rw+0x84`，以及模拟器哪里破坏了该协议。

## 2026-06-24

- 09:04 接手当前源码状态。已有修复/实验包括 512KB `origin_mem_len`、
  `sync_origin_mem_slots()`、64KB low table、MT6229 平台版本、19KB
  `cfunction.ext` timer walker 定位、以及若干窄诊断 hook。当前确认的问题不再
  是资源读取、SCRRAM 分配或游戏初始化完全未执行，而是 `game.ext` present 前
  读取的 `[game_rw+0x84]` draw callback 仍为 0。
- 下一轮复现只启用窄诊断：
  `VMRP_ARM_EXT_CHILD_DIAG=1`、`VMRP_ARM_EXT_TIMER_DIAG=1`、
  `VMRP_ARM_EXT_GAME_RENDER_DIAG=1`、`VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1`，
  避免全量 trace。目标是确认 `graphics.ext`/第三个 child 的初始化是否写入
  primary `game_rw+0x84`，或是否写到了错误 RW/偏移。
- 09:11 根据最新失败 artifact `/tmp/vmrp-e2e-BwR3GO` 重新对照反汇编：
  `TIMER_CHAIN_DIAG dispatch-pre` 的节点 `[node+0x0C] = 0x34E09C`，
  即 `graphics.ext` 的 extChunk；但 `arm_ext_call_dispatch()` 传入
  `R0 = primary game.ext extChunk (0x348E04)`。反汇编确认 `0xE83B50`
  只是 `0xE8261C` 的 thin thunk，后者遍历 `wrapper_rw+0x190` 的
  suspend/resume bucket 并调用 `0xE83590`，`0xE83590` 会递减传入
  extChunk 的 `[0x34]`。这解释了日志里 primary depth 从 0 下溢并每帧
  递减。真正 timer 队列注册/消费在 `0xE83CE0` / `0xE83E6C` 附近，
  不应把 `0xE83B50` 当作 host timer dispatcher。下一步代码改动是移除
  19KB chain-thunk 匹配，让该 wrapper 回到普通 helper `code=2` 路由，
  再用窄诊断验证是否进入 timer consumer 并安装 `game_rw+0x84`。

- 09:32 用户修正测试后按目标命令重跑：
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
  artifact `/tmp/vmrp-e2e-BfLGST`，仍失败于最终 `menu` 截图：
  `uniqueColorCount() == 2`。
  PPM 验证：`bgm-select.ppm` 只有黑/黄两色，`menu.ppm` 只有蓝背景
  `[16,96,168]` 和黄字 `[248,240,0]`，`screen.ppm` 只有黑/近白两色。
  当前源码已明确让 19KB `cfunction.ext` 的 `0x3B50` chain thunk 返回 0，
  不再把 suspend/resume walker 当 host timer dispatcher。下一步用窄诊断
  验证普通 helper `code=2` 路由下 wrapper 是否消费真正 timer 队列，以及
  `graphics.ext` 是否按协议安装 primary `game_rw+0x84` draw callback。

- 09:34 窄诊断复跑 artifact `/tmp/vmrp-e2e-DMETzY`：
  `CHILD_DIAG` 同步了 primary `game.ext`、`graphics.ext`、第三个 child；
  `wrapperTimer=0`，说明 19KB chain thunk 已被正确排除，不再走错误的
  `arm_ext_call_dispatch()` 路径。普通 `code=2` 进入 wrapper helper
  `0xE800B0`，其 ARM/Thumb 反汇编路径为 `0xE800B0 -> 0xE82DD8`，
  code=2 分支调用 `0xE83E6C`，后者消费 `wrapper_rw+0x274` timer chain。
  链节点为 `0x348E40`，`[node+0x0C] = 0xE835E1`，
  `[node+0x10] = primary extChunk 0x348E04`；`0xE835E1` 反汇编确认会调用
  primary game helper 的 `code=2`。因此当前失败不是 host timer 完全未
  进入 game，而是 game/graphics 初始化后没有把 present 前回调安装到
  `game_rw+0x84`。
- 同一 artifact 的 `GAME_RENDER_DIAG`：`game.ext+0x1127C` 图像初始化执行
  1 次，`game.ext+0x15530` renderer 执行并进行资源 lookup；lookup 返回
  `0x350F2C`，但之后走到文字/字形分支，`GAME_PRESENT_DIAG` 120 次都显示
  `draw_cb=0x0`。`GAME_RW_WRITE_DIAG` 在 `game_rw+0x80..0x90` 只观察到
  `+0x88/+0x8C` 写入，没有任何 `+0x84` 写入。下一步改为模块相对的
  `graphics.ext` 诊断，追它的 init/scheduler 是否向 primary RW 暴露
  blit callback，避免继续使用旧的绝对地址 `0x67...` hook。

- 09:54 继续当前任务。用户已修正测试并要求重跑；现有目标用例仍失败，
  最终 `menu.ppm` 只有蓝底/黄字两色。继续遵守“不写场景特判、不做图像兜底”
  的约束，下一步只加/使用窄诊断确认 `graphics.ext` 初始化阶段的回调注册：
  关注 `record+0x7C/0x80`、`P/rw` 桥接槽、primary `game_rw+0x84`，以及
  private-loader `mr_cacheSync` 同步时机是否早于 child entry 完成。

- 10:00 窄诊断 artifact `/tmp/vmrp-e2e-i9zLTM` 仍失败，PPM 三张图分别只有
  2 色。`GFX_REL_DIAG` 命中 260 条但按 `file` 分组后可见：
  `graphics.ext` 只进入 `0x1420/0x15E0/0x1660` 等渲染 helper，没有命中
  `0x12E4` 初始化段；third child 命中 `0x1CD8` 调度器；primary game 也有同形
  `0x1300..0x1348` 初始化路径。继续反汇编时需要注意混合 ARM/Thumb veneer，
  不能用 `-M force-thumb` 解释 `0x2C8` 一类 ARM 小入口。当前重点改为确认
  `graphics.ext` init 对 `record[31]/record[32]` 的导出回调是否写到了私有
  record，但没有被同步到 primary/game present 读取的协议槽。
- 10:03 用户再次要求继续并重跑目标用例：
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
  artifact `/tmp/vmrp-e2e-7rF896`，仍失败在
  `wake.uniqueColorCount() > 16`，实际为 2。PPM 结果保持一致：
  `bgm-select.ppm` 黑/黄 2 色，`menu.ppm` 蓝底 `[16,96,168]` + 黄字
  `[248,240,0]` 2 色，`screen.ppm` 黑/近白 2 色。stdout 只有
  `exRamDetect:start:0x0, end:0x800000`、`exRam:448512B required.`、
  `exRam:Alloc:addr:0x35105c,len:449536`，说明本次基线没有开启额外诊断，
  且 SCRRAM 仍成功分配。下一步只加窄的 `record[25]`/wrapper MPS 调用诊断，
  验证 graphics/child 导出回调是否通过通用 EXT 函数指针协议被调用。
- 反汇编复核：
  - `graphics.ext+0x12E4` 初始化先调用 `record[25]`，随后把
    `record[31]=graphics.ext+0x1A27`、`record[32]=graphics.ext+0x1A4B`
    写入私有 record，这是通用 EXT timerStart/timerStop 替换协议。
  - `graphics.ext+0x1BAC/0x1CD8` 与 `cfunction.ext+0x3CE0/0x3E6C`
    是同形 timer node 注册/调度器，均使用 RW+0x84 作为内部 timer 链表头。
  - `game.ext+0x167F4` present 只读取 primary `game_rw+0x84` 作为提交前
    draw callback；目前该槽一直为 0。真正要确认的是 wrapper 的 timer/MPS
    调用是否把 graphics 的内部 scheduler 持续驱动起来，而不是直接写这个槽。

- 10:24 用户修正测试用例后继续。先按当前源码重新构建并只跑
  `test/e2e/gwkdl/game-prepare.test.ts` 基线，确认失败点是否仍为最终 PPM
  只有 2 色；随后继续沿反汇编确认的 wrapper MPS/timer 与
  `graphics.ext` scheduler 路径收窄，不启用全量 trace。
- 10:30 基线复跑完成，artifact `/tmp/vmrp-e2e-W6BcSL`，仍失败：
  `menu.ppm` 240x320 只有 2 色（蓝底 `[16,96,168]` 与黄字
  `[248,240,0]`），`bgm-select.ppm` 和 `screen.ppm` 也各只有 2 色。
  这确认测试修正后失败形态未变；下一步先修正 `VMRP_ARM_EXT_MPS_DIAG`
  的模块归属，确保只把 `EXT_CODE_ADDR` 主 wrapper 的 `cfunction.ext`
  偏移当作 MPS/timer，而不是把同偏移 child module 误判进去。
- 10:37 已修正两个窄诊断：
  - `hook_mps_diag()` 现在只接受 `EXT_CODE_ADDR..EXT_CODE_ADDR+code_len`
    的主 wrapper PC，并以 `pc-EXT_CODE_ADDR` 计算 `cfunction.ext` 偏移；
    不再用 `nested_modules` 归属，避免同 offset 的 `game.ext`/child module
    混进 MPS/timer 判断。
  - `VMRP_ARM_EXT_GAME_RW_WRITE_DIAG` 扩展到 `game_rw+0x18..0x20`，用于追
    反汇编里控制 renderer 走 `0x270D`/`mr_platDrawChar` 路径的
    `[game_rw+0x1C]` 写入来源。

- 10:45 继续前一轮诊断状态。当前基线仍为 `menu.ppm` 仅 2 色；已确认 wrapper `cfunction.ext+0x3E6C` timer consumer 会运行，并且 primary `game.ext+0x15530` renderer 会处理非空 bitmap/object 数据。下一步修正过期的 `VMRP_ARM_EXT_GFX_WRITE_DIAG`（旧绝对 PC 过滤会漏掉 private-loader 映射的 graphics.ext），并新增 primary renderer 写屏/写 exRam 的窄诊断，先证明像素到底有没有被 ARM 代码写入可见 framebuffer。
- 10:49 新增/修正窄写入诊断后复跑：
  `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_GFX_WRITE_DIAG=1 VMRP_ARM_EXT_GAME_RENDER_WRITE_DIAG=1 VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
  artifact `/tmp/vmrp-e2e-gWiQvn`，仍失败，`menu.ppm` 仅蓝底/黄字 2 色。
  诊断结论：`GAME_RENDER_WRITE_DIAG` 300 次命中 primary `game.ext+0x155DE..0x157F0`，写入地址均为 `0x2A7Axx` 栈/临时缓冲，`screenOff=FFFFFFFF` 且 `exramOff=FFFFFFFF`；说明 renderer 正在解码 bitmap/object 数据，但没有走直接 framebuffer `strh` 写屏路径。`GFX_WRITE_DIAG` 修正为模块相对后命中 `graphics.ext` 与 third child 的 scheduler/renderer，但同样没有写入 screen；后段 primary `game.ext+0x1376..0x1428` 命中 exRam 只是扩展内存分配链表/资源结构更新，不是图像像素提交。
- 10:56 反汇编 `game.ext+0x15530` 与 `graphics.ext+0x1660`：二者在 `[rw+0x1C] == 0x270D` 时都会调用 table[145]（`mr_platDrawChar`）绘制逐点/字形；不等于 `0x270D` 才进入后续 `strh` 直接 framebuffer 路径。该分支解释了为何当前只有黄字被画出，但还不能单独解释 `zm.bmp` 位图没有提交。进一步检查 table[147]/[148] 发现运行库函数表中存在 `mr_transbitmapDraw`/`mr_drawRegion`，但 executor switch 尚未实现；不过现有 stdout 没有 `table[147]/[148] not implemented`，需要用 `TABLE_DIAG` 和资源名诊断确认 `zm.bmp` 实际走哪条 host 接口，避免误修。

## 2026-06-24 continued — delta chain & code=5 analysis

### Timer 架构（19KB cfunction.ext）

wrapper 有两条独立的 timer 链表：
1. Timer chain (wrapper_rw+0x190): suspend/resume bucket
2. Delta chain (wrapper_rw+0x1F4): 精确定时

game 也有自己独立的 delta chain (game_rw+0x88)。

### 修复进度

1. Delta chain seed 有效（timer consumer 可以运行）
2. Code=5 补发给 wrapper → chain walker → 0xE83590 → game helper(code=5) 可以驱动 game timer
3. 需要延迟到内存检测完成后才开始补发

### 下一步

使用 `supplementary_init_done` 标记延迟 code=5 补发，只在 game 的 exRam 分配完成后启用。

## 2026-06-24 16:15 CST — current handoff baseline

- 目标仍是只修 `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` 中自动进入
  “请按任意键”后 logo/游戏图像不显示的问题；先不兼容其它测试用例。
- 当前工作树已有未提交修改和本调试文档，按用户/前序调试状态继续，不回退无关变更。
- 已知 PPM 失败形态：`menu.ppm` 只有蓝底 `[16,96,168]` 和黄字
  `[248,240,0]`，说明 host 文字/填充路径可见，`zm.bmp` 等位图未进入最终画面。
- 前序反汇编结论指向 19KB `cfunction.ext` 的 delta timer/code=5 链路：
  graphics/game 的 bitmap renderer 不是完全没初始化，而是依赖 wrapper 的定时链持续驱动；
  计划检查当前 `src/arm_ext_executor.c` 是否已经实现“exRam 分配完成后才补发 code=5”。
- 继续避免全量 trace；只用窄环境变量诊断、反汇编片段和 PPM 色彩统计验证。

## 2026-06-24 16:16 CST — reproduced on current binary

- 已执行 `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`，
  当前源码/二进制仍失败：`wake.uniqueColorCount()` 实际为 2。
- artifact: `/tmp/vmrp-e2e-kg8jQ2`。
- PPM 色彩统计：
  - `mem-check.ppm`: 2 色，黑色 72119，近白 `#f8fcf8` 4681。
  - `bgm-select.ppm`: 2 色，黑色 76365，黄字 `#f8f000` 435。
  - `menu.ppm`: 2 色，蓝底 `#1060a8` 76499，黄字 `#f8f000` 301。
  - `screen.ppm`: 2 色，黑色 75668，近白 `#f8fcf8` 1132。
- stdout 只出现 `exRamDetect:start:0x0, end:0x800000`、
  `exRam:448512B required.`、`exRam:Alloc:addr:0x35105c,len:449536`；
  无 table[147]/[148] not implemented。
- `tmp_assets/51_zm_176x176.ppm` 等离线资源存在且为多色，说明 MRP 资源可解；
  当前问题仍在运行时 callback/timer/present 链路。

## 2026-06-24 16:18 CST — narrow timer/table diagnostics

- 执行：
  `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_TIMER_DIAG=1 VMRP_ARM_EXT_CHILD_DIAG=1 VMRP_ARM_EXT_TABLE_DIAG=1 VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-mEAuAZ`，仍失败，最终 PPM 2 色。
- 后段 `TIMER_DIAG` 显示 `wrw+0x1F4` 已被注入 `0x348E40`，且
  `gameRW=0x2EB754 game84=[00000000 002F0C74 00000000 ...]` 反复保持：
  `game_rw+0x84` 一直为 0，`game_rw+0x88` 有 timer head。
- `TABLE_DIAG` 后段确认 primary `game.ext` 的 code=5 已反复执行，并访问
  `MR_MALLOC_SCRRAM`/exRam 路径：`table[38] code=0x3F6`、`table[44]`
  从 fd 读 `0x6DC00` 到 `0x35105C`。
- 图形输出仍主要是 `table[119]` 点绘制（`lr=0x2DB5E5`，位于
  primary game renderer 分支）以及少量 `table[29]` present，没有
  table[120] bitmap blit 或 table[147]/[148] 未实现提示。
- 新判断：当前未解问题不是“code=5 完全没跑”，而是 graphics/game 预期安装到
  `game_rw+0x84` 的 draw callback 没有被建立；需要反汇编确认该槽由谁写、
  通过哪个 wrapper/graphics 协议触发。

## 2026-06-24 16:26 CST — handoff integration

- 已收到子 Agent 的资源/PPM/扩展内存链路结论：MRP 条目 `game.ext`、`cfunction.ext`、`graphics.ext`、`zm.bmp`、`logo0.bmp`、`logo1.bmp` 都存在且 gzip 可解；e2e 的 PPM 采集路径直接来自 SDL surface，2 色结果可信。
- 扩展内存已分配成功，当前 stdout 中 `exRam:Alloc` 出现，故剩余 blocker 不是 SCRRAM 未映射。
- 子 Agent 与现有窄诊断一致：`game.ext+0x167F4` present 前读取的 `game_rw+0x84` 始终为 0；只画出 DrawRect/DrawText/DrawPoint 说明位图提交回调未安装。
- 当前源码末尾仍有实验性的“直接 `arm_ext_invoke3(primary_helper, code=5)`”补发逻辑；它能让 game code=5 运行并读取 exRam，但没有安装 `game_rw+0x84`，需要继续反汇编 wrapper/graphics 的真实安装协议，避免用特定场景或位图 fallback 修补。

## 2026-06-24 16:40 CST — resumed focused investigation

- 已接收第二个子 Agent 结论：资源格式、MRP 目录、SCRRAM/exRam 路径可信，gwkdl 不是 DOS/VESA 类路径，而是 MythRoad ARM EXT + gzip 资源 + RGB565 framebuffer。
- 当前源码中 `read_game_timer_head()` 仍把 `game_rw+0x8C` 作为主 timer head、`+0x88` 作为备选；运行诊断显示 gwkdl 的有效 timer head 在 `+0x88`，而 `+0x84` 是 present 前 draw callback 槽。后续检查会避免把这三个相邻槽混用。
- 已再次委派两个只读子 Agent：一个反汇编追 `game_rw+0x84` 的真实安装者，一个检查 executor 的 wrapper/chain-walker 补偿逻辑是否与 19KB `cfunction.ext` 不一致。
- 下一步先用窄诊断确认宿主 timer 是否进入 `arm_ext_call_dispatch()` 的 chain-walker 分支，以及 graphics 子模块 helper 是否被持续调用；不启用全量 trace。

## 2026-06-24 16:57 CST — latest narrow diagnostics

- 执行：
  `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_TIMER_DIAG=1 VMRP_ARM_EXT_MPS_DIAG=1 VMRP_ARM_EXT_GFX_REL_DIAG=1 VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-yj4lDD`，仍失败，`menu.ppm` 只有 2 色。
- `arm_ext_primary_helper()` 在该 wrapper 上持续返回 `0x0`：
  `wrapper_timer_dispatch=0`、`chain_walker_thunk=0xE83B51`，所以 `mr_timer()` 当前不会进入 `arm_ext_call_dispatch()`；实际运行路径仍是普通 wrapper helper `code=2`。
- wrapper `code=2`/MPS 与 graphics 子模块确实在执行；`GFX_REL_DIAG` 命中 second/third child 的 renderer/scheduler，说明 graphics 模块并非完全未初始化。
- `GAME_RW_WRITE_DIAG` 只看到 `game_rw+0x88` timer head 被 `game.ext+0x16A0E` 一类代码反复挂/摘，`game_rw+0x84` 始终没有任何写入；`game_rw+0x1C` 从启动早期开始保持 `0x270D`。
- 因此下一步从反汇编核对 `game_rw+0x1C=0x270D` 的含义：该值会让 primary/graphics renderer 走 `table[145] mr_platDrawChar`/点绘制兼容路径，可能解释为什么最终只有背景色和黄字，而没有 `zm.bmp` 的多色位图。

## 2026-06-24 17:16 CST — executor cleanup before next repro

- 重新反汇编 `/tmp/gwkdl-exts/cfunction.ext`，确认 19KB wrapper 的 helper
  switch 中 `code=2` 在 `0xE82E54` 调用 `0xE83E6C`，`code=5` 在
  `0xE82E68` 调用 `0xE83B50`。`0xE83B50 -> 0xE8261C -> 0xE83590` 是
  suspend/resume bucket walker，不是 timer tick。
- `0xE83E6C` 的 literal 为 `0x278`，执行路径是
  `r7 = r9 + 0x278 - 0x80` 后读写 `[r7+0x7C]`，即真实 delta timer head
  为 `wrapper_rw+0x274`。旧实验把 `wrapper_rw+0x1F4` 当 delta head 是错的。
- 已修改 `src/arm_ext_executor.c`：
  - 移除 `code=2` 前后强行设置/清零 `wrapper_rw+4` busy 标志的实验。
  - 移除向 `wrapper_rw+0x1F4` 注入 extChunk timer node 的实验。
  - 移除宿主直接调用 primary `game helper code=5` 的实验。
  - 保留诊断中对 `wrapper_rw+0x274` 的观测，并补注释说明宿主只发送
    wrapper helper `code=2`，不伪造 resume/code=5。
- 下一步：重建并跑目标 e2e，若仍失败，再只开 `MPS_DIAG/TIMER_DIAG` 检查
  wrapper 自身是否消费 `wrapper_rw+0x274` 并调用 node callback。

## 2026-06-24 继续 — resumed handoff audit

- 接续上一轮结果后先核对当前工作树，而不是假设文档/摘要中的清理已经完全落地。
- 子 Agent 已确认 19KB `cfunction.ext` 的 `0xE83B50 -> 0xE8261C -> 0xE83590`
  是 suspend/resume bucket walker，不是 timer tick；真实 timer consumer 是
  helper `code=2` 内部的 `0xE83E6C`，其 delta head 为 `wrapper_rw+0x274`。
- 当前 `src/arm_ext_executor.c` 仍存在 `arm_ext_call_dispatch()` 中的
  `chain_walker_owner` 特例和后处理。虽然 `arm_ext_primary_helper()` 对这份
  wrapper 返回 0，普通 `mr_timer()` 当前不进入该分支，但它与反汇编协议冲突，
  后续修复不能继续依赖这个路径。
- 另一个子 Agent 指出更可疑的通用缺口：`capture_timer_dispatches()` 会捕获
  ARM 侧替换到 table[31]/table[32] 的 timerStart/timerStop veneer，并把表项
  恢复成宿主 hook；但 table[31]/[32] hook 目前只调用宿主
  `mr_timerStart()/mr_timerStop()`，没有执行捕获到的 ARM veneer。若 child/graphics
  调度器依赖这个 veneer 注册内部 timer/callback，则宿主会吞掉关键副作用。
- 下一步先做窄诊断确认 table[31]/[32] 的 veneer 捕获/调用缺失是否与
  `game_rw+0x84` 未安装、`game_rw+0x1C=0x270D` 的文字路径相关；同时准备移除
  `chain_walker_owner` 旧实验路径，避免它干扰后续判断。

## 2026-06-24 17:25 CST — narrow table/timer repro

- 重建通过：`cmake --build build --target vmrp`。
- 运行：
  `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_TRACE=1 VMRP_ARM_EXT_TABLE_DIAG=1 VMRP_ARM_EXT_GAME_RENDER_WRITE_DIAG=1 VMRP_ARM_EXT_TIMER_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-FdcdoJ`，仍失败；`menu.ppm` 仍只有蓝底/黄字 2 色。
- 该 run 的 `stdout.log` 因 `VMRP_ARM_EXT_TRACE=1` 较大（约 22MB），但关键信息有限：
  - 未出现 `captured timerStart` / `captured timerStop`，说明全局
    table[31]/[32] 在本次路径没有被 ARM veneer 直接替换后再被
    `capture_timer_dispatches()` 捕获。
  - table[31]/[32] 各 168 次，调用 LR 分别为 `0xE83E55`/`0xE83E4B`，
    来自 wrapper `code=2` timer consumer；形态是每 tick stop/start 宿主 timer。
  - `wrapper_rw+0x274` delta head 始终为 `0x348E40`，callback 为
    `0xE835E1`，wrapper timer consumer 确实在跑。
  - `GAME_RENDER_WRITE_DIAG` 300 次命中 primary renderer，但写入地址都在
    栈/临时缓冲 `0x2A7Axx`，`screenOff=FFFFFFFF`、`exramOff=FFFFFFFF`；
    未进入直接 framebuffer 写屏路径。
  - `game_rw+0x84` 仍为 0，`game_rw+0x88` 为 `0x2F0C74`，`game_rw+0x90`
    随时间递增。
- 结论修正：`dispatch_timer_start/stop` 捕获但未调用仍是 executor 的通用缺口，
  但 gwkdl 当前失败路径没有证明是“全局 table[31]/[32] veneer 被捕获后吞掉”。
  需要继续追 private-loader record/RW 内部的 timer veneer，或 renderer 分支
  条件（特别是 `game_rw+0x1C=0x270D` 导致的 `mr_platDrawChar`/点绘制路径）。
## 2026-06-24 18:32 CST — resumed from current workspace

- 本轮任务：修复 `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`
  自动进入游戏界面后 logo/游戏图像不显示的问题；保持“不写应用特判、不要
  xvfb、过程记录到本文档、先修该用例”的约束。
- 已读 `AGENTS.MD` 和本文档结尾。当前历史结论显示：`zm.bmp` 等资源可离线解出，
  `menu.ppm` 失败形态是 2 色蓝底/黄字；扩展内存分配已成功，剩余重点在
  ARM EXT timer/graphics callback/present 链路。
- 已启动 3 个只读子 Agent 分别检查 e2e/PPM 产物、图像解码/绘制链路、MRP
  反汇编与扩展内存调用线索。

## 2026-06-24 18:34 CST — rebuilt and reproduced

- 执行 `cmake --build build --target vmrp` 成功；仅 vendored Unicorn 出现既有
  编译 warning。
- 执行 `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`
  仍失败：`wake.uniqueColorCount()` 实际为 2。
- artifact: `/tmp/vmrp-e2e-4EBcSO`。
- PPM 统计：`menu.ppm` 为 240x320、2 色，`#1060a8` 76499 像素、
  `#f8f000` 301 像素；`mem-check.ppm`/`bgm-select.ppm` 也保持 2 色。
- 下一步用窄诊断确认资源读、table[120/145] 图形调用、game present 前
  `game_rw+0x84` 与 timer 链状态。

## 2026-06-24 18:35 CST — focus on logo BMP loading

- 根据用户提示先收窄到两张 logo：`strings -a test/fixtures/gwkdl_v1003.mrp`
  可见 `logo0.bmp` 与 `logo1.bmp`，历史解包目录
  `/tmp/vmrp-gwkdl-extract-13481/{raw,dec}` 中也存在两者。
- 本文档早期记录显示二者 gzip payload 可离线解码，且曾观察到
  `logo0.bmp` 尺寸级别的大块数据 copy 到 SCRRAM/exRam；下一步直接用
  `VMRP_ARM_EXT_FILE_DIAG` 复跑，确认当前运行时是否打开/读取这两个文件。

## 2026-06-24 18:37 CST — logo BMPs are read at runtime

- 运行：
  `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_FILE_DIAG=1 VMRP_ARM_EXT_TABLE_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-EZjen1`，仍失败；stdout 约 15MB，未开全量 trace。
- 解析 MRP 目录：
  - `logo0.bmp`: offset `336333` (`0x521cd`), packed `15498` (`0x3c8a`), gzip。
  - `logo1.bmp`: offset `351849` (`0x55e69`), packed `2651` (`0xa5b`), gzip。
- 日志确认运行时实际读到了两者：
  - `logo0.bmp`: `seek pos=336333` 后 `read len=15498 ret=15498`，dst `0x2FA666`。
  - `logo1.bmp`: `seek pos=351849` 后 `read len=2651 ret=2651`，dst `0x2F84B5`。
- 离线解压确认 decoded 长度：
  - `logo0.bmp`: `37264` bytes，前几个 RGB565 像素为 `0x0000`。
  - `logo1.bmp`: `15792` bytes，前几个 RGB565 像素为 `0xffdf`。
- 因此两张 logo 的“文件查找/读取”不是当前 blocker；剩余问题在读后解压/
  copy 到 SCRRAM/exRam 之后，没有被提交到可见 framebuffer/present。

## 2026-06-24 18:40 CST — table usage after logo load

- 子 Agent 结果与本地结果一致：资源和 PPM harness 正常；两张 logo 是多色
  decoded payload，当前失败不在文件系统/解包层。
- `/tmp/vmrp-e2e-EZjen1/stdout.log` 统计：`table[29]` 166 次、
  `table[119]` 33646 次、`table[122]` 187 次、`table[145]` 80 次；
  没有 `table[120]`、`table[121]`、`table[147]`、`table[148]`。
- 这说明 logo 读取后没有走 host `_DrawBitmap`/`mr_transbitmapDraw`/
  `mr_drawRegion`，仍然是 ARM 侧 renderer/graphics callback 决定是否把
  decoded RGB565 写进 screen buffer。下一步手动按过“请按任意键”界面，
  捕获 logo/游戏界面的 PPM 与同一组窄诊断。

## 2026-06-24 18:55 CST — protocol recheck

- 收到新的子 Agent 结论：`game_rw+0x84` 在反汇编中只是 present 前可选
  hook 的读取点，不能单独当作根因；更有证据的路径是 `game.ext` 通过
  selector `0x21` 向 `graphics.ext` 请求 9 项导出表，写入约
  `game_rw+0x244` 的调用表。
- 当前工作树的 `git diff` 仅显示本文档变化，说明上一轮实验性源码变更已
  收敛进当前基线或被清理；下一步将以当前源码重新复跑窄诊断，确认
  `0x21` 导出表是否填充、这些函数是否被调用，以及 renderer 是否因
  `[game_rw+0x1C] == 0x270D` 或其它状态只走文字点绘制路径。

## 2026-06-24 19:50 CST — narrow repro on export-table path

- 执行带窄诊断的目标用例：
  `VMRP_ARM_EXT_CHILD_DIAG=1 VMRP_ARM_EXT_GFX_REL_DIAG=1 VMRP_ARM_EXT_GAME_RENDER_DIAG=1 VMRP_ARM_EXT_GAME_RW_WRITE_DIAG=1 VMRP_ARM_EXT_TABLE_DIAG=1 VMRP_ARM_EXT_SCREEN_WRITE_DIAG=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-ev7NKb`，仍失败，`menu.ppm` 只有
  `#1060a8` 和 `#f8f000` 两色。
- 运行时同步了三段 child：primary `game.ext`、`graphics.ext`、第三个
  child。`graphics.ext` 的 selected record 中 `record[31]/record[32]`
  已是模块内 timer veneer，`record[120]/[121]/[122]/[145]` 等仍指向
  host table bridge。
- `GFX_REL_DIAG` 证明 `graphics.ext` 和第三个 child 已执行；`GAME_RENDER_DIAG`
  证明 `game.ext+0x1127C` 和 renderer 进入；但 `SCREEN_WRITE_DIAG` 为 0，
  `table[120]/[121]/[147]/[148]` 调用数为 0。下一步必须监控
  `game_rw+0x244..0x268` 的 9 项 selector `0x21` 导出表，而不是继续追
  logo 文件读取或 present 后端。

## 2026-06-24 20:08 CST — continue: two-logo check and export-table focus

- 继续上一轮状态。当前最新证据仍是：`logo0.bmp` 与 `logo1.bmp` 在运行时都
  已 seek/read 成功，且离线解压为多色 RGB565 数据；失败点不在 MRP 查找或
  gzip 资源读取。
- 已收到子 Agent 的反汇编结论：`graphics.ext+0x1278` 对 selector `0x21`
  会向调用者缓冲区写 9 个函数指针；其中 slot `+0x1C -> graphics.ext+0x094A`
  后续会进入 framebuffer blit/write loop。
- 当前关键矛盾：运行时 `game_rw+0x244..0x264` 仍全 0，且窄诊断没有命中
  `graphics.ext+0x1278`。因此下一步直接检查 `game.ext+0x158D4` 的模块
  selector 调度协议，以及 executor 对 private/nested EXT helper/record 的
  同步实现，确认 selector 请求为什么没有转给 graphics child。

## 2026-06-24 20:24 CST — disassembly pin: 0x158D4 dispatch wrapper

- `game.ext+0x15DF0..0x15E02` 明确构造最终导出请求：
  `r0=5`、`r1=0x21`、`r2=r9+0x244`、`r3=0x24`，然后调用
  `game.ext+0x158D4`。这与 `graphics.ext+0x1278` 的接口完全匹配：
  selector `0x21` 且输出指针非 0 时，写 9 个 graphics 函数到调用者缓冲区。
- `game.ext+0x158D4` 不是直接 host table 调用。它保存原始 `r0-r3`，从当前
  module 的 `P+0x0C` 取 `extChunk`，再从 `extChunk+0x28` 取 dispatch 函数
  `blx`。因此 selector 请求应先进入 cfunction 私有 dispatch，再由 wrapper
  转给目标 child（graphics.ext）。
- 这把问题从“是否读到两张 logo.bmp”进一步缩小到：cfunction 私有 dispatch
  没有把 `code=5, selector=0x21` 路由到 `graphics.ext+0x1278`，或运行时
  `0x158D4` 取到的 `extChunk/dispatch` 槽不是预期值。下一步增加
  `VMRP_ARM_EXT_SELECTOR_DIAG` 只记录该 wrapper 的入口参数和 `extChunk+0x28`
  目标，不打开全 trace。

## 2026-06-24 20:14 CST — selector handoff probe added

- 在 `src/arm_ext_executor.c` 增加 `VMRP_ARM_EXT_SELECTOR_DIAG`，只记录三段
  反汇编锚点：
  - primary `game.ext+0x158D4..0x15910`：最终 `code=5, selector=0x21`
    dispatch 包装器。
  - wrapper `cfunction.ext+0x3260..0x356C`：私有 child loader/dispatcher。
  - child `graphics.ext+0x1278..0x12C0`：selector `0x21` 的 9 项导出表。
- 该 probe 使用 module-relative offset，避免依赖 private loader 每轮不同的
  heap 映射地址；同时打印 `P+0x0C`、`extChunk+0x24/+0x28/+0x34`、栈参数与
  `game_rw+0x244..0x264`。
- 目的：证明 selector 请求是在 `game.ext` 没发出、发给了错误 dispatcher，
  还是到了 `graphics.ext` 但导出表未写。

## 2026-06-24 20:38 CST — selector probe narrowed

- 上一轮 artifact `/tmp/vmrp-e2e-IF2UHf` 证明 broad `VMRP_ARM_EXT_SELECTOR_DIAG`
  会被 wrapper loader/timer 路径消耗上限：360 条内只看到 primary
  `game.ext+0x1590C` 与 wrapper `0x342C` 一类调用，没有捕获
  `game.ext+0x15DF0..0x15E02` 的最终 `code=5, selector=0x21` 导出请求。
- 已把 selector probe 收窄到 disassembly 锚点：
  primary `0x15D90..0x15E08` 和 `0x158D4..0x15910`，
  wrapper `0x1F50/0x2472/0x34C0/0x356C` 且参数呈 selector 形态，
  child `0x1278..0x12C0`。同时新增 `r0`/`ip` 节点快照，重点看
  wrapper `0x2472` 找到的 node 及 `node+0x30` 是否指向 graphics export handler。
- 下一次运行仍只跑 `test/e2e/gwkdl/game-prepare.test.ts`，避免 broad trace。

## 2026-06-24 21:03 CST — setup branch probe added

- 继续接手当前工作树；确认 `/tmp/vmrp-e2e-J0UCcY` 仍失败，且 diagnostics
  下 `menu.ppm` 退化为 1 色 `#1060a8`。这不改变根因方向：bitmap/export
  路径仍未建立。
- 该 artifact 的 `SELECTOR_DIAG` 只看到 primary `game.ext+0x158D4/0x1590C`
  的普通 selector 包装调用，以及 wrapper 内部 `code=1 selector=0x21`
  走到 `cfunction.ext+0x21C2` 的处理；没有 `owner=child off=0x1278`，
  也没有 `game_rw+0x244..0x264` 写入。
- 更关键的是，日志没有命中 `game.ext+0x15D90..0x15E08`，因此最终
  `code=5 selector=0x21` 导出表请求在截图前没有发出，不能继续假设问题
  只在 wrapper 把请求路由到 graphics child。
- 已新增 `VMRP_ARM_EXT_SETUP_DIAG`，只跟踪 primary `game.ext` 的
  `0x15CB0..0x15E08`、`0x15E18..0x15E28` 和 `0x11160..0x112A0`。目的：
  证明 `0x15CB0 -> 0x15E18 -> 0x15D2C` 是否执行、`0x15D2C` 是否早退，
  以及是否到达 `0x15DF0/0x15E02` 最终 export request。

## 2026-06-25 09:02 CST — resumed baseline

- 子 Agent 尝试失败：两个只读 Agent 都因 429 exceeded retry limit 没有返回
  可用结论；本轮继续用本地窄诊断推进。
- 当前工作树复查：`git diff -- src/arm_ext_executor.c ...` 对源码/测试为空；
  `git status --short` 只显示用户已有的 `docs/prompt.md` 修改。当前源码仍包含
  env-gated gwkdl 窄诊断 hook、512KB `origin_mem_len`、origin memory slot
  同步、动态 MRP cache 等既有改动。
- 干净复现命令：
  `cmake --build build --target vmrp && env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`。
- artifact: `/tmp/vmrp-e2e-oyYei4`。测试仍失败：
  `wake.uniqueColorCount()` 实际为 2。
- PPM 统计：
  - `mem-check.ppm`: 2 色，黑色 72119、`#f8fcf8` 4681。
  - `bgm-select.ppm`: 2 色，黑色 76365、`#f8f000` 435。
  - `menu.ppm`: 2 色，`#1060a8` 76499、`#f8f000` 301。
- stdout 关键点：`exRam:448512B required.` 后成功
  `exRam:Alloc:addr:0x35105c,len:449536`。因此 512KB origin memory 已让
  SCRRAM 分配成功，但这不足以恢复 logo/`zm.bmp` 位图显示。
- 下一步继续验证 `game.ext` 是否到达 `0x15D2C/0x15DF0` 并发出
  `code=5, selector=0x21, out=game_rw+0x244` 的 graphics 导出表请求；
  若未到达，则根因在 `game.ext` setup/state gate，而不是 MRP 读文件或
  host `_DrawBitmap`。

## 2026-06-25 09:54 CST — user-info version gate narrowed

- 复查当前 `mr_userinfo` 布局：`IMEI[16] + IMSI[16] + manufactory[8] +
  type[8]` 后 `ver` 位于输出 buffer 偏移 `0x30`，正是
  `game.ext+0x11964` 从 `[sp+0x34]` 读取的字段（调用时输出指针为
  `sp+4`）。
- 反汇编 `game.ext+0x11964` 的当前判断：
  - 先要求 `0x06052340 <= ver < 0x06146580`，即 `101000000..101999999`。
  - 然后只对几个子区间返回非 0：
    `101100000..101109999 -> 0x40017000`，
    `101030000..101039999 -> 0xA000D000`，
    `101040000..101049999` 或 `101070000..101079999 -> 0x40018800`。
- 当前 `#define MT6229` 生成 `ver=101090182` (`0x06068386`)，虽满足早先记录的
  `(ver + 0xF9F9A440) > 0x270F` 阈值，但不在上述任何子区间内，因此
  `0x11964` 返回 0，`0x15E18` 返回 0，`0x15CB0` 跳过 `0x15D2C`，
  最终不会发出 graphics selector `0x21` 导出表请求。
- 先尝试将 vmrp 默认 DSM 平台从 `MT6229` 改为 `MT6253`。这不是
  app-specific 分支；它只是声明一个真实的 MTK 平台，使通用
  `mr_getUserInfo().ver` 变为 `101100182`，落在当前反汇编接受的
  `101100000..101109999` 区间。
- 该值让进程在 MRP startup 内走入较重路径，E2E harness socket 30s 内未创建
  （artifact `/tmp/vmrp-e2e-UmpTSc`，最后反复 `dsmSwitchPath`/`netpay.mrp`）。
  改用同样被反汇编接受的真实 MTK 平台 `MT6227`，生成
  `ver=101070182`，对应 `101070000..101079999 -> 0x40018800`。
- 下一步 rebuild 后先跑目标 e2e；若仍失败，再用 selector/setup 窄诊断验证
  `game.ext+0x15D2C` 和 `graphics.ext+0x1278` 是否已命中。

## 2026-06-25 10:28 CST — wrapper key-node probe added

- `MT6227` 的 startup 窄诊断已证明版本门通过：`game.ext+0x15E18`
  返回 `0x40018800`，并进入 `game.ext+0x15D2C`。
- 该路径先发出 `code=1, selector=0x21, key=5` 清理请求，然后发出
  `code=1, selector=0x20, key=5` 注册请求；日志在 selector `0x20`
  之后停止，尚未到达 `game.ext+0x15DF0..0x15E02` 的最终
  `code=5, selector=0x21, out=game_rw+0x244` 导出表请求。
- 反汇编复核 `cfunction.ext` 的节点表：
  - `0xD30..0x10A4` 创建/注册节点，key `<=32` 时写
    `wrapper_rw+0x30+key*4`，所以 graphics key 5 在 `wrapper_rw+0x44`。
  - `0x10E0..0x1184` 清理节点，并把同一 key slot 清零。
  - `0x2472..0x24A2` 通过 key 查 node，要求 `node+0x30` 为 child callback，
    再进入 `0x34C0` trampoline。
- 已新增 `VMRP_ARM_EXT_NODE_DIAG`，只跟踪上述 wrapper 反汇编范围，打印
  `wrapper_rw`、key slot、key 5 node 字段、`node+0x30` callback 的模块归属。
  下一轮用它确认 selector `0x20` 是否创建 key 5 node，以及 callback 是否
  指向 `graphics.ext+0x1278`。

## 2026-06-25 10:25 CST — node registration detail probe

- 复核 `cfunction.ext+0xD30..0x10A4`：selector key 注册的关键路径为
  `0xF9C -> 0x220`（解析/取出模块数据）、`0xFAC -> 0x32DC`（生成 child
  callback/loader record）、`0xFE2` 写 node 字段、`0xFFA` 写 key table，
  `0x1036/0x1044` 决定失败清理还是成功返回。
- 已新增独立的 `VMRP_ARM_EXT_NODE_DETAIL_DIAG`，只记录这些精确 offset，
  不复用 broad `NODE_DIAG` 的 220 条上限。输出包括：
  - `sp+0xD0` 构造出的 path 字符串；
  - `sp+0x8C/0x88` 的 data pointer/length；
  - `sp+0xC4/0xC8` 输出槽及其当前值；
  - `r4` node 的关键字段、`node+0x30` callback 归属，以及
    `wrapper_rw+0x40/0x44/0x48` key 4/5/6 slot。
- 下一次命令：
  `env VMRP_ARM_EXT_SETUP_DIAG=1 VMRP_ARM_EXT_SELECTOR_DIAG=1 VMRP_ARM_EXT_NODE_DETAIL_DIAG=1 timeout 12s build/vmrp --work-dir . test/fixtures/gwkdl_v1003.mrp > /tmp/gwkdl-node-detail.out 2> /tmp/gwkdl-node-detail.err`。
  目标是直接判断 `0x220` 是否返回非 0、`0x32DC` 是否返回 `-1`，
  还是后续 key-table store 被跳过。

## 2026-06-25 10:41 CST — platform graphics band mapped, target passes

- 复跑当前窄诊断确认了子 Agent 的关键结论：`game.ext+0x15D2C` 的
  selector `0x20` / key `5` 注册进入 `cfunction.ext+0xD30`，并把
  `game.ext+0x11964` 返回的 `0x40018800` 作为 caller-owned output band
  传给 `cfunction.ext+0x220`。
- 反汇编依据：
  - `game.ext+0x15DB2..0x15DCA` 调 `0x11964`，随后发出
    `code=1, selector=0x20, key=5`，栈参数包含 `0x40018800`。
  - `cfunction.ext+0xF80..0xF9C` 若 `node+0x18` 非 0，则将其写入
    `[sp+0x8C]`，并把 `[sp+0x88]` 设为 `0x00100000`，再调用
    `0x220` materialize/load helper。
  - `cfunction.ext+0xFFA` 只有在 `0x220` 和 `0x32DC` 成功后才把 node
    发布到 `wrapper_rw+0x30+key*4`；key 5 即 `wrapper_rw+0x44`。
- 失败机理：vmrp 之前只把 `0x00010000..0x01010000` 映射为 ARM EXT 内存，
  `0x40018800` 不在 `arm_ptr()`/Unicorn 可见范围内。wrapper 希望把
  `graphics.ext` materialize 到该 MTK graphics/device band，结果 key 5
  注册不能完成，后续 graphics selector/export 链路建立不完整。
- 实现修复：
  - 新增 ARM-visible platform bands：
    `0x40000000..0x401FFFFF` 与 `0xA0000000..0xA01FFFFF`。
    前者覆盖本机 `MT6227` 路径返回的 `0x40018800`，后者覆盖同一
    `game.ext+0x11964` 反汇编中另一组可接受版本返回的 `0xA000D000`。
  - `arm_ptr()`/`arm_addr()` 支持这些 band，并在 `arm_ext_load()` 中用
    `uc_mem_map_ptr()` 映射；`hook_restore_r9` 也挂到这些 band，保证 child
    code 若保留高地址运行，仍能走通用 R9/module 恢复逻辑。
  - 这是地址模型修复，不是 gwkdl 分支，也不是图片 fallback。
- 验证：
  - `cmake --build build --target vmrp` 通过。
  - `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`
    通过，artifact `/tmp/vmrp-e2e-ay9OBT`。
  - PPM 统计：`menu.ppm` 从失败时的 2 色变为 52 色；
    主要颜色包括蓝底 `#1060a8` 54801 像素，以及多组位图颜色
    `#c8ecf8`、`#f8f8f8`、`#001020`、`#080830`、`#a87000`、
    `#f86800` 等。该结果说明 logo/bitmap 像素已进入最终 framebuffer。

## 2026-06-25 10:43 CST — completion audit rerun

- 按原目标重新验证当前工作树，而不是只沿用上一轮结果：
  - `cmake --build build --target vmrp` 通过。
  - `git diff --check` 无 whitespace error。
  - 未使用 `xvfb`。
  - `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`
    通过，artifact `/tmp/vmrp-e2e-AGnB50`。
- PPM 证据：
  - `menu.ppm`: `240x320`，52 色；top colors 包括蓝底 `#1060a8`
    54801 像素，以及 `#c8ecf8`、`#f8f8f8`、`#001020`、`#080830`、
    `#a87000`、`#f86800`、`#e81000`、`#70a898` 等位图颜色。
  - `mem-check.ppm` 和 `bgm-select.ppm` 仍是预期的文字/选择页 2 色画面。
- 结论：目标用例自动进入“请按任意键”界面后，`zm.bmp`/logo 类位图像素已
  进入最终 framebuffer；不再是纯色背景。

## 2026-06-25 21:23 CST — full e2e regression surface

- 继续原目标：gwkdl 已通过，但需要修复 `ce382f4` 后其它 e2e 回归。
- 当前工作树复查：存在已暂存的 `src/arm_ext_executor.c` 修正，内容为移除
  早期 gwkdl 调查中未最终采用的 `compact-sdk` RW bridge 镜像，仅保留
  `verdload` 已反汇编确认的 RW 布局；`docs/prompt.md` 仍是用户已有修改。
- 构建：`cmake --build build --target vmrp` 通过。
- 完整命令：`pnpm vitest run test/e2e`。结果 5 个测试文件通过、2 个失败：
  - 通过：`gwkdl/game-prepare`、`dota/download-plugin`、`optwar/game-prepare`、
    `opbzqe/game-prepare`、`gxdzc/gxdzc-pixel`。
  - 失败：`gghjt/download-plugin` 4 个用例、`gghjt/game-start` 2 个用例。
- 失败形态：
  - `download-plugin` 的“返回重进”和 `game-start` 的“游戏正式开始”卡在
    `WAIT_DRAW`，draw count 不再增加。
  - 其它下载完成/付费路径在 `pay-start` 采样点 `(104,147)` 读到
    `[184,144,64]`，预期为 `[104,104,224]`，说明仍停留在前一层/错误画面。
  - `game-start` 的“游戏直接开始”第二次启动 30s 内 socket 未 ready。
- 子 Agent 结论与当前修正方向一致：`compact-sdk` RW bridge 镜像是高风险
  回归点，因为 RW 偏移是 private child 私有布局，不是通用 ABI；但完整 e2e
  证明仅移除该镜像还不足以恢复 gghjt 所有路径。
- 下一步 focused rerun：用 `VMRP_E2E_KEEP_TMP=1` 保留 gghjt 失败用例 artifact，
  对比 `ce382f4` 新增的平台/version/origin memory 改动是否改变 gghjt
  启动、下载、pay modal 的 private child 状态。

## 2026-06-25 21:31 CST — gghjt focused diagnosis

- 最小复现：
  `env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gghjt/download-plugin.test.ts -t "下载付费插件 - 下载完毕$"`。
  artifact `/tmp/vmrp-e2e-Mq81aa`，失败点仍是 `pay-start.ppm`
  `(104,147)=[184,144,64]`，预期 `[104,104,224]`。
- PPM 证据：
  - `download-plugin.ppm`、`download-ing.ppm`、`download-end.ppm` 均为下载 UI
    色系，说明 `verdload` 下载子模块能显示、文件写入也成功。
  - `stdout.log` 有 `mr_remove(mythroad/plugins/netpay.mrp) ret:-1` 和
    `mr_rename(mythroad/vld/7530a10/7530a1.t to mythroad/plugins/netpay.mrp)`，
    说明插件落盘成功。
  - 点击“确定”后 `pay-start.ppm` 变回主菜单色系，未进入 netpay/pay modal。
- `VMRP_ARM_EXT_DIAG=1` artifact `/tmp/vmrp-e2e-EnIs6V`：
  - 下载过程中 active child 为 `P=0x39D548/H=0x32B2FD`，primary
    `extChunk[0x34]=1`，foreground cover 是全屏，符合模态 child 显示。
  - 关闭后 active 回到 primary `P=0x34AB24/H=0x2E2325`，primary
    `extChunk[0x34]=0`，但后续 timer 仍只让 wrapper code=2 反复运行，pay
    modal 没有创建。
  - `arm_ext_primary_helper()` 因 `wrapper_timer_dispatch_addr=0` 返回 0，
    gghjt 19KB wrapper 继续走普通 code=2 路径；这解释了为什么不会进入
    `arm_ext_call_dispatch()` 的 chain-walker fallback。
- 子 Agent 指出另一处高风险：`MODAL_FG_SNAPSHOT_LEN=0xD0` 只覆盖
  `wrapper_rw+0xE0..0x1AF`，而历史反汇编文档确认 gghjt child-flow list
  区域是 `wrapper_rw+0x190..0x1B7`。这会漏还原 `0x1B0..0x1B7`，与
  `verdload` bridge 区 `rw+0x170..0x1B0` 的别名风险相邻。
- 下一步实验 A：把 wrapper 前台/child-flow 快照覆盖扩到
  `0xE0..0x1B7`，这是反汇编已有证据支持的 wrapper 状态区扩展，不是
  app-specific 分支；若 focused gghjt 恢复，再跑 gwkdl 和完整 e2e。

## 2026-06-25 22:46 CST — gghjt regressions fixed, full e2e green

- gghjt 下载/付费路径的 keepable 修复分两部分：
  - 去掉 `ce382f4` 中来自早期 gwkdl 试验、未被最终反汇编证据采用的
    `compact-sdk` private-child RW bridge 镜像，仅保留 `verdload` 已证明
    的 `rw+0x16C..0x1B0` bridge 布局。该变更恢复下载子模块自身状态，
    `pnpm vitest run test/e2e/gghjt/download-plugin.test.ts` 5 个用例全过。
  - gghjt close callback 会在模态关闭时安装新的 game timer head
    `0x32CF44`；旧清理逻辑随后把它覆盖成 suspend 前保存的 `0x2F5874`，
    导致异步请求处理丢失、下载完成后 pay modal 不出现。现在仅当当前
    game timer head 仍为 0 时才恢复保存值。
- `game-start` 的“游戏直接开始”表面是 30s socket timeout，实际是
  `startVmrp()` 内部已经崩溃，E2E socket 尚未创建。窄 gdb 断点显示：
  `table[14] memset(r0=0x4, r1=0, r2=0xFFFFFFFC)`，ARM caller 为
  `game.ext` parser `0x2DFCE2 -> 0x2DB7E0`，其长度来自
  `0x2DD96C` 读取 signed 16-bit count 后返回 `-1`。
- 反汇编结论：这不是宿主 `memset` 问题，而是资源包 parser 在缺失/无效
  `gghjt.pak` 上读到 EOF，随后把 `count=-1` 传给 allocator，形成 `-4`
  字节清零长度。
- 文件桥窄诊断显示冷启动先读 `gghjt\res.list`，再打开
  `gghjt\gghjt.pak`。`gghjt.mrp` 的目录顺序中 `res.list` 位于 entry 17；
  `ce382f4` 把 MRP cache 从固定 16 条扩展成动态全量后，basename fallback
  让 `mr_open("gghjt\\res.list")` 在磁盘目录为空时也从包内成功。game 因此
  认为提取状态存在，但 `gghjt.pak` 尚未落盘，后续直接进入 parser 并崩溃。
- 实现修复：保留动态 MRP cache（gwkdl 需要 entry 16 之后的资源），但只对
  “直接资源名”使用 basename fallback。带目录组件的请求必须按真实文件系统
  语义失败，除非包内 entry 路径与请求路径完全一致；这些 `dir/file` 请求在
  真实设备上通常是持久化提取状态探测，不能被包内 basename 资源掩盖。
  该规则是通用路径语义修正，不是 gghjt 分支。
- 验证：
  - `cmake --build build --target vmrp -j2` 通过。
  - `pnpm vitest run test/e2e/gghjt/download-plugin.test.ts` 通过：
    5 passed，约 206s。
  - `pnpm vitest run test/e2e/gghjt/game-start.test.ts` 通过：
    2 passed，约 53s。
  - `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts` 通过：
    1 passed，约 13s，证明动态 cache 对 gwkdl 后段资源仍有效。
  - `pnpm vitest run test/e2e` 通过：
    7 files / 12 tests 全部通过，约 325s。
  - `git diff --check` 无 whitespace error。
