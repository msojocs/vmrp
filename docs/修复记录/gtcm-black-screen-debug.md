# gtcm.mrp 启动黑屏调试与修复

日期: 2026-07-04(已修复)

## 现象

`build/skyengine build/mythroad/gtcm.mrp`(240x320 默认分辨率)启动后黑屏。
预期: 启动游戏(贪吃猫, SphinxJoy, mrc C SDK: mrc_loader.ext → cfunction.ext
wrapper → game.ext + graphics.ext)。

## 根因链(反汇编取证)

1. **游戏是横屏(480x320)构建**。game.ext init(0x237078)调 `mr_plat(101,3)`
   请求 LCD 旋转;宿主对 code 101 未实现,返回 MR_IGNORE(1)。反汇编
   0x2370AC: `cmp r0,#0 / beq 正常路径` —— 非 0 进入错误分支,写状态
   0x0D(0x23FC60),每 tick 绘制"不支持横竖转换请退出"(UCS2 字符串
   0x23B1B0),并把 ARM 可见 mr_screen_w/h(0x2259C0/0x2259C4)写成
   320x480。
2. **宿主绘制越界抹掉 game.ext**。EXT 屏幕缓冲按物理分辨率(240x320,
   0x25800 字节)分配在 bump 堆首(0x2001BC),紧随其后就是 game.ext 镜像
   (0x226118)。宿主 DrawRect(table[122])信任 guest 写入的 w/h(经
   arm_ext_push_draw_screen_context 采纳 320x480),对 240x320 缓冲填充
   0x4B000 字节 → 越界 150KB 把 game.ext 代码+RW 清零(黑色=0x0000)。
3. **首个 tick 滑入零区,定时器链死亡**。tick 回调执行到被清零的代码,
   NOP 滑行(0x23FB14→0x2C59A0)撞上 0xFFFF → UC_ERR_INSN_INVALID 被吞,
   无人再 timerStart → 黑屏且 draw_count 冻结在 320。

诊断手段: VMRP_ARM_EXT_DIAG / VMRP_ARM_EXT_TRACE(_PC) / VMRP_WATCH_WRITE /
VMRP_WATCH_SENTINEL(捕获宿主侧覆写: sentinel 在 table[122] 前后翻转为全
零,而 guest 写监视无记录 → 宿主 memset 实锤)、PC ring(SP/LR 扩展)、
capstone 反汇编(cfunction.ext/game.ext,子 agent 完成三轮取证)。

## 修复(通用机制,无应用特定分支)

1. `src/mythroad/dsm.c mr_plat`: 实现 code 101(LCD 旋转设置/查询),返回
   MR_SUCCESS。模拟器帧缓冲方向由应用自行绘制、展示层裁剪。
2. `src/arm_ext_executor.c hook_screen_dim_write`(新增): 监视 guest 对
   ARM 可见 mr_screen_w/h 变量(table[92/93] 指向 u32)的写入。当逻辑尺寸
   超过宿主缓冲容量、且 table[91] 仍指向宿主缓冲时,把屏幕缓冲迁移到
   16MB 空间顶部的专属保留区 EXT_SCREEN_RESERVE(1MB,覆盖 480x800x2),
   同步 table[91]/table[95] 描述。迁移发生在写变量瞬间,早于 graphics.ext
   缓存 framebuffer 指针。
   - 门控 1(gghjt 回归修正): table[91] 已被 guest 指向自有缓冲(滚屏
     离屏合成先换 buf 再写更宽的 w=272)时不迁移,否则恢复后 buf 与宿主
     screen_addr 失配导致花屏。
   - bump 堆布局保持与旧版完全一致(屏幕缓冲仍占位堆首);仅迁移发生后
     顶部保留区才对 arm_alloc 关闭。
3. `arm_ext_push_draw_screen_context`: 迁移完成后在容量内采纳 guest 尺寸
   (present stride/damage 行数与绘制一致);任何情况下 guest 尺寸超过
   宿主缓冲容量则回退到宿主已知尺寸,宿主绘制永远不越界。
4. `app_compat_gghjt.c`: chk 提取源判定从硬编码 0x2001BC 改为
   m->screen_addr(与堆布局解耦,语义不变)。

## 显示行为

- 240x320(默认): 逻辑画布 480x320,窗口显示左上 240x320 裁切区,游戏
  正常启动运行(开场"是否开启音乐"→ 主界面猫全图)。
- `--screen 480x320`: 全画面正常显示。附带修复了 main.c 的窗口创建时序
  bug: SDL_CreateWindow 此前用 skyengine_config 的编译期默认值(240x320),
  而 --screen 解析结果要到 startVmrp() 才写入 skyengine_config,导致任何
  非默认分辨率窗口都是 240x320;现在创建窗口前先同步 skyengine_args 的
  分辨率。
- 320x480 / 240x400 运行仍有独立的 Unicorn TCG 崩溃(host 侧,
  tb_tc_cmp bp=0x0),与本修复无关,未处理(剩余风险)。

## 验证

- gtcm 240x320: PPM 20s 显示"是否开启音乐"对话框(0.4% 非零,黑底白字),
  点击后 30s/50s 主画面 90%+ 非零像素,画面正确(贪吃猫主界面)。
- gtcm 480x320(--screen): 同样正常,全画面可见。
- 回归: gghjt game-start(不花屏/不缺渲染)、gxdzc-pixel、talkcat 启动、
  dota download-plugin(返回重进/成功)全部通过;test_skyengine*, 4 个原生
  单测全部通过。
- 临时诊断代码(main.c/mythroad.c/arm_ext_executor.c/arm_ext_internal.h 的
  DIAG 打印、PC ring 扩容、SP/LR ring)已全部移除;保留 dsm.c 的
  VMRP_WATCH_* 既有诊断设施不动。

## 剩余风险

- --screen 320x480/240x400 下的 TCG host 崩溃(独立既有问题)。
- gtcm 在 240x320 窗口只显示逻辑画布左上裁切(治本需要 present 缩放,
  超出本次范围;用户可用 --screen 480x320 获得全画面)。
