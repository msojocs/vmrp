# 模块生命周期、事件路由与定时器

## 1. 模块角色

一个 `ArmExtModule` Unicorn 实例内可以同时存在多层 ARM 模块。

| 角色 | 典型内容 | 状态字段 |
| --- | --- | --- |
| wrapper | 最外层 `cfunction.ext` | `p_addr`、`helper_addr` |
| primary | 主游戏或主业务 EXT | `primary_p_addr`、`primary_helper_addr` |
| active | 当前前台模块 | `active_p_addr`、`active_helper_addr` |
| timer owner | 最近启动宿主 timer 的模块 | `timer_p_addr`、`timer_helper_addr` |
| current call | 当前正在执行的 helper | `current_p_addr`、`current_helper_addr` |
| screen owner | 当前可见提交归属 | `foreground_screen_owner_*` 等字段 |

这些角色可以暂时指向同一个模块，但语义不同。例如 primary game 仍存在时，active 可能是下载插件，timer owner 可能仍是 wrapper。

## 2. nested module 记录

`ArmExtNestedModule` 保存：

- 文件映像地址和长度；
- P 和 helper；
- 提供该 child 的 host MRP 路径；
- RAM-backed MRP 地址和长度；
- ARM 可见短包名槽；
- compact timer scheduler 偏移和 walker 地址；
- 模块自身观测到的 GOT memcpy bridge 偏移。

模块记录上限为 `ARM_EXT_NESTED_MODULE_MAX`。注册新模块前必须处理地址复用和过期模块：同一 staging 区可能先放压缩数据，后放 child code，再被下一次下载复用。

## 3. 两条注册路径

### 3.1 table[25] 路径

标准 nested EXT 调用 `_mr_c_function_new`：

1. guest 把 helper 和 P 长度传给 table[25]；
2. 执行器分配或复用 P；
3. 根据 LR 判断调用来自 wrapper 还是 nested code；
4. 写入 `file_base + 4` 的 P 地址；
5. 记录 active 和 nested module；
6. 首个 child 成为 primary；
7. 设置 `primary_host_init_pending`，让 Mythroad 在 wrapper 返回后补一次 code=0。

table[25] 只完成注册，不等价于 child 已经完成生命周期初始化。

### 3.2 private-loader 路径

部分 wrapper 不调用 host table[25]，而是在 guest 内部自行构造：

- `file_base[0] = module record`；
- `file_base[4] = P`；
- extChunk 的 file/P/RW/helper 字段；
- child 私有 table 镜像；
- 随后通过 cache sync 或 `_mr_TestCom1` 通知宿主。

执行器通过 `arm_ext_has_internal_loader_chunk()`、`arm_ext_sync_internal_nested_module()` 和结构一致性验证确认这种模块。确认后还需要修复 private record 中指向错误 table/RW 的 bridge。

不能只因为某段内存以 ARM 指令开头就注册为模块；必须同时验证 file header、P、extChunk、helper 和映射边界。

## 4. 状态迁移

```text
wrapper loaded
    |
    +-- register first child ----------------> primary = active = child
    |                                             |
    |                                             +-- start timer
    |                                             |      timer owner = primary
    |                                             |
    |                                             +-- open modal/plugin child
    |                                                    active = child2
    |                                                    primary suspended
    |                                                    timer owner may be
    |                                                    wrapper/primary/child2
    |                                                          |
    |                                             child2 closes|
    |                                                          v
    |                                             active = primary
    |                                             restore screen/timer/routes
    |
    +-- no primary yet -----------------------> wrapper keeps host tick alive
```

退役 child 时需要同时处理：

- nested module 数组；
- active/timer/screen owner；
- package context 和短包名；
- modal screenshot 和 wrapper 前台分发区；
- compact heap 保护；
- Unicorn code cache。

只清 `active_p_addr` 不足以完成关闭。

## 5. helper code 路由

当前 `arm_ext_call()` 使用以下原则：

| code | 路由 |
| --- | --- |
| `0` | 生命周期初始化，优先 primary |
| `1` | 原始事件；存在独立 wrapper 时必须先进入 wrapper |
| `2` | timer；优先明确记录的 timer owner，必要时结合前台 child 私有队列 |
| `3..5` | 生命周期/控制消息，优先 primary |
| 其它 | active owner，缺失时回退 wrapper |

事件必须先经过 wrapper，因为 wrapper 维护 suspend/resume、模态 child 和内部转发链。直接把 code=1 发给 game 可能让画面短期正常，但 wrapper 不知道 child 已打开或关闭，后续 timer 和返回键会失配。

## 6. R9 与 owner

每次调用前，执行器从 P 的 `start_of_ER_RW` 设置 R9。guest 在内部跨模块跳转时，`hook_restore_r9()` 又会根据 PC 所属 code 区修正 R9。

owner 判断的主要证据顺序：

1. 当前 `current_p/helper`；
2. LR/PC 落在哪个注册模块映像；
3. pending internal loader staging；
4. active/primary/wrapper 回退；
5. private module record 的 P/helper/RW。

