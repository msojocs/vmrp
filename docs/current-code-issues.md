# 当前模拟器代码问题分析

日期：2026-06-07

## 范围与验证状态

本次分析覆盖主程序入口、共享库 API、文件系统、网络、DSM/Mythroad 桥接、ARM EXT 执行器、构建配置和现有测试。未把 `third_party/unicorn/`、`mrc/` 研究资料、历史构建目录作为主问题来源。

已执行验证：

- `cmake --build build`：通过，Ninja 报告 `no work to do`
- `ctest --test-dir build --output-on-failure`：通过，`1/1 vmrp_unit_tests`

测试结论：现有单测只覆盖 GOT 快照模拟和 Lua parser，且 `test/stubs.c` 用桩替代了 ARM EXT、SDL、文件系统、网络等路径。测试通过不能证明主模拟器运行路径安全。

## 问题清单

### 1. SDL/FFI 画面刷新接口假设输入永远是整屏缓冲

严重程度：高

证据：

- `src/main.c:137-165` 的 `guiDrawBitmap()` 按 `bmp[xx + yy * vmrp_config.screen_width]` 读取源像素。
- `src/vmrp_api.c:19-32` 的 FFI 版本使用同样的整屏索引。
- `src/mythroad/dsm.c:641-643` 把 Mythroad 的 `mr_drawBitmap(bmp, x, y, w, h)` 直接转给平台层。
- `src/mythroad/mythroad.c:1672-1675` 的 `_DispUpEx()` 传入整屏 `mr_screenBuf`，这一路能成立。
- `src/mythroad/mythroad.c:1690-1698` 的 `MRF_DispUp()` 可传入 `mr_bitmap[i].p + ...`，不是整屏基址。
- `src/mythroad/include/mrporting.h:421-423` 的抽象接口语义是“在屏幕上绘 BMP”，不是“只刷新整屏缓冲”。

推断：

当前平台层只正确处理“源指针是整屏 framebuffer 且 stride 等于屏幕宽度”的调用。Lua bitmap、sprite、tile 或局部 bitmap 直接走 `mr_drawBitmap()` 时，会读错源偏移，严重时越界读。

影响：

非整屏绘制可能出现花屏、局部区域错位、读取未初始化内存或崩溃。共享库 API 与 SDL 主程序都有同样风险。

### 2. 共享库生命周期没有释放运行时资源

严重程度：高

证据：

- `src/vmrp_api.c:119-129` 的 `vmrp_api_destroy()` 只释放 `screen_buf` 和编辑文本。
- `src/vmrp.c:360-397` 的 `startVmrp()` 会调用 `vmrp_runtime_init()`，成功路径不调用 `vmrp_runtime_destroy()`。
- `src/runtime_native_mythroad.c:39-42` 的 `vmrp_runtime_destroy()` 才会调用 `native_dsm_funcs_destroy()`。
- `src/native_dsm_funcs.c:828-851` 的 `native_dsm_funcs_destroy()` 负责关闭音频设备并释放 `native_mem_base`。
- `src/native_dsm_funcs.c:124-145` 会按需分配 4MB DSM 内存。

推断：

FFI 宿主调用 `vmrp_api_init()` / `vmrp_api_start()` / `vmrp_api_destroy()` 后，DSM 内存、音频设备和内部 Mythroad 状态不会被完整释放。重复启动不同 MRP 时，状态泄漏和资源泄漏风险较高。

影响：

Flutter/Android 等长生命周期宿主可能出现内存增长、音频设备占用、旧运行状态污染新应用的问题。

### 3. 网络异步路径存在空指针、悬挂指针和线程资源泄漏

严重程度：高

证据：

- `src/network.c:155-179` 的 `my_connect()` 搜索 socket map 后直接解引用 `obj->data`，没有检查 `obj == NULL`。
- `src/network.c:191-196`、`src/network.c:460-489`、`src/network.c:556-593` 也直接解引用 socket 查找结果。
- `src/network.c:167-177` 创建异步 connect 线程，线程持有 `mSocket *data`。
- `src/network.c:235-249` 的 `my_closeSocket()` 删除 map 后立即 `free(data)`。
- `src/network.c:135-145` 的 connect 线程完成后写 `data->s->realState` / `data->s->state`。
- `src/network.c:171`、`src/network.c:333`、`src/network.c:423` 创建 pthread 后没有 detach 或 join。

推断：

