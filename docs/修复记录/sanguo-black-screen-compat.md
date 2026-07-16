# 490111 三国黑屏兼容性与历史回归审计

日期：2026-07-16

## 1. 审计范围

本文只审计测试基础设施、相关真实 MRP 覆盖面和 Git 历史，不修改生产逻辑。
目标命令为：

```bash
build/vmrp build/mythroad/490111_240x320_sanguo.mrp
```

所有历史运行都使用 `SDL_VIDEODRIVER=dummy` 或 SDL 自带的
`offscreen` 驱动，没有使用 Xvfb。历史提交通过 detached `/tmp`
worktree 构建，未切换或改写主工作树。

直接根因的反汇编和运行时证据分别见：

- `docs/修复记录/sanguo-black-screen-disassembly.md`
- `docs/修复记录/sanguo-black-screen-runtime.md`

## 2. 目标包与现有 fixture 的关系

目标 MRP SHA-256 为
`bcbac32ad7567e1112f0c15db6c8aae9752bfe4faf2f2492f5665e7332c51702`，
包头声明 240x320。关键解压资源为：

| 资源 | 解压长度 | 特征 |
| --- | ---: | --- |
| `start.mr` | 2490 | Lua bytecode `version=0x80` |
| `cfunction.ext` | 19428 | direct compact wrapper |
| `game.ext` | 164716 | 主游戏 private EXT |
| `plugins.lst` | 224 | 4 个插件记录 |

`start.mr` 的根 proto 先加载 `cfunction.ext`，再依次发送 code 6 和
code 0；`dealevent/dealtimer/suspend/resume` 转发 code 1/2/4/5。
因此目标同时覆盖 Lua FULL 启动、wrapper、private loader、嵌套 EXT、
LG_mem allocator 和 timer 路由，不能用单独的 ARM EXT smoke load 代替。

目标 wrapper 的离线探测结果为：

```text
len=19428
timer_dispatch file_off=16708
compact scheduler offset=0x20c
compact heap control offset=0x15c
compact free return file_off=11016
chain walker=none
```

现有 fixture 没有相同 SHA-256 或相同布局。最相关的覆盖组是：

| 组 | fixture | 作用 |
| --- | --- | --- |
| 老 direct compact | `gxdzc`, `gzwdzjs`, `gtcm`, `dota` | 相同 16984-byte wrapper；scheduler `0x1fc`、heap control `0x14c` |
| 其它 direct compact | `gfhcq`, `talkcat`, `opglqa`, `wbrw` | 覆盖不同 scheduler/control 偏移；`opglqa` 的 19872-byte 变体最接近目标大小 |
| chain wrapper | `gghjt`, `gwkdl` | 同为 19428 bytes 但走 chain walker；保护另一套 R9/timer 路由 |

结论：现有测试能保护相邻 ABI，但不能证明这个目标包已修复，最终必须有
目标包自己的有界启动和 PPM 验证。

## 3. 当前黑屏的直接证据边界

当前基线在同步 code-0 内成功加载了 `game.ext`：

```text
file=0x226318 len=164716
P=0x2bc5dc
helper=0x238f8d
RW=0x24e68c len=16500
```

它随后读取 `plugins.lst` 并进入 wrapper 的动态 arena setup/teardown，
主线程持续占用约 100% 单核，Lua `dofile` 和 SDL 主循环尚未返回。基线
PPM 是合法 P6 240x320，但 230400 个 RGB payload 字节全部为 0。

反汇编/GDB 已证明直接根因是 LG_mem 共享 ABI 边界分裂：guest wrapper
通过 `table[108]`、`table[110]` 和 `table[146]` 把外部 arena
`0x40018800..0x4001cda0` 临时接入 offset free-list；旧 host
table[0]/[1] 仍按初始化时的固定 end `0x325e5c` 遍历，误判动态节点后
改走 bump 分配，teardown 最终形成四节点环。

因此直接修复约束是：table[0]/[1]/[2] 每次调用都从 ARM 可见 slot
读取当前 base/end/head，并逐节点验证映射；不能禁用 R9 同步、禁用 free、
清链、超时跳出或按包名分支。这一点与下面的历史触发边界要严格区分。

### 当前动态 arena 修复的阶段性证据

当前共享工作树的修复保留 R9 sync，并让 host allocator 动态读取
`table[108]/[110]` 指向的 base/end slot。无 Xvfb 的 SDL dummy 运行使用
`VMRP_PPM=1` 和 20 秒硬超时，产物为：

```text
/tmp/sanguo-dynamic-arena.ppm
P6 240x320
sha256=c4ca6f078322e28d3969479da7b437f55458d6843eea96d9f188b7dd2e3db0e5
colors=26
nonblack=17849/76800
```

