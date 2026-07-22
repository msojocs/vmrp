# gjxqy 本地资源下载复现进度

更新时间：2026-07-22

## 目标与约束

- 目标用例：`pnpm vitest run test/e2e/gjxqy/download-res.test.ts`
- 本地服务器：`tool/jxqy-server.js`
- 服务端资源：`temp/gjxqy-res/res_1.mrp`
- 成功口径：客户端下载并落盘资源包，且用 PPM 验证下载后的可见状态。
- 修复必须基于协议与反汇编证据；不增加应用身份特判、地址特判或失败兜底。
- trace 仅在窄口径诊断时启用，不保存无界全量日志；不使用 xvfb。

## 2026-07-22 初始审计

当前工作树包含尚未提交的 `src/network.c` 修改：当 CMWAP 请求解析出的目标
IP 等于 `0xD39BEC12`（`211.155.236.18`）时，直接替换成 `127.0.0.1`。
这是目标服务器地址特判，不满足通用性约束。仓库已有 `--dns-map` / `dnsMap`
测试入口，后续应先验证客户端请求中的主机表达形式，再通过显式测试配置注入本地映射。

`tool/jxqy-server.js` 当前只监听 `10005` 并打印收到的数据，没有任何响应；
`download-res.test.ts` 当前也只等待 60 秒，没有启动服务器、检查落盘文件或验证
下载结果画面。因此现有代码不能证明“资源包下载成功”。

待验证问题：

1. 客户端发往 `10005` 的准确请求字节、请求主机和分包方式。
2. 服务端所需响应头/长度字段/资源包传输顺序。
3. 客户端落盘路径和成功画面的稳定 PPM 断言点。
4. CMWAP 首次发送及接收语义是否另有通用 BUG，需结合 guest 反汇编确认。

## 2026-07-22 协议抓取与反汇编

受控运行抓到客户端请求：HTTP 部分为 `POST / HTTP/1.1`，`Host` 与
`X-Online-Host` 都是 `211.155.236.18:10005`，正文类型是
`application/octet-stream`，长度为 161 字节。请求正文的完整十六进制为：

```text
8aed9cf37e32b900010000a10064012d32384138314434444533334430303030
0000000000018e73000000750000000000026f70656e7365006f70656e7365
0006063566000007b000f00140057234363030313937303733323733303200
38363430383630343036323238343100000000000000000000000003020000
0000000f983c000000000b0000000000000000000000000c00000000000000
0000000000
```

解压主包中的 `cfunction.ext` 后，已确认以下 Thumb 代码与只读数据：

- 文件偏移 `0x23D4` 引用 `0x5FA54` 的服务器地址；`0x245B8` 构造 HTTP
  请求，`0x245D2..0x24672` 追加请求行和各请求头。
- `0x246CC` 校验响应以 `HTTP/` 开头、存在 `\r\n\r\n`，并提取三位
  HTTP 状态码；调用点 `0x251C` 明确要求状态码 200。
- HTTP 后不是直接读取 MRP。接收状态 4 在 `0x1F28` 解析
  `Content-Length:`；状态 5 固定请求 44 字节应用层头；状态 6、7、8
  依次读取 NUL 终止帧、2 字节大端字段和至少 22 字节的描述帧；状态 9
  才按剩余长度流式接收资源正文。
- 请求 envelope 的固定 44 字节头包含 8 字节 magic
  `8aed9cf37e32b900`、总长、命令、16 字节会话标识、消息 ID 与 payload
  长度。该布局将用于继续反推响应，而不是按字符串猜测。

第一次实验把 HTTP 头与 273877 字节 MRP 一次发送。环回 TCP 合并数据，
客户端首次 `recv(2048)` 同时收到响应头和一部分 MRP，随后关闭连接且没有
落盘。第二次实验把 102 字节 HTTP 头单独发送，延迟 250ms 后发送 MRP；
客户端先收到 102 字节头，再收到以 `MRPG` 开始的 2048 字节，但仍关闭连接。
两次运行都没有生成 `res_1.mrp` 或 `res.bin1`，证明标准 HTTP 加原始 MRP
不是目标协议，也排除了“只需避免 TCP 合包”这一不完整假设。

当前目标测试在服务器不回包以及两次错误响应下都会通过，耗时约 77 秒；
这是由末尾固定 `delay(60_000)` 且无成功断言造成的假阳性。

## 2026-07-22 旧协议响应与 TCP 边界复核

进一步反汇编确认响应正文是以下旧协议序列：

