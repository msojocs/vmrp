# WBRW Proxy Implementation Progress

## Goal
Implement proxy server (tool/proxy.js) so that WBRW browser can load web pages via proxy2 protocol.
Test: `pnpm vitest run test/e2e/wbrw/proxy.test.ts`

## Architecture
- WBRW uses CMWAP mode: all HTTP requests go through proxy2.51mrp.com (DNS-mapped to 127.0.0.1)
- proxy2 uses a binary packet format: magic(0x8aed9cf3) + header + TLV fields + payload
- tool/server-http.js listens on port 80, delegates to tool/proxy.js
- proxy.js already handles /sta (stats) and /page2 (page download) endpoints

## Test Expectations
- Cache file must contain both `http://wap.baidu.com` and `百度`
- Rendered page must differ significantly from home page
- Pixel checks: the loaded URL bar has yellow background `[248,240,168]`
  and gray text `[96,100,96]`

## Current Status
- [x] Phase 1: Run test, examine proxy2 request packets
- [x] Phase 2: Disassemble WBRW to understand proxy2 request/response packet format
- [x] Phase 3: Implement renderable target-page payload (full string-pool rebuild)
- [x] Phase 4: Verify by PPM screenshot, not cache alone
- [x] Phase 5: Verify focused and compatibility e2e tests (30/30 passed, 306s)

## Progress Log

### 2026-07-10: 第一个子链接仍不可激活，重新建立基线
- 当前 `HEAD=178e873`、工作树干净时重新运行
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/proxy.test.ts --reporter=verbose`，
  用例在 76.28s 后失败于 `hasSubLinkCache(...) === false`。保留画面目录为
  `/tmp/vmrp-e2e-pEC0S3`，独立工作区为 `/tmp/vmrp-ws-U8eTpD`。
- PPM 人工检查：`loaded.ppm` 已完整显示目标首页；`focus.ppm` 的焦点框画在第一个链接
  `mynes [顶]`；短按回车后，超时保存的 `sublink.ppm` 仍是同一首页，只是时钟变化。
  因此不是测试缓存判据误报，页面确实没有导航。
- 请求时间线澄清：`_page2_1783670894111.bin`（350B）是启动更新检查；
  `_page2_1783670896145.bin`（179B）才是地址栏首次加载（tag1 为空、tag2=
  `mrp.gddhy.net`、tag3=inputaddr）。点击后没有第三个 `/page2`，故故障发生在浏览器
  焦点/激活阶段，尚未进入代理抓取子页面阶段。
- 首页缓存 `/tmp/vmrp-ws-U8eTpD/mythroad/brw/http/cache3/14352.dat` 为 2602B，
  池和记录流边界均可完整走到 EOF；首个链接记录为 row `0x4E` -> caption `0x38`
  (`mynes [顶]`) -> href `0x54(e2=3)` (`.../sky_SaiNes-v1010`)。数据本身包含正确 URL。
- 将页头标题从 `0x38` 改成 `0x54` 并未解决问题。当前更强的嫌疑是：生成器把所有
  文本/链接行放在一个共享内容节下，并给纯文本行也创建独立 `0x4E` 行节点；渲染层的
  几何焦点会跳过纯文本 `0x54`，但激活路径保存的 DOM 指针仍落后一行。下一步用
  `game.ext` 的 focus view `+0x54`/selected DOM 传递链与真机块结构核实，再试验不共享
  内容节的逐块记录结构；不能用测试多按一次 DOWN 或复制首链接 URL 的补偿办法。

### 2026-07-10: 根因修复与 PPM 验收
- 真机 10 个 `.sky` 样本共解码 320 个 `0x38` 锚点，全部证明链接目标来自
  `pool[captionIndex-1]`。旧生成器却排成 `caption, href`，所以首个 `mynes`
  锚点前一槽是纯文本 filler，回车无动作；第二个 `RX管理器` 锚点前一槽恰好是
  `mynes` href，造成历史上“框在下一行、打开上一行”的假象。反汇编同时证明
  selected view/DOM 没有减一，旧的焦点错位解释被推翻。
- `tool/proxy.js` 现按自包含的 `href, caption, companion` 三槽生成每个链接，
  保留已验证的 `4E -> 38(caption) -> 54(companion)` 记录形状；没有复制首 URL、
  多按一次方向键或站点特判。测试解析器同步改为读取 `pool[captionIndex-1]`，并将
  记录流起点从错误的 `poolEnd+18` 修正为 13B 信封 + 7B 状态节 = `poolEnd+20`。
- 离线生成包为 2720B、payload 2698B、pool 86 项；记录流 87 条精确走到 EOF。
  首锚点 `captionIndex=9`，`pool[8]` 为
  `http://mrp.gddhy.net/mrp/sky_SaiNes-v1010`，`pool[9]` 为 `mynes [顶]`。