人工检查画面已经到达游戏的“连接超时”提示。这同时证明 code-0、绘制和
网络初始化均已越过原活锁；提示本身来自外部服务连接失败，不能把它和旧版
全黑 PPM 混为一谈。现阶段 focused e2e 结果为：

```text
talkcat/game-prepare + gtcm/boot-to-home + gxdzc/gxdzc-pixel
3 files, 6 tests passed
```

随后的最终验收使用同一 allocator 实现完成了目标重跑和默认
19-file/32-test 全量 e2e，结果见第 7 节。

## 4. 历史二分：R9 同步是触发边界，不是直接根因

以“同步 code-0 是否返回”为判据，在 `/tmp/vmrp-sanguo-history` 中构建
和运行关键提交，结果如下：

| 提交/实验 | code-0 | game/plugin 加载 | PPM |
| --- | --- | --- | --- |
| `017723c` (2026-05-27) | 返回 | 日志到 `After app init` | 未作为画面成功证据 |
| `fb72a57` (2026-06-01) | 返回 | 日志到 `After app init` | 未作为画面成功证据；随后命中当时的旧退出启发式 |
| `b736700` | 返回 | 已加载主 EXT/插件 | 未验证正常画面 |
| `355b4da` | 返回 | 见下方精确日志 | **全黑** |
| `92332ce` | **不返回** | 已加载 `game.ext`，在初始化内活锁 | 未到正常画面 |
| `92332ce`，只移除自动 R9 sync | 返回 | 与 `355b4da` 相同级别 | **全黑** |
| `5e85a43`, `905011e`, `0d47e9a`, 当前基线 | 不返回 | 已加载主 EXT | 全黑或没有新 PPM |

`355b4da` 和 `92332ce + no-auto-R9` 的日志都包含：

```text
_mr_readFile reallen:164716
mr_cacheSync(..., 164800)
_mr_readFile reallen:224
_mr_readFile reallen:4532
_mr_readFile reallen:20770
_mr_readFile reallen:14314
[startVmrp] vmrp_runtime_start_dsm returned 0x0
```

这证明它们不是在 Lua、MRP 索引或 `game.ext` 加载前提前返回。两次
`/tmp/vmrp_screen.ppm` 都是 P6 240x320，SHA-256 均为
`1edcc2a5a6f7f596acf98d8fbd248c4175886dc8759e6d75ec652b339c2a53b6`，
且 230400 个像素字节全部为 0。故“code-0 返回”绝不等于“游戏正常启动”。

最后好提交 `355b4da` 的直接后继、首坏提交是：

```text
92332ce23c8f1e853c202376bae131e1cd7cd9b7
perf: 移除硬编码代码
```

该提交引入 `arm_ext_sync_r9_for_code_addr()`，在覆盖全部 EXT memory 的
`UC_HOOK_BLOCK` 上按 PC/module registry 自动切换 R9。在保留该提交其余
所有模块注册、timer probe、文件行为和全范围 block hook 的情况下，只删除
`hook_restore_r9()` 末尾的自动 R9 sync，code-0 即恢复返回；保留空的全范围
block hook 也仍返回，所以不是 hook 性能开销。

这项实验只证明：自动 R9 归属纠正使旧动态 arena 协议的 host/guest
allocator 分歧稳定暴露为同步 teardown 活锁。它没有证明 R9 sync 本身错误，
也没有证明去掉它能得到正常画面。当前修复应保留经过模块归属证明的 R9
同步，并修正 `table[108]/[110]/[146]` 所属的真实共享 allocator 状态。

## 5. 测试基础设施审计

### Vitest e2e

`VmrpE2e` 用 `VMRP_E2E_SOCKET` 启动 `build/vmrp`，自动设置 SDL video/audio
dummy driver，并把 `SCREEN` 输出直接解析为 PPM。`VmrpWorkspace.create()`
为每个 `it()` 复制独立 `mythroad/`，避免插件、缓存和存档并发污染。

当前默认集合由 `vitest.config.ts` 选取，共 19 个文件、32 个测试；
`**/temp.test.ts` 默认排除。`pnpm exec vitest list test/e2e` 可无运行列出
准确集合。全量命令是：

```bash
pnpm exec vitest run test/e2e --reporter=verbose
```

`test/e2e/gfhcq/temp.test.ts` 是 480x800 screen damage 性能回归，默认被排除，
若修改触及 `aex_screen` 或 damage tracking，必须另行显式执行。

### CTest 残留陷阱

