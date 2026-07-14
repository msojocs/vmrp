# 公共 API 与生命周期契约

本文逐项说明 `src/include/arm_ext_executor.h` 暴露的接口。这里描述的是当前实现行为，不把函数名或历史注释中暗示的语义当作事实。

## 1. 使用约束

`ArmExtModule` 是一个不透明、可变且不可重入的 Unicorn 实例。调用方必须遵守以下约束：

- 一个实例从 `arm_ext_load()` 成功开始，到 `arm_ext_unload()` 结束；
- 同一实例的所有 API 必须串行调用，并运行在执行器所属 VM 线程；
- SDL timer、网络线程和 E2E socket 线程只能投递事件，不能直接进入这些 API；
- API 返回的 guest 数据指针依赖实例 backing memory，不得在 unload 后使用；
- helper、P、R9、active owner、timer owner 和 screen owner 是关联状态，不能由调用方单独改写；
- 除 `arm_ext_unload(NULL)` 等明确支持空参数的接口外，不应把失败后的句柄继续传入其它 API。

推荐的最小生命周期：

```c
ArmExtModule *module = NULL;
int32_t guest_load_ret = MR_FAILED;

if (arm_ext_load(&module, code, code_len, load_code,
                 &guest_load_ret) != MR_SUCCESS) {
    /* module 仍由调用方视为 NULL/不可用 */
    return MR_FAILED;
}

uint8 *output = NULL;
int32 output_len = 0;
int32 guest_call_ret = arm_ext_call(module, 0, input, input_len,
                                    &output, &output_len);
/* 在下一次可能覆盖 guest 内存的调用前消费 output。 */

arm_ext_unload(module);
module = NULL;
```

## 2. 两层返回值

执行器经常需要同时表达“宿主是否成功执行”与“guest 函数返回了什么”。两者不能混用。

| API | C 返回值 | guest 返回值 |
| --- | --- | --- |
| `arm_ext_load()` | `MR_SUCCESS/MR_FAILED`，表示实例是否建立并跑完加载入口 | 成功时通过 `ext_ret` 返回加载入口 R0 |
| `arm_ext_call()` | 通常直接返回 helper 的 R0；宿主执行失败时返回 `MR_FAILED` | 与 C 返回值共用；部分原始事件、平台退出和受约束的 callback 兼容结束路径会被规范化为 `MR_SUCCESS` |
| `arm_ext_call_dispatch()` | dispatch 执行结果或 `MR_FAILED/MR_IGNORE` | 正常完成时通常来自目标函数 R0 |
| `arm_ext_invoke0/3()` | 只表示调用机制是否成功 | 通过 `ret_out` 返回 guest R0 |

`arm_ext_load()` 返回 `MR_SUCCESS` 不代表 guest 的 `mr_c_function_load()` 一定返回 `MR_SUCCESS`；调用方需要单独检查 `ext_ret` 所表达的上层协议。

## 3. `arm_ext_load()`

```c
int arm_ext_load(ArmExtModule **out,
                 const uint8 *code,
                 uint32 len,
                 int32 load_code,
                 int32 *ext_ret);
```

参数语义：

| 参数 | 方向 | 说明 |
| --- | --- | --- |
| `out` | 输出 | 仅在成功时写入新实例；调用方应在调用前把自己的变量初始化为 `NULL` |
| `code` | 输入 | wrapper EXT 原始字节；至少 12 字节 |
| `len` | 输入 | wrapper 字节数；必须能放入从 `EXT_CODE_ADDR` 开始的主映射 |
| `load_code` | 输入 | 写入 R0 后传给 `mr_c_function_load()` 的协议值 |
| `ext_ret` | 输出，可空 | 成功时接收加载入口最终 R0；失败时内容不保证更新 |

成功路径完成以下可观察动作：

1. 分配主 backing、平台地址带、低地址镜像和执行器元数据；
2. 创建 Unicorn 并映射所有 guest 区域；
3. 构造 `mr_table`、内部数据槽、应用堆和屏幕缓冲；
4. 预解析当前 MRP；
5. 复制 wrapper，并把文件头第一个 word 改为 `EXT_TABLE_ADDR`；
6. 探测 timer/compact heap/chain walker 模式并安装 hook；
7. 从 `EXT_CODE_ADDR + 8` 执行加载入口；
8. 捕获 helper、P、primary 候选和 timer dispatch 状态。

任一步失败都由函数内部调用 `arm_ext_unload()` 回滚已经建立的资源。`code` 的内容会复制进 guest；实例只保留原 host 指针用于识别调用方是否又把同一 wrapper 作为 helper input 传回，不应依赖该指针承载新的二进制内容。

## 4. `arm_ext_call()`

