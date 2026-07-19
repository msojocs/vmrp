# mpc.mrp 退出菜单无响应——递归根因定位与交接文档

> 用途：当前会话上下文吃紧，把所有现场调查结果整理到这里，下个会话可以直接接着做修复。
> 时间：2026-05-24。
> 涉及代码版本：`HEAD = 9bec591 fix: mpc退出失败` 之上的工作区改动。

---

## 0. TL;DR

- 现象：`build/skyengine mythroad/mpc.mrp` 启动后，从主界面"软左 → 退出 → 确定"三连点（坐标 `(23,305)/(30,280)/(9,310)`）应当让 app 自行退出，**实际不退出**，进程一直挂着直到外部 timeout。
- 根因（已确定）：mpc.mrp 自带的 19KB 引导模块 + 157KB 主模块在退出确认那条事件分发链上构成一个**参数恒定的同尾自递归**：
  ```
  F(0x84298) → 0x843C4 → veneer 0x662B5C → 0x838B4 → F(0x84298) → …
  ```
  每一层都拿到完全相同的 `(r0=0x702CB0, r1=10)`，没有任何自然终止条件；最终 ARM 侧 SP 漂入代码段被 `silent-swallow` 当作"事件回调正常返回"，于是真正应该走的 `mrc_exit()` → `mr_exit()`（table[54]）始终没有触发。
- 闭环完全发生在 mpc.mrp 的字节码空间内（19KB ↔ 157KB），整条递归期间 `table[31]/[32]/[54]` 一次都没被调到，所以靠"改 skyengine 的 loader 顺序 / SDK 同步性"修不了。
- 当前源码状态：把以前几版本的"启发式兜底"（busy_wait_count → mr_exit、event_did_work、SP-in-code → mr_exit、`sleep(1)` 等）全部撤掉了，留下了一条标红的回归用例 `test/mpc_exit_menu_terminates_app.sh` 作为档案。

---

## 1. 复现与验证

### 1.1 跑应用

```
cd /home/msojocs/github/skyengine
build/skyengine mythroad/mpc.mrp
```

### 1.2 用环境变量注入点击序列（无需手工点）

`src/main.c` 已经实现 `VMRP_AUTO_CLICKS`（前一次会话加的，留在工作区里）：

| 变量 | 用法 |
|---|---|
| `VMRP_AUTO_CLICKS` | `"x1,y1;x2,y2;..."`。`x = -1, y = 0` → ESC；`x = -2, y = SDLK_*` → 任意按键 |
| `VMRP_AUTO_CLICK_DELAY_MS` | 注入间隔，默认 800ms |
| `VMRP_ARM_EXT_TRACE` | 打开 hook_table 的每一次 table[X] 调用打印（量很大） |
| `VMRP_ARM_EXT_TRACE_PC` | 打开 64 项 PC ring buffer，仅在异常退出时 dump |

两个标志性点击序列：

| 用途 | `VMRP_AUTO_CLICKS` | 预期 |
|---|---|---|
| 正常菜单（不应退出） | `"17,300;31,263;127,272"` | 三次 click ret=0，进程持续运行 → timeout 杀掉 |
| 退出菜单（应退出） | `"23,305;30,280;9,310"` | 看到 `mr_exit() called by mythroad!` + `mythroad exit.`、exit code = 0 |

### 1.3 测试脚本

`test/` 目录里有用例（前一次会话写好的，不含"出错也算通过"那种兜底）：

```
test/lib.sh                              # 严格断言基础库
test/mpc_menu_click_keeps_app_running.sh # 当前 PASS
test/mpc_exit_menu_terminates_app.sh     # 当前 FAIL（已知 bug 档案）
test/run_all.sh                          # 跑所有 mpc_*.sh
```

跑法：

```
cd test && ./run_all.sh
```

当前结果（去掉所有兜底之后）：
```
passed: 1    failed: 1
FAILED:
  - mpc_exit_menu_terminates_app.sh
```

---

## 2. 关键运行时事实

### 2.1 mpc.mrp 实际加载布局

