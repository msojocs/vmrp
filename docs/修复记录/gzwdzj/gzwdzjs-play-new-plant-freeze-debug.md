# gzwdzjs 第一关结束后新植物弹窗卡死调试记录

## 目标与约束

- 目标用例：`pnpm vitest run test/e2e/gzwdzjs/play.test.ts`
- 预期：第一关正常推进，并在 20 秒内显示“获得新植物”弹窗。
- 实际：放置植物后游戏画面卡住，不再推进。
- 约束：不使用 xvfb；控制 trace 量；通过 ARM 反汇编定位根因；修复必须是通用语义修复，不能按应用或场景特判；代码修改需要解释原因的注释。

## 2026-07-12 初始基线

- `play.test.ts` 已覆盖启动、关闭音乐、教程加载、进入实际战斗等阶段；当前问题发生在第 98 行开始的“放置植物”之后。
- 用例在第 108 行轮询 `(94,145) == rgb(208,244,200)`，把该像素作为“获得新植物”弹窗出现的证据；当前轮询超时为 90 秒，但本任务验收要求为 20 秒。
- 既有 `gzwdzjs-tutorial-loading-hang-debug.md` 与 `gzwdzjs-no-netpay-freeze-debug.md` 证明该应用曾因 compact allocator 错误复用 live timer/module 区域而冻结；这只能作为候选背景，尚不能证明本次战斗中卡死具有相同根因。
- 当前工作树在调试开始前已有用户改动：`docs/prompt.md`、`test/e2e/wbrw/temp.test.ts`。本轮不修改或回退这些文件。

## 进行中

- [ ] 无 xvfb 复现目标用例，保留临时目录、stdout/stderr 和关键 PPM。
- [ ] 确定最后推进的帧/事件/定时器及首个异常现场。
- [ ] 对照 `game.ext` 反汇编还原战斗结束状态机与模拟器桥接调用。
- [ ] 实施有结构依据、无应用特判的修复。
- [ ] 以目标用例、20 秒窗口和 PPM 内容验证“获得新植物”。

## 2026-07-12 原始用例复现与首轮反汇编

- 命令：`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/play.test.ts --reporter=verbose`
- 结果：稳定失败于 `play.test.ts:122`，总用时约 80 秒；保留目录 `/tmp/skyengine-e2e-FTntKv`。
- 最终 `study-end.ppm` 实际仍是战斗画面，不存在“获得新植物”弹窗。第一段轮询的 `(94,145) == rgb(208,244,200)` 偶然命中场景像素，属于测试误判，不能作为结算证据。
- 冻结期间 stdout 连续 532 次出现同一现场：`PC=0x22632C`、`LR=0x23D5C1`、`SP=0x445A4C`，错误为 `UC_ERR_EXCEPTION`。
- 从 MRP 文件表按 `game.ext` 偏移 `0x284F9`、压缩长度 `0x11B62` 提取并解压，得到 116492 字节映像；运行时基址为 `0x226118`。
- ARM 反汇编证明 `0x226320` 是：
  ```asm
  mov r0, #24
  ldr r1, [pc, #8]   ; 0x00020026
  svc 0x123456
  bx  lr
  ```
  这是 ARM semihosting `SYS_EXIT`，不是普通 ARM/Thumb interworking veneer。
- Thumb 调用链为 `0x23D5BC -> 0x226148`；`0x226148` 在 `r0 != 0` 时直接返回，在 `r0 == 0` 时进入上述退出序列。上游 `0x23D58E` 调用 `R9+0x64` 的窄字符串长度函数，结果保存在栈上并传给这项非零检查。
- `0x23D538` 是字符串转换/文本布局函数；冻结时传入的字符串长度为 0，随后每次 timer callback 都在同一检查处主动 abort。当前待确认的是空字符串由哪个战斗对象/资源产生，以及模拟器哪项通用 ABI/桥接语义使其出现。

## 2026-07-12 根因确认

