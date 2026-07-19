# 执行器架构与运行流程

## 1. 责任边界

ARM EXT 执行器负责把一个 ARM/Thumb EXT 二进制放入可运行的 Mythroad guest 环境，并维持它与本机运行时之间的双向桥接。

执行器负责：

- 创建和销毁 Unicorn ARM 实例；
- 映射主内存、低地址表、平台地址带和执行器元数据区；
- 构造 `mr_table`、内部表、端口表和数据槽；
- 加载 wrapper EXT，识别 ARM/Thumb 入口并运行；
- 把 table 调用桥接到本机 Mythroad/DSM；
- 管理 wrapper、primary 和 child 的 P/helper/RW/R9；
- 管理 guest 应用堆、bump 后备区和 compact 私有堆保护；
- 维护 timer owner、屏幕 owner、模态快照和包资源上下文；
- 对 dump/restore 后的 GOT 和 Unicorn 翻译缓存进行修复；
- 输出可定位到 guest PC、模块和寄存器的诊断信息。

执行器不负责：

- SDL 事件循环和窗口生命周期；
- 本机文件、socket、音频设备的底层实现；
- Mythroad MR/Lua VM 的主体逻辑；
- 真实短信、通话、支付或厂商在线服务。

这些能力由 `src/main.c`、`src/skyengine.c`、`src/network.c`、`src/native_dsm_funcs.c` 和 `src/mythroad/` 提供。

## 2. 对外 API

公共接口位于 `src/include/arm_ext_executor.h`。

| API | 作用 |
| --- | --- |
| `arm_ext_load()` | 创建实例、加载 wrapper、运行 `mr_c_function_load()` 并返回模块句柄 |
| `arm_ext_call()` | 通过 helper 发送生命周期、事件、timer 等 code 调用 |
| `arm_ext_helper_addr()` | 返回 wrapper helper 地址 |
| `arm_ext_primary_helper()` | 为宿主 timer 选择“直连 dispatch 或普通 code=2”路径；非零值不保证等于 primary helper |
| `arm_ext_consume_primary_host_init()` | 消费 table[25] 注册路径留下的首次 primary code=0 初始化请求 |
| `arm_ext_call_dispatch()` | 按已探测 ABI 调用 primary compact walker、wrapper dispatch 或 extChunk timer 路径 |
| `arm_ext_invoke0()` / `arm_ext_invoke3()` | 调用已知 guest 函数地址，主要用于内部 dispatch |
| `arm_ext_read_timer_queue()` | 读取 primary RW 中已识别的 game timer head，不覆盖其它队列类型 |
| `arm_ext_reset_child_modules()` | 退役非 primary child、恢复 active/screen owner；timer owner 由完整关闭路径另行收敛 |
| `arm_ext_host_cache_sync()` | 将宿主数据同步到 pending private-loader staging 区并失效实际复制范围的翻译缓存 |
| `arm_ext_unload()` | 停止并释放 Unicorn、映射、缓存、快照和模块状态 |

主要调用方是 `src/mythroad/mythroad.c`：Mythroad 的 `_mr_TestComC()` 路径加载 EXT，事件/生命周期通过 `arm_ext_call()` 进入；宿主 timer 根据 `arm_ext_primary_helper()` 的分流结果，在 `arm_ext_call_dispatch()` 与普通 helper code 2 之间选择。

接口名称不足以表达全部语义，尤其 `arm_ext_primary_helper()` 不是普通 getter。参数、两层返回值、输出指针生命周期和失败回滚见 [api-reference.md](api-reference.md)。

## 3. 源码拆分

| 文件 | 当前职责 |
| --- | --- |
| `src/arm_ext_executor.c` | 实例加载、函数表初始化、Unicorn hook、主调用编排、模态状态与销毁 |
| `src/arm_ext/aex_exec.c` | ARM/Thumb 模式、寄存器、实际执行、PC trace、中断处理、格式化参数 |
| `src/arm_ext/aex_mem.c` | guest first-fit 堆、bump 回收、compact 堆裁剪、保护区和不变量 |
| `src/arm_ext/aex_module.c` | 包名、资源归属、nested module 注册、private-loader 修复、P/helper/RW 所有权 |
| `src/arm_ext/aex_pack.c` | MRP 索引缓存和只读虚拟文件描述符 |
| `src/arm_ext/aex_table.c` | `mr_table` 各槽 handler，参数转换和宿主函数调用 |
| `src/arm_ext/aex_screen.c` | 屏幕缓存、damage、前台覆盖、owner、ROP、旋转和模态截图恢复 |
| `src/arm_ext/aex_timer.c` | timer 队列识别、owner 记录、game head 探测和状态同步 |
| `src/arm_ext/aex_guard.c` | table 返回保护，防止异常 epilogue 或错误返回地址继续执行 |
| `src/arm_ext/aex_diag.c` | 层状态、timer 节点、文件句柄、PC watch 等窄诊断 |
| `src/arm_ext/aex_support.c` | FNV、guest memcpy/strncpy、路径比较和环境开关缓存 |

