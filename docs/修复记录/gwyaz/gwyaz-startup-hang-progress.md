# gwyaz 启动卡死与注册页输入失效修复记录

日期：2026-07-21

## 目标与约束

- 目标命令：`pnpm vitest run test/e2e/gwyaz/temp.test.ts`。
- 预期：进入“冒泡社区”首次安装注册询问页，并可用右软键离开。
- 不使用 Xvfb，不开启无界逐指令 trace；只使用有界生命周期日志、
  `VMRP_ARM_EXT_DIAG`、少量 `SKYENGINE_ARM_EXT_WATCH_PC` 观察点和 ARM
  反汇编。
- 修复不得检查应用名、包名、哈希、像素或场景，不得增加失败兜底、
  吞错或重复事件投递。
- 调查开始时只有用户维护的 `docs/prompt.md` 处于修改状态，本次不覆盖。

## 失败基线

- `test/fixtures/gwyaz.mrp` 与 `build/mythroad/gwyaz.mrp` 均为 282912
  字节，SHA-256 均为
  `34fed48205f9bf8f2ce2817a808fa4cd4638def69307879047d24e1b0a76bbc7`。
- fresh workspace 上的失败进程未 crash，也未陷入 Unicorn 忙循环；进程处于
  sleeping，控制 socket 持续可响应。
- 三次失败 `home.ppm` 完全相同，SHA-256 为
  `86384a2e9335fda7a072218be13447bd2935641e1ba007c2952299fd6473396e`；
  `240x320`，仅两种颜色，`(89,266)=[0,0,0]`，画面为黑底“正在启动...”。
- 保留的失败产物：`/tmp/skyengine-e2e-SrHCTz`、
  `/tmp/skyengine-e2e-mhHljK`、`/tmp/skyengine-e2e-jwnX6l`；转换图为
  `/tmp/gwyaz-hang-home.png`。

## ARM 反汇编：启动生命周期

外层 `cfunction.ext` 解压长度为 9764，运行基址为 `0xE80000`。

- `0xE80B40` 选择子包 `gwy.mrp`。
- `0xE818E8..0xE8195C` 是完整的 restart helper：发布
  `pack_filename="gwy.mrp"`、`start_filename="start.mr"`，在 `0xE81944`
  通过 table[31] 启动 100ms timer，然后写入 `timer_state=1`、
  `mr_state=RESTART`。
- 生命周期日志依次出现
  `LIFE arm state=RESTART pack='gwy.mrp' start='start.mr'`、
  `restart-before-stop`、`restart-before-start`；说明 guest 没有卡在当前 EXT。
- 第二次 `_mr_intra_start` 才报 `cannot read start.mr`，因为 restart 后的
  EFS 中并没有 `gwy.mrp`。

## 启动根因：table[42] 把 source payload 误报为已安装文件

- guest 在 `0xE8168F` 调用 table[42] 查询 EFS 中是否已有
  `gwy.mrp`。这是 first-run installer 的安装状态探针。
- 旧 `aex_t042` 先调用 `mr_info()`；磁盘上不存在时，又查询当前外层
  MRP cache。cache 中的确有同名 payload，因此宿主返回 `MRP_IS_FILE`。
- cache entry 是安装源，不是已安装 EFS 文件。这个假阳性使 guest
  跳过自己数据区 `0x1FA8..0x2220` 中声明的 manifest extraction，
  直接 restart；restart 卸载旧 ArmExtModule 后，cache 来源随之丢失。
- table[40] 的 MRP cache 虚拟 fd 语义是正确的：它让 installer 能从当前
  容器读取 source payload。错误仅在 table[42] 把这个 source 伪装成
  installed file。

最终启动修复只删除 table[42] 的 cache fallback，让它继续表示真实
EFS 状态。修复后 guest 自己创建：

```text
mythroad/gwy.mrp
mythroad/gwy/dload.mrp
mythroad/gwy/gamelist.mrp
mythroad/gwy/gui.mrp
mythroad/gwy/pmsg.mrp
mythroad/gwy/reglogin.mrp
mythroad/gwy/resmng.mrp
mythroad/gwy/rollscr.mrp
mythroad/gwy/svrctrl.mrp
```

宿主没有猜测安装目录，没有解析 opaque manifest，也没有增加跨
restart package registry。目录命名空间仍由 guest manifest 决定。

## 启动修复后的首屏证据

- 首屏 PPM：`/tmp/skyengine-e2e-IVyCzW/home.ppm`。
- SHA-256：
  `4639525d0659fb33a226c456a4abcc9f740d17c6282cbe46408f3c3d0044d33a`。
- 画面为 197 种颜色的“冒泡社区”注册询问页，
  `(89,266)=[32,160,224]`；目视图另保存于 `/tmp/gwyaz-static/layout.png`。

## 输入失效基线

启动修复后，右/左软键、SELECT 及两个按钮点击都是
`draw_count 21 -> 21`，输入前后 PPM 零差异。受控输入探针排除了：

