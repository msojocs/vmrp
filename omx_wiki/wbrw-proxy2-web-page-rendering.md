---
title: "WBRW proxy2 web page rendering"
tags: ["wbrw", "proxy2", "network", "e2e", "ppm", "sky-format"]
created: 2026-07-09T10:26:13+08:00
updated: 2026-07-11T21:20:00+08:00
sources: []
links: ["wbrw-input-paste-ucs2-table77-fix.md", "wbrw-startup-stall.md", "wbrw-manual-clipboard-and-menu-exit-verification.md"]
category: debugging
confidence: high
schemaVersion: 1
---

# WBRW proxy2 web page rendering

2026-07-09: Implemented the local proxy2 `/page2` web-page path in `tool/proxy.js` so WBRW can render a user-entered URL instead of only downloading the built-in `mphome.sky` homepage.

Protocol evidence:
- WBRW uses CMWAP proxy2; default DNS maps `proxy2.51mrp.com` to `127.0.0.1`, while `tool/server-http.js` dispatches port 80 requests to `tool/proxy.js`.
- The `/page2` request body is a binary proxy2 packet: `u32be magic 0x8aed9cf3`, `u16be version`, `u16be headerLength`, `u16be code`, `u16be status`, `u32be payloadLength`, `u8 fieldCount`, then `u16be tag/u16be length/value` fields.
- Live target request packet `tool/temp/_page2_1783563510251.bin` proves `tag1` is an empty path, `tag2` is `wap.baidu.com`, and `tag3` is `http://web.proxy.51mrp.com/?s=inputaddr`. The proxy therefore reconstructs `http://wap.baidu.com/`.
- A separate update-check `/page2` packet carries `tag1=/update/?pageid=0...` and `tag2=help.proxy.51mrp.com`; that path still routes through `handleHelpProxy()`.
- Extracted `game.ext` strings include `/page2`, `/image`, `/sta`, `proxy2.51mrp.com`, `POST`, `Host: `, `%s/cache3`, and `http://web.proxy.51mrp.com/?s=inputaddr`, matching the live request packets.
- Narrow disassembly of extracted `test/fixtures/wbrw.mrp` shows proxy2 lives in `game.ext`, not `cfunction.ext`: `game.ext+0x4DB40` contains `8a ed 9c f3`, `game.ext+0x4DC04` is `/page2`, `game.ext+0x4DC0C` is `/image`, `game.ext+0x4DC24` is `/sta`, `game.ext+0x4DC6C` is `proxy2.51mrp.com`, `game.ext+0x4DCD8` is `application/octet-stream`, `game.ext+0x4E198` is `>HHHHIB`, and `game.ext+0x4E1A0` is `>HH`.
- The endpoint selector at `game.ext+0x21640` maps type 0 to `/page2`; request construction around `game.ext+0x211CC` writes magic/version, tag 1 path at `0x21286`, tag 2 host/domain at `0x2131C`, and optional tag 3 referrer at `0x213F6`. The TLV writer at `game.ext+0x26E9C` and serializer at `game.ext+0x26F78` match `tool/proxy.js`'s fixed-header plus TLV packet layout.

Fix:
- `tool/proxy.js` parses proxy2 TLV fields, fetches real HTTP/HTTPS targets for non-proxy host `tag2`, strips script/style/head blocks, keeps text and links, and returns the simplified page as a proxy2 response packet.
- The response keeps `Content-Type: application/octet-stream`, `Connection: close`, `status=200`, and `tag33=3`, because WBRW accepts the packet and treats socket EOF as completion.
- URL reconstruction uses `URL` so both origin-form tag1 paths and absolute tag1 URLs work; redirects are resolved with `new URL(location, base)`.
- No app-name branch or Baidu-specific branch is present. The only host distinction is protocol-level routing for `help.proxy.51mrp.com` update checks versus target-page proxying.

