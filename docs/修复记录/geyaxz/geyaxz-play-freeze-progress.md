# geyaxz 游玩失败菜单后卡死修复进度

## 目标与约束

- 目标用例:`pnpm vitest run test/e2e/geyaxz/play.test.ts`。
- 失败菜单(您失败了!重玩本关/选择关卡)按 DOWN+ENTER 后必须离开当前画面。
- SDL dummy、无 xvfb、无全量 trace;修复不得含应用/按键/像素分支与兜底逻辑。

## 2026-07-18 复现与表象

- 复现:选择菜单项后画面回到关卡(剩余步数 0)且整机失去响应;
  最后一个 waitFor 超时,断言 `(32,229)==[96,84,144]`(仍是关卡画面)。
- DIAG 运行:事件全程正常进入 game(+0x1367C/+0x12A0C 命中),失败菜单
  导航正常;最后一次 wrapper 活动是 `0x1504`(调度 10ms 后执行 fnC60
  延迟关闭)与 `table31_pre t=10`,此后**整个进程再无输出**;期间正在
  反复读 `plugins\netpay.mrp`(计费插件加载轮询)。

## 2026-07-18 定位:guest 死循环

- WSL 下 core 被吞、yama 禁 attach → 用 `VMRP_BIN` 包装脚本让 skyengine 生为
  gdb 子进程,卡死时 SIGINT 采样。
- 主线程栈:卡在 unicorn 内执行 wrapper 代码;12 次 PC/LR 采样全部落在
  `0xE80D28`(延迟命令队列排空循环)、`0xE81254`(命令入口)、
  `0xE80DE4`(按 id 查订阅者)、`0xE818F2`(首参≠1 的消息转发)。
- gdb 直读 guest 内存:队列 {head,tail}=RW+0x2C/0x30 = {0x270D, 0x0}——
  head 是小整数而非堆指针,排空循环在低内存垃圾链上永久循环,且首次入队
  时 `*tail` 已把节点指针写进 EXT_TABLE 槽(tail 曾为 0x10068=table[26])。

## 2026-07-18 根因:staging 窗口 R9 语义分歧(与 boot bug 同源)

- 内存写观察:{head,tail} 在启动期被写成 {0x270D, 0x10068},写入 PC 在
  0x40000000 arena(graphics.ext file+0x396/+0x33A),r9=0x225E60(wrapper
  的 RW)。boot bug 的 selected=1(RW+0x1C)是同一次 init 的另一处写入。
- graphics.ext 先装入主内存 0x25B95C,后被私有 loader 迁入 arena
  0x40018800(record 0x2BD04C 复用,P=0x2BD0AC);其 helper veneer
  (file+0x17A8)语义是 r9=[P][0],且 P[0] 已被 arena entry 正确写为
  0x25DA90。
- 逐块追踪 hook_restore_r9:fn 0x2E8 入口 r9=0x25DA90(正确)→ init 内
  `blx r5` 进 wrapper 命令(cmd 1,0x64)→ 宿主在 wrapper 代码入口把 r9
  切为 wrapper rw(因旧实例已注册且 rw 相同)→ 返回 arena 代码时新基址
  **尚未注册**(mr_cacheSync 已过、extChunk[8] 未确认),registry 查不到、
  无法恢复 → init 余下部分以 wrapper R9 运行,按固定偏移把自己的初始化
  数据写进 wrapper RW。
- **完整修复已实现并验证可消除腐蚀**(staging 窗口按 child P[0] 恢复 R9 /
  位移记账两种变体):但 graphics/SKY 推广计费框架随即真正跑通启动流程
  (POST rop.skymobiapp.com/payOneAsTlv → 200 OK → 写 system/ntp/termsync、
  gbreg.sys),game 每帧 present 空帧等待一个 skyengine 未模拟的完成信号,
  boot 黑屏 >10s。当前全部通过的用例都建立在该框架瘫痪的基线上,完整
  R9 修复需先补齐该链路,回退。

