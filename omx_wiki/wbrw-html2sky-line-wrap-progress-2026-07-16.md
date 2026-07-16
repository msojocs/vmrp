---
title: "WBRW html2sky explicit line break progress 2026-07-16"
tags: ["wbrw", "html2sky", "line-break", "sky-format", "ppm"]
created: 2026-07-16T18:31:00+08:00
updated: 2026-07-16T20:24:00+08:00
sources: []
links: ["wbrw-html2sky-native-elements-and-ppm-verification-2026-07-12.md"]
category: session-log
confidence: high
schemaVersion: 1
---

# WBRW html2sky explicit line break progress 2026-07-16

## Goal and boundaries

- Repro script: `test/e2e/wbrw/temp.test.ts`, filtered to the `mrp.gddhy.net` scenario.
- Expected: the content following `mynes [顶]` starts on a new line.
- Verification is visual: retain and inspect the loaded PPM. The temporary repro script is not being converted into a visual assertion.
- Source fix belongs in `/home/msojocs/github/sky-engine-tool`, whose Air process hot reloads the proxy2 test server on port 8093.
- Pre-existing vmrp changes in `docs/prompt.md`, `src/vmrp_args.c`, and `test/e2e/wbrw/temp.test.ts` remain untouched. The sky-engine-tool repository starts with all source files untracked.

## Baseline reproduction

- Command: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/temp.test.ts -t 'mrp.gddhy.net' --reporter=verbose`.
- Result: the selected reproduction scenario completed in 25.67 seconds. Passing only proves the page replaced the address editor; its sole final check is a large pixel difference.
- Artifact: `/tmp/vmrp-e2e-NX5u4a/loaded.ppm`, 240x320, SHA-256 `4f6ff731e313be1600c0f32a1b93227b818f5f8d7f9d9a2106535e5753c1f329`.
- Visual inspection: `RX管理器 [顶]` follows `mynes [顶]` on the same text baseline. The remaining links also run together and only wrap when the screen width is exhausted.
- Runtime log confirms `proxy2.51mrp.com -> 127.0.0.1`, followed by `POST /page2` through nginx with HTTP 200. The active backend is sky-engine-tool's Air-managed server on port 8093.

## Root cause

- The live HTML is `<p><a>mynes [顶]</a><br/><br/><a>RX管理器 [顶]</a>...`.
- `html2sky.Extract` assigns one `FlowID` to the whole paragraph. Anchor items inherit it, while the `br`/`hr` branch only flushes pending text and does not advance the flow.
- `generateSkyDisplayList` caches rows by `(containerID, FlowID)`, so every link is emitted under one `0x4E` row. The client correctly treats the links as one inline flow; the explicit HTML breaks never reach the SKY layout.
- A newline inside `Item.Text` cannot express the break because `collapseText` removes it. The protocol-level fix is to start a new flow after an explicit break, causing a new `0x4E` row.
- Consecutive breaks will guarantee a new content row but will not preserve an otherwise empty row. Exact blank-line preservation is outside the requested result; the required strong break is retained without adding synthetic text.

## Implementation and final verification

- `html2sky.Extract` now advances `nextFlowID` after flushing a `<br>` or `<hr>` inside a paragraph. Inline siblings without an explicit break still share their paragraph flow; content following the break receives a new SKY row.
- `TestExtractExplicitBreakStartsNewFlow` covers `<p><a>A</a><br><br><a>B</a></p>` and requires distinct nonzero flow IDs for A and B.
- `go test -count=1 ./...` passes in sky-engine-tool: html2sky and proxy2 pass, with the command package reporting no test files.
- Air rebuilt `.tmp/air/proxy2-test-server` after the source change, restarted the service on port 8093, and `GET /health` returned `ok`.
- The same selected reproduction passed in 25.51 seconds after hot reload.
- Final artifact: `/tmp/vmrp-e2e-dpBTag/loaded.ppm`, 240x320, SHA-256 `403468815271d872cc2965d2c7758fcd8681c7516f42a389df83d8f7e9403731`.
- Visual result: `mynes [顶]` ends its own line. `RX管理器 [顶]` begins on the next lower text baseline at the left edge, and every following application link follows the explicit HTML breaks. This matches the requested result.
- The temporary Vitest repro script was not changed by this fix and gained no visual assertion; the retained page image remains the authoritative end-to-end evidence.

## Completed checks

- [x] Saved and inspected a failing baseline PPM.
- [x] Traced the live HTML break through FlowID and `0x4E` row generation.
- [x] Implemented the generic explicit-break boundary and a focused generator regression.
- [x] Ran all sky-engine-tool Go tests after hot reload.
- [x] Saved and inspected a passing final PPM.

## 2026-07-16 form background continuation

- The updated repro presses `6` after loading `mrp.gddhy.net`, so its final frame exposes both forms near the bottom of the page.
- Current paired artifacts are `/tmp/vmrp-e2e-KXJf4B/loaded.ppm` for `task.jysafe.cn/so1` and `/tmp/vmrp-e2e-D8K03K/loaded.ppm` for `mrp.gddhy.net`.
- The task page source explicitly declares `.c1`/`.title` background `#d2ff8d` and `.cpy` background `#fcffb3`. Its frame correctly contains 32,394 exact green pixels in `x=0..230,y=79..261`.
- The gddhy source has no stylesheet, inline background, or `bgcolor`. Extraction assigns all 38 items `ContainerID=0` and `ContainerElement=0`, yet its frame contains 3,248 exact green pixels in four rectangles surrounding the two form rows.
- Root cause: every generated `0x61` form currently receives hard-coded element `9`. Reference SKY pages use element 9 for forms nested under an explicit element-7 colored container, while another real sample uses neutral element 3 for ordinary top-level forms.
- Planned generic correction: emit element 9 only when the extracted form belongs to a real element-7 styled container; use the reference neutral element 3 for an unstyled form. No host, URL, title, form name, or source color value will select behavior.

