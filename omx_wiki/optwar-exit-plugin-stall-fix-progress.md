# optwar 退出插件更新卡住 100% 修复进度

日期: 2026-06-29

## 测试命令

```bash
pnpm vitest run test/e2e/optwar/exit-plugin.test.ts -t "游戏退出时下载插件"
```

## 已确认的问题链

1. wrapper 进入模态 (extChunk[0x34] = 1)
2. verdload.ext 下载 promote.mrp 成功
3. wrapper 加载 promote (freecurr.ext)
4. promote 检测 brwcore.mrp 缺失 → 触发第二次下载
5. 第二次 verdload 下载 brwcore.mrp 成功
6. verdload 清空定时器队列后 timer_p_addr 仍指向 verdload → **已修复**
7. wrapper 获得定时器后处理队列，加载 brwcore → **activeP 变化修复已实现**
8. wrapper 队列耗尽后合成定时器节点给 promote → **已实现但无效**
9. ★ 实际根因: 当 code=2 路由到 primary game helper 时，PC 跳转到 "brwcore.mrp" 字符串地址

## 根因: ARM 代码空间/数据损坏

wrapper 加载 brwcore 后，primary game helper (0x2D60AD) 的代码空间或 GOT 被子模块数据覆盖:

```
uc_emu_start(0x2D60AD) failed: 21 (UC_ERR_EXCEPTION)
pc bytes @0x2C5644: 62 72 77 63 6F 72 65 2E 6D 72 70 = "brwcore.mrp"
R9=0x002DDA1C (primary game RW)
LR=0x002F1FF9 (verdload code range)
```

game helper 执行过程中 PC 跳转到 promote 模块的数据段（含 "brwcore.mrp" 字符串），
说明 game 的函数指针或 GOT 条目被子模块加载覆写。

## 已实现的修复（前序问题）

1. **timer_owner_transfer**: verdload 队列空后将 timer 所有权从子模块转给 wrapper
2. **activeP 变化保活**: wrapper 加载新子模块后保持宿主定时器运行
3. **foreground_child_active 保活**: 模态下活动子模块存在时维持定时器
4. **合成定时器节点**: wrapper 队列耗尽后为 promote 合成 0xE83E1D 回调节点
5. **嵌套模块 extChunk[8] 修复**: 修复被 wrapper 清零的子模块 helper 指针

## 下一步

调查 brwcore 加载路径中的内存损坏:
- 检查 brwcore (148224字节) 是否覆写了 game helper 或其他模块的代码/数据段
- 检查 arm_alloc 内存分配是否有重叠
- 检查 GOT snapshot restore 是否在 brwcore 加载后产生了错误的恢复

## 2026-06-29 18:37 当前补丁后复测

命令:

```bash
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/optwar/exit-plugin.test.ts -t "游戏退出时下载插件" --reporter=verbose
```

结果仍失败，保留目录 `/tmp/skyengine-e2e-cRvO3g`。失败点仍是
`promote-start.ppm` 的 `(104,296) == [0,0,0]`，但崩溃形态已经从
跳入 `"brwcore.mrp"` 字符串变成 Thumb 模式问题:

```text
arm_ext_executor: uc_emu_start(0xE800B0) failed: 10 (Invalid instruction (UC_ERR_INSN_INVALID))
pc bytes @0x2D0A20: 0A 20 00 F0 49 F8 38 BC 08 BC 01 20 18 47 00 00
R9=0x002C5C84 LR=0x002F1D8B PC=0x002D0A20
```

`0x2D0A20` 按 Thumb 解码是有效指令序列，说明当前剩余问题不是网络或
磁盘写入，也不是再次跳入字符串数据，而是 wrapper/helper 执行链在返回
game Thumb 回调时丢失了 Thumb 状态或未按 `PC|1` 进入。stdout 还显示
第二次 `POST /dl_confirm` 后已进入 `mr_restart_old_app` 驱动的
`_mr_intra_start(... dofile('start.mr'))`，因此“重启中”界面后的启动路径
已经被触发，但 ARM EXT 异常阻断了前台应用超市渲染。

PPM 证据: `promote-start.ppm` 为 240x320，只有 3 色，非黑 bbox
`74,128..166,174`，仍是绿色/白色“重启中”样式。

## 2026-06-29 18:46 R9 归属新证据