## 2026-07-18 落地方案:结构性不变量修复(与第一轮同类)

- `arm_ext_restore_wrapper_command_queue`:head/tail **同时**不是合法堆
  指针(空态 {0,&head} 或可映射堆指针)时恢复 canonical empty;在每次
  wrapper arm_ext_call 入口执行,先于首次入队,避免 table 槽被写坏。
- `arm_ext_restore_drained_event_layer` 恢复(boot 修复),注释重写为
  已证实的腐蚀机制;两者共用同一入口点,无应用分支。
- `mythroad.c` FULL runtime Lua dealevent 单入口改动不受影响,保留。
- R9 完整修复方向、staging 窗口机制与推广框架链路缺口已记入本文档,
  作为后续工作。

## 验证

- geyaxz boot 5.43s 通过;play 92.49s 通过(失败菜单 → 选择关卡正常)。
- 全量 e2e 回归见下节。

## 2026-07-18 最终验证矩阵(SDL dummy,无 xvfb,无全量 trace)

- `cmake --build build --target skyengine`:通过;
- `ctest --test-dir build`:1/1 通过;`pnpm exec tsc --noEmit`:通过;
- geyaxz boot 5.43s、play 92.49s:通过;
- `pnpm vitest run test/e2e --retry=0`:**27 文件 / 49 测试全部通过**,
  278.80s(含当时的 play 用例;此处为早期快照);
- 全部临时实验代码(RW 写观察、arena 块追踪、R9 追踪、gdb 包装)已删除;
  保留的通用诊断增强:`VMRP_ARM_EXT_WATCH_PC` 覆盖 wrapper 本体、
  观察点输出含 r9。

## 2026-07-18 追加:无 plugins 时 boot 卡在声音选择(boot-to-home -t 2)

- 用例 2 与 1 唯一差异:启动前删除 `mythroad/plugins`(无 netpay.mrp)。
- 现场:cmd_queue_restore 正常触发,但 event_layer_restore 全程未触发,
  3 个按键事件全被 IGNORE。
- 拒绝原因(定点 dump):selected=1 而层 1 是 **{0,0}**——无插件时推广
  overlay 从未注册进层 1,该层保持数据段零初始化原样,不满足原谓词要求
  的 canonical empty {0,&head}。
- 结构性论证:真实 push(+0xBA4 delta=+1)必先把层初始化为 {0,&head},
  合法 selected 层不可能是 {0,0};该状态只能来自 selected 被直接踩踏
  (staging R9 腐蚀)。故把 {0,0} 与 {0,&head} 同等视为空态纳入收缩谓词
  (`tail != head_addr && tail != 0` 才算非空)。
- 验证:boot 用例 1(5.50s)、用例 2(5.40s)、play(92.48s)全部通过;
  全量 e2e 回归见下。
- `pnpm vitest run test/e2e --retry=0`:**27 文件 / 50 测试全部通过**,
  274.01s(含 boot 用例 1/2 与 play)。

## 2026-07-18 重新审计:用例绿但页面判定不充分

- 用户现场确认无 plugins 时完成第一关后实际进入菜单,并未进入插件下载页。
- 审计发现用例 2 的最终断言只有 `(11,106)==[0,0,0]`;该像素在下载页与
  菜单页都可能为黑色,所以此前 50/50 通过只能证明执行未超时,不能证明目标
  页面正确。
- 当前修复只恢复了被 staging R9 腐蚀的事件层/命令队列结构不变量,尚未解释
  guest 为何选择菜单分支。下一步先保存下载页与菜单页 PPM 并提取区分像素,
  再对第一关结束后的插件存在性检查和分支目标做定点反汇编/运行时取证。
- Ralph 上下文快照:`.omx/context/geyaxz-missing-plugin-download-20260718T152321Z.md`。

## 2026-07-18 PPM 失败基线

