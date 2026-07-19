# cfunction.ext x86 化整合重构方案

## 1. 目标与结论

目标：把当前项目对外部 `cfunction.ext` 文件的运行时依赖去掉，不再要求用斯凯 SDK/ARMCC 在特定环境中单独编译并提取该文件；将其中的 Mythroad/DSM 层以 **x86 原生代码** 的形式编入本项目，并保留运行 MRP 应用的能力。

结论：可行，但不能简单地把 `cfunction.ext` 转成数组或把 ARM 指令“嵌入”可执行文件。真正的 x86 化应采用 **“Mythroad/DSM 原生化 + 用户 MRP C 扩展仍由 Unicorn 执行”** 的混合架构：

- 外层 `cfunction.ext` 当前承担 Mythroad/DSM 运行时职责，应改为由 `src/mythroad/` 中的 C 源码编译成 x86 静态库/对象库。
- MRP 包内的 `start.mr`、`*.ext` 等 C 扩展仍是 ARM 原始代码，不能被 x86 直接函数调用；需要保留一个 ARM EXT 执行器，基于 Unicorn 执行这些“用户应用 EXT”。
- 这样可以消除项目启动时对 `/cfunction.ext` 文件和斯凯 SDK 产物的依赖，同时避免一次性重写所有 MRP C 应用的 ARM 执行能力。

## 2. 当前项目证据

### 2.1 文件依赖位置

当前启动路径在 `src/skyengine.c`：

- `loadCode()` 固定读取 `cfunction.ext`。
- 文件内容写入 Unicorn 映射内存的 `CODE_ADDRESS`。
- `bridge_ext_init()` 设置 `mr_table` 后从 `CODE_ADDRESS + 8` 执行 `mr_c_function_load()`。

Web 版本在 `wasm/dist/fs.js` 预加载：

- `"/cfunction.ext"  // mythroad层`

README 也明确写到：需要单独编译 `skyengine.mrp`，再从中提取 `cfunction.ext`。

### 2.2 cfunction.ext 形态

当前主文件：

- `wasm/dist/fs/cfunction.ext`：332116 字节，`file` 识别为 data。
- 前 8 字节为 `MRPGCMAP`，实际入口从偏移 `+8` 开始。
- `mrc/asm/mr_cfunction.fmt` 是 `ELF 32-bit LSB executable, ARM`，说明构建链确实是 ARM 产物。

`mrc/asm/cfunction.ext.s`、`doc/MRP模拟器解决方案.txt` 和 `doc/build_cmd.txt` 已记录 EXT 约定：

| 偏移 | 含义 |
| --- | --- |
| `+0` | `mr_table` 指针，由宿主/平台填入 |
| `+4` | `mr_c_function_st` 指针 |
| `+8` | `mr_c_function_load(int32 code)` 入口 |

`mr_c_function_load()` 会回调 `mr_table + 0x64` 的 `_mr_c_function_new()`，传出 `mr_helper/mr_extHelper` 地址，之后所有 DSM/MRP 事件都通过 helper 分发。

### 2.3 已有可复用源码

`src/mythroad/` 已包含可编译的 Mythroad/DSM 源码基础：

- `src/mythroad/dsm.c` 提供 `dsm_init(DSM_REQUIRE_FUNCS *inFuncs)`。
- `src/mythroad/mythroad.c` / `mythroad_mini.c` 提供 `mr_start_dsm()`、`mr_event()`、`mr_timer()`、`mr_pauseApp()`、`mr_resumeApp()`、`mr_stop()` 等运行时入口。
- `src/mythroad/Makefile` 已存在 `full`/`mini` 的 gcc 构建方向，但当前顶层 `CMakeLists.txt` 没有集成这些源码。

因此重构重点不是从二进制反编译 `cfunction.ext`，而是把已有 Mythroad 源码纳入顶层构建，并把原来由 ARM EXT/bridge 完成的调用边界替换成清晰的原生接口。

### 2.4 `temp/jni` 的参考价值评估

`temp/jni/` 有参考价值，但应作为“移植参考/对照实现”，不能直接整体搬到本项目作为 x86 方案。

可参考点：

