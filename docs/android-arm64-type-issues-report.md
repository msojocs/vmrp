# Android arm64 + Flutter MRP 黑屏类型问题分析报告

## 结论摘要

参考提交 `b4c86ab3a01cc7490b324daff66d6320316c918c` 的修复点非常明确：在 Android arm64 的 LP64 ABI 下，`size_t` 是 8 字节，而项目内 Mythroad `int32/uint32` 仍是 4 字节。旧代码把 `int32 *` 强转成 `size_t *` 传给 `mr_L_checklstring()`，被调函数会写入 8 字节长度，直接覆盖 4 字节局部变量之后的栈内容。这类错误在 32 位环境通常不会暴露，在 arm64 上会表现为网络崩溃、回调状态错乱、Lua 栈状态异常，进一步可能导致 MRP 启动黑屏。

当前显式的 `mr_L_checklstring(..., (size_t *)&x)` 模式在活动代码中已基本清除，但项目仍存在多类同源类型风险：

1. `size_t/long/void *` 等平台宽度类型与 MRP 32 位 ABI 类型混用。
2. host 指针通过 `uint32` 或 Lua `number` 往返，在 arm64 上可能截断。
3. ARM EXT 代码同时存在“guest 32 位地址”和“host 指针”的两套语义，但类型名都常落到 `uint32`/`void *`，边界不清。
4. `mrc` offset 生成工具使用 `long` 定义 `uint32/int32`，在 LP64 host 上可能生成 64 位布局，污染 32 位 guest ABI。
5. 多处 `size_t` 到 `int32/uint32` 的长度下转没有显式上界检查。

这些问题不一定都会立刻 crash。对“部分 MRP 启动黑屏”而言，更危险的是静默失败：资源读取失败、EXT 初始化回调丢失、timer/event 函数地址错误、网络异步回调跳错地址、屏幕缓冲或宽高表项错位，都会让主循环还在跑但首屏不绘制。

Flutter 集成路径会把 native Mythroad、DSM、ARM EXT executor 一起打进共享库：`CMakeLists.txt:388-401`。Flutter 侧通过 FFI 直接读取 `Pointer<Uint16>` 屏幕缓冲：`docs/flutter-integration.md:183-184`，并要求所有 `skyengine_api_*` 调用在同一线程：`docs/flutter-integration.md:724-725`。因此 native 层的地址、回调、ABI 错误即使没有立刻崩溃，也会在 Flutter 侧表现为首帧不刷新或黑屏。

## 参考提交说明

提交 `b4c86ab3a01cc7490b324daff66d6320316c918c` 标题为 `fix: 安卓网络崩溃`，只修改两个文件：

| 文件 | 修改类型 | 含义 |
| --- | --- | --- |
| `src/mythroad/src/lib/mr_tcp_target.c` | `int32 size` 改为 `size_t size`，传 `&size`；发送长度显式转 `int` | 修复 TCP/UDP send 路径把 4 字节局部变量伪装成 8 字节输出参数的问题 |
| `src/mythroad/src/lib/mr_strlib.c` | 去掉 `(size_t *)&l` 伪强转，直接传 `&l` | 修复字符串库 `str_sub/str_lower` 的同类输出参数错误 |

`mr_L_checklstring()` 的声明和实现都要求第三个参数是 `size_t *`：

- `src/mythroad/include/mr_auxlib.h:28`
- `src/mythroad/src/lib/mr_auxlib.c:159-163`

修复后的关键调用点：

- `src/mythroad/src/lib/mr_tcp_target.c:92-109`
- `src/mythroad/src/lib/mr_tcp_target.c:136-153`
- `src/mythroad/src/lib/mr_strlib.c:459-477`

根因不是“网络模块本身不稳定”，而是输出参数类型不匹配导致栈写越界。这个提交应作为后续 arm64 排查的基准规则：**输出参数必须使用真实目标类型存储，不能通过强转让编译器闭嘴。**

## Android arm64 上为什么更容易触发

Android arm64 采用 LP64：

| 类型 | arm32 常见大小 | arm64 LP64 大小 | 影响 |
| --- | ---: | ---: | --- |
| `int` | 4 | 4 | Mythroad `int32` 仍安全 |
| `long` | 4 | 8 | 任何“long 也是 4 字节”的假设失效 |
| `size_t` | 4 | 8 | 字符串长度、内存长度输出参数变宽 |
| `void *` | 4 | 8 | host 指针不能放进 `uint32` |
| Lua `number` / `double` | 8 | 8 | 可精确表达 53 位整数，但不等于安全指针类型 |

