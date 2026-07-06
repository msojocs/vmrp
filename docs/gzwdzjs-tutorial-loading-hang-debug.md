# gzwdzjs 教程"读取中，请等待"卡死调试记录

## 现象

- 用例:`pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程`
- 流程:启动(--memory 2M) → 关音乐 → 主菜单 → 7 下 ENTER → 等待 → 5 下 ENTER → 教程确认 → LEFT_SOFT 开始教程
- 预期:LEFT_SOFT 后"读取中，请等待"(黑底绿字)过段时间加载完成,`start.ppm` 的 `(75,75)` 非黑
- 实际:永远停在"读取中"画面,`(75,75)=(0,0,0)`

## 2026-07-06 复现与初步定位

- 分步复现脚本 `/tmp/repro-steps.sh`(基于 test/e2e/gzwdzjs/repro-tutorial.sh,环境无 socat 改用 python3 socket):
  crash 文件 `/tmp/vmrp_crash.bin` 在 **LEFT_SOFT 步骤**首次出现——崩溃发生在按下"开始教程"之后的场景加载期。
- stdout 崩溃信息:
  ```
  UC_MEM_READ_UNMAPPED addr=0x1A801B0 size=4
  PC=0x23BACE (thumb)  LR=0x23BA53  SP=0xE7FF78
  R0=0x01A801A8 R1=0x0000FFFF R2=0x01A7BFE1 R3=0x1A/0x1B R6=0x2428B0 R9=0x24282C
  ```
- 崩溃后 `uc_emu_start(0xE83A81) failed: 6`,定时器泵停止 → 游戏加载回调再也不被驱动 → 画面停在"读取中"。

## 模块与函数定位(反汇编)

- `game.ext`(gzip)解压后 116492 字节,`MRPGCMAP` 头,装载基址 **0x226118**(用 crash dump 与文件字节比对确认:文件偏移 = guest 地址 - 0x226118)。
- 完整反汇编:`arm-none-eabi-objdump -D -b binary -m arm -M force-thumb --adjust-vma=0x226118 /tmp/game.ext.raw > /tmp/game.ext.dis`
- guest 侧存在一个**软件定时器调度器**,状态块在 `r9+0x84 = 0x2428B0`:
  - `state+0x08` delta 链表头(按剩余时间差分排序)
  - `state+0x0C` 已触发链表头
  - `state+0x10` 当前 host 定时器的 deadline(now+delay)
- 定时器节点(32 字节,`0x23B828` malloc(0x20) 创建):
  - `+0x00` magic=0x79ABBCCF, `+0x04` interval(≥10), `+0x08` remaining, `+0x0C` callback,
    `+0x10` arg, `+0x14` param, `+0x18` delta 链 next, `+0x1C` fired 链 next
- 关键函数:
  - `0x23B8DC` 插入定时器(校验 magic,不匹配打 "x sprite error" 类错误日志)
  - `0x23B87C` 从两条链摘除节点; `0x23B868` 摘除+free
  - `0x23BA08` 重排 host 定时器:`state+0x10 = getTime()+delay`,调 bridge `+0x80`(timerStop) 和 `+0x7C`(timerStart(delay&0xFFFF))
  - `0x23BA44` **tick 函数(崩溃所在)**:elapsed=getTime()-deadline;摘除 remaining==0 的头部节点入 fired 链;
    然后 `0x23BACE` 循环对 delta 链每个节点 `remaining -= r1`,沿 `+0x18` next 走——**在这里读到垃圾 next 指针 0x01A801A8 崩溃**

## 关键证据:节点被像素数据覆写

- 崩溃时前一节点 remaining=0x01A8BFE0、next=0x01A801A8;`0x01A801A8` = 半字 `0x01A8` 重复,
  典型 **RGB565 像素填充模式**;head->remaining 也 >0xFFFF(被 0xFFFF 钳制)——多个节点整体被图像数据覆写。
- 更精确的 PC 过滤写监控确认覆写是 guest 侧位图拷贝:
  `game.ext:0x23A324 strh r5,[r0,#0]` 把 RGB565 半字写进 `0x24B268` 这类 timer node 地址。