- `temp/jni/CMakeLists.txt`、`temp/jni/src/CMakeLists.txt`、`temp/jni/src/mr/CMakeLists.txt` 展示了把 Mythroad VM 拆成 `mr_vm_mini` / `mr_vm_full` 静态库，再由平台层 `mrpoid` 共享库链接的结构。这与本方案的 `native_mythroad` + 平台 adapter 分层一致。
- `temp/jni/src/emulator.c` 展示了原生平台入口：启动时先 `dsm_init()`，再根据 FULL/MINI 选择 `mr_start_dsm(runMrpPath)` 或 `mr_start_dsmC("cfunction.ext", runMrpPath)`；暂停、恢复、定时器、按键事件直接转成 `mr_pauseApp()`、`mr_resumeApp()`、`mr_timer()`、`mr_event()`。这可作为 `runtime_native_mythroad.c` 的调用顺序参考。
- `temp/jni/src/dsm.c`、`temp/jni/src/cport.c` 提供了较完整的平台 API 适配样例，包括内存、文件、时间、定时器、屏幕、网络、字符串/内存函数封装。它们可帮助补齐本项目 `native_dsm_funcs` 或 `mr_table` 中尚未实现的函数。
- `temp/jni/src/mythroad/mythroad.c` 中的 `_mr_c_function_table`、`_mr_c_function_new()`、`_mr_TestComC(800/801)`、`mr_doExt()` 与当前项目的 EXT 加载流程高度相似，可作为核对 `mr_table` 字段顺序和启动消息 `6/8/0` 的参考。

限制与不能直接复用的原因：

- `temp/jni/src/mr_pre/libmr_vm_full.a` 和 `libmr_vm_mini.a` 内对象经检查是 `ELF 32-bit ... ARM`，不是 x86 产物，不能解决本目标。
- Android MINI 路径仍然显式 `mr_start_dsmC("cfunction.ext", ...)`，也就是说它没有消除 `cfunction.ext`；FULL 路径更多是 Android/ARM 原生运行方式的参考。
- `_mr_TestComC(800)` 仍是 `mr_load_c_function = (MR_LOAD_C_FUNCTION)(input + 8)` 后直接调用，这只适用于宿主 CPU 能直接执行 EXT 指令的场景；x86 下必须替换为 `arm_ext_executor`，不能照搬。
- `temp/jni` 包含 JNI、Android bitmap/log、`cacheflush`、`mprotect(PROT_EXEC)`、Android NDK CMake/Makefile 等平台专用代码，移植到 PC x86 时应抽象为 SDL/wasm/本项目现有平台层。

因此建议把 `temp/jni` 纳入重构参考清单：优先借鉴其 **库拆分方式、原生 runtime 入口顺序、平台 API 覆盖面、mr_table 字段对照**；避免直接复用其 **ARM 预编译库、直接函数指针执行 EXT、Android/JNI 专用层**。

## 3. 为什么不能只“嵌入 cfunction.ext”

把 `cfunction.ext` 转成 C 数组或资源文件只能消除文件读取，不能完成 x86 化：

- 内容仍是 ARM 指令，仍必须由 Unicorn 执行。
- 仍保留 `mr_table`、`r9/r10`、RWPI 等 ARM ABI 问题。
- 仍依赖特殊环境生成该 ARM 产物，只是把依赖从运行期挪到源码仓库。

该方式可作为短期兜底，不应作为本次“x86 形式完全整合”的最终方案。

## 4. 推荐目标架构

```text
当前：
  x86 host
    -> Unicorn 执行外层 cfunction.ext(ARM Mythroad/DSM)
       -> cfunction.ext 再加载/执行 MRP 内 EXT(ARM)

目标：
  x86 host
    -> native_mythroad.a / object library(x86 Mythroad/DSM)
       -> 直接调用 dsm_init/mr_start_dsm/mr_event/mr_timer
       -> 遇到 MRP 内 ARM EXT 时，交给 arm_ext_executor(Unicorn)
```

### 4.1 保留与删除

保留：

