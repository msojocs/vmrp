# TalkCat 循环“下载-取消”卡死调试

日期：2026-07-02

## 任务

- 用例：`pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环"`
- 预期：重复触发“下载 - 取消”不会卡死。
- 实际：约 17 轮后应用冻结（测试或卡死或像素断言失败）。
- 约束：无 xvfb、无兜底代码、无 app 特定分支、反汇编取证、PPM 验证。

## 2026-07-02 21:10 复现结果

`VMRP_E2E_KEEP_TMP=1 pnpm vitest run ... -t "循环"`：

- CLICK #1..#34（17 轮）正常，`event()` 返回 0。
- CLICK #35 down 时 guest 崩溃：

```text
arm_ext_executor: invalid memory UC_MEM_READ_UNMAPPED addr=0x9D146118 size=4
  crash PC=0x232218 (thumb)
 R0=0x9D1460E0 ... R9=0x0023B684 R10=0x00634310 SP=0x002C5894 LR=0x00227193
```

- 之后 `event()` 返回 -1，应用状态机死掉 → 表现为“卡死”（无新绘制/对话框不再出现）。
- 崩溃现场 512 字节已存 `/tmp/vmrp_crash.bin`（0x232118..0x232318）。
- 遗留观察：先前用户手动运行的 vmrp（PID 11176，cwd=build/）处于 SDL_WaitEvent
  poll 空闲态——与“guest 死掉后主循环闲置”的卡死表象一致。

## 崩溃指令分析

```
232214: ldr r0, [pc, #24]   ; 字面量 @0x232230
232216: add r0, pc          ; r0 = lit + 0x23221A
232218: ldr r0, [r0, #56]   ; ← UC_ERR_READ_UNMAPPED
```

- 字面量应为 `0xFFFF3EC6`（→ 全局结构 0x2260E0，邻近函数同款字面量 0xFFFF3F3A 完好）。
- 实际读到 `0x9CF13EC6`：**高半字 0xFFFF 被覆写为 0x9CF1**，低半字完好。

## 模块归属与覆写范围

- 崩溃代码属于 **talkcat.mrp 内第 2 个 gzip 条目**（解压后 87396 字节，
  mrp 原始偏移 401784），运行时载入基址 **0x226118**（锚点 file+0xc084 ↔ VM 0x23219c）。
- 与原始字节 diff（512 字节窗口）发现 7 处离散覆写：

| VM 地址 | 长度 | 原始 | 覆写后 |
|---|---|---|---|
| 0x232144 | 10 | 08 b5 09 49 3e 22 79 44 08 4b | 28 94 2a 5b 72 42 d3 4b c9 52 |
| 0x2321C6 | 2 | 04 1c | 86 33 |
| 0x2321DC | 2 | 08 b5 | 28 94 |
| 0x232232 | 6 | ff ff 03 49 08 b5 | f1 9c a5 51 87 73 |
| 0x2322B0 | 6 | 00 24 f4 f7 4c ea | 4b 6a a3 18 69 92 |
| 0x23230C | 4 | 0c e0 01 f0 | 6d c1 f1 4c |
| 0x232311 | 7 | f8 09 e0 07 48 48 44 | c9 ee 54 09 5a 2d 8b |

- 覆写呈离散小块（2~10 字节），像是**某数据结构的字段写入**落在了模块代码区
  ——即有分配与已加载模块代码区重叠（参考 dota y80 黑带问题：wrapper 堆指针回退
  导致 RW 与位图记录重叠，同一家族）。

## 覆写形态分析

- 覆写点：0x232144(10B)、0x2321C6(2B)、0x2321DC(2B)、0x232232(6B)、
  0x2322B0(6B)、0x23230C(4B)、0x232311(7B)。散布约 0x1D4 字节内、
  多数非 4 字节对齐、内容像二进制字段——**很像某模块以错误 R9(RW 基址)
  运行时的全局变量写入**，或与代码区重叠的新分配块的字段写入。
- 与 dota y80 黑带同族（模块 RW 被分配到与已有数据/代码重叠的位置）。