Verification:
- `node --check tool/proxy.js` passed.
- `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/proxy.test.ts --reporter=verbose` passed after the URL reconstruction change. Latest artifact: `/tmp/skyengine-e2e-x2ZvJz`; workspace: `/tmp/skyengine-ws-HQZXfE`.
- Cache evidence: `/tmp/skyengine-ws-HQZXfE/mythroad/brw/http/cache3/14056.dat` is 477 bytes and contains `<title>百度一下</title>`, `http://wap.baidu.com/`, and multiple `百度` strings.
- PPM evidence: `/tmp/skyengine-e2e-x2ZvJz/loaded.ppm` is 240x320 with 149 unique colors; compared with `home.ppm`, top 90px diff is 5320 pixels and full-screen diff is 5491 pixels. URL bar samples are `(41,10)=[248,240,168]`, `(100,10)=[96,100,96]`, and `(66,10)=[96,100,96]`.
- Rejected experiments: `tag33=2` moved the target body into `percache` but did not keep a stable target-page foreground; a generated `.sky` payload without the template's matching layout metadata was cached but not reliably rendered. Final code keeps the stable simplified HTML + tag33=3 packet path.
- Full compatibility check passed: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e --reporter=verbose` reported `18 passed (18)` files and `30 passed (30)` tests in 306.30s.

Constraints retained: no xvfb, no full trace, no app-specific or Baidu-specific branch, comments added only around protocol-sensitive behavior, and proof uses request packets, cache files, PPM metrics, and e2e output.

## 2026-07-09 追加: .sky 完整字符串池重建 (残留内容修复)

问题: 上述"简化HTML+tag33=3"路径不稳定;改用 .sky 模板原地改写后,页面能渲染但满屏模板残留
("我的磨"/"我的磨2"/"百度网盘"/"上一页"/©Powered by 彩虹! 等)——模板 `32953109.sky` 共 115 个
池条目,原地改写只覆盖 ~18 个,其余 UCS-2BE 可见标题仍被显示列表按索引绑定渲染。

修复 (`tool/proxy.js` buildSkyPageFromHtml):
- 完整重建字符串池: DL 按**索引**引用条目,条目长度可自由变化;逐条分类
  (ascii/utf8/ucs2/bin),所有可见文本槽按序灌入抓取页面的 title+正文行,用尽后置空,
  每个槽保持原编码;`http(s)://`、`?…`、`/…` 形链接全部指向目标URL。
- 分类顺序陷阱: UCS-2BE ASCII(如 `\0Z\0I\0P`)是合法UTF-8字节但含NUL → strict UTF-8 判定必须
  拒绝控制字符;UTF-8 CJK 按 u16be 读会落入 PUA 区 → UCS-2 判定必须拒绝 PUA/代理项/控制单元。
- `./` 开头的资源引用(基准路径/logo.png)**必须原样保留**——改写会让页面自带的黄色标题栏头部
  元素消失(黄色像素 3575→0,恢复后回到 3336+)。
- 池重排后 `hdr[8..10]=poolEnd-10`;DL 原样拷贝,其 §4a 长度字段 A 只依赖 DL 自身大小,无需 patch。
- 实体解码: 补 &copy; 等命名实体;无分号数字实体(`&#xe750`)也解码;PUA 码点(图标字体)直接剥除。
- 默认模板换成 `179603.sky`(纯文本+链接列表骨架,无表单控件)——`32953109.sky` 的搜索表单
  输入框/下拉框/按钮是 DL 节点,池重写无法去除,属于无关元素。

测试目标 (用户要求): `http://mrp.gddhy.net/`(标题 `第1页 - WAP下载站`,稳定纯WAP页)。
测试字形像素断言随之更新: after-paste (107,17)黑/(108,17)白;前台判定 (41,10)黄[248,240,168] +
(79,10)灰[96,100,96](y=10 的 x=77..84 连续笔画)。

PPM 验证 (loaded.ppm, /tmp/skyengine-e2e-atstsl): 黄色标题栏"第1页 - WAP下载站"、URL 链接行、
正文 软件列表/mynes/RX管理器/skyengine安装包… 全部来自目标站,无表单、无模板残留;
缓存 .dat 同含 `http://mrp.gddhy.net/` 与 UTF-8 `WAP下载站`;聚焦用例通过。

## 2026-07-09 再追加: .sky 完全从零生成(不读模板)

模板路径全部删除,`generateSkyPage` 纯程序化发射 header/池/DL(语法反汇编+三样本逐字节验证,
详见 docs/wbrw-sky-format.md §4b-3/§4b-4):

- DL 记录帧: `[a:be16][flag:be16][bit15→e1][bit14→e2][op:u8][len:u8][payload]`,
  flag低14位=记录id 全页递增,a=父id(元素父带0x4000位)。
