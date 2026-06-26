# gms 付费成功耗时分析

## 任务约束

- 目标用例：`pnpm vitest run test/e2e/gms/game-prepare.test.ts`
- 不使用 `xvfb`，使用 SDL dummy/e2e socket。
- trace 日志量很大，优先使用窄口径日志、PPM 截屏和反汇编定位。
- 不写应用特定分支，例如 `if (is_xxx_app())`。
- 过程记录定期保存到本文档。

## 2026-06-26 初始代码阅读

- `test/e2e/gms/game-prepare.test.ts` 中 “确定付费” 在第 72-79 行：
  - `LEFT_SOFT` 发送后固定等待 `1s`；
  - 截屏验证仍停留在付费确认 UI 的颜色点 `[248, 80, 0]`；
  - 随后固定等待 `5s`，没有在测试里等待“付费成功”文本或成功画面。
- `test/e2e/vmrp-e2e.ts` 中：
  - `key()` 会先读 `DRAW_COUNT`，发送 `KEY`，再调用 `WAIT_DRAW previous timeoutMs`；
  - 当前 gms 用例大多数 `key(..., 1000)` 最多只等下一帧 1 秒；
  - `delay()` 是纯 sleep，不检查状态；
  - `screen()` 通过 e2e socket `SCREEN path` 立即 dump PPM。
- 因此“付费后到付费成功等待很久”的来源大概率不在测试断言等待，而在运行时支付插件的异步流程：按键确认后仍需由 timer/event/network/SMS 相关回调推动状态变化。

## 2026-06-26 基线结果

- 命令：`env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gms/game-prepare.test.ts`
- 结果：通过，Vitest 报告该测试耗时约 `62.18s`。
- 这个总耗时与测试脚本中的固定等待基本吻合：
  - 启动后固定等待 `4s`；
  - 前置剧情 `28` 次 `ENTER`，每次 e2e `KEY` 默认按住约 `500ms`，再固定 `1s`，仅这段约 `42s`；
  - 其它菜单/商店/付费步骤各有多次 `KEY + 1s delay`；
  - 确认付费后额外固定 `1s + 5s`。
- 保留的 stdout 显示确认付费后进入短信支付模拟路径：
  - `mr_sendSms(10668001)`
  - 随后写入 `mythroad/gms/ms.paydate`
- 因此当前“慢”至少分两类：
  1. 测试驱动慢：大量固定 sleep 和默认 `500ms` 按键保持时间；
  2. 支付业务状态慢：需要通过 PPM 轮询确认画面何时离开确认界面或出现成功状态。

## 2026-06-26 子 Agent 只读结论

- e2e 层确认：`vmrp.key(name, 1000)` 展开为 `DRAW_COUNT -> KEY -> WAIT_DRAW previous 1000`。
- `KEY` 控制线程默认 `SDL_KEYDOWN -> SDL_Delay(500ms) -> SDL_KEYUP`，默认保持时间来自 `VMRP_E2E_HOLD_MS`。
- `WAIT_DRAW` 只等任意一次 `guiDrawBitmap()`，不是等“付费成功”。
- `SCREEN` 需要回主线程 dump PPM，因此若主线程被 ARM/timer 处理占住，截图命令本身可能等待，但它也不是成功条件。
- 默认构建未启用真实 `NETWORK_SUPPORT`，stdout 又显示 `mr_sendSms`，当前 gms 支付路径优先按 SMS/本地支付状态机分析。

## 2026-06-26 PPM 轮询结果

- 临时探针在不使用 `xvfb`、不打开全量 trace 的前提下，走到付费确认界面后记录 PPM 和 `DRAW_COUNT`。
- 关键时间点：
  - `05-pay-confirm`：`t=55613ms`，`draw=905`，hash `30c57cb201bc`，关键像素为 `[248, 80, 0]`。
  - 确认付费 `LEFT_SOFT` 本身耗时约 `501ms`，与 e2e 默认按键保持时间一致。
  - `06-pay-plus-1s`：`t=57121ms`，`draw=930`，hash `cbfaf1955066`，关键像素仍为 `[248, 80, 0]`。
  - 后续每 `500ms` 轮询到 `t=72325ms`，`draw` 从 `930` 增至 `1182`，hash 一直保持 `cbfaf1955066`，关键像素也一直是 `[248, 80, 0]`。
