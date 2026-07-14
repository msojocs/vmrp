# 调试与验证

## 1. 先保存可复现条件

ARM EXT 问题通常与二进制版本、工作目录和输入时序有关。开始修改前至少记录：

- MRP 文件及校验值；
- 实际加载的 wrapper/game/child EXT；
- `--screen`、`--memory`、`--work-dir` 和 DNS 映射；
- 字体、插件、下载包和存档文件；
- 自动点击或 E2E 按键顺序及 delay/hold；
- 当前 stdout/stderr；
- 失败前最后一张 PPM；
- 是否有网络或本地代理服务。

不要只保存“某页面黑屏”这一现象。

## 2. 诊断环境变量

| 变量 | 作用 | 日志量 |
| --- | --- | --- |
| `VMRP_ARM_EXT_DIAG=1` | 输出调用层、owner、关键 table、timer 和文件诊断 | 高 |
| `VMRP_ARM_EXT_TRACE=1` | 输出 table 调用和主要执行器状态 | 很高 |
| `VMRP_ARM_EXT_TRACE_PC=1` | 为 wrapper 安装逐指令 hook，滚动保留最近 64 个 PC/CPSR | 运行开销高；通常只在异常时输出 ring |
| `VMRP_ARM_EXT_WATCH_PC=offsets` | 对每个已注册 nested module 安装指定相对偏移观察点 | 低 |
| `VMRP_ARM_EXT_TIMER_LIVENESS_DIAG=1` | 展开 wrapper/primary timer 节点 | 中 |
| `VMRP_ARM_EXT_TIMER_OWNER_DIAG=1` | 跟踪 timer owner 选择和变更 | 中 |
| `VMRP_ARM_EXT_SCREEN_DIAG=1` | 跟踪屏幕 owner、damage 和 present | 中到高 |
| `VMRP_ARM_EXT_INVARIANTS=1` | 在状态边界检查结构不变量 | 低，失败时高价值 |

上述布尔诊断开关在进程内缓存，修改后需要重新启动模拟器。`VMRP_ARM_EXT_WATCH_PC` 在每次 nested module 注册时读取，但为了让 wrapper 加载、primary 注册和后续 child 使用同一组条件，实际排障仍应在启动进程前设置。

## 3. 推荐启用顺序

不要一开始就打开逐指令 PC ring hook。

1. `VMRP_ARM_EXT_INVARIANTS=1`：先排除结构已经损坏。
2. `VMRP_ARM_EXT_DIAG=1`：观察 active/primary/timer/screen owner。
3. 根据问题追加 timer 或 screen 专项诊断。
4. 从反汇编得到候选偏移后使用 `VMRP_ARM_EXT_WATCH_PC`。
5. 只有在控制流完全未知、需要异常前最后 64 个 wrapper PC 时才使用 `VMRP_ARM_EXT_TRACE_PC`。

示例：

```bash
VMRP_ARM_EXT_INVARIANTS=1 \
VMRP_ARM_EXT_DIAG=1 \
./build/vmrp --work-dir . test/fixtures/example.mrp
```

## 4. PC 观察点

`VMRP_ARM_EXT_WATCH_PC` 接受逗号分隔的十六进制模块文件偏移：

```bash
VMRP_ARM_EXT_WATCH_PC="366c,1edf8,0x2370" \
./build/vmrp --work-dir . /path/to/app.mrp
```

注册 primary/child nested module 时，执行器把每个偏移转换为 `module_base + offset` 并安装单地址 hook。当前 wrapper 本身不通过 nested module 注册路径安装这些 watch；wrapper 的异常前 PC ring 需要使用 `VMRP_ARM_EXT_TRACE_PC`，更早或更长的控制流则需要在调试代码中增加有边界的专用 hook。命中日志包含：

- module base；
- relative offset；
- PC；
- R0-R3；
- LR。

注意：同一偏移会应用到所有新注册模块。偏移 0 或超出模块长度的值会被忽略。

## 5. 崩溃输出

无效内存 hook 会输出：

- 访问类型、地址、长度和值；
- crash PC 和 ARM/Thumb 状态；
- 最近文件映像区间；
- PC 前 32 字节和当前位置 16 字节；
- R0-R12、SP、LR、PC、CPSR；
- SP 向上 64 字节；
- DIAG 开启时的层状态和 R9 timer 区。

同时尝试把 PC 前后各 256 字节保存为：

```text
/tmp/vmrp_crash.bin
```

日志会给出带 `--adjust-vma` 的 objdump 命令。需要根据 CPSR/地址 bit0 决定 ARM 还是 Thumb 反汇编。

## 6. wrapper probe 摘要

每次 `arm_ext_load()` 都输出一行 wrapper 探测结果：

```text
arm_ext_executor: wrapper probes len=... timer_dispatch=... compact_sched_off=...
                  compact_heap_ctrl=... compact_free_ret=... chain_walker=...
```

值为 0 表示对应模式没有识别到。新 SDK 在 timer、compact heap 或 chain walker 上失配时，先确认 probe 是否为 0，再分析二进制模式，不要直接修改运行时状态机掩盖探测失败。