- 三个关键坑(各花一轮 e2e+截图定位):
  1. 记录流必须由 **`0F <X:be32> 00 00` 状态令牌**引导(0x0F 切到记录分发状态才产出节点),
     否则 payload 正常落缓存但浏览器静默留在首页,无任何报错。
  2. 不能发射 logo 图片元素:本地图片加载永不完成,标题栏卡在"加载图片0/1"。
  3. 同节下平铺 0x38 文本记录会连排;**每行要独立 0x4E 子节点**才换行。
- 池约定: [0]=URL(ASCII) [1]=标题(UTF-8,DL不引用,chrome/缓存索引层消费——也满足测试的
  UTF-8 子串断言) [2]="./" [3]=标题(UCS-2BE,头部元素)。链接池最终修正为
  [href(ASCII), caption(UCS-2BE), companion]，见文末最终结论。
  可见文本一律 UCS-2BE(渲染器无条件按 UCS-2BE 解码,0x1230C)。
- 链接现在携带真实解析后的 href(extractHtmlPageModel 返回 {text,href})。
- 截图验证: 黄底标题栏+每项独立成行的链接列表,零无关元素,比模板版更干净;聚焦用例通过。

## 2026-07-09 再修: 纯文本 vs 链接(渲染层区分)

问题: 从零生成把每行都用 0x38 绑 caption,导致纯文本(如"软件列表")也变蓝色且能被光标选中。
反汇编渲染层确认(0x275B4/0x28718 焦点扫描、0x2F96C 颜色):**可聚焦当且仅当 op 字节==0x38('8')/0x37('7')**,
与 e2 无关;文本框含 0x38 子元素→链接色,否则→黑色。
- 链接行 = 4E + 0x38(caption,锚点) + 0x54(e2=3,companion) → 蓝色可选；
  href 位于 caption 前一池槽。
- 纯文本行 = 4E + 0x54(e2=6),**无 0x38** → 黑色不可选。
- 操作数怪癖(e2=6/9 皆有):0x54 文本绑定渲染 pool[operand-1],0x38 渲染 pool[operand]。
  故纯文本 operand=文本索引+1、在该槽放 filler,渲染回退一位命中文本。
三重验证: 反汇编(纯文本行无0x38)+ 颜色(纯文本[0,0,0]黑/链接[8,88,176]蓝)+ 焦点框(DOWN键只落链接、跳过纯文本)。

## 2026-07-09 深夜: 子链接点击导航(“数据请求”卡住排查)

结论: **/page2 代理路径本身没有卡住问题**。用户手动看到的"数据请求"卡住,在自动化复现中
定位为两类互相独立的现象:

1. **E2E 注入按键全部变长按**: 测试为剪贴板粘贴稳定性设 `VMRP_E2E_HOLD_MS=1500`,skyengine 会把
   每个注入按键按住 1500ms。WBRW 交互语义(用户确认): **回车=确定,短按直接打开焦点链接;
   数字键 5 才是打开右键/上下文菜单**;长按回车(≥1500ms)也弹出链接菜单(复制/新窗口打开/…,
   无"打开"项),导航请求根本没发出(抓包确认无第三个 /page2);长按 DOWN 触发按键重复,焦点连滚。
   修复: `src/e2e_control.c` 的 `KEY <name> [hold_ms]` 支持按次覆盖按住时长(通用注入能力,
   非场景代码),`skyengine-e2e.ts key(name, timeoutMs, holdMs)`。实测: DOWN 用 120ms 单步移动;
   短按 ENTER(120ms)直接导航——早期"120/500ms 不导航"的观察是焦点错位一行造成的
   (真实焦点在纯文本行,回车无目标),见下方补充。
2. **代理 fetch 失败时回 text/plain**: 反汇编证实状态机(0x410a4/0x1fe22/0x41C60)只被
   socket recv 事件驱动、无独立超时定时器;`retcode=-1` 纯文本不是合法 proxy2 信封,record 层
   组不出响应包,标题栏停在"数据请求"。修复: /page2、/image 的 fetch 错误改回**合法 proxy2 包
   status=404**(空 payload),0x41C60 信封门(status!=200||tag33!=3 → r5=74)会给出"获取页面错误",
   UI 不再悬挂。`makeProxy2ResponsePacket(payload, fields, status)` 增加 status 参数。

点击请求包结构(抓包 `_page2_1783593791382.bin`): 与首次加载同为 /page2,唯三差异——
tag1=`/mrp/new_Sdilaov3003guoguanleiji`(origin-form 路径)、tag3=`http://mrp.gddhy.net/`
(referrer=当前页 URL,取代首载的 `http://web.proxy.51mrp.com/?s=inputaddr`)。现有
`buildProxy2TargetUrl`+`generateSkyPage` 直接就能处理,代理零改动即渲染成功。

