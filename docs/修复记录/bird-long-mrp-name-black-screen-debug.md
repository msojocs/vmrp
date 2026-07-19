# 长中文 MRP 文件名启动黑屏调试记录

日期：2026-07-17

## 目标与约束

- 复现命令：`build/skyengine build/mythroad/愤怒的小鸟VS僵尸2_v1002.mrp`。
- 目标：保持宿主文件名不变也能正常启动；不得按应用名、哈希或场景特判。
- 不使用 Xvfb，不开启全量 trace；运行证据使用 SDL dummy、E2E socket、
  有界 ARM EXT 诊断和 PPM。
- 修复必须由目标 EXT 反汇编证明，并在目标成功后回归其它用例。

## 阶段 0：工作树与样本身份

- 任务开始时只有用户维护的 `docs/prompt.md` 已修改，本次不覆盖该改动。
- `愤怒的小鸟VS僵尸2_v1002.mrp`、`愤怒的小鸟VS僵尸2.mrp`、
  `op6120_v1002.mrp` 和 `op6120.mrp` 内容完全相同，SHA-256 都是
  `611b4cd737dcf458370ff215bc73636cf65bdc6dfc36907c2ce8aa8f00b7c8e2`。
- MRP header `+0x10` 的原始 12 字节 filename 是 `op6120.mrp`；因此 A/B
  差异只来自宿主路径，不能归因于包内容、版本或缓存资源。
- 包内真实启动链为 `start.mr -> mrc_loader.ext -> cfunction.ext -> game.ext`。
  解压后 EXT 身份：
  - `mrc_loader.ext`：232 字节，SHA-256
    `d36151ee3c119717305afe4b1f0ba47f0f0154f8ba6f2c5081d6402c8eddd938`；
  - `cfunction.ext`：23492 字节，SHA-256
    `c2ef8c9995545fbc09e66100e94e502d003fea111b4267bb73d2c85b6d87ef6f`；
  - `game.ext`：5188 字节，SHA-256
    `a1f15d83ad665fac91a68ed84dc42a88c04f0c4efa6f0a76947c5165e092d74b`；
  - `verdload.ext`：18984 字节，SHA-256
    `374445febaf94de6f126e74814782268c94aefc0fa5425a065ec7a030a4bb214`。

## 阶段 1：无 Xvfb 的 A/B 与 PPM

使用 SDL dummy 和 E2E socket 启动 8 秒：

- `_v1002` 长名：进程和 socket 存活，`DRAW_COUNT=0`；强制 `SCREEN` 得到
  P6 `240x320` 全黑 PPM，unique color 为 1、nonblack 为 0、SHA-256
  `1edcc2a5a6f7f596acf98d8fbd248c4175886dc8759e6d75ec652b339c2a53b6`。
- 无后缀短名：`DRAW_COUNT=5077`；PPM unique color 为 1697、nonblack 为
  75128、`pixel(184,128)=(240,240,240)`，SHA-256
  `c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`。
  该 hash 与既有 op6120 成功启动帧完全相同。
- 两个进程都可通过 E2E `QUIT` 正常退出，失败侧无 `UC_ERR`、fatal 或崩溃。

ARM 可见包名的 UTF-8 字节数：

| 宿名 | cwd-relative alias | basename |
|---|---:|---:|
| `愤怒的小鸟VS僵尸2.mrp` | 37 | 28 |
| `愤怒的小鸟VS僵尸2_v1002.mrp` | 43 | 34 |

## 阶段 2：首次分叉与 scanner 误判

`game.ext` 运行时文件基址为 `0x2263A0`，helper 为 `0x226DF9`。code 0
中的 `+0x452..+0x474` 确实命中原
`arm_ext_child_reads_record100_to_compact_r9_buffer()`：函数先清零 R9-relative
的七个 word，再以 `r0=1, r1=100` 调用 `+0x818`。但是把立即数 100 解释为
`record[100]` 是错误的；它在这里是 MPS selector。

关键调用点如下：

```text
+0x452  b530       push {r4,r5,lr}
+0x454  b083       sub  sp,#12
+0x456  2100       movs r1,#0
+0x458  2200       movs r2,#0
+0x45a  2300       movs r3,#0
+0x45c  4448       add  r0,r9
+0x45e  c00e       stmia r0!,{r1,r2,r3}
+0x460  c00e       stmia r0!,{r1,r2,r3}
+0x462  c008       stmia r0!,{r3}
+0x468  4449       add  r1,r9
+0x46a  9101       str  r1,[sp,#4]
+0x46c  2164       movs r1,#100
+0x470  2001       movs r0,#1
+0x474  f000 f9d0  bl   +0x818
```

