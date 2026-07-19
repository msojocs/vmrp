# VMRP Project Internals - Complete Documentation Index

This directory contains comprehensive analysis of the VMRP (Virtual MythRoad Platform) architecture, focusing on:
1. **Auto-click injection** (VMRP_AUTO_CLICKS)
2. **Screen dump mechanism** (PPM format, /tmp/skyengine_screen.ppm)
3. **SDL event loop architecture**
4. **Thread synchronization**

---

## 📄 Documentation Files

### 1. **ARCHITECTURE_ANALYSIS.md** (18 KB, 432 lines)
**Complete, in-depth technical reference**

Contains:
- ✅ Full auto-click parsing and injection flow (lines 11-90)
  - Environment variable format and timing
  - Complete thread architecture (autoClickThread)
  - Event queuing mechanism
  - Main loop event processing
  - Dispatch to emulated code

- ✅ PPM screenshot mechanism (lines 94-172)
  - Three triggering methods (frame 5, VMRP_PPM, SIGUSR1)
  - Binary format specification
  - guiDrawBitmap integration
  - Environment control variables



- ✅ SDL main loop architecture (lines 211-378)
  - Initialization phase (SDL_Init, window creation)
  - Event loop design (SDL_WaitEvent blocking model)
  - Event processing flow diagram
  - Key state management (isMouseDown, isKeyDown, clickSeq)
  - Timer event handling (thread-safe mechanism)
  - Display update pipeline

- ✅ Complete flow diagram (lines 381-432)

**Use this for**: Deep understanding of each system's implementation

---

### 2. **QUICK_REFERENCE.md** (7 KB, 231 lines)
**Fast lookup guide for common tasks**

Contains:
- Auto-click usage examples
  - Format strings
  - Special key codes (-1 to -4)
  - Timing configuration
- PPM triggers (automatic, manual via SIGUSR1)
- SDL event flow diagram (compact)
- Event type table
- Environment variables table
- File location references

**Use this for**: Quick syntax lookups, copy-paste examples, quick reference

---

### 3. **THREADING_MODEL.md** (12 KB, 247 lines)
**Thread safety and synchronization analysis**

Contains:
- Thread architecture diagram
- Auto-click and SDL timer thread details
- SDL Event Queue (critical synchronization point)
- Timeline of auto-click + timer + main thread interaction
- Synchronization mechanisms
  - Producer-consumer pattern
  - Unicorn engine thread isolation
  - State variable access analysis
- Race condition analysis
  - SIGUSR1 handler + SDL surface access
  - Auto-click thread + user input
  - Timer + auto-click
- Environment variable parsing timeline

**Use this for**: Understanding thread safety, debugging race conditions, designing thread-safe changes

---

## 🔍 Quick Navigation by Topic

### Auto-Clicks (VMRP_AUTO_CLICKS)

| Topic | File | Lines |
|-------|------|-------|
| Format specification | ARCHITECTURE_ANALYSIS.md | 18-35 |
| Parsing code walkthrough | ARCHITECTURE_ANALYSIS.md | 40-90 |
| Usage examples | QUICK_REFERENCE.md | 6-30 |
| Threading details | THREADING_MODEL.md | 62-145 |
| Event queue role | THREADING_MODEL.md | 45-58 |

### PPM Screen Dumps

| Topic | File | Lines |
|-------|------|-------|
| Three trigger methods | ARCHITECTURE_ANALYSIS.md | 98-136 |
| Binary format | ARCHITECTURE_ANALYSIS.md | 138-158 |
| guiDrawBitmap integration | ARCHITECTURE_ANALYSIS.md | 160-172 |
| Manual capture via SIGUSR1 | QUICK_REFERENCE.md | 48-56 |
| Race condition analysis | THREADING_MODEL.md | 188-197 |

### SDL Event Loop

| Topic | File | Lines |
|-------|------|-------|
| Initialization | ARCHITECTURE_ANALYSIS.md | 219-237 |
| Main loop design | ARCHITECTURE_ANALYSIS.md | 241-278 |
| Event processing | ARCHITECTURE_ANALYSIS.md | 282-323 |
| State tracking | ARCHITECTURE_ANALYSIS.md | 327-339 |
| Timer handling | ARCHITECTURE_ANALYSIS.md | 343-363 |
| Full flow diagram | ARCHITECTURE_ANALYSIS.md | 367-378 |
| Compact diagram | QUICK_REFERENCE.md | 64-91 |

### Thread Synchronization

| Topic | File | Lines |
|-------|------|-------|
| Architecture overview | THREADING_MODEL.md | 1-37 |
| Event queue (critical) | THREADING_MODEL.md | 39-58 |
| Timeline simulation | THREADING_MODEL.md | 60-80 |
| Producer-consumer pattern | THREADING_MODEL.md | 83-100 |
| Unicorn isolation | THREADING_MODEL.md | 102-127 |
| Race conditions | THREADING_MODEL.md | 160-208 |

---

## 📋 Key Code Sections by File

### src/main.c

| Function | Lines | Purpose |
|----------|-------|---------|
| `dump_screen_ppm()` | 50-68 | Write PPM format to disk |
| `sigusr1_handler()` | 70-73 | Signal handler for manual screenshot |
| `guiDrawBitmap()` | 136-164 | Display update + PPM dump trigger |
| `timerCb()` | 179-188 | SDL timer callback (thread-safe event queuing) |
| `keyEvent()` | 208-295 | SDL keycode → MythRoad key code conversion |
| `autoClickThread()` | 313-374 | Background thread that injects clicks |
| `startAutoClicksIfRequested()` | 376-405 | Parse VMRP_AUTO_CLICKS and start thread |
| `loop()` | 407-498 | **Main event loop** - central processing hub |
| `main()` | 531-587 | Initialization and startup |

