# gxdzc 资源包下载界面 — 取消/确认按钮修复文档

## 问题描述

执行 `test/gxdzc/download-resource.sh` 进入资源包下载界面后：
- 点击"取消"按钮无反应（预期：返回游戏 UI 页面）
- 点击"确认"按钮无反应（预期：启动资源包下载流程）

## 根因分析

### 1. 事件路由问题（取消/确认共同原因）

`arm_ext_call` 对 `code=1`（事件）始终路由到 game 的 primary helper（行 1853-1854）。
真机上事件先经过 **wrapper helper**——wrapper 在模态框（`extChunk[0x34]>0`）期间
拦截软键事件（SOFTLEFT=确认, SOFTRIGHT=取消）并执行对应回调。

VMRP 跳过了 wrapper，事件直接到 game helper。反汇编 game helper 入口 `0x6636C8`
的事件回调 `0x66326C`：不检查键码，始终返回 0（MR_SUCCESS），所有事件被静默吞掉。

### 2. wrapper 模态框机制（反汇编）

**extChunk 结构**（地址 `0x7053CC`，通过 `[primary_p_addr+12]` 获取）：

| 偏移 | 值 | 含义 |
|------|-----|------|
| [0x00] | 0x7FD854EB | magic 校验值 |
| [0x08] | 0x00000000 | game handler 函数指针（**被 wrapper 清零**） |
| [0x1C] | 0x00705428 | context/P addr（= primary_p_addr） |
| [0x24] | 0x00705408 | wrapper timer node |
| [0x28] | 0x00E830BD | wrapper dispatch 函数（Thumb） |
| [0x34] | 0/1 | suspend 嵌套计数器 |

**suspend 函数** `0xE831A4`（Thumb）：
```
[0x34]++
若变为 1：移除 timer node [0x24]（bl 0xE838A8）
blx [0x08] 派发 pauseApp(code=4) → 但 [0x08]=0 → hook_low_zero 无害返回
```

**resume 函数** `0xE83220`（Thumb）：
```
[0x34]--
若回到 0：恢复 timer node（bl 0xE83908）
blx [0x08] 派发 resumeApp(code=5) → 同上，[0x08]=0 → 无害返回
```

**关键发现**：`extChunk[8]=0`（game handler 指针被 wrapper ARM 代码主动清零），
导致 suspend/resume 的 `blx extChunk[8]` 跳转到地址 0。`hook_low_zero` 捕获后
安全返回（设 R0=0, PC=LR），不会崩溃但 game 的 pauseApp/resumeApp 从未被调用。

### 3. game timer 链表被清空

**dispatch 函数**（`arm_ext_call_dispatch`）在每个 timer tick 调用 wrapper dispatch（`extChunk[0x28]`）。wrapper dispatch 在模态框进入时**清空了 game 的 timer 链表头**（`state[8]` at `R9+0x8C`），导致 game timer tick 函数 `0x6643E8` 在 `cmp r0, #0; beq return` 处提前返回，不再绘制。

**state[8] 保存机制**：在 `arm_ext_call_dispatch` 的 `run_arm_with_sp` 前后对比 `state[8]`，检测到被清零时保存旧值到 `m->saved_game_timer_head`。同理在 `arm_ext_call` 的 helper 调用前后也做保存。

### 4. wrapper helper 是 ARM 模式

wrapper helper 入口 `0xE800B0` 是 **ARM 模式**（非 Thumb）：
```arm
push {r2-r11, lr}
mov r7, r0       ; P addr
mov r8, r1       ; code
mov sl, r2       ; input_addr
mov fp, r3       ; input_len
...
blx 0xE82A44     ; 切换到 Thumb 模式调核心函数
...
pop {r2-r11, pc}
```

核心函数 `0xE82A44`（Thumb）有 jump table 处理不同 code：
- code=0 → `0xE82A96`：读取事件结构体（event.code, param0, param1）
- code=1 → `0xE82A6E`：保存 extChunk → `bl 0xE8352C` → `bl 0xE81E38`（事件分发核心）
- code=4 → `0xE82AD4`：pauseApp
- code=5 → `0xE82ACA`：resumeApp

### 5. game pause/resume 计数器（反汇编）

game 有独立的 pause/resume 计数器（偏移 `R9+0x1F14`）：
- **pause** `0x646C20`：counter--，若 ≤0 则停止 timer（清空 state[8]）
- **resume** `0x646B84`：counter++，若 ≤1 则恢复 timer（重建 state[8]）
- 恢复逻辑依赖 `R9+0x1F10`（timer entry count），若为 0 则跳过

由于 `extChunk[8]=0`，wrapper 的 suspend/resume 从未调用 game handler，
game 的 pause counter 始终为初始值 1（未被修改）。

### 6. 确认键下载流程

确认键点击后 wrapper 内部触发的网络操作序列：
```
table[82] mr_closeNetwork      ← 关闭旧连接
table[81] mr_initNetwork(cb=0x6728ED, mode=0x67357C) ← 初始化网络
table[84] mr_socket(type=0, protocol=0)              ← 创建 TCP socket
table[85] mr_connect(s=1, ip=10.0.0.172, port=80)    ← 连接下载服务器
```

`table[84]`/`table[85]` 原先未实现（走 default "not implemented" 分支）。

## 修复方案

### 修改文件：`src/arm_ext_executor.c`

#### A. 模态框事件路由与取消/确认处理

在 `arm_ext_call` 函数中，`code=1` 事件的 helper 调用前：