- 结论：
  - 主线程、timer 和绘制都在继续运行，不是 `SCREEN` 或 `WAIT_DRAW` 卡住。
  - 确认付费后，画面至少 `16s` 没有离开付费确认 UI，问题更像支付/SMS 状态机没有收到某个异步完成条件。

## 2026-06-26 反汇编定位

- `test/fixtures/gms.mrp` 的脚本层 `start.mr` 会加载自身 `cfunction.ext`，随后通过 `_strCom(801, ...)` 把初始化、事件和 timer 转发给 ARM EXT。
- `dealevent(code,param0,param1,param2)` 会打包事件并调用 `_strCom(801, packed_event, 1)`；`dealtimer()` 调用 `_strCom(801, "", 2)`。
- `cfunction.ext` 以 `EXT_CODE_ADDR=0x00E80000` 映射，loader 从 `0x00E80008` 进入。
- ARM 反汇编定位到 `_strCom(801, ..., code)` 的分发入口：
  - `0x00EA45E8`：分发函数入口。
  - `0x00EA4660`：`code=1` 事件分支，加载打包事件参数。
  - `0x00EA4664`：调用 `0x00EA4238`，这是确认付费按键事件会进入的事件/付费处理函数。
  - `0x00EA4680`：`code=2` timer 分支，调用 `0x00EA70B0`。
- `cfunction.ext` 自身包含完整支付/SMS 逻辑字符串，包含：
  - `mrc_smsConfirm.`
  - `LiBoOK`
  - `reConfirm_smsCheck:*`
  - `reConfirm_nextSms:*`
  - `mrc_pushSms Success!`
  - `Return Success...`
  - `gms\\ms.paydate`
- `gms.mrp` 的 `start.mr` / `cfunction.ext` 字符串中没有发现直接引用 `plugins/netpay.mrp`、`netpay.ext`、`smsend.ext` 的证据。当前确认付费路径应优先分析 gms 自身 `cfunction.ext`。
- `cfunction.ext` 初始化阶段明确调用 `timerStart(100)`，即主支付状态机按 `100ms` timer 驱动，不是单纯等待 5 秒或 30 秒 timer。

## 2026-06-26 当前假设

- 确认付费后 stdout 显示：
  - `mr_sendSms(10668001)`
  - `ChrageExOverSea Ret -- 0`
  - 写入 `mythroad/gms/ms.paydate`
- 平台层 `mr_sendSms()` 当前只同步返回 `MR_SUCCESS`，没有投递 `MR_SMS_RESULT` 事件。
- Mythroad/DSM 层存在 SMS 相关语义：
  - `MR_SMS_RESULT = 9`
  - `MR_SMS_RESULT_FLAG = 16`
  - `_com(306, value)` 设置 `mr_sms_return_flag=1` 与 `mr_sms_return_val=value`
  - `_com(307, ...)` 清除该 flag
  - `_strCom(701, ...)` 进入 `mr_smsIndiaction()`
- ARM EXT 桥接层 `table[59]` 调用 `mr_sendSms()` 后直接返回。
- `table[139]` / `table[140]` 是独立 shadow slot，原生表语义上对应 `mr_sms_return_flag` / `mr_sms_return_val`，但这两个槽用于 `mr_smsIndiaction()` 期间覆盖返回值，不是短信发送完成的主通知机制。

## 2026-06-26 窄 SMS 运行时诊断