## 2026-07-02 22:00 临时诊断（修复后删除）

在 `src/arm_ext_executor.c` 加入环境变量门控诊断：

- `VMRP_WATCH_WRITE="lo,hi"`：uc 写钩子监视 guest 对 [lo,hi) 的写入
  （打印 PC/LR，上限 400 条）；同一范围同时监视分配器
  （pool_malloc/bump_reuse/bump_new/pool_free/bump_recycle 与范围重叠即打印）。
- `VMRP_WATCH_SENTINEL="addr"`：每次 table 调用入口校验 addr 处 16 字节，
  变化即打印前后两次 table 调用上下文（可捕获宿主侧 memcpy 覆写）。

运行：`VMRP_WATCH_WRITE=0x226118,0x23B67C VMRP_WATCH_SENTINEL=0x232228
pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环"`

## 2026-07-02 23:20 诊断 v1 输出分析（/tmp/vmrp-e2e-nBaB7j）

v1 诊断本身有缺陷，但时间线仍有价值：

- 存在**两次 arm_ext_load**（两个独立 uc 实例，地址空间互不相通）。
- v1 的哨兵/写日志用共享 static：哨兵在模块加载前 primed（全 0）、
  400 条写配额在启动阶段就被另一实例的合法写耗尽 → 取证无效。
- 启动时间线（WATCH_ALLOC 无配额，可信）：
  1. `pool_malloc addr=0x225C74 len=16940`（实例?）
  2. 第二次 arm_ext_load
  3. `pool_malloc addr=0x2260F4 len=502660`（491KB 大块，0x2260F4..0x2A0BF8）
  4. `pool_malloc addr=0x226118 len=87396` ← 模块代码块，从 origin_mem
     池分配，**完全位于第 3 步大块范围内**（说明大块已被释放，或两条
     记录属于不同实例——v1 没打实例 id，无法区分）
- **10 轮循环期间没有任何 WATCH_ALLOC**：宿主侧分配器
  （pool_malloc/bump_reuse/bump_new/pool_free/bump_recycle）
  从未再碰模块区。覆写者要么是 guest ARM 代码直接操作 origin_mem
  空闲链表（不经宿主桥，纯 guest first-fit malloc），要么是宿主 memcpy 桥。

## 诊断 v2（2026-07-02 23:25，修复后删除）

- 诊断状态改为按 uc 实例（m 指针）隔离，打印带 `[m<id>]`。
- WATCH_WRITE 只记录**落在已注册嵌套模块 file 范围内**的写
  （注册前的池数据写被过滤），每实例 200 条，带 pc/lr/sp/mod。
- 哨兵在 addr 属于已注册模块后才 prime；每次变化打印前后 table
  调用上下文并重新 prime（每实例最多 20 次）。
- 新增 WATCH_MOD 生命周期打印：register / drop_stale / reset_drop。

## 待定

1. 根据 v2 输出确定写入者（guest PC 或宿主桥）。
2. 修复方向（通用）：防止分配器把已加载模块代码区当作空闲内存重用 /
   防止子模块 RW 基址落入其它模块代码区。

## 2026-07-03 14:35 诊断 v3：覆写者是 type-6/RLE blitter

`VMRP_WATCH_WRITE=0x226118,0x23B67C` 的窄范围写钩子捕获到首个模块代码区写入：

```text
WATCH_WRITE[m1] #1 addr=0x226122 size=2 value=0x223
 pc=0x226E48 lr=0x226E1B sp=0x2C5804
 r4=0x226122 r5=0x28A762 r6=0x28512E r7=0x7E0F81F
```

`0x226E48` 位于 `game.ext` 的 `0x226C08` 函数内，反汇编确认这是 type-6/RLE bitmap blitter 的半字像素写：

```asm
226e48: strh r0, [r4]
```

现场栈参数：