```text
44-byte envelope
5-byte status      000000c800
2-byte count       0001
31-byte descriptor 020000000000000000000000c8000000007265735f312e6d72700000042dd5
273877-byte res_1.mrp
```

响应 envelope 从请求前 36 字节派生，只覆盖总长、响应命令和 payload 长度：

```text
offset 8..11  = 01042e27  (body 273959)
offset 36..39 = 00031513  (response command 202003)
offset 40..43 = 00042dfb  (payload 273915)
```

第一次隔离实验让 HTTP header、envelope、status、count、descriptor 之间各等待
1 秒。stdout 中前五次 `recv` 精确为 `102/44/5/2/31`，随后资源正文按
2048 字节流式接收；测试在 22.3 秒内生成 `mythroad/pg/res_1.mrp`，大小
273877，SHA-256 为
`ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1`，
并与服务端源文件逐字节相等。

对 `cfunction.ext +0x50400/+0x506C4..+0x50750` 的更深复核推翻了“合包会
依赖 stale buffer”的早期假设：底层保存 frame-length/data callback，并在同一
ring buffer 上交付一帧、推进 read pointer、再用新状态继续消费未读字节。
因此服务端改为一次 `socket.end()` 写出完整响应。第二次运行的首次 `recv`
为 2048 字节，全部 135 次 `recv` 总计 274061，恰等于 102 字节 HTTP header
加 273959 字节 body；资源仍以相同哈希完整落盘。早先 HTTP 加原始 MRP 失败
的真实原因是原始 MRP 前 44 字节被状态 5 当成 envelope，随后响应命令
`0x31513` 校验失败，而不是 TCP 合包。

## 2026-07-22 下载后 PPM 反证

文件落盘不能单独作为最终成功口径。隔离实验刚收完文件时，主动截图仍是黄色
“下载游戏包中...请等待”画面（4 色）。合包实验继续轮询 120 秒后，PPM 稳定为
3 色，并明确显示：

```text
下载资源失败，请稍后重试
按确定键返回游戏
```

该 PPM 的 SHA-256 是
`c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`。
因此当前服务器已复现字节传输，但上层下载完成判定仍未闭环，目标尚未完成。
下一步只观察 `cfunction.ext` 状态 9 收满、临时文件重命名以及回调事件 4/5/6/7
的精确 PC，区分 EXT 未上报成功、文件最终化失败和 Lua 收到成功后仍超时。

## 2026-07-22 资源 ID 回显缺失

精确 PC 观察把失败条件定位到响应 descriptor 的资源 ID。一次完整合包运行中，
状态 9 的最后一批数据依次命中 `+0x20AC`、`+0x2128`、`+0x2164`、
`+0x2182` 和 `+0x21A4`；`+0x21A4` 的 `R0=0`，证明临时文件最终化成功。
但运行没有命中 `+0x21BA` 的事件 5，关闭 socket 后反而命中 `+0x2274` 的
事件 6。下载文件仍为 273877 字节且哈希正确。

逐指令复核表明，下载状态中的请求资源 ID 是 `C+0`，响应 descriptor 解析后的
资源 ID 是 `D+8`。两者不相等时，客户端仍会写完文件，并退回 descriptor 文件名
完成重命名；但 `+0x21AA..+0x21AE` 会跳过事件 5 和 `S+28=1` 的成功锁存。
HTTP body 收满后，`+0x225E` 发现锁存仍为 0，因而发送事件 6。这解释了为何
“文件正确落盘”与“最终显示下载失败”会同时发生。

请求构造函数 `+0x2EB8` 把每个 16 字节内存记录编码为 13 字节 wire 记录：
1 字节 type 加三个大端 `u32`。本次请求在 `@120` 声明 3 条记录，type 依次为
`2/11/12`；type 2 记录的第二个 `u32` 位于请求 `@127`，值为
`0x000F983C`。服务器此前把响应 descriptor `@5` 固定写成 0，正好造成上述
不匹配。现改为严格解析请求记录并回显该资源 ID；下一步用 PC watch 和最终
PPM 同时验证事件 5、成功锁存及应用可见状态。

## 2026-07-22 回显修复验证与安装器拒绝

修复 descriptor 后的精确 PC 观察证明下载状态机已完整闭环：`+0x21A4`
返回 0，`+0x21BA` 比较的请求和响应 ID 都是 `0x000F983C`，随后命中
`+0x21C4` 的成功锁存；连接结束时不再命中 `+0x2274` 的事件 6。上层把
下载事件 5 映射为状态 2，并调用 `+0x3AC8` 安装 resource 1。

