---
title: "DOTA browser plugin network state investigation"
tags: ["dota", "browser-plugin", "network", "cmwap", "cmnet"]
created: 2026-06-27T08:29:28.334Z
updated: 2026-06-27T08:29:28.334Z
sources: []
links: []
category: debugging
confidence: high
schemaVersion: 1
---

# DOTA browser plugin network state investigation

Focused test: `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`. Current test can pass while the final 60s observation has no assertion, so stdout/PPM evidence is required to prove network success.

2026-06-27 evidence from `VMRP_E2E_KEEP_TMP=1 ... --reporter=verbose`, temp dir `/tmp/skyengine-e2e-uVkuCx`:
- Download stage calls `my_initNetwork(NULL, "cmwap")`, connects to mapped `spd.skymobiapp.com -> 159.75.119.124` on port `6009`, sends `POST /simpleDownload`, receives `HTTP/1.1 200 OK`, and writes `mythroad/plugins/embrw.mrp`.
- Browser request stage calls `my_initNetwork(NULL, "CMNET")`, resolves `spd.skymobiapp.com -> 159.75.119.124`, starts async connect to `159.75.119.124:80`, and `my_connectSync()` logs success.
- Despite successful host `connect()`, `my_getSocketState(2..5)` repeatedly returns `2` (`MR_WAITING`) for roughly the 60s observation. No `my_send` happens in this stage.

Root cause: `src/network.c` stored network mode in process-global `isCMWAP`. `my_initNetwork()` set it true on `cmwap` but did not reset it when later initialized as `CMNET`. `my_connectAsync()` only copied `realState` to visible `state` when `!isCMWAP`; therefore a later CMNET async connect could succeed while visible state remained `MR_WAITING`. This is a generic network state-machine bug, not an IP reachability problem and not DOTA-specific.

Disassembly evidence from `build/skyengine` before the fix:
- `my_initNetwork` at `0x22000` only wrote `isCMWAP=1` on matching `"cmwap"` and had no clear path for `"CMNET"`.
- `my_connectAsync` at `0x21c50` wrote `realState`, then checked global `isCMWAP`; when nonzero it skipped writing visible `state` and only set `cmwapProxyAck` on success.
- `my_getSocketState` at `0x21da0` returned `mSocket.state`, so the ARM side kept seeing `MR_WAITING`.

Fix:
- Add per-socket `cmwapMode`, copied from the active mode when `my_socket()` creates the socket.
- Replace active mode in `my_initNetwork()` for every call, so `"CMNET"` clears the active CMWAP mode for future sockets.
- Use per-socket `cmwapMode` in async connect, CMWAP proxy connect, and send paths. Existing CMWAP sockets keep proxy behavior; later CMNET sockets expose successful async connect as `MR_SUCCESS`.
- Remove the unsafe diagnostic `printf("[my_send] data: %s", buf)` because `mr_send` buffers are length-based and not guaranteed NUL-terminated.

Verification after the fix:
- `cmake --build build --target skyengine -j2` passed with existing warnings.
- `pnpm exec tsc --noEmit --pretty false` passed.
- `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed with a new stdout assertion.
- `pnpm vitest run test/e2e/dota/download-plugin.test.ts --reporter=verbose` passed, 2 tests.
- `pnpm vitest run test/e2e/gghjt/download-plugin.test.ts --reporter=verbose` passed, 5 tests.
- Post-fix stdout from `/tmp/skyengine-e2e-uK13zI` showed `my_initNetwork(..., 'CMNET')`, async `my_connect('159.75.119.124', 80)` success, `my_getSocketState(2): 0`, `[my_send] cmwap off.`, `my_send(...): sent=575`, and `my_recv data: [HTTP/1.1 404 Not Found...]`. This proves connection success and request/response progress; the 404 is a server application response, not TCP connection failure.

Constraints retained: no xvfb, avoid full trace, no app-name/file-name branch, add explanatory comments to code edits, use PPM/stdout and disassembly/symbol evidence for verification.