在 `UC_ERR_INSN_INVALID` 后把 Thumb 重试入口改成 `PC|1`，并把同类
`UC_ERR_EXCEPTION` 也纳入一次性 Thumb 重试后，目标测试仍失败，最新目录
`/tmp/skyengine-e2e-KbnGRK`。异常稳定为:

```text
arm_ext_executor: uc_emu_start(0xE800B0) failed: 21 (Unhandled CPU exception)
pc bytes @0x2D0AC4: 01 B0 08 BC 18 47 00 00 ...
R9=0x002C5C84 SP=0x002C578C LR=0x002D0A27 PC=0x002D0AC4
```

`0x2D0AC4` 落在 primary child 文件区间
`0x2C6208..0x2DDA14` 内，而该模块的 RW 是 `0x2DDA1C`；异常时
`R9=0x2C5C84` 是 wrapper RW。当前根因收敛为：wrapper 调入 primary
child 后，宿主侧 R9 恢复/同步逻辑把 R9 过早还原成 wrapper RW，导致
primary child 的 R9-relative 函数指针/回调栈取错。怀疑点是
`outer_r9/nested_return_addr` 的返回点恢复逻辑误把 child 内部地址当成
“返回 wrapper”的地址。

## 2026-06-29 19:20 当前复现与“重启目标”确认

命令:

```bash
VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_DIAG=1 pnpm vitest run test/e2e/optwar/exit-plugin.test.ts -t "游戏退出时下载插件" --reporter=verbose
```

结果仍失败，保留目录 `/tmp/skyengine-e2e-yiJkxm`。PPM 形态未变：
`promote-start.ppm` 为 240x320、3 色，非黑 bbox `74,128..166,174`，
`(104,296)=[0,0,0]`，仍是黑底绿/白“重启中”样式。

关键结论：这里的“重启”目标是 promote（应用超市/营销商店），不是
brwcore。证据是第二次进入启动路径时，wrapper 先重新打开
`plugins/promote.mrp`，随后 promote 以 `pack='plugins/promote.mrp'`
读取 `hf.bmp/hsv.bmp/hfl.bmp/hfm.bmp/hfr.bmp`、`promote/font12`、
`promote/config`、`promote/brw/error`、`promote/brw/downlist`，active
child 变为 promote:

```text
DIAG synced file=0x2C6208 len=36092 ... P=0x3BA16C H=0x2CCF7D ... activeP=0x3BA16C
DIAG table125 name='hf.bmp' ... pack='plugins/promote.mrp' ... ownerP=0x3BA16C
DIAG table42 name='promote/font12' ... ownerP=0x3BA16C
DIAG table40 name='plugins/brwcore.mrp' ... activeP=0x3BA16C
```

`brwcore.mrp` 是 promote 启动后检查/加载的浏览器核心组件。异常发生在
brwcore 私有加载阶段：`table131 cmd=9 data=0x2D0578 len=148224` 后，
旧 20KB/52KB 子模块记录被 `stale_child_drop` 清掉，随后 brwcore 的
semihost `writec` 出现在 `pc=0x2D0AC4`，但最终异常 PC 变为
`0x3BA9AC`:

```text
DIAG table131 cmd=9 data=0x2D0578 len=148224 ... P=0x3DEAEC lastFile=0x3BA504/148224
DIAG semihost_writec pc=0x2D0AC4 ch=0x0A r1=0x2C578C
arm_ext_executor: uc_emu_start(0xE800B0) failed: 10 (Invalid instruction)
pc bytes @0x3BA9AC: 0A 20 00 F0 49 F8 38 BC 08 BC 01 20 18 47 00 00
R9=0x002C5C84 LR=0x003DBD17 PC=0x003BA9AC
```

`0x3BA9AC` 不在 brwcore 的 ARM 文件区 `0x2D0578..0x2F4908`，而落在
wrapper 为 brwcore 记录的 `lastFile=0x3BA504/148224` 范围内。该字节序列
按 Thumb 解码是有效尾声/调用序列，但当前 `R9=0x2C5C84` 仍是 wrapper RW。
下一步应围绕私有 loader 的 staged file/record 区间和 R9/Thumb 返回点：
确认 `lastFile` 是否被误当作可执行 file range，或 `nested_return_addr`
/ `hook_restore_r9` 是否在 brwcore loader 返回期间把 R9/PC 恢复到了错误
上下文。

## 2026-06-29 20:21 brwcore 样本与入口 ABI 重新校准