仓库中已有注释能佐证历史设计偏 32 位 ABI：

- `src/mythroad/include/type.h:8-10` 将 Mythroad `int32/uint32` 定义为 `int/unsigned int`，当前是固定 4 字节。
- `docs/cfunction_x86_refactor.md:181-185` 明确写到 MRP ABI 是 32 位，64 位会发生指针与 `uint32` 互转截断。
- `test/stubs.c:208-224` 也说明测试桩必须把 fake memory 放在低 32-bit 地址，因为 MRP 代码会把 `char *` 截断为 `uint32`。

## 风险分级

| 等级 | 类型 | 对黑屏/崩溃的影响 |
| --- | --- | --- |
| P0 | 输出参数宽度不匹配、host 指针截断后回调、ABI 布局错位 | 可直接 crash、跳错函数、栈破坏，或 EXT 初始化失败 |
| P1 | guest 地址和 host 指针混用、Lua number 传指针、`size_t` 无检查下转 | 可能只在部分 MRP、部分资源大小、部分地址分配布局下触发 |
| P2 | 命名误导、编译警告不足、工具链无静态断言 | 本身不一定触发，但会让同类问题反复引入 |

## 已确认问题与证据

### 1. `size_t *` 输出参数被 4 字节变量接收

状态：参考提交已修复已知活动调用点，但这是最高优先级防回归项。

证据：

- `mr_L_checklstring()` 声明为 `size_t *l`：`src/mythroad/include/mr_auxlib.h:28`
- 实现会执行 `*len = mrp_strlen_t(...)`：`src/mythroad/src/lib/mr_auxlib.c:159-163`
- 参考提交前 `mr_tcp_target.c` 使用 `int32 size` 并传 `(size_t *)&size`，arm64 上会写 8 字节覆盖栈。
- 修复后 `meth_send()` 使用 `size_t size`：`src/mythroad/src/lib/mr_tcp_target.c:92-109`
- 修复后 `meth_sendto()` 使用 `size_t size`：`src/mythroad/src/lib/mr_tcp_target.c:136-153`
- 修复后 `str_sub/str_lower` 使用 `size_t l`：`src/mythroad/src/lib/mr_strlib.c:459-477`

影响链路：

1. Lua 传入字符串。
2. C 层通过 `mr_L_checklstring()` 取字符串和长度。
3. 被调函数按 `size_t` 写 8 字节。
4. 调用方实际只准备了 4 字节 `int32`。
5. 邻近栈变量如 `sent/start/end/tcp`、Lua 临时状态或返回地址附近数据被污染。
6. 网络 send、字符串处理或后续 VM 调度表现为 crash、无响应、黑屏。

建议：

- 禁止任何输出参数强转，例如 `(size_t *)&x`、`(uint32 *)&ptr`。
- 对 `mr_L_checklstring/mr_L_optlstring/mrp_Chunkreader` 这类 API 加专门静态扫描。
- 编译开启 `-Werror=incompatible-pointer-types`。如果历史代码噪音过大，至少对 `src/mythroad/src/lib/*.c` 和新代码启用。

### 2. `long` 被当成 32 位整数使用

状态：相关定义已按当前项目标准改为固定宽度类型；仍需继续排查业务代码中把 `long` 当作 32 位值使用的场景。

证据：

- `src/mythroad/include/mr_conf.h:39`：`MRP_INTEGER` 已改为 `int32_t`。
- `src/mythroad/include/mr_conf.h:122-126`：`MRP_UINT32/MRP_INT32/MRP_MAXINT32` 已改为 `uint32_t/int32_t/INT32_MAX`。
- `src/mythroad/src/lib/mr_strlib.c:24`：`sint32` 已从 `long` 改为 `ptrdiff_t`，匹配“signed size_t”语义。
- `mrc/mrc_base.h:16-17`：`uint32/int32` 已改为 `uint32_t/int32_t`，避免 offset 工具受 LP64 host 污染。

影响：

- 在 Android arm64 上 `long` 是 8 字节；任何名字暗示 32 位的类型都不能再用 `long` 定义。
- 如果这些类型进入结构体、offset 生成、二进制序列化或 ARM guest ABI 表，字段大小和偏移会与真实 ARM32 MRP ABI 不一致。
- `strlib.c` 的 `sint32` 名称仍然容易误导，但底层已改为 `ptrdiff_t`，行为上跟随平台指针差值宽度。

