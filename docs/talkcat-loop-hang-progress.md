# talkcat 循环取消卡死 — 调查进度

## 问题
- 测试：`pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "循环"`
- 现象：循环触发「下载 - 取消」，约第 18 轮在点击下载确认时 guest 崩溃，随后界面不再进入预期状态。
- 预期：重复触发不卡死、不崩溃。

## 约束
- 不用 xvfb；trace 日志量巨大，优先用窄范围 `VMRP_WATCH_*`、PPM、反汇编。
- 修改必须说明原因；不写兜底/吞错逻辑；不写应用特定分支。
- 先修复 TalkCat loop，再做兼容性测试。

## 已确认事实
- 崩溃 PC 为 `0x232218`，属于 TalkCat 运行时 `game.ext`，模块基址 `0x226118`，偏移 `0xC100`。
- 崩溃指令 `ldr r0, [r0, #56]` 的基址来自 PC-relative literal；该 literal 高半字被覆写，导致读 `0x634F6118` 等未映射地址。
- 第一个明确覆写者不是自修改代码，而是 guest 位图/RLE blitter `0x226C08` 内的 `0x226E48: strh r0, [r4]`。
- 反汇编和现场栈显示，该次绘制参数本身有效：`x=32,y=225,w=180,h=31`，目标屏幕为 `240x320`，初始目标地址应在屏幕内。
- 覆写发生在 blitter 已跑到屏幕外：首个命中位置等价于 row 323, col 227，说明 type-6/RLE 命令流越过声明的 `180x31` 图像范围。
- 出错 blit 的源 bitmap descriptor 为 `0x277A9C`，内容 `{w=180,h=31,len=0x339D,type=6,base=0x282224}`。
- `0x282224` 在每次成功循环中按 `4886, 4886, 13213` 的顺序被 `readFile("abc")` 复用；失败的 #35 down 只看到两个 `4886`，还未重新加载 `13213` 就崩溃。
- descriptor parser `0x2274A4` 的 type-6 分支会先 `0x230EF4(size)` 取 backing buffer，再 `0x23390C` 填充/解码；但它随后不检查 `0x23390C` 返回值就写出 descriptor。
- 正常轮次 descriptor 写入时留下的 `LR=0x233A3F` 对应 `0x23390C` 成功收尾；失败 #35 的 `LR=0x233A29` 对应 `0x23390C` 失败/清理收尾。也就是说 #35 的 180x31 descriptor 是在 nested decode/read 失败后发布的。

## 当前判断
- 根因更像是 `0x23390C` 在第 35 次点击的 13213/type-6 backing 填充阶段失败；parser 忽略失败并发布 descriptor，使 blitter 用两个较小 `readFile("abc")` 结果覆盖后的 `0x282224` 解释为 180x31 type-6 图像，RLE 流失控并写穿屏幕，最终覆写已注册 `game.ext` 代码/literal。
- 现有“已注册模块区不可再分配”实验只能降低代码区被 allocator 复用的风险，不能阻止这次 guest blitter 从屏幕尾部写入模块区，因此还不是根修复。
- 缺失的第三个 `WATCH_READFILE fl=13213` 还不能区分“未调用 table[125]”与“table[125] 失败后未进入成功日志”。已加入窄 `WATCH_READFILE_FAIL` 临时诊断。

## 下一步
- 已确认：`WATCH_READFILE_FAIL` 证明失败轮次确实调用了 table[125]；`fl=13213`
  表示 `_mr_readFile` 已找到 `abc` 并解析出解压后长度，但返回的 host buffer 为 NULL。
- 根因：table[125] 把 host `_mr_readFile` 的 `mr_malloc` 输出复制到 ARM 可见 LG_mem
  后，没有释放原 host buffer。ARM guest 后续只能释放 ARM 返回值，无法释放这个桥接
  暂存 buffer；循环下载/取消不断解压 `224742/4886/13213` 等载荷，最终耗尽 host
  LG_mem，先导致大图 `224742` 分配失败，再导致 `13213` type-6 backing 分配失败。
- 修复：table[125] 完成 ARM copy、gzip slot 同步、`last_file_copy` 缓存和 adjacent-slot
  镜像后，若 host pointer 位于 host `LG_mem` 且不是直接 RAM package pointer，则调用
  `mr_free(hp, fl)` 释放桥接暂存 buffer。不吞错、不改 app 分支，保持 ARM 返回值和
  guest free 语义不变。
- 验证：`VMRP_WATCH_ALLOC=0x282000,0x286000 pnpm vitest run
  test/e2e/talkcat/game-prepare.test.ts -t "循环" --reporter=verbose` 已通过。
  最新 watched artifact 在 `/tmp/vmrp-e2e-9ckQqC`，#35 之后点击返回值保持 0，
  `4886,4886,13213` 的 `readFile("abc")` 序列继续出现，无
  `WATCH_READFILE_FAIL`/invalid memory。
- 补充：full TalkCat e2e 运行到「下载喝水资源包并安装」时命中既有像素断言
  `[128,220,168] != [32,64,120]`，未出现 invalid memory/readFile failure；该用例未能
  作为本轮完整回归结果。