- 复现命令:`VMRP_E2E_KEEP_TMP=1 pnpm vitest run
  test/e2e/geyaxz/boot-to-home.test.ts -t 2 --reporter=verbose`;默认 retry
  共 4 次均失败,最终 `(11,106)=[192,132,24]`,总耗时 151.45s。
- 四张最终 PPM 均为 240x320 主菜单,约 801 种颜色;它们之间只有鳄鱼菜单
  动画造成的 359--536 像素差异。真实通用插件下载提示页参考 PPM 只有 5 种
  颜色,两类画面整帧相差 76421/76800 像素。
- 稳定区分点:菜单 `(80,80)=[248,220,112]`,下载页
  `(80,80)=[0,4,0]`;菜单 `(112,125)=[160,132,32]`,下载页同样为
  `[0,4,0]`。后续目标断言应同时约束页面低颜色数和正文区域像素,不能只用
  一个常见黑色像素。
- PPM 文件名 `download-plugin.ppm` 只是 `screen(name)` 的测试标签,不表示
  guest 已进入该页面。

## 2026-07-18 最终根因:wrapper 当前包上下文被 staging 写坏

- `game.ext+0xDF3C` 向 wrapper 发送 `(code=1, selector=0x20, id=2)`,
  wrapper 返回 `-1` 后 game 才跳到菜单;菜单不是 game 主动选择的成功分支。
- `cfunction.ext+0xE44` 收到正确参数
  `(2, NULL, "plugins", "verdload.ext", ...)`。其 `+0x3D4C` package-path
  构造器在第二参数为 NULL 时不使用 `plugins`,而是把 `R9+0x34` 直接复制到
  record 的 128 字节内联路径缓冲 `record+0x38`;`verdload.ext` 另存于
  `record+0xB8`。
- 失败现场 wrapper R9 正确为 `0x225E60`,但 `R9+0x34` 首字节已被 staging
  窗口写成 `0x0C`。generic loader 因此调用 table[40] 打开 `"\x0C"`,
  `mr_open` 按正常失败契约返回 0,后续 table[44] 对 fd=0 读取返回 -1。
  保留 `plugins/`、只删除 `netpay.mrp` 仍复现,进一步排除了宿主目录语义。
- broad pending-R9 修复再次验证会激活 SKY 推广/计费链并使 boot 等待未实现
  的完成信号,首屏保持黑色;该方案没有落地。

## 2026-07-18 落地修复与页面证明

- `arm_ext_sync_wrapper_package_context` 先从 wrapper path-builder 的稳定
  Thumb 指令形状发现 R9-relative 包上下文偏移(geyaxz 为 `0x34`),再在
  wrapper dispatch 边界把平台 ABI 的权威 `table[100]` 当前包名重新发布到
  该 128 字节内联缓冲。它与同源的事件层/命令队列修复共用入口,不检查
  应用、包名、按键、像素或插件文件名。
- 修复后目标路径 table[40] 重新打开 `geyaxz.mrp`,wrapper loader 成功,
  第一关结算后进入通用插件下载提示。
- 用例最终判定改为正文两点 `(80,80)`、`(112,125)` 都是 `[0,4,0]`,
  且整帧恰为 5 种颜色;不再使用下载页与菜单都可能命中的单个黑色像素。
- 已通过:`pnpm vitest run test/e2e/geyaxz/boot-to-home.test.ts -t 2
  --retry=0`(27.88s),完整 geyaxz boot 两用例与 play(92.50s)共 3/3 通过。
- 最终全量回归:`pnpm vitest run test/e2e --retry=0`:**27 文件 / 50 测试全部通过**,
  275.11s;另有 `ctest --test-dir build --output-on-failure` 1/1 通过、
  `pnpm exec tsc --noEmit` 与 `git diff --check` 通过。

## 2026-07-18 去硬编码重构:取证与设计