截图证据(/tmp/skyengine-e2e-WjYiU6/sublink.png): 标题栏 `S地牢-斗圣 - WAP下载站`,正文
应用程序详细信息/应用名称/版本/应用ID/开发者/分辨率/大小/应用介绍,全部来自子页面;
缓存 `20652.dat` 池头 = URL `http://mrp.gddhy.net/mrp/new_Sdilaov3003guoguanleiji` +
标题 `S地牢-斗圣 - WAP下载站`。

测试断言改为解析 cache3 池头(URL@10+u16len,标题紧随其后 u16len+UTF-8),接受"同站非首页、
标题以 ` - WAP下载站` 结尾"的任意子页面——焦点落在哪一行链接不影响判定。

### 补充: 焦点作用行 = 可见焦点框的上一行(经验规律,多轮抓包对齐,仍适用)
- 焦点框画在"首页"时长按菜单打开了上一行"S地牢-斗圣"(成功轮);焦点框画在首个链接 mynes 时
  上一行是纯文本"软件列表",短按回车/菜单"新窗口打开"均静默无动作(失败轮,无 /page2 发出)。
- 最终用例序列(回车直接导航): 短按 DOWN×2(120ms,每次一行) → 焦点框画在 RX管理器
  (真实作用目标=mynes) → **短按 ENTER(120ms)直接打开链接**。长按菜单+数字2 路径也可导航,
  但非必需。
