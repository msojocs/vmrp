# ARM EXT 执行器 ABI / 内存布局系统性参考

> 版本：2026-07-08  
> 定位：重构安全网 P0.4，见 `docs/arm-ext-executor-refactor-plan.md`  
> 本文只记录已有证据所支持的事实；不确定/矛盾处明确标注**【待核实】**。  
> 改进建议不属于本文范围，见 refactor-plan.md。

---

## 目录

1. [EXT 地址空间全景图](#1-ext-地址空间全景图)
2. [mr_c_function_P_t（P 结构）](#2-mr_c_function_p_tp-结构)
3. [extChunk 布局表](#3-extchunk-布局表)
4. [compact 堆控制块](#4-compact-堆控制块)
5. [timer 节点格式与调度器偏移矩阵](#5-timer-节点格式与调度器偏移矩阵)
6. [模块生命周期状态](#6-模块生命周期状态)
7. [guest 内存分配器三层](#7-guest-内存分配器三层)

---

## 1. EXT 地址空间全景图

### 1.1 地址常量表

| 常量名 | 值 | 大小 | 描述 |
|---|---|---|---|
| `EXT_BASE_ADDR` | `0x00010000` | — | 主映射区起点（Unicorn 映射基址） |
| `EXT_MEM_SIZE` | `0x01000000`（16 MB） | — | 主映射区总容量 |
| `EXT_TABLE_ADDR` | `0x00010000`（= EXT_BASE_ADDR） | — | EXT 函数桥接表起点 |
| `EXT_TABLE_COUNT` | `150` | 600 字节 | 函数桥接槽数 |
| `EXT_LOW_TABLE_SIZE` | `0x10000`（64 KB） | — | 低地址宿主侧映射大小（gwkdl 等内存检测需要） |
| `EXT_STOP_ADDR` | `0x0007FFF0` | — | 宿主停止执行的边界地址（hook 用） |
| `EXT_HEAP_ADDR` | `0x00200000` | — | bump 堆起点（挂载于主映射区内） |
| `EXT_STACK_ADDR` | `0x00E00000` | — | wrapper 栈区起点 |
| `EXT_STACK_SIZE` | `0x00080000`（512 KB） | — | wrapper 栈保留区 |
| `EXT_CODE_ADDR` | `0x00E80000` | — | wrapper EXT 代码加载基址 |
| `EXT_WRAPPER_STACK_SIZE` | `0x00020000`（128 KB） | — | 每个 wrapper 的栈容量 |
| `EXT_SCREEN_RESERVE` | `0x00100000`（1 MB） | — | 屏幕缓冲顶部保留区大小（覆盖 480×800×2 及以下） |
| `EXT_PLATFORM_MEM_ADDR` | `0x40000000` | 2 MB | MTK 图形存储带（部分 wrapper 读取后回传为模块存储地址） |
| `EXT_SCRRAM_ADDR` | `0x50000000` | 16 MB | `MR_MALLOC_SCRRAM` 返回的独立平台扩展内存 |
| `EXT_PLATFORM_IO_MEM_ADDR` | `0x80000000` | 18 MB | 平台 ROM/MMIO/IO 区（RX4.5 等读 R9+0x100 = 0x80110004） |
| `EXT_PLATFORM_ALT_MEM_ADDR` | `0xA0000000` | 2 MB | 备用平台存储区 |
| `EXT_EXECUTOR_META_ADDR` | `0x70000000` | 64 KB | 执行器元数据区（宿主专用，ARM 代码不应直接访问） |
| `MRP_VFD_BASE` | `0x7FFF0000` | — | 虚拟文件描述符基地址 |

### 1.2 ASCII 布局图

```
ARM 可见地址空间（主映射区 EXT_BASE_ADDR..EXT_BASE_ADDR+EXT_MEM_SIZE）
───────────────────────────────────────────────────────────────
0x00000000  ┌─────────────────────────────────────┐
            │  (未映射，宿主有 64KB low_table 覆盖  │
            │   0x10000 字节，gwkdl 内存检测需要)    │
0x00010000  ├─────────────────────────────────────┤ ← EXT_TABLE_ADDR / EXT_BASE_ADDR
            │  EXT 函数桥接表                       │
            │  150 槽 × 4 字节 = 600 字节           │
            │  table[N] = EXT_TABLE_ADDR + N*4      │
0x00010258  ├─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┤  (表结束)
            │  低地址通用区（heap_top 初始此处向上）  │
            │  extChunk、P 结构、origin_mem 池等      │
            │  由 arm_alloc(bump) 从 EXT_HEAP_ADDR  │
            │  向上分配                              │
0x00200000  ├─────────────────────────────────────┤ ← EXT_HEAP_ADDR（bump 堆起点）
            │  bump 堆（向上增长）                   │
            │  origin_mem 池（512 KB，first-fit）    │
            │  子模块映像、extChunk、P 结构、         │
            │  timer 节点等由 compact 分配器细分      │
            │                                       │
            │  [EXT_HEAP_ADDR, EXT_STACK_ADDR)      │
            │  为可分配区间；bump 跳过栈/代码保留区    │
            │                                       │
0x00E00000  ├─────────────────────────────────────┤ ← EXT_STACK_ADDR（wrapper 栈区起点）
            │  wrapper 栈区（保留，bump 跳过）         │
            │  512 KB                               │
0x00E80000  ├─────────────────────────────────────┤ ← EXT_CODE_ADDR（wrapper 代码）
            │  wrapper EXT 代码段（加载后固定）        │
            │  code_len 由实际 EXT 文件决定           │
0x00Exxxxx  ├─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┤  (代码段末)
            │  代码末尾之后仍可 bump 分配（跳过保留区）│
            │                                       │
0x00F00000  ├─────────────────────────────────────┤  (若屏幕缓冲迁移，保留区起点)
            │  屏幕缓冲顶部保留区                    │
            │  1 MB（EXT_SCREEN_RESERVE）           │
            │  逻辑画布放大时（hook_screen_dim_write）│
            │  缓冲迁移至此，bump 堆对此区间关闭       │
0x01010000  └─────────────────────────────────────┘  (EXT_BASE_ADDR + EXT_MEM_SIZE)

平台扩展区（独立 Unicorn 映射，arm_ptr() 分段解析）
───────────────────────────────────────────────
0x40000000  EXT_PLATFORM_MEM      2 MB   MTK 图形存储带
0x50000000  EXT_SCRRAM           16 MB   图像/Flash 平台扩展内存
0x70000000  EXT_EXECUTOR_META    64 KB   宿主执行器元数据（ARM 代码不应直接写）
0x7FFF0000  MRP_VFD_BASE                虚拟文件描述符
0x80000000  EXT_PLATFORM_IO_MEM 18 MB   平台 ROM/MMIO（RX4.5 0x80110000 等）
0xA0000000  EXT_PLATFORM_ALT_MEM 2 MB   备用平台存储
```

### 1.3 各区段所有者

| 区段 | 分配者 | 使用者 |
|---|---|---|
| EXT 桥接表 `[0x10000, 0x10258)` | 宿主 `init_table` | ARM guest（通过 GOT 间接调用） |
| origin_mem 池 | 宿主 `init_table` → table[0]~[2] | ARM guest（mr_malloc/mr_free） |
| bump 堆 | 宿主 `arm_alloc` | 宿主分配 extChunk/P/屏幕缓冲/子模块暂存 |
| wrapper 栈区 `[0xE00000, 0xE80000)` | 宿主保留（跳过） | ARM guest（wrapper call frame） |
| wrapper 代码 `[0xE80000, +code_len)` | 宿主 `arm_ext_load` | ARM guest 执行 |
| 屏幕缓冲顶部保留区 | 宿主 `hook_screen_dim_write` | 宿主（渲染时读取） |
| PLATFORM_MEM / IO / ALT | 宿主按需 mmap | ARM guest（MTK 硬件寄存器地址空间） |
| SCRRAM | table[38] 首次 1014 请求时惰性映射 | ARM guest（大图像、Flash、离屏数据） |
| EXECUTOR_META | 宿主专用 | 宿主执行器（ARM 不应直接访问） |

### 1.4 证据来源

| 条目 | 来源文件 | 证据强度 |
|---|---|---|
| 所有地址常量 | `src/include/arm_ext_internal.h:16-54` | 【SDK 定义】 |
| 屏幕缓冲迁移语义 | `src/arm_ext_executor.c:700-711,713-730` | 【SDK 定义】 |
| PLATFORM_IO 18 MB 映射 | `omx_wiki/rx4-5-arm-ext-0x80000000-platform-io-mapping.md` | 【运行观测】 |
| bump 跳过栈/代码区注释 | `src/arm_ext_executor.c:688-710` | 【SDK 定义】 |
| EXT_LOW_TABLE_SIZE 64 KB 原因 | `src/include/arm_ext_internal.h:39-42` | 【运行观测】（gwkdl 崩溃） |

---

## 2. mr_c_function_P_t（P 结构）

### 2.1 结构体定义

```c
typedef struct mr_c_function_P_t {
    uint32 start_of_ER_RW;   // +0x00
    uint32 ER_RW_Length;     // +0x04
    int32  ext_type;         // +0x08
    uint32 mrc_extChunk;     // +0x0C
    int32  stack;            // +0x10
} mr_c_function_P_t;   // 总大小 20 字节（0x14）
```

### 2.2 字段表

| 偏移 | 字段名 | 类型 | 语义 | 读写方 |
|---|---|---|---|---|
| `+0x00` | `start_of_ER_RW` | `uint32` | 该 EXT 模块 RW 段（ER_RW）的 ARM 可见基址；子模块 R9 寄存器值 | ARM guest 在入口时填写；宿主读取（`arm_ptr(m, p_addr)`） |
| `+0x04` | `ER_RW_Length` | `uint32` | RW 段字节长度；宿主用于确认 RW 范围合法，也是 compact 堆保护集的 ER_RW 区间来源 | ARM guest 填写；宿主读取（`arm_ptr(m, p_addr+4)`） |
| `+0x08` | `ext_type` | `int32` | EXT 模块类型标识（具体枚举值**【待核实】**） | ARM guest 填写；宿主诊断读取 |
| `+0x0C` | `mrc_extChunk` | `uint32` | 指向对应的 extChunk 节点地址；宿主通过此字段定位 extChunk，验证 `p_ext_chunk == ext_chunk` | ARM guest 在 `mr_c_function_load` 阶段填写；宿主读取 |
| `+0x10` | `stack` | `int32` | 模块私有栈相关字段（**【待核实】**，目前宿主未直接读取此字段） | ARM guest 管理 |

### 2.3 P 结构的定位

- 宿主通过 `m->p_addr`（wrapper P）、`m->primary_p_addr`（primary game P）、`m->active_p_addr`（当前前台模块 P）等字段记录各模块 P 结构的 ARM 地址。
- `arm_ext_wrapper_rw_base_(m)` 等内联函数通过读取 `*(p_addr + 0)` 拿到 RW 基址。
- extChunk 中 `+0x1C` 存有 P 结构地址（见第 3 节），两者互指。

### 2.4 证据来源

| 条目 | 来源文件 | 证据强度 |
|---|---|---|
| 结构体字段定义 | `src/include/arm_ext_internal.h:65-71` | 【SDK 定义】 |
| `+0x00/+0x04` 读取（ER_RW）在保护集 | `src/arm_ext_executor.c:4654-4659` | 【SDK 定义】 |
| `+0x0C`（mrc_extChunk）验证 | `src/arm_ext_executor.c:2194,2203` | 【SDK 定义】 |
| `p_addr` 字段说明 | `src/include/arm_ext_internal.h:155` | 【SDK 定义】 |

---

## 3. extChunk 布局表

extChunk 是 cfunction.ext 私有 loader 分配的 0x38 字节结构体，存放在 bump 堆内（EXT_HEAP_ADDR 以上），magic 字为 `EXT_CHUNK_MAGIC = 0x7FD854EB`。

### 3.1 字段表

| 偏移 | 字节数 | 字段语义 | 读写方 | 证据强度 |
|---|---|---|---|---|
| `+0x00` | 4 | **magic**：`0x7FD854EB`（EXT_CHUNK_MAGIC），标识有效 extChunk | guest 填写，宿主校验 | 【SDK 定义】 |
| `+0x04` | 4 | **init_func**：子模块入口地址（`file_base + 8`）；loader 在 `mr_cacheSync` 前写入 | guest loader 写；宿主读取验证（`init_func == file_addr + 8`） | 【SDK 定义】 |
| `+0x08` | 4 | **helper_addr**：子模块的回调 helper 函数地址（Thumb，可能带 bit0=1）；子模块 entry 执行后填写 | guest 子模块入口填写；宿主读取后注册模块 | 【SDK 定义】 |
| `+0x0C` | 4 | **chunk_file**：子模块文件在 ARM 地址空间的起始地址（`file_addr`） | guest loader 写；宿主校验 `chunk_file == file_addr` | 【SDK 定义】 |
| `+0x10` | 4 | **chunk_len**：子模块文件字节长度 | guest loader 写；宿主校验 `chunk_len == file_len` | 【SDK 定义】 |
| `+0x14` | 4 | **chunk_rw**：子模块 RW 段基址（entry 后回写，loader 写 0 暂存）；与 P+0x00 一致 | guest 入口后填写；宿主在 sync 时校验 | 【SDK 定义】 |
| `+0x18` | 4 | **chunk_rw_len**：RW 段字节长度；与 P+0x04 一致 | guest 入口后填写；宿主验证 | 【SDK 定义】 |
| `+0x1C` | 4 | **p_addr**：P 结构的 ARM 地址（`mr_c_function_P_t *`）；`file_base+4` 处也存有此值 | guest loader 写；宿主读取（`memcpy(&p_addr, arm_ptr(m, ext_chunk + 0x1C), 4)`） | 【SDK 定义】 |
| `+0x20` | 4 | **p_len**：P 结构区域字节长度（需 `>= sizeof(mr_c_function_P_t)` = 20） | guest loader 写；宿主校验 | 【SDK 定义】 |
| `+0x24` | 4 | **param**（code=2 分发参数）：`arm_ext_call_dispatch` 读此字段作为 timer 回调参数 | guest wrapper 写；宿主读取（`memcpy(&param, arm_ptr(m, ext_chunk + 0x24), 4)`）；cfunction.ext 0xE83098/0xE83514 detach/relink | 【SDK 定义】 |
| `+0x28` | 4 | **dispatch**（code=2 分发函数指针）：wrapper timer dispatch 入口地址，Thumb（bit0=1）；宿主判断是否为 wrapper 代码段内有效地址 | guest wrapper 填写；宿主读取（`memcpy(&func, arm_ptr(m, ext_chunk + 0x28), 4)`） | 【SDK 定义】 |
| `+0x2C` | 4 | **chunk_record**：私有 loader 分配的 module record 地址（EXT_TABLE 镜像，大小 `PRIVATE_CHILD_RECORD_SPAN = 0x248` 字节） | guest loader 写；宿主校验 `chunk_record == record` | 【SDK 定义】 |
| `+0x30` | 4 | **【待核实】**：此偏移宿主未在已审阅代码中直接读取；可能为 loader 或 SDK 内部字段 | — | 待核实 |
| `+0x34` | 4 | **suspend_depth**（pause/suspend 计数）：wrapper 进入模态子界面时递增，退出时递减；`> 0` 表示 primary 模块处于挂起（foreground child 存在） | guest cfunction.ext 在 0xE83098/0xE83514 递增，0xE83590/0xE835FC 递减；宿主读取（`arm_has_suspended_foreground_child`） | 【wrapper 反汇编】 |
| `+0x35` ~ `+0x37` | 3 | **【待核实】**：宿主仅以 `arm_ptr(m, ext_chunk + 0x37u)` 验证该字节是否映射（校验 extChunk 覆盖至少 0x38 字节），但不读取该字段内容 | — | 待核实 |

**注意**：extChunk 最小有效大小为 0x38 字节（`arm_ext_has_internal_loader_chunk` 以 `ext_chunk + 0x38 <= m->heap_top` 作为扫描终止条件）。

### 3.2 file_base 头部格式（子模块映像前 8 字节）

| 偏移（相对 file_addr） | 字段 | 语义 |
|---|---|---|
| `+0x00` | `record`（uint32） | module record 地址（须与 `extChunk+0x2C` 一致） |
| `+0x04` | `p_addr`（uint32） | P 结构地址（须与 `extChunk+0x1C` 一致） |
| `+0x08` | `entry`（uint32） | 子模块入口（须等于 `extChunk+0x04` 处的 `init_func`） |

### 3.3 证据来源

| 条目 | 来源文件:行 | 证据强度 |
|---|---|---|
| magic、+0x04~+0x2C 全部字段 | `src/arm_ext_executor.c:2161-2224`（`arm_ext_read_internal_loader_tuple`） | 【SDK 定义】 |
| +0x34 suspend_depth 语义 | `src/arm_ext_executor.c:2677-2679`、`5921-5924` | 【wrapper 反汇编】 |
| +0x24/+0x28 dispatch | `src/arm_ext_executor.c:9910-9914`、`9830-9831` | 【SDK 定义】 |
| +0x24 relink 注释（0xE83098/0xE835FC） | `src/arm_ext_executor.c:5921-5924` | 【wrapper 反汇编】 |
| 0x38 字节最小大小边界 | `src/arm_ext_executor.c:2232-2236` | 【SDK 定义】 |
| suspend_depth 引用 wrapper 地址 | `docs/gxdzc-start-app-fix.md`、memory MEMORY.md `[Wrapper suspend counter 0x34]` | 【运行观测】 |

---

## 4. compact 堆控制块

### 4.1 控制块定位

compact 堆是某些 SDK 版本（gzwdzjs game.ext、verdload.ext 等）内嵌的私有堆分配器，其控制块地址存放在 **R9（RW 基址）相对偏移** 处：

```
ctrl_addr = *(rw_base + wrapper_compact_heap_ctrl_off)
```

其中 `wrapper_compact_heap_ctrl_off` 由 `find_wrapper_compact_heap_free_return` 从 wrapper 代码中的 LDR-literal 指令解析得到（范围限定 `[0x80, 0x1000)` 且 4 字节对齐）。

宿主在 `arm_ext_sanitize_compact_heap_for_rw` 中通过以下步骤访问控制块：

```c
uint32_t ctrl_slot = rw + m->wrapper_compact_heap_ctrl_off;
uint32_t ctrl = *(uint32_t *)arm_ptr(m, ctrl_slot);  // 控制块指针
```

### 4.2 控制块字段表

| 偏移 | 字节数 | 字段语义 | 读写方 | 证据强度 |
|---|---|---|---|---|
| `+0x00`~`+0x07` | 8 | **【待核实】**：宿主未直接读取 +0 和 +4 处字段，但 `arm_ext_compact_heap_cut_range` 要求控制块至少映射 `0x1C` 字节 | — | 待核实 |
| `+0x08` | 4 | **base**：堆区起始地址（ARM 地址） | guest 分配器读；宿主读取（`arm_ext_read_u32_or_zero_(m, ctrl + 0x08u)`） | 【wrapper 反汇编】 |
| `+0x0C` | 4 | **free_bytes**：当前空闲字节数；cut_range 裁剪时相应减少此值 | guest 分配器读写；宿主读写（`ctrl + 0x0Cu`） | 【wrapper 反汇编】 |
| `+0x10` | 4 | **end**：堆区末尾地址（ARM 地址，exclusive） | guest 分配器读；宿主读取（`arm_ext_read_u32_or_zero_(m, ctrl + 0x10u)`） | 【wrapper 反汇编】 |
| `+0x14`~`+0x17` | 4 | **【待核实】** | — | 待核实 |
| `+0x18` | 4 | **free_list head**（相对堆基址的偏移量 `off`）：空闲链表头；`base + off` 是第一个空闲节点地址 | guest 分配器读写；宿主读写（`prev_slot = ctrl + 0x18u`） | 【wrapper 反汇编】 |

注：控制块大小宿主仅验证至 `0x1C` 字节（`arm_ext_compact_heap_cut_range` 的 `arm_ext_addr_range_mapped(m, ctrl, 0x1Cu)` 检查），`+0x18` 之后的字段宿主不访问。

### 4.3 free-list 节点格式

free-list 采用偏移量（相对堆基址 `base`）链表：

```
节点地址 = base + off
节点结构（8 字节，按 8 字节对齐）：
  +0x00  uint32  next_off   下一个空闲节点相对 base 的偏移；0 或 >= heap_len 表示链表末尾
  +0x04  uint32  len        本节点（含头）的字节数；最小 8 字节
```

链表遍历示意：
```
off = *(ctrl + 0x18)        // 链表头偏移
while off < heap_len:
    node = base + off
    next_off = *(node + 0)
    len = *(node + 4)
    ... 处理 [node, node+len) 区间 ...
    off = next_off
```

### 4.4 find_wrapper_compact_heap_free_return 匹配模式

`find_wrapper_compact_heap_free_return`（`src/arm_ext_executor.c:1837`）在 wrapper 代码中定位 compact mr_free() 的 pop/return 指令地址，同时提取 `ctrl_off`（LDR-literal 立即数）。匹配序列共 17 个固定偏移的半字，包括：

- `off+0x02`: `0xB4F0`（push 入口）  
- `off+0x1C`: `0x691E`（ldr from ctrl+0x18 = free-list head）  
- `off+0x2E`: `0x3118`（加 0x18 偏移）  
- `off+0x30`: `0x6998`（ldr ctrl+0x0C = free_bytes）  
- `off+0x8C`: `0xBCF0` / `off+0x8E`: `0x4770`（pop + bx lr，即 hook 点）

### 4.5 证据来源

| 条目 | 来源文件:行 | 证据强度 |
|---|---|---|
| 控制块 +0x08/+0x0C/+0x10/+0x18 | `src/arm_ext_executor.c:4488-4568`（`arm_ext_compact_heap_cut_range`） | 【wrapper 反汇编】 |
| 定位 ctrl_off 方式（LDR-literal） | `src/arm_ext_executor.c:1837-1887`（`find_wrapper_compact_heap_free_return`） | 【wrapper 反汇编】 |
| free-list 节点 {next_off, len} 格式 | `src/arm_ext_executor.c:4503-4513` | 【wrapper 反汇编】 |
| wrapper_compact_heap_ctrl_off 字段含义 | `src/include/arm_ext_internal.h:254` 注释 | 【SDK 定义】 |
| mr_free() 语义注释 | `src/arm_ext_executor.c:1855-1861` | 【wrapper 反汇编】 |

---

## 5. timer 节点格式与调度器偏移矩阵

### 5.1 magic 值

```c
#define ARM_EXT_COMPACT_TIMER_MAGIC 0x79ABBCCFu  // src/arm_ext_executor.c:4339
```

此 magic 出现在两种不同节点布局中，位置不同（详见下文）。

### 5.2 节点布局变体 A：compact/game 变体

magic 位于节点 `+0x00`，链接字段在尾部。gzwdzjs game.ext 使用此布局（节点大小 0x20 字节，分配时实际请求 0x20+4 = 0x24，compact 分配器对齐到 0x28）：

| 偏移 | 字节数 | 字段语义 | 证据强度 |
|---|---|---|---|
| `+0x00` | 4 | **magic** = `0x79ABBCCF` | 【wrapper 反汇编】 |
| `+0x04` | 4 | **interval**（定时器间隔，ms，`>= 10`） | 【wrapper 反汇编】（`gzwdzjs-tutorial-loading-hang-debug.md`） |
| `+0x08` | 4 | **remaining**（剩余时间） | 【运行观测】 |
| `+0x0C` | 4 | **callback**（回调函数指针） | 【运行观测】 |
| `+0x10` | 4 | **arg** | 【运行观测】 |
| `+0x14` | 4 | **param** | 【运行观测】 |
| `+0x18` | 4 | **delta 链 next**（delta queue 下一节点） | 【wrapper 反汇编】（`arm_ext_collect_compact_timer_chain` next_off=0x18） |
| `+0x1C` | 4 | **fired 链 next**（fired/current queue 下一节点） | 【wrapper 反汇编】（next_off=0x1C） |

宿主有效性验证条件（`arm_ext_compact_timer_magic_node_is_valid`）：节点起始 0x20 字节全部映射，且 `*(node+0x00) == 0x79ABBCCF`。

### 5.3 节点布局变体 B：wrapper（optwar cfunction.ext）变体

magic 位于节点 `+0x08`，链接字段在头部（类似双向链表，`next/prev` 在 +0/+4）：

| 偏移 | 字节数 | 字段语义 | 证据强度 |
|---|---|---|---|
| `+0x00` | 4 | **next**（队列链接） | 【wrapper 反汇编】（optwar `0xE84920`） |
| `+0x04` | 4 | **prev** | 【wrapper 反汇编】 |
| `+0x08` | 4 | **magic** = `0x79ABBCCF` | 【wrapper 反汇编】 |
| `+0x0C` | 4 | **due_time**（到期时间） | 【wrapper 反汇编】 |
| `+0x10`~`+0x18` | — | **【待核实】** 中间字段 | 待核实 |
| `+0x14` | 4 | **callback** | 【wrapper 反汇编】（"invokes callback/data fields through +0x14/+0x18/+0x1C"） |
| `+0x18` | 4 | **data** | 【wrapper 反汇编】 |
| `+0x1C` | 4 | **param** | 【wrapper 反汇编】 |

宿主有效性验证条件（`arm_ext_wrapper_timer_node_is_valid`）：节点起始 0x20 字节全部映射，且 `*(node+0x08) == 0x79ABBCCF`。

### 5.4 节点布局变体 C：compact wrapper 变体

magic 位于 `+0x00`，链接字段在 `+0x18/+0x1C`（与变体 A 一致，但宿主校验的映射长度只到 0x1C+4 = 0x20）：

| 偏移 | 字节数 | 字段语义 | 证据强度 |
|---|---|---|---|
| `+0x00` | 4 | **magic** = `0x79ABBCCF` | 【wrapper 反汇编】 |
| `+0x01`~`+0x17` | — | 节点主体（**【待核实】**具体字段与变体 A 是否相同） | 待核实 |
| `+0x18` | 4 | **sorted queue next**（sorted/pending 链链接） | 【wrapper 反汇编】（`arm_ext_wrapper_compact_timer_queue_at`：`[R6+0x0c]` 作为排序队列） |
| `+0x1C` | 4 | **active/due chain next** | 【wrapper 反汇编】（`[R6+0x10]` 作为已触发链） |

### 5.5 调度器偏移矩阵

以下各偏移均相对于对应模块的 **RW 基址（R9）**：

| R9-相对偏移 | 调度器结构 | 字段含义 | 应用/wrapper | 证据强度 |
|---|---|---|---|---|
| `+0x84`（或 `+0x80`） | game.ext 调度器头 | `+0x08` delta queue，`+0x0C` fired queue，`+0x10` deadline | gzwdzjs（`+0x84` 主，`+0x80` 备） | 【运行观测】 |
| `+0x94`（`+0x9C`/`+0xA0`） | frame.ext 专用调度器 | `+0x08`（`+0x9C`）queued，`+0x0C`（`+0xA0`）active | DOTA frame.ext `0x2C96A0`、`0x2C9538` | 【wrapper 反汇编】 |
| `+0xC0` | compact child 调度器头 | `+0x08` queued，`+0x0C` current | DOTA/verdload 等 compact child | 【wrapper 反汇编】 |
| `+0x248` | compact child 调度器头（另一变体） | `+0x08` queued，`+0x0C` current | DOTA/verdload 等 compact child | 【wrapper 反汇编】 |
| `wrapper_compact_timer_scheduler_off` | compact wrapper 调度器头（动态） | `+0x0C` sorted queue，`+0x10` active chain，`+0x14` last tick | gxdzc/gghjt/verdload/optwar compact wrappers | 【wrapper 反汇编】 |
| `+0x3C8` | 旧版 wrapper 调度器（queued） | 存放 queued 头节点地址（变体 B 格式节点） | wrapper（gxdzc/gghjt cfunction.ext 直接调度布局） | 【wrapper 反汇编】 |
| `+0x3D8` | 旧版 wrapper 调度器（active） | 存放 current/active 节点地址（变体 B 格式节点） | wrapper（同上） | 【wrapper 反汇编】 |
| `+0x04` | wrapper 旧版忙标志 | `wrapper_rw+4 != 0` 表示 wrapper timer queue 非空（旧直接调度布局专用） | cfunction.ext `0x1F50` 附近 | 【wrapper 反汇编】 |

调度器头通用子结构（compact/game 调度器）：

```
scheduler_header（R9 + scheduler_off）：
  +0x00  uint32  【待核实】
  +0x04  uint32  【待核实】
  +0x08  uint32  queued_head     （delta 队列头节点地址，game.ext 变体）
  +0x0C  uint32  current_head   （fired/current 队列头节点地址，game.ext 变体）
         或      sorted_queue_head（compact wrapper 变体）
  +0x10  uint32  deadline / active_chain / last_tick（因变体而异）
  +0x14  uint32  last_tick_timestamp（compact wrapper 变体）
```

### 5.6 证据来源

| 条目 | 来源文件:行 | 证据强度 |
|---|---|---|
| magic 定义 | `src/arm_ext_executor.c:4339` | 【SDK 定义】 |
| 变体 A 全字段（gzwdzjs 反汇编） | `docs/gzwdzjs-tutorial-loading-hang-debug.md:28-36` | 【wrapper 反汇编】 |
| 变体 B 字段（optwar 0xE84920） | `src/arm_ext_executor.c:2803-2808` | 【wrapper 反汇编】 |
| 变体 C 链接字段（0x18/0x1C） | `src/arm_ext_executor.c:2811-2819` | 【wrapper 反汇编】 |
| game.ext R9+0x84/0x80 调度器 | `src/arm_ext_executor.c:4422-4440` | 【运行观测】（gzwdzjs） |
| frame.ext R9+0x94/0x9C/0xA0 | `src/arm_ext_executor.c:2710-2720` | 【wrapper 反汇编】（DOTA 0x2C96A0） |
| compact child R9+0xC0/0x248 | `src/arm_ext_executor.c:2763-2768`、`4475-4484` | 【wrapper 反汇编】 |
| compact wrapper scheduler_off（0x0C/0x10/0x14） | `src/arm_ext_executor.c:2826-2840` | 【wrapper 反汇编】 |
| 旧版 0x3C8/0x3D8 | `src/arm_ext_executor.c:2850-2858` | 【wrapper 反汇编】 |
| wrapper_rw+4 忙标志 | `src/arm_ext_executor.c:2785-2796` | 【wrapper 反汇编】 |
| MEMORY.md gzwdzjs-tutorial-frame-stop | 项目 memory 文件 | 【运行观测】 |

---

## 6. 模块生命周期状态

### 6.1 状态概念

宿主执行器通过以下 `ArmExtModule` 字段维护多个并发存在的 EXT 模块实例：

| 字段 | 语义 |
|---|---|
| `p_addr` / `helper_addr` | wrapper EXT 的 P 地址和 helper 地址（整个 session 不变） |
| `primary_p_addr` / `primary_helper_addr` | primary 子模块（game.ext）的 P/helper；由第一次 sync 确定后不再改变（除 dump0 恢复路径） |
| `primary_file_addr` / `primary_file_len` | primary 子模块映像在 ARM 地址空间的位置 |
| `active_p_addr` / `active_helper_addr` | 当前前台活动模块的 P/helper；有 foreground child 时指向该 child，否则等于 primary |
| `current_p_addr` / `current_helper_addr` | 当前 `run_arm_with_sp` 执行上下文中的模块 P/helper（从 LR 推断） |
| `timer_p_addr` / `timer_helper_addr` | 最近一次 table[31]（timerStart）调用归属的模块 P/helper |
| `nested_modules[]` | 所有已注册（sync 成功）的子模块列表；最多 `ARM_EXT_NESTED_MODULE_MAX = 64` 条 |
| `pending_internal_file_addr` / `pending_internal_file_len` | 已暂存（cacheSync 完成但未 BLX 返回）的子模块文件地址 |

### 6.2 状态转换点

```
wrapper 代码 BLX 子模块入口
  └──> [子模块写 extChunk+0x08 = helper，extChunk+0x14/0x18 = rw/rw_len]
         └──> arm_ext_sync_internal_nested_module()   (代码归属检测)
                └──> arm_ext_record_nested_module()    (注册到 nested_modules[])
                     → active_p_addr = p_addr          (active 更新)
                     → primary_p_addr 若为首次确定     (primary 记录)
                       pending_internal_file_addr = 0   (清除 pending)

wrapper 调 table[44]（readFile）且读地址与已注册模块重叠
  └──> arm_ext_retire_modules_overwritten_by_data_read()
         └──> arm_ext_drop_overlapping_stale_nested_modules()
                → nested_modules 中重叠的非 primary 条目被移除（drop）
                → 若 active_p_addr 被 drop，active_p_addr = 0

dump0 路径：wrapper table[44] 读回整个 primary 映像
  └──> arm_ext_restore_primary_mapping_after_dump0()
         → nested_modules 中重叠的非 primary 条目被移除
         → arm_ext_record_nested_module() 重新注册 primary
         → active_p_addr 还原为 primary_p_addr

模态子界面关闭（arm_ext_reset_child_modules）
  └──> nested_modules 中所有非 primary 条目清除
       active_p_addr 置 0 或还原 primary

timer owner 记录（arm_ext_record_timer_owner，table[31] 钩子）
  └──> 从 LR 推断调用者 P（arm_ext_p_for_code_addr）
       → timer_p_addr / timer_helper_addr 更新
       → LR 在 wrapper 代码范围时优先 current_p_addr（子 helper 发起的 timerStart）
```

### 6.3 关键函数一览

| 函数 | 行号（约） | 作用 |
|---|---|---|
| `arm_ext_record_nested_module` | 1233 | 注册子模块到 nested_modules[]（有则更新，无则追加） |
| `arm_ext_sync_internal_nested_module` | 2243 | 扫描 heap 确认 extChunk 有效，调 record_nested_module，更新 active/primary |
| `arm_ext_drop_overlapping_stale_nested_modules` | 2499 | 移除与 [file_addr, +file_len) 重叠的非 primary 子模块（drop/retire） |
| `arm_ext_retire_modules_overwritten_by_data_read` | 2545 | table[44] 读文件时调用，触发 drop 并可能还原 active = primary |
| `arm_ext_restore_primary_mapping_after_dump0` | 2581 | dump0 恢复后重建 primary 代码归属 |
| `arm_ext_reset_child_modules` | 9865 | 清除所有非 primary 模块注册（模态框关闭后） |
| `arm_ext_record_timer_owner` | 3728 | table[31] 钩子，根据 LR 记录 timer_p_addr |

### 6.4 证据来源

| 条目 | 来源文件:行 | 证据强度 |
|---|---|---|
| ArmExtModule 相关字段定义 | `src/include/arm_ext_internal.h:233-310` | 【SDK 定义】 |
| sync/record/drop/retire/restore 函数 | `src/arm_ext_executor.c:1233,2243,2499,2545,2581,9865` | 【SDK 定义】 |
| timer owner 推断逻辑 | `src/arm_ext_executor.c:3728-3775` | 【SDK 定义】 |
| suspend_depth 与 foreground child 状态 | `src/arm_ext_executor.c:2670-2679` | 【wrapper 反汇编】 |
| gxdzc dump0 恢复路径注释 | `src/arm_ext_executor.c:2592-2604` | 【运行观测】 |
| dota 模态框事件路由修复 | `src/include/arm_ext_internal.h:316-325` | 【运行观测】 |

---

## 7. guest 内存分配器三层

### 7.1 架构全景

```
table[0]（mr_malloc）→ 优先 origin_mem first-fit → 失败时 bump 后备
table[1]（mr_free）  → origin_mem 池内归还，bump 块可回收（bump_reuse）
table[2]（mr_realloc）→ 同上
table[125]（readFile）→ 内部调 mr_malloc，输出落在 origin_mem 或 bump 区

guest compact 分配器（game.ext/verdload.ext 内嵌）→ 从 wrapper 分配的大 arena 内细分
```

### 7.2 第一层：LG_mem origin_mem 池（first-fit）

**初始容量**：`init_table` 按 `--memory`/`VMRP_MEMORY` 从 bump 区分配一块连续区间，默认 1 MB，可配 1–16 MB，当前 ARM 布局将池长钳制到 8 MB。该初始区间缓存为 `origin_mem_addr..origin_mem_addr+origin_mem_len`，但缓存值不是 table bridge 运行时的最终 ABI 边界。

**可变 ABI slots**：`table[108]` 指向 `LG_mem_base`，`table[110]` 指向 `LG_mem_end`，`table[146]` 是 `&LG_mem_free` 头节点地址。旧 wrapper 会在配对的 setup/teardown 中改写这些 ARM 可见状态，把 `0x40000000` 等平台映射带的外部 arena 临时接入 LG_mem。因此 table[0]/[1]/[2] 每次调用都必须重读当前 base/end/head，不能使用 `init_table` 时的宿主缓存副本。

**头节点**：头节点格式与空闲节点相同：`{next_off, len}`（8 字节）。`next_off` 始终相对当前 `LG_mem_base`；节点可分别落在不同 ARM 映射带，数值上的 `base..end` 不要求形成一段宿主连续内存。遍历时必须验证每个 offset、`{next,len}` 元数据和整个节点范围均可映射。

**first-fit 语义**（逐语句对应 `src/mythroad/mem.c` 的 `mr_malloc/mr_free`）：

```
malloc:
  previous = head_addr（头节点）
  nextfree = base + *(previous)
  while nextfree < end:
      if nextfree 空间 >= len:
          按 first-fit 分配，拆分剩余部分，更新 previous→next 链接
          return nextfree（或子区间）
      previous = nextfree
      nextfree = base + *(nextfree)
  return 0（池满，调用方退回 bump）

free:
  找到 freep 使 freep <= p < freep.next，插入新节点或合并相邻节点
```

**空闲节点格式**（与 compact heap 节点格式相同，均为 8 字节）：

```
+0x00  uint32  next_off   相对 base 的偏移（0 表示链表末）
+0x04  uint32  len        本节点（含头）字节数；8 字节对齐
```

对齐宏：`ARM_EXT_LG_MEM_ALIGN(x) = (x + 7) & ~7`

**为何应用能预测 mr_malloc 返回地址**（talkcat 花屏根因）：应用先 mr_free 一块自有内存，再调 readFile（table[125]），readFile 内部 mr_malloc 按 first-fit 必然落回已 free 的地址；应用随后**丢弃 readFile 返回值**，直接从预测地址读取像素。宿主必须严格实现 first-fit 语义才能兼容此行为（见 `src/arm_ext_executor.c:3793-3815`）。

**统计 slots**：`table[111]/[135]/[136]` 分别指向 `LG_mem_left/min/top`。table bridge 先读 ARM slot 当前值再更新，避免用初始化时缓存的统计覆盖 wrapper 已发布的外部 arena 状态。动态 end 存活期间，`LG_mem_top` 由 wrapper 的跨映射带 offset 协议管理；宿主不能用固定 `origin_mem_len` 反推并覆盖它。

**宿主后备分配所有权**：宿主 bump 块可能在数值上也位于动态 `base..end` 内，但它不属于 guest free-list。free 时先查 `bump_live` 所有权；对动态数值区间，只有 `app_live_blocks` 记录的 table 分配才能返回 LG_mem。注册为 EXT 映像的范围会从 live 记录中删除，且 free-list 操作仍会排除可执行区，防止宿主私有存储进入 guest 链。

### 7.3 第二层：bump 后备（arm_alloc）

**语义**：堆顶指针 `m->heap_top` 从 `EXT_HEAP_ADDR (0x200000)` 向上单调增长，分配成功则 `heap_top += len`，**从不回收**（除 bump_reuse 机制）。

**保留区跳过规则**：

```
分配时：
if heap_top < EXT_CODE_ADDR + code_len &&
   heap_top + len > EXT_STACK_ADDR:
    heap_top = EXT_CODE_ADDR + code_len  // 跳过栈+代码保留区
if screen_addr == mem_end - EXT_SCREEN_RESERVE:
    mem_end -= EXT_SCREEN_RESERVE        // 屏幕保留区关闭
if heap_top + len >= mem_end:
    return 0  // 失败
```

**bump_reuse 机制**（`ArmExtBumpBlock bump_free_blocks[]`）：table[0]/[2]/[125] 发出的 bump 块记入 `bump_live[]`；table[1] 释放时若地址在 bump 区且在 `bump_live` 中，将其移入 `bump_free_blocks[]` 以便下次 bump_reuse 复用。这是防止 talkcat 下载路径耗尽 16 MB 的关键机制。

**live blocks 跟踪**（`app_live_blocks[]`）：table[0]/[2]/[125] 发给应用的所有 allocation（含 origin_mem 池内块和 bump 块）统一记录，供 DrawBitmap 边界检查使用。

### 7.4 第三层：wrapper compact 堆

**所有者**：game.ext / verdload.ext 等内嵌的私有分配器；通过 wrapper 的 table[0]（mr_malloc）分配一大块 arena，然后在 arena 内用自己的 compact 分配器细分（timer 节点、精灵缓冲等）。

**关键交互约束**：

1. compact 分配器从 LG_mem/bump 获得的 arena 被 wrapper free 时，arena 地址进入 LG_mem free-list；若 arena 区间包含已注册 EXT 模块的文件映像或 ER_RW，后续 mr_malloc first-fit 会把代码/数据区分给新分配，导致崩溃（gzwdzjs 无 netpay 冻结根因）。
2. 宿主通过 `arm_ext_sanitize_compact_timer_heaps` 在每次 code=2 前从 compact 堆 free-list 裁除 live timer 节点和已注册模块的存活存储，防止这些区间被分配器复用。
3. compact 分配器内部的 free 操作（compact mr_free）通过 `find_wrapper_compact_heap_free_return` 定位的 hook 点触发宿主裁剪。

**三层分配决策流程**：

```
table[0] mr_malloc(len):
  1. arm_ext_guest_mem_malloc(m, len)   // origin_mem first-fit
     success → return addr（记账，sync slot）
  2. arm_ext_bump_reuse(m, len)          // bump_free_blocks 复用
     success → return addr
  3. arm_alloc(m, len)                   // bump 后备
     success → bump_track，return addr
     failure → return 0（MR_FAILED）
```

### 7.5 talkcat / gzwdzjs 案例引用

| 案例 | 症状 | 根因 | 相关文档 |
|---|---|---|---|
| talkcat 主界面花屏 | 位图渲染为 MRP 索引字节 | origin_mem 未用 first-fit，预测地址处残留旧数据 | `src/arm_ext_executor.c:3793-3815` |
| talkcat 下载路径崩溃 | PC 变成像素值（0x6246834A 类） | bump 只增不减，累计分配越过 0xE00000 栈区 | `src/arm_ext_executor.c:688-710` |
| gzwdzjs 无 netpay 冻结 | ER_RW 帧回调指针被像素覆写，blx 到未映射地址 | RAM pack free 后 compact free-list 获得包含 game.ext ER_RW 的空闲段，精灵缓冲复用该段 | `docs/gzwdzjs-no-netpay-freeze-debug.md` |
| gzwdzjs 教程 timer 链表覆写 | UC_MEM_READ_UNMAPPED，SP=0x9 | compact 分配器把精灵缓冲分配到 live timer 节点地址 | `docs/gzwdzjs-tutorial-loading-hang-debug.md` |

### 7.6 证据来源

| 条目 | 来源文件:行 | 证据强度 |
|---|---|---|
| origin_mem first-fit 注释 | `src/arm_ext_executor.c:3791-3817` | 【SDK 定义】 |
| table[146] = &LG_mem_free head 头节点 | `src/include/arm_ext_internal.h:175-179` | 【SDK 定义】 |
| bump 保留区跳过 | `src/arm_ext_executor.c:686-711` | 【SDK 定义】 |
| bump_live / bump_free_blocks | `src/include/arm_ext_internal.h:186-193` | 【SDK 定义】 |
| compact 三层分配器交互约束 | `src/arm_ext_executor.c:4606-4661` | 【运行观测】 |
| origin_mem = 512 KB 原则 | `src/arm_ext_executor.c:3808-3814` | 【运行观测】 |
| memory MEMORY.md guest-lg-mem-first-fit | 项目 memory 文件 | 【运行观测】 |
| gzwdzjs 无 netpay 根因链 | `docs/gzwdzjs-no-netpay-freeze-debug.md` | 【运行观测】 |
