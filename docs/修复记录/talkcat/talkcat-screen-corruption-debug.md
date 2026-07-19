# TalkCat 主界面花屏横线调试

日期:2026-07-02(接续 talkcat-startup-crash-debug.md)

## 问题

- 命令:`build/skyengine build/mythroad/talkcat.mrp`
- 预期:游戏主界面显示正常。
- 实际:进入主界面后画面存在大量散布的短横线花屏(彩色噪点线段)。

## 复现(2026-07-02 11:21)

```bash
rm -rf /tmp/skyengine-tc-repro /tmp/talkcat-repro.ppm
install -d /tmp/skyengine-tc-repro
timeout 60s env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy VMRP_NO_MOUSE=1 \
  VMRP_PPM=1 SKYENGINE_PPM_PATH=/tmp/talkcat-repro.ppm \
  build/skyengine --work-dir /tmp/skyengine-tc-repro build/mythroad/talkcat.mrp
```

结果:

- PPM 240x320 有效,主场景(砖墙小巷+猫)可见,但全屏散布短横线彩色噪点。
- 行差分析:第 14/22/25-27/59-67/84/91-92 行等异常。
- stdout 关键日志:
  - `native_playSound: unsupported compressed sound type 2 len=10560`
  - 紧接一次 `arm_ext_executor: uc_emu_start(0xE83A55) failed: 10 (UC_ERR_INSN_INVALID)`
  - 崩溃现场:PC=0xE7F5F8(wrapper 栈页,非代码),R9=0x2DB6A4,LR=0x2D239B
  - 与上一轮修复前的崩溃签名一致(wrapper timer dispatcher 入口 0xE83A55,返回地址被踩)。

## 假设

- 花屏与 wrapper 栈被踩可能同源:某个 bridge/plat 调用写坏 guest 内存
  (解码帧缓冲 + wrapper 栈)。
- playSound 失败(compressed type 2 = MP3?)返回值/行为可能触发 guest 错误路径。

## 进行中

- 子 Agent A:带 VMRP_ARM_EXT_DIAG 重跑,过滤崩溃前的 bridge 调用序列,找出踩内存操作。
- 子 Agent B:反汇编 cfunction.ext,梳理帧解码/绘制路径与 0xE83A55 定时器调度逻辑。

## 2026-07-02 修复 1:table[125] RAM 源镜像溢出(崩溃根因)

子 Agent 诊断结论:
- `arm_ext_mirror_read_file_to_ram_source()`(未提交的实验代码)在 table[125]
  (mr_readFile_from_ram, pack='$')后把解压输出(fl)写回压缩输入缓冲区(raml),
  守卫方向 `len <= ram_len → skip` 使其只在 fl > raml(解压必然)时触发,
  每个图标越界 599~2108 字节,连续互踩 0x317xxx 堆区。
- 原生实现(src/mythroad/mythroad.c `_mr_readFile` '$' 分支)解压到新分配的
  mr_gzOutBuf,从不回写 RAM 源。镜像逻辑与原生语义矛盾,删除。
- playSound type 2(MP3)返回 MR_FAILED 是正常路径,红鲱鱼。

修改:src/arm_ext_executor.c 删除该函数、调用点及 DIAG ramMirror 字段。

验证(60s 干净 workdir):uc_emu_start/UC_ERR 0 次(修复前每次必现 1 次)。
**但 PPM 仍有散布横线花屏 → 花屏另有根因,与崩溃独立(基线对照也证实)。**

## 待查:花屏第二根因

- 子 Agent B:磁盘缓存帧(start/1, 83x88 RGB565)解码干净,宿主
  guiDrawBitmapWithStride 边界正确 → 损坏发生在 guest 内存位图或 present 路径。
- 下一步:检查未提交 diff 中的屏幕损伤跟踪/stride 改动
  (arm_ext_note_screen_damage_addr_range、ArmExtBitmapPresentCtx、
  arm_ext_screen_stride),以及 PPM 生成链路。

## 2026-07-02 花屏根因(已确认)

证据链:
1. 时序快照:splash(snap1-4)完全干净,进入主场景(snap6)后出现噪点;
   噪点在两次快照间静止。VMRP_SOURCE_PPM(临时诊断,修复后已随调试代码
   清理移除)对比证明噪点在 guest 屏幕缓冲内。