- Unicorn：仍用于执行 MRP 包内的 ARM EXT。
- `src/mythroad/`：作为原生 Mythroad/DSM 源码来源。
- 现有平台能力：文件、绘图、网络、声音、定时器等实现可继续复用。

删除/弱化：

- 运行时固定读取 `/cfunction.ext`。
- `wasm/dist/fs.js` 对 `/cfunction.ext` 的预加载。
- 外层 `bridge_ext_init()` 启动 `mr_c_function_load()` 的路径。
- 为生成外层 cfunction.ext 所需的斯凯 SDK/ARMCC 构建步骤。

## 5. 分阶段实施方案

### 阶段 0：加开关，保持可回退

新增构建选项：

```cmake
option(VMRP_USE_NATIVE_MYTHROAD "Use native x86 Mythroad instead of cfunction.ext" ON)
```

保留旧路径作为 `OFF` 模式，便于对照日志和回归。

### 阶段 1：抽象运行时入口

新增一个小接口，例如：

```c
// src/include/runtime.h
typedef struct VmrpRuntime VmrpRuntime;

int skyengine_runtime_init(VmrpRuntime *rt);
int skyengine_runtime_start_dsm(VmrpRuntime *rt, const char *mrp, const char *ext, const char *entry);
int skyengine_runtime_event(VmrpRuntime *rt, int32_t code, int32_t p0, int32_t p1);
int skyengine_runtime_timer(VmrpRuntime *rt);
void skyengine_runtime_destroy(VmrpRuntime *rt);
```

实现两个后端：

- `runtime_arm_cfunction.c`：包装当前 `loadCode()` + `bridge_ext_init()` 路径。
- `runtime_native_mythroad.c`：新 x86 原生路径，直接调用 `dsm_init()`、`mr_start_dsm()`、`mr_event()`、`mr_timer()`。

`src/skyengine.c` 只依赖该接口，不再散落 `cfunction.ext` 的细节。`temp/jni/src/emulator.c` 的 `native_1startMrp/native_1pause/native_1resume/native_1event/native_1callback` 可作为该 runtime 层入口顺序的参考。

### 阶段 2：原生化 DSM_REQUIRE_FUNCS

当前 `bridge.c` 的 `dsm_require_funcs_funcMap` 是给 ARM 侧看的：它把每个函数放在 Unicorn 内存中，通过 PC hook 拦截。

原生路径不需要 PC hook，应新增直接函数表：

```c
// src/platform/native_dsm_funcs.c
DSM_REQUIRE_FUNCS native_dsm_funcs = {
    .log = native_log,
    .mem_get = native_mem_get,
    .mem_free = native_mem_free,
    .timerStart = native_timerStart,
    .timerStop = native_timerStop,
    .getDatetime = native_getDatetime,
    .open = native_open,
    .close = native_close,
    .read = native_read,
    .write = native_write,
    .seek = native_seek,
    .drawBitmap = native_drawBitmap,
    // ...补齐与 dsm.h 顺序一致的函数
    .flags = FLAG_USE_UTF8_EDIT,
};
```

这些函数可以复用 `src/fileLib.c`、`src/network.c`、`src/memory.c`、GUI/SDL/wasm 现有实现，但参数是普通 x86 指针，不再使用 `toMrpMemAddr()` / `getMrpMemPtr()`。补齐函数语义时可对照 `temp/jni/src/dsm.c` 与 `temp/jni/src/cport.c`，但不要引入 JNI/Android 依赖。

### 阶段 3：把 `src/mythroad` 纳入顶层 CMake

建议先做 32-bit x86 目标：

- MRP ABI 是 32 位。
- `src/mythroad/include/type.h` 中 `uintptr_t`、`size_t` 被固定为 32 位。
- 代码中存在大量指针与 `uint32` 互转，64 位会截断。

建议编译参数：

