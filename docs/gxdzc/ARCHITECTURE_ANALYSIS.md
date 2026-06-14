# VMRP Architecture Analysis

## 1. VMRP_AUTO_CLICKS: Complete Auto-Click Flow

### Overview
Auto-clicks are injected through the `VMRP_AUTO_CLICKS` environment variable. The clicks bypass user interaction and are injected directly into the SDL event queue as real input events, allowing automated testing of UI flows.

### Format
```
VMRP_AUTO_CLICKS="x1,y1[,delay1];x2,y2[,delay2];..."
```

- Each entry is a coordinate pair separated by semicolons
- Optional third parameter specifies custom delay in milliseconds
- Special case: `x < 0` denotes key presses instead of clicks:
  - `-1`: ESC key
  - `-2`: SOFTRIGHT (否/No)
  - `-3`: SOFTLEFT (是/Yes)  
  - `-4`: SELECT (确认/Confirm)

### Timing Control
- `VMRP_AUTO_CLICK_DELAY_MS`: Global delay between clicks (default: 800ms)
- Individual clicks can override with third parameter
- Initial wait time: one default delay interval (allows app startup)

### Complete Injection Flow

**File: src/main.c, lines 297-405**

```
startAutoClicksIfRequested() [line 376]
  ├─ Parse VMRP_AUTO_CLICKS env var (line 377)
  ├─ Build array of AutoClickPoint structs (lines 380-400)
  │  └─ Each entry: {int x, int y, int delay_ms}
  └─ Create dedicated thread: SDL_CreateThread(autoClickThread) (line 403)

autoClickThread(void *data) [line 313]
  ├─ Read VMRP_AUTO_CLICK_DELAY_MS env var, default 800ms (line 315)
  ├─ Initial SDL_Delay(default_delay) - let app startup (line 320)
  └─ For each click in autoClickList:
     ├─ Determine delay_ms for this click (line 324)
     │  └─ Use custom if >= 0, else global default
     └─ Two cases:
        
        A. Key Press (x < 0): [lines 328-348]
           ├─ Create SDL_KEYDOWN event (lines 334-338)
           │  └─ Set: type=SDL_KEYDOWN, state=SDL_PRESSED
           ├─ SDL_PushEvent(&ev) → main thread queue (line 339)
           ├─ SDL_Delay(50ms) - hold key down (line 340)
           ├─ Create SDL_KEYUP event (lines 341-345)
           │  └─ Set: type=SDL_KEYUP, state=SDL_RELEASED
           └─ SDL_PushEvent(&ev) → main thread queue (line 346)
           └─ SDL_Delay(cur_delay) (line 347)
        
        B. Mouse Click (x >= 0): [lines 350-371]
           ├─ Create SDL_MOUSEBUTTONDOWN event (lines 350-358)
           │  └─ Set: type=SDL_MOUSEBUTTONDOWN, button=SDL_BUTTON_LEFT
           │     state=SDL_PRESSED, x, y coordinates
           ├─ SDL_PushEvent(&ev) → main thread queue (line 358)
           ├─ SDL_Delay(500ms) - click duration (line 360)
           ├─ Create SDL_MOUSEBUTTONUP event (lines 362-368)
           │  └─ Set: type=SDL_MOUSEBUTTONUP, button=SDL_BUTTON_LEFT
           │     state=SDL_RELEASED, x, y coordinates
           ├─ SDL_PushEvent(&ev) → main thread queue (line 369)
           └─ SDL_Delay(cur_delay) - wait before next click (line 371)
```

### Main Event Loop Processing

**File: src/main.c, loop() function [lines 407-498]**