继续反汇编 `game.ext+0x818` 可见它从 child `extChunk+0x28` 取
`sendAppEvent`，并把最后一个栈参数固定为 `extCode=2`。因此 `+0x474` 的完整
八参数调用是：

```text
app=1, code=100, param0=0, param1=0,
param2=0, param3=R9+0x104, param4=0, extCode=2
```

外层 `cfunction.ext+0x18B0` 的 selector-100 分支给出了决定性证据：它从
`[sp+0xBC]` 取 `param3`，向目标的 `+8/+12/+16/+20/+24` 写函数和运行时状态
指针，首两个 word 保持清零。这里初始化的是七 word MPS API/state 结构，不是
字符串，也完全没有读取 private record table。

真正的包名读取在另一路径：

- `game.ext+0x9CC` 直接从 private record 取 `record[100]` 指针并返回；
- `+0x9E0..+0x9F8` 把该指针传给 `+0xB54`；
- `+0xBE8..+0xBF8` 调 `strstr(pack, ".mrp")`；
- 未命中时 `+0xC1C..+0xC28` 再检查 `.skf`，两者都没有则经
  `+0xD0A -> +0xD3E` 提前返回。

原 scanner 因此制造了失败：28 字节短 basename 可直接作为 alias，而 34 字节
长 basename 超过 `ARM_EXT_SHORT_PACK_ALIAS_MAX=32` 后变成 `~p0`。guest 随后
把这个错误注入的 token 当文件名检查，因缺少 `.mrp`/`.skf` 提前返回；它从未
走到 alias-aware 文件桥。长名本身没有进入固定包名缓冲区，直接保留真实路径
才是正确语义。

## 阶段 3：否决的候选修复

曾验证把 synthetic token 临时改为 `~p0.mrp` 可以绕过 suffix gate 并生成正确
PPM，但独立 review 和进一步反汇编证明这只是掩盖 scanner 误判，而且引入新的
通用兼容风险：

- 无条件追加 `.mrp` 会把合法长 `.skf` owner 伪装成另一种 package 类型；
- `~pN.mrp` 可能与真实根包 basename 冲突，而当前 resolver 在 synthetic
  registry 前先匹配 root alias/suffix；
- table[47]/[48] 尚不解析 synthetic alias，remove/rename 语义并不完整。

因此最终没有修改 synthetic alias 格式，也没有增加 suffix、重试或文件 I/O
兜底逻辑。

## 阶段 4：最终通用修复

删除错误的 `arm_ext_child_reads_record100_to_compact_r9_buffer()` 分类器及其只供
该分类器使用的 Thumb BL helper。在 `arm_ext_child_needs_short_pack_alias()` 中
保留生产注释，明确普通 MPS selector-100 输出是七 word API/state 结构，selector
语义属于 parent dispatcher，不能单凭立即数和目标指针推断固定包名复制。

DOTA 已有的真实危险形状保持不变：

- `B5FF/B089` 栈帧中的 32 字节 package buffer；
- `B5B0` 初始化函数中的 clear32 + strlen + memcpy；
- compact R9 state-list 形状。

修复不读取应用名、文件名、哈希或 payload 长度，没有失败重试和 fallback。
新增 E2E 用 34 字节 basename 固定复现 scanner 误判，并断言已知成功 PPM hash。

## 阶段 5：目标与兼容复验

- `cmake --build build --target skyengine -j2` 通过；删除失效 helper 后无新增 warning。
- OP6120 定向 E2E 2/2 通过：既有 4M cold boot 和新增长名 cold boot 分别约
  4.70 秒、4.31 秒。
- 使用原命令 `build/skyengine build/mythroad/愤怒的小鸟VS僵尸2_v1002.mrp`
  在 SDL dummy 下直接运行，`DRAW_COUNT=582`，到达 `my_initNetwork`。
- 直接运行和两个 E2E 的 P6 `240x320` PPM SHA-256 都是
  `c84009d93e752378bfaee8d98ab972ae5c2474d447b4e2c3895db7a7228b60d8`，
  与短名基线逐字节相同；stdout/stderr 无 emulator/fatal 标记。
- DOTA `下载浏览器插件 - 成功` 通过（38.76 秒），证明真实 32 字节 package-copy
  保护仍然有效。
- Cookie `方式一` old-package handoff 通过（18.61 秒），证明既有 `~pN`
  synthetic alias 往返语义未回归。
- `pnpm exec tsc --noEmit` 通过；`ctest --test-dir build --output-on-failure`
  1/1 通过。
- 最终使用 `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy`、不使用 Xvfb
  运行全量 E2E：24/24 test files、46/46 tests 全部通过，总耗时 272.75 秒。
- `wiki_lint` 接受新增 wiki 页面；报告的两个 broken ref 属既有页面，和本次
  修改无关。最终 `git diff --check` 通过。
