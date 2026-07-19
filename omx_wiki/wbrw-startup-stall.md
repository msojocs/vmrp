# WBRW Startup Stall

## 2026-07-01 09:15 CST

Command under investigation:

```sh
build/skyengine build/mythroad/wbrw.mrp
```

Observed failure: the app stays on the green `正在启动` progress screen.

Important constraint from current evidence: this is not primarily a network bug. Matrix runs already showed `mphome.sky` being downloaded and written to `mythroad/brw/lpg/mphome.sky`, but the UI still stayed on the startup progress screen. Real devices can also enter the browser without network, so the active investigation moved to EXT startup, timer ownership, platform capability responses, and the transition from the loader module to the browser renderer.

Current runtime chain from narrow diagnostics:

- wrapper `cfunction.ext`: `P=0x2A5C6C`, helper `0xE80154`
- primary `game.ext`: file `0x2C6130/329836`, `P=0x38F114`, helper `0x309719`, RW `0x3169A4`
- startup UI `frame.ext`: file `0x31CA18/11480`, `P=0x3922C0`, helper `0x31E395`
- downloader `verdload.ext`: file `0x31F988/20976`, `P=0x39DA24`, helper `0x3238E1`, RW `0x324B7C`

The stall loop repeatedly routes code `2` timer events through wrapper `P=0x2A5C6C/H=0xE80154` while `verdload.ext` remains the active module. The primary page state diagnostics remain zero, which is expected if the real browser/page parser has not been entered yet rather than a page payload parse failing after entry.

Next evidence to collect:

- Disassemble `verdload.ext` around helper offset `0x3F59`, receive/write offsets `0x4049/0x4503`, and its `mr_timerStart` call sites.
- Check whether wrapper LR `0xE8459D` is only a veneer for timerStart while the semantic owner should be the active child.
- Check platform calls `mr_plat(1016/1100)` and `mr_platEx(4033)` against disassembly before keeping those stubs.

## 2026-07-01 09:37 CST

Built current tree with:

```sh
cmake --build build --target skyengine -j 4
```

Reproduced with no xvfb:

```sh
timeout 35 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 SKYENGINE_PPM_PATH=/tmp/wbrw-current.ppm \
  VMRP_ARM_EXT_DIAG=1 ./build/skyengine build/mythroad/wbrw.mrp \
  >/tmp/wbrw-run.log 2>&1
```

Result: still timed out (`status=124`) and wrote a valid 240x320 PPM. The
runtime is no longer stuck only in `verdload.ext`: later diagnostics show the
foreground module as `P=0x3A0A60/H=0x4001A761`, with wrapper timer owner
`P=0x2A5C6C/H=0xE80154`.

Representative final loop:

- `activeP=0x3A0A60 activeH=0x4001A761 activeRW=0x31CA1C`
- `primaryP=0x38F114 primaryH=0x309719 primaryRW=0x3169A4`
- `timerP=0x2A5C6C timerH=0xE80154`
- `active[28]=0xE83A25`, `wrapperTimer=0xE845BD`
- `game_timer_head_zero grw=0x3169A4 code=2`
- repeated file probes: `brw/upd/wvercfg.bin`, `brw/wvercfg.bin`,
  `brw/upd/wbrw.mrp`, `mythroad/wbrw.mrp`, `brw/brw2.cfg`

This means the prior download phase made progress and handed off into the
next browser/update module, but the page renderer is still not reached. The
next target is to identify `P=0x3A0A60/H=0x4001A761` from nested-module
registration logs and check why its event/timer path does not advance past
the update/config probes.

## 2026-07-01 09:48 CST

New evidence from `/tmp/wbrw-run.log`:

- `P=0x3A0A60/H=0x4001A761` is `brwgraphics.ext`, loaded from the
  `wbrw.mrp` entry whose gunzipped size is 11012 bytes.
- The local `/page2` request succeeds through `proxy2.51mrp.com` and the
  payload later appears in the receive log as `[b.runapp]`.
- The last PPM is still the startup screen, not a stale capture:
  `[48,176,32]` covers 87.77% of pixels.
- After `/page2`, `brwgraphics.ext` opens `brw/lpg/mphome.sky.dt` for write
  and later `brw/lpg/mphome.sky.dt` for read, then continues into another
  `simpleDownload`/update loop.
- No `present29` or accepted screen-write diagnostics appear for
  `activeP=0x3A0A60`; the only visible presents are still the old
  `frame.ext` progress bar region/full-screen startup paint.

Working hypothesis: the current bug is after successful page download. Either
the browser-side response/file format is rejected and causes a generic update
retry, or a host ABI/file-open/timer semantic mismatch prevents
`brwgraphics.ext` from entering the renderer. Do not narrow the fix to WBRW
by name; verify through disassembly and generic platform semantics.