2. 磁盘缓存 normal(240x320 RGB565)干净 → 损坏不在解码/缓存链路。
3. 噪点像素原始字节 = talkcat.mrp 的 3408 字节索引片段(start1.jpg/mbg.slg 等
   文件名+偏移),多个 100~256 字节片段以不同 delta 散布——符合"带透明色的
   精灵 blit 从错误源地址逐行拷贝"。
4. DIAG:游戏把 MRP 索引反复读进自有池(0x31766C 等);table125('abc',pack='$')
   的解压输出被 bridge 放在 arm_alloc bump 地址(0x35xxxx),lenSlot 正确。
5. 反汇编(子 Agent,cfunction.ext):
   - wrapper readFile(0xE83698)与 thunk(0xE83683)原样透传 table125 返回值;
   - 调用方 0xE82744 在 0xE827A8 调 readFile 后【丢弃 r0】,从自行推算的池地址
     blit(0xE83810);
   - 该推算依赖原生 first-fit 语义:游戏先把一块池内存 free 回 DSM 空闲链表
     (mythroad.c:1290 "骚操作"),readFile 内部 mr_malloc(reallen) 按 first-fit
     必然复用该块 → 输出地址可预测;
   - VMRP 的 table[0]=arm_alloc(bump)、table[1]=无操作,空闲链表从不运转,
     预测地址处只剩陈旧索引字节 → 花屏。
   - 崩溃(已修)与 mirror_to_ram_source 是上一轮对该问题的错误近似。

## 修复方案 2(进行中)

按 src/mythroad/mem.c 原生语义在 guest origin_mem 池上实现 first-fit 分配器:
- table[0] malloc / table[1] free / table[2] realloc 走真实空闲链表
  (节点 {next,len} 内嵌 guest 内存,head 存宿主侧,与原生全局 head 等价);
- table[125] 输出改用该分配器(原生 asm_mr_readFile 内部 mr_malloc 语义);
- LG_mem_left/min/top 统计与 slot 同步保留。

## 2026-07-02 修复 2:guest LG_mem first-fit 分配器(花屏根因修复)

修改(src/arm_ext_executor.c + src/include/arm_ext_internal.h):
- 新增 `arm_ext_guest_mem_malloc/free`,逐语句对应 src/mythroad/mem.c 的
  mr_malloc/mr_free,直接操作 guest origin_mem 池内的空闲链表节点
  ({next,len} 内嵌空闲块,8 字节对齐),头节点即 table[146] 暴露的
  8 字节块(原生 ABI 中该项就是 &LG_mem_free)。
- table[0]=malloc、table[1]=free、table[2]=realloc 全部改走该分配器
  (原为 arm_alloc bump + 空操作 free)。
- table[125] readFile 的解压输出改用该分配器分配(原生 asm_mr_readFile
  内部 mr_malloc 语义),分配失败与原生一致返回 0。
- LG_mem_left/min/top 统计以 ARM 可见 slot(table[111]/[135]/[136])为
  权威存储。
- 唯一的宿主侧防护:空闲链表遍历加节点数上限(pool_len/8,链表成环
  即 guest 已破坏,按原生 corrupted memory 返回失败),避免宿主死循环。

## 2026-07-02 验证

两次干净 workdir 60s/45s 运行 `build/skyengine build/mythroad/talkcat.mrp`:
- PPM 240x320 主界面完全正常:背景、猫、全部 UI 图标(齿轮/信息/关闭/
  静音/脚印/旋转)清晰渲染;打哈欠动画帧正常。
- 屏幕像素中搜索 MRP 索引特征字节(.jpg/.slg/start/yawn):0 命中
  (修复前噪点即这些索引字节片段)。
- 无 uc_emu_start/UC_ERR/no memory/corrupt 日志。
- 此前的"横线花屏"实为图标从预测地址 blit 出的索引残留,现图标像素
  由 first-fit 分配器落在游戏预测地址,渲染正确。

待:全量 e2e 回归(后台运行中)。

## 2026-07-02 兼容性回归与最终方案(混合分配器)