- 临时启用 `VMRP_SMS_DIAG=1`，只观察 `table[59]` 以及 `table[139]/[140]` 两个 4 字节槽，不启用全量 `VMRP_ARM_EXT_TRACE`。
- 命令：`env VMRP_E2E_KEEP_TMP=1 VMRP_SMS_DIAG=1 pnpm vitest run test/e2e/gms/game-prepare.test.ts`
- 结果：通过，Vitest 报告测试耗时约 `62.61s`，未明显被诊断拖慢。
- 保留临时目录：`/tmp/vmrp-e2e-mcwvqE`。
- 关键日志：
  - `Chn -- 0 Cost -- 20 Mcc -- 460`
  - `SMS_DIAG table[59] pre lr=0xE9A3F4 num=10668001 encode=24 flagSlot=0x200060 flag=0 valSlot=0x200064 val=0`
  - `mr_sendSms(10668001)`
  - `SMS_DIAG table[59] post ret=0 flag=0 val=0`
  - `ChrageExOverSea Ret -- 0`
  - 写入 `mythroad/gms/ms.paydate`
- 解释：
  - `encode=24` 等于 `MR_SMS_REPORT_FLAG(8) | MR_SMS_RESULT_FLAG(16)`，gms 明确要求平台给发送结果。
  - `lr=0xE9A3F4` 是 ARM EXT 调用 `table[59]` 后返回地址，对应 `gms cfunction.ext` 的文件偏移 `0x1A3F4` 附近，后续反汇编应围绕这个点。
  - 整个用例期间没有出现对 `table[139]/[140]` 的读写日志；因此 gms 此路径不是在轮询这两个短信返回槽。单纯同步 `table[139]/[140]` 不能解释或优化当前现象。
  - `mr_sendSms()` 返回后，gms 立即打印 `ChrageExOverSea Ret -- 0` 并写 `ms.paydate`，说明发送 API 同步返回被当作成功处理了一部分状态；但 PPM 仍显示确认 UI 长时间不变，剩余停留更可能由支付 UI 状态机、成功事件或关闭流程决定。

## 2026-06-26 `mr_sendSms` 调用点反汇编

- 运行时 `table[59]` 的 LR 是 `0xE9A3F4`，对应 `gms cfunction.ext` 文件偏移 `0x1A3F4`。
- `0xE9A1CC` 附近是包含短信发送的支付处理函数。关键片段：
  - `0xE9A3E0`：计算短信配置/通道条目地址，`r0 = table_entry + 12`。
  - `0xE9A3E8`：`r1 = r6`，短信内容或目标缓冲。
  - `0xE9A3EC`：`r2 = #24`，即 `MR_SMS_REPORT_FLAG | MR_SMS_RESULT_FLAG`。
  - `0xE9A3F0`：`bl 0xEA15CC`。
  - `0xE9A3F4`：返回点，把 `mr_sendSms()` 返回值存入 `r7`。
  - `0xE9A3F8`：若返回 `-1` 走失败分支。
  - `0xE9A400` 起：成功分支清 `[base+0x640]`，随后调用 `0xEA5C5C` 继续处理。
- `0xEA15CC` 是一个通用长度检查后跳平台表项的 wrapper：
  - 检查号码长度 `1..32`；
  - 检查内容长度 `<=140`；
  - 从 helper table 偏移 `0xEC` 取函数指针，即 `table[59]`；
  - 通过 `bx ip` 进入平台 `mr_sendSms()`。
- `0xEA5C5C` 会根据状态索引清一段 `0x94` 字节结构，并把 `r5/r7/r8` 参数写入状态结构；它更像支付记录/状态推进函数，不是阻塞等待点。
- 这条证据链确认：慢点不在 bridge 调用本身，`mr_sendSms()` 已经同步返回成功；如果后续 UI 不动，要么状态机需要额外 SMS result 事件，要么它本来就停留在确认层并依赖测试之外的关闭/轮询逻辑。

## 2026-06-26 `MR_SMS_RESULT` 试验

- 为验证“缺 SMS result 事件”是否是画面停留原因，临时做了通用语义试验：
  - `mr_sendSms()` 保持同步返回 `MR_SUCCESS`；
  - 当 flags 带 `MR_SMS_RESULT_FLAG` 时，仅在 `VMRP_SMS_RESULT_EXPERIMENT=1` 下排一个 pending result；
  - 在下一次 timer 安全点投递 `mr_event(MR_SMS_RESULT, MR_SUCCESS, 0)`，避免在 `table[59]` 调用栈内重入 ARM EXT。