mpc.mrp 启动到点击退出全程，`case 800` 只触发了**一次**：

```
[DEBUG case800] len=19532 code=0 native_ext=(nil)
```

也就是说"先加载 cfunction.ext（332KB）再被 case 800 覆盖"的那个假设的前提条件，在 mpc.mrp 上**根本不成立**——mpc.mrp 直接把它自己的 19KB 引导模块当 primary 装上。在 native 编译里 `loadCode()` 只在非 native VM 编译路径下使用，所以 native skyengine 启动时本来就没动 cfunction.ext。

| 加载步骤（按时间顺序） | 大小 | 落地位置 | 备注 |
|---|---|---|---|
| `mr_get_method(1018)` | 2,490 字节 | 不是 ext，应该是 start.mr 之类 | 不走 case 800 |
| `mr_get_method(13382)` → `case 800` | 19,532 字节 | `EXT_BASE_ADDR=0x10000~`、code 落 `0x80000`、长 `0x4C4C` | **primary ext，含 0x838B4 / F=0x84298 / 0x843C4** |
| `mr_get_method(84090)` → case 131 r1=9（mr_cacheSync） | 157,040 字节 | 0x646380 ~ 0x66C8F0 | **nested ext，含 veneer 0x662B5C / 0x662BA8** |

文件 dump 落点（前一次会话生成，可以参照）：
- `/tmp/skyengine_ext_code.bin`：19KB primary（0x80000 区段在 entry 命中时的内存快照）
- `/tmp/skyengine_nested_code.bin`：157KB nested（0x646380 区段）

> 提示：每次 skyengine 重新跑会重新 alloc，绝对地址（0x702C74、0x749860 这类堆地址）会变；`0x80000+`、`0x646380` 的 VMA 是稳定的（受 EXT_BASE_ADDR / EXT_HEAP_ADDR / arm_alloc 顺序决定）。

### 2.2 递归链结构

每层栈帧 **120 字节** = 4 个函数的栈帧之和（实测稳定）：

| 函数 | 位置（VMA） | 入栈量 | 角色 |
|---|---|---|---|
| `0x84298` (F) | 19KB primary | push `{r3,r4,r5,r6,r7,lr}` = 24 | "注册定时器到链表"。末尾 `bl 0x843c4` **无条件** |
| `0x843C4` | 19KB primary | push `{r3,r4,r5,lr}` + sub sp #?? ≈ 16 | 取当前时间 + 调两条 veneer |
| `0x662B5C` 或 `0x662BA8` | 157KB nested | push `{lr}` + sub sp #20 = 24 | 从 `ctx.field_40` 取出 `0x838B4` 再 `blx ip` |
| `0x838B4` | 19KB primary | push `{r0..r7,lr}` + sub sp #20 = 56 | 跨 ext r9 切换 + 派发到 F / 0x84238 |

链条：

```
F (r0=0x702CB0, r1=10)                       ← 第 n 层
 └─ bl 0x843C4 (无条件)
     ├─ blx (struct.field_56).field_128 = 0x662BA8  ← lr=0x843EF
     │    └─ blx ip = 0x838B4 (r5=1)            ← lr=0x662BE4
     │         └─ case r5=1 → bl 0x84238（链表移除，无递归）
     └─ blx (struct.field_56).field_124 = 0x662B5C  ← lr=0x843F9
          └─ blx ip = 0x838B4 (r5=0)            ← lr=0x662B9C
               └─ case r5=0 → bl F (recurse!)
                    └─ F (r0=0x702CB0, r1=10)   ← 第 n+1 层（同样参数）
```

`F` 入口 16 次采样（`VMRP_F_PROBE=1`，hook 加在 `0x84298`/`0x8429A`），参数恒定：

```
#0  r0=0x749860 r1=0xA lr=0x81AA9      ← 第一层从 0x80F9C(:0x81AA4) 调入
#2  r0=0x702CB0 r1=0xA lr=0x83917      ← 之后全部递归层
#4  r0=0x702CB0 r1=0xA lr=0x83917
#6  r0=0x702CB0 r1=0xA lr=0x83917
…  （直到 SP 漂入代码段，约 25,000 次后）
```