## 2026-07-01 10:16 CST

Rechecked current worktree state and latest `/tmp/wbrw-run.log`. The newest
build now stops earlier than the 09:48 run: after `verdload.ext` receives the
first `/simpleDownload` response it sends `/dl_confirm`, then one more
`code=2` callback returns with `hostTimer=0 mrTimer=0`.

Representative tail:

- active module: `verdload.ext`, `P=0x39DA24`, `H=0x3238E1`, `RW=0x324B7C`
- timer owner remains wrapper veneer: `P=0x2A5C6C`, `H=0xE80154`
- first network tick sends `POST /simpleDownload`; host post-processing
  re-arms to `hostTimer=1 mrTimer=1`
- next tick receives 879 bytes and sends `POST /dl_confirm`; host
  post-processing again re-arms
- final tick returns from `verdload.ext` at `lr=0x322B1F` with no socket recv
  line and no re-arm: `hostTimer=0 mrTimer=0`

This points at host timer liveness around private downloader children. The
existing open-socket re-arm only runs when the foreground child has a nonempty
private timer queue; `verdload.ext` can still need another tick for socket or
state-machine progress after its due-node queue drains. Next step: add narrow
diagnostics for the post-`code=2` liveness booleans and socket count, then
generalize the timer re-arm based on proven open socket ownership rather than
package/app names.

## 2026-07-01 10:55 CST

Two generic fixes moved the startup path forward but did not finish the launch:

- Timer liveness: when a private-loader foreground child remains active with
  its own queue drained, wrapper dispatch liveness now keeps the host timer
  armed and routes the next tick through the wrapper. This moved WBRW from the
  `verdload.ext` post-`/dl_confirm` stall into `brwgraphics.ext`.
- Socket EOF: `my_recv()` now treats `select()` readable plus `recv()==0` as
  TCP FIN, closes the Mythroad socket, and returns `MR_FAILED`. This prevents
  the browser/update state machine from spinning forever on a closed socket
  while still preserving `0` for temporary no-data polling.

Latest no-xvfb check:

```sh
timeout 75 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 SKYENGINE_PPM_PATH=/tmp/wbrw-eof-fix.ppm \
  ./build/skyengine build/mythroad/wbrw.mrp >/tmp/wbrw-eof-fix.log 2>&1
```

Result: still timed out and the PPM remained the green startup screen:

- `/page2` socket `s:2` read 2048 bytes, then 1885 bytes, including
  `[b.runapp]`, so the page payload is no longer stuck behind socket EOF.
- The runtime wrote/updated `build/mythroad/brw/lpg/mphome.sky.dt` and `.ut`.
- `/sta` sockets now close on FIN, but the app opens new `/sta` requests
  repeatedly (`s:4`, `s:5`, `s:6`, ...), each reading the 146-byte HTTP response
  then EOF.
- `/tmp/wbrw-eof-fix.ppm` is still 240x320 with dominant color `(48,176,32)`
  across about 88% of pixels.

The remaining root cause is therefore after page payload receipt. Current
branches to distinguish:

- Timer ownership may still be pinned to wrapper code `P=0x2A5C6C/H=0xE80154`
  by a stale `wrapper_rw+4` liveness read, starving active `brwgraphics.ext`
  timer/helper work.
- The `/sta` response may be a normal stats heartbeat or may be rejected by the
  parser. Do not change it without protocol or disassembly evidence.

## 2026-07-01 11:28 CST

Disassembled the WBRW `cfunction.ext` compact timer path and narrowed the
remaining loop:

- The wrapper timer walker at offset `0x45BC` loads scheduler literal `0x2A8`,
  so the live queue is `wrapper_rw+0x2A8`, not the older `+0x3C8/+0x3D8`
  layout and not the stale `wrapper_rw+4` broad state word.
- The walker consumes `[R9+0x2A8+0x0C]` as queued head and
  `[R9+0x2A8+0x10]` as due/current chain. Due nodes call `[node+0x0C]` with
  `R0=[node+0x10]`.
- The recurring callback `0xE83BD1` disassembles at offset `0x3BD0` as:
  load `r3=data`, load `r0=[data+0x1C]`, load `r4=[data+8]`, set `r1=2`,
  and `blx r4`. In other words, wrapper compact timer nodes are generic
  extChunk dispatch nodes: node data decides the real module being called.
- Current node diagnostics confirm this shape. The live nodes use callback
  `0xE83BD1` with data values `0x38F0B8` (primary/game extChunk),
  `0x39D9C8` (verdload extChunk), and `0x392264` (frame extChunk); after
  `brwgraphics.ext` loads, the queue mostly repeats the primary/game node.
