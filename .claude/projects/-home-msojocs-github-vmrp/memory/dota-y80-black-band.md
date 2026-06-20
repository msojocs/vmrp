---
name: dota-y80-black-band
description: SOLVED — dota 主界面 y=80..159 黑带根因=sync_r9 把未登记新实例(menu)的正确 R9 覆盖成旧实例(loading)的，致 memset(sb-0xA44) 清掉共享 cg_1
metadata:
  type: project
---

**已修复（2026-06-20）。** `test/dota/hp.sh` 主菜单 y=80..159 黑带消失，band nonblack 0→17397/19200，整屏完整连贯。ctest 仍 6/8（失败的 gxdzc_pay_cancel/vitest-timeout 在基线即失败）；gghjt 下载/付费/continue-twice、gxdzc boot/audio/download、mpc/nes/mrpinfo/dota-community 与基线逐项一致、零新增崩溃。

## 修复（落地）
`src/arm_ext_executor.c` `arm_ext_sync_r9_for_code_addr` else 分支：当 `mod` 的 file_addr 有 **>1 个已登记实例**（歧义共享代码），且当前 R9 **不等于任何已知上下文的 rw**（primary / wrapper(m->p_addr) / 任一已登记 nested 的 rw）时，判定它是“wrapper 刚 BLX 进、game 已自行装载 sb 的未登记新实例”，`return` 不覆盖、信任 game 的 R9。残留旧 R9 必等于某已知上下文 rw，仍走原纠正；单实例模块(same_code==1)零影响。纯数据驱动、无 app 指纹。

**为什么之前几个改法不行**：(A)歧义即 return / (B)歧义改用 active_p_addr / (C)只比同代码实例——都因 R9 是全局、sync 改一处级联整张 sb 相对绘制/取数，或把 wrapper/旧实例的残留 R9 也误信任，结果 band 仍黑或绘制被扰。关键是 **必须用“当前 R9 是否任何已知 rw”来区分 game-set-新实例 vs 残留**（前者唯一特征：不匹配任何已登记 rw，因为新实例还没进 nested_modules）。

## 根因链（指令级）
`test/dota/hp.sh` 第一个自动点击 (0,0) → MR_MOUSE_UP(type3) → host code=1 → wrapper 派发给 menu 屏消息处理器 0x662B34（switch，跳表 0x662B54），**msg==4**→0x662B64→`bl 0x662218`→`memset(sb-0xA44,0,0x1C)`。menu 实例(0x73C7E4,rw=0x665494)此刻 active 已设但**尚未登记进 nested_modules**；hook_restore_r9 每个 block 调 sync_r9，`arm_ext_find_nested_module` 只能返回最后登记的同址实例=loading(0x702F00,rw=0x66712C)，在 0x662B4A 把 game 刚设好的 0x665494 覆盖回 0x66712C。于是 memset 落点 = 0x66712C-0xA44 = 0x6686E8 = primary 共享的 **cg_1 背景条结构**（菜单背景是 cg_0..3 四张 240×80 条，存于 primary 全局 ~0x6686xx，被各实例 sb 相对共享），清 0 其指针 → 绘制循环跳过 cg_1@y80。正确 R9=0x665494 时落点 0x664A50（menu 私有数据，无害）。

## 已证伪的旧假设
"cg_1 被 verdload part(b) RW 镜像覆盖"：**错**。完全禁用 `arm_ext_repair_private_child_bridges` part(b) band 依旧；part(a)/(b) 只写非 0 桥值，cg_1 变 0 与之无关。

## 旧版残留说明（已不准确，保留备查）

## 现象（可复现判据）
- `VMRP_PPM=1 VMRP_NO_MOUSE=1 VMRP_AUTO_CLICKS="0,0,3000;223,308,1000" build/vmrp mythroad/dota.mrp`，每秒 cp `/tmp/vmrp_screen.ppm`。稳态(t≥5)：top(0-79)与 bottom(160-319) 有内容，band(80-159) 全黑(nonblack=0/19200)。
- 基线 9d24757 **更糟**（top 也不画）。所以这不是回归，是 dota 菜单一直没完整渲染；HEAD 已修好 top/bottom，只剩中段。

## 真因（已证实，逐层反汇编）
菜单背景是 4 张 240×80 的位图条 `cg_0/1/2/3.bmp`（各 38400B），由 loading 子屏(P=0x702F00)一次性 readFile，指针存进**模块级全局数组**(物理 ~0x6686xx，primary game 的 RW 段，sb 相对，被各实例/primary 跨模块共享)。draw 循环按 y=index*80 画 cg_0@0、cg_2@160、cg_3@240，**cg_1@80 因其指针槽(0x6686EC)在画之前被清零而跳过**。
- 清零者：screen 模块(0x661AC0) 的**消息处理器** 0x662B34，`switch(msg)`，**msg==4** 跳到 0x662B64→`bl 0x662218`，后者 `memset(sb-0xA44, 0, 0x1C)`。在 emulator 里 R9(=该实例 rw_base 0x66712C)使 sb-0xA44=0x6686E8，正好落在共享 cg_1 结构上，把 cg_1 指针清 0。memset 之后**没有任何代码回填** 0x6686EC（ARM 写监视点确认只有 init-0 与 store-0x71BD38 两次，清零是 host 侧 memset 经 table[14] 旁路了 unicorn 写 hook）。
- **证实**：临时 `if(r0==0x6686E8&&r2==0x1C) skip memset` 后 band 填满(17397/19200)，整屏是完整连贯菜单 → cg_1 本应可见，清空就是 bug。

