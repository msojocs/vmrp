# DOTA download-browser-components display debug

## 2026-06-27 initial scope

- Target command: `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`.
- Current symptom: after the browser plugin is downloaded and launched, the `download-browser-components` capture is expected to show the browser component logo and text, but currently shows only one horizontal line.
- Constraints: no `xvfb`; avoid large trace logs; use focused stdout/PPM evidence; inspect disassembly for the real bug location; no app-specific branches such as checking a specific app name; code edits must include comments and must not add fallback logic.
- Existing context: earlier DOTA browser plugin work already fixed package provenance and short package-name ABI hazards. This round must not assume those fixes cover the current low-detail/line-only display issue.
- Parallel investigation started with subagents:
  - Explorer: map the target test, screenshot capture, and render path.
  - Debugger: look for likely ARM/disassembly or terminal/PPM render causes without editing files.

## Current observations

- `download-browser-components` is not an application step identifier in product code. It is the PPM filename passed to `VmrpE2e.screen()` in `test/e2e/dota/download-plugin.test.ts`.
- The test currently checks only `uniqueColorCount() > 2` at that capture point, which can pass for a broken image that has a background and a single line.
- Further work will reproduce with `VMRP_E2E_KEEP_TMP=1`, inspect the kept PPM, and correlate file/resource loading with the ARM EXT child module currently drawing the screen.

## 2026-06-27 reproduction evidence

- Reproduced with `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`.
- Failure: line 173, `uniqueColorCount()` is exactly `2`, so the current assertion already catches the line-only screen.
- Preserved directories: `/tmp/vmrp-e2e-7VsswN` and `/tmp/vmrp-e2e-pfMs7A`.
- `download-browser-components.ppm` stats in both runs:
  - `240x320`, colors: black plus `#2844a0` only.
  - Non-black pixels: `272`, limited to rows `190..191`, `136` pixels per row.
  - Sample points `(80,80)`, `(120,160)`, `(152,146)` are all black.
- Plain stdout shows the second-stage browser component downloads are happening after the download result is confirmed: repeated `POST /simpleDownload` over `CMNET`, returning HTTP 200. Therefore this is not a network-start failure.

## 2026-06-27 narrow screen diagnostics

- Reproduced with `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 VMRP_ARM_EXT_SCREEN_DIAG=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`.
- Preserved directory: `/tmp/vmrp-e2e-NMVZag`.
- The PPM remained identical: only rows `190..191` contain `#2844a0`.
- Diagnostic evidence near the bad screen shows explicit `present29 x=0 y=0 w=240 h=320 accept=1` from owner `P=0x3CA7A8 H=0x2C92A5`, but `visible_present` samples are all `0000/0000/0000`. This means a full-screen present is being accepted, but the buffer being submitted has only the progress-line style pixels and no logo/text at sampled points.
- Working hypothesis: browser component UI draws logo/text into the ARM screen cache using host screen-cache APIs, but the host-visible submit path only receives a later explicit present with a mostly black source. Need prove by instrumenting table[120]/[122]/[123] dirty spans and disassembling the caller around `0x2C9470`/related `frame.ext` functions.

## 2026-06-27 root-cause update

- Subagent and local evidence agree that the line-only frame is a real browser startup transition frame, not a corrupt PPM parser or terminal rendering issue.
- `git show HEAD:test/e2e/dota/download-plugin.test.ts` shows this path previously waited through low-color startup frames with a comment explaining that the browser plugin first submits a low-color transition frame before drawing body content.
- A subagent sampled the same flow over time: after 4s and 8s the capture remains 2 colors; after 12s and 20s it reaches 27 colors with non-black browser content at `(95,88)`.
- Narrow diagnostics with `/tmp/vmrp-e2e-PzxxLk` showed no `UC_ERR`/crash; the browser component download path continued issuing `simpleDownload` requests. Screen diagnostics showed accepted full-screen presents from the guest, confirming the horizontal line comes from guest startup frames.
- Fix direction: restore content-aware waiting at `download-browser-components`, keeping the screenshot label but allowing timer-driven guest rendering to pass the low-color transition frame. This is generic timing synchronization in the E2E test, not app-specific product code.

