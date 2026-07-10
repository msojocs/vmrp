# WBRW Proxy Implementation Progress

## Goal
Implement proxy server (tool/proxy.js) so that WBRW browser can load web pages via proxy2 protocol.
Test: `pnpm vitest run test/e2e/wbrw/proxy.test.ts`

## Architecture
- WBRW sends its browser traffic to proxy2.51mrp.com (DNS-mapped to 127.0.0.1);
  the passing emulator run initialized the socket layer as CMNET, so proxy2 is
  an application protocol choice rather than proof that the socket uses CMWAP
- proxy2 uses a binary packet format: magic(0x8aed9cf3) + header + TLV fields + payload
- tool/server-http.js listens on port 80, delegates to tool/proxy.js
- proxy.js already handles /sta (stats) and /page2 (page download) endpoints

## Test Expectations
- Cache must contain `http://mrp.gddhy.net` and the `WAP下载站` title
- The exact detail page `/mrp/sky_SaiNes-v1010/` must compile the exact
  `sky_SaiNes-v1010.mrp` URL into a native `skyscript:` download action
- Completion requires the final `.mrp`, matching MRP header/actual lengths,
  `dllist1`, a 4-byte `dwnlist.dat`, and manually inspected PPM evidence
- Rendered page must differ significantly from home page
- Pixel checks: the loaded URL bar has yellow background `[248,240,168]`
  and gray text `[96,100,96]`

## Current Status
- [x] Phase 1: Run test, examine proxy2 request packets
- [x] Phase 2: Disassemble WBRW to understand proxy2 request/response packet format
- [x] Phase 3: Implement renderable target-page payload (full string-pool rebuild)
- [x] Phase 4: Verify by PPM screenshot, not cache alone
- [x] Phase 5: Verify focused and compatibility e2e tests (30/30 passed, 306s)
- [x] Phase 6: Verify native MRP download, complete bytes, and download metadata
- [x] Phase 7: Harden generic range transport and rerun exact-target visual acceptance

## Progress Log

### 2026-07-11: 上游 Range/对象校验加固与当前工作树复验
- 完成审查发现旧 `/res` 每收到一个 1800 字节客户端范围，都会从上游重新缓冲
  完整对象；52,792 字节文件因此产生 30 次全量抓取。现改为上游
  `Range: bytes=start-end` + `Accept-Encoding: identity`，严格要求 HTTP 206、
  校验 `Content-Range` 起止/总长与实际 body 长度，并把单次 body 硬限制为 1800
  字节。上游忽略 Range、对象越界或元数据不一致均返回明确错误，不退回全量抓取。
- `server-http.js` 每请求热加载 `proxy.js`，因此用有 TTL/数量上限的进程级 Map
  仅保存强 ETag（无强 ETag 时 Last-Modified）和对象总长。后续范围携带
  `If-Range`；响应校验器或总长变化即中止。多段对象必须有可用校验器，续传必须
  找到已有状态；完成后的校验器保留至 TTL，使同 URL 并发下载也固定在同一版本。
- tag10 缺失仍表示合法首包 offset 0，但重复 tag10 或长度不是 8 的请求现在返回
  400，不再被静默当作从零重启。HTML→SKY 编译还会在一字节脚本池引用耗尽前
  按既有尾部预算策略裁剪，并把非法 URL 文件名等异步编译错误转换为 proxy2 502，
  防止不可信页面异常退出代理进程。截断的TLV包会继承解析错误并返回400，非法
  重定向 Location 也在异步回调内转为抓取错误。
- E2E 增加精确契约：详情页规范路径必须是 `/mrp/sky_SaiNes-v1010`，缓存 SKY
  必须包含完整目标下载 URL、文件名和 `application/sky-mrp`，最终发布文件名必须
  恰为 `sky_SaiNes-v1010.mrp`，不能由任意其他完整 MRP 使测试通过。