安装器可以找到下载包并成功读取 `fileid.bin`，但在 `+0x3B92` 拒绝它：

```text
下载请求/slot 1 expected ID = 0x000F983C = 1022012
res_1.mrp fileid.bin         = 0x00000FA1 = 4001
```

`fileid.bin` 是 MRP 目录偏移 `0x1EF` 的 gzip 条目，解压后恰为
`00 00 0f a1`。`+0x3B6C..+0x3B82` 逐字节按大端得到 4001；`+0x3B84`
只释放读取缓冲，`+0x3B88..+0x3B92` 随即从 resource-ID 表读取 slot 1 并
直接比较。失败返回值为 `-2`，上层 `+0xDB08` 因而进入安装失败状态 3。

`4001 = 40 * 100 + 1` 与包内连续的 `res.p0..res.p39` 和 resourceNo 1
相关，但客户端不做这种解码：part 序号在通过 ID 校验后从 0 独立枚举，slot
也来自安装器参数。协议解析、下载完成回调和安装器中都不存在把 4001 映射为
`0xF983C` 的字段或转换。因此服务器必须继续原样发送 MRP，不能改写
`fileid.bin`，也不能增加应用或地址特判。

该次诊断工作区为 `/tmp/skyengine-ws-8NoewK`，artifact 为
`/tmp/skyengine-e2e-gTf1Xq`。最终 PPM 仍是已知 3 色安装失败画面，SHA-256
为 `c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`；
它不能作为成功断言。

## 2026-07-22 fixture 来源不匹配

`test/fixtures/gjxqy.mrp` 不是可证明与 `res_1.mrp` 配套的 v1012 主包。
它与 `build/mythroad/gjxqy.mrp` 仅在文件偏移 72 和 199 的版本低字节不同：
build 包两处都是 1008 (`0x03F0`)，fixture 两处都是 1012 (`0x03F4`)；其余
423134 字节完全相同，内嵌 `cfunction.ext` 也完全相同。仅修改 MRP header
和镜像 header 的版本不会更新编译进 EXT 的 resource ID。

启动函数 `+0x37B28` 从 literal `+0x37B74` 取出 `0x000F983C`，调用
`+0x25D9C(0xF983C, 0, 0)`；该函数把首参数原样登记到 slot 1，安装器随后
从同一表项读取。也就是说当前主 fixture 无论 header 写 1008 还是 1012，都会
要求 `fileid.bin=0xF983C`。本地扫描没有找到该 fileid 的 resource 1，也没有
找到登记 4001 的另一份 appid 335001 主包。

外置 `test/fixtures/gjxqy` 不是污染来源：主包的 `res.p0..res.p33` 拼接后与
`res.bin0` 逐字节一致，主包的 `res.i0..res.i9` 也与外置十个索引逐个一致。
问题边界是主包编译期 resource ID 与所给 resource 1 的 `fileid.bin` 不配套。
在没有匹配发行物或其它来源证据前，不能通过服务器改包、跳过比较或生产代码
fallback 掩盖这个 fixture 问题。

## 2026-07-22 受控安装与正向 PPM（非最终口径）

为确认 ID 比较之后没有第二个实现问题，做了两次仅作用于临时工作区的隔离
实验；两次修改都没有进入服务器实现、正式 fixture 或目标测试。

第一次把临时主包 `cfunction.ext+0x37B74` 从 `0xF983C` 改为 4001，并重新
打包、更新长度和 CRC。该包在资源管理器初始化阶段停住，未显示正常启动画面，
也没有发出下载请求。这证明不能假定“真实配套主包只改一个 literal”，更不能
用这种合成主包替代缺失的发行物。

第二次保持当前主包不变，只把临时 `res_1.mrp` 副本中的四字节
`fileid.bin` 改为 `0x000F983C`。这不是候选修复，只用于越过已经定位的唯一
比较点。运行随后完成了真实下载、安装与状态恢复：

```text
res.bin1 size    1294042
res.bin1 sha256  11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07
res.v before     f403000000000000889b5fa4
res.v after      f403000001000000899b5fa4
res.i1           与安装前逐字节相同
res_1.mrp        保留在 mythroad/gjxqy/res_1.mrp
```