建议：

- 表达 MRP ABI 宽度时统一使用 `int32_t/uint32_t` 或项目现有固定宽度 `int32/uint32`。
- 表达平台长度时使用 `size_t/ptrdiff_t/ssize_t`。
- 若 `MRP_INTEGER/MRP_INT32/MRP_UINT32` 必须保持 Lua 旧实现语义，应在头文件旁明确“至少 32 位，不等于 MRP ABI 32 位”，并禁止进入 ABI 结构体。
- 增加静态断言：`sizeof(int32) == 4`、`sizeof(uint32) == 4`、所有 ABI table 字段偏移等于 ARM32 期望值。

### 3. `size_t` 到 `int32/uint32` 的无检查下转

状态：仍存在，P1。

证据：

- `src/mythroad/mythroad.c:2952-2958`：`size_t input_size` 取 Lua 字符串长度后直接 `(int32)input_size`。
- `src/mythroad/mythroad.c:3627-3631`：`size_t len_sz` 直接 `(int32)len_sz` 传入 `_mr_TestCom1()`。
- `src/mythroad/src/lib/mr_iolib_target.c:279-286`：`size_t l` 直接 `(uint32)l` 传给 `mr_write()`，再与 `(int32)l` 比较。
- `src/mythroad/src/lib/mr_strlib.c:408-445`：多处 `size_t l_sz` 转 `int32 l` 后参与字符串长度逻辑。

影响：

- 普通 MRP 资源通常不会超过 2 GiB，这类问题不一定频繁触发。
- 但黑屏问题往往发生在异常资源、解包失败、错误长度字段或 Lua 字符串承载二进制 blob 时；无检查下转会把大长度变成负数或小正数，导致截断读写、越界、资源加载失败。

建议：

- 引入统一 helper，例如 `mr_size_to_int32_checked(size_t n, int32 *out)` 和 `mr_size_to_uint32_checked(size_t n, uint32 *out)`。
- 下转前必须检查 `n <= INT32_MAX` 或 `n <= UINT32_MAX`，失败时返回 `MR_FAILED` 或 Lua error。
- 对发送长度、文件写入长度、平台扩展输入长度分别使用目标 API 的真实上限，而不是直接 cast。

### 4. host 指针经 `uint32` 截断

状态：仍存在，P0/P1；是否触发取决于传入值到底是 guest 地址还是 host 指针。

证据：

- `src/mythroad/mythroad.c:183-187`：native event/timer/stop/pause/resume 函数保存为 `uint32`。
- `src/mythroad/mythroad.c:3012-3027`：`input1` 被 `(uint32)` 保存为这些 native callback。
- `src/mythroad/mythroad.c:4141-4143`：事件回调再以 `uint32` 调 `native_ext_callback3()`。
- `src/mythroad/mythroad.c:2138`：bitmap 指针通过 `mrp_pushnumber()` 返回 Lua。
- `src/mythroad/mythroad.c:3541`：`filebuf` 指针通过 Lua `number` 返回。
- `src/mythroad/mythroad.c:2035`：`MRF_BitmapShowEx` 从 Lua number 取回 `uint16 *` 后进入绘制链路。
- `src/mythroad/src/lib/mr_strlib.c:1038-1044`：`str_subV()` 把字符串 buffer 指针作为 Lua number 返回。
- `src/mythroad/src/lib/mr_strlib.c:1242-1246`：`str_pupdate()` 从 Lua number 取回指针并写内存。

影响：

- 如果这些值是 ARM guest 地址，32 位是合理的。
- 如果这些值是 host malloc/mmap 指针，Android arm64 上高 32 位会丢失；后续解引用或回调会跳到错误地址。
- Lua `number` 是 `double`，可以精确表达 53 位整数，但这不解决 C 指针语义问题；更重要的是代码没有区分“地址值”属于 guest 还是 host。

建议：

- 引入类型别名区分语义：`typedef uint32_t guest_addr_t;`、`typedef uintptr_t host_ptr_bits_t;`。
- 所有 ARM guest 函数地址、guest buffer 地址使用 `guest_addr_t`，只能通过 `arm_ptr()` 转 host 指针。
- 所有 host 指针 round-trip 使用 `uintptr_t` 或不透明 handle 表，不得放入 `uint32`。
- Lua 层如必须暴露 host 指针，应改成 userdata/lightuserdata 或 handle，不再用 number。
- 对现阶段无法重构的路径加运行时截断检测：`if ((uintptr_t)p > UINT32_MAX) LOGE/return MR_FAILED`，至少让黑屏变成可诊断错误。