当前必须以运行时 dump 为准。`/tmp/skyengine-internal-0x2D0578-148224.bin`
的 SHA256 是 `1978f27d90887ae9398aef643b95d9fe61b5e5c2bceeb4c70cb1d121eb5a92de`；
而 `/tmp/brwcore.ext`、`/tmp/skyengine-brwcore.ext` 和子代理提取的
`/tmp/skyengine-readonly-disasm-46407/brwcore_brwcore.ext` 都是
`6c2c3065c0f134fdb3847080c3b205b0e71e05234d861d38e9ad054bd5667758`。
两者同为 148224 字节，但 offset `0x217F0..0x21820` 不同，因此不能混用
反汇编结论。

运行时样本的 ARM entry 仍是:

```asm
0x00000008: push {r4,lr}
0x0000000c: mov  r4,r0
0x00000010: mov  r0,r4
0x00000014: blx  0x21808
0x00000018: pop  {r4,pc}
```

运行时样本在 `0x217FE..0x21812` 的 Thumb 序列是:

```asm
0x217fe: push {r3,r4,r5,r6,r7,lr}
0x21800: ldr  r5,[pc,#64]
0x21802: adds r6,r0,#0
0x21804: add  r5,pc
0x21806: ldr  r0,[r5,#60]
0x21808: movs r4,#0
0x2180a: mov  r7,r9
0x2180c: ldr  r0,[r0,#0]
0x2180e: blx  0x4a8
0x21812: ldr  r0,[r5,#56]
```

这说明当前崩溃样本里 `BLX 0x21808` 的确跳进了该函数 prologue 之后。
最新运行时诊断也显示首次执行已在 staging 区 `0x2D0578+0x21808`，
并非 raw mirror；但进入时 `r0=1`、`r5=0xE7FFC8`、`r9=wrapper RW`，
随后马上进入 `0x4A8` 的错误打印/返回尾巴。raw mirror PC 是该错误路径
之后的下游症状，不应作为根因用 invalid-PC fallback 处理。

下一步只做窄诊断：在 wrapper private loader `0xE83BEA` 直接 `BLX
file_base+8` 前记录寄存器、extChunk/P/record 字段和栈参数，确认真机
loader 是否本应在 entry 前装入 child RW/SB 或其它 ABI 上下文。

## 2026-06-29 21:10 根因收敛: 子模块入口 R9/r5 = wrapper 值（未切换到 brwcore 自身上下文）

精确反汇编 brwcore 解码镜像 (/tmp/skyengine-internal-0x2D0578-148224.bin, base 0x2D0578):

- ARM 入口 file+8 (0x2D0580): `push{r4,lr}; mov r4,r0; mov r0,r4; blx 0x2F1D80(=file+0x21808); pop{r4,pc}`。
- 入口 helper 完整 prologue 在 file+0x217FE: `push{r3-r7,lr}; ldr r5,[pc,#0x40]; mov r6,r0; add r5,pc; ldr r0,[r5,#0x3c]`。
  **但 ARM 入口 blx 的是 0x21808（prologue 之后），跳过了 `ldr r5/add r5,pc` 的 r5(GOT基址) 建立**。
  因此 r5、r9 必须由调用方(wrapper)在 blx 前设为 **brwcore 自身的 GOT/RW**。
- 0x21808 起: `movs r4,#0; mov r7,r9; ldr r0,[r0]; blx 0x2D0A20(=file+0x4A8)`。
  随后 0x2F1D8A `ldr r0,[r5,#0x38]` 用 r5 读全局；0x2F1D9E `blx r1`(r1=pc字面量+r9) 用 r9 算函数指针。

运行时实测(pc_hit off=0x21808): **r5=0xE7FFC8(wrapper 数据)、r9=0x2C5C84(wrapper RW)** —— 都是 wrapper 上下文，不是 brwcore 的。
导致 0x2F1D8A 读 wrapper 全局、0x2F1D9E 用 wrapper r9 算出错误函数指针，分支进入 raw readFile 镜像(0x3BA594)的 ARM 态 → 0x4A8 Thumb 字节按 ARM 解码 = 未定义指令崩溃。

崩溃点 0x4A8 是 `blx 0x4A8`(Thumb→ARM) 的目标，本应 Thumb 执行；一次性 Thumb 重试只覆盖第一份(copy1=0x2D0578)，
第二份(copy2=raw 0x3BA594) 再次进入 ARM 态崩溃，无重试剩余。