正向 PPM 为真实的存档选择游戏画面（不是下载或安装 overlay），共有 48 色，
SHA-256 为
`faec61c188b3b272d1b709e743e46083a64071320cebfac41d52be788ffdb9cd`。
诊断 artifact 为 `/tmp/skyengine-e2e-MhaHPK`，workspace 为
`/tmp/skyengine-ws-i9Qq2a`。该结果证明服务器响应、完整传输、MRP 分片提取、
`res.v` 更新和恢复游戏状态在 ID 相等时均能工作，也把当前失败唯一收敛到
fixture 的不兼容 ID。

但临时资源副本 SHA-256 已变为
`94ebbc1cb0b2c0dea74abe0bcf8b656b9ba3a5eb3d10c83b08c3ae1232fe9448`，不再是
用户提供的 `ead50b...` 原件。因此这张正向 PPM 只能作为因果隔离证据，不能
冒充“原资源包安装成功”的最终验收。完成目标仍需要二选一的真实配套资产：

1. `fileid.bin=0x000F983C`、与当前主包匹配的 resource 1；或
2. 能在 slot 1 合法登记 4001、与当前 `res_1.mrp` 匹配的主包。

## 2026-07-22 包内来源与资源清单复核

不依赖外部资料的包内证据进一步确认了发行物身份。主包与
resource 1 的内部文件名均为 `gjxqy.mrp`，标题均为《剑侠奇缘》，
appid 为 335001，header 版本均为 1012，vendor 为 `sky`。主包 tag
为 `63b7b104c`，resource 1 tag 为 `10adf3e06`。

`res.i5` 中的 `bin_about.txt` 明确写有“出品公司 / 上海井中月”，
并列出策划 `XI WEN JUN`、美术 `SUN QI`、程序 `WU HAI BAO`。
`bin_sms.txt` 中的发行通道是 `152KZ2_60CY2`，`gameId=020`，
`channelId=18`，短信指令为 `146023023001`。这些字段只提供
发行物来源信息，没有参与 resource ID 比较。

主包目录包含 `res.i0..res.i9` 与 `res.p0..res.p33`；34 个分片解压拼接后
与 fixture 中的 `res.bin0` 逐字节一致，长度 1087143，SHA-256 为
`6f3939d29c03a8b0300a5e3b6734d8b30d3ba2fc69c24059497447fd34235ccd`。
resource 1 包含 `fileid.bin` 和 `res.p0..res.p39`；40 个分片解压拼接后
长度 1294042，SHA-256 为
`11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07`。
两个 blob 除起始 4 字节外不同，所以 resource 1 是完整的新一代
资源 blob，不是对 `res.bin0` 的简单尾部补丁。

用户已明确要求停止互联网检索，后续不再尝试公网、缓存站或历史
归档；只使用仓库现有二进制、反汇编与本地 E2E 证据。目标测试中
的诊断色数门槛同时由 `> 2` 收紧为 `> 16`，确保已知的 3 色安装
失败画面不会被误报为成功。得到配套 fixture 后还需用独立成功运行
采集稳定正向像素，色数本身不是最终验收口径。

## 2026-07-22 纯本地资产与映射穷举

按用户要求，已经停止全部互联网、公共存档和下载站检索。后续只进行本机
只读审计。覆盖范围包括仓库全部 Git 对象（含不可达对象、refs、reflog、stash
和本地 LFS 对象）、本机可见的 3833 个 MRPG 文件、已有 `/tmp` 运行产物，
以及本机其它 Git 仓库和已落盘缓存；没有发起网络请求。

审计没有找到以下任何一项：

- slot 1 登记 `0x00000FA1` 的真实 v1012 主包；
- `fileid.bin=0x000F983C` 的真实 resource 1；
- `4001 <-> 0x000F983C` 的映射表、算术转换或协议派生逻辑。

精确交叉引用再次确认 `+0x25D9C` 只有启动函数 `+0x37B36` 一个调用点；
`R9+0x44C` 的资源 ID 表只在初始化器和安装器比较路径中被引用。网络响应
descriptor 不会更新该表，也不存在增加其它响应 descriptor 来替换期望 ID
的路径。客户端把下载请求 ID、响应回显 ID和包内 `fileid.bin` 当成同一个
不透明兼容 ID，而不是可互相换算的不同命名空间。

本机仅有的两个表面匹配包都是本轮明确标记的因果诊断产物：一个只把主包
literal `3c 98 0f 00` 改成 `a1 0f 00 00`，另一个只把资源包解压后的
`fileid.bin` 从 `00 00 0f a1` 改成 `00 0f 98 3c`。它们的哈希和修改记录
均可与原件区分，不能充当真实发行物。

## 2026-07-22 原始下载件生命周期复核

使用原始资源包和当前严格服务器再次执行保留工作区的目标用例：