- 聚焦 E2E 通过：`1 passed`，17.41s；保留目录 `/tmp/vmrp-e2e-0f6UxK`，
  工作区 `/tmp/vmrp-ws-bFGMJ0`。第三个 `/page2` 包
  `_page2_1783672497346.bin` 的 tag1=`/mrp/sky_SaiNes-v1010`、
  tag2=`mrp.gddhy.net`、tag3=`http://mrp.gddhy.net/`。
- 缓存证据：`14327.dat` 是首页（2698B，`第1页 - WAP下载站`）；
  `16207.dat` 是目标子页（1630B，`mynes - WAP下载站`）。
- PPM 人工检查：`loaded.ppm` 是无模板残留/表单的首页，`focus.ppm` 框在
  `mynes`，`sublink.ppm` 完整显示应用详情，无“数据请求”或图片加载提示。
  子页相对首页 top/body/full diff 分别为 3121/8793/13198 像素，远高于等待门槛。
- 最终回归：`pnpm exec tsc --noEmit` 通过；`pnpm vitest run test/e2e/wbrw`
  报告 3 files / 3 tests 全部通过（boot 5.64s、paste 7.66s、proxy 18.67s）。
  逐锚点离线审计：首页 24 个、子页 6 个 `0x38` 的前一池槽均为 HTTP(S) URL，
  两页记录流都精确结束于 EOF。

### 2026-07-08: Initial Analysis
- Read proxy.js, server-http.js, proxy.test.ts, network.c
- proxy2 packet format: 4B magic + 2B version + 2B headerLen + 2B code + 2B status + 4B payloadLen + 1B fieldCount + fields + payload
- WBRW sends POST /page2 with proxy2 binary packet containing target URL
- Current /page2 handler returns mphome.sky (homepage content) - need to handle URL requests

### 2026-07-09: Proxy Implementation
- proxy2 request fields: tag1=path, tag2=host, tag3=referrer
- Added HTTP fetch in proxy.js: extractProxy2RequestUrl + fetchUrl + simplifyHtml
- WBRW recv buffer is exactly `len` bytes (2048); FIONREAD expansion causes buffer overflow crash
- WAP gateway proxy must simplify HTML to fit in ~1900 bytes (proxy2MaxPayloadBytes=7000)
- WBRW crashes on 79KB response (R0=ASCII data treated as pointer); 3906B update check also overflows with FIONREAD
- Solution: No FIONREAD expansion; proxy does WAP-style HTML simplification (strip JS/CSS, extract text+links)
- Cache verified: cache3/14175.dat contains simplified HTML with "百度一下" + "http://wap.baidu.com/"
- URL bar renders with yellow [248,240,168] background and gray [96,100,96] text
- Pixel positions differ from original test expectations (text at x=66+ not x=41)

### 2026-07-09: Current-State Revalidation
- Confirmed `tool/server-http.js` is listening on port 80.
- Re-ran `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/proxy.test.ts --reporter=verbose`;
  the focused test can pass while the preserved screenshot still shows a page
  load error.