无效 socket 句柄会直接崩溃；异步连接期间关闭 socket 会导致后台线程写已释放内存；大量异步 DNS/连接请求会泄漏线程资源。

影响：

网络 MRP 或回归脚本一旦触发异常关闭、重复打开网络或错误句柄，可能出现随机崩溃、状态污染和资源耗尽。

### 4. 原生 DSM 网络回调链路没有真正接通

严重程度：高

证据：

- `src/native_dsm_funcs.c:174-176` 的 `native_initNetwork()` 忽略传入 `cb` 和 `userData`，固定调用 `my_initNetwork(NULL, NULL, mode, NULL)`。
- `src/native_dsm_funcs.c:178-183` 的 `native_getHostByName()` 固定返回 `MR_FAILED`。
- `src/arm_ext_executor.c:1353-1359` 的 ARM table 也把 `mr_getHostByName` 固定为 `MR_FAILED`。
- `src/mythroad/src/lib/mr_socket_target.c:132-172` 期待 `mythroad_initNetwork()` 返回 `MR_SUCCESS`、`MR_FAILED` 或 `MR_WAITING`，并在等待态通过回调更新 socket 状态。

推断：

当前网络支持只覆盖部分同步 host socket 行为，Mythroad 层要求的异步初始化和 DNS 回调没有完整模拟。依赖 `getHostByName` 或 initNetwork 回调的 MRP 可能走错误分支。

影响：

联网 MRP、支付、下载、NTP 或服务器更新流程可能卡住、误判网络不可用，或只在少数硬编码/CMWAP 场景下可用。

### 5. 文件读写工具函数对失败路径处理不完整

严重程度：中

证据：

- `src/fileLib.c:223-236` 的 `vmrp_writeFile()` 不检查 `my_open()` 返回 0，就继续 `my_write()` / `my_close()`。
- `src/fileLib.c:238-260` 的 `readFile()` 在 `my_getLen()` 返回 `-1` 时把负数传给 `malloc(len)`。
- `src/fileLib.c:238-260` 的 `readFile()` 如果 `my_open()` 失败，会直接返回 `NULL`，没有释放已经分配的 `p`。
- `src/utils.c:71-76` 的 `printScreen()` 同样不检查 `my_open()` / `my_write()` / `my_close()` 结果。

推断：

缺失文件、权限错误、空文件、超大文件或磁盘错误会触发大额分配尝试、内存泄漏或静默写失败。

影响：

调试 dump、截图、MRP 文件读取失败时难以定位问题，极端情况下会引发内存压力或崩溃。

### 6. 屏幕尺寸输入缺少统一校验，存在溢出和配置覆盖

严重程度：中

证据：

- `src/vmrp.c:149-157` 的 `--screen` 路径会校验正数。
- `src/vmrp.c:184-187` 的 `VMRP_SCREEN_WIDTH` / `VMRP_SCREEN_HEIGHT` 使用 `atoi()`，不校验正数、上限或溢出。
- `src/vmrp_api.c:78-86` 的 `vmrp_api_init()` 对非正数回退默认值，但没有限制过大值，且 `(screen_w * screen_h)` 先以 `int` 相乘再转 `size_t`。
- `src/vmrp_api.c:92-93` 在 API init 中调用 `prepareVmrpArgs(1, fake_argv)`，环境变量可以覆盖调用者传入的分辨率。
- `docs/flutter-integration.md:728` 写明分辨率通过 `vmrp_api_init(width, height)` 设置。

推断：

CLI 环境变量可把屏幕设置成负数、0 或极大值；FFI 宿主传入的分辨率也可能被环境变量覆盖。大尺寸会造成整数溢出和分配尺寸错误。

影响：

SDL 窗口创建失败、screen buffer 分配错误、越界写和宿主 API 行为不符合文档。

### 7. ARM EXT 执行器包含样本专用补丁和异常吞掉逻辑

严重程度：中

证据：

- `src/arm_ext_executor.c:330-463` 的 `run_arm_with_sp()` 对多种 `UC_ERR_INSN_INVALID` / `UC_ERR_EXCEPTION` 情况直接写 `PC = EXT_STOP_ADDR` 并返回 `MR_SUCCESS`。
- `src/arm_ext_executor.c:1691-1733` 的 `patch_gxdzc_game_touch_map()` 通过包名和硬编码偏移修改 `gxdzc` 的触摸映射。
- `src/arm_ext_executor.c:2020-2079`、`src/arm_ext_executor.c:2184-2225`、`src/arm_ext_executor.c:2283-2288` 包含 gxdzc 模态框截图/取消尾状态处理。
- `docs/gghjt-jit-cache-fix.md:136-144` 明确复盘过“不要乱写 recovery”，但当前执行器仍保留多层异常恢复逻辑。