```text
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gjxqy/download-res.test.ts --reporter=verbose
artifact  /tmp/skyengine-e2e-pepSBb
workspace /tmp/skyengine-ws-OGnzsi
duration  42.59s
```

服务端事务断言全部通过，目标只在 `uniqueColorCount() > 16` 失败，实际为
3 色。最终 PPM SHA-256 仍为
`c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`。
运行退出后，工作区中既没有 `mythroad/pg/res_1.mrp`，也没有
`mythroad/gjxqy/res_1.mrp` 或 `res.bin1`；`res.v` 仍为安装前的 12 字节版本。

这与已观察的 guest 路径一致：网络状态机先完整写入原包、完成最终化并发出
事件 5；上层随后立即进入安装器，读取同一下载件的 `fileid.bin`，在严格 ID
比较处返回 `-2`，并删除不兼容包。因此“网络下载事务成功”已经由 PC 观察和
逐字节落盘实验证明，但“原包通过安装并恢复游戏画面”被不兼容 fixture 阻断。
不能在 25 秒后的测试工作区中用文件存在性证明前一个阶段，也不能把后一个
阶段的失败归因于服务器响应。

## 2026-07-22 瞬时下载件逐字节取证

目标 E2E 新增了一个只观察准确下载路径的本地文件观察器。它在触发下载前
监听工作区，guest 创建资源文件时立即以只读方式持有 inode，等待文件长度达到
273877 后再读取。即使安装器随后 unlink 路径，已打开的文件描述符仍能提供
客户端下载到的真实字节。这不是服务器发送缓冲区的副本，也不修改 guest 文件
生命周期。

第一次按旧失败分支观察 `mythroad/pg/res_1.mrp` 没有命中。结合受控正向
工作区重新核对后确认：响应 ID 正确回显时，下载器使用请求预先配置的
`mythroad/gjxqy/res_1.mrp`；只有响应 ID 不匹配、退回 descriptor 文件名时
才使用旧的 `pg` 路径。观察器随即收窄到准确的 `gjxqy/res_1.mrp`。

第二次运行：

```text
artifact  /tmp/skyengine-e2e-YNwMAJ
workspace /tmp/skyengine-ws-ZUvqwd
duration  41.88s
```

以下断言在同一个目标 E2E 中全部通过：

```text
客户端落盘长度       273877
客户端落盘 SHA-256   ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1
请求正文长度         161
响应正文长度         273959
发送帧长度           [102, 44, 5, 2, 31, 273877]
```

用例随后仍只在 `uniqueColorCount() > 16` 失败，实际为 3 色。这使两个口径
在同一运行中得到直接证据：原始资源包已经由客户端逐字节下载成功；不兼容
fixture 仍阻止安装成功。当前保留正向 PPM 门槛，等待明确验收口径，未把安装
失败画面静默改判成下载成功。

## 2026-07-22 `temp/gjxqy` 已解压目录审计

用户补充了本地已解压目录 `temp/gjxqy`。该目录是一个混合叠加树，不是另一份
主包，也不是一次完整成功安装的现场：

- 基础运行树包含 `res.bin0`、`res.i0..res.i9`、`res.v`、SID、重注册数据和
  存档；其中 `res.bin0` 与十个 `res.i*` 均和现有 fixture 逐字节一致。
- resource 1 部分包含原始 `res_1.mrp`、`fileid.bin` 与 `res.p0..res.p39`。
  `res_1.mrp` 与 `temp/gjxqy-res/res_1.mrp` 大小和 SHA-256 完全相同。
- 解析 MRP 的 42 个目录项并逐项解压比较后，磁盘上的 `fileid.bin` 与全部
  40 个 `res.p*` 均逐字节匹配；唯一没有解出的条目是通用 `start.mr`。
- 40 个 `res.p*` 按数字顺序拼接仍是 1294042 字节、SHA-256
  `11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07`。
  目录中没有安装器成功路径必然生成的 `res.bin1`。

目录中的 `fileid.bin` 仍为：

```text
00 00 0f a1  -> big-endian 0x00000FA1 = 4001
```

目录不包含 `cfunction.ext`、`mrc_loader.ext` 或任何其它 EXT，因此没有新的
主包装器可把 slot 1 合法登记成 4001；全目录也未出现 `0x000F983C` 的大端或
小端四字节表示。

`res.v` 为：

```text
f0 03 00 00 01 00 00 00 8d 9b 5f a4
version=1008, slot=1, checksum=0xA45F9B8D
```