- Latest contradictory artifact `/tmp/vmrp-e2e-WzdETQ` and workspace
  `/tmp/vmrp-ws-75t8yV`: `cache3/13998.dat` is 477 bytes and contains
  `<title>百度一下</title>`, `http://wap.baidu.com/`, and multiple `百度`
  strings, but `/tmp/vmrp-e2e-WzdETQ/loaded.png` visibly shows title-bar text
  `获取页面错误32` and the built-in home page body. Cache presence is therefore
  not sufficient evidence of success.
- Reverse-engineering evidence from extracted `game.ext`: proxy-related strings
  include `/page2`, `/image`, `/sta`, `proxy2.51mrp.com`, `POST`, `Host: `,
  `%s/cache3`, and `http://web.proxy.51mrp.com/?s=inputaddr`; the live `/page2`
  packet body confirms tag2=`wap.baidu.com` and tag3=input-address referrer.
- Deeper disassembly check on extracted `test/fixtures/wbrw.mrp`:
  `game.ext+0x4DB40` contains `8a ed 9c f3`, `game.ext+0x4DC04` is `/page2`,
  `game.ext+0x4DC0C` is `/image`, `game.ext+0x4DC24` is `/sta`,
  `game.ext+0x4DC6C` is `proxy2.51mrp.com`, `game.ext+0x4DCD8` is
  `application/octet-stream`, `game.ext+0x4E198` is `>HHHHIB`, and
  `game.ext+0x4E1A0` is `>HH`.
- The endpoint selector at `game.ext+0x21640` maps type 0 to `/page2`, and
  request construction around `game.ext+0x211CC` writes proxy2 magic/version
  plus tag 1 path (`0x21286`), tag 2 host/domain (`0x2131C`), and optional
  tag 3 referrer (`0x213F6`). The TLV writer at `game.ext+0x26E9C` stores
  `tag`, `length`, and `valuePtr`; serializer `game.ext+0x26F78` writes the
  big-endian fixed header and each `>HH` field.
- Tightened `tool/proxy.js` URL construction so tag1 absolute URLs and origin-form
  paths are normalized with `URL`; redirects now use `new URL(location, base)`.
- Current disassembly conclusion: the proxy2 packet parser accepts the response
  and extracts tag 33, but the later page renderer rejects the cached payload
  with error 32. This points at the payload format, not at DNS, HTTP fetch, or
  cache writes.
- The response builder at `game.ext+0x21690` writes tag 33 and tag 8 for proxy2
  packets. The target-page response path at `game.ext+0x41C60` requires
  `status=200` and tag 33 byte value `3` before it opens the cached object for
  rendering. Returning raw simplified HTML with tag 33 = 3 writes the cache but
  is not renderable as WBRW's `.sky` page format.
- Next implementation target: convert fetched HTML text into a minimal valid
  WBRW `.sky` payload or an equivalently supported renderable format, then prove
  success from the PPM screenshot.

### 2026-07-09 (later): Full string-pool rebuild — target page renders cleanly
- Root cause of stray content ("我的磨" etc.): the previous in-place slot
  rewrite of template `32953109.sky` only touched ~18 of 115 pool entries; the
  remaining ~35 visible UCS-2BE captions (我的磨/我的磨2/百度网盘/上一页/下一页/
  ©Powered by 彩虹! …) were still bound by the display list and rendered.
- Fix in `tool/proxy.js` (`buildSkyPageFromHtml` rewrite):
  - `parseSkyContainer`: header(10B) + u16be length-prefixed pool walk + DL.
  - `classifySkyEntry`: ascii / utf8 / ucs2 / bin. Order matters: strict UTF-8
    decode that also rejects control chars (UCS-2BE ASCII like `\0Z\0I\0P` is
    valid UTF-8 bytes but has NULs), then UCS-2BE check that rejects
    control/surrogate/PUA units (UTF-8 CJK bytes read as u16be land in PUA).
  - Rebuild: every visible text slot is fed sequentially from
    `extractHtmlPageModel` (title first), exhausted slots become empty strings;
    each slot keeps its original encoding (utf8 vs ucs2be). ASCII link-shaped
    entries (`http(s)://`, `?…`, `/…`) all point at the request URL. `./` asset
    refs (base + logo.png) must be kept verbatim — replacing them breaks the
    page's own yellow title-bar header element (verified: yellow pixels
    3575→0→restored).
  - Pool re-emitted with new u16be lengths; `hdr[8..10] = poolEnd-10` patched;
    DL copied verbatim (its §4a `A` length field only depends on DL size, so no
    DL patch is needed).
