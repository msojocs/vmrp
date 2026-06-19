# VMRP Emulator - Testing Infrastructure & Patterns Report

## Executive Summary
The vmrp emulator project has a **multi-layered testing infrastructure** combining:
- **Unit tests** (C-based, cmake/ctest integration)
- **PPM image-based testing** (for visual regression & headless verification)
- **Manual automation scripts** (shell-based test scenarios via VMRP_AUTO_CLICKS)
- **Environment variable-based testing controls** (VMRP_PPM, VMRP_AUTO_CLICKS)

---

## 1. UNIT TEST FRAMEWORK

### Location
- **Test sources:** `test/test_*.c` (4 test modules)
- **Test runner:** `test/test_main.c`
- **Build output:** `build/test_vmrp`
- **Build integration:** CMakeLists.txt (lines 311-335)

### Test Modules

#### 1.1 Parser Tests (`test/test_parser.c`)
- **Purpose:** Verify MRP Lua parser can compile source code at runtime
- **Background:** Previous build used `mr_noparser.c` which broke server-side code updates
- **Test cases:**
  - `test_parser_compile_simple()` - Basic assignment compilation
  - `test_parser_compile_def_keyword()` - MRP extension "def" keyword (alias for "function")
  - `test_parser_compile_server_payload()` - applist-server.js format code frames
  - `test_parser_syntax_error()` - Detects invalid syntax (non-zero return)

#### 1.2 GOT Snapshot Tests (`test/test_got_snapshot.c`)
- **Purpose:** Validate GOT (Global Offset Table) snapshot preservation logic for nested plugins
- **Background:** Plugins like netpay overwrite bridge GOT pointers; need recovery after dump0 restore
- **Test cases:**
  - `test_got_bridge_write_records_snapshot()` - Bridge values trigger snapshot recording
  - `test_got_nonbridge_preserves_snapshot()` - Non-bridge writes preserve existing snapshot
  - `test_got_different_r9_no_hijack()` - Different R9 regions don't steal base pointer
  - `test_got_fixup_restores_after_memcpy()` - GOT recovery after memory copy

#### 1.3 DNS Mapping Tests (`test/test_dns_map.c`)
- **Purpose:** Verify domain name remapping for network requests
- **Test cases:**
  - `test_dns_map_arrow()` - Arrow separator format: "Original.Example. -> 127.0.0.1"
  - `test_dns_map_equals_and_multiple_entries()` - Equals/semicolon format with multiple domains
  - `test_dns_map_rejects_invalid_entry()` - Invalid maps fail correctly

### Test Framework Pattern
- **Style:** Manual assertion macros (no external framework)
- **Assertions:**
  ```c
  #define ASSERT_INT(desc, actual, expected) do { \
      if ((actual) != (expected)) { \
          printf("  FAIL: %s: got %d, expected %d\n", desc, (actual), (expected)); \
          failures++; \
      } \
  } while(0)
  ```
- **Test organization:**
  - Individual test functions (non-void, return failure count)
  - Aggregator function collects results from all subtests
  - Test runner (`test_main.c`) calls aggregators and reports total failures
  - Exit code: 0 if all pass, 1 if any fail

### Stubbing Mechanism (`test/stubs.c`)
- **Purpose:** Provide minimal implementations for external dependencies
- **Covered stubs:**
  - ARM extension executor (all 9 functions stubbed as -1 returns)
  - DSM platform layer (log, timers, file I/O, network callbacks)
  - Graphics, dialog, socket operations (all return -1 or NULL)
  - **Total:** ~100+ stub functions

### Build Configuration
- **Flag:** `VMRP_BUILD_TESTS` (enabled by default on non-MSVC, disabled on Windows)
- **CMake integration:** `enable_testing()` + `add_test(NAME vmrp_unit_tests COMMAND test_vmrp)`
- **Runtime:** Tests can be executed with `ctest` or direct `./build/test_vmrp`

---

## 2. PPM IMAGE-BASED TESTING

### Purpose
Visual regression testing and headless environment verification (no display server required).

### TypeScript E2E Pixel Tests

