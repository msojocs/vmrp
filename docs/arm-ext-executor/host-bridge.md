# `mr_table` 宿主桥接

## 1. 桥接方式

Mythroad 原生 EXT 期望拿到一个包含 150 个槽的函数/数据表。执行器不能把 host 函数指针直接写给 32 位 ARM guest，因此为每个函数槽建立一个 guest 跳转地址：

```text
table[N] = EXT_TABLE_ADDR + N * 4
```

这是 `init_table()` 的初始默认值。随后数据槽、internal table、port table、包名和堆状态槽会被覆盖为真实 guest 地址；只有仍保留桥地址的槽才会在作为代码执行时进入 `hook_table()`。

guest 调用该地址时 Unicorn 的 code hook 进入 `hook_table()`：

1. 根据地址计算 index；
2. 同步可能刚完成的 private child；
3. 读取 R0-R3；
4. 从 `aex_table_handlers[index]` 选择 handler；
5. handler 转换 guest 指针并调用宿主 Mythroad/DSM；
6. 把返回值写入 R0，并返回 LR。

未实现槽会打印参数并返回 `MR_IGNORE`。这能保留平台“不接管该能力”的语义，但不能作为关键 ABI 的长期兜底。

## 2. 调用约定

`AexTableCtx` 保存 index、R0-R3 和返回值。前四个参数直接来自寄存器，第五个及后续参数通过 `arg_read()` 从 guest SP 获取。

handler 必须遵循：

- 所有 guest 指针先验证再解引用；
- 输出指针指向 guest 地址槽，不是 host `void **`；
- host 返回缓冲需要复制到 guest，除非它已经位于 guest 可见 RAM；
- 返回 guest 指针时使用 guest 地址；
- 改写 code/GOT 后失效 Unicorn cache；
- timer、模块和屏幕调用使用统一 owner API。

## 3. 表项总览与支持状态

Mythroad 期望的槽位定义在 `src/mythroad/mythroad.c` 的 `_mr_c_function_table_init()`，执行器实际写入 guest 的值由 `init_table()` 决定，当前 handler 分发表在 `src/arm_ext/aex_table.c`。三者需要一起核对：原生表里是函数或 `NULL`，不等于执行器 guest 表里就是 host 函数指针或 0。

当前 150 个槽可精确分成三类：100 个有 `AexTableHandler`，32 个被替换为 guest 数据/表地址，余下 18 个保留桥地址但没有 handler。最后一类被 guest 调用时会记录 `not implemented` 并返回 `MR_IGNORE`。

| 槽位 | 执行器状态 | 典型接口或数据 |
| --- | --- | --- |
| `0..20` | handler | malloc/free/realloc、C 运行库、格式化、随机数 |
| `21` | fallback | 原生表为 `NULL`；guest 表当前仍是桥地址，调用返回 `MR_IGNORE` |
| `22` | handler | `mr_stop_ex` |
| `23..24` | guest 指针 | internal table、port table |
| `25..61` | handler | 模块注册、日志、图形、timer、平台、文件、设备、音频、短信 |
| `62..68` | fallback | WAP/menu；其中原生 table[66] 为 `NULL` |
| `69..90` | handler | dialog/text/edit/window、screen info、network/socket |
| `91..112` | guest 指针/数据 | screen、资源数组、包名、RAM 包、应用堆、SMS 配置 |
| `113..115` | handler | MD5 init/append/finish |
| `116..117` | fallback | load/save SMS config |
| `118..121` | handler | DispUpEx、DrawPoint、DrawBitmap、DrawBitmapEx |
| `122..127` | handler | DrawRect、DrawText、BitmapCheck、readFile、wstrlen、registerAPP |
| `128..129` | fallback | DrawTextEx、EffSetCon |
| `130..134` | handler | TestCom、TestCom1、c2u、div、mod |
| `135..136` | guest 数据 | LG_mem_min、LG_mem_top |
| `137` | fallback | updcrc |
| `138..140` | guest 指针/数据 | start parameter、SMS return flag/value |
| `141` | fallback | unzip |
| `142..143` | guest 数据 | exit callback、exit callback data |
| `144` | fallback | mr_entry |
| `145` | handler | platDrawChar |
| `146` | guest 数据 | `LG_mem_free` 头节点 |
| `147..149` | fallback | transbitmapDraw、drawRegion、保留；原生 table[149] 为 `NULL` |

这里的 `fallback` 表示执行器当前行为，不表示对应 Mythroad 能力在设计上可有可无。

表的“存在”与执行器“已经实现 handler”是两回事。新增 handler 前应先确认上游函数签名、返回语义、guest 指针所有权和异步回调方式。

## 4. 关键表项

### table[0..2]：应用分配器

这三个槽必须保持 Mythroad first-fit 语义，并与 table[125] 使用同一个应用可见堆。不能直接改成 host `malloc/realloc/free`。

### table[3] / table[14]：memcpy/memset

除了普通内存操作，它们还可能覆盖已经重定位的 GOT。handler 在操作后根据 snapshot 恢复被覆盖的 bridge 指针，但只处理确认为 table 地址的槽，并尊重模块保护规则。

### table[25]：模块注册

table[25] 负责建立 P/helper/module ownership，是 wrapper 与 primary/child 之间的关键边界。它还需要识别 nested loader 的 LR、设置 child R9、记录首次 primary 初始化请求。

### table[29] / table[118..124]：图形

图形 handler 不能直接“画到窗口就返回”。它们需要：