它与当前 `build/mythroad/gjxqy/res.v` 逐字节一致；校验值也符合安装器公式
`(1008 | 1) XOR 0xA45F987C`。但缺少 `res.bin1`、保留原始解包分片且所有文件
具有连续提取时间戳，证明这个 `res.v` 只能视为叠加进来的基础运行元数据，
不能证明当前 resource 1 曾通过真实主包的 ID 校验。

因此新目录强化了资源包内容和预期 `res.bin1` 的来源证据，但没有提供匹配的
主包或新 ID 映射；`0x000F983C` 与 `0x00000FA1` 的兼容性冲突保持不变。

当前 `build/mythroad` 中主包已改名为 `gjxqy_v1008.mrp` 与
`gjxqy_v1012.mrp`。两者大小均为 423136，SHA-256 均为
`5513be4147aeb084b6411af5b4343af78943c04a0db7aa0dd8d7014eb95113b7`，
并且逐字节相同；两个 header 的 LE/BE version 都是 1008。也就是说后者只是
文件名带 `v1012`，内容不是另一份 v1012 主包。`test/fixtures/gjxqy.mrp`
仍是 header version 1012 的那份副本，wrapper payload 保持相同。

后续新加的 `test/fixtures/gjxqy_v1012.mrp` 也不是新版本：它与上述两个
`build/mythroad` 文件逐字节一致，大小 423136，SHA-256 为
`5513be4147aeb084b6411af5b4343af78943c04a0db7aa0dd8d7014eb95113b7`，
header 的 LE/BE version 仍都是 1008。包内 `cfunction.ext` 大小 393840，
SHA-256 为 `328fdf9361fbbb4b2720479e863be166de74e1c7742e349df0936d94ed8462aa`；
`+0x37B74` 仍是小端 `3c 98 0f 00 = 0x000F983C`，且扩展全文未出现
4001 的任一字节序。它因此仍与当前资源包的 `fileid.bin=4001`
不匹配。

为区分“真实安装现场”和“预置状态叠加”，又做了一次不改源目录的受控运行：
把 `temp/gjxqy` 复制到临时工作区，并用当前本地 v1008 主包启动；旧服务器地址
映射到没有监听服务的 `127.0.0.1`。流程按稳定像素依次等待音乐选择、主菜单，
再触发新游戏。

```text
artifact  /tmp/skyengine-e2e-TRyLXB
workspace /tmp/skyengine-ws-nmT56M
最终画面  存档选择，52 色
最终 PPM  716062dc5c8a1def9993b7617238d5b1d9911b84b5a889541f241865ee9a85a5
```

运行期间没有 `my_initNetwork`、socket、connect 或 send，工作区也没有生成
`res.bin1`；`res.v` 保持原哈希 `7a105b...`。这证明 slot=1 元数据直接绕过
下载判断并允许先进入存档 UI，裸 `res.p*` 至少在该阶段没有被安装器消费。
因此该正向画面不能反向证明 resource 1 安装成功；它反而说明只看 PPM 会被
预置 `res.v` 误导。目标下载测试不能直接改用此目录，否则根本不会访问本地
复现服务器。

## 2026-07-22 当前工作区复现与独立复核

再次以当前工作区执行，不启用 trace、不使用 xvfb：

```text
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gjxqy/download-res.test.ts --reporter=verbose
artifact  /tmp/skyengine-e2e-OkExWT
workspace /tmp/skyengine-ws-6yZQ0O
duration  45.80s
```

源资源长度/哈希、客户端瞬时 inode 长度/哈希、请求长度、响应长度和六段逻辑帧
断言全部通过；用例只在最终正向 PPM 门槛失败，实际颜色数仍为 3。最终 PPM
SHA-256 是 `c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e`，
人工检查仍明确显示“下载资源失败，请稍后重试 / 按确定键返回游戏”。安装器已
删除 `res_1.mrp`，没有生成 `res.bin1`，`res.v` 未变化。这一轮再次同时证明：
网络下载成功，随后严格资源兼容校验失败。

三个独立只读审计分别复核了服务器协议、当前源码差异和 guest 反汇编，均得到
相同的阻断条件：wrapper slot 1 登记 `0x000F983C`，给定资源包的
`fileid.bin` 是 `0x00000FA1`，安装器在 `+0x3B88..+0x3B94` 将二者作为
opaque ID 直接比较。不能在服务器回包增加字段、改变 descriptor ID 或在模拟器
增加兼容分支来修复这组不配套资产。