```text
args saved={r0/x=0x20,r1/y=0xE1,r2/w=0xB4,r3/h=0x1F,lr=0x234C3D}
stack68={sx=0x20,sy=0xE1,sw=0xB4,sh=0x1F,dst=0x251C7C,src=0x277A9C}
desc dst=0x251C7C {w=240,h=320,len=0x25800,type=0x0,base=0x2001BC,pos=227,323}
src=0x277A9C {w=180,h=31,len=0x339D,type=0x6,base=0x282224}
locals clip={x=0x20,y=0xE1,rows=0x1F,dst_gap=0x78,src_gap=0x0,src_h=0x1F,src_w=0xB4,dst_w=0xF0}
```

结论：

- draw 请求本身有效：`x=32,y=225,w=180,h=31`，初始目标地址应为 `0x21A7DC`，在 `240x320` screen buffer 内。
- 首个代码区写入已经等价于屏幕 row 323, col 227，说明 blitter 的 type-6 command stream 已越过声明的 31 行图像。
- 源 descriptor `0x277A9C` 仍声明 `{w=180,h=31,len=0x339D,type=6,base=0x282224}`；按 `180*31*2` 计算，command stream 从 `0x284DBC` 起，命中时 `r6=0x28512E`，已经在 command stream 中推进 `0x372` 字节。
- `r5=0x28A762` 远超 `src_base+len=0x2855C1`，符合“用错误/陈旧 backing storage 解释 RLE 命令流”。

## 2026-07-03 14:45 诊断 v4：源 backing storage 被同名 readFile 复用

运行：

```bash
VMRP_WATCH_ALLOC=0x282000,0x286000 \
pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环" --reporter=verbose
```

在成功循环中，点击下载确认时反复出现：

```text
WATCH_ALLOC[m0] pool_malloc addr=0x282224 len=4886
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
WATCH_ALLOC[m0] pool_malloc addr=0x282224 len=4886
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
WATCH_ALLOC[m0] pool_malloc addr=0x282224 len=13213
WATCH_READFILE name='abc' ret=0x282224 fl=13213 ...
```

失败的 #35 down 只有前两个较小载荷：

```text
[CLICK] #35 down x=139 y=266
WATCH_ALLOC[m0] pool_malloc addr=0x282224 len=4886
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
WATCH_ALLOC[m0] pool_malloc addr=0x282224 len=4886
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
arm_ext_executor: invalid memory UC_MEM_READ_UNMAPPED addr=0x634F6118
```

当前工作假设：

- `0x282224` 是 TalkCat 通过 `table[146]`/LG_mem free-list 预测复用的 readFile 输出地址。
- 成功轮次中第三个 `13213` 载荷会重新填充 source bitmap backing storage，使 `0x277A9C` 的 `{len=0x339D,base=0x282224}` 与内容匹配。
- 失败轮次中 source descriptor 仍保持旧的 type-6 图像描述，但 `0x282224` 已被两个 `4886` 载荷覆盖；blitter 随后把较小资源当 type-6 图像解码，RLE 失控并写穿 screen buffer。
- 下一步需要区分三个同名 `abc` 载荷的调用点/内容指纹，并观察 descriptor `0x277A9C..0x277AAC` 的写入生命周期。

## 2026-07-03 14:53 诊断 v5：type-6 parser 忽略 nested decode 失败

补充观察 descriptor watch（`/tmp/vmrp-e2e-5LSnFY`）：

```text
[CLICK] #35 down x=139 y=266
...
WATCH_WRITE #753..#757 -> {w=0xB4,h=0x1F,len=0x339D,type=6,base=0x282224}
  pc=0x227604..0x227612 lr=0x233A29 r4=0x27FD96 r6=0x339D
WATCH_WRITE #758 addr=0x277AA8 value=0x0 pc=0x22D142 ...
WATCH_WRITE #759 addr=0x277A9C value=0x18 pc=0xE826B0 ...
arm_ext_executor: invalid memory UC_MEM_READ_UNMAPPED addr=0x634F6118
```

反汇编结论：

- descriptor parser 起点是 `0x2274A4`。type-6 分支在 `0x2275E4`
  先调用 `0x230EF4(size=0x339D)` 取得 backing buffer，再在 `0x2275FE`
  调 `0x23390C` 填充/解码该 buffer。