```cmake
add_library(mythroad_native OBJECT
  src/mythroad/dsm.c
  src/mythroad/mythroad.c          # 或先用 mythroad_mini.c 做 MVP
  src/mythroad/encode.c
  src/mythroad/mr_unzip.c
  src/mythroad/mr_base64.c
  src/mythroad/mr_inflate.c
  src/mythroad/mr_graphics.c
  src/mythroad/string.c
  src/mythroad/printf.c
  src/mythroad/other.c
  src/mythroad/strtol.c
  src/mythroad/strtoul.c
  src/mythroad/md5.c
  src/mythroad/mem.c
)

target_compile_definitions(mythroad_native PRIVATE
  VMRP TARGET_MOD MTK_MOD MR_PLAT_DRAWTEXT DSM_FULL
)

target_compile_options(mythroad_native PRIVATE
  -m32
  -funsigned-char
  -Wall
)
```

说明：

- `-funsigned-char` 用于贴近 ARMCC 行为，README 已指出 char 符号性差异会引发 bug。
- 初期不要直接混改 `src/mythroad/` 主体；优先在外层 adapter 做兼容，必要变更用 `#ifdef VMRP_NATIVE` 隔离。
- `temp/jni/src/mr/CMakeLists.txt` 可作为拆分 `mr_vm_mini` / `mr_vm_full` 静态库的参考，但其 `mr_pre/*.a` 是 ARM 对象，不能链接进 x86 目标。
- 若宿主环境不能稳定提供 32-bit 依赖，再另开 64-bit 迁移任务，把所有指针承载类型改为 `uintptr_t`，同时修复 MRP 文件格式中的 32 位地址边界。

### 阶段 4：替换外层 cfunction.ext 启动

新路径伪代码：

```c
int runtime_native_mythroad_init(void) {
    DSM_REQUIRE_FUNCS funcs = native_dsm_funcs_init();
    int32 ret = dsm_init(&funcs);
    return ret == VMRP_VER ? MR_SUCCESS : MR_FAILED;
}

int runtime_native_mythroad_start(const char *filename, const char *ext, const char *entry) {
    return mr_start_dsm((char *)filename, (char *)ext, (char *)entry);
}

int runtime_native_mythroad_event(int32 code, int32 p0, int32 p1) {
    return mr_event((int16)code, p0, p1);
}

int runtime_native_mythroad_timer(void) {
    return mr_timer();
}
```

`startVmrp()` 在 native 模式下不再调用：

- `loadCode()`
- `bridge_ext_init()`
- 外层 `bridge_dsm_init()` / `bridge_dsm_mr_start_dsm()`

而是调用 runtime 接口。

### 阶段 5：处理 MRP 内 ARM EXT

这是重构成败关键。

原 `src/mythroad/mythroad.c` / `mythroad_mini.c` 中 `_mr_TestComC(800/801)` 逻辑假定当前代码也是 ARM：

- `800`：`mr_load_c_function = (MR_LOAD_C_FUNCTION)(input1 + 8)`，然后直接调用。
- `801`：调用 `mr_c_function(...)` 分发事件。

x86 原生后不能直接调用 `input1 + 8`，因为 `input1` 是 MRP 包中的 ARM EXT 字节。应增加一个 EXT 执行器接口：

```c
typedef struct ArmExtModule ArmExtModule;

int arm_ext_load(ArmExtModule **out, const uint8_t *code, uint32_t len, int32_t load_code);
int arm_ext_call(ArmExtModule *m, int32_t code, const void *input, uint32_t input_len,
                 uint8_t **output, int32_t *output_len);
void arm_ext_unload(ArmExtModule *m);
```

在 `VMRP_NATIVE` 下，把 `_mr_TestComC(800/801)` 改成调用该接口：

- `800`：创建 Unicorn 实例或模块上下文，映射 EXT 代码，写入 `mr_table`，执行 `mr_c_function_load()`。
- `801`：把 x86 输入结构复制到 ARM 内存，设置 ARM 寄存器，执行 helper，再把输出复制回 x86。
- 每个已加载 EXT 保存自己的 `mr_c_function_st`、RW 段、helper 地址、ext handle，支持插件化 EXT 嵌套。

可以复用当前 `bridge.c` 的 ARM hook 思路，但需要把“外层 cfunction.ext 的 mr_table”拆成“用户 EXT 的 mr_table”。当前 `mr_table_funcMap` 中不少函数尚为 `NULL`，原生化后应按 MRP 应用需要逐步补齐，至少覆盖 `mrc/asm/asm.mrp`、`dsm_gm.mrp` 和样例游戏会触发的接口。`temp/jni/src/mythroad/mythroad.c` 的 `_mr_c_function_table` 与 `_mr_TestComC(800/801)` 可作为字段顺序和事件序列参考，但直接调用 `input+8` 的做法必须替换。