`r1 = 10` 是 F 对 delay 做的 clamp 下限（`cmp r1, #10; bge ...; movs r1, #10`），所以即使外层传入更小值最终也变成 10，自然无法让"delay 为 0"那条早退分支生效。

### 2.3 mr_timer 系列从未触发

整段忙等期间：

```
$ grep -c "table\[31\]" /tmp/full.log   →  0   # mr_timerStart
$ grep -c "table\[32\]" /tmp/full.log   →  0   # mr_timerStop
$ grep -c "table\[54\]" /tmp/full.log   →  0   # mr_exit
```

证明这条递归**完全发生在 mpc.mrp 二进制内部**（19KB ↔ 157KB），不经过 skyengine 提供的任何 SDK 入口。因此：

- 修 skyengine 的 timer slot、改 `mr_timerStart` 的同步性，**接不到链路上**——因为根本没去那里。
- 改 skyengine 的 `case 800/802` 加载顺序、改 `native_ext_reset`，**也接不到**——实测 mpc 只调一次 `case 800`，没有"被覆盖的 cfunction.ext"。

### 2.4 SGL / mpc 源码侧的预期路径

mpc 源码在 `/home/msojocs/github/mpc/冒泡C/src/`，路径已确认：

1. `(9,310)` 落在主对话框 toolbar 的最左 1/3 区域。
2. `smp_toolbar.c::WM_MOUSEUP` 转换为 `WM_KEYUP` + `MR_KEY_SOFTLEFT`，转发给当前 modal。
3. `smp_msgbox.c::WM_KEYUP` 在收到 SOFTLEFT 时调 `SGL_ExitModal(hDlg, ID_OK, ...)`。
4. `SGL_ExitModal` 同步派 notify 到 mainwnd 的 listener。
5. `mainwnd.c::command()` 的 `MAINWND_MSGBOXEXIT / ID_OK` 分支：
   ```c
   _FUNCSET_STYLE(FUNC_EXIT);
   mrc_exit();        // → mr_exit → table[54] → native_exit → exit(0)
   return;
   ```

设计上没有任何同步的 `mrc_timerStart` 调用，整条 SGL 链路是同步、无忙等的。所以 `bl 0x843c4` 末尾那个"再注册一次自己"的设计，在真机上**也应该**只发生一次或不发生——但具体打破点目前没法确认（需要真机或 mpc 源码侧排查）。

---

## 3. 已经撤掉 / 不再用的修复尝试

下面的"修法"都被验证过失败或者属于"启发式兜底"，**当前源码里都不存在**，仅作为后续会话避坑参考：

| 尝试 | 结果 |
|---|---|
| `case 33` (mr_getTime) 计数 200 次后强 `mr_exit()`（原 6f0ca40） | 误伤普通菜单切换，已撤 |
| `case 33` usleep(5ms) + 真实时间走过 3s 再 `mr_exit()` | 还是误伤，已撤 |
| 在 SP-in-code 崩溃分支按 `event_did_work` 区分"普通 UI / 退出自旋"决定要不要 `mr_exit()` | 字段加在 ArmExtModule 上、跨 invoke 路径要维护，被判定为兜底，已撤 |
| `case 33: sleep(1)` | 每帧睡 1 秒，应用直接不可用，已撤 |
| `case 800` 改为 in-place 挂载（方向 1） | mpc 只调一次 case 800，分支永远走"首次正常 load"，行为不变。实现已删除（包括头文件里的 `arm_ext_load_inplace`） |

---

## 4. 当前工作区差异

```
src/arm_ext_executor.c | 109 +------------------------------------------------
src/main.c             |  17 +++++---
doc/prompt.md          |  67 +++++++++++++++---------------（仅是用户改的复现描述）
```