- 运行命令：
  - `env VMRP_E2E_KEEP_TMP=1 VMRP_SMS_RESULT_EXPERIMENT=1 VMRP_SMS_DIAG=1 pnpm vitest run test/e2e/gms/payment-latency-probe.test.ts`
- 保留临时目录：`/tmp/vmrp-e2e-5wCnX9`。
- stdout 证明 result 事件已经实际投递：
  - `SMS_DIAG table[59] pre lr=0xE9A3F4 num=10668001 encode=24 ...`
  - `mr_sendSms(10668001)`
  - `ChrageExOverSea Ret -- 0`
  - `mr_dispatchSmsResult(0)`
  - result 事件后再次写入 `mythroad/gms/ms.paydate`
- PPM 结果：
  - `confirm-before`：`t=55568ms`，`draw=907`，hash `e6dc2f196b44`，像素 `[248,80,0]`。
  - `confirm-plus-1s`：`t=57078ms`，`draw=932`，hash `dd716938a16f`，像素 `[248,80,0]`。
  - `poll-00` 到 `poll-29`：`t=57584ms` 到 `72293ms`，`draw=940` 到 `1184`，hash 始终是 `dd716938a16f`，像素始终 `[248,80,0]`。
- 结论：
  - gms 的 `MR_SMS_RESULT` 事件处理路径存在，且事件投递能触发状态文件写入。
  - 初始判断误把 hash `dd716938a16f` 当成仍在确认 UI；用户确认该 hash 实际就是付费成功界面。
  - 因此这个实验反而证明：补投 `MR_SMS_RESULT` 可以把成功界面提前到确认后约 `1s`，避免原本约 `30s` 的 fallback 等待。
  - 需要保留的不是实验环境变量，而是 `MR_SMS_RESULT_FLAG` 对应的通用平台语义：`mr_sendSms()` 同步返回后异步投递一次 `MR_SMS_RESULT`。

## 2026-06-26 阶段性结论

- 目标用例本身只负责自动点击到付费步骤。后续分析重点不是优化测试 sleep，而是确认 gms 付费状态机内部为什么在确认付费后长时间保持同一 UI。
- 目标用例没有真正等待“付费成功”状态。确认付费后它只验证固定颜色点，然后固定 sleep `5s`；因此需要用更长 PPM/状态诊断补上测试没有覆盖的等待阶段。
- 用例总耗时约 `62s`，主要来自测试驱动：
  - 前置剧情 `28` 次 `ENTER` 每次约 `500ms` 按键保持 + `1000ms` 固定等待，约 `42s`；
  - 启动、菜单、商店和付费路径还有多处固定 `1000ms`；
  - 确认付费后固定 `1000ms + 5000ms`。
- PPM 证明确认付费后主线程、timer 和绘制仍在运行；当时误把 `dd716938a16f` 判断为确认 UI，后续已由用户更正为成功界面。
- 反汇编和运行时 LR 定位确认，短信发送来自 gms 自身 `cfunction.ext`：
  - `_strCom(801, event, 1)` -> `0xEA4660` -> `0xEA4238`；
  - `mr_sendSms()` 调用点在 `0xE9A3F0`，返回点 `0xE9A3F4`；
  - `0xEA15CC` 是跳 `table[59]` 的 sendSms wrapper。
- `mr_sendSms()` 已同步返回 `MR_SUCCESS`，gms 立即写入 `ms.paydate`；后续用户更正确认，补投通用 `MR_SMS_RESULT` 事件可以让 UI 更快进入成功界面。
- 当前已经排除的运行时优化点：
  - 单纯同步 `table[139]/[140]`，因为当前 gms 路径没有读写这两个槽；
  - 单纯写状态槽或改 gms 内部状态，因为这是应用特定篡改。