### 阶段 6：清理资源与发布文件

native 模式验证通过后：

- 从 `wasm/dist/fs.js` 删除 `/cfunction.ext` 预加载项。
- 运行包不再要求 `bin/cfunction.ext` 或 `wasm/dist/fs/cfunction.ext`。
- README 中删除“单独编译 skyengine.mrp 并提取 cfunction.ext”作为必需步骤，改为历史/回退说明。
- CI/本地构建增加 native 模式检查。

## 6. 关键风险与处理

| 风险 | 影响 | 处理 |
| --- | --- | --- |
| x86_64 指针截断 | 大量 `uint32` 保存指针会崩溃 | 第一阶段强制 32-bit x86；64-bit 另立迁移任务 |
| `char` 符号性差异 | 文本、资源解析、颜色/编码逻辑异常 | 默认 `-funsigned-char`，并为关键路径加回归样例 |
| 原 `fixR9`/`asm/r9r10.s` 是 ARM 汇编 | x86 无法编译 | native Mythroad 不应使用 r9/r10 切换；在 `VMRP_NATIVE` 下替换为无操作或 C 层上下文切换 |
| 用户 MRP EXT 仍是 ARM | x86 不能直接调用 `input1+8` | 必须实现 `arm_ext_executor`，不能跳过 |
| bridge 函数表未完全实现 | 某些游戏运行到未实现 API 会退出 | 用样例和真实 MRP 逐步补齐，未实现函数返回 `MR_IGNORE/MR_FAILED` 并记录日志 |
| wasm 目标不是 x86 | wasm 不能用 `-m32` | wasm 可走“原生 C 编译到 wasm + ARM EXT 仍 Unicorn/wasm”的同样架构；先以 PC x86 为第一目标 |

## 7. 验收标准

完成重构时应满足：

1. 默认构建不需要外部 `cfunction.ext` 文件即可启动。
2. 顶层构建系统能编译 `src/mythroad` 原生目标。
3. `startVmrp()` 或等价入口通过 native runtime 调用 `dsm_init()` 并得到 `VMRP_VER`。
4. `dsm_gm.mrp` 能启动到原有入口流程。
5. 至少一个包含 ARM EXT 的样例，例如 `mrc/asm/asm.mrp`，能通过 `arm_ext_executor` 进入 `mrc_init()`。
6. wasm/发布资源列表不再包含 `/cfunction.ext`，旧模式只作为显式回退开关存在。
7. README/构建文档更新，说明不再需要斯凯 SDK 生成外层 cfunction.ext。

建议的回归命令/检查：

```bash
# 不应再出现强制读取外层 cfunction.ext 的默认路径
grep -RIn "cfunction.ext" src wasm CMakeLists.txt README.md

# x86 native 模式构建
cmake -S . -B build-native -DVMRP_USE_NATIVE_MYTHROAD=ON
cmake --build build-native

# 回退模式仍可构建
cmake -S . -B build-armext -DVMRP_USE_NATIVE_MYTHROAD=OFF
cmake --build build-armext
```

## 8. 建议落地顺序

1. 先抽 `VmrpRuntime` 接口，不改变行为。
2. 加 `runtime_native_mythroad.c`，只做到 `dsm_init()` 返回 `VMRP_VER`。
3. 对照 `temp/jni/src/emulator.c` 接入 `dsm_init()`、`mr_start_dsm()`、`mr_event()`、`mr_timer()` 等 native runtime 入口，让纯 MR/Lua 或不触发 C EXT 的路径先跑通。
4. 实现 `arm_ext_executor`，替换 `_mr_TestComC(800/801)`。
5. 用 `mrc/asm/asm.mrp` 和真实 `dsm_gm.mrp` 做对照测试。
6. 删除默认发布资源中的 `/cfunction.ext`。
7. 最后清理 README 与旧构建说明。