- `src/arm_ext_executor.c`：相对 `HEAD=9bec591` 删了 109 行启发式兜底（`busy_wait_count`、`event_did_work`、`is_exit_spin` 等），新增 1 行恢复 `case 33: ret = mr_getTime(); break;` 的最朴素实现。`run_arm_with_sp` 里 PC-in-code 的 silent-swallow 分支保留（这是 commit `57d4c50` 引入的，不是我加的，去掉会引发更多 UC_ERR_INSN_INVALID 暴露）。
- `src/main.c`：加了 `VMRP_AUTO_CLICKS` 自动点击注入（用于测试）。

`test/` 下的 `lib.sh / mpc_*.sh / run_all.sh` 都是新增文件，未跟踪。

---

## 5. 下一步可走的两条不带兜底语义的路

下个会话可以直接挑一条继续：

### 路线 A：加载期二进制 patch

在 `arm_ext_load` 复制完代码后，按指令字节模式定位 `0x843C4` 函数末尾 `0x843F6: blx r1`（或更安全：定位 F 的 `bl 0x843c4` 在 `0x84374`），把那条指令改成 `bx lr` 或 `nop`，让"注册完后再触发硬件 timer 注册"那一步消失。

判定方式：
- 找定长字节签名（比如 F 入口附近的几条特征指令）确认目标二进制确实是 mpc 的 19KB 引导模块再 patch，避免误伤其它 ext。
- 一次性的"已知二进制有 bug，加载时永久修正"，不属于运行期兜底。

风险：
- 二进制不变更换版本（mpc 重新打包）签名失效；可以接受失败但要醒目报告。

### 路线 B：skyengine 拦截 0x843C4 一次性返回

在 `arm_ext_load` 加 hook 把 PC 命中 `0x843C4` 的瞬间直接接管：
- 不让它去调 veneer（避免递归），改为把 F 已经写到链表头的那条目通过 skyengine 的 native `mr_timerStart`（table[31]）真正登记一次后立即 `bx lr` 返回。
- 实质等于"在仿真器边界把 mpc 这条本想自己处理 timer 的回调换回去走 SDK"。
- 仍然是确定性拦截（hook 仅作用在已知地址），不是"出错再清退"。

要做的事：
1. `arm_ext_load` 里加 `UC_HOOK_CODE` 在 `0x843C4`，读 F 写入的链表头节点（offset 已知：`field_4 = delay, field_8 = remaining, field_12 = cb arg, field_16 = cb, field_20 = cb_data`）；
2. 把这些字段映射回 skyengine 的 `mr_timerStart(t)`（直接调 native `mr_timerStart`，host 端拿 SDL_AddTimer 调度）；
3. 把 PC 设到 `pop {r3,r4,r5,pc}` 那条尾巴（0x84406）直接返回，跳过 `bl 0x662B5C/0x662BA8` 这两条会触发递归的 `blx`。

风险：
- F 末尾还有插入排序链表那段（`b 0x84378` 及后）会被跳过，但 19KB 的 timer 链表只用于"找到最近的 deadline 再注册到硬件"——只要我们直接把它转给 SDL_AddTimer，那张链表就没用了，跳过反而干净。
- 需要先确认 `mr_c_function_P.field_92.field_8.field_0` 这个 state 字段在 native skyengine 下的值。F 早退依赖它 == 3 或 4，目前看到的递归没触发早退，可能就是因为这个字段在 skyengine 下永远是 0。

---

## 6. 用过的诊断 hook / 调用样例（一次性，不在源码里了，但用法记下）

- 一次性栈 dump + 把 ext 代码段写到 /tmp 文件：`VMRP_BUSYWAIT_STACK_DUMP=1`，触发条件 `lr ∈ [0x84300, 0x84400]`。
- 每 200 次 `lr=0x84324` 采样栈帧上代码段返回地址：`VMRP_BUSYWAIT_SAMPLE=1`。
- 对几个可疑入口加 instruction hook 打印 r0 / r1 / lr：`VMRP_RECURSE_SAMPLE=1`，覆盖 `0x838B4 / 0x662B5C / 0x662BA8`。
- F 入口前 20 次 r0/r1/lr/sp：`VMRP_F_PROBE=1`。

代码段反汇编：

