# VMRP Testing

## Test Runner

VMRP now keeps the automated test entrypoint in Vitest only. CMake only builds
emulator/library targets; build `skyengine`, then run the TypeScript e2e tests with
pnpm.

```bash
cmake --build build --target skyengine
pnpm test:e2e
```

To run a focused scenario:

```bash
pnpm vitest run test/e2e/opbzqe/game-prepare.test.ts
```

## E2E Harness

The Vitest harness starts `build/skyengine` with `SKYENGINE_E2E_SOCKET` and talks to the
emulator through a local socket.  Commands are marshalled onto SDL's main
thread before they touch input handling or screenshot capture.

Supported commands:

```text
CLICK x y
KEY name
SCREEN path
DRAW_COUNT
WAIT_DRAW previous timeout_ms
QUIT
```

The harness sets `SDL_AUDIODRIVER=dummy` by default and does not require
`xvfb`.

Useful environment overrides:

| Variable | Purpose |
| --- | --- |
| `VMRP_BIN` | Path to the skyengine executable, defaults to `build/skyengine`. |
| `SKYENGINE_WORK_DIR` | Runtime working directory, defaults to repository root. |
| `VMRP_TIMEOUT_MS` | Socket startup and command timeout. |

## Visual Assertions

Screenshots are dumped as PPM files by the e2e socket `SCREEN` command and read
directly in TypeScript.  Tests assert exact RGB pixels for stable UI states and
menu transitions.

Current e2e examples live under `test/e2e/`, including:

- `test/e2e/opbzqe/game-prepare.test.ts`
- `test/e2e/dota/download-plugin.test.ts`
- `test/e2e/gxdzc-pixel.test.ts`
- `test/e2e/gghjt/*.test.ts`