- `0x2275FE` 之后代码**不检查 `0x23390C` 的返回值**，直接在
  `0x227604..0x227612` 发布 descriptor `{w,h,base,len,type=6}`。
- `LR=0x233A3F` / `LR=0x233A29` 不是 parser 的真实返回地址，而是
  parser 调用 `0x23390C` 后寄存器里留下的 nested 调用痕迹：
  - `0x233A3C -> 0x233A3F`：`0x23390C` 的成功收尾分支；
  - `0x233A26 -> 0x233A29`：`0x23390C` 的失败/清理分支。
- 因此 #35 不是“descriptor 没重写”，而是“descriptor 被重写为 180x31，
  但 nested decode/read 路径失败，parser 仍发布了指向 `0x282224` 的 type-6
  descriptor”。随后 blitter 在 cleanup 前使用该 descriptor，源数据仍是前两个
  `4886` readFile 载荷留下的内容。

当前新增临时诊断：

- `VMRP_WATCH_ALLOC` 下新增 `WATCH_READFILE_FAIL`，只在 table[125] 找不到宿主数据
  或无法分配 ARM 输出缓冲时打印。下一轮用它确认失败的 13213 路径是“未调用
  table[125]”还是“table[125] 返回 NULL”。

## 2026-07-03 15:06 修复：释放 table[125] 的 host readFile 暂存 buffer

`WATCH_READFILE_FAIL` 后续确认失败轮次不是“未调用 table[125]”：

```text
[CLICK] #35 down x=139 y=266
WATCH_READFILE_FAIL name='abc' reason=no_host_data fl=224742 ...
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ...
WATCH_READFILE_FAIL name='abc' reason=no_host_data fl=13213 ...
```

关键解释：

- `_mr_readFile` 返回 NULL 时 `fl=13213`，说明 RAM package 里已经找到 `abc`，
  并且 gzip footer/头信息已经给出解压后长度；失败点在 host 侧解压输出 buffer
  分配或解压阶段，不是 package entry 缺失。
- table[125] 的桥接语义是：host `_mr_readFile` 先返回一个 host `mr_malloc`
  buffer，executor 再把它复制到 ARM 可见 LG_mem 并把 ARM 地址返回给 guest。
- 修复前，复制完成后 host buffer 没有释放。guest 只知道 ARM 返回值，后续
  `table[1]` 释放的是 ARM copy，不可能释放 host `_mr_readFile` 的暂存 buffer。
- 循环下载/取消不断解压同名 `abc` 的大/中/小资源，host LG_mem 被这些不可见
  暂存 buffer 耗尽；所以先看到 `224742` 失败，随后到 #35 时 `13213` 也失败。

代码修复：

- 增加 host `LG_mem` 范围判断，只释放确实来自 host `mr_malloc` 的 readFile 结果。
- 在 table[125] 成功路径完成 ARM copy、gzip slot 映射、`last_file_copy` 缓存和
  adjacent-slot 镜像之后调用 `mr_free(hp, fl)`。
- 如果 `_mr_readFile` 返回的是 ARM RAM package 内的直接 pointer，则不释放；该路径
  不是 host `mr_malloc` 所有权。

验证：

```bash
cmake --build build --target vmrp -j2
timeout 180s env VMRP_E2E_KEEP_TMP=1 \
  VMRP_WATCH_ALLOC=0x282000,0x286000 \
  pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环" --reporter=verbose
```

结果：

```text
✓ test/e2e/talkcat/game-prepare.test.ts > talkcat 进入游戏 > 循环取消 123843ms
```

通过 artifact：`/tmp/vmrp-e2e-9ckQqC`。stdout 中 #35 之后点击返回值保持 `0`，
`4886,4886,13213` 的 `readFile("abc")` 序列继续出现，未再出现 invalid memory
或 `WATCH_READFILE_FAIL`。

补充回归：