- 唯一上游调用链：
  ```text
  0x227FE6
    -> 0x228458  文本控件构造器
    -> 0x23D538  文本转换/分行
    -> 0x23BCCC  mr_platEx(MR_UCS2GB=1207)
    -> table[38]
    -> 0x23D58E  table[15]/strlen
    -> 0x23D5BC  长度非零检查
    -> 0x226148  raise
    -> 0x226320  semihost SYS_EXIT
  ```
- `res_lang0.rc` 解压后 6716 字节；运行时用到的资源文本不是空字符串。例如 ID 154 是 UCS-2BE 的“奉天承运，皇帝诏曰…”，因此已排除资源缺失/损坏。
- `game.ext:0x23BCCC` 从 literal `0x23BD28` 取 `0x4B7`，即 `MR_UCS2GB=1207`，把调用方预分配缓冲及长度指针交给 `table[38]`。
- `src/mythroad/dsm.c` 的既有 ABI 在 `*output` 非空时原地写入转换结果，但不更新 `*output_len`。这是调用方管理缓冲区的有效路径，`output_len` 保持 0 不代表输出为空。
- `aex_t038` 原实现仅在 `host_output && host_output_len > 0` 时保留/复制输出，否则将 guest 输出地址写成 0。因此原地转换虽已成功，返回 guest 前其有效指针仍被错误清零；后续 `strlen(NULL)` 结果为 0 并触发退出。
- 窄 PC watch 在修复后确认：布局入口文本地址非空，转换后缓冲为 `0x24D654`，`strlen` 返回 `0x1E`，`0x23D5BC` 的非零检查通过。

## 2026-07-12 修复与目标验证

- 通用修复位于 `src/arm_ext/aex_table.c`：`table[38]` 通过 host 输出指针与原 guest 映射的指针身份判断原地写入；两者相同时保留原 ARM 地址，不再把 `output_len=0` 误判成空输出。代码不包含应用名、MRP 名或场景地址特判。
- 目标测试同时强化最终验收：`new-plant.ppm` 轮询“获得新植物”界面独有的绿色外框与浅绿标题背景，继续使用 20 秒 timeout，避免用普通战斗画面的偶然同色像素作为成功证据。
- 构建：`cmake --build build --target skyengine -j2` 通过。
- 原测试逻辑下首次验证：`play.test.ts` 通过，约 60.8 秒；stdout 299 字节，无 `UC_ERR_EXCEPTION`/`UC_ERR`。
- 强化 PPM 条件后再次验证：`play.test.ts` 通过，约 59.8 秒；保留目录 `/tmp/skyengine-e2e-9nhlhS`。
- `study-end.ppm` 时间为 `12:03:17.896`，`new-plant.ppm` 时间为 `12:03:19.404`，确认后约 1.5 秒显示新植物，满足 20 秒要求。
- `new-plant.ppm`：`240x320`，28 种颜色，SHA-256 `9232a38618d36814a3daa94dd10afe33c0e353aeacacdddfc866601d91a3dae8`；画面明确显示“获得新植物 / 向日葵”。
- 同应用兼容验证：`pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts --reporter=verbose` 通过，2 个用例（教程开始、花屏检查）全部通过。
- 公共桥接抽查：`gxdzc-pixel.test.ts` 与 `opbzqe/game-prepare.test.ts` 同批通过，2 个文件/2 个用例全部通过。
- 最终按原始命令 `pnpm vitest run test/e2e/gzwdzjs/play.test.ts` 复验通过，1 个文件/1 个用例通过，约 59.8 秒。

## 清单状态

- [x] 无 xvfb 复现目标用例，保留 stdout/stderr 和关键 PPM。
- [x] 确定最后推进路径与重复退出现场。
- [x] 对照 `game.ext` 反汇编还原文本转换、桥接与退出调用链。
- [x] 实施有 ABI 依据、无应用特判且带注释的通用修复。
- [x] 以目标用例、20 秒窗口和 PPM 内容验证“获得新植物”。
