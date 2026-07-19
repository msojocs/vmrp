# op6120 4M 启动卡死调试记录

日期：2026-07-17

## 目标与约束

- 复现命令：`build/skyengine build/mythroad/op6120.mrp --memory 4M`。
- 预期：游戏完成加载并进入可交互界面；当前 4M 卡在进度画面，1M 正常。
- 根因必须由 ARM 反汇编和运行现场共同证明，不能按应用名、包哈希或固定场景分支。
- 不使用 Xvfb；只做有界日志、精确地址观察和 PPM/绘制计数验证。
- 先完成目标修复和定向测试，再运行并兼容现有测试集。

## 阶段 0：工作树审计

- 当前分支 `dev` 比 `origin/dev` 多 3 个提交，任务开始时只有用户维护的
  `docs/prompt.md` 处于修改状态，本次调查不覆盖该改动。
- 样本位于 `build/mythroad/op6120.mrp`，大小 525104 字节。
- 最近提交 `0bfa6a6` 调整过 ARM EXT 的 nested resource owner、wrapped
  `LG_mem` first-fit 解码和 guest write span 验证；这些变化会作为候选影响面，
  但在取得 `op6120` 的分叉指令和数据之前不直接修改。
- 仓库已有无 Xvfb 的 E2E socket 与 PPM 采集能力，可用独立 work-dir 验证
  加载页、目标页、draw count 和按键响应。

## 当前计划

1. 在相同的干净运行树中分别复现 1M 和 4M，保存 PPM、draw count、进程状态
   和有界错误摘要。
2. 提取实际执行的 EXT，反汇编内存大小相关分支和卡死现场附近调用链。
3. 将 guest 原生语义与 host bridge/allocator 实现逐字段对照，提交通用修复。
4. 新增 4M 启动回归，并在最终源码上运行全量 E2E。

## 待填证据

- 样本 SHA-256、内部文件清单和实际 EXT 基址/哈希。
- 1M/4M 首个分叉 PC、寄存器、关键内存字段和调用链。
- 卡死 PPM 与成功 PPM 的稳定特征。
- 根因反汇编、源码修复点、定向测试和全量回归结果。

## 阶段 1：稳定复现与首个分叉

样本 SHA-256 为
`611b4cd737dcf458370ff215bc73636cf65bdc6dfc36907c2ce8aa8f00b7c8e2`。
在复制 `wasm/dist/fs/mythroad` 的独立 work-dir 中，以 SDL dummy 和 E2E
socket 运行，不使用 Xvfb：

- 4M：draw count 到 13 后至少 20 秒不再变化，PPM SHA-256
  `56071684cf261086bd89fed8614138df21aeaff996b7455455b5a7b64f054003`，
  进程和 E2E socket 仍存活，没有 `UC_ERR` 或结构不变量错误。
- 2M：10 秒后得到与 4M 逐字节相同的 PPM，证明问题不是只在 4M 的单点
  地址，而是主堆超过 1M 后的容量边界。
- 1M：约 10 秒时 draw count 已超过 6000，进入网络/广告流程；采样 PPM
  SHA-256 为
  `c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`，
  有 1697 种颜色和 75128 个非黑像素。

三个档位都生成了相同的 `cache/op6120/sky.bmp`（3822820 字节，SHA-256
`6db8e39e624a3606ce40e4cc5cc4c42f06a30055f625c4a28e6f51df66f56def`），
所以卡死不是 SKY 文件生成失败或脏缓存差异。

## 阶段 2：容器与 ARM 反汇编

包内启动链为 `start.mr -> mrc_loader.ext -> cfunction.ext -> game.ext`。
`game.ext` 再从 `mythroad/plugins/flaengine.mrp` 加载真正的 Flash 引擎：

- `flaengine.ext`：129312 字节，SHA-256
  `da95583347450358a093fc0c95687cdff52438728e63dca9309f59066da6ca8b`；
- 标准 ARM 入口 `+0x8`，Thumb loader `+0x18734`，helper `+0x1AD3D`；
- exRAM 缓存检查 `+0x5D4`，申请入口 `+0x6C4`，检测入口 `+0x7C0`；
- helper 的 code=2 分支 `+0x1AD9A` 调 timer 函数 `+0x1B7D0`。

精确 PC 观察表明 4M 运行中 `flaengine.ext` 基址为 `0x227928`。在
`file+0x6C4` 命中时 `R0=0x00A00000`；随后实际
`MR_MALLOC_SCRRAM(1014)` 从 `input_len=0x00A00400` 开始申请。

## 阶段 3：根因闭环

1M 和 4M 的 table[38] 调用点相同，返回 LR 均为 `flaengine.ext+0x733`：

- 1M：`0x345EF4 + 0xA00400 < EXT_STACK_ADDR(0xE00000)`，当前实现从
  主 bump 堆一次分配成功，返回 `out=0x345EF4`。
- 4M：origin pool 已把主 bump 顶推高约 3MiB；10MiB 请求跨过 wrapper
  栈，`arm_alloc()` 跳到 wrapper code 末 `0xE85BC4` 后又超过 16MiB 主映射，
  返回 `MR_FAILED/out=0`。
- guest 按自身探测协议每次把请求减 `0x400` 重试。从 `0xA00400` 到首次可
  成功的 `0x18A400` 相差 8664 个递减档，最后只能在 `0xE85BC4` 得到约
  1.5MiB；长循环造成表面卡死，且所得 SCRRAM
  不足以推进 Flash 资源阶段。

