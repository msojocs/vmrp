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
- [x] Phase 8: Implement `/mrp` font transport and verify prompt/progress/success PPMs
- [x] Phase 9: Implement direct `/sd`, share the immutable MRP catalog, and verify the full chain
- [x] Phase 10: Fix nested table[125] package context and verify font installation/restart

## Progress Log

### 2026-07-11: 字体安装失败根因、修复与重启验收完成
- 深入反汇编 `brwgraphics.ext` 闭合了安装路径：`+0x7B8` 选择
  `brw/wfnt16c.mrp`、包内 `1008.uni` 和目标 `system/1008.uni`；`+0x24C4`
  把包名写到私有 module record 的 `table[100]`，再由同一 record 的
  `table[125]` 读取。`+0x7D0` 只有该读取失败才返回 -1，因此“安装失败”明确指向
  `_mr_readFile("1008.uni")` 失败，而不是目标文件 open/write 失败。
- 宿主 `aex_t125` 原先固定从主 `EXT_TABLE_ADDR` 读取 `[100]/[104]/[105]`，丢失了
  LR 所属嵌套模块的 ABI 表上下文，所以仍在外层 `wbrw.mrp` 查找 `1008.uni`。
  现通过 `arm_ext_resource_owner_for_lr` 选择 owner，从 `owner->file_addr[0]` 取得
  私有 record，并要求 `[0..105]` 是连续有效区间；存在 owner 但 record 损坏时明确
  失败，不回退到外层包。主模块无 owner 时仍直接使用主表。私有 record 取出的显式
  `brw/wfnt16c.mrp` 继续按现有优先级走 host-package 分支，不会被 child-resource 覆盖。
- 单元测试覆盖主表、私有 record 和损坏 record 不回退；`skyengine-unit` 为 104 checks
  全通过。
- E2E 使用 guest 固定连接的 `127.0.0.1:80` 服务，并保持 HTTP Host 为
  `proxy2.51mrp.com`；不增加端点改写参数。
- 最终命令 `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/proxy.test.ts
  --reporter=verbose` 在 41.08s 通过。首轮恰好 13 个 `/mrp` 偏移
  `0,1800,...,21600`；落盘 MRP 为 22,302 字节，SHA-256
  `98763f1c6bff5cdef6a3fd3c22e34168d0445359f9fdcb3c34bc381e85dbac3a`；安装产物
  `system/1008.uni` 为 34,272 字节，SHA-256
  `5e3515001810f6e25363a9152b4d449877aa24b38c17a9358b178af94c36e5e6`。
- 保留证据为 `/tmp/skyengine-e2e-sqyAuE`（下载/安装）、`/tmp/skyengine-e2e-NTbM4h`
  （同 workspace 重启）和 `/tmp/skyengine-ws-c4Dp1J`。人工检查 PPM：下载询问、
  `wfnt16c.mrp 1K/21K`、下载成功/立即安装均正确；安装确认后无失败提示；重启后
  `16号字体` 带选中圆点，设置中心显示 `字体: 16号字体`，且没有新增 `/mrp` 请求。
  全程未使用 xvfb 或大 trace。

### 2026-07-11: 安装失败复核重新开始
- 当前 `proxy.test.ts` 在下载提示后只等待 60 秒，没有确认下载、安装、落盘或最终
  PPM 断言；17:46 的成功文档早于 19:01 的测试改动，因此不能代表当前状态。
- 从 VS Code 本地历史恢复了曾验证下载链的分段/哈希断言，并继续补上“立即安装”
  确认。`wfnt16c.mrp` 索引和 `brwgraphics.ext` 字符串表共同证明该分支读取包内
  `1008.uni` 并写入 `system/1008.uni`；解包产物应为 34,272 字节，SHA-256 为
  `5e3515001810f6e25363a9152b4d449877aa24b38c17a9358b178af94c36e5e6`。
- 新验收从不存在的 `brw/wfnt16c.mrp` 与 `system/1008.uni` 开始，要求真实 13 包
  proxy2 下载、MRP 哈希、下载完成 PPM、安装结果 PPM和最终字体字节全部成立。

