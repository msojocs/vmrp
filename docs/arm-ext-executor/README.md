# ARM EXT 执行器文档

本文档目录说明 SkyEngine 当前 ARM EXT 执行器的设计、运行流程、ABI、内存模型、模块生命周期、宿主桥接和调试方法。

文档以以下源码为准：

- `src/arm_ext_executor.c`
- `src/arm_ext/*.c`
- `src/include/arm_ext_executor.h`
- `src/include/arm_ext_internal.h`
- `src/include/arm_ext_priv.h`
- `src/include/arm_ext_host.h`

当前说明对应 2026-07-14 的源码状态。ARM EXT 兼容行为仍在持续演进，修改执行器后应同步更新本目录。

## 阅读顺序

| 文档 | 内容 | 建议读者 |
| --- | --- | --- |
| [architecture.md](architecture.md) | 执行器边界、源码拆分、加载与调用流程、Unicorn hook | 首次接触执行器的开发者 |
| [api-reference.md](api-reference.md) | 公共 API、返回值、指针所有权、路由语义和生命周期契约 | 修改 Mythroad 调用点或嵌入执行器的开发者 |
| [memory-and-abi.md](memory-and-abi.md) | guest 地址空间、P/extChunk、分配器、R9/GOT、屏幕缓冲 | 排查崩溃、花屏、内存破坏的开发者 |
| [modules-and-timers.md](modules-and-timers.md) | wrapper、primary、前台 child、事件路由、timer owner、挂起恢复 | 排查插件、下载、模态框、定时器卡死的开发者 |
| [host-bridge.md](host-bridge.md) | 150 槽函数表、调用约定、关键表项、文件与图形桥接 | 新增或修复平台 API 的开发者 |
| [debugging-and-verification.md](debugging-and-verification.md) | 诊断变量、崩溃转储、PC 观察点、PPM/E2E 验证流程 | 复现和定位兼容问题的开发者 |

更细的二进制布局表见上级文档：

- [`../arm-ext-abi.md`](../arm-ext-abi.md)：地址常量、P 结构、extChunk、compact 堆和 timer 节点偏移。
- [`../arm_ext_executor-issues.md`](../arm_ext_executor-issues.md)：已识别的缺陷、技术债与风险。
- [`../mythroad-platform-reference.md`](../mythroad-platform-reference.md)：真机 Mythroad 平台行为依据。

## 执行器解决的问题

MRP 包内的 C 扩展通常是 ARM/Thumb 机器码。桌面宿主不能直接调用这些函数，且 EXT 还假定以下条件成立：

- 存在固定布局的 `mr_table` 函数表和数据槽；
- R9 指向当前模块的 ER_RW/GOT 基址；
- wrapper、game 和插件模块可以嵌套加载并互相回调；
- ARM 地址、模块私有堆、屏幕缓冲和定时器链表在整个生命周期中保持稳定；
- 文件系统、网络、图形、音频和平台扩展接口具有 Mythroad 语义；
- dump/restore、包内资源读取和插件切换不会破坏已经重定位的函数表指针。

执行器在 Unicorn 中建立这套 guest 环境，并把 guest 的函数表调用桥接到本机 Mythroad/DSM 实现。

## 核心术语

| 术语 | 含义 |
| --- | --- |
| guest | 在 Unicorn 中运行的 ARM/Thumb EXT 代码及其地址空间 |
| host | SkyEngine 本机 C 代码、Mythroad/DSM 层和操作系统资源 |
| wrapper | 最外层 `cfunction.ext`，负责加载 game/插件并分发生命周期和事件 |
| primary | 首个确认的主要业务模块，通常是 `game.ext` |
| child | primary 之后加载的浏览器、下载、支付、广告等嵌套模块 |
| P 结构 | `mr_c_function_P_t`，记录模块 RW、extChunk 和栈信息 |
| extChunk | wrapper loader 维护的模块描述结构，记录入口、helper、P、RW 和挂起状态 |
| helper | 模块接收 code/input/output 调用的 ARM 函数入口 |
| module record | 私有 loader 为 child 建立的函数表镜像和模块上下文 |
| active owner | 当前前台模块；事件或绘制归属判断会参考它 |
| timer owner | 最近启动宿主 timer、应接收下一次 tick 的模块 |
| screen owner | 当前允许把 guest 屏幕内容提交给宿主窗口的模块 |

## 一次调用的最短路径

```text
Mythroad runtime
  -> arm_ext_call(code, input)
     -> 依据 code 和队列证据选择 wrapper / primary / active / timer owner
     -> 设置 R0-R3、SP、R9 和输出指针
     -> run_arm_with_sp()
        -> Unicorn 执行 guest
        -> guest 调用 EXT_TABLE_ADDR + index * 4
        -> hook_table()
        -> aex_table_handlers[index]
        -> 宿主 Mythroad/DSM 函数
     -> 同步 timer、屏幕、模块和退出状态
```

这里的返回值存在两层语义：Unicorn/宿主调用是否成功，以及 guest helper 的 R0。`arm_ext_load()` 用 C 返回值与 `ext_ret` 分开表达两者；`arm_ext_call()` 通常直接返回 guest R0，但会为原始事件、模态进入和平台退出流程规范化结果。完整契约见 [api-reference.md](api-reference.md)。

## 事实等级

执行器包含三类知识，文档和代码评审时必须区分：

1. 固定契约：来自公开头文件、Mythroad 函数表或稳定结构定义，例如 `mr_c_function_P_t` 和 150 槽表。
2. 反汇编证据：来自具体 SDK wrapper/game 的指令模式，例如 extChunk `+0x34` 的 suspend 计数。
3. 运行时探测：通过代码特征、链表 magic、地址归属和状态变化识别 SDK 变体。

运行时探测不能被描述成所有 MRP 都遵守的固定 ABI。新增变体时应优先扩展通用探测和结构校验，不应按包名、应用名或固定样本路径分支。

## 维护原则

- 所有 Unicorn 调用必须串行执行，不能从 SDL timer 线程或其它工作线程直接进入 VM。
- R9、P、helper、extChunk、module record 和包上下文必须作为一个所有权整体更新。
- 不允许把 host 指针未经转换直接写入 guest；必须通过 `arm_ptr()`、`arm_ptr_span()`、`arm_addr()` 或显式 guest 分配。
- 改动函数表槽位时必须对照 `src/mythroad/mythroad.c` 的 `_mr_c_function_table_init()`。
- 改动模块或 timer 状态后应在 `VMRP_ARM_EXT_INVARIANTS=1` 下运行相关回归。
- 图形修复必须验证最终 PPM 像素，不能仅依据日志或“窗口没有崩溃”。
- 遇到新 SDK 变体时，先保留 MRP、EXT、启动参数、工作目录、输入序列和日志，再修改兼容逻辑。
