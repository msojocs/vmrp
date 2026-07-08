# gzwdzjs 无 netpay.mrp 教程卡死(柚子不动)调试记录

## 现象

- 用例:`pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程`
- 用例在启动前删除 `mythroad/plugins/netpay.mrp`(见 game-start.test.ts:19)
- 预期:游戏正常运行
- 实际:主菜单按第 4 下 ENTER 后画面冻结(柚子卡住不动),
  测试报 `ERR wait_draw_timeout current=271 target>271`

## 2026-07-07 复现(手动脚本 /tmp/gz-dbg/repro.sh)

- 环境:SDL dummy 驱动,--memory 2M,work-dir 内删除 plugins/netpay.mrp
- draw_count 走势:ENTER1 264→267,ENTER2 267→270,ENTER3 270→275,
  **ENTER4 之后恒为 275**,画面不再刷新。
- 进程未退出:主线程低 CPU 睡眠(hrtimer_nanosleep),e2e socket 正常响应。
- stdout 出现重复崩溃(每个定时器 tick 一次,状态完全相同):
  ```
  arm_ext_executor: invalid memory UC_MEM_WRITE_UNMAPPED addr=0xFFFFFFF5 size=4 value=0x82
    crash PC=0x23A880 (thumb)
   R0=0 R1=0x64 R2=0xF0 R3=0x82 R4=0x82 R5=0 R6=0x243D5C R7=0x14
   R8=1 R9=0x24282C R10=0x1B R11=0x14 R12=1 SP=0x00000009 LR=0x0023209D
  arm_ext_executor: uc_emu_start(0xE800B0) failed: 7
  ```
- 0xE800B0 = EXT_CODE_ADDR(0xE80000)+0xB0,wrapper(mrc_loader)事件入口。
- 每次事件/定时器回调都确定性地在 game.ext `0x23A880`(prologue
  `push {r4-r7,lr}`)处以 SP=0x9 崩溃 → 定时器泵瘫痪 → 游戏冻结。

## 历史关联

- omx_wiki `gzwdzjs-table-bridge-stale-epilogue-guard`:同一签名
  (0x23A880, SP=0x9)曾由 table bridge stale epilogue 双重 pop 导致,
  已加通用 guard(记录 blx-reg/pop-pc 型 epilogue 的合法返回,拦截重复)。
- commit 802e7ba 仅修掉了盲目 Thumb 重试造成的二次崩溃;
  无 netpay 的"异常路径栈损坏"根因仍在。

## 2026-07-07 第二轮(工作区含候选修复:注册代码映像加入 compact 保护集)

- 候选修复背景(见 src/arm_ext_executor.c 未提交改动注释):无 netpay 错误路径
  实测 free-list 获得覆盖 game.ext `0x226118+0x1C70C` 的空闲段,精灵缓存分配到
  代码区,像素拷贝把 `0x231550` 指令覆写成 `ldrb.w sp,[...]` → SP=0x61/0x9 崩溃。
  修复后 SP 类崩溃消失,但**仍冻结**(ENTER4 后 draw 恒 302),新签名:
  ```
  UC_MEM_FETCH_UNMAPPED addr=0x9359B3BA (thumb)
  R0=R4=0x2428D0 R3=0x9359B3BB R9=0x0024282C SP=0x00445B04(正常) LR=0x0023
  1CFB
  uc_emu_start(0xE800B0) failed: 8   ← 只崩一次,之后定时器泵不再进入
  ```
- 解读:调用点 0x231CF6 附近 `blx r3`,函数指针取自静态区对象 0x2428D0
  (紧邻静态基址 R9=0x24282C),值为垃圾 0x9359B3BB → 该静态区疑似同样被
  错误 free 后再分配踩掉。候选修复只保护了代码区间,没保护静态数据,更
  说明根因是"谁在 netpay 缺失时把覆盖 game.ext 映像的区间放进了 free-list"。
- 复现脚本:`node /tmp/gz-dbg/repro.mjs`(输出在 /tmp/gz-dbg/out/)。

## 2026-07-07 根因确认与修复

用 VMRP_ARM_EXT_TRACE/DIAG 全量 trace 还原完整链条:

1. **启动布局**(与 netpay 是否存在无关的既有事实):wrapper 把 gzip 代码段
   读进 RAM pack 缓冲 `0x2314C0`(len 72570,尾部 0x24303A),`readFile('abc')`
   解压出 game.ext 到 `0x226118+0x1C70C`(映像尾 0x242824),ER_RW 静态段紧随
   其后 `0x24282C+0x4DF4`。**模块映像和 ER_RW 头部 0x80E 字节都落在 RAM pack
   区间内。**
2. RAM pack 之后被 wrapper free 进 compact 堆 free-list → free-list 拿到覆盖
   game.ext 存活存储(代码+ER_RW 头部)的空闲段。
3. 有 netpay 时该空闲段恰好被 netpay 模块映像占走,无副作用;删掉 netpay 后
   (table[40] open 正确返回 0),ENTER4 关卡加载时 compact malloc 把精灵/位图
   缓冲切进该空闲段:
   - 未保护时:像素覆写 0x231550 指令 → 事件回调 SP=0x61/0x9 循环崩;
   - 只保护文件映像(上一轮候选修复):精灵缓冲落到 0x242828 附近,RGB565
     像素(0xF81F 透明色、0x9CDB 等)覆写 ER_RW 头部,帧回调函数指针变成
     像素垃圾 → `blx 0x9359B3BB` UC_MEM_FETCH_UNMAPPED 崩一次,定时器泵
     瘫痪,draw_count 恒 302。崩溃现场 DIAG rw_low 显示 rw+00..+1C 全是
     RGB565 像素值,即铁证。
4. **修复**(src/arm_ext_executor.c):`arm_ext_collect_registered_code_ranges`
   扩展为 `arm_ext_collect_registered_module_ranges` —— compact 堆保护集在
   文件映像之外,把每个已注册模块 P 结构声明的 ER_RW(start_of_ER_RW /
   ER_RW_Length,P+0/P+4)也加入,由既有 cut_range 逻辑从 free-list 裁掉。
   通用结构化判定,无应用特定逻辑。

验证:
- `node /tmp/gz-dbg/repro.mjs`:ENTER4 后 draw 318→426 持续推进,无崩溃日志;
  PPM 确认演示动画(植物/僵尸移动)与剧情对话框正常。
- `pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts -t 教程` ✓ 通过。
- 已移除上一轮遗留的 VMRP_DEBUG_SP_WATCH 临时诊断代码。

## 待办

- [x] 复现并确定冻结点(ENTER4,SP=0x9 崩溃循环)
- [x] 弄清 SP=0x9 直接机理(代码被精灵像素覆写;候选修复已挡住)
- [x] 找到覆盖 game.ext 的空闲段来源(启动期 RAM pack 与解压输出重叠,free 后进 free-list)
- [x] 弄清 netpay 关联(netpay 映像本会占走该空闲段;缺失时段被精灵缓冲复用)
- [x] 通用修复(保护集加入 ER_RW)+ PPM 验证 + 教程用例通过
- [x] 全量 e2e 回归:`pnpm test:e2e` 16 文件 / 27 用例全部通过
      (含 gzwdzjs 教程+花屏、gxdzc、gghjt、dota、talkcat、opbzqe、wbrw、
      optwar、gtcm、gms、gwkdl、istore、opglqa)