- 问题:`arm_ext_find_wrapper_package_context_offset` 用 18 条精确 Thumb
  半字比对匹配 path-builder 指令形状来发现 R9+0x34——本质是把一个编译
  产物的字节序列硬编码进宿主;事件层/命令队列修复也各自硬编码 wrapper
  布局偏移(0x1C/0xF4/0x2C/0x30)。三者都是同一根因的症状修复。
- 新取证(临时钩子 VMRP_AEX_STAGING_WATCH,记录"非 wrapper 本体 PC 写
  wrapper ER_RW"):-t 2 全程仅 23 笔外来写,全部来自 staged child
  (0x40018Bxx):
  - pending=1 共 22 笔:off=0x1C(=1,事件层)、off=0x30(=0x10068,队列
    tail)、off=0x34..0x74(=0x1000C..0x1004C,**child 初始化自己的 GOT
    桥表**——0x34 首字节 0x0C 之谜即 EXT_TABLE_ADDR+0xC 的低字节)、
    off=0x8C/0x90/0x94(清零)。
  - pending=0 共 1 笔:注册确认后 pc=0x40018B96 以 staging 期算出的
    **陈旧指针**写 off=0x2C(=0x270D 注册句柄,队列 head)。说明纯
    "pending 窗口 PC" 判据会漏掉注册后的余波,判据必须是"外来 PC"。
- 设计(替换全部三个症状修复):**wrapper 静态区外来写屏障**。
  - 对 wrapper ER_RW([P[0], P[0]+P[4]))挂 ranged UC_HOOK_MEM_WRITE;
  - 外来 PC(不在 EXT_CODE_ADDR..+code_len)写入时按字节记旧值+置位
    (真机上这些写全部落在 child 自己的 RW,对 wrapper 从未发生);
  - wrapper 本体 PC 写入已置位字节时清位(wrapper 后续合法写接管);
  - 在 arm_ext_call 入口、且无 pending staging 窗口时回滚置位字节
    (窗口内不能回滚:staged child 还要从借来的 R9 读回自己刚写的 GOT)。
  - 无指令签名、无布局偏移、无应用特判;语义=「wrapper 静态区只有
    wrapper 代码与宿主可持久写入」。

## 2026-07-18 去硬编码落地:wrapper RW 外来写屏障

- 实现(src/arm_ext_executor.c):`hook_wrapper_rw_foreign_write`(ranged
  UC_HOOK_MEM_WRITE,外来 PC 首触按字节记旧值+置位,wrapper 本体重写则
  清位)+ `arm_ext_wrapper_rw_journal_install`(P[0]/P[4] 首次可用时挂载)
  + `arm_ext_wrapper_rw_journal_flush`(arm_ext_call 入口、无 pending
  staging 窗口时回滚)。
- 删除的硬编码:`arm_ext_find_wrapper_package_context_offset`(18 半字
  指令签名匹配器,aex_module.c)、`arm_ext_sync_wrapper_package_context`
  (table[100] 重发布)、`arm_ext_restore_drained_event_layer`(0x1C/0xF4
  布局偏移)、`arm_ext_restore_wrapper_command_queue`(0x2C/0x30 布局
  偏移)及 `wrapper_package_context_off` 字段/声明——三个症状修复全部
  由 journal 一个机制取代,宿主不再持有任何 wrapper 数据布局知识。
- 验证:
  - `-t 2` 通过(27.8s),PPM 实检 download-plugin.ppm:240x320、恰 5 色、
    (80,80)=(112,125)=[0,4,0]——真实插件下载页;
  - geyaxz 全部 3 用例通过(boot 1/2 + play 92.4s):journal 同时接管了
    旧事件层修复(boot 1)与旧命令队列修复(play)覆盖的场景;
  - ctest 1/1、tsc --noEmit、git diff --check 通过;全量 e2e 回归见下。
- 临时取证钩子 VMRP_AEX_STAGING_WATCH 已删除。