- E2E/SDL 映射错误：`RIGHT_SOFT -> SDLK_MINUS -> MR_KEY_SOFTRIGHT=18`。
- native text widget 截获：当时 widget 未激活，filter 返回 0。
- Lua 与 `_strCom(801)` 边界丢事件：`dealevent` 存在，press/release
  均进入，`_mr_pcall` 返回 0。
- wrapper 漏分发：`0xE82B1C` 事件链实际遍历 reglogin
  (`P=0x31C354`) 与 GUI (`P=0x31C4BC`) 两个节点。

## ARM 反汇编：GUI 事件在当前页指针处短路

GUI EXT 解压长度为 40124，运行基址 `0x24212C`，
`P=0x31C4BC`，helper=`0x24A9D1`，RW=`0x24BDF0`。

- wrapper 分发目标 `0x24A750` 的真实 Thumb 入口为
  `push {r3,lr}; bl 0x24B5B4`。
- `0x24B5B4` 把 guest 事件 `0/1/2/3` 分别翻译为 GUI 消息
  `0x3004/0x3005/0x3001/0x3002`，然后调用 `0x2436DC`。
- press/release 定点实测为 `r0=0x3004/0x3005`、`r1=18`；软键值
  和 GUI ABI 都正确。
- `0x2436DC` 经 GUI RW 取到 root=`0x226750`。`root+0x24` 的全局
  interceptor 为 0，随后 `0x243710` 读取 `root+4`的当前页；实测
  该值为 0，`0x243714` 立即跳到返回，未进入任何 widget
  callback。
- 另一组生命周期 PC 观察证明，GUI 曾在自身 `+0x12A0`
  把顶层 widget `0x24C118` 写入精确地址 `0x226754`；之后没有进入
  `+0x34F0` detach 或 `+0x1558` destructor。guest 并未清空当前页。

## 输入根因：wrapper RW journal 把已登记 child 合法写入当成污染

wrapper RW 基址为 `0x225E60`，因此：

```text
0x226754 - 0x225E60 = 0x8F4
```

旧 journal write hook 把所有不在 wrapper 代码区内的 PC 都当成
foreign write。GUI 是已登记 nested module，它通过 wrapper 分配并传入的
共享对象指针写 `root+4` 是合法 guest 行为，但旧 journal 仍记录并在
下一个 `arm_ext_call` 前回滚：

```text
DIAG wrapper_rw_journal revert off=0x8F4 0x18->0x00
DIAG wrapper_rw_journal revert off=0x8F5 0xC1->0x00
DIAG wrapper_rw_journal revert off=0x8F6 0x24->0x00
DIAG wrapper_rw_journal revert off=0x8F7 0x00->0x00
```

即将 `0x24C118` 回滚为 0。日志还同时回滚 `0x8EC`、`0x8F0`、
`0x8FC`、`0x904` 等同一 GUI 共享对象状态。

修复将 journal 的 foreign 定义改为：PC 既不属于 wrapper，也不属于任何
已确认 nested module。

- 已登记 child 可合法操作 wrapper 分配的共享对象。
- 私有 loader 中因借用 wrapper R9 而写错目标的 staging PC 还没有
  模块所有权，仍被 journal 捕获。
- geyaxz 中登记后继续写入的陈旧 staging PC 也不在登记后的
  runtime image 范围内，因此原保护语义仍保留。

这是基于已确认代码所有权的通用判据，没有引入应用、地址偏移、
指针值、按键或页面特判。

## 修复后证据与回归

- `cmake --build build --target skyengine -j2`：通过。
- 用户指定命令：1/1 通过，约 8.35s；开启有界 DIAG 的二次
  运行也是 1/1 通过，日志中不再有 `off=0x8F4` 回滚。
- 离开注册页后的 `/tmp/skyengine-e2e-IVyCzW/not-register.ppm`
  SHA-256 为
  `70f2dccf4bc408a2a7c5af0816da4798266b2b221de4f32c9346a2e47337a1aa`，
  与首屏哈希不同。
- journal 原职责回归：geyaxz `boot-to-home` 两例和约 97s 的
  `play` 用例共 3/3 通过，包括无 plugins 时的下载页路径。
- 相关回归：`gghjt`、`gwkdl`、`cookie/run-mrp`、`dota`、
  `optwar`、`wxmdld`、`wbrw` 共 10 文件/18 用例通过。
- 最终 `pnpm vitest run test/e2e --retry=0 --reporter=verbose`：
  **28 文件 / 51 测试全部通过**，无 retry，总耗时 308.63s。
- `ctest --test-dir build --output-on-failure`：1/1 通过。
- `pnpm exec tsc --noEmit`：通过。
- `git diff --check`：通过。

全部临时 `SKYENGINE_INPUT_DIAG` 探针已删除；最终源码行为修改只有
table[42] 恢复真实 EFS 语义，以及 wrapper RW journal 改用已确认模块
所有权识别无主写入。
