# 490111 三国启动黑屏修复进度

日期：2026-07-16

## 目标与约束

- 复现命令：`build/skyengine build/mythroad/490111_240x320_sanguo.mrp`。
- 目标：默认参数下正常启动游戏；修复不得依赖包名或应用特定分支。
- 图形验证使用 SDL dummy 与 PPM，不使用 Xvfb。
- ARM trace 仅使用有界诊断、PC 环形缓冲和反汇编地址观察点。

## 基线证据

- MRP SHA-256：`bcbac32ad7567e1112f0c15db6c8aae9752bfe4faf2f2492f5665e7332c51702`。
- 10 秒基线运行持续占用约 100% 单核，没有产生任何 PPM；进程只能由超时器终止。
- Lua `start.mr`、19428 字节 `cfunction.ext` wrapper 和 164716 字节 `game.ext`
  都已经成功解压。`game.ext` 注册为 primary：文件区 `0x226318..0x24E684`，
  P=`0x2BC5DC`，helper=`0x238F8D`，RW=`0x24E68C`。
- 卡死发生在同步 `code=0` 启动调用内，尚未返回 Lua，也未进入 SDL/timer 主循环，
  因而不是 present、窗口或宿主 timer 问题。
- `--memory 2M` 与 `--memory 4M` 仍然卡死，说明宿主 Mythroad 堆容量不是根因。

## 反汇编与运行时定位

- `VMRP_ARM_EXT_TRACE_PC=1` 只记录 64 项环形缓冲，不打印逐 PC 日志。7 秒采到约
  7700 万条 wrapper 指令；末态稳定重复 `0xE82DE2..0xE82E3C`。
- `cfunction.ext` 以 `0xE80000` 为文件基址反汇编。循环所在函数从 `0xE82DA0`
  开始，调用 `table[0]`/`table[1]` 做一次 1 字节 malloc/free 探测，随后以
  `table[108]` 的内存基址、`table[110]` 的末地址遍历 offset 形式的空闲链表。
- 卡死时 executor 的池为 `0x225E5C..0x325E5C`，链表头为 `0x225E54`
  (`table[146]`)。运行时内存已经不满足 `mem.c` 的 `{next,len}` 不变量：
  头节点首字为 `0x3FDF29A4`；池首节点为 `{next=0x3B0,len=0}`；另一个节点
  `0x22620C` 为 `{next=1,len=0}`。wrapper 加基址后在奇数地址间循环。
- host 的 `arm_ext_guest_mem_malloc()` 会检测零长度/成环并退到 bump 分配器，
  但 guest wrapper 直接读取同一 ABI 链表，没有这层 host 检查，所以错误链表会
  在 guest 内永久活锁。增加 host 超时或强行跳出不是正确修复；必须找到首次破坏。

## 下一步

1. 对 `origin_mem_head_addr` 和当前空闲节点安装有界 guest 写监视，并在 table
   bridge 前后比较链表头，区分 guest store 与 host bridge 写入。
2. 用首次破坏 PC 对照 `game.ext`/wrapper 反汇编，确认 ABI 所有权或地址换算错误。
3. 实施通用修复后，用非黑 PPM 和目标游戏交互证明启动，再执行相关 ARM EXT E2E、
   unit/ctest 与最终全量回归。

## 根因确认与修复

- wrapper `0xE82C80` 的操作不是破坏：它把 `0x40018800/0x45A0` 的外部
  arena 临时接到 LG_mem 链上，把 `table[110]` 的 end 从 `0x325E5C`
  改成 `0x4001CDA0`。新链以 `table[108]` 的 base 为 offset 基准，节点本身
  分别落在主 EXT 内存和 0x40000000 平台映射带，中间数值区间允许不连续映射。
- 旧 host `arm_ext_guest_mem_malloc/free` 始终读取初始化缓存
  `m->origin_mem_addr + m->origin_mem_len`，因此把合法的 external node 误判成
  越界并改走 bump。guest teardown 仍按动态 end 遍历，两端从此操作不同链。
- 修复位于 `src/arm_ext/aex_mem.c`：每次分配/释放从 table[108]、table[110]
  指向的当前 base/end slot 和 table[146] 的当前 head 读取 arena；只要求端点和
  实际节点分别可映射，不要求包含映射空洞的整个数值区间连续；每个节点再验证
  `{next,len}` 和实际映射范围。left/min/top 同样先读取 ARM slot 当前值再更新，
  不覆盖 wrapper 已发布的 arena 统计。
- 保留了 talkcat 等应用依赖的 first-fit、free 后地址复用和 bump 回收；没有
  超时强退、坏链跳过、包名判断或特定场景分支。