```c
int arm_ext_call(ArmExtModule *m,
                 int32 code,
                 const void *input,
                 uint32 input_len,
                 uint8 **output,
                 int32 *output_len);
```

### 4.1 输入转换

函数进入时会先把 `*output` 清为 `NULL`、把 `*output_len` 清为 0。输入按以下顺序转换：

1. `input == NULL`：R2 为 0；
2. `input == arm_ext_load()` 的原始 `code` 指针：R2 直接使用 `EXT_CODE_ADDR`；
3. `code == 3` 或 `code == 6`：`input` 被解释为 32 位标量/标记，不解引用；
4. 其它情况：在 guest 中分配 `input_len` 字节并复制 host 输入。

因此 code 3/6 的 `input` 不能被当作普通 host buffer 使用。新增 code 的特殊输入语义时，应在这里显式记录，不能让一个整数地址落入 `memcpy()` 路径。

### 4.2 路由规则

| 条件 | 默认 P/helper |
| --- | --- |
| code 1 且 wrapper 与 primary 分离 | wrapper；原始事件必须先经过 wrapper |
| code 0..5 且 primary 存在 | primary |
| 其它 code | active，缺失时回退 wrapper |
| code 2 且已记录 timer owner | timer owner 覆盖上述选择 |
| code 2 且前台 child 有结构有效的私有队列 | 仅在 child 已拥有 timer 或尚无 owner 时直达 child |

这张表是入口选择，不是完整状态机。guest 返回后函数还会根据 wrapper 队列、primary 队列、child 队列、suspend 深度和新 table[31] 调用重新收敛 timer/active/screen owner。

### 4.3 guest 调用帧

```text
R0 = 选中模块的 P
R1 = code
R2 = guest input 地址或 code 3/6 的原始标量
R3 = input_len
R9 = P.start_of_ER_RW
SP = EXT_STACK_ADDR + EXT_STACK_SIZE - 16

[SP + 0x00] = guest output 指针槽地址
[SP + 0x04] = guest output_len 槽地址
[SP + 0x08] = 0
[SP + 0x0C] = 0
```

### 4.4 输出与生命周期

helper 返回后，执行器读取两个 guest 输出槽：

- `output_len` 接收 guest 写入的 32 位长度；
- `output` 接收 guest 地址经 `arm_ptr()` 转换后的 host 可读指针；
- 执行器不会为调用方复制或释放这段输出；
- 指针最长只在 `ArmExtModule` 存活期间有效，实际内容还可能被后续 guest 分配、free、dump/restore 或模块复用覆盖；
- 调用方读取前必须同时检查返回值、`output != NULL`、`output_len > 0`，并尽快复制需要长期保存的数据。

对于 20 字节的 host 原始事件，wrapper 已经消费事件时函数返回 `MR_SUCCESS`，避免 Lua 的 12 字节转发路径重复处理。同样，进入模态流程或平台退出/重启已经接管控制权时，返回值可能被规范化；这属于事件协议，不是 guest 原始 R0 的透明通道。

## 5. 查询和一次性状态 API

### `arm_ext_helper_addr()`

返回 wrapper helper 地址，`m == NULL` 时返回 0。这是普通 getter，但返回地址仍只属于当前实例的 guest 地址空间。

### `arm_ext_primary_helper()`

尽管名称像 getter，这个函数实际服务于 `mr_timer()` 的分流判断。其契约是：

- 返回 0：宿主 timer 应走普通 `arm_ext_call(code=2)`；
- 返回 primary helper：当前 wrapper extChunk 适合由 `arm_ext_call_dispatch()` 直接调度；
- 返回 primary compact timer walker：应由 `arm_ext_call_dispatch()` 走已探测的 primary walker；
- timer 属于独立 child、wrapper compact queue 必须走公开 helper、模态恢复尚未完成，或结构证据不足时返回 0。

因此不能用它判断“primary 是否已经注册”，也不能假定非零结果总等于 `primary_helper_addr`。调用方目前只把返回值当作布尔分流信号。

### `arm_ext_consume_primary_host_init()`

消费 `primary_host_init_pending` 一次性标志：

- table[25] 首次注册 host 可见 primary 时置位；
- 返回 1 的同一次调用会清零；
- private loader 已自行初始化 child，不设置该标志；
- Mythroad 使用它决定是否补发一次 foreground code 0。

不要通过重复调用轮询模块是否存在；第一次消费后它会永久返回 0，直到新的注册流程再次置位。

### `arm_ext_read_timer_queue()`

读取 primary P 对应 RW 中当前识别出的 game timer head，偏移由 `+0x8C/+0x88` 探测结果决定。它不枚举 wrapper compact queue、frame scheduler、前台 child queue，也不返回“当前 timer owner 的任意队列”。没有实例、没有 primary 或没有有效 head 时返回 0。

