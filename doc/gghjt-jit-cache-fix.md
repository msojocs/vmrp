# gghjt.mrp 根因修复：Unicorn JIT cache 在嵌套 ext re-staging 时未失效

提交基线：`b5382e7 fix: 罪恶都市游戏包释放失败`（以前那次只到 splash，停在 BLX 进数据的崩溃上）
本次修复：`src/arm_ext_executor.c` +31 行
现象：splash 通过，进入 NTP 阶段，开始读 `pyone.sys`，控制台 0 错误。

## 0. 一句话结论

不是 GOT 重定位错了，是 **Unicorn / QEMU TCG 的 translation block cache 在 wrapper 把新 .ext 字节写到同一段内存后没失效**，PC 进了新 .ext 的代码区，执行的却是上一个 .ext 留下的旧翻译。

修法：在 `case 131 (r1 == 9)`（也就是 ARM 侧的 `mr_cacheSync`）末尾对刚 staging 的 `[r2, r2+r3)` 区间调 `uc_ctl_remove_cache`。一条调用就修好了 splash 通过这一段所有的"鬼指令 + BLX 进数据"现象。

## 1. 故障现场

提取阶段（gghjt.pak 673448 字节、10 条 chk）一切正常。splash「请等待片刻」画出来后，trace 22000 行处稳定崩在：

```
arm_ext_executor: stack-corrupt plugin callback pc=0x627A60 sp=0x6278A0 (treated as clean exit)
```

`0x627A60` 不是代码——是 gghjt.mrp 的路径字符串 `/home/msojocs/.../gghjt.mrp` 在堆里的某个副本。

之前一轮的 fix report（`doc/gghjt-fix-report.md` §2）判断是 cfunction.ext 给嵌套 ext 做 GOT 重定位时基址用错了。这个判断是**错的**——具体看下文 §3 的反复推演。

## 2. 排查工作流

### 2.1 第一轮误判：以为是 GOT 重定位

按现场 dump：
- 出错指令是 `BLX r3`，位于 memory `0x67E0E2`
- LR = `0x67E0E5`（说明 BLX 在 `0x67E0E2`，正确）
- r9 = `0x6828AC`
- 旁边的字面池 0x67E1AC 处值是 `0x1488`
- 算出来 r3 应当来自 mem[`r9 + 0x1488`] = mem[`0x683D34`]

那个槽里如果是 `0x10038`（table[14] = memset 入口），调起来就是正常的 memset；但崩在 `0x627A60` 上说明取到的是 `0x627A61`，正好等于 `gghjt 的某缓冲区基址 + 一个小偏移`，所以第一轮的推论是「wrapper 在做 GOT 重定位时把 code base 算错了，netpay 子插件的函数指针都指到 gghjt 数据区了」。

写了一份 `doc/gghjt-fix-report.md`，开了一堆 recovery（栈崩溃静默吃掉、blx-into-data 时根据寄存器签名模拟 memset / memcpy 等）。这些 recovery 让程序不崩了但 splash 仍然推不动，因为它们都是在错误已经发生之后做的补救。

### 2.2 第二轮：真的去读 cfunction.ext / advsms.ext 反汇编

`arm-none-eabi-objdump -D -b binary -m arm` 和 `-M force-thumb` 把 gghjt.mrp 内嵌的 cfunction.ext（19428 字节，**不是**仓库根目录的那个 332116 字节的 standalone 版本，那个版本里同样的 trace 跟不上）以及 netpay.mrp 内嵌的 smcheck.ext / advsms.ext 都拉出来逐字节看。

关键观察：

1. 所有 .ext 的 entry 都是同样的 boilerplate：`push {r4, lr}; mov r4, r0; mov r0, r4; BLX <loader>; pop {r4, pc}`，BLX 目标是文件内某个 Thumb 函数。
2. cfunction.ext 的 wrapper 启动后通过 table[25] (`mr_c_function_new`) 注册自己的 dispatcher 在 `0x800B0`。
3. wrapper 给每个嵌套 ext 做了一份**单独的 table 拷贝**（看 trace 里 `table[3](0x6EE490, 0x10000, 0x248, …)`：拷贝 0x248 字节，正好是 146 个 4 字节槽）。在这份拷贝里 table[0] / table[1] / table[25] 被改成指向 wrapper 自己的实现（`0x828E1`、`0x82865`、`0x83225`），其余还是指回我们 host 这边的 hook。所以子插件 `blx r2` 走 table[25] 时根本不会 fire 我们的 case 25——它是 wrapper 在内部处理。
4. case 131 (r1 == 9) 实际是 `_mr_TestCom1(NULL, 9, ptr, len)`，定义在 `src/mythroad/mythroad.c:3399`：

   ```c
   case 9:
       mr_cacheSync((void*)((uint32)(input1) & (~0x0000001F)), …);
       return 0;
   ```

   就是个 cache sync。**它根本不负责把 .ext 字节从某个源拷过去**——我们当前的 case 131 hook 里那段 "若 m->last_file_copy 在、把它 memcpy 到 r2、并把头 4 字节改成 table_addr" 是一条根本不会进的分支（trace 里 m->last_file_copy 一直是 NULL，因为 case 125 在这个执行流里压根没被调用）。

   那 advsms 的二进制怎么进的 `0x67D240`？是 wrapper 自己用 inline inflate / 一堆 table[3] memcpy 写过去的。