## 修复后验证进度

- 目标包在 SDL dummy（无 Xvfb）下 20 秒内完成 code=0、进入绘制和网络初始化。
  `/tmp/sanguo-dynamic-arena.ppm` 为 P6 240x320，SHA-256
  `c4ca6f078322e28d3969479da7b437f55458d6843eea96d9f188b7dd2e3db0e5`；
  共有 26 色，76800 像素中 17849 个非黑。画面显示游戏自己的“连接超时”提示；
  日志已尝试连接 `211.155.236.226:20000`，远端失败不再表现为同步黑屏活锁。
- legacy bump 仅用于诊断，已经撤销；动态 arena first-fit 得到与该实验完全相同的
  PPM，证明最终结果来自 ABI 修复，而不是保留旧分配退路。
- 历史二分显示移除早期 R9 basic-block 同步也可让 code=0 返回，但历史 PPM 仍是
  230400 字节全零、SHA-256 `1edcc2a5...`。该实验只改变活锁是否持续，不能证明
  游戏启动；当前直接根因仍由动态链节点、固定 host end 和修复后非黑 PPM 共同证明。
- 已通过：`cmake --build build --target skyengine -j2`、`pnpm exec tsc --noEmit`、
  `git diff --check`。`build/CTestTestfile.cmake` 和 `build/skyengine-unit` 都是已从
  当前源码删除的旧 unit target 残留，所以本次不把 `ctest` 结果当作当前源码覆盖证据。
- allocator focused E2E 已通过 3 files / 6 tests：talkcat 启动、资源包安装和循环取消；
  gtcm 启动和付费界面；gxdzc 像素流程。
- 最终重建后的 allocator focused E2E 为 3 files / 6 tests，307.26s。
- 默认并行全量 E2E 已通过：19 files / 32 tests，289.89s，覆盖
  Cookie、Dota、GHHJT、GMS、GTMC、GWKDL、GXDZC、GZWDZJS、iStore、
  OPBZQE、OPGLQA、OPTWAR、Talkcat、WBRW 和 golden frames。
- 最终重建后运行使用 `/tmp/sanguo-final.ppm`：P6 240x320，
  230415 字节，SHA-256 仍为
  `c4ca6f078322e28d3969479da7b437f55458d6843eea96d9f188b7dd2e3db0e5`。日志进入
  `my_initNetwork` 并连接 `211.155.236.226:20000`，不再占满单核；15 秒
  外层 `timeout` 只用于结束已进入正常事件循环的游戏，不是生产修复逻辑。

## 回归缺口补齐（fixture 与 e2e 用例）

- 目标 MRP 已版本化为 `test/fixtures/sanguo_490111.mrp`，SHA-256 与本文基线
  相同：`bcbac32ad7567e1112f0c15db6c8aae9752bfe4faf2f2492f5665e7332c51702`，
  来源 `build/mythroad/490111_240x320_sanguo.mrp`。
- 新增 `test/e2e/sanguo/boot-to-home.test.ts`（自动纳入默认 e2e 集合）：
  1. `waitForPixel(120,8)` 等到游戏自绘"连接超时"提示框的蓝色标题栏
     （游戏 IP 直连已停服的 `211.155.236.226:20000`，失败后弹框）。该帧要求
     code-0 返回且 timer 网络状态机推进，活锁回归时永远等不到；超时 180s
     覆盖真实网络下的 TCP 连接超时。
  2. 用 `screenDraw(1)` 回读保留的首帧"幻想三国"加载图并断言 4 个像素点。
     draw 1..3 仅进度条区域（y 268..273）不同，断言点避开该区域，
     不依赖采样时机。
  3. 提示框文案为"点击确定退出游戏"：发送 ENTER 后断言进程干净退出，
     证明按键分发链路完好（活锁回归时进程只能 SIGKILL）。
- 探测记录：修复后 drawCount 稳定为 4（draw 1..3 加载图 126..128 色，
  draw 4 提示框 53 色）；两次独立运行全部断言像素值逐字节一致。
- 用基线二进制（stash 掉 src/ 修复后重建）复跑该用例验证其能抓住回归：
  基线活锁使 e2e socket 30s 内无法就绪，`VmrpE2e.start` 即失败，用例 FAIL；
  恢复修复后重建，用例 8.99s 通过。`pnpm exec tsc --noEmit` 与
  `git diff --check` 通过。
- 含新用例的默认并行全量 e2e：20 files / 33 tests 全部通过，279.24s
  （此前基线为 19 files / 32 tests，新增的 1 file / 1 test 即 sanguo 用例）。