- `arm_ext_primary_helper()` still returns `0` while
  `wrapper_timer_dispatch=0xE845BD` because a foreground child is active and
  non-modal. Therefore `mr_timer()` keeps entering normal wrapper helper
  `code=2`; each callback re-registers table[31] from LR `0xE8459D`, keeping
  `timerP=wrapper`, and no `brwgraphics.ext` present appears.

Next code change: allow only the disassembly-proven compact wrapper walker path
to use `arm_ext_call_dispatch()` even when a foreground child is active, and
ensure compact dispatch repairs `extChunk[8]` helper slots for every nested
module because the queue node data may target any extChunk.

## 2026-07-01 11:18 CST

Rebuilt current tree and reran the no-xvfb baseline:

```sh
cmake --build build --target skyengine -j 4
timeout 90 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 \
  SKYENGINE_PPM_PATH=/tmp/wbrw-current-run.ppm \
  ./build/skyengine build/mythroad/wbrw.mrp \
  >/tmp/wbrw-current-run.log 2>&1
```

Result: still timed out (`status=124`). PPM is still the startup screen:
240x320, 127 colors, dominant `(48,176,32)` across 88.12% of pixels.
The tail keeps opening `/sta` sockets, reading the 146-byte HTTP response,
then receiving FIN.

Narrow timer diagnostics:

```sh
timeout 45 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 \
  VMRP_ARM_EXT_DIAG=1 VMRP_ARM_EXT_TIMER_LIVENESS_DIAG=1 \
  ./build/skyengine build/mythroad/wbrw.mrp \
  >/tmp/wbrw-timerdiag.log 2>&1
```

Representative loop:

- `activeP=0x3A0A00 activeH=0x4001A761 activeRW=0x31CA1C`
- `primaryP=0x38F114 primaryH=0x309719`
- `timerP=0x2A5C6C timerH=0xE80154`
- `wrapper_compact_nodes off=0x2A8 queued=0x38F0F4 current=0`
- queued node: `marker=0x79ABBCCF cb=0xE83BD1 data=0x38F0B8`
- `timer_liveness suspended=0 fgChildPost=1 childTimerPost=0 wrapperTimer=1`
- `primary_helper ret=0x0 ... wrapperTimer=0xE845BD`

This confirms the remaining stall is exactly the gate described above:
there is a real compact wrapper queue, but `arm_ext_primary_helper()` returns
0 because a non-modal foreground child is active. The next edit should let the
disassembly-proven compact wrapper scheduler enter `arm_ext_call_dispatch()`
in this state; old non-compact direct-dispatch wrappers should keep the
existing foreground-child guard.

## 2026-07-01 11:38 CST

Added narrow `VMRP_ARM_EXT_DIAG` logging for file descriptors, sockets,
`mr_plat`, and `mr_platEx`. The current no-xvfb check still times out:

```sh
timeout 60 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 \
  SKYENGINE_PPM_PATH=/tmp/wbrw-platform-diag.ppm \
  VMRP_ARM_EXT_DIAG=1 \
  ./build/skyengine build/mythroad/wbrw.mrp \
  >/tmp/wbrw-platform-diag.log 2>&1
```

Result: `status=124`, log has 9722 lines, and the PPM remains the green
startup screen. The dominant color is still `(48,176,32)`, covering 87.77% of
the 240x320 frame.

The platform capability calls suspected earlier are now proven not to be the
current blocker:

- `game.ext` calls `mr_plat(1016)` at LR `0x3096EB`; return is `MR_SUCCESS`.
- `game.ext` calls `mr_plat(1100)` at LR `0x3096EB`; return is `MR_SUCCESS`.
- `game.ext` calls `mr_platEx(4033)` at LR `0x309711`; return is `MR_SUCCESS`.
- `game.ext` also calls `mr_platEx(1017)` and receives the expected 4-byte
  signal structure.

The `/page2` packet is accepted past the initial network layer:

- `POST /page2` is sent from `game.ext` (`ownerP=0x38F114`).
- socket 2 receives the HTTP 200 response and the first payload chunk.
- `brw/lpg/mphome.sky.dt` is written in two chunks, total 3774 bytes.
- `brw/lpg/mphome.sky.ut` is written with the 4-byte timestamp/hash prefix and
  the 16-byte metadata block.
- `mr_getLen("brw/lpg/mphome.sky.dt")` returns `0xEBE`.
- the full `.dt` is immediately reread into ARM memory.

This rules out "the page packet is not downloaded" and "platform init fails"
as the primary explanations for the remaining visible stall. The current tail
shows the compact wrapper dispatch path is active:

