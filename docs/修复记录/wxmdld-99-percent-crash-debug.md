# wxmdld 99% 崩溃调试记录

日期：2026-07-17

## 结论

`build/vmrp build/mythroad/wxmdld.mrp` 的 99% 崩溃由两个通用 ARM EXT
兼容问题叠加造成，现均已修复：

1. table bridge 的直接 `LR` 落在共享 wrapper thunk 内，隐藏了真正发起
   `table[125]` 的 nested child。资源读取因此错误使用外层 MRP，返回了不属于
   当前引擎的数据。
2. guest 按原生 32 位 `mem.c` 语义构造了一个回绕到 `LG_mem_base` 以下的
   free-list 节点；host 在执行 32 位 `base + offset` 之前按 offset 大小拒绝它，
   破坏 first-fit 地址预测。冷启动生成阶段随后把数据写到 bump 返回值，而
   guest 继续读取预测地址，最终缺少 `pet` 定义并解引用空对象。

修复不包含应用名、包哈希、`pet` 字符串、固定场景或错误吞并。最终新目录
冷启动和同目录暖启动均没有 `UC_MEM`、`UC_ERR` 或
`uc_emu_start failed`，进入 240x320 游戏标题画面并响应真实 E2E 按键。

## 样本与约束

- MRP：505794 字节，SHA-256
  `8a0d57d6952412cb9bf7ca21766b0a7831ccf8de90aded3cebef2bcb5b32c0e8`。
- 提取的 `flaengine.ext`：129312 字节，运行基址 `0x227890`，SHA-256
  `da95583347450358a093fc0c95687cdff52438728e63dca9309f59066da6ca8b`。
- 不使用 Xvfb；SDL dummy 只提供可重复的无窗口 surface 和 PPM。
- 不开启全量 PC/内存 trace，只使用 table 边界、精确 PC 和单字段/单地址观察点。
- 不通过增加默认内存、映射坏地址、补写字典项或异常兜底改变应用行为。

## 初始复现

默认 1MB 应用内存稳定停在“冒泡 FLASH 游戏 / 99%”：

```text
UC_MEM_READ_UNMAPPED addr=0x7801023E size=1
PC=0x23E128 (flaengine.ext+0x16898), LR=0x23E123
R4=0x7801023D R9=0x2471B4 SP=0x345A8C
```

崩溃指令是 `ldrb r2,[r4,#1]`。初始 PPM 为 P6 240x320，SHA-256
`826856f124fa8b49faaf12e7e2ba764390dd85f45d5febdbf6a29ba714ad59f5`，
确认不是日志报错后仍在后台推进。

1M、2M、4M、8M、16M 的隔离对照还表明增大内存不是修复：1M 在 99%
访问 `0x7801023E`；其余档位在 60%、`PC=0x2389CA` 访问 `0x010CC9D4`。
内存档位只改变了错误资源和分配布局暴露坏状态的位置。

## 第一根因：wrapper thunk 隐藏 nested owner

反汇编与运行现场证明，`table[125]` 的直接 `LR` 位于共享 wrapper 代码段，
而真正调用者的 Thumb 返回地址保存在 thunk 的小栈帧中。旧逻辑只按直接
`LR` 查 nested module，因此 owner 为空并使用外层 MRP。

`arm_ext_resource_owner_for_lr()` 现在保留直接命中路径，只在直接 `LR` 属于
wrapper 代码段且没有 nested owner 时，扫描最多 16 个栈 word。候选必须同时：

- 带 Thumb 位；
- 落入已登记 child 的代码区；
- 返回地址前一条半字指令满足 `BLX Rm` 编码。

这三个结构条件证明候选是实际调用帧，而不是碰巧落入代码 RAM 的数据。修复后
四个关键 `abc` 资源长度 `27791`、`76016`、`94289`、`434627` 都从正确
资源 owner 读取。没有使用 active/foreground 状态作为模糊 fallback。

## 第二现场：冷启动缺少 `pet`

owner 修复后，已有缓存的暖启动成功，但全新 work-dir 冷启动改为：

```text
UC_MEM_READ_UNMAPPED addr=0x38383838
PC=0x239644
```

`0x38383838` 是空 lookup 结果继续传播后的二次症状。失败 key 为标准
`h = h * 31 + byte` 得到的：