## 2026-06-27 disassembly and final verification

- Extracted `frame.ext` and `brw.ext` from `mythroad/plugins/embrw.mrp` into `/tmp` for static inspection. The MRP index contains `focus.bmp`, `progress.bmp`, `logo.bmp`, `font.uni`, `brw.ext`, and `frame.ext`; the browser component visual assets are present in the downloaded package.
- `strings`/offset evidence from `/tmp/vmrp-frame.ext`:
  - `font.uni` at `0x23e0`, adjusted address `0x2c9b60`.
  - `progress.bmp` at `0x2408`, adjusted address `0x2c9b88`.
  - `logo.bmp` at `0x2418`, adjusted address `0x2c9b98`.
  - `CMWAP`/`CMNET` and `brw/vercfg.bin` strings also exist in `frame.ext`.
- Thumb disassembly with `arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x2C7780 /tmp/vmrp-frame.ext` confirms the startup/UI path calls helper functions around `0x2c947c` and the earlier network/timer flow, so the low-color line is produced by guest-side startup frames before the later asset-backed page is visible.
- Final code change: `test/e2e/dota/download-plugin.test.ts` now keeps the `download-browser-components` PPM label but polls through up to 10s of low-color transition frames, then requires `uniqueColorCount() > 4` and a non-black content sample at `(152,146)`.
- Build verification: `cmake --build build --target vmrp -j2` passed.
- E2E verification: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed in `51.681s`.
- Kept verification directory: `/tmp/vmrp-e2e-dbtcUH`.
- PPM evidence:
  - `download-browser-components.ppm`: `240x320`, `unique=29`, `nonzero=74300`, nonzero rows `3..319`, `(80,80)=(240,244,240)`, `(152,146)=(240,244,240)`, `(95,88)=(240,244,240)`.
  - `browser-running.ppm`: `unique=27`, `nonzero=74300`, same content sample points are non-black.
- Runtime logs in the kept directory have normal `simpleDownload` activity and no `UC_MEM`, `UC_ERR`, invalid memory, crash PC, `2CF5CA`, `757478`, or `2C9844` signatures.

## 2026-06-27 correction after frame sampling

- Previous hypothesis that waiting to a 27/29-color frame fixed `download-browser-components` was wrong. That later frame is a different browser stage, not the expected component-download page.
- A custom frame sampler preserved `/tmp/vmrp-e2e-sample-xUy7bc`. After confirming the download result, captures from `250ms` through `12000ms` all remain the same 2-color screen: black background plus `#2844a0` rows `190..191`, bbox `52,190..187,191`. At `14000ms` the screen changes to a 27-color later browser page.
- Correct root-cause target: during the real `download-browser-components` window, `frame.ext` should render the package logo/text but only a progress/separator line becomes visible. The fix must be in the guest resource/draw/present bridge, not in test waiting.

## 2026-06-27 current repair round

- Re-read the dirty worktree before editing. `test/e2e/dota/download-plugin.test.ts` already contains a warning that the target screen must not be loop-polled because polling advances into a later browser stage.
- Spawned a debugger sub-agent for static/disassembly evidence and an explorer sub-agent for ARM EXT table/screen bridge mapping.
- Local code search confirms the relevant bridge entries are the native C-function table slots: `table[118]` (`DispUpEx`/present), `table[120]` (`_DrawBitmap`), `table[122]` (`DrawRect`), `table[123]` (`_DrawText`), and `table[125]` (`mr_readFile`). The next probe will use narrow screen diagnostics and PPM statistics rather than full trace.

## 2026-06-27 foreground cache/present bridge finding