```
# 19KB primary（Thumb，已知是 mpc 的引导模块）
arm-none-eabi-objdump -D -b binary -m armv7 -EL --adjust-vma=0x80000 \
    -M force-thumb /tmp/skyengine_ext_code.bin | less

# 157KB nested（ARM）
arm-none-eabi-objdump -D -b binary -m arm -EL --adjust-vma=0x646380 \
    /tmp/skyengine_nested_code.bin | less
```

关键 VMA / 偏移速查表：

| VMA | 偏移 in dump | 说明 |
|---|---|---|
| `0x84298` | `0x4298`（skyengine_ext_code.bin） | F 入口 |
| `0x84324` | `0x4324` | F 内部某 mr_getTime 调用点的返回地址（lr=0x84325） |
| `0x84374` | `0x4374` | F 末尾 `bl 0x843c4`，**关键的递归触发点** |
| `0x843C4` | `0x43C4` | 取时间 + 双 veneer 调用 |
| `0x843EC` | — | `blx r0` 到 veneer 0x662BA8（lr=0x843EF） |
| `0x843F6` | — | `blx r1` 到 veneer 0x662B5C（lr=0x843F9） |
| `0x838B4` | `0x38B4` | 跨 ext r9 切换 + 派发 |
| `0x662B5C` | `0x1C7DC`（skyengine_nested_code.bin） | nested ext veneer，blx ip = 0x838B4（r1=0 路径） |
| `0x662BA8` | `0x1C828` | nested ext veneer，blx ip = 0x838B4（r1=1 路径） |
| `0x6651D0` | `0x1EE50` | mpc 自己的 timer 注册函数的 mr_getTime 调用点（第一层 F 调用前的合法 mr_getTime） |

---

## 7. mpc 源码侧的关键位置（已确认）

```
/home/msojocs/github/mpc/冒泡C/src/mainwnd.c
  MenuEvent: case STR_EXIT → SMP_MsgBox(MAINWND_MSGBOXEXIT, ...)
  command:   case MAINWND_MSGBOXEXIT, ID_OK → mrc_exit()

/home/msojocs/github/mpc/冒泡C/sfw/controls/smp_msgbox.c
  WM_KEYUP: MR_KEY_SOFTLEFT/SELECT → SGL_ExitModal(hDlg, ID_OK, ...)
  WM_CREATE: 创建 toolbar 子窗口（在对话框底部 26px）

/home/msojocs/github/mpc/冒泡C/sfw/controls/smp_toolbar.c
  WM_MOUSEUP: 转鼠标坐标为 MR_KEY_SOFTLEFT/SELECT/SOFTRIGHT 再 SGL_DispatchEvents

/home/msojocs/github/mpc/冒泡C/sfw/gui/window.c
  SGL_DispatchEvents: 模态对话框路由
  SGL_ExitModal:      SuspendDrawing + SendNotifyMessage + UpdateWindow
```

所有这些都是同步、无延时的实现，按源码看不会引入 mr_getTime 自旋。源码 vs 二进制的差异提示我们 19KB 引导模块的 `0x84298 / 0x843C4 / 0x838B4` 这段不是来自 mainwnd.c / smp_*.c，是 mpc 内部的 timer / event 框架代码（很可能对应 sfw/gui 下某个 .c，但还没有按 1 对 1 对上）。

下个会话如果走路线 B，可以先在 mpc 源码里找"用 mrc_timerStart 注册定时器后再次发送注册请求"那种模式，对照确认 `0x843C4` 大概是哪个源文件，可能能让"跳过 blx 0x662B5C 那一步"更有把握。

---

## 8. 给下个会话的简短任务清单

1. 选定路线 A 或 B（见 §5）。
2. 在 `src/arm_ext_executor.c` 加实现，**严禁**重新引入 `busy_wait_count`、`event_did_work`、SP-in-code → `mr_exit` 这种运行期"出错才清理"型代码。
3. 跑 `test/run_all.sh`，要求 `mpc_exit_menu_terminates_app.sh` 转绿、`mpc_menu_click_keeps_app_running.sh` 继续绿。
4. 用 trace 复核：希望看到
   ```
   table[54]   被调用 ≥ 1 次
   table[31]   被调用 ≥ 1 次（如果走路线 B 接到了 SDL timer）
   mr_exit() called by mythroad! / mythroad exit.
   ```