```text
hash31("pet") = 0x1B11F
```

这不是 hash、查询 owner 或 key 解析错误。精确断点得到：

- 父解析链 `0x233D60 -> 0x233DEE -> 0x23A9F4` 确实执行；
- generated child `0x40018800` 只在稍后登记，不负责注册 `pet`；
- 两个 parser object 是 `0x2B4F74`、`0x2AA4B4`；
- 两者 `[object+0x48]` definition count 都是 0；
- 两次解析都在 `0x23AA86` 合法跳过 `0x23AACE -> 0x239528` 定义循环；
- pair 表仍正确产生 key `0x1B11F`，但全局字典查询返回 NULL。

正确暖启动中，definition index `r7=0x106` 经 `0x2395B6 -> 0x239840`
把 key `0x1B11F`、value `0x3C7E58`、attr 0 插入 owner `0x348B24`。
失败查询链为
`0x233E2C -> 0x244148(index 0) -> 0x229C8C -> 0x2328E0 -> 0x2396F8`；
冷启动返回 0，正确进程返回 `0x3C7E58`。所以补写 `"pet"`、修改 lookup 或
强迫 generated child 初始化都会掩盖真正的上游数据问题。

三态隔离进一步确定了注册时序：

| 状态 | 结果 |
| --- | --- |
| 干净首进程 | 生成新 SKY 和 child 后，同进程缺少 `pet` |
| 已有正确缓存的暖进程 | 父解析器先注册 `pet`，随后 lookup 成功 |
| 保留首轮新 SKY、删除 `avm_cache` 后重启 | 仍会先注册 `pet`，并重新生成正确 avm |

因此 `pet` 由父 `flaengine.ext` 消费 SKY/definition blob 时注册，不是 generated
child 注册。私有 wrapper 在 `table[131]` 返回后会 `BLX file_base+8`，相对
`0x8` 才是 child initialization entry；曾怀疑的 `+0x439D` 只是 helper/callback。

## 反汇编闭环：guest 依赖 first-fit 地址预测

`flaengine.ext` 的 `0x2431B8` 函数完成以下操作：

1. 预先取得后续代码将直接消费的目标输出地址；
2. 经 `0x23FDB8` 直接编辑 Mythroad `LG_mem_free` 链；
3. 把预测目标挂成 first-fit 首节点；
4. 调 `table[125]` 读取 `abc`；
5. 不保存 `table[125]` 返回指针，继续从预测地址消费数据。

冷启动 sky 转换阶段的一条链为：

```text
LG_mem_base = 0x225E5C
relative offset = 0xFFFDA360
(0x225E5C + 0xFFFDA360) modulo 2^32 = 0x2001BC
```

运行日志在 `0x2001BC` 看到 little-endian 头字节
`60 A3 FD FF`，与 guest 写入的 offset 完全相等。原生
`src/mythroad/mem.c` 使用：

```c
nextfree = (LG_mem_free_t *)(LG_mem_base + previous->next);
```

ARM 目标上的指针和 `uint32` 都是 32 位，因此该加法会自然回绕到
`0x2001BC`。节点虽然低于 base，但完整映射且低于当前 `LG_mem_end`，原生
`mr_malloc` 会遍历并接受它；只有 `mr_free` 明确拒绝 `p < LG_mem_base`。

## Host 偏差与修复

旧 `arm_ext_origin_node_read()` 在地址还原之前执行：

```c
if (next_off > arena->end - arena->base) return 0;
```

所以合法 offset `0xFFFDA360` 被当成普通正向长度判坏，guest first-fit 失败并
退到 bump。冷启动中 `76016`、`94289`、`434627` 三个关键读取曾分别返回
`0xEF4D8C`、`0xF0767C`、`0xF1E6D0`；应用却继续读取预测地址。准备链失配后，
后续 definition blob 没有落到预测的 `0x24A404`，parser count 保持 0。

修复只调整通用 free-list 解码和验证：

- `next_addr = (uint32_t)(base + next_off)`，先按 guest 32 位语义还原地址；
- `next_addr == end` 保持链尾哨兵；`next_addr > end` 仍判坏；
- 普通节点允许低于 base，但必须有完整 8 字节头、非零且不溢出的长度、
  完整连续映射并位于 end 前；