- 后续已按此方向执行 `90s` PPM 和聚焦 PC watch，最终判断见文末。

## 2026-06-26 方向修正

- 用户澄清：测试用例只是自动点击入口，不应把结论停在“优化测试用例”。
- 已启动新的只读子 Agent 深挖 `gms cfunction.ext` 付费状态机：
  - `0xE9A3F4`：`mr_sendSms()` 返回点；
  - `0xEA5C5C`：发送成功后的状态/记录函数；
  - `0xEA70B0`：timer 分支；
  - `0xEA4238`：事件分支，尤其 `MR_SMS_RESULT(9)`。
- 本轮继续目标：
  - PPM 轮询扩大到 `60s+`；
  - 添加通用、环境变量控制的窄诊断点，不写 gms/app 特定分支；
  - 如果定位到通用平台缺失语义或 timer 调度问题，再考虑代码优化；如果只是 gms 内部设计的长轮询，需要给出无法安全通用优化的证据。

## 2026-06-26 90 秒 PPM 长观察

- 临时探针只把测试作为自动点击入口：到确认付费后继续观察 `90s`。
- 命令：`env VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gms/payment-long-probe.test.ts`
- 结果：通过，临时目录最新为 `/tmp/vmrp-e2e-*`，该探针运行约 `147s`。
- 关键采样：
  - 确认前：`t=55996ms`，`draw=904`，hash `e6dc2f196b44`，像素 `[248,80,0]`。
  - 确认后 `1s`：`t=57507ms`，`draw=929`，hash `f36a187e1797`，像素 `[248,80,0]`。
  - 确认后 `5s/10s/15s/20s/25s`：hash 一直 `f36a187e1797`，像素 `[248,80,0]`。
  - 确认后 `30s`：hash 变为 `dd716938a16f`，像素仍 `[248,80,0]`。
  - 确认后 `35s` 到 `90s`：hash 一直 `dd716938a16f`，像素仍 `[248,80,0]`，`draw` 从 `1495` 增到 `2412`。
- 新结论：
  - gms 支付流程内部确实存在约 `30s` 的阶段变化或轮询超时点。
  - 用户确认 hash `dd716938a16f` 是付费成功界面；所以原始路径是在确认后约 `30s` 才进入成功界面。
  - 下一步应围绕 25-35 秒窗口做状态机诊断，尤其关注支付 timer 链表节点、`[base+0x640]` 记录槽计数，以及 `0xEA5C5C` 写入的两个状态槽。

## 2026-06-26 聚焦 PC watch 结果

- 为避免全量 trace，只临时加了环境变量控制的 PC watch，观察已知支付 PC 和少量状态槽。该诊断包含 `base = r9 + 0xE24` 的 gms 状态偏移假设，采集完成后已从 `src/arm_ext_executor.c` 移除，不作为通用产品逻辑保留。
- 命令：
  - `env VMRP_E2E_KEEP_TMP=1 VMRP_ARM_EXT_WATCH_PC='0xEA207C,0xEA8044,0xEA8124,0xEA8378,0xEA8400,0xEA8584,0xE9A3F4,0xEA5C5C,0xEA4238' pnpm vitest run test/e2e/gms/payment-long-probe.test.ts`
- 结果：通过，临时目录 `/tmp/vmrp-e2e-OSMe5z`，stdout 只有 `576` 行，没有打开全量 trace。
- PC 命中计数：
  - `0xEA4238`: `70`
  - `0xEA8378`: `70`
  - `0xEA207C`: `1`
  - `0xE9A3F4`: `1`
  - `0xEA5C5C`: `1`
  - `0xEA8044` / `0xEA8124` / `0xEA8400` / `0xEA8584`: `0`