5. 改动落 commit，commit message 写明走的是 A 还是 B、为什么不是兜底（重点强调"加载期一次性 patch"或"对已知地址的确定性 hook"，与"运行期检测异常再补救"区分开）。

---

## 9. 2026-05-24 续：递归终止条件实际上是标准 SDK 行为

### 9.1 试过的路线 A/B（签名+hook 拆递归）被驳回

第一次尝试用过两套方案：
1. 加载期靠多签名命中后把 `0x83912` 的 `bl F` patch 成 `nop.w`；
2. 加载期挂 `UC_HOOK_CODE @ 0x84298` 跟踪 F 是否已经在栈上，嵌套再入就把
   `PC=LR; r0=0` 立刻返回。

两种实现确实能把同尾递归切干净（`table[33]` 调用从 25,913 次降到 1 次），正常
菜单测试继续绿；但 `mpc_exit_menu_terminates_app.sh` 还是红——`table[54]` 一次
都不响。

用户驳回，原话："不能使用死代码进行处理，哪有操作系统给应用做适配的？原环境
是 cfunction.ext，分析汇编逻辑看能不能参考"——意思是：skyengine 是 OS 角色，不应该
针对某一个 .mrp 二进制装签名 patch 或者地址 hook，应该让标准 SDK 行为自然驱动
mpc 的终止逻辑。

**已把两套方案从 `src/arm_ext_executor.c` 彻底撤掉**，工作区里现在只剩 §3 那
轮兜底清理。

### 9.2 重新读 §2.2 那条链路上的反汇编：F 早退依赖的是标准 mr_state

第一次会话把 F 的早退条件写作 `mr_c_function_P.field_92.field_8.field_0 ∈ {3,4}`，
听起来像私有字段。重新分别用 Python 跟踪一遍 RWPI 偏移之后，结论是：

- `0x842A2..0x842A6`：`r0 = pc + literal = 0x7FFC8`；`r2 = [r0,#56] = [0x80000]`，
  而 `0x80000` 是 EXT primary 加载时被覆盖的"`mr_table` base"（见 `arm_ext_load`
  里的 `memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4)`），所以 r2 = `0x10000`。
- `0x842BC..0x842C0`：`r6 = [table+0x5C] = table[23]`，table[23] 在 `init_table()`
  里写死为 `m->internal_table_addr`；再 `r6 = [internal_table+#8] = internal[2]`，
  在 `init_table()` 里 `internal[2] = m->mr_state_slot`；最后 `r6 = *mr_state_slot`。
- `0x842C2..0x842C8`：`cmp r6,#3` / `cmp r6,#4` 命中就跳到 0x842CA 早退。

也就是说：**F 早退的是 `mr_state == MR_STATE_RESTART(3)` 或 `MR_STATE_STOP(4)`**，
这是 `src/mythroad/include/mr.h` 里的标准 SDK 状态枚举。同理，0x843DC..0x843E4 把
`internal[5] = m->mr_timer_state_slot` 清成 0，也是标准 SDK 语义（`mr_timer_state
= IDLE`）。第一次会话把这两个槽误以为是 mpc 私有字段。

### 9.3 反过来看 nested ext (`game.ext`, 0x646380~) 的 init：mpc 自己会改 mr_state

`game.ext` 在挂载期就**直接覆盖 `mr_table[31]/[32]`** 把它们指向自己的 veneer，
这一点已经用 `UC_HOOK_MEM_WRITE` 在 0x6463CC/0x6463D8 处确认：

```
[PROBE] write table[31] = 0x662B5C (size=4) pc=0x6463CC lr=0x6463AC
[PROBE] write table[32] = 0x662BA8 (size=4) pc=0x6463D8 lr=0x6463AC
```

所以 §2.2 那条链里 0x843F6 的 `blx r1`、0x843EC 的 `blx r0` 跳到的根本不是
table 入口，是 `game.ext` 自己装的两个 veneer——这是 mpc/game.ext 的内部 timer
派发机制，**不走 SDK 的 mr_timerStart/mr_timerStop**。