## 已证伪的旧假设
- "cg_1 被 verdload part(b) RW 镜像覆盖"：**错**。完全禁用 `arm_ext_repair_private_child_bridges` 的 part(b)（甚至整段 return）band 依旧。part(a)/(b) 只写非 0 桥值，cg_1 变 0 与之无关。
- 我加的"前台主模块跳过 part(b)"也无效，已回滚。

## 真正的触发链（已定位到指令级，2026-06-20 第二轮）
1. 第一次自动点击 (0,0) 产生 **MR_MOUSE_UP(type=3,p1=0,p2=0)** → host `code=1` 事件 → wrapper 把它分发给 menu 屏的消息处理器 `0x662B34`（switch(msg)，跳转表 0x662B54）。**msg==4** 跳到 `0x662B64 → bl 0x662218 → memset(sb-0xA44,0,0x1C)`。
2. memset 落点 = `R9 + (-0xA44)`。menu 实例的正确 rw=0x665494（→落 0x664A50，menu 自己的私有数据，无害）；但实际 **R9=0x66712C（loading 实例 0x702F00 的 rw）→落 0x6686E8 = primary 共享的 cg_1 结构**，清 0。
3. 为什么 R9 是 loading 的：menu 屏的处理器运行时，menu 实例**尚未登记进 nested_modules**（active_p_addr 已=0x73C7E4，但走的是另一条注册路径）。`arm_ext_sync_r9_for_code_addr`（hook_restore_r9，每个 basic block 触发）用 `arm_ext_find_nested_module` 按 PC 找实例，只能返回**最后登记**的同址实例=loading(0x702F00)。实测在 0x662B4A：game 自己（经 0x661b60）刚把 R9 设成 0x665494（正确 menu rw），下一拍 sync_r9 又把它**覆盖回 0x66712C**（find_nested 的 loading rw）。即 **sync_r9 把 game 设好的、尚未登记实例的正确 R9 反向冲掉**。

## 修复尝试（均失败/有副作用，已全部回滚）
在 `arm_ext_sync_r9_for_code_addr` 的 else 分支按 file_addr 统计同代码实例数(>1=歧义)后：
- (A) 歧义就 `return`（完全不覆盖、信任 game R9）：cg_1 那次清除被挪走，但 R9 在别处被留成残值(0x6672CC)，band 仍黑。
- (B) 歧义时改用 active_p_addr 的 rw：cg_1(0x6686E8) 清除被挡住(CLRCG1 不再触发)，但 band 仍黑，且 case29/case120 的绘制调用都变了——**R9 是全局的，sync 改一处会级联改掉整个 menu 的 sb 相对绘制/取数**。
- (C) “仅当 R9 不是任何已登记同代码实例 rw 时才信任(return)”：同样 band 仍黑且绘制被扰动。
共同教训：**R9-sync 层的任何改动都会沿所有 sb 相对访问级联**；且 menu 实例“active 已设、nested 未登记”的极窄时序窗口让 find_nested 与 active 不一致。要么修**实例登记时序**（让 menu 在其 handler 运行前就进 nested_modules，从而 find_nested 命中正确实例），要么在 sync_r9 里精确区分“game 刚设的新实例 R9”vs“旧实例残留 R9”（需跟踪 R9 写）。

## 验证锚点
- `if(r0==0x6686E8 && r2==0x1C) 跳过该 memset` → band 填满(17397/19200)、整屏完整连贯菜单。这是“只要 cg_1 不被清就正确”的硬证据，但硬编码地址=app 专用，禁用。
- 触发事件：`VMRP_AUTO_CLICKS` 第一个 (0,0) 的 MOUSE_UP。
- 禁止硬编码地址 skip / app 指纹。相关 [[app-compat-generalization]] [[dota-splash-corner-garbage]]。

## 复现/诊断脚手架（已回滚，需要时重加）
case 14(memset) 打印命中 [0x6686C0,0x668720] 的 memset；case 125(readFile) 打印 name/ap；unicorn UC_HOOK_MEM_WRITE 监视 0x6686EC；扩 trace_pc 到 0x661AC0..0x6654B0 并在命中时 dump pc_ring；jump table 在 0x662B54（msg→handler）。
