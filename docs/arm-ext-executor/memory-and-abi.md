# guest 内存、寄存器与 ABI

## 1. 地址空间

核心常量位于 `src/include/arm_ext_internal.h`。

| 区域 | guest 地址 | 大小 | 用途 |
| --- | --- | --- | --- |
| 低地址镜像 | `0x00000000` | 64 KB | 低地址探测、空槽和 legacy table 镜像 |
| 主映射 | `0x00010000` | 16 MB | table、数据槽、heap、stack、wrapper code、screen reserve |
| table | `0x00010000` | 150 x 4 字节 | guest 调用宿主的桥入口 |
| stop 地址 | `0x0007FFF0` | 单点 | 宿主控制的 guest 执行终点 |
| bump heap | `0x00200000` 起 | 动态 | 内部对象、应用后备分配、模块暂存 |
| wrapper stack | `0x00E00000` | 512 KB | helper 和 wrapper 调用栈 |
| wrapper code | `0x00E80000` | EXT 实际长度 | 最外层 `cfunction.ext` |
| screen reserve | 主映射最高 1 MB | 1 MB | 逻辑屏幕扩大后的稳定 framebuffer |
| platform memory | `0x40000000` | 2 MB | MTK 图形/设备存储带 |
| executor metadata | `0x70000000` | 64 KB | host 管理的稳定元数据 |
| virtual FD band | `0x7FFF0000` | 逻辑句柄区 | MRP cache 的只读虚拟文件描述符 |
| platform IO | `0x80000000` | 18 MB | ROM/MMIO/平台状态探测 |
| platform alternate | `0xA0000000` | 2 MB | 备用平台地址带 |

主映射的末地址为 `EXT_BASE_ADDR + EXT_MEM_SIZE = 0x01010000`。所有范围计算必须使用溢出安全的 `addr + len` 检查，不能只判断起始地址。

## 2. guest 地址与 host 指针

执行器尝试把主 backing memory 固定映射到 host `0x00010000`。成功时，主区内 guest 地址可以与 host 指针数值相同；失败时 backing memory 位于任意 host 地址。

因此代码不能依赖“guest 地址等于 host 指针”。统一使用：

| 函数 | 作用 |
| --- | --- |
| `arm_ptr(m, addr)` | 把单个 guest 地址转换为 host 指针 |
| `arm_ptr_span(m, addr, len)` | 验证整个 guest 区间可映射后返回 host 指针 |
| `arm_addr(m, ptr)` | 把属于已知 backing 区的 host 指针转换为 guest 地址 |
| `arm_ext_addr_range_mapped()` | 只做 guest 区间映射验证 |

新桥接函数的基本规则：

- 读写缓冲区时必须验证完整长度；
- host 返回的新内存不能直接把指针截成 `uint32_t`；
- 需要 guest 长期持有的数据必须复制到 guest 分配区；
- 指针身份本身有 ABI 意义时，必须明确数据原本是否已经位于 guest 可见 RAM。

## 3. `mr_c_function_P_t`

P 结构是模块运行上下文的最小稳定描述：

```c
typedef struct mr_c_function_P_t {
    uint32 start_of_ER_RW;  /* +0x00 */
    uint32 ER_RW_Length;    /* +0x04 */
    int32  ext_type;        /* +0x08 */
    uint32 mrc_extChunk;    /* +0x0C */
    int32  stack;           /* +0x10 */
} mr_c_function_P_t;
```

| 偏移 | 意义 | 执行器用途 |
| --- | --- | --- |
| `+0x00` | ER_RW 基址 | 写入 R9、识别模块所有者、timer 和屏幕归属 |
| `+0x04` | ER_RW 长度 | 边界验证和 compact 堆保护 |
| `+0x08` | EXT 类型 | 诊断和兼容识别 |
| `+0x0C` | extChunk 地址 | helper、dispatch、suspend 状态和模块关联 |
| `+0x10` | 栈字段 | guest SDK 自用，宿主目前不作为主状态源 |