### 5. 异步网络回调的函数指针截断

状态：仍存在可疑路径，P0；与参考提交同属网络相关，但根因不同。

证据：

- `src/network.c:452-458`：`my_initNetworkAsync()` 将 `data->cb` 和 `data->userData` cast 成 `uint32_t` 后传给 `bridge_dsm_network_cb()`。
- `src/network.c:539-545`：`my_getHostByNameAsync()` 同样截断 `data->cb/userData`。
- `src/include/bridge.h:15`：桥接函数签名把 callback 地址和 `userData` 固定为 `uint32_t`。
- `src/native_dsm_funcs.c:1020-1023`：`bridge_dsm_network_cb()` 再把 `uint32_t addr` cast 回函数指针并调用。
- `src/include/types.h:108-109`：`MR_INIT_NETWORK_CB/MR_GET_HOST_CB` 是 C 函数指针类型。
- `src/mythroad/dsm.c:1146-1161`：DSM 网络回调会把 `userData` 当 `networkData_st *` 解引用，再调用其中的 `cb`。
- `src/mythroad/dsm.c:1165-1189`：`networkData_st *data` 作为异步 `userData` 传给平台层。

影响：

- 如果 `cb` 是真正 host 函数指针，arm64 上经 `uint32_t` 截断后回调必然不可靠。
- 如果 `cb` 实际承载 ARM guest 函数地址，则不应把它声明/保存为 C 函数指针，也不应在 async pthread 中直接当 host 函数调用。
- 如果 `userData` 是 `networkData_st *` 这样的 host 指针，被截断后即使 callback 地址碰巧可用，`network_cb()` 也会解引用错误对象。
- Flutter/Android 环境下网络初始化或 DNS 经常发生在启动阶段；回调丢失或跳错地址会导致资源下载、登录初始化、状态机推进失败，表现为黑屏而不是立即崩溃。

补充观察：

- `src/arm_ext_executor.c:1741-1756` 的 ARM EXT 网络路径目前对 `mr_initNetwork/getHostByName` 使用同步或 `cb=NULL` 路径，规避了一部分异步 guest 回调问题。
- 但 `src/native_dsm_funcs.c:179` 仍存在把 `uint32_t cb` cast 成 `MR_GET_HOST_CB` 的接口入口，应明确它只允许低 32 位 guest thunk，还是需要改为 handle/队列。

建议：

- `bridge_dsm_network_cb()` 不应接收 `uint32_t addr` 表示 host 函数指针。若是 host callback，参数应是 `MR_*_CB cb, void *userData` 或 `uintptr_t`。
- 若是 guest callback，async 线程只产出结果，不直接调用 guest 地址；结果应投递回 VM/Unicorn 所在线程，由 `guest_addr_t` 调度执行。
- 网络模块结构体中的 callback 字段按语义拆成 host callback 和 guest callback，禁止同一字段既是函数指针又是 32 位地址。

### 6. ARM EXT 低地址映射和 guest/host 地址混用

状态：仍存在，P1；在 Android arm64 上与系统 mmap 策略有关。

证据：

- `src/include/arm_ext_internal.h:11-19`：EXT guest 地址空间固定为 `0x00010000` 等 32 位地址。
- `src/include/arm_ext_internal.h:151-154`：`arm_ptr()` 明确把 `uint32_t addr` 映射到 `m->mem + offset`，这是正确的 guest 地址模型。
- `src/arm_ext_executor.c:115-118`：`arm_addr()` 通过 `ptr - m->mem + EXT_BASE_ADDR` 生成 guest 地址。
- `src/arm_ext_executor.c:2396-2416`：加载 EXT 时尝试把 ARM 内存 mmap 到固定 `EXT_BASE_ADDR`，注释说明希望“ARM 虚拟地址等于宿主指针”。
- `src/arm_ext_executor.c:2582-2588`：对某些 bootstrap 参数直接 `input_addr = (uint32_t)(uintptr_t)input`。
- `src/mythroad/mythroad.c:3388-3391`：`mr_cacheSync()` 先把 `input1` cast 成 `uint32` 对齐，再传给 host cache sync。