### 2026-07-11: `/sd` 直连接口与跨层联调完成
- 现有字体 E2E 已证明客户端的 proxy2 `/mrp` 分块契约，但 `/mrp` 直接读取本地
  MRP 快照；`dmrp.wapproxy.sky-mobi.com/sd` 还会落入通用 `helloworld`，因此尚未
  证明 `/sd` 接口本身。反汇编确认当前字体路径不会裸 GET `/sd`：URL 作为 tag1
  经 `POST /mrp` 发送，raw HTTP Range 是下载器的并列能力，不能混同为客户端契约。
- 实现范围确定为同一 appid 资源服务的两个入口：`/sd` 提供 GET/HEAD、单范围
  Range、强 ETag/If-Range 与明确 4xx/5xx；`/mrp` 继续输出 tag7/tag8/tag16 信封，
  两者共享进程内不可变 MRP 快照。联调已校验 `/sd` 全量/首中尾范围、`/mrp`
  13 包重组和最终安装文件三方逐字节及 SHA-256 一致，并再次人工检查四张 PPM。
- `/sd` 已在请求体聚合前按规范化 Host 精确分派，只接受 `/sd`、`type=2` 和唯一
  uint32 appid；文件仍只由 MRP header appid 唯一解析，不按文件名或路径兜底。
  GET 全量返回 200；单一 closed/open/suffix Range 返回 206；HEAD 返回完整元数据；
  强 ETag 来自快照 SHA-256，If-Range 不匹配时按标准忽略 Range 返回完整 200。
  非法参数/多范围、未知资源、非法方法和越界分别明确返回 400/404/405/416。
- 运行中 `server-http.js` 热加载时发现旧 `/mrp` 快照没有 ETag 元数据；共享入口会
  对这种已存在的同代 Buffer 补算哈希，避免不停服更新后写入 undefined 响应头。
  冷启动加载仍通过同一 fd 完整读取、读后 fstat/header 复验和容量上限约束。
- `server-http.js` 现在默认只绑定 `127.0.0.1`，避免仅凭可伪造 Host 从局域网读取
  本地 MRP；确需外部访问时可显式设置 `HOST=0.0.0.0`。已用临时 18080 端口确认
  监听地址和 `/sd` 全量响应，未中断用户现有的端口 80 进程。
- 直连验收得到 full `200/22302`，Range `0-1799`、`1800-3599`、
  `21600-22301` 分别为 `1800/1800/702` 字节，Content-Range 与 ETag 正确；
  unknown appid=404，越界=416 且带 `bytes */22302`，多范围=400。三个切片均与
  fixture 对应区间逐字节一致。
- 独立构造 proxy2 请求后，`/mrp` 仍严格返回 13 包，偏移
  `0,1800,...,21600`，每包 tag7=22、tag8=`22302-offset`、tag16 起止/总长
  和 payload 一致；重组体与 `/sd` full 和 fixture 三方相等，SHA-256 均为
  `98763f1c6bff5cdef6a3fd3c22e34168d0445359f9fdcb3c34bc381e85dbac3a`。
- 最终加固后的 E2E 在 23.91s 内通过，目录为 `/tmp/skyengine-e2e-ikTR0a` 和
  `/tmp/skyengine-ws-Ezi2zo`。测试先由浏览器完成 `/mrp`，避免 `/sd` 预热掩盖首包
  加载问题；并核对本轮恰好 13 个偏移，再验证第二个有效 appid 和完整 HTTP
  Range/错误矩阵。落盘 MRP 为 22302 字节且哈希准确；`dllist1` 为 1320
  字节，包含目标名、MIME 和完整 `/sd` URL，完成/总长均为 22302；
  `dwnlist.dat` 精确为 `010d0101` 且无 `.dt`。人工检查四张 PPM 确认 16 号字体焦点、
  下载询问、`1K/21K` 进度以及下载成功/立即安装提示。

### 2026-07-11: 字体 `/mrp` 协议反汇编、实现与 PPM 验收完成
- 字体选择是独立于网页脚本的通用下载入口：`game.ext+0x3A768`
  从表中取 URL、目标名和 MIME 后调用下载器。16 号中文字体表项为
  `dmrp.wapproxy.sky-mobi.com/sd?...appid=422006`、`brw/wfnt16c.mrp`、
  `application/sky-mrp`；12/16 号与中英文变体由同一表驱动。
