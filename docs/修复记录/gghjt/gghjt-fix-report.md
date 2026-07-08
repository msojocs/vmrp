# gghjt.mrp 启动修复报告

提交：`b5382e7 fix: 罪恶都市游戏包释放失败`
变更范围：4 个文件，+226 / -3

```
src/arm_ext_executor.c  | 198 +++++++++++++++++++++++++
src/main.c              |  27 +++
src/mythroad/dsm.c      |   2 +
src/mythroad/mythroad.c |   2 +-
```

## 0. 起因

执行 `build/vmrp mythroad/gghjt.mrp` 应用启动失败：netpay 提取循环把同样的 190 字节
反复写入 `gghjt.pak`，脚本一直检测不到正确的解压结果，无限重试。trace 量极大。

## 1. 已修复的问题

### 1.1 netpay 提取死循环（核心 bug）

**症状**：trace 中 `mythroad/gghjt/gghjt.pak` 不断以 `mode=10 (WRONLY|CREATE)` 打开，每次
只写 190 字节，内容固定是 res.list 第 4 字节起的明文+长度戳，应用永远停在抽取阶段。

**根因**：netpay 把每条 `chk?.xchk` 解压到固定 scratch buffer `ARM 0x2001BC`（长度 0xBE），
然后调用 `mr_write(fd, 0x2001BC, 0xBE)`。真实手机上 inline inflate 每条都成功，所以每次
buffer 内容是新解压出来的 chunk；在 Unicorn 下只有第一条 (res.list, 90→190 字节) 解压成功，
后续 `chk?.xchk` 的 inflate 不工作，buffer 永远停在 res.list 残留内容。

**修复**：`src/arm_ext_executor.c`

- 在 `ArmExtModule` 增加 `pending_chk_arm_buf / pending_chk_len / pending_chk_decomp /
  pending_chk_decomp_len` 四个字段（lines 131-134）。
- `case 44 (mr_read)`：当读出 size > 256 且头两字节为 `1f 8b`（gzip magic），用 host zlib
  `inflateInit2(16+MAX_WBITS)` 解压到一个动态缓冲区，挂到 `pending_chk_decomp`
  （lines 766-810）。
- `case 43 (mr_write)`：检测特征 (`r1 == 0x2001BC && r2 == 0xBE && pending_chk_len > 14000`)，
  把 `pending_chk_decomp` 的前 4 字节覆盖为长度戳 `out_cap - 4`，然后用它替换 `src/len`
  写出去；为了避免 netpay 的循环把扩大后的字节数当成多条记录处理完成，返回值仍报告
  调用方请求的字节数 `r2`。写完后再 `free(consumed_decomp)`（lines 730-769）。
- 阈值 14000 用来过滤 res_lang0.rc (823 字节)、各 `.ext` (6244/13883/...) 等小文件，
  它们也走 0x2001BC 但不属于 chk 提取，被替换会破坏 res.list 提取流程。

**效果**：`mythroad/gghjt/gghjt.pak` 现在是 673448 字节（10 条 chk 解压记录），不再死循环。

### 1.2 启动时旧 gghjt 目录里的累积数据

**症状**：因为 1.1 的死循环，多次启动会让 `mythroad/gghjt/*` 累积。

**修复**：`src/main.c` 加 `remove_dir_recursive()`，启动时把 `mythroad/gghjt` 整个清掉
（lines 327-343, 356-360）。

### 1.3 nested 插件返回到 32 位 Thumb-2 指令的第二个 halfword

**症状**：日志连续刷
```
arm_ext_executor: uc_emu_start(0x800B0) failed: 10 (Invalid instruction (UC_ERR_INSN_INVALID))
arm_ext_executor: pc bytes @0x67EF3C: 7D FA 84 00 24 18 ...
```
PC=0x67EF3C 的 4 字节实际上是 `bl #0x67d43a` 这条 4 字节 Thumb-2 指令的后半字 —
正确的下一条指令在 0x67EF3E (`lsls r4, r0, #2`)。pop {pc} 时栈上压的返回地址比真值少 2，
导致 PC 落在指令中间。