- Empirical invariants validated offline before each run: pool walk ends at
  poolEnd, `poolEnd+5+A == filesize`, DL byte-identical, no residual template
  strings, cache-required substrings present.
- Entity handling: added &copy;/&reg;/&middot;/&mdash;/&hellip;, decode
  numeric refs without trailing `;` (mobile icon-font markup `&#xe750`), strip
  PUA code points U+E000–U+F8FF (no glyphs on device).
- Template default switched to `temp/brw/http/cache3/179603.sky` (plain
  text+links list skeleton, no form widgets) — with `32953109.sky` the search
  form input/dropdown/button remained as irrelevant chrome.
- Test target switched to `http://mrp.gddhy.net/` (user request; simple stable
  WAP page, title `第1页 - WAP下载站`). Test glyph-pixel assertions updated:
  after-paste (107,17) black/(108,17) white; foreground (41,10) yellow
  [248,240,168] + (79,10) gray [96,100,96] (stable stroke run x=77..84 at y=10).
- PPM-verified result (`/tmp/vmrp-e2e-atstsl/loaded.ppm`): yellow title bar
  with `第1页 - WAP下载站`, URL link line, body = 软件列表/mynes/RX管理器/… all
  from the target site; no forms, no template residue. Cache `.dat` contains
  both `http://mrp.gddhy.net/` and UTF-8 `WAP下载站`. Focused test passes.

### 2026-07-09 (evening): From-scratch .sky generation — no template file at all
- User request: stop reading template files; generate from format knowledge.
- Disassembly (subagents, game.ext): DL record framing
  `[a:be16][flag:be16][flag&0x8000→e1][flag&0x4000→e2][op:u8][len:u8][payload]`
  (fill 0xE582, dispatch 0xDC60); pool resolution = lazy walk-table (0xA470);
  A field consumed as BE32 node-region bound; visible text decoded UCS-2BE
  unconditionally (0x1230C); full spec in docs/wbrw-sky-format.md §4b-3/§4b-4/§5b.
- `tool/proxy.js`: template path (readWebSkyTemplate/parseSkyContainer/
  classifySkyEntry/buildSkyPageFromHtml/simplifyHtml) replaced by
  `generateSkyPage` + `generateSkyDisplayList` + `skyRecord` — pure programmatic
  emission of header/pool/DL. `extractHtmlPageModel` now returns
  `{title, items:[{text, href}]}` with real resolved hrefs.
- Iteration findings (each verified by PPM screenshot, ~75s per e2e run):
  - v1 (records only): payload cached byte-identical but browser stayed on home
    page — records were consumed without emitting nodes. Root cause: record
    stream must be led by the `0F <X:be32> 00 00` state token (switches parser
    into the record-dispatch state).
  - v2 (with 0F token): page rendered, but (a) title bar stuck at "加载图片0/1"
    because the emitted logo image element never finishes loading locally, and
    (b) all items flowed inline in one paragraph.
  - v3 (final): no image element; one `4E` child node per item row → proper
    line-per-item link list. Screenshot: yellow title bar `第1页 - WAP下载站`,
    URL line, one row per item, zero irrelevant elements — cleaner than the
    template version. Focused test passes; offline invariants (pool walk, A/C,
    X, record walk) all byte-exact.