The stable CI path uses an in-process Unix socket instead of desktop tools such
as `xdotool`/`import`. Set `VMRP_E2E_SOCKET` to make `build/vmrp` listen for
stepwise test commands; each command is marshalled back onto SDL's main thread
before it touches input handling or screenshot capture.

Supported commands:

```text
CLICK x y
KEY name
SCREEN path
DRAW_COUNT
WAIT_DRAW previous timeout_ms
QUIT
```

Run the Vitest example directly:

```bash
npm ci
cmake --build build --target vmrp
npm run test:e2e
```

The TypeScript harness defaults to `SDL_VIDEODRIVER=dummy` and
`SDL_AUDIODRIVER=dummy`, so this path does not require `xvfb` or a real desktop
session.

Or through CTest, which injects the actual built executable path via
`VMRP_BIN`:

```bash
ctest --test-dir build --output-on-failure -R e2e_vitest
```

Example test code lives in `test/e2e/gxdzc-pixel.test.ts`; it sends a click,
waits for a new draw, takes a PPM screenshot, and checks exact RGB pixels.

### Implementation Details

#### 2.1 PPM Dump Function (`src/main.c`, line 109-133)
```c
static void dump_screen_ppm(const char *path) {
    // Converts SDL surface (RGB32) to PPM format
    // Writes to /tmp/vmrp_screen.ppm by default
    // Called during game execution
}
```

#### 2.2 Automatic Snapshot Points
- **Default:** 5th guiDrawBitmap() call (initial UI render)
- **Periodic:** Every 30th draw when `VMRP_PPM` env var set (for continuous monitoring)
- **On-demand:** SIGUSR1 signal handler (after SDL_Init, line 96-101)

#### 2.3 Environment Variables

| Variable | Purpose | Format | Example |
|----------|---------|--------|---------|
| `VMRP_PPM` | Enable periodic PPM dumps | boolean | `export VMRP_PPM=1` |
| `VMRP_AUTO_CLICKS` | Inject automated clicks | `x,y[,delay_ms];x,y[,delay_ms]` | `"0,0,500;228,309,500"` |
| `VMRP_AUTO_CLICK_DELAY_MS` | Default delay between clicks | milliseconds | `export VMRP_AUTO_CLICK_DELAY_MS=800` |

### Headless Testing Pattern
The combination of these variables enables unattended verification:
1. Set `VMRP_AUTO_CLICKS` to replay test scenario
2. Enable `VMRP_PPM` for continuous output to `/tmp/vmrp_screen.ppm`
3. Run: `build/vmrp mythroad/game.mrp`
4. Compare PPM files or parse game state from logs

---

## 3. MANUAL AUTOMATION SCRIPTS

### Test Scenario Directories
```
test/dsm_gm/          - DSM system tests (1 script)
test/gghjt/           - Game (gghjt) payment flow tests (3 scripts)
test/gxdzc/           - Game (gxdzc) payment/download tests (8 scripts)
test/mpc/             - Game (mpc) tests (1 script)
test/mssj/            - Game (mssj) tests (1 script)
```

### Example Test Script (`test/gxdzc/pay.sh`)
```bash
#!/bin/bash
# up和down要间隔500ms；每个事件都显式带 delay，便于复现付费超时路径。
export VMRP_AUTO_CLICKS="0,0,500;228,309,500;158,293,1000;133,295,3000"
build/vmrp mythroad/gxdzc.mrp
```

### Test Scenarios Documented

#### gxdzc/ (Download+Payment Game)
- `pay.sh` - Payment flow with 500ms click intervals
- `pay-cancel.sh` - Cancel payment dialog
- `audio-enable.sh` - Audio system test
- `download-resource-*.sh` (5 variants) - Resource download with back/exit/timeout scenarios
- `exit.sh` - Clean exit

#### gghjt/ (Payment Game)
- `pay-normal-enter.sh` - Enter payment with default delay
- `pay-normal-back.sh` - Back button during payment
- `pay-timeout-back.sh` - Timeout path with back button

#### Other Games
- `mpc/exit.sh`, `mssj/dsm_gm-boot-and-exit.sh`, `dsm_gm/update-applist.sh`