对比: verdload/promote 等子模块加载时宿主会把 R9 切到子模块 RW（alloc hook 的 nested R9-switch），
brwcore 是 **promote(子) 再加载 brwcore(孙)** 的二级嵌套，宿主 nested R9-switch 条件未触发，R9 仍是 wrapper RW。

下一步: (A) 反汇编 cfunction.ext 在 blx 子入口前如何取 r5/r6/r9（来自哪个 chunk/record/P 字段）；
(B) 定位宿主 nested R9-switch 为何对 promote 下的 brwcore 不触发，做通用修复（非 app 特定）。

## 2026-06-29 23:10 最终根因: table[44] GOT restore 污染 RAM 包 gzip

21:10 的 R9/r5 假设已被运行时包体证据推翻：brwcore 入口异常是 gzip
payload 被宿主写坏后的下游现象，不是 brwcore 专用 ABI 入口问题。

关键证据:

- 已安装的 `mythroad/plugins/brwcore.mrp` 与 `build/mythroad/plugins/brwcore.mrp`
  完全一致，大小 99357，SHA256
  `5f312bce40cad9570a9e49f5d6c6e96561251b7470391a6fe292c5eabfa426c4`。
- 其中 `brwcore.ext` gzip payload 位于 offset 288、长度 99069，Python
  解压后得到正确 148224 字节镜像，SHA256
  `6c2c3065c0f134fdb3847080c3b205b0e71e05234d861d38e9ad054bd5667758`。
- 运行时 `$` RAM 包 dump 是一个 old-style MRPG 单文件包，payload 从 offset
  24 开始，长度同为 99069，但 Python gzip 报
  `invalid distance too far back`。
- RAM payload 与正确 payload 的首个差异在 `0xCD0`；差异段包含
  `0x10068, 0x1000C, 0x10010, ...` 这类 `EXT_TABLE_ADDR + slot*4`
  bridge 指针值。
- 地址映射为 `ramp + 24 + 0xCD0 = 0x2DDA60`，正好落在当次 active RW
  `0x2DDA1C + 0x44`，即 GOT snapshot restore 写回窗口。

真正根因: table[44] 的大块 `mr_read` 后处理在任何读缓冲区覆盖
`m->got_snapshot_base` 时都会把桥函数指针写回。该逻辑本来服务于
dump0/primary executable restore，但 optwar 的下载包/RAM staging 缓冲区
恰好复用了同一地址范围，导致压缩 payload 被 GOT 指针覆盖。后续
`mr_readFile_from_ram()` 仍能走出一个“看似可执行但已损坏”的 brwcore 镜像，
于是出现误导性的 Thumb/R9/entry 异常。

最终修复:

- table[44] 仍对大块 read 执行 `uc_ctl_remove_cache()`，并继续调用
  `arm_ext_restore_primary_mapping_after_dump0()`。
- GOT snapshot restore 仅在 read range 被证明完整覆盖 primary executable
  image 时执行:
  `arm_ext_range_contains(r1, ret, m->primary_file_addr, m->primary_file_len)`。
- 没有新增 app-specific 分支，也没有把异常吞掉；普通下载包/RAM staging
  read 不再被当作 executable restore 修桥。
- 已移除本轮临时 dump/watch 诊断，包括 `VMRP_ARM_EXT_DUMP_WRAPPER`、
  `VMRP_ARM_EXT_DUMP_INTERNAL` 写 `/tmp`、hard-coded PC/R9 watch 等。

最终验证:

```bash
cmake --build build --target skyengine -j2
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/optwar/exit-plugin.test.ts -t "游戏退出时下载插件" --reporter=verbose
VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gghjt/download-plugin.test.ts -t "下载付费插件 - 下载完毕$" --reporter=verbose
```

结果:

- optwar focused test passed，`1 passed | 1 skipped`，保留目录
  `/tmp/skyengine-e2e-6hoY4V`。
- `promote-start.ppm` 为 240x320，`pixel(104,296)=220,240,160`，
  `unique=53`，`nonzero=76619`，不再停在黑底“重启中”画面。
- GGHJT netpay/download regression passed，`1 passed | 4 skipped`，
  覆盖原本依赖 table[44] dump0/GOT restore 的路径。
- 最新保留日志未出现 `UC_ERR`、`UC_MEM`、`Invalid instruction`、
  `Unhandled CPU exception` 或 `pc bytes` 崩溃签名。