- malloc 遍历仍拒绝已登记 executable storage，循环仍有结构上限；
- 结构上限统计 end 以下所有映射 band，使合法低地址节点也计入容量；
- free 路径的 `p < base` 拒绝保持不变。

另外，`table[43]/mr_write` 改为验证整个 guest source span。初始坏状态曾把
`src=0x6C012F,len=0xFFBE0018` 交给 host `write(2)`，Linux 在越过 16MB guest
backing 后才以 EFAULT 返回，造成宿主越界读和误导性 partial write。现在未映射
span 直接按 ABI 返回 `MR_FAILED`；这是一项必要边界修复，但不是 `pet` 缺失的
上游根因。

## 冷启动验证

最终使用复制了基础 `mythroad/` 运行树的全新目录
`/tmp/vmrp-wxmdld-fixed-e2e-5QhMYB`，没有预置 `sky.bmp` 或 `avm_cache`。
关键运行证据：

```text
table125 fl=76016  -> ret=0x24A404
table125 fl=94289  -> ret=0x24A404
table125 fl=434627 -> ret=0x24A404
table43 avm_cache len=219848 -> ret=219848
```

sky 转换期间的地址观察还记录了多次
`source=origin addr=0x2001BC`，证明 wrapped lower-address 节点实际进入了
first-fit，而不是再次退 bump。生成结果：

- `mythroad/avm_cache`：219848 字节，SHA-256
  `d9d91908442aad3e766ba357bfa5f1a430a5bb9baf691a2db5b054d1f75d572f`；
- `mythroad/cache/wxmdld/sky.bmp`：1196230 字节，SHA-256
  `2d2708d82c46ef5a830b9f9635bd60b4e21e795fa6cf9be9e8dc6850615e9565`；
- E2E socket 就绪，标题首帧 draw count 56；
- P6 240x320 标题 PPM 有 196 种颜色，SHA-256
  `098e2b474feb2f9217d5bbf87b6fbf50e8bf088c13e2b82d15ce39a4fd5536cb`。

删除本轮临时 write/alloc probes 并重新编译后，又在全新目录
`/tmp/vmrp-wxmdld-final-ot2skl` 重复冷启动；它生成完全相同大小和 SHA-256 的
两个缓存，到达相同 draw count 56 和标题 PPM hash，最终日志同样没有任何
Unicorn/FATAL 标记。以上地址级日志来自保留的诊断运行，最终生产代码不新增
临时环境变量或 hook。

按 `ENTER` 后 draw count 从 56 增至 77，画面显示“请稍后...”，证明按键已
进入游戏自己的启动分支。随后游戏尝试连接其服务器
`122.224.212.135:18080`；当前网络连接失败后保持等待属于应用联网行为，不是
99% 崩溃或 emulator 内存错误。

## 暖启动与交互验证

在同一 work-dir 使用已生成缓存重启：

- 没有 `UC_MEM`、`UC_ERR`、`uc_emu_start failed` 或 `FATAL`；
- 标题首帧 draw count 为 48，PPM 与冷启动标题逐字节相同；
- E2E `KEY DOWN` 成功，draw count `48 -> 49`；
- 前后 PPM 改变 1138 个像素，变化边界严格位于菜单区
  `x=84..154,y=239..284`；
- DOWN 后 SHA-256 为
  `2e34259c05bbc975e8baef15749afd68b3c32f7fa4d8dad9ef2047d1ffbb3b88`；
- E2E `KEY UP` 后 draw count `49 -> 50`，PPM SHA-256 精确恢复为
  `098e2b47...5536cb`；
- E2E `QUIT` 返回 `OK quit`，进程退出码 0。

这同时证明 socket、真实按键派发、draw 递增、PPM、全图 hash 和区域 pixel
diff；结果不是加载页上的动画变化。

## 默认 work-dir 的历史坏缓存

完成源码修复后，用户再次执行原命令：

```bash
build/vmrp build/mythroad/wxmdld.mrp
```

加载虽然结束，但画面基本全黑。该命令默认使用可执行文件目录 `build/` 作为
work-dir；检查发现其中保留了旧版崩溃时生成的
`build/mythroad/cache/wxmdld/sky.bmp`：

- 文件大小仍是 1196230，头 4 字节也仍是 `B6 4C 0D 4F`；
- SHA-256 却是
  `34664c5a0af5451a85599252636da3abc2731157eb04736ffac60e70ff1e8792`；