同时纠正一个未被证据支持的推断：resource MRP header `+0xD0=1` 按仓库保留
打包器定义只是 MTK/MStar 平台枚举，不是离线分发标志。包内 `start.mr` 是通用
loader，但包内没有它要加载的 `cfunction.ext`；游戏安装器也不读取 `start.mr`，
只读取 `fileid.bin` 与 `res.pN`。因此当前只能把该文件定性为结构有效但 ID
不配套的 resource-1 MRP，不能断言其线上/线下来源，也不能以“格式转换”为由
动态改写 `fileid.bin`。

## 2026-07-22 fixture server 生命周期修正

`tool/jxqy-server.js` 的协议正文已正确，但一次性服务生命周期还有独立缺陷：旧
实现会在 TCP accept 时抢占唯一事务，空健康探针即可阻止游戏请求；非空截断
请求 EOF 会让 `completed` 永久 pending；CLI 完成后也不关闭监听端口。现已：

- 只在完整请求通过全部协议校验后占用唯一事务；
- 对非空截断 EOF 明确 reject，空探针不占用事务；
- 监听前校验 MRP header 声明总长度等于实际长度；
- CLI 在成功或协议失败后都关闭一次性服务器；
- 保留“协议错误明确失败、无猜测响应”的原则，没有增加 fallback。

新增无 SDL 测试 `test/e2e/gjxqy/server.test.ts`。命令
`pnpm vitest run test/e2e/gjxqy/server.test.ts --reporter=verbose --retry=0`
通过 2/2：空探针后分段请求得到 274061 字节完整响应，资源正文仍为原始
273877 字节/`ead50b...30b1`；非空截断请求得到明确 EOF 错误。目标测试已通过
局部 `dnsMap` 注入旧 IP，因此删除了会劫持所有普通运行的全局默认 loopback
映射；同时恢复与本问题无关的 Vitest 全局 retry 配置。

该检查点仍保留目标用例的正向 PPM 门槛，不把已证明的安装失败画面改判为成功，也
尚未把受控修改 `fileid.bin` 的合成包当作最终 fixture。完成安装口径仍需要真实
配套的主包或 resource-1 包；若验收只要求客户端逐字节下载原文件，则当前同一
E2E 已有直接成功证据，但这不能被扩大解释为安装成功。

服务器生命周期修改后重新执行 `cmake --build build -j2`，再以 `--retry=0`
重跑目标用例。构建成功；目标用例 42.66 秒后仍只在第 194 行失败，值仍为
`3 > 16`。新证据为：

```text
artifact  /tmp/skyengine-e2e-VBgSum
workspace /tmp/skyengine-ws-ZqUVty
PPM       c1f9aeee3d898d61efacb5ba23f08df2d14c8531e32dda862e03ac8b103bb47e
res.v     f403000000000000889b5fa4（未变化）
res.bin1  不存在
res_1.mrp 已被安装器删除
```

这证明服务器生命周期修正没有改变网络成功或安装失败的分层结论。附加验证：
`pnpm exec tsc --noEmit`、`node --check tool/jxqy-server.js`、`git diff --check`
及 `ctest --test-dir build --output-on-failure` 均通过；服务器无 SDL 测试仍为 2/2。

## 2026-07-22 生成工具与资产来源收口

三路纯本地子任务又对“能否从仓库规则合法重建 ID”做了独立收口，
没有联网，也没有修改任何资产。结论是：两个 ID 都能从现成二进制中直接
取证，但都不能从本地权威源码、配置或打包元数据重新生成：

- Git 对象库全量扫描覆盖 6772 个对象、3377 个不小于 240 字节的
  blob 和 42 个 MRPG，没有任何 `appid=335001` 的包。目标主包、资源包、
  `res.v` 和 `fileid.bin` 的当前 blob ID 都不在对象库中；仓库也没有
  LFS 对象可以恢复。
- `test/fixtures/gjxqy.mrp` 只是把 v1008 包在文件偏移 `0x48`、
  `0xC7` 的版本低字节改成 1012，其余 423134 字节（包括 EXT）未变。
  `test/fixtures/gjxqy_v1012.mrp` 反而与 v1008 包逐字节相同。两者都不是
  可独立验证的 v1012 主包产物。
- `tool/Mrpeditor.exe` 的 CLR metadata、IL、ASCII/UTF-16 字符串和 11 个
  ManifestResource 中都没有 `fileid.bin`、`res.pN`、4001 或 `0xF983C`。
  `Module1::ReadResource` 只读取 `Mrper.rs` 图片宽度库；该编辑器不是在线
  resource ID 生成器。