```
loop() main event loop:
  ├─ Call startAutoClicksIfRequested() [line 411]
  │  └─ Spawns auto-click thread if VMRP_AUTO_CLICKS set
  └─ while (isLoop):
     └─ while (SDL_WaitEvent(&ev)):  [line 421]
        └─ if (ev.type == SDL_MOUSEBUTTONDOWN) [line 474]
           ├─ Log click: "[CLICK] #%u down x=%d y=%d"
           ├─ Call: event(MR_MOUSE_DOWN, ev.button.x, ev.button.y)
           │  └─ Routes to emulated code via vmrp_runtime_event()
           └─ Log return: "[CLICK] #%u down ret=%d"
        
        └─ if (ev.type == SDL_MOUSEBUTTONUP) [line 486]
           ├─ Log: "[CLICK] #%u up x=%d y=%d"
           ├─ Call: event(MR_MOUSE_UP, ev.button.x, ev.button.y)
           └─ Log return: "[CLICK] #%u up ret=%d"
        
        └─ if (ev.type == SDL_KEYDOWN) [line 457]
           └─ if (isKeyDown == SDLK_UNKNOWN)
              └─ Call: keyEvent(MR_KEY_PRESS, keysym) [line 460]
                 └─ Converts SDL keycode to MythRoad key code
        
        └─ if (ev.type == SDL_KEYUP) [line 463]
           └─ Call: keyEvent(MR_KEY_RELEASE, keysym) [line 466]
```

### Event Dispatch to Emulated Code

**File: src/vmrp.c, event() function [lines 347-349]**
```c
int32_t event(int32_t code, int32_t p1, int32_t p2) {
    return vmrp_runtime_event(&runtime, code, p1, p2);
}
```

This routes all events (from auto-clicks, real input, or timers) to the ARM emulator runtime, which executes the emulated code's event handlers.

---

## 2. PPM Screen Dumps

### Mechanism
Screenshots are saved as PPM (Portable PixMap) format to `/tmp/vmrp_screen.ppm` for verification in headless environments.

### Triggering PPM Dumps

**Three ways to capture screenshots:**

1. **Automatic on 5th frame** (lines 141-143):
   ```c
   if (guiDrawBitmapCount == 5 || 
       (guiDrawBitmapCount % 30 == 0 && getenv("VMRP_PPM"))) {
       dump_screen_ppm("/tmp/vmrp_screen.ppm");
   }
   ```
   - Always dumps on the 5th guiDrawBitmap() call
   - If VMRP_PPM env var is set, also dumps every 30 frames

2. **SIGUSR1 Signal Handler** (lines 70-73):
   ```c
   static void sigusr1_handler(int sig) {
       dump_screen_ppm("/tmp/vmrp_screen.ppm");
   }
   ```
   - Installed at startup (line 539) 
   - Re-installed after SDL_Init (line 563) since SDL resets signal handlers
   - Send `kill -USR1 <pid>` from another terminal to capture

3. **guiDrawBitmap() Flow** (lines 136-164):
   ```
   guiDrawBitmap(bmp, x, y, w, h)
     ├─ Increment guiDrawBitmapCount (line 137)
     ├─ Check if should dump (lines 141-143)
     ├─ Update SDL surface with new pixels (lines 145-161)
     └─ Call SDL_UpdateWindowSurface() (line 162)
   ```

### PPM Format

**File: src/main.c, dump_screen_ppm() [lines 50-68]**

```
1. Header:
   "P6\n<width> <height>\n255\n"
   
2. Pixel Data:
   For each pixel (left-to-right, top-to-bottom):
     ├─ Extract 16-bit RGB565 pixel from SDL surface
     ├─ Decode to separate R, G, B bytes using SDL_GetRGB()
     └─ Write 3 bytes per pixel (R, G, B in raw binary)

3. File Location:
   /tmp/vmrp_screen.ppm
```

**PPM is a text/binary format:**
- P6 = "raw" (binary) PPM format
- Simple format readable by ImageMagick, GIMP, etc.
- Can convert: `convert /tmp/vmrp_screen.ppm output.png`

### Environment Variable Control

- **VMRP_PPM**: If set, enable continuous PPM dumps every 30 frames
  - Without this: only dumps on frame 5
  - With this: frame 5 + every 30 frames after
- **Purpose**: Keep `/tmp/vmrp_screen.ppm` always showing current screen state
- **Tradeoff**: Small disk I/O overhead vs. real-time visibility in headless testing

---

## 3. SDL Main Loop Architecture

### Initialization Phase