共享结构在 `src/include/arm_ext_internal.h`，拆分单元之间的私有接口在 `src/include/arm_ext_priv.h`，宿主依赖面集中在 `src/include/arm_ext_host.h`。

## 4. 加载流程

`arm_ext_load()` 的主要步骤如下：

1. 校验输入二进制长度并分配 `ArmExtModule`。
2. 为 16 MB 主地址空间申请 host backing memory。
3. 尝试把 backing memory 固定到 `EXT_BASE_ADDR`；失败时使用普通堆内存并保留 guest/host 转换。
4. 分配平台地址带、执行器元数据区和低地址镜像。
5. `uc_open(UC_ARCH_ARM, UC_MODE_ARM)` 创建 Unicorn。
6. 用 `uc_mem_map_ptr()` 映射主内存、平台区和元数据区。
7. `init_table()` 构造 150 槽表、数据槽、屏幕缓冲和应用堆。
8. `parse_mrp_cache()` 预解析当前 MRP，避免 guest 重复线性扫描包索引。
9. 把低地址表镜像映射到 guest `0x00000000..0x0000FFFF`。
10. 把 wrapper 二进制复制到 `EXT_CODE_ADDR`。
11. 运行二进制模式探测，识别 wrapper timer dispatch、compact scheduler、compact heap control 和 chain walker。
12. 安装 table、低地址、R9 恢复、屏幕写、GOT 写、无效内存和中断 hook。
13. 设置 `R0=load_code`，从 `EXT_CODE_ADDR + 8` 运行 wrapper 加载入口。
14. 捕获 wrapper/helper/P 和 timer dispatch 状态，把 `R0` 返回给 Mythroad 调用方。

任何一步失败都会进入 `arm_ext_unload()`，因此新资源必须能在“只初始化了一半”的状态下安全释放。

## 5. Unicorn hook

| Hook | 范围 | 用途 |
| --- | --- | --- |
| `hook_table` | `EXT_TABLE_ADDR` 后 150 个 4 字节槽 | 把 guest 函数表跳转转换为本机 handler |
| `hook_low_zero` | guest 低 64 KB | 兼容空函数、低地址镜像和返回语义 |
| `hook_restore_r9` | 主映射和平台映射的基本块 | 根据执行地址恢复对应模块的 R9 |
| `hook_screen_write` | 当前 guest 屏幕缓冲 | 记录脏区和可见提交归属 |
| `hook_got_write` | guest heap/code 可写区域 | 记录 GOT 中写入的 table bridge 地址 |
| `hook_invalid` | 全局无效内存访问 | 输出 PC、寄存器、指令字节、栈和 crash dump |
| `hook_intr` | guest 中断 | 处理可识别中断；未知中断标记失败 |
| `trace_pc` | wrapper code | `VMRP_ARM_EXT_TRACE_PC` 下滚动保留最近 64 个 PC/CPSR，异常时输出 |
| PC watch hook | 每个已注册 nested module 的指定偏移 | `VMRP_ARM_EXT_WATCH_PC` 下进行低噪声观察 |

屏幕宽高槽还安装单独的读写 hook。应用直接改写 `mr_screen_w/h` 时，执行器需要在 guest 缓存 framebuffer 指针之前决定是否迁移屏幕缓冲。

## 6. guest 调用帧

`arm_ext_call()` 调 helper 时使用以下寄存器：

| 寄存器 | 值 |
| --- | --- |
| R0 | 当前模块 P 地址 |
| R1 | helper code |
| R2 | guest input 地址或特定 code 的原始标量 |
| R3 | input 长度 |
| R9 | 当前模块 ER_RW 基址 |
| SP | wrapper 栈顶部附近 |

额外参数通过 SP 传入：

```text
SP + 0x00  output 指针槽的 guest 地址
SP + 0x04  output_len 指针槽的 guest 地址
SP + 0x08  保留
SP + 0x0C  保留
```

table handler 读取参数时遵循 ARM AAPCS：前四个参数来自 R0-R3，后续参数由 `arg_read()` 从 guest SP 读取。返回值由 `cb_ret()` 写入 R0，并把 PC 调整到 LR。

## 7. `arm_ext_call()` 的阶段