推断：

执行器已经从通用 ARM EXT 模拟器演化出多个样本专用兼容分支。部分异常会被当成正常返回，可能掩盖真实崩溃，也可能让新样本进入不可解释状态。

影响：

新增 MRP 样本时，行为可能依赖历史样本 workaround；排查崩溃时日志与返回值不再可靠区分“正常退出”和“被模拟器吞掉的异常”。

### 8. ARM 内存和 host 指针等值假设不稳固

严重程度：中

证据：

- `src/arm_ext_executor.c:1872-1889` 先尝试把 `m->mem` mmap 到固定 `EXT_BASE_ADDR`，失败后退回 `calloc()`。
- 同一段注释说明目标是“ARM 虚拟地址等于宿主指针”。
- `src/arm_ext_executor.c:251-259` 的 `arm_ptr()` / `arm_addr()` 以 `m->mem + (addr - EXT_BASE_ADDR)` 做转换。
- `src/arm_ext_executor.c:753-766` 在进入 EXT 时把全局 `mr_screenBuf` 指向 `arm_ptr(m, m->screen_addr)`，退出后再 memcpy 回 host buffer。

推断：

大部分显式转换是安全的，但注释中的“虚拟地址等于宿主指针”只在固定 mmap 成功时成立。若部分 Mythroad/EXT 代码把 32-bit ARM 地址当 host 指针直接使用，calloc fallback 会破坏这个假设。

影响：

在地址空间被占用、Windows fallback 或其他平台上，可能出现只在特定机器复现的指针错误。

### 9. 测试覆盖没有命中主要风险路径

严重程度：中

证据：

- `CMakeLists.txt:294-303` 的 `test_vmrp` 只编译 test 文件、`test/stubs.c` 和 Mythroad 源码。
- `test/stubs.c:325-353` 把 `arm_ext_executor` 全部替换为返回失败的桩。
- `test/stubs.c:372-407` 把文件系统和网络函数替换为桩。
- `test/test_got_snapshot.c:160-197` 复制了一份 GOT 逻辑做模拟测试，不直接调用 `src/arm_ext_executor.c`。

推断：

现有单测无法发现 SDL/FFI 绘制、共享库生命周期、真实文件读写、真实网络、Unicorn hook、timer dispatch 等问题。

影响：

回归主要依赖手工样本脚本和运行日志，修复一个样本时容易让另一个样本退化。

### 10. 内存分配失败路径普遍缺少保护

严重程度：低到中

证据：

- `src/main.c:105-108` 的 `saveEditText()` 调用 `my_mallocExt()` 后直接 `memcpy()`。
- `src/vmrp_api.c:182-185` 的 `vmrp_api_set_edit_text()` 也没有检查 `my_mallocExt()` 返回值。
- `src/fileLib.c:49-52`、`src/fileLib.c:186-189`、`src/network.c:218-228` 多处 `malloc()` 后直接写字段。
- `src/native_platform_memory.c:3-7` 的 `my_mallocExt()` 本身会在失败时返回 `NULL`。

推断：

内存压力下多个入口会从可恢复失败变成空指针崩溃。

影响：

在移动端共享库、Wasm 或长时间运行场景中，低内存行为不可预测。

## 建议优先级

1. 先处理高严重度问题：绘制接口 stride/源指针语义、共享库 destroy、网络句柄和线程生命周期、DSM 网络回调。
2. 再补测试：至少增加文件失败路径、API init/destroy、无效 socket、局部 bitmap 绘制、屏幕尺寸校验的单测或可脚本化回归。
3. 对 `arm_ext_executor.c` 的样本专用兼容逻辑做台账：每个 workaround 绑定样本、触发条件、验证脚本和退出条件，避免继续扩散成不可控的隐式策略。

## 未确认项

- 未运行真实 MRP 回归脚本，因此没有给出每个问题对应的样本复现结果。
- 未在 Windows、Wasm、Android/Flutter 宿主上验证资源泄漏和固定地址 mmap fallback。
- 未审计 `third_party/unicorn/` 和 `mrc/` 研究代码。