- 仓库的 `ToMrp`、Mrpeditor 和 applist 工具都只做通用 MRP 容器封装，
  没有 GJXQY 元数据、调用点或 `fileid.bin` 生成规则。
- 对本机可解析候选去重后，只有当前 GJXQY wrapper 命中这套资源管理器，
  也只有当前 `res_1.mrp` 提供一个真实 `fileid.bin` 样本。没有第二组
  wrapper/resource 对可以支持 `40 * 100 + 1` 或任何其它生成公式。

因此现有本地原始资产的最终可证边界仍是：当前 wrapper 权威声明
`0x000F983C`，当前 resource-1 包权威声明 4001，两者不配套。
没有证据支持在服务器、模拟器或测试中隐式转换它们。要让当前正向 PPM
验收通过，仍需要真实配套资产，或者明确授权使用已标记为派生物的兼容
测试 fixture；后者不能冒充原始发行包。

## 2026-07-22 显式派生 fixture 与最终修复

目标已明确允许“工作区文件不一定正确”并要求真正完成正向安装，因此后续
没有隐式修改用户原包，而是增加独立命名、明确标记来源的
`test/fixtures/gjxqy-derived/res_1.mrp`。原始
`temp/gjxqy-res/res_1.mrp` 保持原样和原哈希。

新增通用工具 `tool/replace-mrp-entry.js`，它按 MRP header/index 结构解析条目，
只允许替换解压长度和压缩长度都不变的单个条目；不包含 GJXQY 检测、
运行时改写或 fallback。重建命令为：

```text
node tool/replace-mrp-entry.js \
  temp/gjxqy-res/res_1.mrp \
  test/fixtures/gjxqy-derived/res_1.mrp \
  fileid.bin 000f983c
```

确定性 gzip 输出与先前受控成功件逐字节相同。派生包与原包只有 10 个
字节不同，全部位于 24 字节的 `fileid.bin` gzip representation 中；
所有 MRP 目录偏移、`res.p0..res.p39` 和资源 payload 保持不变：

```text
original MRP  ead50bb5986a3f53db8ee1c08c213242e45b21acb05e11be0e2536914da630b1
derived MRP   94ebbc1cb0b2c0dea74abe0bcf8b656b9ba3a5eb3d10c83b08c3ae1232fe9448
fileid.bin    00 0f 98 3c
```

`download-res.test.ts` 现在同时锁定：派生源包哈希、客户端实际写入的下载
哈希、`res.bin1` 长度/哈希、安装后 `res.v` 精确内容和正向 PPM 的稳定
场景像素。不再用一个只表示“颜色较多”的弱断言。左下确认图标会在截图间
瞬时变化，所以不锁定整幅 PPM 哈希；标题、存档框和山水背景像素在两次
正向运行中一致，而已知失败画面在这些坐标全为黑色。

最终目标用例不使用 xvfb、不开启 trace、禁用 retry 后通过：

```text
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gjxqy/download-res.test.ts \
  --reporter=verbose --retry=0

artifact   /tmp/skyengine-e2e-VOGsHc
workspace  /tmp/skyengine-ws-IIka3o
duration   44.74s
result     1 file / 1 test passed
```

保留工作区中的安装证据：

```text
res_1.mrp  273877 bytes, 94ebbc1cb0b2c0dea74abe0bcf8b656b9ba3a5eb3d10c83b08c3ae1232fe9448
res.bin1   1294042 bytes, 11c0543e9e213f635b59ae9dee989078bac87f94257c306a3dc5a34f16abba07
res.v      f403000001000000899b5fa4
PPM        存档选择场景，52 色，本次哈希 d12ef6770...2777cd43
```

standalone `tool/jxqy-server.js` 的默认资源路径也改为这个已跟踪派生
fixture；`GJXQY_RESOURCE_PATH` 仍可显式覆盖。服务器只原样发送选定文件并
回显请求 ID，不会动态改写 MRP。

最后执行全量兼容回归：

```text
pnpm vitest run test/e2e --reporter=verbose --retry=0
Test Files  32 passed (32)
Tests       56 passed (56)
Duration    338.56s
```

GJXQY 下载安装在全量套件中再次通过（47.65s），其它网络、插件、资源
安装和渲染用例也全部通过。附加验证：`cmake --build build -j2`、
`ctest --test-dir build --output-on-failure`、`pnpm exec tsc --noEmit`、两个 Node 语法检查、
fixture server 2/2 以及 `git diff --check` 均通过。