影响：

- 当固定低地址 mmap 成功时，部分“guest 地址等于 host 指针”的历史假设能继续工作。
- Android arm64 上低地址固定映射不一定可用；一旦 fallback 到普通 `calloc/malloc`，host 指针大概率高于 4 GiB，任何 `uint32` 截断都会失效。
- `mr_cacheSync`、EXT load/call、Lua 返回 buffer 指针、wrapper 传参都可能受影响，黑屏常见于 EXT 初始化、JIT/cache 同步或首屏资源解包阶段。

建议：

- 不把低地址映射作为正确性前提，只作为兼容优化。
- EXT 内部统一使用 guest 地址；host 访问必须经过 `arm_ptr()`。
- `input_addr = (uint32_t)(uintptr_t)input` 这类 fallback 需要加注释约束和断言：只允许非指针标量，禁止 host pointer。
- `mr_cacheSync()` 的 host 入口应接收原始 `void *` 做 host 行为，guest cache sync 另走 `guest_addr_t`。

### 7. mrc offset 生成可能受 LP64 布局污染

状态：仍存在，P0/P1，取决于这些 offset 文件是否在 arm64 host 重新生成并参与打包。

证据：

- `mrc/mrc_base.h:16-17` 已把 `uint32/int32` 固定为 `uint32_t/int32_t`。
- `mrc/mr_table_offsets.c:3`、`mrc/mr_c_function_offsets.c:3`、`mrc/mrc_extChunk_offsets.c:3` 自定义 `offsetof` 返回 `uint32`。
- `mrc/mr_table_offsets.c:58-66` 和 `mrc/mr_table_offsets.c:126-133` 中表结构包含大量 `void *`、`int32 *` 字段。
- `src/mythroad/include/mr_helper.h:353-508` 的 `mr_c_port_table/mr_c_function_st` 结构含大量函数指针和数据指针。
- `src/mythroad/include/mr_helper.h:642-650` 的 `mrc_extChunk_st` 注释写死 `0x04/0x08/0x0c` 等 32 位 offset，但字段实际包含函数指针和 `uint8 *`。
- `mrc/asm/cfunction.ext.s:34-37`、`mrc/asm/cfunction.ext.s:259-264` 显示真实 ARM 汇编按 4 字节槽访问，例如 `timerStart=0x7c`、`timerStop=0x80`、`mr_screen_w=0x170`、`mr_screen_h=0x174`。

影响：

- 在 LP64 host 上，`void *` 和 `long` 是 8 字节；如果 offset 工具直接按 host 编译运行，生成的偏移会变成 64 位布局。
- 头文件里的 32 位 offset 注释与 arm64 C 结构实际布局不一致，说明这些结构不能直接作为 host ABI 布局使用。
- ARM EXT 汇编仍按 32 位槽读写，表项偏移错位会导致 timer、screen size、网络函数、文件函数全部取错。
- 首屏黑屏的典型路径：EXT wrapper 读取错误 screen width/height 或错误函数表项，绘制/定时器/资源加载不执行。

建议：

- offset 生成必须固定为 ARM32 ABI，不能依赖当前 host ABI。
- 最低要求是在 offset 生成程序中加入 `static_assert(sizeof(void*) == 4)`；若在 64 位 host 上运行应直接失败。
- 更稳妥是把 table layout 定义改为 `uint32_t` 槽数组或显式 `guest_addr_t`，不要使用 host `void *` 参与 guest ABI offset。
- 将关键 offset 与 `mrc/asm/cfunction.ext.s` 中已知值做自动校验。

### 8. native memory 低地址依赖

状态：仍存在，P1。

证据：

- `src/native_dsm_funcs.c:124-145`：仅在 Linux x86_64 使用 `MAP_32BIT`，Android arm64 走普通 `malloc()`。
- `test/stubs.c:208-224`：测试桩明确要求 fake memory 放在低 32-bit 地址。
- `src/mythroad/mem.c:37-43`：内部 allocator 使用 host base 指针加 32 位 offset，offset 模型本身可行。
- `src/mythroad/mem.c:121-123`：free list 保存的是相对 `LG_mem_base` 的 32 位偏移。

影响：

- allocator 内部用 offset 是合理的；风险出现在把 `Origin_LG_mem_base/LG_mem_base` 或分配出来的 host 指针暴露给 MRP 32 位字段、Lua number 或 EXT。
- Android arm64 普通 `malloc()` 不保证低 32 位地址，历史“指针可放进 uint32”的假设会随机失败。