## 6. 调度与直接调用 API

### `arm_ext_call_dispatch()`

```c
int arm_ext_call_dispatch(ArmExtModule *m,
                          int is_stop,
                          uint32 timer_interval);
```

该函数由宿主 timer 路径使用。它不是固定调用一个 wrapper 地址，而是在运行时选择：

- 已探测的 primary compact walker；
- wrapper timer dispatch；
- extChunk `event_func/event_data`；
- chain walker 补充路径；
- 不满足直连证据时返回 `MR_IGNORE` 或让上层改走普通 helper code 2。

调用前提是 `arm_ext_primary_helper()` 的当前分流结果非零。`is_stop` 表示 stop/dispatch 选择，`timer_interval` 参与 guest timer ABI；两者不是通用 helper 的 code/input 参数。

### `arm_ext_invoke0()` / `arm_ext_invoke3()`

这两个接口调用已经确认的 guest 函数地址：

```c
int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out);
int arm_ext_invoke3(ArmExtModule *m, uint32 func,
                    uint32 arg0, uint32 arg1, uint32 arg2,
                    int32 *ret_out);
```

- `func` 保留 bit 0 的 ARM/Thumb 语义；
- `invoke0` 把 R0-R3 清零；
- `invoke3` 设置 R0-R2；调用方不应依赖未声明寄存器的旧值；
- 执行前按 active/wrapper 上下文恢复 R9；
- 执行器进入 screen/timer 上下文并在退出时同步状态；
- C 返回 `MR_SUCCESS` 只说明执行完成，guest R0 由 `ret_out` 返回；
- 任一空实例/空函数地址或 Unicorn 失败会把 `ret_out` 预置/保留为 `MR_FAILED` 并返回 `MR_FAILED`。

它们只适合调用归属已经由模块状态证明的函数。把任意 guest 数据地址传入会触发无效指令或错误 R9，而不是自动完成模块识别。

## 7. 模块与 code 同步 API

### `arm_ext_reset_child_modules()`

当前实现会：

- 保留与 primary file range 匹配的 nested module 记录；
- 删除其它 nested module 记录；
- 把 active P/helper 指回 primary；
- 清除 foreground screen owner/cover。

当前实现不会自行清零或重选 timer owner，也不会恢复模态截图、wrapper 前台快照或 guest timer 链。完整的模态关闭路径在调用它前后还会处理这些状态。单独调用该 API 时，调用方不能假定 timer 已经回到 primary。

### `arm_ext_host_cache_sync()`

该接口只在 private loader 已建立 `pending_internal_file_addr/len` 时生效：

1. 将 `min(len, pending_len)` 字节复制到 pending staging 区；
2. 对实际复制范围调用 `uc_ctl_remove_cache()`；
3. 只按实际复制长度重新识别 internal nested module。

它不是任意 guest 地址的通用 cache flush API。没有 pending internal file、`host_data == NULL` 或 `len == 0` 时直接返回。

## 8. `arm_ext_unload()`

`arm_ext_unload(NULL)` 是安全空操作。有效实例会按可处理半初始化状态的顺序释放：

- app compatibility profile state；
- MRP cache 和 VFD；
- Unicorn engine；
- 最近文件副本、模态快照和短包名；
- bump/app live/free 记录；
- screen damage/present/cover 行区间；
- 低地址、平台、元数据和主 backing memory；
- `ArmExtModule` 自身。

unload 不会替调用方清空其句柄变量，也不会复制仍由调用方引用的 guest 输出。调用方必须立即把外部句柄和所有派生指针置为不可用。

## 9. 状态收敛时序

```text
host event/timer
    |
    v
校验实例与入口不变量
    |
    +-- 转换 input/output 槽
    +-- 选择 P/helper/timer owner
    +-- 设置 R0-R3、R9、SP
    +-- 同步 host internal slots -> guest
    v
run_arm_with_sp()
    |
    +-- table hook -> host API -> 回写 R0/PC=LR
    +-- block hook -> 按 PC 修正跨模块 R9
    +-- screen/GOT/invalid/intr hooks
    v
同步 guest timer、模块、屏幕与退出状态
    |
    +-- 重新判断 wrapper/primary/child queue liveness
    +-- 转移或清理 timer/active/screen owner
    +-- 处理 suspend 0->N / N->0 和模态快照
    +-- 必要时重新启动 host tick
    v
返回 guest output 与协议返回值
```

新增早退分支时，必须确认 `current_p/helper`、`in_dispatch`、screen context 和 timer 同步都已恢复。最稳妥的做法是沿用现有执行前后收敛路径，不在中间直接返回。
