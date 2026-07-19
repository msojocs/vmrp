# VMRP Threading Model

## Thread Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                     MAIN PROCESS                             │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────────────┐     ┌─────────────────────┐   │
│  │  Main Thread             │     │ Auto-Click Thread   │   │
│  │                          │     │ (spawned by loop()) │   │
│  │  - SDL_Init()            │     │                     │   │
│  │  - SDL_CreateWindow()    │     │ - Parse .env vars   │   │
│  │  - startVmrp() [ARM VM]  │     │ - Sleep delays      │   │
│  │  - loop() [EVENT LOOP]   │     │ - SDL_PushEvent()   │   │
│  │  - Signal handlers       │     │                     │   │
│  └──────────────────────────┘     └─────────────────────┘   │
│           ▲                               │                  │
│           │                               │                  │
│     ┌─────┴───────────────────────────────┴──────┐            │
│     │                                            │            │
│     │     SDL Timer Thread (if timers active)    │            │
│     │                                            │            │
│     │ - Runs timerCb() callback                  │            │
│     │ - Creates custom SDL event                │            │
│     │ - Pushes to queue                         │            │
│     │                                            │            │
│     └────────────────────────────────────────────┘            │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

## Critical Synchronization Point: SDL Event Queue

```
╔════════════════════════════════════════════════════════════╗
║               SDL EVENT QUEUE (Thread-Safe)                ║
║              (maintained by SDL internals)                 ║
╠════════════════════════════════════════════════════════════╣
║                                                            ║
║ Producers:                                                 ║
║   1. Auto-Click Thread   → SDL_PushEvent()  ┐             ║
║   2. SDL Timer Thread    → SDL_PushEvent()  │ Thread-safe ║
║   3. X11/Wayland Input   → OS integration   │             ║
║   4. System Events       → OS integration   ┘             ║
║                                                            ║
║ Consumer:                                                  ║
║   Main Thread: SDL_WaitEvent(&ev) [BLOCKS until event]   ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝
```

## Timeline: Auto-Click Sequence

```
TIME │ MAIN THREAD          │ AUTO-CLICK THREAD      │ SDL TIMER THREAD
─────┼──────────────────────┼────────────────────────┼──────────────────
  0  │ loop() starts        │                        │
     │                      │ SDL_CreateThread()     │
  1  │                      │ created                │
     │ Blocks on            │                        │
  2  │ SDL_WaitEvent()      │ Parse env vars         │
     │ [BLOCKING]           │                        │
  3  │                      │ Sleep 800ms            │
     │                      │ (app startup)          │
  4  │                      │                        │ SDL_AddTimer() called
     │                      │ Done sleeping          │  from emulated code
  5  │                      │ Create KEYDOWN event   │ └─ timerCb registered
     │                      │ SDL_PushEvent()        │
  6  │ Wakes up!            │ [queue updated]        │
     │ (event arrived)      │                        │
  7  │ Handle KEYDOWN       │ Sleep 50ms             │
     │ (execute game code)  │ (hold key)             │
  8  │ Back to              │ Create KEYUP event     │
     │ SDL_WaitEvent()      │ SDL_PushEvent()        │
  9  │ [BLOCKING]           │ Sleep 800ms            │
     │                      │ (before next)          │ [timer fires]
 10  │ Wakes up!            │                        │
     │ (KEYUP arrived)      │                        │ timerCb()
 11  │ Handle KEYUP         │ Sleep over             │ ├─ Create event
     │ Back to wait         │ Create click DOWN      │ ├─ SDL_PushEvent()
 12  │ SDL_WaitEvent()      │ SDL_PushEvent()        │ └─ return 0
     │                      │ (queue updated)        │
 13  │ Wakes up! (click)    │ Sleep 500ms            │
     │ Handle DOWN          │ (click duration)       │
     │                      │                        │
```

## Synchronization Mechanisms

### 1. SDL Event Queue (Producer-Consumer)

```c
// Auto-Click Thread (Producer)
SDL_Event ev;
ev.type = SDL_MOUSEBUTTONDOWN;
ev.button.x = 50;
ev.button.y = 100;
SDL_PushEvent(&ev);  // ← Thread-safe queue operation
```