建议：

- 明确 native memory 对外暴露的是 guest offset/handle 还是 host pointer。
- 如必须兼容低地址指针旧逻辑，Android arm64 需要显式 mmap 低地址并验证成功；失败时禁止继续启动，而不是继续进入黑屏。
- 长期应移除低地址依赖，统一走 offset/handle。

### 9. 长度和符号转换缺少统一边界

状态：仍存在，P1；异常资源、畸形 MRP 包、EXT 输出异常时可升级为 P0。

证据：

- `src/fileLib.c:84-101`：POSIX `read/write` 返回值是 `ssize_t`，当前落入 `int32_t`。
- `src/fileLib.c:134-139`：`stat.st_size` 是 `off_t`，当前直接作为 `int32_t` 返回。
- `src/fileLib.c:238-240`：`readFile()` 未检查 `my_getLen()` 的负值就 `malloc(len)`，负数会转成巨大 `size_t`。
- `src/arm_ext_executor.c:796-823` 和 `src/skyengine.c:132-158`：`ftell()` 返回 `long`，后续多次 `(uint32_t)sz` 参与 MRP 边界判断。
- `src/arm_ext_executor.c:834-854` 和 `src/skyengine.c:67-90`：zlib 输出长度从 `total_out` 下转到 `uint32`，扩容 `cap * 2` 缺少溢出检查。
- `src/arm_ext_executor.c:1578-1616`：`mr_platEx` 的 `host_output_len` 用于 `arm_alloc((uint32_t)host_output_len + 1)` 和 `memcpy()`，缺少 `0..INT32_MAX` 与 `+1` 溢出检查。
- `src/arm_ext_executor.c:2814-2819`：ARM EXT 写回的 `uint32_t arm_output_len` 直接转 `int32`。
- `src/mythroad/mythroad.c:3576-3591`：调用侧只判断 `output && output_len`，负 `output_len` 进入 `mrp_pushlstring()` 时会再转成巨大 `size_t`。
- `src/arm_ext_executor.c:1761-1768`：ARM 网络参数 `r2` 是 `uint32_t`，直接转 `int` 传给 `mr_recv/mr_send/mr_sendto`。
- `src/mythroad/src/mr_dump.c:35-49` 写 host `size_t`；`src/mythroad/src/mr_undump.c:72-75` 固定读 4 字节 `uint32`，预编译 chunk 长度格式在 LP64 下不一致。
- `src/mythroad/dsm.c:1117-1125` 把 `int32 *output_len` 强转成 `uint32 *` 给 `UCS2BEStrToGBStr()`；`src/mythroad/encode.c:183-199` 被调函数按 `uint32` 写输出长度。
- `src/arm_ext_executor.c:2320-2328`、`src/skyengine_api.c:80-88`：屏幕尺寸乘法先在 `int32/int` 中完成，再转 `uint32_t/size_t`。

影响：

- 这些问题不一定在正常小资源上触发，但会让错误长度从“加载失败”变成“巨大分配、越界读写或静默截断”。
- 对黑屏来说，最相关的是资源包解析失败、EXT 输出字符串长度变负、网络收发长度变负、屏幕缓冲大小错误；这些都会打断首屏资源或绘制链路。
- 参考提交的 `size_t *` 栈覆盖是“输出宽度不匹配”；本节问题是同一根因在长度值域上的表现：平台宽度/符号进入 32 位 ABI 前没有显式检查。

建议：

- 引入 checked cast helper：`size_t/off_t/ssize_t/long/uLong -> int32/uint32/int` 必须统一走范围检查。
- 文件长度统一规则：`0 <= st_size <= INT32_MAX` 后才能返回 MRP 长度；`readFile()` 在 `malloc` 前必须拒绝 `len <= 0`。
- zlib 扩容用 `size_t` 管理容量，并检查乘法、加法和写回 32 位长度的范围。
- ARM EXT 输出长度读回后先拒绝 `> INT32_MAX`，再写给 `int32 output_len`；调用 `mrp_pushlstring()` 前必须保证长度非负。
- 网络 `r2 -> int len` 前检查 `r2 <= INT_MAX`，POSIX `send/recv` 返回值用 `ssize_t` 接收后再转 `int32`。
- `mr_dump/mr_undump` 若目标兼容 32 位 Mythroad，应固定以 `uint32_t` 序列化字符串长度，不写 host `size_t`。

