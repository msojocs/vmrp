---
title: "DOTA browser plugin second simpleDownload trigger"
tags: ["dota", "browser-plugin", "simpleDownload", "network", "frame-ext", "reverse-engineering"]
created: 2026-06-27T09:11:15.121Z
updated: 2026-06-27T09:24:55.248Z
sources: []
links: []
category: debugging
confidence: medium
schemaVersion: 1
---

# DOTA browser plugin second simpleDownload trigger

2026-06-27 follow-up for `pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功"`. Fresh preserved stdout `/tmp/skyengine-e2e-GLjzVe`, `/tmp/skyengine-e2e-aj82ck`, and sendto strace `/tmp/skyengine-dota-simpledownload-sendto.strace` show the final 60s window does contain a second `POST /simpleDownload`. Trigger chain: the test deletes runtime `mythroad/plugins/embrw.mrp`; line 158 LEFT_SOFT downloads outer browser plugin; first `/simpleDownload` TLV tag `0x29CE=480004 (0x75304)`, matching `embrw.mrp`, returns 200 and installs it. After line 166 confirms download result, `embrw.mrp` starts and its `frame.ext` shell becomes visible. Line 180 LEFT_SOFT in that `frame.ext` UI triggers CMNET: DNS `spd.skymobiapp.com`, `mr_connect(..., port=80, async)`, `my_getSocketState(...): 0`, `[my_send] cmwap off.`, then a second `POST /simpleDownload` with Host `spd.skymobiapp.com:6009` and TLV `0x29CE=700015 (0xAAE6F)`, returning HTTP 404 from the server. Static evidence: `/tmp/skyengine-embrw-extract/frame.ext` contains `CMWAP` at offset `0x24f0`, `CMNET` at `0x24f8`, and browser config/component strings `brw/vercfg.bin`, `brw/upd/vercfg.bin`, `brw/wvercfg.bin`, `brw/brw2.cfg`, `plugins/embrw.mrp`; disassembly around adjusted `0x2c9200` chooses CMWAP when `r0==1`, otherwise CMNET. Runtime `mythroad/plugins` only has `embrw.mrp` and `netpay.mrp`; lower browser packages such as `brwmain.mrp`, `brwcore.mrp`, and `brwshell.mrp` are absent from the test workdir. Conclusion: the last-60s simpleDownload is directly triggered by LEFT_SOFT inside the started `frame.ext` browser shell, likely for missing lower browser component/config appid 700015. `connect:80` is a guest/plugin parameter to `mr_connect`; VMRP does not rewrite Host `:6009` to port 80 in CMNET mode, because Host parsing only occurs for CMWAP sockets. Do not fix by reconnecting CMNET sockets based on HTTP Host; investigate/provide the requested browser component/config instead.

---

## Update (2026-06-27T09:19:00.469Z)

2026-06-27 17:16 revalidation: current-state command `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/dota/download-plugin.test.ts -t "下载浏览器插件 - 成功" --reporter=verbose` passed with `1 passed | 1 skipped`; target test took `83299ms`. New preserved directory `/tmp/skyengine-e2e-i2ymhU` shows the same trigger chain: first `/simpleDownload` is CMWAP, connects `159.75.119.124:6009`, returns HTTP 200; final 60s second `/simpleDownload` begins after CMNET init, DNS `spd.skymobiapp.com`, async `mr_connect` to `159.75.119.124:80`, socket state `0`, `[my_send] cmwap off.`, HTTP header `Host: spd.skymobiapp.com:6009`, returns HTTP 404. PPM evidence: `/tmp/skyengine-e2e-i2ymhU/start-browser.ppm` and `screen.ppm` are 240x320 raw pixmaps with nonzero pixel bytes, so this is not a black-screen/crash path. Conclusion remains: line 180 LEFT_SOFT inside started `frame.ext` triggers the CMNET lower-component/config download request, likely appid 700015; not a CMNET Host-port rewrite bug in VMRP.

---

## Update (2026-06-27T09:22:28.614Z)

Additional reverse-disassembly boundary: `LEFT_SOFT` maps to platform `MR_KEY_SOFTLEFT`, enum value 17 (`src/e2e_control.c` -> `src/main.c` -> `src/include/types.h`). `frame.ext` is an MRPGCMAP mixed ARM/Thumb child, so only known Thumb regions should be sliced. In the Thumb handler at adjusted `0x2c8658`, `0x2c8736..0x2c873c` compares the input key against 20 and 17; key 17 is left soft. On match, `0x2c873e..0x2c8752` reads selection byte `[r4+7]`, calls `0x2c93c0` to save it, calls `0x2c9044` to redraw, then calls `0x2c9200` with that selection as `r0`. `0x2c9200` selects `CMWAP` only when `r0 == 1`, otherwise `CMNET`; current stdout proves the line-180 path takes the CMNET branch. This upgrades the trigger evidence from timing-only to function-level: left soft/confirm in `frame.ext` directly reaches the network mode selector. Limit remains: the exact `700015` resource identity is still an inference from runtime TLV and missing lower browser files; `simpleDownload` and `700015` are not found as plaintext in `frame.ext`/`brw.ext`.

---

## Update (2026-06-27T09:24:55.248Z)

Correction/refinement of frame.ext key path: in the `0x2c8658` Thumb handler, `r0 == 0` handles key-down navigation (`r1 == 12/13`, up/down) and updates selection byte `[r4+7]`; that is selection movement, not the final network trigger. The actual confirm trigger is the `r0 == 1` branch: `0x2c8736..0x2c873c` compares `r1 == 20` or `r1 == 17`; 17 is `MR_KEY_SOFTLEFT`. On match, `0x2c873e..0x2c8752` reads selection byte `[r4+7]`, saves it through `0x2c93c0`, redraws through `0x2c9044`, and calls `0x2c9200(selection)`. Current stdout proves this selected the CMNET branch. `0x2c9200` then calls the initNetwork-like `0x2c9a48`; if return value is 2 it registers a 20000ms retry callback around `0x2c84d0`, which calls `0x2c9260` and `0x2c9200` again.