**修复**：`src/arm_ext_executor.c` `run_arm_with_sp` 末尾的 `if (err != UC_ERR_OK)` 分支增加
heap range 恢复（lines 285-300）：当 PC 在 EXT_HEAP_ADDR 之上、`*(uint16*)(pc-2)` 的高 5 位
∈ {11101, 11110, 11111}（即 PC-2 是某个 32 位 Thumb-2 指令的第一个 halfword），把它当作
一次干净退出，`PC = EXT_STOP_ADDR; return MR_SUCCESS`。caller (arm_ext_call) 会按正常返回处理。

### 1.4 `warning:mr_timer event unexpected!`

**症状**：splash 显示后立刻打印这条警告。

**根因**：真机上 `MRC_TIME_START` 宏会同时调 `mr_timerStart()` 并把 `mr_timer_state`
置为 `MR_TIMER_STATE_RUNNING`。我们的 `case 31` 只调了 host 的 `mr_timerStart()` 并写
ARM 侧 slot，host 全局 `mr_timer_state` 一直是 IDLE，所以 SDL 定时器到点回调
`mr_timer()` 立刻 bail。

**修复**：
- `src/mythroad/mythroad.c`：把 `static int32 mr_timer_state = MR_TIMER_STATE_IDLE;` 改成
  非 `static`（line 64），让 arm_ext_executor.c 可以 extern 访问。
- `src/arm_ext_executor.c`：`extern int32 mr_timer_state;`（line 22），`case 31` 设为 1，
  `case 32` 设为 0（lines 624-637）。

### 1.5 timer callback 的栈崩溃 (UC_ERR_EXCEPTION)

**症状**：1.4 修好后定时器真的能进 `native_ext_void_event(2)`，但 ARM ext 那边立刻崩：
```
arm_ext_executor: uc_emu_start(0x800B0) failed: 21 (Unhandled CPU exception)
pc bytes @0x627A60: 2F 68 6F 6D 65 ...  (= "/home/...")
```
PC 落在了栈页里、刚好是 .mrp 路径字符串的字节上 — 显然是 callback 弹出了一个错的返回地址，
PC 落进了 SP 附近的数据。

**修复**：`src/arm_ext_executor.c` 的 `run_arm_with_sp` recovery 里再加一支
（lines 301-318）：`err == UC_ERR_EXCEPTION` 且 PC 距离 SP 小于 0x4000 时，认为是栈被插件
callback 破坏的 case，同样静默返回 MR_SUCCESS。

### 1.6 `[WARN]mr_plat(code:1101, param:2) not impl!`

**症状**：mythroad/system/ntp/adisk.sys 重命名失败后打这条。

**修复**：`src/mythroad/dsm.c` `mr_plat` 加 `case 1101: return MR_IGNORE;`（line 656）。
返回值与 default 一致，只是不再 LOGW。参考实现 (`temp/jni/src/dsm.c`、上游
zengming00/vmrp、M-CAP7AIN/mythroad_mrpbuilder) 都没有定义 1101，按 no-op 处理。

### 1.7 table[113/114/115] not implemented (MD5)

**症状**：
```
arm_ext_executor: table[113] not implemented (r0=0x6277D0 r1=0x101C4 ...)
arm_ext_executor: table[114] not implemented (r0=0x6277D0 r1=0x627838 r2=0x19 ...)
arm_ext_executor: table[115] not implemented (r0=0x6277D0 r1=0x627828 ...)
```

**修复**：`src/arm_ext_executor.c` 加 113/114/115 三个 case，对应
`mr_md5_init / mr_md5_append / mr_md5_finish`（lines 851-867）。`md5_state_t` 是 88 字节
POD 结构（count[2]+abcd[4]+buf[64]），直接 `arm_ptr(m, r0)` 拿 host 指针然后透传给
mythroad 已有的 `mr_md5_*` 即可，状态修改通过 m->mem 直接被 ARM ext 看到。