- `game.ext+0x21640` 的端点表是 type 0..4 对应
  `/page2,/image,/mrp,/res,/sta`。`0x17AB8` 按目标主机选择下载类型：
  `dmrp.wapproxy.sky-mobi.com` 始终用 `/mrp`，其他 `lib.download` 资源用
  `/res`；两者在 `0x21474` 共用 tag10 续传请求。因此 `/mrp` 不是裸
  HTTP 文件接口，也不是首包后切换 `/res`。
- 现存 `tool/temp/_mrp_1783742320649.bin` 等 6 包全部为 343 字节，
  约 6.2 秒一次重试。请求 code=3，tag1 含 `appid=422006`，tag2 是
  `dmrp.wapproxy.sky-mobi.com`，首包无 tag10。旧代理没有 `/mrp` 分支，
  落入 `helloworld` 纯文本响应，是重试后“获取页面错误”的直接原因。
- 响应处理器 `game.ext+0x17488` 的普通路径在 `0x175B4` 强制要求 tag7，
  并解析 tag16 范围三元组。这与已验证 `/res` 契约一致：tag7=u16 22、
  tag8=`total-rangeStart`、tag16=`>III(start,end,total)`、payload 最多 1800 字节。
- `temp/wfnt16c.mrp` 为 22302 字节 MRPG，header offset 8 声明长度一致，
  offset 68 的 appid 为 422006，SHA-256 为
  `98763f1c6bff5cdef6a3fd3c22e34168d0445359f9fdcb3c34bc381e85dbac3a`。
  代理新实现按请求 query appid 扫描 `PROXY2_MRP_ROOT` 下的 MRP header，
  唯一匹配后使用与 `/res` 共享的范围信封；缺失、重复、长度不符或续传期间
  变更均返回明确错误，不回退到其他文件。
- 最终审查后收紧了通用边界：首包从同一已验证 fd 读取整个包，读后再次
  `fstat` 和校验 MRP header，然后按 appid 存为进程生命周期内的不可变内存快照。
  这同时消除按路径重开的 TOCTOU、每分块整包重读和并发下载的跨版本混包。
  请求没有 transfer id，所以同一进程不热切换同 appid fixture；更新资源需重启服务。
  快照有数量、单包字节和总字节上限，超限明确报错，不驱逐旧代。所有资源错误
  信封都带 tag7=22，避免客户端将明确错误折叠为 140。
  `/mrp` 还校验 code=3、配置的 dmrp 服务主机、`/sd` 路径与 `type=2`，不允许任意
  proxy2 目标按 appid 读取本地包。
- 当前局限：用户指定的 `temp/brw/` 在工作树中不存在，无法比对真机成功包/
  画面；已以当前客户端实际 `/mrp` 续包、PPM 和最终落盘文件完成验收。
- 实现后已把现存真实首包直接 POST 到运行中的 `server-http.js`，再按
  tag10 循环续传并独立解析每个响应。13 个偏移为
  `0,1800,...,21600`，前 12 包各 1800 字节，末包 702 字节；每包
  tag7=22，tag8=`22302-offset`，tag16 与 payload 起止严格一致。重组结果
  22302 字节，与目标文件逐字节相等，SHA-256 一致。这一阶段只证明代理
  协议层，后续再由客户端 E2E 和 PPM 完成验收。
- 目标 E2E 在收紧按键时长、提示区域差异、下载列表和落盘哈希断言后多轮通过，
  最终实现轮为 23.77s，保留目录 `/tmp/skyengine-e2e-Y4X8x9` 和
  `/tmp/skyengine-ws-JIB10I`。真实客户端请求 `/mrp` 13 次，tag10 偏移为
  `1800,3600,...,21600`；最终 `brw/wfnt16c.mrp` 是 22302 字节，
  SHA-256 与指定目标一致，`dllist1` 为 1320 字节，`dwnlist.dat`
  收敛为 4 字节。