**File: src/main.c, main() [lines 531-587]**

```
main(argc, args):
  ├─ Set up signal handlers for SIGPIPE, SIGUSR1, SIGABRT (lines 537-539)
  ├─ Parse command-line arguments via prepareVmrpArgs() (line 545)
  ├─ SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) (line 555)
  │  └─ Initialize SDL video and timer subsystems
  ├─ Register custom event type: timerEventType = SDL_RegisterEvents(1) (line 559)
  │  └─ Allocate unique event ID for internal timer events
  ├─ Re-install SIGUSR1 handler (line 563) - SDL_Init resets signals
  ├─ SDL_CreateWindow(..., screen_width, screen_height, ...) (line 566)
  │  └─ Create SDL window matching VMRP screen resolution
  ├─ startVmrp(argc, args) (line 573)
  │  └─ Initialize ARM Unicorn emulator & load MRP file
  └─ Enter main loop: loop() (line 582)
```

### Event Loop Design

**File: src/main.c, loop() [lines 407-498]**

```
loop():
  ├─ startAutoClicksIfRequested() (line 411)
  │  └─ Spawn auto-click thread if VMRP_AUTO_CLICKS env set
  ├─ while (isLoop):
  │  └─ while (SDL_WaitEvent(&ev)) [BLOCKING - waits for events]
  │     ├─ Process special edit mode (lines 429-451)
  │     │  └─ If isEditMode, handle ctrl+v (paste) and ctrl+z (cancel)
  │     └─ Normal event processing (lines 453-495):
  │        ├─ if (ev.type == timerEventType)
  │        │  └─ Call timer() - internal Mythroad timer callback
  │        ├─ if (ev.type == SDL_KEYDOWN)
  │        │  └─ Convert SDL keycode → Mythroad key code
  │        │     └─ Call event(MR_KEY_PRESS, key, 0)
  │        ├─ if (ev.type == SDL_KEYUP)
  │        │  └─ Call event(MR_KEY_RELEASE, key, 0)
  │        ├─ if (ev.type == SDL_MOUSEMOTION)
  │        │  └─ If isMouseDown, call event(MR_MOUSE_MOVE, x, y)
  │        ├─ if (ev.type == SDL_MOUSEBUTTONDOWN)
  │        │  └─ Call event(MR_MOUSE_DOWN, x, y)
  │        └─ if (ev.type == SDL_MOUSEBUTTONUP)
  │           └─ Call event(MR_MOUSE_UP, x, y)
  └─ Cleanup: SDL_DestroyWindow(), SDL_Quit()
```

### Event Processing Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│  Input Sources                                              │
├─────────────────────────────────────────────────────────────┤
│ 1. Auto-Click Thread  │ 2. Real Input (X11)  │ 3. SDL Timer │
│    (pushes events)    │    (SDL captures)    │    (callback) │
└──────┬────────────────┴──────┬───────────────┴───────┬──────┘
       │                       │                       │
       └───────────────────────┼───────────────────────┘
                               │
                        ┌──────▼──────────────┐
                        │ SDL Event Queue     │
                        │ (FIFO ordered)      │
                        └──────┬──────────────┘
                               │
                        ┌──────▼──────────────┐
                        │ loop()               │
                        │ SDL_WaitEvent(&ev)  │
                        │ [BLOCKING]          │
                        └──────┬──────────────┘
                               │
                    ┌──────────┼──────────────┐
                    │          │              │
            ┌───────▼────┐ ┌──▼──────────┐ ┌─▼──────┐
            │ Key Events │ │Mouse Events │ │Timers  │
            └───────┬────┘ └──┬──────────┘ └─┬──────┘
                    │         │              │
                    └────┬────┴────┬─────────┘
                         │        │
                    ┌────▼────────▼────┐
                    │ vmrp_runtime_event()
                    │ Routes to ARM     │
                    │ Unicorn engine    │
                    └───────┬──────────┘
                            │
                    ┌───────▼──────────┐
                    │ Emulated Code    │
                    │ Executes event   │
                    │ handler in MRP   │
                    └──────────────────┘