当前源码已经没有可构建的 `vmrp-unit`：`9e7bcd8` 删除
`test/unit/test_arm_ext.c`、`unit_stubs.c` 和 wrapper assets，`f4b6c11`
删除 CMake 的 test target/`add_test`。但既有 `build/CTestTestfile.cmake` 和
`build/vmrp-unit` 仍可能残留，使 `ctest -N` 显示一个测试、`ctest` 甚至报告
通过。该结果来自陈旧 build tree，不能作为当前源码门禁。

当前 `CMakeLists.txt` 中“vmrp-unit 只链接这部分”的注释也已过期。除非后续
恢复 unit target，否则最终验证不要把 stale `ctest` 计入成功项。

## 6. 修复后的分层验证命令

### 6.1 必须先补目标回归

仓库目前没有 `490111`/sanguo e2e，目标文件也只在 `build/mythroad`，不是
版本化 fixture。应新增目标启动用例并把精确 MRP 以可复现 fixture 提供，记录
上述 SHA-256 和来源；不能写“fixture 不存在就跳过”的兜底。

用例必须：

1. 使用 `VmrpWorkspace`，把包置于 workspace 的 `mythroad/` 下。
2. 在有界时间内证明 code-0 返回且 e2e socket 保持响应。
3. 抓取 240x320 PPM，断言明确的正常 UI 像素或经人工确认后的稳定区域；
   `uniqueColorCount > 1`/非黑只能作为辅助，不能单独定义游戏正常。
4. 保存首次正常画面和至少一次输入后的画面，证明不是静态 splash。

新增后，首轮命令应为：

```bash
cmake --build build --target vmrp -j2
pnpm exec tsc --noEmit
pnpm exec vitest run test/e2e/sanguo/boot-to-home.test.ts --reporter=verbose
```

### 6.2 最小现有兼容集

allocator/R9/private loader 修复后先运行：

```bash
pnpm exec vitest run \
  test/e2e/gtcm/boot-to-home.test.ts \
  test/e2e/opglqa/font.test.ts \
  test/e2e/golden/golden-frames.test.ts \
  test/e2e/gghjt/game-start.test.ts \
  --reporter=verbose
```

- `gtcm`：老 direct-compact wrapper、启动/场景/付费路径。
- `opglqa`：与目标大小较接近的 direct-compact 变体和二次启动。
- `golden`：两种 legacy direct wrapper 的全帧零 diff。
- `gghjt`：同长度 chain wrapper，保护不同 R9/嵌套模块路由。

如果改动触及 table[0]/[1] first-fit，必须再运行已知依赖地址复用的：

```bash
pnpm exec vitest run test/e2e/talkcat/game-prepare.test.ts --reporter=verbose
```

### 6.3 最终兼容门禁

```bash
cmake --build build --target vmrp -j2
pnpm exec tsc --noEmit
pnpm exec vitest run test/e2e --reporter=verbose
git diff --check
```

若屏幕/damage 路径也有改动，追加：

```bash
pnpm exec vitest run test/e2e/gfhcq/temp.test.ts --reporter=verbose
```

最终报告必须分别列出目标 PPM、目标交互结果、默认 19-file/32-test e2e
结果和显式 temp 结果；不能用全量 e2e 通过替代目标包验证，也不能用目标包
一张非黑图替代其它 fixture 的兼容性回归。

## 7. 最终验收结果

- 目标包：SDL dummy/无 Xvfb，开启 `VMRP_ARM_EXT_INVARIANTS=1`。code-0
  返回后进入绘制和网络初始化，尝试连接 `211.155.236.226:20000`。
  `/tmp/sanguo-final.ppm` 为 P6 240x320，SHA-256
  `c4ca6f078322e28d3969479da7b437f55458d6843eea96d9f188b7dd2e3db0e5`，
  画面为游戏自身的“连接超时”对话框，而非全黑帧。服务器不可达是外部状态。
- 定向 allocator 兼容集：3 files / 6 tests 通过，包含 Talkcat 的
  free-after-read 地址复用、GTMC 付费界面和 GXDZC 像素流程。
- 最终重建后定向集：3 files / 6 tests 通过，307.26s。
- 默认并行全量：19 files / 32 tests 通过，289.89s。
- `cmake --build build --target vmrp -j2`、`pnpm exec tsc --noEmit` 和
  `git diff --check` 通过。旧 `build/vmrp-unit`/`CTestTestfile.cmake` 不对应
  当前源码中的 target，不计为 unit 覆盖。
- 本次未修改 screen/damage 路径，因此默认排除的
  `test/e2e/gfhcq/temp.test.ts` 不属于此 allocator 修复的必需门禁。