资源读取、屏幕提交和 timer owner 都复用这一所有权判断。不要为三个子系统各自实现一套不一致的“当前模块”。

## 7. 包上下文

table[100] 是 ARM 可见当前包名。很多 EXT 把它复制到固定 32 字节缓冲，所以执行器使用尽可能短且可重新打开的 alias，并在 host 侧保留完整路径。

child 资源可能来自：

- 外层 MRP 的包内文件；
- 另一个已经下载到文件系统的 MRP；
- table[104]/[105] 指向的 RAM-backed MRP；
- private child module record 中独立的包名槽。

`table[125]` 读取资源时根据 LR 找到调用 child，再选择该 child 的 table[100]/[104]/[105]，而不是始终使用 VM 全局包名。

如果 child 清空了自己的 table[100]，已记录的 `package_host_path` 或 RAM package 仍是它的资源上下文。否则插件会错误读取外层 game 包。

## 8. 宿主 timer 与 guest timer

宿主只维护“何时再次进入 VM”的 timer。真正的回调节点、delta queue 和 owner 状态在 guest wrapper/game/child 内存中。

table[31] `mr_timerStart`：

- 启动宿主 timer；
- 设置 guest/internal timer state；
- 标记 `host_timer_pending`；
- 记录当前调用者为 timer owner。

table[32] `mr_timerStop`：

- 停止宿主 timer；
- 清 timer state；
- 清 timer owner。

timer 回调必须回到执行器所属主线程。SDL timer 线程只投递事件。

## 9. 已识别的 timer ABI

执行器目前支持多种结构，不能把它们混成一个固定偏移。

| 类型 | 位置/识别 | 说明 |
| --- | --- | --- |
| primary game head | RW `+0x8C` 或 `+0x88` | 运行时自动探测的 SDK 变体 |
| frame scheduler | RW `+0x94` 起 | `+0x9C` queued，`+0xA0` active 的已观测布局 |
| compact scheduler A | RW `+0xC0` 起 | child walker 从 R9 加固定偏移得到 |
| compact scheduler B | RW `+0x248` 起 | private child 常见变体 |
| wrapper compact | 二进制探测得到的偏移 | queue/current/last tick 相对 scheduler |
| wrapper legacy | RW `+0x3C8` / `+0x3D8` | 旧式 wrapper queue/current |

compact 节点常使用 `ARM_EXT_COMPACT_TIMER_MAGIC = 0x79ABBCCF`，但 magic 在不同布局中的位置不同。必须同时验证：

- 节点地址可映射；
- interval/callback/next 字段合理；
- callback 属于已知模块 code；
- 链表不会循环或越界；
- scheduler 偏移由对应模块代码探测得出。

## 10. `arm_ext_call_dispatch()`

普通 timer 可以走 `arm_ext_call(code=2)`。只有 wrapper extChunk 提供有效 `event_func/event_data`，且当前 timer 链由 wrapper 私有 dispatch 驱动时，才直接调用 `arm_ext_call_dispatch()`。

该函数需要：

1. 选择 wrapper/active/primary 的 dispatch 目标；
2. 设置正确 R9；
3. 传入 stop 标志和 timer interval；
4. 处理 chain walker 和 supplementary init；
5. 同步 timer state；
6. 根据队列活性决定是否重新启动宿主 tick；
7. 处理 child 关闭和 primary 恢复。

发现某个 child 有 timer 节点，不代表它一定拥有宿主 timer。owner 必须结合最近的 table[31] 调用和 wrapper 调度状态判断。

## 11. suspend/resume 与模态状态

primary extChunk `+0x34` 是 suspend 深度。进入浏览器、下载、支付或其它模态 child 时，wrapper 可能：

- 增加 suspend 深度；
- 从 game timer 链摘除节点；
- 改写前台 helper 分发表；
- 把 active 切到 child；
- 在同一 backing screen 上绘制覆盖层。

执行器在进入可能触发模态流程的事件/timer 前保存：

- guest 屏幕快照；
- wrapper RW 前台分发区快照；
- game timer head；
- primary/active/timer/screen owner。

child 关闭且结构证据确认恢复时，再还原这些状态。不能在所有返回键后无条件恢复截图，否则非模态页面更新会被旧画面覆盖。

## 12. 重置与卸载

`arm_ext_reset_child_modules()` 保留 wrapper 和 primary 的模块状态，清理其余 child，把 active 回到 primary，并清除前台 screen owner。它当前不自行重选 timer owner，也不恢复 guest timer 链或模态快照；完整关闭路径会在调用它前后完成这些状态的收敛。该接口适用于已经确认插件完成关闭后的 game-only 状态，不能单独充当通用 resume 操作。

`arm_ext_unload()` 则销毁整个实例，包括：

- Unicorn engine；
- 主 backing memory 和平台映射；
- MRP cache、VFD、短包名和 nested modules；
- bump live/free 表；
- screen damage/cover 和模态快照；
- app compatibility state；
- 诊断和临时文件副本。

新增 `ArmExtModule` 动态字段时，必须同步检查 unload 和失败回滚路径。