1. 读取 `extChunk[0x34]`（suspend depth）
2. 若 `_sd > 0`（模态框活动）：
   - 将事件路由到 wrapper helper，而不是直接交给 game helper。
   - 普通触屏坐标继续保持为 `MR_MOUSE_*`，不再按底部 20px 自动转换为软键。否则会出现应用未显示左右软键按钮时，左右下角隐形触发软键的错误行为。
   - **取消（键盘/平台 SOFTRIGHT）**：
     - 手动调用 wrapper resume 函数（Thumb `0xE83221`）
     - 恢复 `saved_game_timer_head` 到 `state[8]`
     - game timer 重新运行，绘制前一页
   - **确认（键盘/平台 SOFTLEFT）**：
     - 将 `MR_KEY_PRESS(SOFTLEFT)` 写入事件缓冲
     - 路由到 wrapper Thumb 核心函数（`0xE82A45`）
     - **不恢复 state[8]**（避免 game timer 抢绘覆盖 wrapper UI）
     - **不修复 extChunk[8]**（避免 wrapper resume 内嵌套调用 game handler 导致 Unicorn 状态损坏）
     - wrapper 内部处理确认回调，触发网络下载

#### B. state[8] 保存机制

在 `arm_ext_call` 和 `arm_ext_call_dispatch` 的 `run_arm_with_sp` 前后
对比 game timer 链表头（`[game_rw + 0x8C]`），检测到被清零时保存到
`m->saved_game_timer_head`。

新增 `ArmExtModule` 字段：`uint32_t saved_game_timer_head`。

#### C. 网络函数实现（table[83-90]）

在 `hook_table` switch-case 中添加网络函数桥接：

| case | 函数 | 实现 |
|------|------|------|
| 83 | mr_getHostByName | 返回 MR_FAILED（回调机制不适用于 Unicorn） |
| 84 | mr_socket | `mr_socket(r0, r1)` |
| 85 | mr_connect | `mr_connect(r0, r1, (uint16)r2, r3)` |
| 86 | mr_closeSocket | `mr_closeSocket(r0)` |
| 87 | mr_recv | `mr_recv(r0, arm_ptr(m,r1), r2)` |
| 88 | mr_recvfrom | `mr_recvfrom(r0, arm_ptr(m,r1), r2, arm_ptr(m,r3), arm_ptr(m,a4))` |
| 89 | mr_send | `mr_send(r0, arm_ptr(m,r1), r2)` |
| 90 | mr_sendto | `mr_sendto(r0, arm_ptr(m,r1), r2, r3, (uint16)a4)` |

`arm_ptr(m, addr)` 将 ARM 地址转换为宿主指针（直接指向 Unicorn 映射内存）。
`arg_read(m, 4)` 读取第 5 个参数（从栈上）。

## 验证

```bash
# 取消键：返回下载列表页
test/gxdzc/download-resource-back.sh
# PPM 验证：y=0~280 有大量非黑像素（>200 采样点）

# 确认键：触发下载流程
test/gxdzc/download-resource-confirm.sh  
# 日志验证：出现 mr_socket + mr_connect 调用
# PPM 验证：停留在提示页（102 采样点，未跳到主菜单）
# 连接 10.0.0.172:80 不可达是预期的（需要真实下载服务器）

# 回归：gghjt 不受影响
test/gghjt/pay-normal-back.sh
# 验证：1 次 After app init，正常运行至 timeout
```

## 关键反汇编地址表

| 地址 | 模式 | 函数 |
|------|------|------|
| 0x6636C8 | Thumb | game helper 入口（jump table） |
| 0x66326C | Thumb | game event callback（存储事件，始终返回 0） |
| 0x6643E8 | Thumb | game timer tick（检查 state[8]，为 0 时跳过绘制） |
| 0x646B84 | Thumb | game resume counter（counter++，≤1 时恢复 timer） |
| 0x646C20 | Thumb | game pause counter（counter--，≤0 时停止 timer） |
| 0x662250 | Thumb | game resumeApp handler |
| 0x662270 | Thumb | game pauseApp handler |
| 0xE800B0 | **ARM** | wrapper helper 入口（ARM→Thumb 跳板） |
| 0xE82A44 | Thumb | wrapper 核心函数（code jump table） |
| 0xE81E38 | Thumb | wrapper 事件分发核心 |
| 0xE831A4 | Thumb | wrapper suspend（[0x34]++, 移除 timer, blx [0x08]） |
| 0xE83220 | Thumb | wrapper resume（[0x34]--, 恢复 timer, blx [0x08]） |
| 0xE830BD | Thumb | wrapper dispatch 函数（extChunk[0x28]） |
| 0xE83908 | Thumb | wrapper 恢复 timer node 到活动链表 |
| 0xE838A8 | Thumb | wrapper 从活动链表移除 timer node |

## 遗留问题

1. **extChunk[8] = 0**：wrapper ARM 代码主动清零 game handler 指针。
   初始化时写入后会被覆盖。当前通过 `hook_low_zero`（blx 0 → 安全返回）
   规避。确认路径不修复此字段以避免嵌套调用导致 Unicorn 状态损坏。

2. **mr_getHostByName**（table[83]）：ARM ext 的回调机制（通过
   `network_cb` + `isExtCB` + `setR9`）不适用于 Unicorn 环境。
   gxdzc 跳过 DNS 直接使用 IP 连接，暂返回 MR_FAILED。

3. **下载服务器不可达**：connect 目标 10.0.0.172:80 是 gxdzc 内置的
   资源下载服务器地址，实际不可达。需要搭建 mock server 或配置代理才能
   完成完整下载流程。