这与真机 ABI 不同：`src/mythroad/dsm.c` 中 1014 的预留参考实现以独立
`malloc` 表达扩展 RAM（当前 native 分支仍返回 `MR_IGNORE`）；SCRRAM 不属于
主 `LG_mem` 或 ARM executor bump 堆。
因此修复方向是新增独立的 guest SCRRAM 映射，并让 1014 只使用该区域。
缩小 `--memory`、修改 guest 请求、扩大主映射或按应用绕过重试都会掩盖错误的
内存所有权模型，不作为方案。

## 阶段 4：通用实现与生命周期验证

实现新增独立的 `0x50000000..0x50FFFFFF` SCRRAM guest 地址带。table[38]
第一次收到 1014 时以 `calloc` 创建 host backing，并通过 `uc_mem_map_ptr()`
映射为 guest RW 数据；它不进入 `arm_alloc()`、`LG_mem` first-fit 或
`table[108..111]` 的内存统计。单次请求超过 16 MiB 时明确失败，没有退回主堆
的兜底分支。

同一活动分配的较小或相等请求复用稳定地址；增长请求只清零新暴露的尾部，
保留 guest 已写入的旧前缀。1015 对 `NULL` 按 native free 语义返回成功；对当前
分配首址执行 Unicorn unmap 后释放 backing；外来、重复的非空指针明确失败。
模块未显式释放时，unload 在 `uc_close()` 后回收仍存活的 backing。

最终代码的有界 table[38] 诊断只观察到一次 SCRRAM 请求：

```text
code=1014 inputLen=10486784 ret=0 out=0x50000000 outLen=10486784
```

没有出现 guest 每次减 `0x400` 的重试。目标应用的宿主 `QUIT` 不进入 guest
清理流程，未触发 1015；因此另以最小 Unicorn ARM 调用级诊断验证在
`UC_HOOK_CODE` 内完成 `map_ptr -> unmap/free -> 同址重映射 -> read/write`，
结果为 `hook unmap/remap/read/write: ok`。这直接覆盖 1015 所依赖的 hook 内
映射生命周期能力，没有向产品加入测试入口。

## 阶段 5：4M 冷启动回归

新增固定样本 `test/fixtures/op6120.mrp`，SHA-256 与原始样本一致。E2E 每次使用
独立 work-dir，预置本地 `advbar.mrp`，删除 `cache/op6120/sky.bmp` 强制资源
转换，并通过 `--dns-map` 把应用可能访问的外部域名约束到 `127.0.0.1`。

最终源码在 4M 下约 6 秒完成冷启动：

- 成功 PPM SHA-256：
  `c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`；
- 与此前 1M 成功帧逐字节相同，关键像素 `(184,128)` 为
  `rgb(240,240,240)`，颜色数为 1697；
- draw count 大于卡死基线 13，随后继续增长；
- 冷生成 SKY 大小 3822820 字节，SHA-256 为
  `6db8e39e624a3606ce40e4cc5cc4c42f06a30055f625c4a28e6f51df66f56def`；
- stdout/stderr 不含 `UC_ERR`、`UC_MEM`、`unmapped`、`INVARIANT` 或 fatal
  执行器错误。

`pnpm exec tsc --noEmit` 和定向
`pnpm vitest run test/e2e/op6120/boot-4m.test.ts` 已通过。共享内存相关兼容测试
`wxmdld`、`sanguo`、`istore`、`gwkdl` 共 4 个文件全部通过。最终
`pnpm test:e2e` 在默认并发模式下通过 24 个文件、45 个测试，耗时 295.00 秒；
其中 op6120 冷启动用例在并发压力下耗时 6.78 秒。最终构建、TypeScript、
`git diff --check` 和定向冷启动复跑（5.69 秒）也全部通过。

## 阶段 6：独立复核（方案审查 + 回归复跑）

对最终改动做了独立方案审查与全量复跑：

- 地址带审查：`0x50000000..0x50FFFFFF` 位于 `EXT_PLATFORM_MEM_ADDR`
  (0x40000000) 与 `EXT_EXECUTOR_META_ADDR` (0x70000000) 之间的空隙，
  与主映射、低地址表、IO/ALT band 均不重叠。
- 多模块审查：嵌套 EXT（game.ext → flaengine.ext）复用同一
  `ArmExtModule`/`uc`（`aex_t025` 记录进 `nested_modules[]`，不新建实例），
  单活动 SCRRAM 映射不会与嵌套加载冲突。
- 生命周期审查：`arm_ext_unload()` 中 `free(m->scrram_mem)` 在
  `uc_close()` 之后，与 `platform_mem`/`low_table`/`mem` 等所有
  `uc_mem_map_ptr` backing 的既有释放顺序一致（backing 必须存活到引擎
  关闭），顺序正确。
- 字段隔离审查：`exram_addr/exram_len` 仅在 `aex_t038` 的 1014/1015 分支
  读写，table[108..111] 内存统计只使用 `origin_mem_*`，无遗留耦合。
- 回归复跑：定向 `boot-4m.test.ts` 通过（6.39 秒）。第一次全量
  `pnpm test:e2e` 中 `gghjt/download-plugin` 的"下载完毕返回重进"在并发
  压力下出现一次像素时序抖动（该用例不使用 `--memory`，也不经过
  table[38] SCRRAM 路径）；单独重跑该文件 5 例全部通过，第二次全量
  24 文件 / 45 测试全部通过（298.01 秒），确认为并发时序抖动而非本次
  修改引入的回归。
