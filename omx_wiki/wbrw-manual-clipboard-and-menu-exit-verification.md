---
title: "WBRW manual clipboard and menu exit verification"
tags: ["wbrw", "clipboard", "ctrl-v", "menu-exit", "e2e", "ppm", "arm-ext"]
created: 2026-07-12T01:14:44.827Z
updated: 2026-07-12T10:09:53+08:00
sources: []
links: ["wbrw-proxy2-web-page-rendering.md"]
category: session-log
confidence: high
schemaVersion: 1
---

# WBRW manual clipboard and menu exit verification

2026-07-12 correction: the earlier completion was premature. Its E2E opened the editor and injected Ctrl+V with effectively no pause. A physical 500ms pause reproduced the user's exact stop after `editGetText()` in both clean and persisted state. The SDL timer callback is one-shot: it clears `timeId`, posts `timerEventType`, and depends on guest `timer()` to rearm. Because the main loop entered the edit-mode filter first, it discarded that event while the editor was open and permanently stopped the guest scheduler. The main loop now handles `timerEventType` before edit-mode keyboard filtering. This is generic platform behavior with no application, package, URL, endpoint, or fallback branch.

The final regression pauses 500ms in the editor and sends no successful `SCREEN` or `WAIT_DRAW` command anywhere in the input/load/exit flow. It reads a PPM continuously written by normal rendering, so screenshot events cannot serve as scheduler heartbeats. Two clean dummy-driver 10s/5s matrices passed at `/tmp/skyengine-e2e-tcWtRh`, `/tmp/skyengine-e2e-7RmxsP`, `/tmp/skyengine-e2e-7QXMqO`, and `/tmp/skyengine-e2e-zAI4IE`; the final post-review matrix is `/tmp/skyengine-e2e-nS1ojQ` and `/tmp/skyengine-e2e-yuetNx`. The complete persisted-state path on the final binary passed at `/tmp/skyengine-persistent-final-v2`: `/simpleDownload` and `/page2` were sent, the exact URL/title cache oracle matched, the guest menu exited with code 0, and `final.ppm` is a visually verified 240x320, 113-color rendering of the complete target page. TypeScript, 103 C checks, input-text, and all four gzwdzjs/gghjt game-start scheduler regressions pass.

2026-07-12 final: The prior E2E PASTE operation set the SDL clipboard only after editCreate, so it could not reproduce physical Ctrl+V. Direct source evidence showed editCreate overwrote the pre-existing user clipboard with the field value; this is the test/manual discrepancy. The host editor now leaves the platform clipboard unchanged. E2E control separates SET_CLIPBOARD from PASTE_SHORTCUT, and temp.test.ts sets the URL before opening the editor, then injects only Ctrl+V.

The same test loads the exact cache URL/title, captures loaded/menu/exit-confirm PPMs, selects guest browser menu item 7, confirms with LEFT_SOFT, and requires actual child-process exit rather than SDL QUIT. Two independent post-build dummy-driver matrices passed for idle 10s and 5s, plus input-text. Retained second-round artifacts: /tmp/skyengine-e2e-Wr3KSB, /tmp/skyengine-e2e-zNuZbZ, /tmp/skyengine-e2e-UTJvhc. Both loaded PPMs have 114 colors; home-to-loaded diffs are title 3841, body 59424, full 67230; loaded-to-menu ROI diff 13515; menu-to-confirm ROI diff 18242; settled title pixel (40,15) is 248,240,168. Both stderr logs contain exact editGetText URLs and both stdout logs contain guest mythroad exit.

Narrow PC watches completed the causality proof without a full trace. WBRW game.ext has one compact walker at file +0x440CC with scheduler R9+0x44AC. The due-node path +0x44170/+0x4418A/+0x44190 consumed callback guest 0x2562FD (game +0x30205) immediately before the observed exit. lib.exitbrw watches did not fire. The final host mr_exit may be entered through table[54], callback cleanup observing shared mr_state >= 3, or both; the two exit logs do not distinguish these and sibling ptrace is unavailable. This unknown does not change the proven scheduler dependency: stalled page requests and ineffective menu exit are two consequences of the same lost primary compact-scheduler tick/owner defect.

Compact scheduler scanning was also corrected so walker file offset zero is valid and invalid early literal candidates do not mask later valid walkers. Unit suite: 103 checks/0 failures. Cross-app dummy-driver scheduler regressions passed: gzwdzjs 2/2 and gghjt 2/2. No app/package/host-specific C branch was added.