- Truncation fix (user report): the page cut off after `冒泡娱乐城 [顶]` — that
  was the legacy 12-line cap in `extractHtmlPageModel` (from the ~1900-byte
  simplified-HTML era), not a renderer limit. Cap raised to 64 (runaway guard
  only); the real bound is the byte-budget trim loop in `gener

### 2026-07-09 (night): Sub-link click navigation — investigation
- New task: clicking a sub-link on the rendered target page hangs at the
  "数据请求" progress stage (user report).
- Test now clicks a sub-link: after the target page loads, `DOWN` moves focus
  (lands on the LAST visible link row, `听听音阅 [顶]` → `/mrp/sky_opttyy`),
  not the first one.
- First repro round (screenshots `/tmp/vmrp-e2e-tkIlrI`): `ENTER` did NOT
  navigate — it opened the link long-press context menu (复制/新窗口打开/快速搜索/
  书签/…, no plain "打开" item). Cause: the test sets `VMRP_E2E_HOLD_MS=1500`
  for paste stability, and vmrp holds every injected key for that long, so
  ENTER registers as a long-press. Packet dumps confirm NO third `/page2`
  request reached the proxy in that round.
- Navigation path chosen: link menu item `2. 新窗口打开` via digit key "2"
  (menu items are digit-selectable), which matches the user's manual flow.
- Second repro round (`/tmp/vmrp-e2e-WjYiU6`, packets `_page2_1783593791382.bin`):
  menu item 2 ("新窗口打开") DID navigate. The click request is structurally
  identical to first load — same endpoint `/page2`, same header `code=1`
  (packet-level; the internal request-type code=2 selects the /page2 endpoint),
  tag1=`/mrp/new_Sdilaov3003guoguanleiji` (origin-form path), tag2=
  `mrp.gddhy.net`, tag3=`http://mrp.gddhy.net/` (referrer = current page URL,
  replacing first load's `http://web.proxy.51mrp.com/?s=inputaddr`).
  The existing `extractProxy2RequestUrl`/`buildProxy2TargetUrl`/`generateSkyPage`
  path handled it with **zero proxy changes**: cache gained
  `20652.dat` = `http://mrp.gddhy.net/mrp/new_Sdilaov3003guoguanleiji` titled
  `S地牢-斗圣 - WAP下载站`, and `sublink.ppm` shows the fully rendered sub page
  (title bar `S地牢-斗圣 - WAP下载站`, body 应用程序详细信息/应用名称/版本/…).
  **No "数据请求" stall exists on the /page2 path** — the original hang was the
  long-press menu swallowing the ENTER (no request ever sent).

### 2026-07-09 (night): Disassembly — progress state machine + link-click path
(game.ext, subagent-verified; file offsets)
- UI string table: base `0x4b4a0`, u16le offset table `0x4c9da` (389 entries).
  Progress captions: 74=数据请求… 75=图片请求… 76=接收数据 77=接收图片
  78=获取页面超时 79=获取页面错误 80=网络连接错误 82=等待响应….
- `browser_post_stage(ctx, code, stringIndex)` @ `0x1e6a4` posts the title-bar
  caption; driver `browser_on_net_event` @ `0x410a4` switches on socket-event
  type `[event+14]`. The caption leaves "数据请求" ONLY on a recv/response
  event (recv cb `0x1fe22`) or when the proxy2 response handler `0x41C60`
  reaches its completion callback. **There is no independent timeout timer** —
  a proxy that never writes a valid proxy2 envelope stalls the UI forever.
- `0x41C60` error codes (r5): 0=ok 2=empty-body 8=cache-open-fail
  50=doc-build-fail 51=len-mismatch 74=envelope gate (status!=200 || tag33!=3).
  Numerically distinct namespace from string indices (74 collision is
  coincidence).
- ONE request builder `0x211CC` / high-level send `0x1EDF8` serves both
  address-bar loads and link clicks (view state machine `0x1E6D0`, navigate at
  state 4 `0x1E8B8`). URL splitter `0x5118`: host→tag2, path(with leading `/`
  and query)→tag1. tag7/tag8 only for POST-with-body; no request-id matching
  in the response path. `If-Modified-Since`/`Range` strings belong to a
  separate raw-HTTP transport (parser `0x1fc40`) used for downloads, not /page2.