5. **关键证据**——把硬件 trace 钩在 `0x67E0DA`（advsms file 0xE9A 的 `ldr r3, [r3, #0]`，理论上每次进函数都该走一次）上，只要它执行就计数，结果计数 = 0：

   ```
   [trace] BAD BLX at 0x67E0E2, r3=0x6278A0 r9=0x6828AC lr=0x67F95B (saw_DA=0)
   ```

   PC 序列直接从 `0x67E0D8` 跳到 `0x67E0DC`，**`0x67E0DA` 那条 LDR 没有被执行**。这就解释了所有 dump 都看着完全合理的怪事：寄存器和内存都没问题，**指令流跟字节流对不上**。

6. 第二次 staging 复盘：smcheck.ext 先被 staging 到 `0x67D240`（0x5804 字节），它的代码跑过、被 Unicorn TCG 翻译成 TB 缓存住；然后同一段内存被 advsms.ext（0x5664 字节）**覆盖**。覆盖之后 PC 进 `0x67E0D6` 区域，Unicorn 用的还是缓存里 smcheck 的翻译——这条翻译过的 block 里根本就没有 advsms 的那条 `ldr r3, [r3, #0]`。所以 r3 一直停在调用方留下的 SP 值，`mov r0, sp` 之后 `BLX r3` = `BLX sp`，PC 跳进栈数据。

   smcheck.ext 在 `0xE9A` 是 `adds r2, r0, #0`，跟 advsms 的 `ldr r3, [r3, #0]` 是不同的指令（之前我们看到 r2 莫名其妙地变了，就是 smcheck 的 `adds r2, r0, #0` 在 advsms 的 PC 上执行）。

### 2.3 验证假设

`Unicorn` 头文件里搜 `TB_FLUSH` / `TB_REMOVE` 一找到：

```c
#define uc_ctl_remove_cache(uc, address, end)                                  \
    uc_ctl(uc, UC_CTL_WRITE(UC_CTL_TB_REMOVE_CACHE, 2), (address), (end))
```

正好是 QEMU TCG 失效特定区间 translation 的 API。在 case 131 r1==9 末尾加一条调用，重编，重跑。崩溃没有了，splash 通过，进入 `mythroad/system/ntp/pyone.sys` 读取，控制台 0 个 `arm_ext_executor:` 错误。

## 3. 修复内容

`src/arm_ext_executor.c`（+31 行）：

### 3.1 case 131：staging 完成后失效 TB cache

```c
case 131:
    ret = _mr_TestCom1(NULL, (int)r1, arm_ptr(m, r2), (int32)r3);
    if (r1 == 9 && m->last_file_copy && r2 && r3 <= m->last_file_len && arm_ptr(m, r2)) {
        memcpy(arm_ptr(m, r2), m->last_file_copy, r3);
        ...
    }
    /*
     * 关键修复：case 131 (r1=9) 是 mr_cacheSync。原意是同步指令缓存，
     * 因为 wrapper 刚把新的 .ext 二进制写到了 [r2, r2+r3) 范围。在真机
     * 上这会清掉 D-cache 并 invalidate I-cache，让后续取指看到新指令。
     *
     * 我们用 Unicorn 跑 ARM，Unicorn 通过 QEMU TCG 做 JIT 翻译并 cache
     * translation blocks（TB）。当 gghjt 把 netpay 的 smcheck/advsms
     * 等子插件 staging 到同一段内存时，TB cache 仍然是上一个插件留下的
     * 翻译结果，结果就是 PC 进了新 .ext 的代码区，执行的却是旧代码。
     */
    if (r1 == 9 && r2 && r3 > 0 && arm_ptr(m, r2) && arm_ptr(m, r2 + r3 - 1)) {
        uc_err cerr = uc_ctl_remove_cache(m->uc, r2, r2 + r3);
        ...
    }
    break;
```