### src/skyengine.c

| Function | Lines | Purpose |
|----------|-------|---------|
| `event()` | 347-349 | Dispatcher to skyengine_runtime_event() |
| `timer()` | 351-353 | Timer dispatcher |

### src/skyengine_api.c

| Function | Lines | Purpose |
|----------|-------|---------|
| `guiDrawBitmap()` | 19-32 | Headless variant (no SDL) |
| `skyengine_api_event()` | 131-133 | Public API event injection |

---

## 🎯 Use Cases by Reader Type

### 👨‍💻 **If you're debugging auto-click behavior:**
1. Start: QUICK_REFERENCE.md → Auto-Clicks section (syntax validation)
2. Then: ARCHITECTURE_ANALYSIS.md → "Complete Injection Flow" (line 40)
3. Finally: THREADING_MODEL.md → "Timeline" (line 60) if timing is weird

### 🔧 **If you're modifying the event loop:**
1. Start: ARCHITECTURE_ANALYSIS.md → "Event Loop Design" (line 241)
2. Reference: QUICK_REFERENCE.md → "Event Types" table (line 76)
3. Safety check: THREADING_MODEL.md → "Race Condition Analysis" (line 160)

### 🧵 **If you're adding threading or synchronization:**
1. Start: THREADING_MODEL.md → "Thread Architecture" (line 1)
2. Deep dive: "Synchronization Mechanisms" (line 82)
3. Reference: ARCHITECTURE_ANALYSIS.md → "Timer Event Flow" (line 343)

### 📊 **If you're adding a new input source (keyboard, joystick, etc.):**
1. Reference: ARCHITECTURE_ANALYSIS.md → "Event Dispatch" (line 315)
2. Check: THREADING_MODEL.md → "Synchronization" (line 82)
3. Modify: src/main.c loop() function (line 407)

### 🖼️ **If you're debugging PPM screenshot issues:**
1. Start: QUICK_REFERENCE.md → "Screen Dumps" section (line 42)
2. Details: ARCHITECTURE_ANALYSIS.md → "PPM Format" (line 138)
3. Integration: ARCHITECTURE_ANALYSIS.md → "guiDrawBitmap() Flow" (line 160)

---

## 💡 Key Insights

### 1. Event Queue as Central Hub
```
Auto-clicks → SDL Event Queue ← Real Input ← SDL Timers
                        ↓
                   Main loop
                   (single-threaded)
                        ↓
                 skyengine_runtime_event()
                 (Unicorn ARM engine)
```

**Why this design?**
- Unicorn engine is NOT thread-safe → forces single-threaded access
- SDL event queue is thread-safe → safe to push from multiple threads
- Main thread serializes all emulator calls → no race conditions

### 2. PPM Dumps Don't Block
- Triggered every 30 frames (when VMRP_PPM set)
- Happens on main thread (no new thread spawned)
- Locks SDL surface briefly to read pixels
- Writes to `/tmp/skyengine_screen.ppm` (usually fast on modern filesystems)

### 3. Timer Events Use Custom SDL Event Type
- Timer callback runs in SDL's timer thread
- Can't call Unicorn directly (not thread-safe)
- Instead: Create custom SDL event, push to queue
- Main loop processes it safely

---

## 🔗 Cross-References

### Auto-Click Format Details
- Parsing: ARCHITECTURE_ANALYSIS.md lines 376-405
- Examples: QUICK_REFERENCE.md lines 6-30
- Timing: QUICK_REFERENCE.md lines 31-39
- Threading: THREADING_MODEL.md lines 62-145

### PPM Screenshot Process
- Dump function: src/main.c lines 50-68
- Triggers: ARCHITECTURE_ANALYSIS.md lines 141-143
- Integration: ARCHITECTURE_ANALYSIS.md lines 136-164
- SIGUSR1: QUICK_REFERENCE.md lines 51-56

### Event Processing Loop
- Structure: ARCHITECTURE_ANALYSIS.md lines 407-498
- Diagram: QUICK_REFERENCE.md lines 64-91
- Threading: THREADING_MODEL.md lines 39-145
- Safety: THREADING_MODEL.md lines 160-208

---

## ✅ Verification Checklist

To verify your understanding:

- [ ] I can explain the auto-click format with at least 3 examples
- [ ] I understand why Unicorn access must be serialized to the main thread
- [ ] I can trace an SDL_MOUSEBUTTONDOWN event from user input to emulator call
- [ ] I can explain the timeline of auto-clicks vs. real input
- [ ] I understand PPM format and the three triggering methods
- [ ] I know what VMRP_PPM does differently from the default behavior

If you answer "yes" to all of these, you have a solid understanding of VMRP internals!

---

## 📞 Quick Lookups

**"How do I enable auto-clicks?"**
→ QUICK_REFERENCE.md, "Auto-Clicks" section

**"What's the format for VMRP_AUTO_CLICKS?"**
→ ARCHITECTURE_ANALYSIS.md, lines 18-35

**"How does the event loop work?"**
→ ARCHITECTURE_ANALYSIS.md, lines 241-378 (or QUICK_REFERENCE.md for diagram)

**"Is my change thread-safe?"**
→ THREADING_MODEL.md, "Synchronization Mechanisms" and "Race Condition Analysis"

**"Why can't I call timer() from the timer thread?"**
→ THREADING_MODEL.md, lines 102-127

**"How do I capture a screenshot manually?"**
→ QUICK_REFERENCE.md, lines 51-56

**"What environment variables are available?"**
→ QUICK_REFERENCE.md, lines 143-158

---

**Generated:** 2026-06-05
**Files:** 3 comprehensive documents (37 KB total)
**Coverage:** Auto-clicks, PPM dumps, mouse filtering, SDL loop, threading