首版"全量切换 first-fit(512KB 池,失败即返回 0)"在 e2e 全量回归中
使 dota/opbzqe/optwar/gghjt/opglqa/wbrw 约 9 个用例失败(基线对照:
用改动前代码重编译验证,基线除 wbrw/input-text 与 gxdzc 外全绿)。
机制:这些游戏的内存检测与资源加载依赖远超 512KB 的累计 table[0]
分配(修复前 bump 实际容量 16MB),纯 512KB 池导致分配失败/低内存
路径,黑屏。

最终方案(池优先 + bump 后备):
- 空闲链表只管理 512KB origin_mem 池;table[0]/[2]/[125] 分配先在池内
  first-fit(地址预测成立),池内无合适块退回 arm_alloc bump(容量与
  修复前完全一致);
- table[1] free:池内块真正插回空闲链表并合并;bump 块被 mem.c 同款
  边界校验忽略(等价修复前的纯统计行为);
- LG_mem_left/min/top 统计沿用 note_origin_mem_alloc/free 原账本,ARM
  可见预算值与修复前一致;
- talkcat 的"free 后 readFile 复用"发生在池内(wrapper 大 arena 分配
  首先落池,its 子块均为池地址),预测成立。

验证:
- talkcat:干净 workdir 45s,PPM 主界面/打哈欠动画全部正常,屏幕无
  MRP 索引字节污染,无 uc_emu_start 失败。
- e2e 全量:13 文件 20 用例全部通过(优于基线,基线 wbrw/input-text
  与 gxdzc 失败的用例在混合方案下也通过)。

## 2026-07-02 调试代码清理

以 git diff(HEAD 之后的全部未提交改动)为界,移除其中所有调试性新增:
- src/main.c:dump_source_ppm() 与 VMRP_SOURCE_PPM_PATH 调用点;
- src/arm_ext_executor.c:
  - VMRP_ARM_EXT_SCREEN_DIAG 三个新增块(screen_memcpy/screen_memset/
    screen_read,HEAD 上仅有 arm_ext_diag_visible_present 一处该开关);
  - table45 seek 诊断块及其专用常量 ARM_EXT_MR_GET_FILE_POS;
  - present29 打印恢复为 HEAD 原始字段(bmp/srcStride 等扩展字段随
    调试移除)。
- 清理后 git diff 中不再有任何新增 printf/getenv 调试行;HEAD 既有的
  62 处 DIAG 设施不受影响。

验证:重编译无警告;talkcat 干净 workdir 40s 无花屏无崩溃;e2e 全量
13 文件 20 用例全部通过。

## 2026-07-02 新问题:test.sh 交互中 UC_MEM_WRITE_UNMAPPED 0x3036353A

- 现象:`test/talkcat/test.sh`(真窗口交互)执行中出现
  `arm_ext_executor: invalid memory UC_MEM_WRITE_UNMAPPED addr=0x3036353A size=4 value=0x0`。
- 关键特征:0x3036353A 的 LE 字节 = ASCII ":560" —— 指针被运行时生成的
  文本覆盖(疑似 "%d:%d"(秒:毫秒)或时间串溢出小缓冲踩了相邻指针,
  guest 后续经该指针写 0)。
- 已排除:包内文件与 RGB565 缓存均不含该字节串(运行时生成);
  headless 下纯空闲/快速连点/设置菜单/旋转/退出/启动早期点击 60-150s
  未复现;mr_getTime 为相对毫秒小值;media GETTIME 返回 0;
  table[38] platEx 输出回拷为新分配,安全。
- 进行中:子 Agent 并行动态模糊复现(长时+动画中断点击)与静态反汇编
  (game.ext 中 getTime/datetime+sprintf 数据流、"K:%s %04d"@0xf524 使用者)。

## 2026-07-02 UC_MEM_WRITE_UNMAPPED 0x3036353A 根因与修复

复现(子 Agent):真实 SDL 视频驱动(WSLg 窗口)下启动 ~4.5s 必崩,
无需任何交互;SDL_VIDEODRIVER=dummy 永不触发。最小复现:
`rm -rf build/mythroad/talkcat && DISPLAY=:0 build/skyengine build/mythroad/talkcat.mrp`。