- 判断当前调用模块是否有权提交可见画面；
- 把 guest bitmap 写回共享 screen cache；
- 记录 damage 和 foreground cover；
- 处理 RGB565、stride、ROP、透明色、旋转和裁剪；
- 避免被前台 child 覆盖的 primary 提交穿透到窗口；
- 在 child 关闭后恢复底层画面。

table[121] DrawBitmapEx 的两个 guest 位图描述符含 4 字节 ARM 指针，在 64 位
宿主上不能强转为原生结构。handler 逐字段读取 12 字节描述符与 10 字节矩阵，
验证源/目标区间和可逆矩阵后再构造宿主描述符。主 framebuffer 写入继续经过
damage/foreground owner 跟踪；离屏目标只更新 guest 缓冲。

### table[31]/[32]：宿主 timer

除了调用 `mr_timerStart/Stop`，还必须同步 guest 数据槽、`mr_timer_state`、`host_timer_pending` 和 timer owner。

### table[38]：`mr_platEx`

这是一个多形态接口，输入/输出所有权随 code 变化。当前实现特别处理屏幕扩展内存 1014/1015，并正确区分：

- host 在 caller-owned guest buffer 中原地写入；
- host 返回新分配缓冲；
- output 为空；
- output_len 为 0 但指针身份仍有效。

新增 code 时不能假定所有输出都需要复制。

### table[44]：`mr_read`

该槽同时支持真实 host fd 和 MRP cache VFD。大块读取可能恢复整个模块映像，因此 handler 还会：

- 识别覆盖了哪些注册 code；
- 退役被数据覆盖的旧模块；
- 清 Unicorn TB cache；
- 在确认为 primary dump/restore 时恢复 GOT 和间隙映射。

普通下载数据不能触发可执行模块 GOT 修补。

### table[81..90]：网络

网络桥接把 guest socket id 映射到 host socket，并维持 Mythroad 的 socket 返回语义。table[83] 当前忽略 guest ARM callback，改用 `my_getHostByName(NULL, ...)` 同步解析并直接返回 IP；不能把 ARM 函数地址交给宿主 DNS 线程调用。未来若恢复异步 DNS/网络回调，必须先投递回执行器所属线程，再通过 `arm_ext_invoke*()` 或明确的 helper 协议进入 guest。

### table[125]：`mr_readFile`

这是最复杂的资源桥之一。处理流程包括：

1. 根据 LR 确定调用模块；
2. 选择该模块自己的 table[100]/[104]/[105]；
3. 从 host MRP、RAM MRP 或 VM 当前包读取资源；
4. 保留直接指向 RAM package entry 的 native 指针语义；
5. 其它数据通过应用 first-fit 堆分配；
6. 更新 gzip 相关数据槽；
7. 保存最近文件副本，供 cacheSync/private loader 识别；
8. 必要时镜像到相邻 slot，兼容私有 loader ABI；
9. 记录资源所属包，供后续 child sibling read 使用。

不要把 table[125] 简化成 `_mr_readFile()` 加一次 memcpy。

### table[131]：`_mr_TestCom1`

该槽是 private loader/cacheSync 的重要通知点。执行器会识别 guest 是否已经在 staging 区建立有效 extChunk，避免用原始缓存文件覆盖已经重定位的运行时模块映像。

## 5. 数据表与 module record

wrapper 的主 table 位于 `EXT_TABLE_ADDR`。private child 可能有自己的 module record，它通常镜像部分 table 槽并加入 child 私有状态。

桥接函数通过 LR 识别调用来自哪个模块，并在需要时改用该 record：

- table[100] 当前包名；
- table[104]/[105] RAM package；
- table[125] readFile 回调；
- table[3] memcpy 等基础桥；
- table[131] cacheSync/loader 通道。

private record 可能被 dump、memset 或错误 bridge copy 覆盖。修复时必须验证 child 自身的 RW 布局和重定位指令，不能拿另一个模块的 GOT 偏移套用。

## 6. 文件和包缓存

`aex_pack.c` 在加载时预解析 MRP 条目，建立 `MrpCacheEntry`。未压缩条目可以通过 VFD 暴露给 guest，避免每次 open/read 都重新扫描大包。

缓存规则：

- cache 只优化读取，不改变 Mythroad 文件语义；
- basename 和带目录路径需要区分；
- 包被下载替换后，匹配缓存的键必须包含内容/包头信息；
- RAM package 的生命周期由 owning module 记录；
- child 关闭后不能让旧 package context 污染新的 child。

## 7. 新增 handler 的检查清单

1. 在 `src/mythroad/mythroad.c` 确认槽号和真实函数签名。
2. 明确每个参数是标量、guest 指针、guest 指针槽还是函数地址。
3. 对所有输入区间使用完整映射校验。
4. 明确 host 返回内存的所有者和释放方式。
5. 明确返回给 guest 的地址生命周期。
6. 判断调用是否改变模块、timer、屏幕、文件或网络全局状态。
7. 为异步回调设计回到 VM 线程的队列。
8. 未支持的行为返回正确的 `MR_FAILED/MR_IGNORE/MR_WAITING`，不要随意返回成功。
9. 加入窄日志或 E2E 断言，避免依赖全量 trace。
10. 在 sanitizer、不变量和相关真实 MRP 流程下验证。

完成后还要更新本节的精确支持矩阵。只在 `_mr_c_function_table_init()` 中存在函数名、或只让 `init_table()` 写入桥地址，都不代表槽位已经可用；必须确认 `aex_table_handlers[index]` 非空且行为有验证覆盖。