- 通过轮 /tmp/skyengine-e2e-bDO9Yu: 第三个 /page2 发出(tag1=/mrp/sky_SaiNes-v1010,
  tag2=mrp.gddhy.net, tag3=http://mrp.gddhy.net/),缓存 17327.dat =
  `http://mrp.gddhy.net/mrp/sky_SaiNes-v1010` + `mynes - WAP下载站`,
  sublink.png 完整渲染子页面(应用详细信息列表),无"数据请求"卡住。
- 测试断言加固: `waitForSubLinkPage` 同时要求正文区(y=30..280)相对首页差异>3000 像素——
  只看标题栏会被过渡态"数据请求..."进度标题误判。

## 2026-07-09 续: 第一个子链接点击无反应 — 根因定位进行中

目标(用户要求): 访问 http://mrp.gddhy.net/ 后**第一个**子链接必须可点击——即修掉
"焦点错位一行"本身,不再用 DOWN×2 补偿。

排查进度:
1. **页面数据无罪**(已证): 解码器(/tmp/skydec/decode.js,按 wbrw-sky-format.md 逐记录解码)
   对比 `build/mythroad/brw/http/cache3/7715.dat` 与 `tool/proxy.js generateSkyPage` 对
   实时抓取 HTML 的输出 —— **逐字节完全一致**(2599 bytes)。所有 href 均为真实子链接 URL。
   - 推论A: 7715.dat/26660.dat 是**本地 skyengine 运行产物**,不是真机数据(否则不可能与
     生成器逐字节相同)。真机参考 temp/brw/(wbrw-sky-format.md 引用的 179603.sky 等)
     目前不在仓库,需用户补充。
   - 排查陷阱: 用 node vm 沙箱加载 proxy.js 提取内部函数时,忘了给 sandbox 传 `URL`
     全局 → extractHtmlPageModel 的 `catch { href = requestUrl }` 兜底吞掉了
     ReferenceError,所有 href 退化成首页 URL——险些误诊。该 catch 兜底本身也应清除
     (违反"无兜底"约束,会把解析 bug 静默变成错误 href)。
2. **行为模型**(与既有观察一致): 锚点序列 = [页头标题0x38(id4,无href), mynes(id11),
   RX管理器(id14), …]。可见焦点框序列却从 mynes 开始。DOWN×1: 框在 mynes,逻辑焦点在
   id4(标题,无 href) → ENTER 无反应; DOWN×2: 框在 RX,逻辑焦点 mynes → 打开 mynes。
   即绘制路径跳过了 body 下的标题锚点而激活路径没有(或者等价机制)。
3. 反汇编任务已派发(game.ext 焦点扫描 0x275B4/0x28718、paint 0x27F50、ENTER 激活路径),
   等待结论后决定修复层(生成器记录结构 vs skyengine C 侧)。

## 2026-07-10 首链接激活最终结论

旧的“可见焦点框与 selected DOM 落后一行”解释已被反汇编推翻：view 构造器把当前
`0x54` DOM 直接写入 `view+0x54`，焦点保存路径也不做 prev/减一。真正的一格错位来自
字符串池：真机 10 个 `.sky` 的 320 个 `0x38` 锚点都以
`pool[captionIndex-1]` 为链接目标，而生成器原先使用 `caption, href`，使第一个
caption 前面是 filler、第二个 caption 前面才是第一个 href。

最终生成布局是每链接 `href(ASCII), caption(UCS-2BE), companion`，记录保持
`4E(row) -> 38(caption) -> 54(companion)`。聚焦 E2E 在 17.41s 通过；第三个
`/page2` 请求为 `/mrp/sky_SaiNes-v1010`，子页缓存标题是
`mynes - WAP下载站`。PPM `/tmp/skyengine-e2e-0f6UxK/sublink.ppm` 人工检查为完整
应用详情页，无模板残留、无“数据请求”或图片加载提示；相对首页正文差异 8793 像素。

## 2026-07-11: 5 秒成功、10 秒失败排查进度

原样运行 `temp.test.ts` 在约 18.8 秒稳定报
`ERR wait_draw_timeout current=29 target>29`，不是 Vitest 超时、进程退出或 socket
空闲断开。失败前 stderr 已记录 `editCreate(title='网址')`，说明第二次 ENTER 已经
成功进入宿主编辑模式；`editCreate` 只修改 `isEditMode`，本身不提交位图。

当前证据把 5/10 秒差异解释为 E2E 同步误判：

- `VmrpE2e.key()` 在发出 KEY 后无条件等待全局 `guiDrawBitmapCount` 增长，但有效的
  编辑模式切换不要求绘图。等待期间若恰好有后台图片重绘，这个无关 draw 会让动作
  看似成功；图片加载结束后同一动作则报超时。
- 用例把 `VMRP_E2E_HOLD_MS` 提高到 1500ms，却把 `key(name, timeoutMs, holdMs)` 的
  第二参数误当按住时间，所有按键实际仍是 1500ms 长按。既有 game.ext 行为证据已
  证明长按 ENTER/DOWN 会改变 WBRW 语义。
- 启动约 8--10 秒还会发送后台更新 `/page2`：350 字节请求的 tag1 是
  `/update/?pageid=0...`、tag2 是 `help.proxy.51mrp.com`，不是用户输入地址请求。
  它会贡献后台重绘，因此扩大上述同步误判的时间窗口。

代理路由还存在独立的确定性问题：guest 固定连接解析地址的 port 80，而当前
`tool/server-http.js` 监听 `127.0.0.1:18080`。当前默认 DNS 只改变地址，不改变端口；
请求实际落到本机另一个 nginx，收到的 `text/plain retcode=0` 不是合法 proxy2 信封。
后续验证必须明确使用 guest 实际连接的 port 80，不能把监听 18080 的服务误认为
已经接管请求，也不能依赖机器上碰巧存在的另一个 80 端口服务。

尚未完成：需要给 E2E 输入 API 增加显式的“不等待 draw”契约，改用短按；用例必须
等待目标页缓存和前台正文同时变化，保存 `loaded.ppm` 并检查像素，不能以固定 60 秒
delay 或仅观察 `/page2` 日志作为成功证据。

## 2026-07-11: 当前验证边界

E2E 输入 API 已增加 `{ timeoutMs, holdMs, waitForDraw }`，编辑器切换使用
`waitForDraw:false`，按键改为 120ms 短按。用例同时检查目标 URL/标题缓存、标题区和
正文区 PPM 差异，并保留 `loaded.ppm`。5 秒路径通过，目标缓存为 2697 字节；人工
检查 `/tmp/skyengine-e2e-k27O6m/loaded.ppm` 为“第1页 - WAP下载站”，相对首页全屏差异
67230 像素、标题区 3841、正文区 59424，共 114 色。

服务端 `/image` 还修正了独立协议错误：上游非 2xx 不再作为 status=200 的图片
payload 返回，而是返回合法 proxy2 错误信封；定向 Go 测试通过。该修复没有消除
10 秒失败。

10 秒路径在 60 秒页面等待后仍失败，`loaded.ppm` 保持首页且没有目标缓存。窄诊断
证明失败发生在目标 `/page2` 前：后台更新在图片响应后创建 socket 5，但 WBRW 的
任务链随后只处理统计下载，未继续 `getSocketState(5)`/`send(5)`，因此真正的 179
字节目标页请求从未产生。wrapper/primary/active-child owner、open-socket keepalive、
真实 elapsed 和图片短超时实验均未通过 10 秒验证，已全部撤销，不保留未经验证的
C 或服务端时序改动。仓库不增加端点重映射参数。

## 2026-07-11: 5/10 秒空闲分界已解决

最终根因是两层 compact scheduler 的宿主建模不完整，而不是服务端页面处理或 socket
保活。WBRW wrapper walker 位于 `0xE845BC`，scheduler 为 `wrapper_rw+0x2A8`；primary
`game.ext` 还有同形 walker `0x26A1C4`，其 Thumb literal 解出 scheduler 为
`game_rw+0x44AC`。旧代码只探测 primary/child 的固定 `0x80/0x84/0xC0/0x248`，所以把
WBRW primary queue 报成 `primaryTimer=0`，也没有把该队列中的 live timer node 从
compact free-list 中保护出来。

通用修复在模块注册时从 walker 指令形状动态解出 R9-relative scheduler offset，并记录到
模块元数据。primary/child 判活和 timer-node 保护统一读取该模块自己的 queued/current
head，并校验 `0x79ABBCCF` magic；没有 app 名称、MRP 名称、host 或 socket 条件。wrapper
队列为空但 primary 真实队列仍活时，宿主只补一次 primary tick；primary 经 wrapper veneer
重新挂回外层节点后，owner 再归 wrapper。

另一个确定错误在 `arm_ext_call_dispatch()`：guest walker 会先 timerStop，再按队列计算并
调用 `table[31]`，但宿主此前无条件用固定 50ms 再次 `mr_timerStart`，覆盖了 guest 请求的
200/2000/23980ms 等间隔。现在 dispatch 入口先消费当前 host tick；仅当 guest 留下真实
live queue 却没有重启 timer 时，宿主才补 50ms tick。

验证：`SDL_VIDEODRIVER=dummy ./build/skyengine-unit` 为 98 checks/0 failures；10 秒定向用例
16.92s 通过，完整 10s/5s 矩阵 29.09s 全绿，`input-text.test.ts` 通过；替换固定 scheduler
偏移后，`gzwdzjs/game-start.test.ts` 两个用例也通过。保留的 10 秒与 5 秒 PPM 分别是
`/tmp/skyengine-e2e-m9aJxn/loaded.ppm`、`/tmp/skyengine-e2e-ZTIR6z/loaded.ppm`；人工检查均完整显示
“第1页 - WAP下载站”、软件列表、滚动条和底部菜单。服务端图片非 2xx 信封测试仍通过。
仓库与运行命令均未加入端点重映射参数。

## 2026-07-11: 重复验证后的最终修正

上节的 owner handoff 单轮结论不完整，重复矩阵仍能复现 10 秒失败。最终日志显示 host
只直调 `0xE845BC` wrapper walker 时跳过了公开 `code=2` 路径随后执行的 `0xE841F4`。
在 wrapper bridge 排空的那轮，primary socket 扫描节点 `callback=0x2562FD` 从 queued
摘出后没有留在 scheduler current head，后续 socket 5 虽连接成功却永远不再执行
`getSocketState(5)`。

通用最终修复是：动态识别到 compact scheduler 的 wrapper owner 走完整 public helper
`code=2`，不再只直调 walker；非 compact direct-dispatch ABI 保持原路径。动态扫描同时
记录 compact walker 入口，确需 primary owner 时以 primary R9 直接进入其 walker。判断
只依赖模块自身指令形状、P/helper owner 和真实队列，不含应用名、包名或网络端点条件。

E2E 也改为解析 cache3/percache pool head：偏移 10 的大端 u16 URL 长度、URL、随后大端
u16 标题长度和 UTF-8 标题，严格要求 `http://mrp.gddhy.net/` 与
`第1页 - WAP下载站`；并等待黄色完成态标题，避免把蓝色“接收数据”阶段当成功。

最终验证为 100 checks/0 failures，完整 10s/5s 矩阵连续三轮通过，WBRW 输入回归 1/1、
gzwdzjs 教程/花屏 2/2。人工检查 `/tmp/skyengine-e2e-UoQ1Ww/loaded.ppm` 与
`/tmp/skyengine-e2e-WjvyKe/loaded.ppm` 均为完整目标页。没有增加或使用任何端点重映射参数。