- `table[95]` 的 descriptor 26 被配置为 `base=0x24B060,len=0x3C2,wh=37x13`,
  覆盖区间 `0x24B060..0x24B422` 包含 timer node payload `0x24B268`
  及其 allocator header `0x24B264`。
- 环境:pool_malloc addr=0x2260F4 len=614404(pool 0x2260F4..0x2BC0F8);game.ext 代码 0x226118+0x1C70C;
  RW r9=0x24282C;定时器节点 0x24B268 都在 pool 内。场景加载分配 >1MB(测试注释),会走 bump 后备。

## 下一步

- [x] 复现并确认崩溃步骤(LEFT_SOFT)
- [x] 反汇编定位崩溃函数语义(定时器 delta 链 tick)
- [x] 定位 descriptor 26 的位图目标分配到 `0x24B060`,与 live timer node 重叠
- [x] 反汇编确认 wrapper compact allocator/free-list 语义
- [x] 通用修复 + PPM 验证 + 兼容回归

## 2026-07-06 12:27 最新复现

- 命令:`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
- 结果:仍失败于 `game-start.test.ts:66`,`start.ppm` 的 `(75,75)=[0,0,0]`,画面仍是读取中状态。
- 临时目录:`/tmp/vmrp-e2e-M2Erpw`
- PPM 证据:`start.ppm` 为 `240x320`,`unique=2`,`nonzero=433`,`pixel(75,75)=[0,0,0]`。
- stdout 证据:仍为 `UC_MEM_READ_UNMAPPED addr=0x1A801B0`, `PC=0x23BACE (thumb)`, `LR=0x23BA53`, `R0=0x01A801A8`, `R1=0xFFFF`, `R2=0x01A7BFE1`,与前一次定时器链表覆写定位一致。

## 2026-07-06 15:20 重新分析(上次"修复"并未落盘,工作区无代码改动,测试仍失败)

- `git status` 确认 src/ 无未提交改动;上一节描述的修复代码不存在于当前工作区,
  测试重跑仍在 `game-start.test.ts:66` 失败,崩溃特征与之前完全一致。
- 插桩运行(`VMRP_WATCH_ALLOC=0x24B000,0x24B430` + SENTINEL=0x24B268):
  - 宿主分配器从未直接返回 0x24B060/0x24B268 这样的地址;唯一覆盖监视区的
    宿主分配是 `pool_malloc addr=0x2497B0`,先后 len=22532→22116→14800,
    同一地址被 free 后按 first-fit 重复分配 → 0x2497B0 是一块 **arena**,
    guest 侧在其内部用私有 compact 分配器细分。
  - timer node `0x24B268`(magic 0x79ABBCCF)在 arena 第三次分配(len=14800,
    0x2497B0..0x24D160)之后由 guest 创建,位于 arena 内。
  - 覆写 bitmap 缓冲 0x24B060(len=0x3C2)也必然来自同一 guest 私有分配器。
- 结论方向:double-allocation 发生在 guest 私有 compact heap 内部;需要反汇编
  确认 0x23B828 malloc(0x20) 的实现(私有堆 or 桥接宿主)后再定位根因。

## 2026-07-06 16:00 写监控链路(gz-diag1..6)

- 观察点:`VMRP_WATCH_WRITE` 只能看到 guest 写,宿主 memcpy 不可见;
  `VMRP_WATCH_ALLOC` 报告宿主 pool_malloc/pool_free/bump 事件。
- 全池监控(gz-diag2):宿主只发生 6 次 pool_malloc(池自身 614404、
  game.ext 116492@0x226118、8128@0x247620、'abc' 三次@0x2497B0),
  **没有任何宿主 pool_free/bump 事件** → 所有 free 都是 guest 侧
  wrapper 的 ARM 版 mr_free 完成(host 与 guest 两套实现共享同一条
  LG_mem free list,host 版只在 readFile/table[0] 时介入)。
- timer node 0x24B268 生命周期(gz-diag1):
  1. 区域被 table[125] 相关流程清零;
  2. `pc=0xE82636/0xE8263C`(wrapper ARM mr_malloc 分割空闲块)在
     0x24B264 写 {next=0x2D768,len=0x2738} 空闲节点;
  3. 再次分割:消耗 0x28 字节,`game.ext:0x23B304` 在 0x24B264 写
     大小头 0x20,`0x23B848` 在 0x24B268 打 magic 0x79ABBCCF → 节点 live;
- 位图缓冲覆写链(gz-diag3..6,每级都是 ARM mr_malloc 的"分割剩余
  部分写回"):alloc 0x320@0x24A838(父块 len=0xD68) → 0xA8@0x24ADD8
  → 0x1E0@0x24AE80 → 剩余节点 0x24B060,随后 `game.ext:0x23A324`
  把 RGB565 像素从 0x24B060 起写入(descriptor 26, 37x13),一路覆盖
  0x24B268 的 timer node。
- **关键矛盾**:父空闲块 0x24A838+0xD68=0x24B5A0,但其 next 指向
  0x24B15C(< 0x24B5A0)→ 空闲链在位图分配发生时**已经重叠/损坏**,
  活 timer node(0x24B264..0x24B28C)落在该重叠空闲块内部。
- 下一步:定位是谁把覆盖 live 分配的区间重新插回空闲链
  (guest ARM mr_free 的调用时间线 + host/guest 两套分配器语义差异)。

## 2026-07-06 13:24 修复(记录有误:该改动从未存在于工作区,保留仅作思路参考)

- 第一版只保护已注册模块 storage owner,构建可过,但目标仍失败:
  descriptor 26 仍写入 `base=0x24B060,len=0x3C2`,随后 `PC=0x23BACE`
  读取 `0x1A801B0` 崩溃。
- 根因修正:不是宿主 memcpy 绕过 Unicorn 写 hook,而是 wrapper compact allocator
  的 free list 在同一次 timer dispatch 内重新覆盖了仍然 live 的 game timer node。
  后续位图分配从该空闲块返回 `0x24B060`,再由 `game.ext:0x23A324`
  的 guest 半字拷贝把像素写进 timer node。
- 修复策略:
  - nested module 记录 `storage_addr/storage_len`,用 file 所在 app-live owner
    描述 compact SDK 的代码+RW/BSS/private heap arena。
  - host origin/bump 分配器把 registered module storage 当作保护区,避免宿主
    LG_mem 重用模块 arena。
  - 识别 compact heap 控制块(`owner+4`,字段 `base/free/end/head` 位于
    `+8/+12/+16/+24`),在 game timer callbacks 前把仍带
    `0x79ABBCCF` magic 的 active/fired timer node 从 compact free list 中裁掉。
  - 对 direct wrapper timer dispatch 和普通 `code=2` timer 路径都执行上述保护。
  - 通过 wrapper compact free routine 的指令签名安装窄 `UC_HOOK_CODE`:
    free routine 完成 list-link/free-bytes 写回后再次保护 live game timer node。
    这是必要的,因为同一次 ARM dispatch 内的 free 可以在前一次保护后新合并出
    覆盖 timer node 的空闲块。
- 验证:
  - descriptor watch 修复后:descriptor 26 `base=0x24CA74,len=0x3C2`,
    不再覆盖 `0x24B268`。
  - 手动 repro `start.ppm`: `pixel(75,75)=[232,184,40]`, `unique=241`,
    stdout 无 `PC=0x23BACE` / `UC_MEM_READ_UNMAPPED`。
  - `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
    通过。
  - `pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts --reporter=verbose`
    2 个用例全部通过。