```

### Key State Management

```c
static bool isMouseDown = false;        // Track click state
static SDL_Keycode isKeyDown = SDLK_UNKNOWN;  // Track held key
static uint32_t clickSeq = 0;           // Click sequence counter
```

- **isMouseDown**: Prevents duplicate MOUSEMOTION without prior MOUSEBUTTONDOWN
- **isKeyDown**: Prevents SDL key repeat events from re-triggering
- **clickSeq**: Unique ID per click sequence for logging

### Timer Event Flow

**File: src/main.c, timerCb() [lines 179-188]**

```
Timer Callback (runs in SDL timer thread):
  └─ Create custom SDL event with registered type (timerEventType)
     └─ SDL_PushEvent() → queues to main thread
        └─ Main loop detects custom event
           └─ Calls timer() → vmrp_runtime_timer()
              └─ Executes Mythroad timer callback in emulator
```

**Why not call directly?**
- Avoids race conditions: timer thread vs. main SDL event thread
- Both would access same Unicorn ARM engine → thread-unsafe
- Solution: Serialize via event queue → single-threaded execution in main loop

### Display Update Flow

```
Emulated code calls guiDrawBitmap():
  │
  ├─ Increment guiDrawBitmapCount
  │
  ├─ Check if PPM dump needed:
  │  ├─ Always on frame 5
  │  └─ Every 30 frames if VMRP_PPM set
  │     └─ dump_screen_ppm("/tmp/vmrp_screen.ppm")
  │
  ├─ Lock SDL surface if needed
  │
  ├─ Copy bitmap pixels to SDL surface
  │  └─ Convert RGB565 → SDL format using SDL_MapRGB()
  │
  └─ SDL_UpdateWindowSurface()
     └─ Composites window and displays to X11/Wayland
```

---

## Summary: Complete Auto-Click to Execution Path

```
VMRP_AUTO_CLICKS="50,100;150,200"
VMRP_AUTO_CLICK_DELAY_MS=1000
                │
                ▼
        main() startup
                │
                ▼
        startAutoClicksIfRequested()
                │
        ┌───────┴────────┐
        ▼                │
    Parse env var       │
    Build array:        │
    [                   │
      {50, 100, -1}     │
      {150, 200, -1}    │
    ]                   │
        │               │
        └───────┬───────┘
                │
                ▼
    SDL_CreateThread(autoClickThread)
    [Dedicated auto-click thread]
                │
        ┌───────┴──────────────┐
        ▼                      │
    Sleep 1000ms              │ Main thread
    (app startup)             │ enters loop()
        │                     │
        ├─ Click #1: 50,100   ├─ SDL_WaitEvent()
        │  ├─ SDL_MOUSEBUTTONDOWN  │
        │  ├─ SDL_PushEvent()      │
        │  └─ Sleep 500ms          │
        │  ├─ SDL_MOUSEBUTTONUP    │
        │  ├─ SDL_PushEvent()      │
        │  └─ Sleep 1000ms         │
        │                     │
        ├─ Click #2: 150,200 │ Processes queued events:
        │  ├─ SDL_MOUSEBUTTONDOWN  │
        │  ├─ SDL_PushEvent()      ├─ Detects SDL_MOUSEBUTTONDOWN
        │  └─ Sleep 500ms          │  for (50, 100)
        │  ├─ SDL_MOUSEBUTTONUP    │
        │  ├─ SDL_PushEvent()      │  ├─ Logs "[CLICK] #1 down"
        │  └─ Sleep 1000ms         │  ├─ event(MR_MOUSE_DOWN, 50, 100)
        │                     │     │  │  └─ vmrp_runtime_event()
        │                     │     │  │     └─ Calls emulator
        │                     │     │  │        with new state
        │                     │     │  └─ Logs "[CLICK] #1 down ret=..."
        │ Returns            │     │
        └─────────────────────┼──────┤ Eventually detects
                              │     │  SDL_MOUSEBUTTONUP
                              │     ├─ event(MR_MOUSE_UP, 50, 100)
                              │     │
                              │     └─ Repeat for #2...
```