一次 call 不是简单的 `uc_emu_start()`，而是包含以下阶段：

1. 入口不变量检查和忙等计数复位。
2. 把 host input 复制到 guest，或按 code 语义保留原始标量。
3. 根据 code、primary、active 和 timer owner 选择 P/helper。
4. 修复 wrapper 可能清零的 primary helper 槽。
5. 必要时保存模态前的屏幕和 wrapper 前台分发区。
6. 设置 R0-R3、R9、SP 和当前调用 owner。
7. 同步宿主内部状态到 guest 数据槽。
8. timer call 前后清理 compact 堆中与模块/timer 重叠的空闲范围。
9. 进入屏幕上下文并运行 guest。
10. 同步 timer 状态，捕获新注册模块和 dispatch 入口。
11. 根据 suspend 深度、timer 活性和 child 状态决定是否保持宿主 tick。
12. 恢复模态屏幕/前台路由，或退役已经关闭的 child。
13. 把 guest 输出地址转换为 host 可读结果。

这些步骤的先后次序属于兼容行为。尤其不能把屏幕 owner、timer owner 和模块 owner 分开更新，否则会出现“事件已经回到 game，但 timer 或画面仍属于旧插件”的半状态。

## 8. 执行与停止

`run_arm_with_sp()` 负责：

- 根据地址 bit0 和 CPSR 切换 ARM/Thumb；
- 设置 PC/SP/LR 和停止地址；
- 执行 `uc_emu_start()`；
- 检查未处理的中断和 Unicorn 错误；
- 在失败时输出最近 PC ring；
- 应用 table 返回保护；
- 在真实 wrapper 返回点恢复外层 R9。

guest 跳转到 0 时，执行器把它解释为“返回 LR”；LR 也为空时才停止。这是为了兼容部分 wrapper 的空回调槽，不能简单把 fetch 0 全部视为崩溃。

### 8.1 ARM/Thumb 修正

部分 nested EXT 的函数指针缺少 Thumb bit。Unicorn 会把 Thumb-2 字节按 ARM 解码，并报告 `UC_ERR_INSN_INVALID` 或 `UC_ERR_EXCEPTION`。执行器只在 PC 位于 wrapper 或已注册 nested code range、且当前 CPSR 不是 Thumb 时做一次 Thumb 重试；同一 PC 再次失败就按真实异常处理。

这个限制很重要。若对任意 heap 地址盲目切 Thumb 重试，extChunk、P、timer 节点或路径字符串也可能碰巧解码成有效指令，随后以更难定位的写越界结束。

### 8.2 可恢复的 callback 结束

`run_arm_with_sp()` 对少数已经由地址和指令形态约束的历史返回错误执行“结束当前 callback，保持平台状态继续调度”的兼容处理：

- wrapper code 内的 non-instruction PC；
- nested heap code 返回到 Thumb-2 32 位指令的中间 halfword；
- wrapper 分发到内容为 `0xFFFF...`、全零或其它已确认 heap data 的回调地址；
- `UC_ERR_EXCEPTION` 且 PC 落在 SP 附近的栈破坏返回模式；
- table bridge 返回后重复进入已经消费过栈帧的 `blx-reg` / `pop {...,pc}` epilogue。

这些路径把 PC 移到 `EXT_STOP_ADDR` 并返回 `MR_SUCCESS`，表示执行器有意放弃当前 callback，而不保证 guest 已经运行到源码级正常 return。开启 `VMRP_ARM_EXT_TRACE=1` 时会看到 `treated as clean exit`、`stopped at non-instruction` 或 table return guard 日志。

未映射读写、无法归类的非法指令以及未知 SVC/HVC 仍返回 `MR_FAILED`。唯一直接消费的中断 ABI 是 ARM semihosting `SYS_WRITEC`，用于 vendor runtime 的调试字符输出；其它 trap 会保存 `pending_intr_*` 并转成执行异常。

新增兼容结束条件必须同时约束错误类型、PC 所属区间、指令/数据形态和调用上下文，并用真实 MRP 验证后再加入。只按某个固定 PC、包名或“异常发生在 heap”吞掉错误，会掩盖新的内存破坏。

## 9. 串行模型

Unicorn、Mythroad 全局状态、模块表和屏幕缓存都不是可并发访问的。

- SDL timer 线程只能向 SDL 主线程投递事件。
- E2E socket 线程只能排队命令，不能直接调用 `arm_ext_call()`。
- 共享库由内部 worker 串行消费事件和 timer。
- 同一个 `ArmExtModule` 不能同时执行两个 guest 调用。

如果新增异步宿主 API，回调必须回到执行器所属线程后再进入 guest。