- 确认付费附近的实际顺序：
  - `mr_open(mythroad/gms/ms.paydate,1)`：先读支付日期/状态文件。
  - `0xEA207C`：进入 charge / sms confirm setup，进入时 `[base+0x588]=0`、`[base+0x640]=0`、`[base+0x65c]=0`。
  - `mr_sendSms(10668001)`。
  - `0xE9A3F4`：`mr_sendSms()` 返回点，`r0=0`，即平台同步返回 `MR_SUCCESS`；此时 `[base+0x588]=1`、`[base+0x58c]=0`、`[base+0x640]=0`、`[base+0x65c]=0`。
  - `0xEA5C5C`：发送成功后的状态记录函数，状态槽仍为 `[0x588]=1`、`[0x58c]=0`、`[0x640]=0`、`[0x65c]=0`。
  - `ChrageExOverSea Ret -- 0`。
  - `mr_open(mythroad/gms/ms.paydate,12)`：写状态文件。
  - 下一次事件分支 `0xEA4238` 已看到 `[base+0x588]=0`、`[base+0x640]=1`、`[base+0x65c]=1`，随后又写一次 `ms.paydate`。
- 解释：
  - `mr_sendSms()` 不是长等待点；调用返回后 gms 已经同步进入成功返回和文件写入。
  - 观察期内没有命中 `reConfirm_nextSms`、`reConfirm_smsCheck` 或二次确认发送 helper，说明原始 `30s` 等待不是由这些已命名的 reconfirm 分支推进出来的。
  - 30 秒 PPM hash 变化与主 SMS confirm 等待路径的 timeout/fallback 更吻合，不是平台 sendSms bridge 阻塞，也不是 `reConfirm_nextSms` 的 50ms/1000ms retry 逻辑。

## 2026-06-26 字符串 xref 与反汇编补充

- `ChrageExOverSea Ret -- %d`：
  - 字符串地址 `0xEAC4F4`，xref 在 `0xE9D170/0xE9D174`。
  - 所在函数约从 `0xE9CFB8` 开始，先调用 `0xE90C24` 读 `gms\\ms.paydate`，再调用 `0xEA695C` / `mrc_smsConfirm`，返回为 `0` 时做计数并调用 `0xE9B2EC` 写 `ms.paydate`。
- `mrc_smsConfirm.`：
  - 字符串地址 `0xEAC374`，xref 在 `0xEA69E8/0xEA69F4`。
  - 函数约从 `0xEA69E4` 开始，wrapper 在 `0xEA695C`。
  - 关键行为：检查 `[state+0x701]` 开关；拒绝冲突 active state；调用 `0xEA2078/0xEA207C`；存储确认参数到 `[state+0x6DC]`、`[0x64C]`、`[0x650]`、`[0x648]`；停止 `[0x580]` 和 `[0x654]` timer；后续通过 `0xE83024` 驱动等待/进度。
- `reConfirm_nextSms`：
  - 函数入口 `0xEA8044`。
  - xref 字符串包括：
    - `0xEA8064/0xEA8068`: `reConfirm_nextSms:total==nTx发送完毕。`
    - `0xEA808C/0xEA8090`: `reConfirm_nextSms:发送SP短信失败!重发`
    - `0xEA80D0/0xEA80D4`: `reConfirm_nextSms:Submit成功!`
  - 逻辑比较 `[state+0x588]` total 与 `[state+0x58C]` submitted；未完成时根据 helper `0xEA908A4(arg)` 结果重试或递增 submitted；使用 `[state+0x584]` 的 `1000` 单位延迟。
- `reConfirm_smsCheck`：
  - 函数入口 `0xEA8400`，主要日志点在 `0xEA8584` 之后。
  - 检查 `[state+0x6BF]` total reply 与 `[state+0x6C0]` current reply/send count。
  - 若需要后续自动二次确认，会停止 `[state+0x580]` 并以 `50` 单位 delay 调度 `0xEA8124`；`0xEA8124` 失败重试也使用 `1000` 单位 delay。
- 30 秒值：
  - 二进制里确实有一个 `30000` / `0x7530` literal，在 `0xEA5514` 附近初始化并写入 `[state+0x650]`。
  - 该字段会被 `mrc_smsConfirm` 路径复制/使用，但没有证据表明它直接属于 `reConfirm_nextSms` 或 `reConfirm_smsCheck`。
  - 因此 30 秒 PPM 阶段变化更可能来自主 SMS confirm 等待/进度路径的 timeout 参数，而不是平台缺少 SMS 发送结果事件。

