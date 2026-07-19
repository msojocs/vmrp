# Cookie E2E key timer-generation boundary

## Scope

This page records a generic E2E input synchronization bug found while diagnosing
`cookie_v6110.mrp`. The cookie scenario test was still under construction and was
left unchanged. The production fix contains no package-name, scene, or pixel
special cases.

## Failure evidence

`VmrpE2e.key()` used the sequence `DRAW_COUNT -> KEY -> WAIT_DRAW`. The old KEY
server pushed SDL KEYDOWN, waited a wall-clock hold, pushed KEYUP, and returned as
soon as the events were queued. A draw count only proves that a bitmap was
submitted; it does not prove that the guest consumed release or settled its input
state machine.

The misleading public failure was:

```text
ERR wait_draw_timeout current=8 target>8
```

Retained PPM files proved that the first soft-left action and its menu draw had
already completed. The failing action was the following key. Narrow diagnostics
showed `LEFT release` immediately followed by the next press without an intervening
guest timer dispatch. The host `isKeyDown` latch was already clear.

## ARM evidence

The MRP EXT chain was `mrc_loader.ext -> logo.ext -> game.ext`. For `game.ext`:

```text
base          0x226110
length        0x40B24
P             0x2BC4EC
Thumb helper  0x25D189
file offset   0x37078
```

The event path was:

```text
helper code=1 @ 0x25D1CC
-> 0x25BFAC -> 0x25C0D4
PRESS:   0x25C148 -> 0x244498
RELEASE: 0x25C198 -> 0x244498
-> 0x2455CC widget dispatch
```

Soft-left PRESS/RELEASE became UI events `0x3004/0x3005`. Both ARM callbacks
returned normally, with no invalid memory access, Unicorn error, block, or exit.
The bug was therefore in host-side completion semantics, not ARM event delivery.

## Invalid boundaries

- `SDL_PushEvent()` success only proves queue insertion.
- KEYUP callback return proves release delivery, not later guest scheduler work.
- `draw_count` proves a bitmap submission and can belong to unrelated work.
- An arbitrary timer epoch can consume an event queued before the key.
- A fixed 50/60/250/500 ms delay is load-dependent and can turn direction keys
  into repeats or long-press actions.

## Implemented boundary

Every `timerStart()` allocates a monotonically increasing generation. The SDL
timer callback carries that generation in its event. The main thread publishes a
generation as dispatched only after the complete guest `timer()` call returns.
Pending generation and dispatch-in-progress state distinguish a genuine timer
stop from a temporary stop/start inside one guest callback.

For an unconfigured short key, KEYDOWN enters through the SDL queue. At the end
of the first following complete guest timer tick, the main thread synchronously
executes KEYUP before processing another SDL event. This makes the key visible
for exactly one guest tick without depending on control-thread wakeup latency.
After release, the control thread waits for a fully dispatched generation that is
strictly newer than the arm generation captured at the release acknowledgement.

Explicit `holdMs` remains a physical long press. Runtime exit is a first-class
terminal result (`OK key <code> exited`); only that explicit result suppresses a
subsequent draw wait.

## Verification and constraints

- All emulator diagnostics used `SDL_VIDEODRIVER=dummy`; no `xvfb` was used.
- PPM evidence confirms the first soft-left menu frame and expected highlight.
- `cmake --build build --target skyengine -j2` passed.
- `ctest --test-dir build --output-on-failure` passed 1/1.
- `pnpm exec tsc --noEmit` passed.
- `test/e2e/cookie/run-mrp.test.ts` has no retained diff because its scenario
  expectations were explicitly outside this fix.

## Reusable rule

When a guest application polls input from a timer-driven state machine, define
E2E key completion with scheduler-owned identities and main-thread completion
publication. Do not infer completion from queue insertion, release delivery,
unrelated rendering, an anonymous timer tick, or wall-clock sleep.