- 人工检查最终 PPM：`font-settings.ppm` 是“字体设置”且光标在“16号字体”；
  `font-download-prompt.ppm` 明确显示“无该字体，是否下载？”；
  `font-download-started.ppm` 是下载管理界面的 `wfnt16c.mrp 1K/21K`；
  `font-download-complete.ppm` 明确显示下载成功并询问是否立即安装 16 号字体。
  验收过程使用实际 X11 `DISPLAY=:0`，未使用 xvfb，也未启用大量 trace。

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
  `/tmp/skyengine-e2e-3bKSD7` 与 `/tmp/skyengine-ws-K7oLWr`。人工检查 PPM 确认目标链接焦点、
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
  `/tmp/skyengine-e2e-TJWx2E` 和 `/tmp/skyengine-ws-9bFFtK`：焦点框确实位于
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
  `_page2_1783737479117.bin`，但 `/tmp/skyengine-e2e-Eeo8IJ/stdout.log` 只看到
  socket 15 收到 160B 的 302 后创建 socket 16，没有 raw GET；
  `download-activated-8.ppm` 明确显示“网络连接错误74”。这证明 proxy2 模式
  要求外层 200，不能复用 raw HTTP 模式的 301/302/303 跟随器。
- 焦点 PPM 同时校正了测试交互：详情页链接在首屏下方，DOWN 会先滚动正文；
  `/tmp/skyengine-e2e-Eeo8IJ/download-focus.ppm` 已人工确认蓝色焦点框位于
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
  `/tmp/skyengine-e2e-pEC0S3`，独立工作区为 `/tmp/skyengine-ws-U8eTpD`。
- PPM 人工检查：`loaded.ppm` 已完整显示目标首页；`focus.ppm` 的焦点框画在第一个链接
  `mynes [顶]`；短按回车后，超时保存的 `sublink.ppm` 仍是同一首页，只是时钟变化。
  因此不是测试缓存判据误报，页面确实没有导航。
- 请求时间线澄清：`_page2_1783670894111.bin`（350B）是启动更新检查；
  `_page2_1783670896145.bin`（179B）才是地址栏首次加载（tag1 为空、tag2=
  `mrp.gddhy.net`、tag3=inputaddr）。点击后没有第三个 `/page2`，故故障发生在浏览器
  焦点/激活阶段，尚未进入代理抓取子页面阶段。
- 首页缓存 `/tmp/skyengine-ws-U8eTpD/mythroad/brw/http/cache3/14352.dat` 为 2602B，
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
- 聚焦 E2E 通过：`1 passed`，17.41s；保留目录 `/tmp/skyengine-e2e-0f6UxK`，
  工作区 `/tmp/skyengine-ws-bFGMJ0`。第三个 `/page2` 包
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
- Latest contradictory artifact `/tmp/skyengine-e2e-WzdETQ` and workspace
  `/tmp/skyengine-ws-75t8yV`: `cache3/13998.dat` is 477 bytes and contains
  `<title>百度一下</title>`, `http://wap.baidu.com/`, and multiple `百度`
  strings, but `/tmp/skyengine-e2e-WzdETQ/loaded.png` visibly shows title-bar text
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
- PPM-verified result (`/tmp/skyengine-e2e-atstsl/loaded.ppm`): yellow title bar
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
- First repro round (screenshots `/tmp/skyengine-e2e-tkIlrI`): `ENTER` did NOT
  navigate — it opened the link long-press context menu (复制/新窗口打开/快速搜索/
  书签/…, no plain "打开" item). Cause: the test sets `VMRP_E2E_HOLD_MS=1500`
  for paste stability, and skyengine holds every injected key for that long, so
  ENTER registers as a long-press. Packet dumps confirm NO third `/page2`
  request reached the proxy in that round.
- Navigation path chosen: link menu item `2. 新窗口打开` via digit key "2"
  (menu items are digit-selectable), which matches the user's manual flow.
- Second repro round (`/tmp/skyengine-e2e-WjYiU6`, packets `_page2_1783593791382.bin`):
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
  hold override (falls back to VMRP_E2E_HOLD_MS/500); `skyengine-e2e.ts` `key()`
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
- Result (`/tmp/skyengine-e2e-bDO9Yu`, PASSED with tightened assertions): third
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