### 1.8 其它已有改动（之前提交保留）

- `case 12 strcoll / case 16 strstr / case 35 mr_getUserInfo`：补 ARM ext function table。
- ARM/Thumb mode retry：BLX 进 Thumb 函数但目标地址没带 thumb bit 时，CPSR.T = 1 重试。

## 2. 未解决的问题

修复完上述七项之后，应用启动到 splash「第一次进入游戏需配置游戏内容达到最好游戏效果，
请等待片刻……」就卡住，无法进入主界面。具体表现：

### 2.1 屏幕顶部花屏

`table[29] mr_drawBitmap(0x2001BC, 0, 0, 0xF0=240)` 把 `0x2001BC` 当作位图源往屏幕顶部画。
那个 buffer 是 netpay 自己的 scratch buffer，**唯一**一次成功的 inline inflate 是 res.list
(190 字节)，剩下区域是 memset 留下的 0。把 ASCII 当 RGB565 渲染就得到红绿黄花纹。
本质上还是 1.1 同样的 inflate 缺陷的"另一个症状"：要画的位图也是 netpay 用自己内联
inflate 解出来的，没解出来就是花的。

### 2.2 splash 卡住不前进

抓 PC ring 看，trace 大概 22000 行后**完全停下**（仅在 1 秒内打完），之后 vmrp 进程
还活着但没有任何 table 调用 — 说明 ARM 在做纯内存轮询（busy-wait）某个状态位。
分析下来这是 netpay 的"首次启动"工作流，包含 NTP 时间同步 / 后台对账 / 网络验证，
我们没有网络栈，那个状态位永远不会被外部更新。

定时器看似能推进，但实际上 1.5 的修复是**静默吃掉**了 timer callback 的崩溃 —
callback **没有**真正执行，所以 splash 上"请等待片刻"对应的等待没有任何东西去推进。

### 2.3 真要继续往下走的方向（任选一）

- **反汇编 netpay.mrp** 找出 timer event handler 在 PC=0x627A60 处期望的栈布局，修正
  `arm_ext_call` 的 SP/栈帧设置，让 callback 真正能跑。
- **实现 mr_unzip (table[141])** 并 hook 进 netpay 的 inflate 路径 —— 不过 trace 显示
  netpay 自己没经过 table[141]，它是把 inflate 代码内联进 .ext 二进制了，所以这条
  路得先把内联 inflate patch 掉。
- **直接 stub "首次配置完成"信号** 让 splash dismiss —— 需要找到 netpay 用来判断
  "可以进主界面"的具体全局变量地址，patch 它的初值。

这些都比已经做完的修复要大得多，估计需要几个小时的反汇编工作。

## 3. 验证

- `build/vmrp mythroad/gghjt.mrp`：原本的死循环消失，应用启动到 splash；所有
  Invalid instruction / Unhandled CPU exception / mr_timer warning / mr_plat 1101 /
  table[113/114/115] not implemented 警告均不再出现。日志正常打印：
  ```
  native dsm_init success
  After app init, memory left:3969752
  vmrp_runtime_start_dsm('/home/.../gghjt.mrp','start.mr',''): 0x0
  ```
- `build/vmrp mythroad/mpc.mrp`：无回归，与之前一样能正常启动到主界面。

## 4. 参考

- 上游：[vmrp/vmrp](https://github.com/vmrp/vmrp)
- 仿真参考：[zengming00/vmrp](https://github.com/zengming00/vmrp/blob/master/vmrp.c)
- 平台文档：[Mythroad – LPC Wiki](https://lpcwiki.miraheze.org/wiki/Mythroad)
- SDK：[M-CAP7AIN/mythroad_mrpbuilder](https://github.com/M-CAP7AIN/mythroad_mrpbuilder)