## 7. 常见问题定位

### 7.1 启动即 invalid fetch/read/write

检查：

- crash PC 属于 wrapper 还是 nested module；
- R9 是否落在对应 P 的 RW；
- LR 是否是 Thumb 地址；
- code 区是否刚被文件读取或 dump 覆盖；
- Unicorn TB cache 是否失效；
- P/extChunk/module record 是否互相一致。

### 7.2 进入插件后事件失效

检查：

- code=1 是否仍先经过 wrapper；
- active P/helper 是否为可见 child；
- wrapper 前台分发区是否在 child 关闭后恢复；
- extChunk helper 槽是否被清零；
- 返回键按下/释放是否成对。

### 7.3 页面停住但不崩溃

启用 timer owner 和 liveness 诊断，检查：

- host timer 是否仍 pending；
- timer owner 是 wrapper、primary 还是 child；
- queue/current 节点是否结构有效；
- callback 是否属于已注册 code；
- wrapper dispatch busy 时是否错误直达 child；
- child 已关闭但旧 timer owner 是否仍存活。

### 7.4 黑屏、旧画面或前后台互相覆盖

启用 screen diag，并检查 PPM：

- guest backing screen 是否有像素；
- present 是否被 owner 规则接受；
- primary 是否在前台 child 存在时穿透提交；
- child cover region 是否完整；
- full-screen black clear 是缓存清理还是显式 present；
- 模态快照是否在正确关闭边界恢复；
- `mr_screen_w/h` 与实际 stride 是否一致。

### 7.5 child 找不到字体、图片或 sibling 文件

检查 table[125] DIAG：

- LR 识别到的 owner；
- table[100] 包名和 host alias；
- table[104]/[105] RAM 包；
- child `package_host_path`；
- readFile 返回地址是否直接位于 RAM package；
- 工作目录下插件/字体是否存在。

### 7.6 下载/恢复后执行旧代码

检查：

- table[44] 是否覆盖注册模块；
- `uc_ctl_remove_cache()` 范围是否正确；
- GOT snapshot 是否只在可执行 restore 上应用；
- staging 地址是否被旧 nested module 继续占有；
- private loader 已构造 runtime image 时是否又被 raw file 覆盖。

## 8. PPM 验证

桌面程序可通过 `VMRP_PPM_PATH` 指定截图路径。设置 `VMRP_PPM=1` 后，每次有效提交会更新最新 PPM：

```bash
VMRP_PPM=1 \
VMRP_PPM_PATH=/tmp/skyengine-screen.ppm \
./build/vmrp --work-dir . /path/to/app.mrp
```

也可以向进程发送 `SIGUSR1` 获取当前窗口 surface。自动化测试优先使用 E2E socket 的 `SCREEN` 命令，因为它在 SDL 主线程执行并返回明确完成状态。

画面验证至少包含：

- PPM 头部尺寸正确；
- 不是全黑或单色；
- 关键坐标 RGB 与预期一致；
- 前台 child 打开和关闭前后的区域正确；
- 非默认屏幕尺寸下没有裁切或越界。

## 9. E2E 验证

构建后运行相关 Vitest：

```bash
cmake --build build --target vmrp --parallel
pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts
pnpm vitest run test/e2e/gghjt/game-start.test.ts
pnpm vitest run test/e2e/dota/download-plugin.test.ts
```

保留失败现场：

```bash
VMRP_E2E_KEEP_TMP=1 \
VMRP_ARM_EXT_INVARIANTS=1 \
pnpm vitest run test/e2e/<case>.test.ts --reporter verbose
```

临时目录包含 stdout、stderr、socket 和 PPM。不要让测试直接写仓库根 `mythroad/`。

## 10. ARM EXT smoke

`VMRP_ARM_EXT_SMOKE` 可以绕过正常 MRP 启动流程，直接从指定 MRP 提取示例 EXT 并验证入口：

```bash
VMRP_ARM_EXT_SMOKE=mrc/asm/asm.mrp ./build/vmrp --work-dir .
```

它适合验证执行器基础加载和 `mrc_init()`，不能替代包含 nested module、timer、文件、网络和图形流程的真实应用测试。

## 11. Sanitizer

```bash
cmake -S . -B build-san -DSANITIZE=address,undefined
cmake --build build-san --target vmrp --parallel
```

Sanitizer 主要发现 host 侧越界、use-after-free 和整数问题。guest 内的非法地址由 Unicorn hook 捕获，两者需要结合使用。

## 12. 完成标准

一次 ARM EXT 修复至少应满足：

1. `cmake --build build --target vmrp` 通过。
2. `git diff --check` 通过。
3. 相关真实 MRP 用例通过。
4. `VMRP_ARM_EXT_INVARIANTS=1` 不产生新失败。
5. 最终 PPM 或像素断言证明画面正确。
6. 没有依赖包名、应用名或固定用户路径的新分支。
7. 新增探测有结构验证和失配日志。
8. 文档记录新增 ABI 事实、未知项和验证命令。