根因链(运行时 DIAG + 反汇编交叉验证):
1. game.ext 用格式串 "%m%d.jpg"(0x2D7188)构建路径,目标是 30 字节
   栈缓冲;原生 sprintf(src/mythroad/printf.c default 分支)对未知
   转换符 %m 只输出字符 'm' 且不消耗参数 → 原生结果 "m5.jpg" 级短串。
2. 宿主 format_arm 把未知 spec 直接交给 glibc snprintf:
   - glibc %m 展开为 strerror(errno)。dummy 驱动 errno=0 → "Success"
     (7 字节,装得下,不崩);真实视频驱动的 socket I/O 置 errno
     (如 EAGAIN → "Resource temporarily unavailable" 33 字节)→ 超长;
   - 且错位多消耗一个参数,后续 %d 读到表槽值 0x10044 → "65604"。
3. 42 字节结果经 case 17 的 strcpy 写回 30 字节 guest 缓冲,溢出尾部
   "…65604.jpg" 覆盖相邻指针字段(崩溃现场 R4="6560" R5="4.jp",
   故障地址 = 0x30363536+4 = 0x3036353A);定时器回调经该指针写 0
   → UC_MEM_WRITE_UNMAPPED。
4. 基线(bump 分配器)下相同溢出也发生,但内存布局不同未撞上要害
   指针,属隐蔽的既有缺陷,混合分配器改变布局后暴露。

修复(src/arm_ext_executor.c format_arm):按原生 printf.c 语义处理
未知转换符——仅输出转换字符本身、不消耗可变参数;已知转换
(s/c/p/d/i/u/x/X/o)行为不变。不给 case 17 的写回加长度钳制:
原生 sprintf 本就无界,钳制属非原生兜底;格式语义对齐后串长与
原生一致,不会溢出。

验证:
- 真实视频驱动 3/3 次 25s 运行无 invalid memory/uc_emu_start(修复前
  ~4.5s 必崩,现场完全一致可对比);
- headless 45s PPM:主界面完好、无索引字节污染;
- e2e 全量回归:13 文件 21 用例全部通过。

## 2026-07-02 喝水资源包下载安装 e2e(Invalid memory read)修复

现象:`pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "下载喝水资源包并安装"`
UI 流程(下载确认/进度/安装确认/安装)全部通过,但安装期 stdout 出现
`Invalid memory read`(unicorn UC_ERR 错误串)。

现场(镜像 e2e stdout 抓取):
- 首崩 `UC_MEM_FETCH_UNMAPPED addr=0x6246834A`,wrapper 栈(SP=0xE7FF10)
  内容全是 RGB565 像素对,PC 即两个像素值;
- 崩溃时 R0=R2=0x25800(=153600 整屏字节)、R1=0xE732A0(EXT 栈区内)
  ——一次整屏大小的拷贝把 wrapper 栈覆盖成位图。

根因(两点叠加):
1. `arm_alloc`(bump)从 0x200000 生长,只检查 16MB 总界,从不避让
   EXT 栈区 [0xE00000,0xE80000) 与 wrapper 代码区;
2. 应用可见分配的 bump 后备块 free 是空操作:安装流程反复大块
   readFile(池被 wrapper arena 占满,全部落 bump)只增不减,累计
   越过 0xE00000 后 SCRRAM/大缓冲直接落入栈区。
   原生设备 LG_mem 仅 512KB,应用堆本就依赖 free 复用,"只增不减"
   与原生语义相悖。

修复(src/arm_ext_executor.c + arm_ext_internal.h):
- arm_alloc 硬性跳过 [EXT_STACK_ADDR, EXT_CODE_ADDR+code_len) 保留区,
  之后 [代码区末,16MB) 仍可分配;
- 新增宿主侧 bump 回收器:live 表登记 table[0]/[2]/[125] 发给应用的
  bump 块;free 只回收登记过的地址(长度取登记值),guest 无法伪造
  free 回收执行器内部分配(屏幕缓冲/模块暂存同在 bump 区);空闲表
  按地址有序合并,first-fit 拆分复用;
- table[0]/[1]/[2]/[125] 统一走 池 first-fit → bump 空闲复用 → bump
  新块 的 arm_ext_app_mem_malloc/free。

验证:目标用例通过(157s 全流程:下载确认→下载→安装确认→安装→
80s 稳定期无 Invalid memory read);全量 e2e 回归:13 文件 21 用例全部通过。