## 黑屏可能链路

结合上述类型问题，Android arm64 + Flutter 下“部分 MRP 黑屏”的可能链路如下：

1. **网络启动链路**：MRP 启动时调用网络初始化或 DNS；字符串长度输出参数曾导致栈破坏，参考提交已修；剩余异步 callback 截断仍可能让状态机无法推进。
2. **EXT 初始化链路**：`TestCom1(800/801/802)` 加载或调用 ARM EXT；host pointer 与 guest address 混用、低地址 mmap 失败、`mr_cacheSync` 地址截断会导致 EXT 入口、wrapper 或 cache sync 失效。
3. **绘制链路**：function table offset 或 screen width/height 表项错位，ARM 侧读到错误宽高或错误绘图函数，主循环存在但不产生有效画面。
4. **资源链路**：文件 buffer、字符串 buffer、bitmap buffer 通过 Lua number/uint32 传递，arm64 高地址截断后读取资源失败或写错内存。
5. **timer/event 链路**：native event/timer callback 以 32 位地址保存；若来源不是纯 guest 地址，回调丢失会导致动画、首屏推进、异步加载完成事件不触发。

## 建议修复规则

### 类型规则

1. `size_t`：只用于 C host 内存长度、Lua 字符串长度、`strlen/memcpy` 这类平台 API。
2. `int32/uint32`：只用于 MRP ABI 规定的 32 位整数和值域明确的 guest 数据。
3. `guest_addr_t`：新增，底层为 `uint32_t`，表示 ARM guest 地址或 MRP 32 位地址。
4. `uintptr_t`：只用于 host 指针 bit round-trip，不允许缩窄到 `uint32`。
5. `void *`：只表示 host 可解引用指针，不允许直接写入 guest ABI table。
6. Lua number：不再承载 host 指针；确需兼容时使用 handle 表并记录生命周期。
7. `off_t/ssize_t/long/uLong`：进入 MRP 32 位 ABI 前必须显式 checked cast。

### API 规则

1. 输出参数禁止强转。调用方变量类型必须与 callee 签名完全一致。
2. `size_t -> int32/uint32` 必须通过 checked helper。
3. callback 必须分 host callback 与 guest callback。host callback 用函数指针；guest callback 用 `guest_addr_t` 并回到 VM 线程执行。
4. ARM EXT function table 使用 32 位槽描述，不使用 host 指针结构体直接生成 guest offset。
5. 低地址 mmap 只能作为兼容层，不能作为逻辑正确性的必要条件；如果当前版本仍依赖它，失败必须显式报错。
6. 文件、zlib、网络、EXT 输出长度都必须先检查符号和上限，再传给 `malloc/memcpy/send/recv/mrp_pushlstring`。

## 验证建议

### 编译期

- 对项目代码启用：
  - `-Werror=incompatible-pointer-types`
  - `-Werror=int-conversion`
  - `-Wpointer-to-int-cast`
  - `-Wint-to-pointer-cast`
- 分阶段引入：
  - `-Wconversion`
  - `-Wshorten-64-to-32`（Clang）
- 加静态断言：
  - `sizeof(int32) == 4`
  - `sizeof(uint32) == 4`
  - `sizeof(guest_addr_t) == 4`
  - ABI table 关键 offset 等于 `0x7c/0x80/0x170/0x174` 等已知 ARM 值。
- 对 Android/Flutter 共享库路径单独跑一轮转换告警，至少覆盖 `src/fileLib.c`、`src/arm_ext_executor.c`、`src/network.c`、`src/mythroad/dsm.c`、`src/mythroad/src/mr_dump.c`。

### 运行期

启动时打印并检查：

- `sizeof(void*)`
- `sizeof(size_t)`
- `sizeof(long)`
- `native_mem_base`
- `ArmExtModule.m->mem`
- 是否固定低地址 mmap 成功
- 任何准备写入 `uint32` 的 host pointer 是否超过 `UINT32_MAX`

对 Android arm64 黑屏样本建议开启专项日志：

- `VMRP_ARM_EXT_TRACE=1`
- 网络 init/DNS callback 地址和值域日志
- `TestCom1(800/801/802)` 输入地址、长度、EXT load 返回值
- `mr_cacheSync` host 地址与 guest 地址分别打印
- 首帧绘制前 screen width/height、screenBuf 地址、timer/event callback 注册值

### 静态扫描