更关键的是：`game.ext` 在 0x664678 这个函数（caller 在 0x64D360）里有一段：

```
664738: ldr r1, [r6, -8]          ; r1 = mr_table base
66473c: mov r0, #3                ; r0 = MR_STATE_RESTART
664740: ldr r1, [r1, #92]          ; r1 = internal_table_addr
664744: ldr r1, [r1, #8]           ; r1 = internal[2] = mr_state_slot
664748: str r0, [r1]               ; *mr_state_slot = 3
```

**即 mpc 自己拥有一段会把 `mr_state` 写成 `MR_STATE_RESTART(3)` 的代码**——它
就是为了让 §2.2 那条同尾递归在所属 timer dispatch 结束后自然停下来。

### 9.4 现状：那段 0x664678 在 skyengine 退出 click 里没被调到

加 `UC_HOOK_MEM_WRITE` 监视 `m->mr_state_slot` 之后跑 `23,305;30,280;9,310`，
12 秒内：

- `mr_timer_state` 被写 0 **12,957 次**（全部 pc=0x843E4 lr=0x843D5，正是同尾
  递归在 0x843C4 里反复把 timer-state 清 IDLE 的副作用）。
- `mr_state` 被写 **0 次**——0x664748 那条指令一次都没跑到。

也就是 mpc 自带的 RESTART-写入路径在 skyengine 这边整个 click 流程里都没被走通。
原因只能是上面那条 `click → softkey → modal → SGL_ExitModal → mainwnd
listener → mrc_exit/mrc_restart …` 链上某一段，在我们的环境里行为跟真机不一
致——递归卡死之前根本没机会调到 0x664678，所以 mr_state 永远卡在
`MR_STATE_RUN(1)`，F 就永远满足不了早退条件。

### 9.5 给下个会话的新方向（已经过用户复核，不准再走 §5 那两条死代码路线）

1. **重点排查 click 派发链路**：不要再去 patch mpc 二进制。要找的是为什么在
   skyengine 下 click → softkey 之后到达不了 `0x664678` 这条 RESTART 通路。可挂的
   探针：
   - 给 0x664678 加 `UC_HOOK_CODE`，看 mpc 启动期/正常菜单期它是否被调过。
     如果"启动期被调过、退出时没被调"，问题就在 mpc 自己 dispatch；如果"从
     来没被调过"，那是 mpc/game.ext 内部状态没初始化好。
   - 给 0x664678 caller 0x64D360 也加 hook，沿这条链反向找派发源头。
2. **顺便也对照 native cfunction.ext (332KB)**：尽管 mpc.mrp 启动时会把它
   `case 800` 覆盖掉（doc §2.1 已确认），但它仍然是 mpc 19KB 引导模块所对应的
   "原 SDK 二进制"——`src/mythroad/include/mr.h` 里 enum 顺序与 `arm_ext_executor.c`
   里 `init_table()` 的 internal_table 顺序，都要跟 332KB 那份保持一致；下一轮
   修改要先确认这些表布局没漂。
3. **不要再做的事**：
   - 不要按签名识别 mpc 然后挂 F 入口 hook（已被用户驳回）；
   - 不要在 case 33 / silent-swallow 里手动 `mr_exit()`（§3 已被撤）；
   - 不要把 mr_state 写成 STOP 来"骗" F 早退——这是 OS 越权改 app 状态；
   - 不要把 SDL_AddTimer 同步触发 `mr_timer()`，会跟主线程的 `uc_emu_start`
     竞争同一个 unicorn 引擎（实测会卡）。
4. **测试基线**：当前两条 mpc 测试的预期跟 §1.3 一致——
   `mpc_menu_click_keeps_app_running.sh` 必须 PASS；`mpc_exit_menu_terminates_app.sh`
   保持 FAIL 作为档案，**修复后**才允许它转绿，转绿条件是看到 `table[54]` 被调用
   并且日志有 `mr_exit() called by mythroad!` / `mythroad exit.`。