### 2026-07-11: idle 5s/10s navigation timer fix

- The 10s failure occurred before `/page2`: WBRW created a socket but its
  primary `game.ext` compact scheduler stopped reaching the socket poll/send
  callback.  The primary walker at runtime `0x26A1C4` decodes an R9-relative
  scheduler at `RW+0x44AC`; the host only recognized fixed offsets near
  `RW+0x80`, so primary liveness and live-node heap protection were false.
- Module registration now decodes each compact walker's scheduler literal and
  records the offset on the module.  Liveness checks and compact free-list
  protection use the discovered queued/current heads and require the timer
  node magic, with no package-name or endpoint condition.
- The compact wrapper walker already calls `table[31]` with its calculated next
  interval.  `arm_ext_call_dispatch()` now marks the current host tick consumed
  before dispatch and supplies a 50ms tick only if guest code leaves a live
  queue without rearming the timer; it no longer overwrites intervals such as
  2000ms or 23980ms.
- Verification: 98 C checks passed; the 10s focused run passed in 16.92s and the
  full 10s/5s matrix passed in 29.09s.  WBRW input and both gzwdzjs scheduler
  regressions passed.  Retained `loaded.ppm` files are
  `/tmp/skyengine-e2e-m9aJxn/loaded.ppm` (10s) and
  `/tmp/skyengine-e2e-ZTIR6z/loaded.ppm` (5s); visual inspection confirms the same
  complete `第1页 - WAP下载站` title, software list, scrollbar, and menu bar.
  No endpoint-remapping option is present or used.

### 2026-07-11 (final correction): preserve the compact wrapper helper tail

- Repeated matrices disproved the first owner-handoff result.  At the failing
  boundary the host entered only the wrapper walker at `0xE845BC`; the real
  public `code=2` path continues with a call to `0xE841F4`.  Skipping that tail
  detached the primary socket-poll node (`callback=0x2562FD`) from both the
  queued and current heads after it became due.
- A byte-proven compact wrapper owner now uses its public helper for the whole
  `code=2` path.  Older non-compact direct-dispatch wrappers keep their existing
  host dispatch.  Compact walker discovery also records the walker entry so a
  genuine primary owner can be invoked with the primary module's R9; there are
  no app, package, host, socket, or endpoint conditions.
- The E2E cache oracle now parses the cache pool header exactly and requires
  URL `http://mrp.gddhy.net/` plus title `第1页 - WAP下载站`.  It also waits for
  the settled yellow page title instead of accepting the blue receive-progress
  title.
- Final verification: 100 C checks passed; three complete 10s/5s matrices
  passed, followed by WBRW input and both gzwdzjs regressions.  Final retained
  PPMs `/tmp/skyengine-e2e-UoQ1Ww/loaded.ppm` and
  `/tmp/skyengine-e2e-WjvyKe/loaded.ppm` were visually checked and both show the
  complete target page.

### 2026-07-12: physical Ctrl+V ordering and browser-menu exit

- The remaining automated/manual difference was outside proxy2: `editCreate()`
  copied the edit field's old value into SDL's clipboard.  A user who copied a
  URL before opening the editor therefore lost it, while the old E2E `PASTE`
  command set the clipboard only after `editCreate()` and could never expose
  the defect.  The host editor now leaves the platform clipboard unchanged.
- E2E input separates `SET_CLIPBOARD` from `PASTE_SHORTCUT`.  The WBRW test now
  copies the URL before opening the editor, injects only Ctrl+V afterward, and
  requires the exact `editGetText()` URL, cache URL/title, and foreground PPM.
- The same test opens WBRW's own menu, selects its displayed `7.退出` command,
  captures the confirmation dialog, confirms with the left soft key, and waits
  for the skyengine child process to exit.  Test cleanup's SDL `QUIT` is not accepted
  as evidence.  Both successful runs end with guest `mythroad exit.` logs.