- 当前工作树在全部审查修复后重新运行目标 E2E 通过（49.60s），证据位于
  `/tmp/vmrp-e2e-3bKSD7` 与 `/tmp/vmrp-ws-K7oLWr`。人工检查 PPM 确认目标链接焦点、
  `0/未知 -> 1K/51K -> 22K/51K -> 49K/51K` 进度及“下载成功”提示。请求偏移仍为
  `0, 1800, ... 52200`，无 EOF 请求；最终文件为 52792 字节，`MRPG` 和 offset 8
  声明长度均正确，SHA-256 `adec80acc99e82a5122bef5fc9c6d600232cf6da51466e6cea4e6da48c9617ba`
  与同一时刻直接抓取的上游文件一致，`dllist1` 存在且 `dwnlist.dat` 为 4 字节。

### 2026-07-11: proxy2 范围协议完成并通过完整 MRP 验收
- `lib.download` 发出的 `/res` 首包没有 tag10；续包 tag10 长度为 8，按
  `>II` 解码后第一个值依次为 `1800, 3600, ... 52200`，即已落盘偏移。
  响应以 1800 字节分块，tag16 长度为 12，按 `>III` 表示
  `(rangeStart, rangeEnd, objectLength)`，等价于 `Content-Range`。
- 下载器要求响应 tag7 为 u16 类型枚举 22（`application/sky-mrp`）；缺少时
  `game.ext+0x175B4` 返回错误 140。tag8 不是固定对象总长，而是从本次请求起点
  计算的剩余长度 `objectLength - rangeStart`。固定传 52792 时，进度 UI 总量会随
  已下载字节从 `90K` 增至 `99K`，证明客户端把 tag8 加在当前落盘量之后。
- 最后一次有效请求从 52200 开始，响应 592 字节后客户端直接完成；没有 52792
  EOF 请求。曾尝试的空 200 EOF 包会删除已经完整的 `.mrp.dt` 并显示失败，因此
  `/res` 对 `rangeStart >= objectLength` 返回 416，不提供额外完成分支。
- 收紧后的 E2E 于 2026-07-11 运行通过（59.51s）。保留证据位于
  `/tmp/vmrp-e2e-TJWx2E` 和 `/tmp/vmrp-ws-9bFFtK`：焦点框确实位于
  `sky_SaiNes-v1010.mrp`；下载管理画面从 `0/未知` 依次增长到 `50K/51K`，随后
  显示“下载成功”。最终文件 `mythroad/sky_SaiNes-v1010.mrp` 为 52792 字节，
  以 `MRPG` 开头，头部 offset 8 声明长度同为 52792，SHA-256 为
  `adec80acc99e82a5122bef5fc9c6d600232cf6da51466e6cea4e6da48c9617ba`；与直接抓取
  上游文件完全一致。`dllist1` 已生成，`dwnlist.dat` 为最终 4 字节。

### 2026-07-11: 下载点击回首页的直接证据与待确认协议层
- 当前 `HEAD=3609435`、工作树干净。上一提交仅在 HTML→SKY 时把 `.mrp`
  href 标注为 `sky_download:<absolute-url>`，尚未修改 C 网络层，也未执行目标
  E2E 下载验收。
- 现存两次实际点击请求 `tool/temp/_page2_1783733845542.bin` 与
  `_page2_1783733960712.bin` 字节结构一致：客户端发送 `POST /page2`，tag1=
  `sky_download:http://mrp.gddhy.net/mrp-files/sky_SaiNes-v1010.mrp`，tag2 为空，
  tag3 为详情页 URL。这证明点击和下载前缀均已进入客户端请求层；故障不是焦点
  未激活，也不是上游站点把请求重定向到首页。
- `tool/proxy.js` 的 `/page2` 分支只在 tag2 非空时抓取目标；tag2 为空会无条件
  `readProxy2PageFixture()` 并返回 `mphome.sky`。该分支与“点击后回首页”的画面和
  请求一一对应，是当前直接根因。
- 仍需反汇编确认的下一层契约：`sky_download:` 请求为何仍由 `/page2` 发出，以及
  网关应返回何种字段/页面动作才能把浏览器切换到 raw HTTP 下载器。不能直接把
  MRP 字节包成 tag33=3；`game.ext+0x41C60` 会把它当 SKY 页面解析。
- 上游文件当前为 HTTP 200、`Content-Length: 52792`、
  `Content-Type: application/octet-stream`、正文 `MRPG`。下载器反汇编表明
  `Content-Disposition` 非必需，缺失时会从 URL basename 派生文件名。