### Automation Thread Implementation (`src/main.c`, lines 355-405)
- Parses `VMRP_AUTO_CLICKS` format
- Spawns background thread: `SDL_CreateThread(autoClickThread, ...)`
- Thread sleeps between clicks, injects synthetic events
- Supports optional per-click delays (default: 800ms via `VMRP_AUTO_CLICK_DELAY_MS`)

---

## 4. TESTING DATA & EXAMPLES

### MRP Files (Mythroad Games)
- **Production:** `mythroad/` directory (symlink to `wasm/dist/fs/mythroad/`)
- **Available test games:**
  - `dsm_gm.mrp` (41K) - DSM system game
  - `gghjt.mrp` (366K) - Payment game
  - `gxdzc.mrp` (438K) - Download+payment game
  - `dota.mrp` (295K), `mpc.mrp` (142K), `mssj.mrp` (245K) - Other titles
  - `nes.mrp` (52K) - NES emulator
  - `mrpinfo.mrp` (42K) - Info display game
  - `xmwq.mrp`, `ydqtwo.mrp` - Additional games

### Plugin Examples
- `mythroad/plugins/netpay.mrp` - Payment plugin (tests GOT snapshot logic)
- `mythroad/plugins/flaengine.mrp` - Graphics engine
- `mythroad/plugins/ose/brwcore.mrp` - Browser core plugin

### Test Logs
- **Location:** `temp/test_*.log` (debug output from previous runs)
- **Format:** Printf-style output from emulator
- **Sample contents:**
  - Memory initialization (`CODE_ADDRESS`, `STACK_ADDRESS`, `MEMORY_MANAGER_ADDRESS`)
  - File operations (`mr_open`, `mr_close`)
  - Decompression logs (`mr_unzip`)
  - Memory profiling (`MEM-I: total, iTop, ctrl`)
  - Platform extension calls

---

## 5. GREP PATTERNS FOUND

### No Traditional Assert/Check Macros
- **Not found:** `CHECK(x)` patterns in vmrp codebase (only in Unicorn samples)
- **Not found:** `EXPECT_*` (Google Test style)
- **Not found:** `assert()` macros (except in Unicorn third-party code)

### Manual Testing Instrumentation
- `[TEST]` prefix for test output
- `[INFO]`, `[WARN]` for runtime diagnostics
- `[CLICK]` for click event tracing
- `[AUTO_CLICK]` for automation logging
- `[PPM]` for screenshot operations (commented out, lines 142-143)

---

## 6. BUILD & RUN

### Building Tests
```bash
cmake -DVMRP_BUILD_TESTS=ON -B build
cmake --build build
# or just: cmake --build build --target test_vmrp
```

### Running Tests
```bash
# Via cmake
cd build && ctest

# Direct execution
./build/test_vmrp

# Individual test scenarios
./test/gxdzc/pay.sh
./test/gghjt/pay-normal-enter.sh
```

### Running with PPM Capture
```bash
export VMRP_PPM=1
./build/vmrp mythroad/gxdzc.mrp

# Check output
file /tmp/vmrp_screen.ppm
identify /tmp/vmrp_screen.ppm  # if ImageMagick installed
```

### Running Headless Payment Test
```bash
export VMRP_AUTO_CLICKS="228,309,500"  # Click coordinate with 500ms delay
export VMRP_PPM=1                       # Capture screenshots
./build/vmrp mythroad/gxdzc.mrp

# Later: compare /tmp/vmrp_screen.ppm to reference for regression detection
```

---

## 7. TESTING ARCHITECTURE DIAGRAM