- Narrow PC watches prove that exit is another consumer of the same primary
  scheduler, not an unrelated menu bug.  The `game.ext` walker at file
  `+0x440CC` (scheduler `R9+0x44AC`) reaches its due-node callback branch at
  `+0x44170/+0x4418A/+0x44190` and invokes guest pointer `0x2562FD`
  (`game.ext+0x30205`) immediately before the observed exit.  `lib.exitbrw`
  watches did not fire.  The final host `mr_exit()` may be reached through table
  slot 54, callback cleanup observing shared `mr_state >= 3`, or both; the two
  exit logs do not distinguish those entry points and sibling `ptrace` is not
  permitted in this environment.  This uncertainty does not affect the proven
  scheduler dependency: losing the primary tick prevents the exit callback and
  explains both the stalled page request and ineffective exit confirmation.
- Compact walker registration was also tightened: file offset zero is a valid
  Thumb entry, not a not-found sentinel, and a matching shape with an invalid
  scheduler literal no longer masks a later valid walker.  The focused unit
  test covers both cases.
- Two independent dummy-driver matrices passed for both 10s and 5s idle paths,
  plus `input-text.test.ts`.  Retained second-round artifacts are
  `/tmp/skyengine-e2e-Wr3KSB`, `/tmp/skyengine-e2e-zNuZbZ`, and
  `/tmp/skyengine-e2e-UTJvhc`.  The two target-page images are byte-identical and
  have 114 colors; home-to-loaded differences are 3,841 title pixels, 59,424
  body pixels, and 67,230 full-screen pixels.  Menu and confirmation ROI
  differences are 13,515 and 18,242 pixels.  Visual inspection confirms the
  full target page, the menu containing `7.退出`, and the exit confirmation.
- Cross-application scheduler regressions passed under the dummy driver:
  `gzwdzjs/game-start.test.ts` 2/2 and `gghjt/game-start.test.ts` 2/2.  The C
  unit suite reports 103 checks and zero failures.

### 2026-07-12 (final correction): preserve one-shot timers in edit mode

- The preceding completion was premature.  Its E2E commands opened the editor
  and injected Ctrl+V back-to-back, while a physical user necessarily pauses.
  Adding only 500ms between those operations reproduced the reported stop at
  `editGetText()` in both clean and persisted `brw` state.
- The host SDL timer is one-shot: `timerCb()` clears `timeId`, posts
  `timerEventType`, and relies on guest `timer()` to schedule the next tick.
  The main loop handled edit mode before `timerEventType`, so any tick arriving
  while the editor was open was consumed by the edit-input branch.  The guest
  scheduler then had no timer with which to submit or poll the page request.
- `timerEventType` is now dispatched before edit-mode keyboard filtering.  This
  restores the pre-serialization behavior in which platform editing does not
  stop guest timers.  The existing KEYUP latch fix remains necessary because
  the ENTER that opens the editor has its matching release delivered while the
  editor owns keyboard input.  Both fixes are platform-generic and contain no
  package, URL, endpoint, or fallback condition.
- The regression now waits 500ms inside the editor and contains no successful
  `SCREEN` or `WAIT_DRAW` command.  PPM verification reads the frame written by
  normal rendering, so screenshots cannot act as hidden SDL heartbeats.
- Two clean dummy-driver 10s/5s matrices passed, including exact cache
  URL/title, target-page pixels, and guest menu exit.  Retained artifacts are
  `/tmp/skyengine-e2e-tcWtRh`, `/tmp/skyengine-e2e-7RmxsP`,
  `/tmp/skyengine-e2e-7QXMqO`, and `/tmp/skyengine-e2e-zAI4IE`.
- A full persisted-state run also passed on the final binary without screen
  events: `/tmp/skyengine-persistent-final-v2`.  It exercised `/simpleDownload`, paused
  500ms before Ctrl+V, sent `/page2`, matched the exact page cache, then exited
  through `菜单 -> 7.退出 -> 确定` with code 0.  `final.ppm` is 240x320 with
  113 colors and visually shows the complete target page.  TypeScript checks,
  the 103-check C unit suite, `input-text.test.ts`, and the four gzwdzjs/gghjt
  game-start scheduler regressions also pass.