### 3.2 补三个 table

修好 TB cache 之后 trace 露出来三个之前根本没机会到达的「table 未实现」：

```c
/* table[61] mr_getNetworkID()：返回 0 表示移动网络（MR_NET_ID_MOBILE） */
case 61: ret = 0; break;
/* table[81] mr_initNetwork(cb, mode)：直接返回 MR_FAILED，让插件按
 * "网络不可用" 路径走。我们没有可靠的方式回调 ARM 侧 cb，且 netpay
 * 的网络功能本来就在我们的桌面环境中不可用。 */
case 81: ret = MR_FAILED; break;
/* table[82] mr_closeNetwork()：return success，跟 mr_initNetwork 配对。 */
case 82: ret = MR_SUCCESS; break;
```

## 4. 验证

| 用例 | 结果 |
|---|---|
| `build/vmrp mythroad/gghjt.mrp` | gghjt.pak 673448 B；splash 通过；生成 `mythroad/gghjt/data~.bin` 146880 B；开始读 `mythroad/system/ntp/pyone.sys`；控制台 0 个 `arm_ext_executor:` 错误行；不再出现 `stack-corrupt callback` / `mid-Thumb2` / `not implemented` 任何提示。trace 长度 91739 行（之前是 22098 行就崩溃了）。 |
| `build/vmrp mythroad/mpc.mrp` | 无回归，0 错误。 |

## 5. 经验复盘

1. **现场 dump 看着合理 ≠ 没问题**。寄存器、内存、字面池、GOT 写入全是对的，唯一的不对是「这条指令根本没执行」。在 JIT 模拟器上排查崩溃要把"指令流跟字节流是否一致"放在很前面去验证。

2. **TB cache 失效要在 self-modifying code 出现的所有点都做**。本仓库现在已知的 self-modifying 入口就是 case 131 r1==9 一处；如果将来出现其它的（比如 wrapper 用别的方式把内容写到运行过的内存），需要在那个点也补 `uc_ctl_remove_cache`。一个保守的兜底是改成对**所有**写入堆 (`>= EXT_HEAP_ADDR`) 的 table 调用都做 TB cache invalidate，但代价是 splash 阶段每次 memcpy 都要重新翻译，性能影响要测。当前选最小代价方案——只 hook cache sync 这一个点，跟真机语义对齐。

3. **不要乱写 recovery**。之前一轮在 `run_arm_with_sp` 加了五六层「BLX 进数据时根据寄存器猜函数」「读未映射内存自动跳过」「pop 一帧栈」一类的兜底，跑起来不崩了但 splash 还是推不动——因为崩点之后整个状态机已经错乱，靠 host 端模拟无论如何也救不回来。**正确的修法永远是去找上游**。

4. **走真机语义最靠谱**。case 131 r1==9 在真机上就是个 `mr_cacheSync`；在模拟器上对应的语义动作是 invalidate TB cache。两边语义对齐之后整个问题消失。

## 6. 后续待办（可选）

- splash 通过之后是反复的 `mr_open(mythroad/system/ntp/pyone.sys, 1); mr_close`——netpay 在 retry 某个网络初始化相关的步骤。trace 中没有 `arm_ext_executor:` 错误，所以这是 mythroad / DSM 层面（host C 代码）的事件循环，与 ARM 模拟器无关。要让游戏真正进主界面还需要看 NTP 子系统的更上层逻辑（不在 `arm_ext_executor.c` 范围内）。
- 仓库根目录 `cfunction.ext`（332116 字节）跟 gghjt.mrp 内嵌的那一份（19428 字节）显著不同。如果以后需要复盘 wrapper 行为，记得用**实际加载的那一份**：从 `mythroad/gghjt.mrp` 里按 mrp 索引提取出来。

## 7. 参考

- 提取脚本（Python）：见本次会话中 §2.2 用过的 mrp 索引解析代码，从 `offset = 240` 开始按 `{name_len, name, off, packed_len}` 三元组迭代，遇到 `\x1f\x8b` 头要 gunzip。
- Unicorn API：`uc_ctl_remove_cache(uc, addr, end)`，header `unicorn.h:610`。
- mythroad 侧 `mr_cacheSync` 的实现：`src/mythroad/mythroad.c:3399`（`_mr_TestCom1` 的 `case 9`）。