执行器同时维护 `m->p_addr`、`primary_p_addr`、`active_p_addr`、`timer_p_addr` 和 `current_p_addr`。它们代表不同角色，不应合并成一个“当前 P”。

## 4. extChunk

当前代码通过命名偏移访问 extChunk：

| 常量 | 偏移 | 意义 |
| --- | --- | --- |
| `AEX_CHUNK_INIT_OFF` | `0x04` | 模块 init 入口 |
| `AEX_CHUNK_HELPER_OFF` | `0x08` | helper 入口 |
| `AEX_CHUNK_FILE_BASE_OFF` | `0x0C` | 模块文件映像地址 |
| `AEX_CHUNK_FILE_LEN_OFF` | `0x10` | 文件映像长度 |
| `AEX_CHUNK_RW_BASE_OFF` | `0x14` | ER_RW 基址 |
| `AEX_CHUNK_RW_LEN_OFF` | `0x18` | ER_RW 长度 |
| `AEX_CHUNK_P_ADDR_OFF` | `0x1C` | P 地址 |
| `AEX_CHUNK_P_LEN_OFF` | `0x20` | P 区长度 |
| `AEX_CHUNK_EVENT_DATA_OFF` | `0x24` | code=2 dispatch 参数 |
| `AEX_CHUNK_EVENT_FUNC_OFF` | `0x28` | code=2 dispatch 函数 |
| `AEX_CHUNK_RECORD_OFF` | `0x2C` | private module record |
| `AEX_CHUNK_SUSPEND_OFF` | `0x34` | suspend 深度 |
| `AEX_CHUNK_HEAP_TOP_OFF` | `0x38` | 模块堆顶界 |

`+0x34` 是计数器，不是布尔退出标志。首次挂起和嵌套挂起可能使其大于 1，恢复路径必须按深度递减。

详细字段证据和 `EXT_CHUNK_MAGIC` 见 [`../arm-ext-abi.md`](../arm-ext-abi.md)。

## 5. 函数表与数据槽

`init_table()` 先把 150 个槽初始化为各自的可执行桥地址：

```text
table[N] = EXT_TABLE_ADDR + N * 4
```

数据槽不能指向 host 全局变量，所以执行器在 guest 内分配 shadow storage，再把 table 槽改成对应 guest 地址。主要数据槽包括：

- `91..94`：屏幕缓冲地址、宽、高、位深；
- `95..99`：bitmap/tile/map/sound/sprite 数组；
- `100..103`：当前包、启动文件和旧包名；
- `104..105`：RAM package 地址和长度；
- `108..111`：应用堆 base/len/end/left；
- `135..136`：应用堆最小剩余量和最高使用量；
- `139..140`：短信结果状态；
- `142..143`：退出回调及数据；
- `146`：guest 可见 first-fit 空闲链表头。

`table[23]` 指向内部表，`table[24]` 指向 port 表。private child 的 module record 可能复制或改写这些槽，因此资源和桥接调用必须根据 LR 选择正确 table 上下文。

## 6. 三层分配模型

### 6.1 应用 first-fit 堆

`table[0]/[1]/[2]` 和 `table[125]` 返回给应用的普通内存优先使用 Mythroad 风格 first-fit 空闲链表。

关键语义：

- 8 字节对齐；
- `table[146]` 暴露空闲链表头；
- free 后插回并合并相邻区间；
- 应用可能通过观察链表预测下一次分配地址；
- `readFile` 也必须遵循同一地址复用语义。

这不是普通 `malloc()` 可以替代的实现细节。已有应用会先 free 一块内存，再忽略 `readFile` 返回值并从预测地址读取数据。

### 6.2 bump 后备与回收