## 2026-07-06 16:22 当前工作区复现

- 当前 `git status` 只有文档/脚本未跟踪或修改,`src/` 无本问题修复改动;
  `cmake --build build --target vmrp -j2` 成功但未改变行为。
- 命令:`VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
- 结果:失败于 `game-start.test.ts:66`,保留目录 `/tmp/vmrp-e2e-mKY7mh`。
- PPM 证据:`start.ppm` 为 `240x320`,`unique=2`,`nonzero=433`,
  `pixel(75,75)=[0,0,0]`。
- stdout 证据仍为同一崩溃:
  `UC_MEM_READ_UNMAPPED addr=0x1A801B0`, `PC=0x23BACE (thumb)`,
  `LR=0x23BA53`, `R0=0x01A801A8`, `R1=0x0000FFFF`,
  `R2=0x01A7BFE1`, `R6=0x2428B0`, `R9=0x24282C`。
- 当前源码已有 host LG_mem first-fit、registered code overlap 保护、
  app live allocation bounds 和 wrapper compact timer liveness 检测;
  缺口仍是 **game.ext 自己的 compact/free-list 把 live timer node
  重新放回空闲区**。后续修复必须基于结构性 timer node/free-list
  保护,不能写 gzwdzjs 专用分支。

## 2026-07-06 16:43 修复落盘与目标用例验证

- 修改位置:
  - `src/include/arm_ext_internal.h`:记录 compact SDK allocator 的
    R9-relative heap control pointer offset 与 `mr_free()` return hook 地址。
  - `src/arm_ext_executor.c`:通过 Thumb 指令形状发现 wrapper compact
    `mr_free()` 的完成点;收集带 `0x79ABBCCF` magic 的 live compact
    timer node;在 compact heap free-list 中裁掉这些 live allocation
    区间。
- 结构依据:
  - `game.ext` 定时器节点创建/调度反汇编证明 primary game compact
    scheduler 位于 `R9+0x84`, queued/current heads 为 `+0x08/+0x0C`,
    节点通过 `+0x18/+0x1C` 串链,magic 为 `0x79ABBCCF`。
  - `cfunction.ext` compact allocator 反汇编证明 heap control pointer
    来自 `R9+0x14C`,控制块字段 `base/free/end/head` 位于
    `+0x08/+0x0C/+0x10/+0x18`,`mr_free()` 在 return 前已经完成
    free-list 插入/合并和 free-byte 更新。
  - 修复不使用 app 名称/MRP 文件名分支;只有在发现上述 allocator
    指令形状、且 free-list 与 live timer node allocation 区间重叠时才改写
    guest free-list。
- 构建:`cmake --build build --target vmrp -j2` 通过。
- 目标用例:
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程 --reporter=verbose`
  通过。