## 2026-06-26 PPM 复核

- 从 `/tmp/vmrp-e2e-OSMe5z` 保留的 PPM 文件重算 hash，关键像素均为 `[248,80,0]`：

| 时间点 | PPM hash 前缀 | `pixel(61,119)` |
| --- | --- | --- |
| 确认前 | `e6dc2f196b44` | `[248,80,0]` |
| 确认后 1s | `f36a187e1797` | `[248,80,0]` |
| 确认后 5s | `f36a187e1797` | `[248,80,0]` |
| 确认后 10s | `f36a187e1797` | `[248,80,0]` |
| 确认后 15s | `f36a187e1797` | `[248,80,0]` |
| 确认后 20s | `f36a187e1797` | `[248,80,0]` |
| 确认后 25s | `f36a187e1797` | `[248,80,0]` |
| 确认后 30s | `dd716938a16f` | `[248,80,0]` |
| 确认后 35s | `dd716938a16f` | `[248,80,0]` |
| 确认后 60s | `dd716938a16f` | `[248,80,0]` |
| 确认后 90s | `dd716938a16f` | `[248,80,0]` |

- 复核结论：
  - 30 秒附近确实有可见画面 hash 变化。
  - 用户确认 `dd716938a16f` 是付费成功界面；关键像素 `[248,80,0]` 不能单独区分确认界面和成功界面。
  - 画面持续绘制，故不是主循环停死或截图 IPC 卡死。

## 2026-06-26 用户更正与通用修复

- 用户确认：`dd716938a16f` 是付费成功界面，不是仍在确认界面。
- 这会改变最终判断：
  - 原始路径不是“90 秒还没成功”，而是“确认付费后约 `30s` 才成功”。
  - `MR_SMS_RESULT` 实验中确认后 `1s` 变成 `dd716938a16f`，实际已经证明补投短信结果事件能把成功提前。
  - gms 发送 SMS 时 `encode=24`，即 `MR_SMS_REPORT_FLAG(8) | MR_SMS_RESULT_FLAG(16)`，明确要求平台返回短信发送结果事件。
- 已实现的通用修复：
  - 在 ARM EXT `table[59] mr_sendSms(pNumber, pContent, flags)` 中，如果宿主 `mr_sendSms()` 返回 `MR_SUCCESS` 且 `flags` 带 `MR_SMS_RESULT_FLAG`，在 `ArmExtModule` 上排一个 pending SMS result。
  - 在 ARM EXT 调用返回到宿主安全点后，用普通 `code=1` 事件投递 `MR_SMS_RESULT`，`param1=MR_SUCCESS`、`param2=0`。
  - 该实现不在 `mr_sendSms()` 调用栈内重入 ARM 代码，也不判断 gms 名称、不写 gms 状态槽、不修改 `30000` 常量。
- 修复后验证：
  - 命令：`pnpm vitest run test/e2e/gms/payment-long-probe.test.ts`
  - 结果：通过，耗时约 `147.27s`。
  - 关键采样：
    - 确认前：`t=55718ms`，`draw=903`，hash `e6dc2f196b44`，像素 `[248,80,0]`。
    - 确认后 `1s`：`t=57228ms`，`draw=928`，hash `dd716938a16f`，像素 `[248,80,0]`。
    - 确认后 `5s/10s/.../90s`：hash 持续 `dd716938a16f`。
- 最终解释：
  - 慢点不是 `mr_sendSms()` 本身，它同步返回成功。
  - 慢点是平台缺少 `MR_SMS_RESULT_FLAG` 对应的异步 `MR_SMS_RESULT` 事件，导致 gms 主 SMS confirm 流程等待内部约 `30s` fallback/timeout 才进入成功界面。
  - 通用补齐 SMS result 语义后，成功界面从确认后约 `30s` 提前到确认后 `1s` 采样点内。