first-fit 池没有合适区间时，`arm_ext_app_mem_malloc()` 使用 `arm_alloc()` 后备。执行器记录 app-visible bump block，free 时只回收已登记的块，避免 guest 伪造地址释放屏幕、模块或执行器内部对象。

`arm_alloc()` 需要跳过：

- wrapper 栈；
- wrapper code；
- 已注册模块映像和 ER_RW；
- compact timer 节点；
- 顶部屏幕保留区；
- 执行器元数据区。

### 6.3 compact 私有堆

部分 wrapper/child 在自己的 RW 中维护 compact 堆控制块和 free-list。执行器不接管其正常分配，但会在模块注册和 timer 调度前后裁掉与以下范围重叠的空闲节点：

- 已注册 code/P/RW；
- 活跃 timer 节点；
- guest 应用 live block；
- 屏幕和关键执行器状态。

控制块和节点布局存在 SDK 变体，必须先由代码模式和结构约束确认，不能仅凭某个 magic 修改内存。

## 7. 屏幕缓冲

默认 guest 屏幕为 RGB565，长度 `screen_w * screen_h * 2`。它是 `init_table()` 中的重要早期分配，部分应用会扫描 heap 寻找 framebuffer，因此地址布局具有兼容意义。

当应用直接改写 `table[92]/[93]` 指向的宽高槽时：

1. 写 hook 读取新宽高；
2. 校验所需字节数和上限；
3. 容量不足时迁移到主映射顶部 1 MB 保留区；
4. 更新 screen table、bitmap 描述符和写 hook；
5. 保持 guest 后续缓存到的是稳定地址。

绘制桥接还要区分：

- 写入 guest backing screen；
- 应用显式 present 到宿主；
- primary 被前台 child 覆盖时的不可见写；
- 模态 child 关闭后的底层画面恢复。

这些规则由 `aex_screen.c` 维护，不能在单个 table handler 中绕过 owner/damage API 直接调用 SDL。

## 8. R9、R10 与 GOT

ARM EXT 的全局变量通常以 R9 为基址访问。wrapper、primary 和 child 各有独立 ER_RW，因此每次跨模块执行都必须恢复正确 R9。

执行器使用以下信息判断 R9：

- 当前 helper/P；
- PC/LR 所属的注册模块 code 区；
- pending internal loader 的 staging 区；
- module record 和 extChunk 的 P/RW 关联；
- wrapper 返回地址和保存的 outer R9。

GOT 中的 host bridge 实际是 guest table 地址。dump/restore、memcpy 或 memset 可能把已经重定位的槽覆盖成原始文件值，因此执行器维护 GOT snapshot，并在有充分证据时恢复 table 指针。

恢复必须满足两个条件：

- 写入范围确实覆盖已知 GOT；
- 当前操作是可执行模块恢复，而不是普通压缩数据或下载包内容。

无条件扫描并修补“看起来像 GOT”的数据会破坏资源文件。

## 9. Unicorn 翻译缓存

guest code 被以下操作覆盖后，必须调用 `uc_ctl_remove_cache()` 失效对应范围：

- 从 dump 文件恢复模块映像；
- host cache sync 更新 code；
- private loader 在同一 guest 地址复用新模块；
- 大块文件读取覆盖注册 code 区。

只修改 backing memory 而不清 Unicorn TB cache，会继续执行旧翻译结果，表现为“内存里已经是新代码，但 PC 行为没有变化”。

## 10. 不变量

设置 `VMRP_ARM_EXT_INVARIANTS=1` 后，执行器会在关键调用和模块状态迁移处检查：

- P/helper 成对存在；
- active/primary/timer owner 指向已知模块；
- nested module 数量不超过上限；
- 模块 code/P/RW 区间可映射且不非法重叠；
- live allocation 和关键保护区不被 compact free-list 覆盖；
- screen、heap 和 timer 状态保持结构一致。

修改内存、模块或 timer 代码时，应把不变量失败视为根因线索，不应在检查函数中加入静默修复。