- 头后图像区从第 5 字节起大面积为 0；
- `avm_cache` 已是正确的 219848-byte/hash，`exdet.dat` 也与干净运行一致；
- 运行没有 Unicorn 错误且 draw count 为 48，但 PPM 只有 8 种颜色、33 个
  非黑像素，SHA-256
  `548abccec290de586d4cc399e421ba22beec42f7a375ea4f5c86d8323e2ce877`。

所以这是旧 crash 留下的同长度未完成 SKY，不是当前渲染器把正确资源画黑。
应用自己的 metadata 只标识资源版本，不包含 SKY 内容校验，无法区分它与完整
文件。

先在默认运行树副本中只删除 `cache/wxmdld/sky.bmp`，保留 `avm_cache`、
`exdet.dat` 和其它状态。下一次启动应用自行重建出正确
`2d2708...9565` 文件，PPM 恢复为 196 色、76026 个非黑像素和标题 hash
`098e2b47...5536cb`。随后只清理默认 `build/` work-dir 中同一个旧生成物并
原样重跑用户命令，得到相同文件/hash、draw count 48 和完整标题画面，日志无
`UC_MEM`、`UC_ERR`、`uc_emu_start failed` 或 `FATAL`。

没有为此增加按文件名、应用名或内容 hash 自动删除的运行时代码，也没有清空
整个 cache 或用户状态。该文件是本次调试期间由已证实有错误的旧二进制产生的
工作区产物；当前修复后的全新冷启动会直接生成完整文件。

## 回归与保留证据

- `cmake --build build --target vmrp -j2`：通过；
- `git diff --check`：通过；
- `pnpm exec tsc --noEmit`：通过；
- `ctest --test-dir build --output-on-failure`：历史 build-tree 的
  `vmrp-unit` 通过，但当前分支已从 CMake/源码树移除该测试，不能把它当成
  当前源码重新构建的单测证据；
- 清理临时诊断 hook、重新编译最终源码后的 `pnpm test:e2e`：22 个文件、
  43 个测试全部通过，耗时 300.94 秒；同一最终二进制还完成了上述全新目录、
  同目录暖启动和默认 `build/` work-dir 三组目标验证。

关键诊断日志：

- `/tmp/wxmdld-ledger-cold.log`；
- `/tmp/wxmdld-ledger-restart.log`；
- `/tmp/wxmdld-abc-free-watch.log`；
- `/tmp/vmrp-wxmdld-parserbranch-jBYJZQ/parser-branch.log`；
- `/tmp/vmrp-wxmdld-seq-a4GWJo/sequence.log`；
- `/tmp/vmrp-wxmdld-sourcefield-OEbny0/source-field.log`；
- `/tmp/wxmdld-fixed-e2e.log`；
- `/tmp/wxmdld-fixed-warm.log`。
- `/tmp/wxmdld-default-black.log`；
- `/tmp/wxmdld-stale-recovery.log`；
- `/tmp/wxmdld-default-rebuilt.log`。

## 已证伪方向

- 增大默认应用内存；
- 给 `0x7801023E`、`0x38383838` 增加映射；
- 修改 hash/lookup 或直接补 `"pet"`；
- 强迫 generated child `file+8` 初始化；
- 按应用名、哈希或场景自动回收 `table[125]`；
- 用 active/foreground child 猜资源 owner；
- 把 `table[43]` partial write 当作最初根因；
- 吞掉 `uc_emu_start` 错误继续运行。

## 窄断点索引

以下为 `flaengine.ext` 相对文件偏移，可用已有 `VMRP_ARM_EXT_WATCH_PC` 重建
parser/insert/lookup 证据，无需开启全量 trace：

```text
C4D0   parser driver 0x233D60
C52E   loaded blob pointer 0x233DBE
C55E   legacy parser call 0x233DEE -> 0x23A9F4
131F2  definition count 0x23AA82
1323E  definition loop call 0x23AACE
11C98  property-definition parser 0x239528
11D26  exact insertion call 0x2395B6
11FB0  dictionary insert 0x239840
C59C   root-symbol resolver 0x233E2C
B050   global lookup 0x2328E0
B066   lookup return 0x2328F6
```