- `primary_helper ret=0x309719`
- `call_dispatch nestedP=0x38F114 chunk=0x38F0B8 func=0xE845BD`
- repeated wrapper-owned `table[31]` calls from LR `0xE8459D`

The next target is the primary `game.ext` state machine after the `.dt` reread:
identify what should schedule the renderer or the first `brwgraphics.ext`
present, and why the observed loop only continues `/sta` heartbeats and wrapper
timers.

## 2026-07-01 12:35 CST

Resolved the startup stall without app/path-specific branches.

Final root cause: the WBRW wrapper `cfunction.ext` has a generic parser ABI
around `_mr_readFile` that passes a stack-adjacent pair shaped as
`{fileLen, dataPtr}`. The host bridge already returned the decoded bytes in
`r0`, but this wrapper path only uses `r0` as a success flag and later
materializes the file from the adjacent `dataPtr`. Because the bridge only
wrote `fileLen`, the adjacent `dataPtr` still held stale wrapper/package bytes.

Concrete evidence:

- Before the fix, `game.ext` read the stale active file into wrapper storage:
  `table44 name='brw/lpg/mphome.sky' want=3774 dst=0x31D0EC`, preview
  `00 00 00 00 C0 0E ... start.mr`.
- `_mr_readFile("abc")` then decoded the correct page:
  `table125_ret ret=0x3A09F8 fl=3774 lenSlot=0xE7FC64/3774`, preview
  `02 02 01 00 00 02 ... http://www.175176.com/wtwap/dh.html`.
- A targeted length-slot dump proved the wrapper stack pair:
  at `lenSlot+0` was `0x00000EBE`, and at `lenSlot+4` was stale source
  `0x0031D0E8`.
- Immediately after, the wrapper materialize helper wrote from that stale
  source:
  `table43 name='brw/lpg\mphome.sky' src=0x31D0E8 len=3774`, preview
  `00 00 00 00 C0 0E ... start.mr`.
- Disassembly matches the dataflow. The write helper at
  `0xE82B40..0xE82C4E` calls parser wrapper `0xE82AC4`, receives the source
  pointer in `r7` and length in `[sp]`, then calls `table[43]` at
  `0xE82C18..0xE82C24`. The parser path around `0xE8049C..0xE804A4` and
  `0xE80958..0xE80962` writes pointer/length through caller-provided slots;
  the table[125] bridge must mirror the decoded bytes into the caller-owned
  pointer slot when this stack-pair form is present.

Fix in `src/arm_ext_executor.c`:

- Added `arm_ext_mirror_read_file_to_adjacent_slot()`.
- On successful table[125], after allocating/copying the normal returned ARM
  buffer and writing `*fileLen`, the bridge checks for the generic wrapper ABI:
  active `R9` is the wrapper RW base, `lenSlot` contains the decoded length,
  `lenSlot+4` points to a mapped ARM buffer distinct from the returned
  allocation, and `lenSlot+8` contains the disassembled parser return marker
  for the wrapper call site. That marker keeps ordinary `mr_readFile(&len)`
  callers out of this private `{fileLen,dataPtr}` ABI path.
- When that shape is present, the bridge copies the immutable decoded
  `last_file_copy` bytes into the adjacent caller-owned buffer. Callers that use
  the normal `r0` return still work unchanged; wrapper callers that use the
  adjacent pointer now see the decoded payload.

Verification:

```sh
cmake --build build --target skyengine -j 4

timeout 12 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 SKYENGINE_PPM_PATH=/tmp/wbrw-mirrorfix.ppm \
  VMRP_ARM_EXT_DIAG=1 ./build/skyengine build/mythroad/wbrw.mrp \
  >/tmp/wbrw-mirrorfix.log 2>&1
```

The diagnostic run proved the fixed transition:

- `table125_ret ... fl=3774 ... mirror=1 mirrorSlot=0x31D0E8`
- the following `table43 name='brw/lpg\mphome.sky' src=0x31D0E8 len=3774`
  preview starts with the decoded page bytes:
  `02 02 01 00 00 02 ... http://www.175176.com/wtwap/dh.html`.
- `build/mythroad/brw/lpg/mphome.sky` and `.dt` are byte-identical.

Final no-diagnostic check:

```sh
timeout 30 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  VMRP_NO_MOUSE=1 VMRP_PPM=1 SKYENGINE_PPM_PATH=/tmp/wbrw-final.ppm \
  ./build/skyengine build/mythroad/wbrw.mrp >/tmp/wbrw-final.log 2>&1
```

The timeout status is expected because the process was capped. The captured PPM
is no longer the green startup screen: dominant color is `(248,252,248)` over
47,695 pixels with 122 unique colors, and `mphome.sky` still equals
`mphome.sky.dt`. This confirms WBRW launches past the green `正在启动` screen.