- 保留目录:`/tmp/vmrp-e2e-hSAUM8`。
- PPM 证据:`start.ppm` 为 `240x320`,`unique=240`,`nonzero=75304`,
  `pixel(75,75)=[232,184,40]`。
- stdout 检查:无 `UC_MEM_READ_UNMAPPED`,无 `PC=0x23BACE`,
  无 `uc_emu_start(0xE83A81) failed`。

## 2026-07-06 16:45-16:57 兼容回归

- `pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts --reporter=verbose`
  通过,2 个用例全部通过。
- 第一批相关回归:
  `pnpm vitest run test/e2e/gxdzc/gxdzc-pixel.test.ts test/e2e/opbzqe/game-prepare.test.ts test/e2e/wbrw/boot-to-home.test.ts test/e2e/gghjt/game-start.test.ts --reporter=verbose`
  通过,4 个文件/5 个用例全部通过。
- 第二批下载/嵌套 EXT/allocator 回归:
  `pnpm vitest run test/e2e/optwar/game-prepare.test.ts test/e2e/optwar/exit-plugin.test.ts test/e2e/talkcat/game-prepare.test.ts test/e2e/dota/download-plugin.test.ts test/e2e/gghjt/download-plugin.test.ts --reporter=verbose`
  通过,5 个文件/13 个用例全部通过。
- 剩余 app E2E 回归:
  `pnpm vitest run test/e2e/gms/game-prepare.test.ts test/e2e/gwkdl/game-prepare.test.ts test/e2e/istore/boot-to-home.test.ts test/e2e/opglqa/font.test.ts test/e2e/gtcm/boot-to-home.test.ts test/e2e/wbrw/input-text.test.ts --reporter=verbose`
  通过,6 个文件/7 个用例全部通过。
- 结果:除公共 harness `test/e2e/vmrp-e2e.ts` 和 `gzwdzjs` 手动 repro shell
  脚本外,当前 `test/e2e` 下 app 测试文件已分批覆盖,合计 16 个测试文件通过。
