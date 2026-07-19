# VMRP Systems Quick Reference

## Auto-Clicks (VMRP_AUTO_CLICKS)

### Usage
```bash
export VMRP_AUTO_CLICKS="x1,y1[,delay1];x2,y2[,delay2];..."
export VMRP_AUTO_CLICK_DELAY_MS=1000  # default: 800
./skyengine game.mrp
```

### Format Examples
```
"10,50;100,150"              # Two clicks: (10,50) then (100,150)
"10,50,500;100,150,1000"     # Custom delays: 500ms, 1000ms
"-4,0;50,100;-2,0"           # SELECT key, click, then NO key
```

### Special Key Codes
- `-1` → ESC (POWER)
- `-2` → SOFTRIGHT (否/No)
- `-3` → SOFTLEFT (是/Yes)
- `-4` → SELECT (确认/Confirm)

### Timing
- **Global default**: 800ms (configurable via VMRP_AUTO_CLICK_DELAY_MS)
- **Initial delay**: One default interval (lets app startup)
- **Click hold duration**: 500ms (hardcoded in autoClickThread)
- **Individual override**: Third parameter in format string

---

## Screen Dumps (PPM Format)

### Output File
```
/tmp/skyengine_screen.ppm
```

### Automatic Triggers
```bash
# Always captures on 5th frame
./skyengine game.mrp

# Continuous capture every 30 frames (5th frame + every 30 after)
VMRP_PPM=1 ./skyengine game.mrp
```

### Manual Capture
```bash
# In another terminal, send SIGUSR1 signal
kill -USR1 <skyengine-pid>
```

### Format
- **Header**: `P6\n<width> <height>\n255\n` (P6 = raw binary PPM)
- **Pixels**: RGB8 triplets (3 bytes per pixel)
- **View**: `display /tmp/skyengine_screen.ppm` or `convert /tmp/skyengine_screen.ppm output.png`

---

## SDL Event Flow

### Sources
```
┌──────────────────────────────────────────┐
│ Input Sources                            │
├──────┬──────────────────┬────────────────┤
│ Auto │ Real Input       │ SDL Timers     │
│ Clicks│ (X11/Wayland)   │                │
└──┬───┴─────┬────────────┴────────┬───────┘
   │         │                     │
   └─────────┼─────────────────────┘
             │
      ┌──────▼──────────┐
      │ SDL Event Queue │
      └──────┬──────────┘
             │
      ┌──────▼──────────┐
      │ loop()           │
      │ SDL_WaitEvent()  │
      │ [BLOCKING]       │
      └──────┬──────────┘
             │
      ┌──────▼──────────────────┐
      │ Event Handlers:          │
      │ - SDL_KEYDOWN/UP         │
      │ - SDL_MOUSEBUTTONDOWN/UP │
      │ - SDL_MOUSEMOTION        │
      │ - Custom timer event     │
      └──────┬──────────────────┘
             │
      ┌──────▼──────────────┐
      │ event()              │
      │ → skyengine_runtime_event │
      └──────┬───────────────┘
             │
      ┌──────▼──────────────┐
      │ ARM Unicorn Engine  │
      │ (Mythroad VM)       │
      └──────────────────────┘
```

### Event Types
| Event | Handler | Notes |
|-------|---------|-------|
| SDL_KEYDOWN | keyEvent(MR_KEY_PRESS) | Skips if key already down |
| SDL_KEYUP | keyEvent(MR_KEY_RELEASE) | Checks matching key |
| SDL_MOUSEBUTTONDOWN | event(MR_MOUSE_DOWN) | |
| SDL_MOUSEBUTTONUP | event(MR_MOUSE_UP) | |
| SDL_MOUSEMOTION | event(MR_MOUSE_MOVE) | Only if isMouseDown=true |
| timerEventType | timer() → skyengine_runtime_timer() | Custom SDL event |

---

## Key State Tracking

### Global Variables
```c
static bool isMouseDown = false;           // Click state
static SDL_Keycode isKeyDown = SDLK_UNKNOWN;  // Held key
static uint32_t clickSeq = 0;              // Click counter (for logging)
```

### Why
- **isMouseDown**: Prevent MOUSEMOTION without prior DOWN
- **isKeyDown**: Prevent SDL key-repeat from re-triggering
- **clickSeq**: Match DOWN/UP pairs in logs

---

## Display Updates

### Pipeline
```
Emulated code
    ↓
guiDrawBitmap(bmp, x, y, w, h)
    ├─ Increment guiDrawBitmapCount
    ├─ If count==5 or (VMRP_PPM && count%30==0): dump_screen_ppm()
    ├─ Lock SDL surface
    ├─ Copy RGB565 pixels to SDL surface
    │   └─ SDL_MapRGB() converts to surface format
    └─ SDL_UpdateWindowSurface()
        └─ Composites to X11/Wayland
```

### PPM Dumps
- **Frame 5**: Always
- **Every 30 frames**: Only if VMRP_PPM env var set
- **On demand**: `kill -USR1 <pid>`

---

## Timer Events (Thread-Safe)

### Why Custom Event?
Timer callback runs in SDL's timer thread, but ARM Unicorn is NOT thread-safe.

### Solution
```
SDL_AddTimer(t, timerCb, NULL)
    ↓
timerCb() [timer thread]
    └─ Create custom SDL event (timerEventType)
       └─ SDL_PushEvent() → queues to main thread
          ↓
          Main loop detects event
          └─ timer() → skyengine_runtime_timer()
             └─ Executes in main thread [SAFE]
```

### Key Code
```c
// In main.c:179-188
Uint32 timerCb(Uint32 interval, void *param) {
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = timerEventType;  // Custom event ID
    SDL_PushEvent(&ev);        // Queue to main thread
    return 0;                  // No repeat
}
```

---

## Environment Variables Summary

| Variable | Default | Purpose |
|----------|---------|---------|
| VMRP_AUTO_CLICKS | (unset) | Automated click sequence |
| VMRP_AUTO_CLICK_DELAY_MS | 800 | Delay between clicks (ms) |
| VMRP_PPM | (unset) | Continuous PPM dumps every 30 frames |
| SKYENGINE_SCREEN_WIDTH | 240 | Screen width (overridden by --screen) |
| SKYENGINE_SCREEN_HEIGHT | 320 | Screen height (overridden by --screen) |
| VMRP_MRP | dsm_gm.mrp | MRP file to load |
| VMRP_EXT | start.mr | Extension/entry point |
| VMRP_ENTRY | (none) | Entry function |

---

## File References

| File | Purpose |
|------|---------|
| src/main.c (1-100) | PPM dump functions, signal handlers |
| src/main.c (297-405) | Auto-click parsing & threading |
| src/main.c (407-498) | SDL event loop implementation |
| src/main.c (179-188) | Timer event callback |
| src/skyengine.c (347-349) | event() dispatcher |
| src/skyengine_api.c | Headless API (no SDL) |