- Hardening added from this analysis (`tool/proxy.js`): fetch errors on
  `/page2` and `/image` now return a **valid proxy2 packet with status=404**
  (empty payload) instead of `text/plain retcode=-1` — a non-envelope reply
  cannot be parsed by the record layer and would stall the caption at
  "数据请求"; a 404 envelope drives `0x41C60`'s gate (r5=74) → 获取页面错误.
  `makeProxy2ResponsePacket` gained an optional `status` parameter.

### 2026-07-09 (night): E2E per-key hold override
- Root cause of the test-only navigation failure: `VMRP_E2E_HOLD_MS=1500`
  (needed for clipboard paste stability) makes EVERY injected key a long-press;
  WBRW treats a held confirm key as "open link menu" and a held DOWN as key
  repeat (focus scrolled past the target row: 长按DOWN focus landed on 上一页).
  [CORRECTED, see late-night section] Early rounds appeared to show that short
  ENTER (120/500ms) doesn't navigate — that was actually the focus off-by-one
  quirk (real focus was on a plain-text row). Short ENTER (120ms) DOES
  navigate when the real focus row is a link; digit 5 opens the context menu.
- `src/e2e_control.c`: `KEY <name> [hold_ms]` now accepts an optional per-press
  hold override (falls back to VMRP_E2E_HOLD_MS/500); `vmrp-e2e.ts` `key()`
  gained a `holdMs` parameter. This is generic input-injection plumbing, not
  app-specific logic.
- Sub-link assertion generalized: cache3 `.dat` pool head is parsed
  (URL@10+u16len, title u16len-prefixed UTF-8 right after) and the test accepts
  any same-site page other than the home page whose title ends with
  `- WAP下载站` — robust against which link row focus lands on.

### 2026-07-09 (late night): Sub-link click — RESOLVED, test green
- Interaction semantics (user-confirmed correction): **short ENTER = 确定 =
  open the focused link directly; digit 5 = open the context menu**; a long
  ENTER (>=1500ms) also opens the link context menu. The earlier conclusion
  "120/500ms short press does not navigate" was wrong — those rounds pressed
  ENTER while the REAL focus was on a plain-text row (see next point).
- Focus off-by-one quirk (multiple packet-correlated repro rounds): the row
  the app acts on is the one ABOVE the visible focus ring.
  - Round WjYiU6 (global hold 1500ms, DOWN auto-repeated): visible ring on
    首页, menu item 2 opened S地牢-斗圣 (the row above) — navigation worked.
  - Round hdH4RS (single short DOWN): visible ring on mynes (first link), the
    row above is the plain-text 软件列表 → no link target, ENTER/menu did
    nothing (no /page2 sent; confirmed via stdout my_send log).
  - Round mCh5hh/bDO9Yu (2× short DOWN): visible ring on RX管理器, real target
    = mynes → short ENTER navigated directly.
- Final test sequence: 2× short DOWN (120ms each, one row per press) →
  **short ENTER (120ms) navigates directly** (no menu needed). The
  long-press-menu + digit-2 path also works but is unnecessary.
- Result (`/tmp/vmrp-e2e-bDO9Yu`, PASSED with tightened assertions): third
  /page2 request `tag1=/mrp/sky_SaiNes-v1010 tag2=mrp.gddhy.net
  tag3=http://mrp.gddhy.net/`; cache `17327.dat` =
  `http://mrp.gddhy.net/mrp/sky_SaiNes-v1010` titled `mynes - WAP下载站`;
  `sublink.png` shows the fully rendered sub page (title bar
  `mynes - WAP下载站`, body 应用程序详细信息/应用名称/版本/应用ID/开发者/
  分辨率/平台/大小/应用介绍) — no "数据请求" stall.
- Test assertion hardening: `waitForSubLinkPage` additionally requires the
  body region (y=30..280) to differ from the home page by >3000 pixels —
  title-bar-only checks can be fooled by the transient "数据请求..." progress
  caption while the body still shows the old page.
- Proxy needed no URL-handling changes for sub-links; the only proxy change is
  the fetch-error hardening (valid proxy2 status=404 envelope instead of
  text/plain), which prevents genuine stalls when the upstream fetch fails.