### Form background implementation and PPM proof

- Historical SKY files refined the neutral mapping: `test/html2sky/cache3/1078817.sky` uses element 3 for two ordinary forms, while `test/html2sky/32918048.sky` uses element 9 for both forms under explicit green containers.
- Generation now selects element 3 by default and element 9 only when the extracted pair has a real container ID with container element 7. The decision is based solely on parsed CSS container ownership.
- Binary regression coverage generates one unstyled form and one form inside an explicit section background, parses the resulting `0x61` records, and requires elements 3 and 9 respectively.
- Final task artifact: `/tmp/vmrp-e2e-1UrKUI/loaded.ppm`, SHA-256 `21eeca48c162fdafe311d1d79fd63b9e1e9aef04bac28ce03cc702702d567676`. It retains 32,394 exact green pixels and 3,208 exact yellow pixels; visual inspection shows both styled green sections, controls, and the yellow footer.
- Final gddhy artifact after pressing `6`: `/tmp/vmrp-e2e-IyUwMO/loaded.ppm`, SHA-256 `a398a627982d047cf55f2340d639c483cd9a3e184bf44621585591c21cf885a0`. Exact green and yellow pixel counts are both zero; both forms, their controls, surrounding body background, and footer links remain visible.
- The selected gddhy repro passes. The complete two-page script currently exits on task's pre-existing logo-matte assertion because `(100,43)` is black in the live image rather than framebuffer white; the retained loaded PPM proves this does not affect the background comparison or the preserved CSS green regions.

## 2026-07-16 horizontal-rule acceptance correction

- The original acceptance criterion is stricter than the earlier line-wrap result: a visible horizontal line must appear immediately below `软件列表`.
- The live source is `<p>WAP下载站 - 第1页<br/><br/>软件列表</p><hr/><p>...`. The prior FlowID change correctly preserves `<br>` boundaries, but the shared `br`/`hr` branch still discards the `<hr>` itself.
- Real SKY evidence identifies `0x60` with a one-byte zero payload as the horizontal separator operation. The current generator's static style table uses element 14 for the grey rule; a historical element-8 rule belongs to a different style table with an equivalent rule definition and must not be copied as an absolute index. The record remains a direct child of the current content/container parent so it stays in DOM order with surrounding rows.
- Generated live-page record counts contain no `0x60`, directly proving the visible rule is absent before rendering. `/tmp/vmrp-e2e-dpBTag/loaded.ppm` shows the current top frame with `软件列表` followed immediately by `mynes [顶]` and no line.
- A fresh pre-fix run completed at `/tmp/vmrp-e2e-M19GN0/loaded.ppm`, but the temporary script's later `6` key jumped to the footer before capture. That step is unrelated to the requested visual and will be removed so the retained final image covers the actual acceptance region.
- Planned generic fix: preserve `<hr>` as a dedicated model item in DOM order and emit a real `0x60` record. Keep `<br>` as a flow boundary; do not approximate an `<hr>` with an empty text row.

### Horizontal-rule implementation and PPM proof

- `ItemKindHorizontalRule` now preserves each `<hr>` without fabricating display text. `buildPage` retains the structural pair, and display-list generation writes `op=0x60`, element 14, payload `00` directly under the current content or styled-container parent.
- The focused binary regressions require the rule to remain between surrounding model items, verify the exact top-level record, and verify that a rule inside an element-7 background container uses that container as parent while retaining global separator element 14.
- Live generated gddhy records are ordered as `软件列表` text (`0x54`), separator (`parent=0x4005, e2=14, op=0x60, payload=00`), first application row (`0x4e`), then the `mynes` anchor (`0x38`).
- Air rebuilt and restarted the test service on port 8093. `go test -count=1 ./...` and `go vet ./...` pass, and `/health` returns `ok`.
- The focused repro completed in 15.77 seconds after removing the unrelated footer jump. Final image: `/tmp/vmrp-e2e-yQ9666/loaded.ppm`, 240x320, SHA-256 `2dfee34fc01fe11f5cb1069466c79c7744a7415a52f4f10b677df82d0fd31331`; PNG inspection copy: `/tmp/vmrp-e2e-yQ9666/loaded.png`.
- Visual result: a light-grey horizontal rule spans the content area directly below `软件列表`, and `mynes [顶]` begins beneath it. Pixel proof: rows `y=84` and `y=86` each contain 230 exact `[232,236,232]` rule pixels; the pre-fix image had only 2 such border pixels on each row.