- 当前测试在进入 `mynes` 详情页后即结束，没有点击第二个链接
  `sky_SaiNes-v1010.mrp`，也没有下载 UI PPM、下载列表或落盘文件断言；测试通过
  不能证明目标完成。最终至少检查 `download-focus.ppm`、激活/进度 PPM、
  `brw/download/dwnlist.dat` 与下载产物魔数。
- 用户指定的 `temp/brw/` 在当前仓库及 `/home/msojocs` 下均不存在，且 `temp`
  被 `.gitignore` 忽略；现阶段无法对照真机下载画面，只能继续以 `game.ext`
  反汇编、现存请求包和后续 PPM 为证据。

### 2026-07-11: 外层 302 证伪，改用页面内 `lib.download`
- 第一版特殊请求处理返回外层 `HTTP 302 Location`。目标请求确实到达
  `_page2_1783737479117.bin`，但 `/tmp/vmrp-e2e-Eeo8IJ/stdout.log` 只看到
  socket 15 收到 160B 的 302 后创建 socket 16，没有 raw GET；
  `download-activated-8.ppm` 明确显示“网络连接错误74”。这证明 proxy2 模式
  要求外层 200，不能复用 raw HTTP 模式的 301/302/303 跟随器。
- 焦点 PPM 同时校正了测试交互：详情页链接在首屏下方，DOWN 会先滚动正文；
  `/tmp/vmrp-e2e-Eeo8IJ/download-focus.ppm` 已人工确认蓝色焦点框位于
  `sky_SaiNes-v1010.mrp`，不是在线模拟器链接。
- 内置 `mphome.sky` 自带可复核的下载脚本：字符串池有 `lib.download`、下载 URL、
  文件名、`application/sky-mrp`、`FALSE`；脚本段为
  `01 <bodyLen:be32>` 信封，`d597` 定义函数、`de9b` 调库函数、`dfa8` 压入
  四个参数。页面的 `skyscript:download` 锚点证明该执行通道由浏览器原生支持。
- 当前实现方向改为在 HTML→SKY 编译时，把任意 `.mrp` HTTP(S) 链接转成唯一的
  `skyscript:downloadN` 锚点，并从上述反汇编格式生成
  `lib.download(url, filename, application/sky-mrp, FALSE)` 页面函数。该逻辑只按
  文件类型工作，不依赖站点、应用名或固定 URL；下一轮以 raw GET、下载 UI PPM
  和落盘文件验证脚本字节码是否完整。

### 2026-07-10: MRP 链接被当成二进制网页的根因与第一阶段修复
- 以当前工作树和手工运行留下的实际包为准：
  `tool/temp/_page2_1783674382410.bin` 的 tag1=
  `/mrp-files/sky_SaiNes-v1010.mrp`、tag2=`mrp.gddhy.net`。旧代理随后把
  52792 字节的 `application/octet-stream` MRP 无条件传给
  `generateSkyPage`，产生了 `build/mythroad/brw/http/cache3/67676.dat`
  这个 428 字节的二进制“网页”；下载列表的时间戳未变。
- 上游证据：详情页的真实下载 href 是
  `http://mrp.gddhy.net/mrp-files/sky_SaiNes-v1010.mrp`；响应为 200，
  `Content-Type: application/octet-stream`，`Content-Length: 52792`，文件头为
  `MRPG`。
- 反汇编证据：`game.ext` 0x30BC0 对链接前 13 字节与
  `sky_download:` 做精确比较，在相对 URL 解析后仍重新加回该前缀；
  0x4A150 的内容类型/扩展名表包含 `.mrp`，并与 0x4A01C 的
  `application/sky-mrp` 对应。因此下载必须在网关编译 HTML→SKY
  链接时标注，不是等 `/page2` 已经拿到文件字节后再判断。
- `tool/proxy.js` 已对解析后 pathname 以 `.mrp` 结尾的 HTTP(S)
  链接生成 `sky_download:<absolute-url>`。这是基于客户端协议类型的通用
  转换，不依赖应用名、站点域名或特定 URL。
- 当前尚未宣称完成：需先离线验证真实 `/page2` 响应的池槽，
  再根据 C 侧下载传输的实际问题决定修复；按约束，未修改 C 代码前
  不运行目标 E2E。最终必须同时有下载 UI PPM、下载列表/文件产物和
  网络请求证据。

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