- full `test/e2e/talkcat/game-prepare.test.ts` 运行时，「游戏启动正常」通过。
- 「下载喝水资源包并安装」命中像素断言：
  `expected [ 128, 220, 168 ] to deeply equal [ 32, 64, 120 ]`。该 run 未出现
  invalid memory/`WATCH_READFILE_FAIL`，但由于断言失败和外层 timeout，不能作为
  full TalkCat 通过结果。

## 2026-07-03 16:30 修复：table[44] 覆写子模块后释放代码归属

安装用例的断言失败不是像素不稳，而是 guest 在 install-confirm 前崩溃：

```text
arm_ext_executor: uc_emu_start(0xE83A55) failed: 10 (Invalid instruction)
pc bytes @0x2275AE: 38 DE E1 79 ...
```

窄写监控已证明写坏 `0x2275AE` 的仍是 game.ext 的 type-6/RLE blitter
`0x226E48: strh r0,[r4]`。源 descriptor 为：

```text
src=0x277A9C {w=83,h=28,len=0x1316,type=0x6,base=0x282224}
```

关键差异来自子模块注册后的地址预测破坏：

- 下载器子模块通过 `table[125]` 读入并在 `table[131]` 注册为
  `file=0x282220+0x4A28`。
- 为避免 allocator 复用仍在执行的 EXT 映像，执行器把已注册模块范围视为
  app-visible LG_mem 的保护洞。
- 随后主 game.ext 通过 `table[44]` 把 `talkcat/msg/mbg` 直接读到
  `0x28223C`，这已经覆写了上述子模块的代码存储。此时 native 语义下该范围
  不再是可执行子模块，而是主 game 的资源 scratch。
- 修复前 nested module 记录仍存在，`table[125]` 解码 `mbutton` 的 4886 字节
  `abc` 时跳过 `0x282224`，返回 `0x2C5D38/0x2C7058`；但 TalkCat 的反汇编路径
  仍按 first-fit 预测发布 descriptor `base=0x282224`。
- 后续 `table[44]` 又把 `talkcat/msg/m1` 和 `str1` 读到 `0x282224/0x28223C`，
  blitter 将这些 raw bytes 当作 4886 字节 type-6 按钮图解码，RLE 跑出屏幕后
  覆写 game.ext 代码。

代码修复：

- 新增 `arm_ext_retire_modules_overwritten_by_data_read()`。
- `table[44]` 成功读入后，如果目标范围与已注册代码范围重叠：
  - `uc_ctl_remove_cache()` 失效 Unicorn TB；
  - 调用既有 stale child drop 逻辑移除非 primary 子模块记录；
  - 若被覆写的是 pending internal staging，则清空 pending；
  - 若 active child 因此被清除，恢复 active 到 primary。
- 这是通用语义修复：直接文件读覆写已注册 child image 后，该 child record 不能再
  参与 PC/R9/timer 归属或 allocator 保护。不裁剪 blitter，也不吞 crash。
- 同时移除了本轮临时 `WATCH_TABLE` 诊断；保留更窄的 `WATCH_ALLOC`/`WATCH_READFILE`
  取证路径。

验证：

```bash
cmake --build build --target vmrp -j2
timeout 180s env VMRP_E2E_KEEP_TMP=1 \
  VMRP_WATCH_ALLOC=0x282000,0x286000 \
  pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "下载喝水资源包并安装" --reporter=verbose
timeout 180s env VMRP_E2E_KEEP_TMP=1 \
  pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环" --reporter=verbose
```

结果：

```text
✓ 下载喝水资源包并安装 156502ms
✓ 循环取消 123611ms
```

安装通过 artifact：`/tmp/vmrp-e2e-FcrpeL`。关键证据：

```text
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ... lenSlot=0xE7FDDC
WATCH_READFILE name='abc' ret=0x282224 fl=4886 ... lenSlot=0xE7FDDC
```

也就是说子模块范围被 `table[44]` 数据读覆写后，后续 4886 字节 `mbutton`
解码重新回到 TalkCat 预测的 `0x282224`，descriptor/base 与 backing storage
再次匹配。循环回归 artifact：`/tmp/vmrp-e2e-RUAyZp`。