建议把以下模式纳入 pre-commit 或 CI：

```sh
rg -n "checklstring\\([^\\n]*\\(size_t\\s*\\*\\)|optlstring\\([^\\n]*\\(size_t\\s*\\*\\)" src
rg -n "\\(uint32\\)\\s*\\w+|\\(uint32_t\\)\\s*\\w+" src/mythroad src/network.c src/native_dsm_funcs.c
rg -n "mrp_pushnumber\\(L, \\(mrp_Number\\).*(\\*|buf|ptr|filebuf|\\.p)" src/mythroad
rg -n "typedef\\s+.*long.*(uint32|int32)|MRP_(UINT32|INT32|INTEGER).*long" src mrc
```

这些扫描会有误报，但误报成本低于 arm64 上随机黑屏的排查成本。

### 测试用例

1. 构造 Lua 长字符串调用 TCP `send/sendto`、`string.sub/string.lower`，在 ASan/UBSan 下验证无栈破坏。
2. 在 Android arm64 上强制 native memory 和 EXT memory 使用高地址 fallback，验证所有 pointer-to-uint32 断言能明确报错。
3. 对网络异步回调构造高地址 fake callback/userData，验证不会经 `uint32_t` 截断直接调用。
4. 对 offset 生成工具在 64 位 host 上运行，确认会失败或生成与 ARM32 汇编一致的固定 offset。
5. 对黑屏 MRP 记录 `TestCom1(800/801/802)`、timer/event callback、screen width/height、首帧 draw 调用，区分是 EXT 初始化失败还是绘制链路失败。
6. 构造异常长度测试：`stat.st_size > INT32_MAX`、`readFile()` 文件不存在、畸形 gzip 扩容溢出、ARM EXT 写回 `output_len >= 0x80000000`、网络 `r2 > INT_MAX`。

## 优先级行动清单

### P0

1. 保持参考提交修复，并用 CI 禁止 `mr_L_checklstring(..., (size_t *)&...)` 回归。
2. 修复 `src/network.c` 与 `src/native_dsm_funcs.c` 的 async callback 截断：host callback 不走 `uint32_t`，guest callback 不在线程里直接当 host 函数调用。
3. 固化 mrc offset 生成的 ARM32 ABI，禁止 LP64 host layout 污染 guest table。
4. 修复 `arm_ext_executor.c` 的 `arm_output_len -> int32`、网络 `r2 -> int`、`mr_platEx` 输出长度写回这几条无检查转换。

### P1

1. 为 `size_t -> int32/uint32` 引入 checked helper，先覆盖 `MRF_platEx()`、`TestCom1()`、I/O write、EXT load/call。
2. 梳理 `mythroad.c`、`mr_strlib.c` 中 Lua number 传递指针的路径，改为 guest address 或 handle。
3. ARM EXT 中拆分 guest address 与 host pointer 类型，清理 `input_addr = (uint32_t)(uintptr_t)input` 这类边界。
4. Android arm64 启动时对低地址 mmap 失败、高地址 host pointer 写入 `uint32` 做硬错误。
5. 修复 `fileLib.c` 的 `st_size/read/write/readFile` 和 zlib 解压长度写回；把 `mr_dump/mr_undump` 的字符串长度格式固定到 32 位或显式标记为不可跨架构。

### P2

1. 继续清理或重命名 `sint32` 这类名称误导；`MRP_INT32 long` 和 `mrc_base.h` 的固定宽度定义已完成。
2. 在文档中明确 MRP ABI 32 位、host runtime 可为 64 位，两者不得用同一 typedef 混用。
3. 对 `-Wconversion` 做分模块治理，先从网络、EXT、mythroad lib 三个高风险模块开始。

## 最终判断

参考提交证明，Android arm64 当前黑屏/崩溃类问题的核心方向不是 Flutter UI 层，而是 C 层 32 位 MRP ABI 与 64 位 host ABI 混用。已修复的网络崩溃是“输出参数宽度不匹配”的典型样本；剩余最高风险集中在 callback 指针截断、guest/host 地址混用、offset 生成 LP64 污染和长度下转缺少边界检查。

建议后续排查黑屏时优先把日志和断言加在 P0/P1 清单位置。只要能确认某个黑屏样本的失败点是网络回调、EXT load/call、cache sync、timer/event 或绘制表项中的哪一条，就能进一步收敛到具体类型修复，而不需要从 Flutter 渲染层盲查。