```
┌─────────────────────────────────────────────────────────┐
│                  TESTING INFRASTRUCTURE                  │
└─────────────────────────────────────────────────────────┘

        ┌──────────────────────┐
        │   Unit Tests         │
        │  (test/test_*.c)     │
        ├──────────────────────┤
        │ • Parser (Lua)       │
        │ • GOT Snapshot       │
        │ • DNS Mapping        │
        │ • 4 test modules     │
        └──────────────────────┘
                   │
                   ├──> CMake: add_test()
                   ├──> ctest runner
                   └──> ./build/test_vmrp

        ┌──────────────────────────────┐
        │   PPM Visual Testing         │
        │  (Headless Screenshots)      │
        ├──────────────────────────────┤
        │ • PPM dumping (SDL→PPM)      │
        │ • Periodic snapshots         │
        │ • /tmp/vmrp_screen.ppm       │
        │ • VMRP_PPM env var           │
        └──────────────────────────────┘
                   │
                   ├──> Compare with reference
                   ├──> Detect visual regression
                   └──> Headless CI integration

        ┌──────────────────────────────┐
        │   Automation Scripts         │
        │  (test/*/*.sh)               │
        ├──────────────────────────────┤
        │ • Payment scenarios          │
        │ • Download scenarios         │
        │ • Exit/reset flows           │
        │ • VMRP_AUTO_CLICKS parsing   │
        └──────────────────────────────┘
                   │
                   ├──> Manual testing
                   ├──> CI/CD integration
                   └──> Regression detection
```

---

## 8. KEY FINDINGS

### Strengths
1. ✅ **Multi-layer testing:** Unit + integration + automation
2. ✅ **Headless support:** PPM + no-mouse + auto-clicks for CI
3. ✅ **Well-organized test data:** Multiple MRP games with documented flows
4. ✅ **Environment-based configuration:** Flexible without code changes
5. ✅ **CMake integration:** Standard test discovery via ctest

### Gaps/Limitations
1. ⚠️ **No formal test framework:** Manual assertion macros vs. Google Test/Catch2
2. ⚠️ **Limited test coverage:** Only 3 unit test modules (parser, GOT, DNS)
3. ⚠️ **No baseline PPM images:** No committed reference screenshots for regression
4. ⚠️ **MSVC unsupported:** Tests disabled on Windows (POSIX mmap dependency)
5. ⚠️ **No assertion messages:** Some assertions lack descriptive context
6. ⚠️ **Manual test scripting:** Shell scripts not version-controlled or parameterized

### Opportunities for Enhancement
1. Add more unit tests (e.g., memory allocation, file I/O mocking)
2. Commit reference PPM images for automated visual regression
3. Create Python/shell harness for PPM comparison in CI
4. Parameterize test scenario scripts
5. Add Windows support for unit tests (replace POSIX mmap)

---

## 9. ENVIRONMENT VARIABLES FOR TESTING

### Complete List
| Variable | Type | Default | Purpose |
|----------|------|---------|---------|
| `VMRP_PPM` | bool | unset | Enable periodic PPM dumps (every 30 draws) |
| `VMRP_AUTO_CLICKS` | string | unset | Click sequence: "x1,y1[,ms];x2,y2[,ms];..." |
| `VMRP_AUTO_CLICK_DELAY_MS` | int | 800 | Default pause between auto-clicks (ms) |
| `VMRP_ARM_EXT_TRACE` | bool | unset | Debug: trace ARM extension execution |
| `VMRP_ARM_EXT_TRACE_PC` | bool | unset | Debug: trace program counter (verbose) |
| `VMRP_ARM_EXT_LOAD_CODE` | int | unset | Debug: ARM extension load behavior |
| `VMRP_ARM_EXT_SMOKE` | string | unset | Debug: smoke test configuration |
| `VMRP_MRP` | string | "dsm_gm.mrp" | Override default game file |
| `VMRP_EXT` | string | unset | ARM extension file path |
| `VMRP_ENTRY` | string | unset | Entry point function name |
| `VMRP_SCREEN_WIDTH` | int | 240 | Emulated device width (overridden by --screen) |
| `VMRP_SCREEN_HEIGHT` | int | 320 | Emulated device height (overridden by --screen) |
| `VMRP_DNS_MAP` | string | "wap.skmeg.com->127.0.0.1" | Domain remapping |

---

## Conclusion
The vmrp emulator employs a **pragmatic multi-layer testing strategy**:
- **Unit tests** for critical logic (Lua, GOT, DNS)
- **Visual testing** via PPM screenshots
- **Automation scripts** for end-to-end game scenarios
- **Environment variables** for runtime control

This approach works well for a **single-developer/small-team project** without external test framework overhead. For production CI/CD, consider **adding baseline PPM images** and a **visual comparison harness** to catch regressions automatically.