- Narrow diagnostics around `/tmp/vmrp-e2e-eo4lmx/stdout.log` show `frame.ext` (`P=0x3CA7CC H=0x2C92A5`) is loaded and initially claims screen writes; its full-screen `table[29]` present is accepted at LR `0x2C9471`, but visible samples are black, matching the 2-color PPM.
- Immediately after, another private child (`P=0x3CF5D8 H=0x2CDADD`) becomes `activeP` for network/download work while the visual helper still issues presents. The current bridge treats `activeP` as the only visible foreground child and drops cache-damage tracking whenever any foreground child exists.
- Static disassembly confirms `frame.ext` calls through host table wrappers around `0x2C947C` for `table[29]` and resource loading at `0x2C9810`/`0x2C9860`, so the failure is in host screen bridge ownership/presentation semantics rather than missing `logo.bmp`/`font.uni`.
- Next implementation target: allow cache-only damage tracking for the accepted foreground visual owner even while a different private child is active, without adding app-specific checks or long polling.

## 2026-06-28 current worktree retest

- Rebuilt with `cmake --build build --target vmrp -j2`; target was already up to date.
- Reproduced failure with `VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 VMRP_ARM_EXT_SCREEN_DIAG=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose`.
- Kept directory: `/tmp/vmrp-e2e-KazAU3`.
- Failure remains at `download-browser-components`: `uniqueColorCount()` is `2`, confirming the current C change does not yet fix the target page.
- Narrow stdout evidence:
  - `DIAG cache_write lr=0x2C902D ownerP=0x3CA7C0 ownerH=0x2C92A5 claimP=0x0 claimH=0x0 activeP=0x3CF5CC activeH=0x2CDADD targetPrimary=1 track=0 damage=0,180..240,319`
  - `DIAG present29 x=0 y=0 w=240 h=320 accept=1 claimP=0x0 claimH=0x0`
  - `DIAG visible_present bitmap ... ownerP=0x3CA7C0 ownerH=0x2C92A5 ... samples=0000/0000/0000`
- Interpretation: `frame.ext` owns the visual cache writes and has real damaged rows, but `arm_ext_should_accept_screen_write()` returns accepted-without-claim for a child that is neither active nor primary/wrapper. Because foreground child state exists, `arm_ext_should_track_screen_cache_damage()` rejects the unclaimed write. The subsequent accepted full-screen present submits the stale mostly black source, leaving only the blue line visible.

## 2026-06-28 final package-provenance fix

- The foreground cache/present hypothesis above was superseded by narrower resource-read diagnostics. `frame.ext` was registered as a child of the installed browser package, but its `table[125]` calls for `logo.bmp`, `progress.bmp`, `focus.bmp`, `no_focus.bmp`, and `font.uni` were resolved through the outer DOTA package whenever shared `table[100]` still named the root app.
- Root cause: private loaders give children their own package provenance, while the shared ARM C-function table can still expose a blank or root-app `pack_filename`. For child resource reads, `table[125]` must use the caller LR to find the registered child and scope the read to that child package. Explicit non-root package switches remain authoritative.
- Final code change: `src/arm_ext_executor.c` now resolves the resource owner from LR, detects blank/root `table[100]`, and reads from the child module's recorded RAM or host package owner. Temporary `VMRP_ARM_EXT_READ_DIAG`/screen pixel probes were removed.
- Test hardening: `test/e2e/dota/download-plugin.test.ts` keeps the single capture at `download-browser-components` because polling exits this transient page, but now requires `uniqueColorCount() > 100` plus non-black logo/progress samples.
- Verification:
  - `cmake --build build --target vmrp -j2` passed.
  - `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed in `42.77s`; kept directory `/tmp/vmrp-e2e-hPO7o8`.
  - `/tmp/vmrp-e2e-hPO7o8/download-browser-components.ppm`: `240x320`, `unique=1395`, `nonzero=7905`, bbox `52,71..187,234`, `(95,88)=(248,248,120)`, `(120,190)=(40,68,160)`, `(152,146)=(0,16,32)`. The converted PNG shows the browser logo, `冒泡浏览器`, progress line, and `正在更新`.
  - Runtime logs had no `UC_ERR`, `UC_MEM`, invalid-memory, crash-PC, `2CF5CA`, `757478`, `2C9844`, or PC-byte signatures.
  - Regression: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/optwar/game-prepare.test.ts --reporter=verbose` passed. The untracked `test/e2e/optwar/exit-plugin.test.ts` probe currently fails because it captures the optwar exit failure dialog rather than a plugin download result; it is not a committed regression test.