```c
// Main Thread (Consumer)
while (SDL_WaitEvent(&ev)) {  // ← Blocks until event available
    if (ev.type == SDL_MOUSEBUTTONDOWN) {
        event(MR_MOUSE_DOWN, ev.button.x, ev.button.y);
    }
}
```

### 2. Unicorn Engine Access Control

**Problem**: Unicorn ARM emulator is NOT thread-safe.

**Solution**: All emulator access serialized through main thread.

```
┌─────────────────────────┐
│ Timer Thread            │
├─────────────────────────┤
│ timerCb() [runs here]   │
│  ├─ CANNOT call         │
│  │  emulator directly   │
│  │                      │
│  └─ Instead:            │
│     └─ Create SDL event │
│        └─ SDL_PushEvent │
└─────────────────────────┘
         ▼
┌─────────────────────────┐
│ Main Thread             │
├─────────────────────────┤
│ loop() [SAFE]           │
│  └─ timer()             │
│     └─ skyengine_runtime_    │
│        timer()          │
│        └─ Unicorn ✓     │
│           [SAFE]        │
└─────────────────────────┘
```

### 3. State Variables (Not Protected)

```c
static bool isMouseDown = false;           // Accessed by:
                                           //   - Main: read/write
                                           //   - Auto-click: none
                                           //   - Timer: none
                                           // → Safe (main only)

static uint32_t clickSeq = 0;             // Accessed by:
                                          //   - Main: ++read/write
                                          //   - Auto-click: none
                                          //   - Timer: none
                                          // → Safe (main only)

static AutoClickPoint *autoClickList;     // Accessed by:
                                          //   - Main: read (setup)
                                          //   - Auto-click: read only
                                          //   - Timer: none
                                          // → Safe (no concurrent writes)
```

**Key insight**: Auto-click thread reads `autoClickList` without writing, and main thread doesn't write after thread starts.

## Race Condition Analysis

### Potential Issue #1: SIGUSR1 Handler + SDL_UpdateWindowSurface

```c
// Signal Handler (async context)
static void sigusr1_handler(int sig) {
    dump_screen_ppm("/tmp/skyengine_screen.ppm");  // ← Reads window
}

// Main Thread
void guiDrawBitmap(...) {
    SDL_LockSurface(surface);
    // ... update pixels
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);  // ← Signal could fire here
}
```

**Mitigation**: Lock surface before reading; dump_screen_ppm handles locking too.

### Potential Issue #2: Auto-Click Thread + User Input

```
Both produce to same queue, but:
  - Queue is thread-safe (SDL handles locking)
  - Main thread consumes in order (FIFO)
  - No race condition possible
```

### Potential Issue #3: Timer Callback + Auto-Click

```
timerCb() [timer thread]
    ├─ Create event
    └─ SDL_PushEvent()

autoClickThread()
    └─ SDL_PushEvent()

Both safe: SDL_PushEvent() is thread-safe; events processed sequentially.
```

## Environment Variable Timing

### When Parsed

```
startup sequence:
  ├─ main() called
  ├─ prepareVmrpArgs() → reads VMRP_SCREEN_*
  ├─ SDL_Init()
  ├─ startVmrp() → reads VMRP_MRP, VMRP_EXT, VMRP_ENTRY
  └─ loop()
     ├─ startAutoClicksIfRequested()
     │  └─ getenv("SKYENGINE_AUTO_CLICKS") ← READ
     │  └─ getenv("SKYENGINE_AUTO_CLICK_DELAY_MS") ← READ
     └─ Event loop
        ├─ autoClickThread()
        │  └─ getenv("SKYENGINE_AUTO_CLICK_DELAY_MS") ← READ
        └─ [main loop]
           └─ getenv("SKYENGINE_PPM") ← CHECK per draw (guiDrawBitmap)
```

**Thread safety**: All env var reads are safe (environment doesn't change at runtime).

---

## Summary

| Component | Thread | Safety | Notes |
|-----------|--------|--------|-------|
| SDL Event Queue | Multiple | ✓ | SDL manages locks |
| Unicorn Engine | Main only | ✓ | Serialized via events |
| Auto-click list | Main (read) + Auto (read) | ✓ | No concurrent writes |
| isMouseDown | Main only | ✓ | Single writer |
| clickSeq | Main only | ✓ | Single writer |
| Environment vars | All (read) | ✓ | Read-only, stable |
| SIGUSR1 handler | Signal + Main | ⚠️ | Needs careful locking |

