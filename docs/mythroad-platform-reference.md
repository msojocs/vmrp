# Mythroad 平台源码对模拟器通用化的参考分析

## 结论

`temp/SKY平台-MTK11B-COSMOS风格/code/plutommi/mmi/Mythroad` 可以作为 VMRP 模拟器通用化开发的重要参考依据，但它更适合作为“平台行为契约”的证据，而不是直接移植实现。

仓库此前存在的 `patch_gxdzc_game_touch_map()` 这类针对 `gxdzc.mrp` 包名、固定偏移和固定状态字段的修复，确实偏离模拟器应有的通用边界。Mythroad 平台源码显示，真机侧的职责主要是：

- 把宿主平台按键、触笔、对话框、菜单等输入统一转换成 `MR_KEY_*`、`MR_MOUSE_*`、`MR_DIALOG_EVENT` 等 Mythroad 事件。
- 维护按键/触摸事件成对关系，过滤重复 press/down/up。
- 由平台 UI 或本地 UI 层决定 OK/CANCEL、菜单返回、编辑框确认等事件语义。
- 由定时器、pause/resume、screen group 和本地 UI 生命周期共同驱动应用状态。

因此，模拟器通用化方向应优先补齐“平台层事件路由、生命周期、定时器、屏幕和本地 UI 语义”，而不是修改具体 MRP 包内 `game.ext` 的代码或状态。

## 当前特判问题

### `arm_ext_executor.c` 中的样本绑定历史

执行器里曾存在明显的样本专用逻辑：

- `patch_gxdzc_game_touch_map()` 先检查当前包是否为 `gxdzc.mrp`，再按固定 offset 修改 `game.ext` 指令；该补丁已被移除。后续曾尝试按底部坐标在 runtime/SDL/modal 层转换为软键事件，但平台源码不支持无条件转换，这类转换也应撤掉。
- `src/arm_ext_executor.c:1967` 的 `arm_ext_is_gxdzc_pack()` 通过文件名判断当前包。
- `src/arm_ext_executor.c:2020`、`src/arm_ext_executor.c:2041`、`src/arm_ext_executor.c:2055` 只对 gxdzc 保存模态截图、检测下载 pending 状态、清理取消后的状态尾巴。
- `src/arm_ext_executor.c:2244`、`src/arm_ext_executor.c:2258`、`src/arm_ext_executor.c:2294` 在模态取消路径继续以 `arm_ext_is_gxdzc_pack()` 作为行为开关。

这些逻辑的共性是：它们把“平台应如何分发输入和恢复 UI/定时器状态”的问题，降级成“某个包的某个状态字段该怎么改”。短期能修样本，长期会让新样本兼容性依赖历史 workaround。

### `dsm.c` 中已移除的包名特判

`src/mythroad/dsm.c` 曾存在 `netpay_blocked_for_current_pack()`：它将 `gxdzc` 对 `netpay.mrp` 的访问直接伪装成文件不存在。这个特判已移除，DSM 文件层现在按实际文件系统结果处理 `netpay.mrp` 是否存在。这个问题和触摸映射不同，背后是支付/联网插件不可用，后续仍应收敛成可配置、可解释的插件/网络模拟策略，而不是硬编码某个包名。

## Mythroad 平台源码提供的直接证据

### 1. 事件码和键码是稳定 ABI

`temp/.../MythroadInc/mrporting.h:16` 到 `temp/.../MythroadInc/mrporting.h:90` 定义了 `MR_KEY_0` 到 `MR_KEY_NONE`；`MR_KEY_SOFTLEFT=17`、`MR_KEY_SOFTRIGHT=18`、`MR_KEY_SELECT=20` 的顺序与当前仓库 `src/include/types.h:45` 到 `src/include/types.h:73` 一致。

`temp/.../MythroadInc/mrporting.h:132` 到 `temp/.../MythroadInc/mrporting.h:154` 定义 `MR_KEY_PRESS=0`、`MR_KEY_RELEASE=1`、`MR_MOUSE_DOWN=2`、`MR_MOUSE_UP=3`、`MR_DIALOG_EVENT=6`、`MR_MOUSE_MOVE=12`，也与 `src/include/types.h:76` 到 `src/include/types.h:97` 对齐。

结论：模拟器应把这些事件码当作公开 ABI，不应通过修改包内代码来绕过 ABI。

### 2. 真机平台会统一转换按键事件

`temp/.../MythroadSrc/mrp_application.c:637` 到 `temp/.../MythroadSrc/mrp_application.c:668` 注册 DSM 屏幕的 key/pen handler。

`temp/.../MythroadSrc/mrp_application.c:996` 到 `temp/.../MythroadSrc/mrp_application.c:1030` 从平台 `GetkeyInfo()` 读取键值，把 `KEY_EVENT_DOWN/UP` 转为 `MR_KEY_PRESS/RELEASE`，再经 `mr_app_translate_keycode()` 转成 Mythroad 键码。

`temp/.../MythroadSrc/mrp_application.c:895` 到 `temp/.../MythroadSrc/mrp_application.c:992` 显示平台键映射：

- `KEY_LSK` -> `MR_KEY_SOFTLEFT`
- `KEY_RSK` / `KEY_BACK` -> `MR_KEY_SOFTRIGHT`
- `KEY_ENTER` / `KEY_OK` -> `MR_KEY_SELECT`
- 数字、方向、音量、拍照等键也在平台层统一转换

结论：`gxdzc` 音乐确认框的问题如果本质是“点击应进入 SOFTLEFT 路径而不是 SELECT 路径”，更合理的修复位置应是模拟器输入/平台 UI 路由，而不是 patch `game.ext` 某条 `movs r0,#20` 指令。

### 3. 真机平台会统一转换触笔事件

`temp/.../MythroadSrc/mrp_application.c:1100` 到 `temp/.../MythroadSrc/mrp_application.c:1128` 注册的触笔处理函数只做坐标转换，然后发送：

- `MR_MOUSE_MOVE`
- `MR_MOUSE_UP`
- `MR_MOUSE_DOWN`

`temp/.../MythroadSrc/mrp_misc.c:1550` 到 `temp/.../MythroadSrc/mrp_misc.c:1625` 进一步说明触摸查询接口会读取平台触点，处理旋转/缩放，并转换为 Mythroad 触摸事件。

结论：平台源码没有支持“按包修改触摸表”的证据，也没有支持“普通触屏点击按底部坐标全局改写成软键”的证据。触摸坐标、旋转、缩放、多点触摸状态机都属于平台输入层职责。

### 4. 事件包装器维护事件成对和重复过滤

`temp/.../MythroadSrc/mrp_application.c:811` 到 `temp/.../MythroadSrc/mrp_application.c:859` 的 `mr_app_send_event_wrapper()` 是关键参考：

- `MR_MENU_SELECT`、`MR_MENU_RETURN`、`MR_DIALOG_EVENT`、`MR_LOCALUI_EVENT` 会重置前一次 key/mouse 状态并直接发送。
- `MR_KEY_PRESS/RELEASE` 只有状态发生变化时才转发，避免重复 press/release。
- `MR_MOUSE_DOWN/UP` 同样只在状态变化时转发。
- `MR_MOUSE_MOVE` 每次都转发。

当前 `src/main.c:467` 到 `src/main.c:517` 已经有 SDL 事件到 `event()` 的路径，但没有完整复刻这个 wrapper 语义。后续通用化时，应把事件包装器做成宿主无关的核心逻辑，SDL、Wasm、FFI 都复用它。

### 5. 本地 UI 会把按钮明确转换为 dialog/menu 事件

`temp/.../MythroadSrc/mrp_localui.cpp:434` 到 `temp/.../MythroadSrc/mrp_localui.cpp:449` 根据 `MR_DIALOG_OK`、`MR_DIALOG_OK_CANCEL`、`MR_DIALOG_CANCEL` 创建 OK/BACK toolbar。

`temp/.../MythroadSrc/mrp_localui.cpp:481` 到 `temp/.../MythroadSrc/mrp_localui.cpp:490` 将 toolbar item `0` 转为 `MR_DIALOG_KEY_OK`，item `1` 转为 `MR_DIALOG_KEY_CANCEL`。

`temp/.../MythroadSrc/mrp_localui.cpp:584` 到 `temp/.../MythroadSrc/mrp_localui.cpp:607` 对编辑页做相同处理。

`temp/.../MythroadSrc/mrp_localui.cpp:635` 到 `temp/.../MythroadSrc/mrp_localui.cpp:669` 对原生窗口页则把返回键和触笔事件转成 `MR_KEY_*` / `MR_MOUSE_*`。

结论：对于由 Mythroad 本地 UI 创建的对话框/编辑框，模拟器应优先生成 `MR_DIALOG_EVENT` 或对应平台键事件，而不是把所有点击都交给 MRP 内容区域自行命中。

### 6. 触屏能力是平台配置，不是样本补丁

`temp/.../MythroadSrc/mrp_setting.c:333` 到 `temp/.../MythroadSrc/mrp_setting.c:343` 根据编译期开关返回 `MR_ONLY_TOUCH_SCREEN`、`MR_TOUCH_SCREEN` 或 `MR_NORMAL_SCREEN`。

当前 `src/mythroad/dsm.c:799` 到 `src/mythroad/dsm.c:807` 对 `MR_CHECK_TOUCH` 固定返回 `MR_TOUCH_SCREEN`。这可以作为模拟器默认值，但更通用的方式是让屏幕/键盘能力来自模拟设备配置，而不是按样本调整。

### 7. 定时器和 pause/resume 是平台状态机的一部分

`temp/.../MythroadSrc/mrp_core.c:433` 到 `temp/.../MythroadSrc/mrp_core.c:451` 用平台 timer 启停 DSM 定时器。

当前 `src/mythroad/mythroad.c:4025` 到 `src/mythroad/mythroad.c:4133` 显示 pause/resume 会调用 native ext、Lua `suspend/resume`，并在 timer running/suspended 之间切换。

`src/mythroad/mythroad.c:4135` 到 `src/mythroad/mythroad.c:4186` 显示事件先给 native callback / native ext，再落到 Lua `dealevent`。

`src/mythroad/mythroad.c:4188` 到 `src/mythroad/mythroad.c:4248` 显示 timer 先处理 native ext / ARM dispatch，再落到 Lua timer 函数。

结论：gxdzc 下载模态框的 cancel/resume 问题，不应理解为 gxdzc 私有状态字段问题；更接近“wrapper/native ext 的模态层、timer 链和 resume 路由没有被模拟成真机平台状态机”。

## 对当前 `gxdzc` 修复的重新归类

| 当前逻辑 | 当前位置 | 更合理的通用归属 |
| --- | --- | --- |
| 修改 `game.ext` 触摸映射指令 | `patch_gxdzc_game_touch_map()` | 输入/本地 UI 路由：识别平台软键、dialog OK/CANCEL、触摸区域语义 |
| 模态期间把底部触屏坐标改成 `SOFTLEFT/SOFTRIGHT` | 已撤销的 runtime/SDL/modal 坐标转换 | 不能作为通用行为；只有可见平台 UI、物理/宿主软键或显式设备外壳控件才能产生软键事件 |
| cancel 后恢复 framebuffer | `arm_ext_save_gxdzc_modal_screen_snapshot()` | 通用屏幕层/窗口层：模态层关闭后露出底层 framebuffer 或触发底层重绘 |
| cancel 后恢复 game timer head | `saved_game_timer_head` + gxdzc 状态清理 | 通用 timer/suspend 状态机：wrapper suspend/resume 对子模块 timer 的影响应可被建模 |
| 清理 `rw+0x03C8`、`rw+0x0388` 等下载 pending 字段 | gxdzc 专用 offset | 不应作为模拟器通用行为；只能作为反汇编研究记录，不能进入核心通用路径 |
| 屏蔽 `netpay.mrp` | 已移除的 `netpay_blocked_for_current_pack()` | 可配置插件/网络/支付模拟策略，按能力和资源配置决定，不按游戏包名决定 |

## 建议的通用化准则

1. 不在模拟器核心中按 MRP 文件名分支。

   例外只能是测试夹具、兼容性数据库或显式用户配置，并且要有日志说明“启用了哪个兼容策略”。核心执行器和 DSM 层不应直接写 `strcmp(name, "xxx.mrp")`。

2. 不修改包内 ARM EXT 指令来修输入问题。

   如果某个点击路径期望 `MR_KEY_SOFTLEFT`，应检查平台输入、软键、本地 UI、模态路由，而不是 patch `game.ext` 的立即数。

3. 输入事件进入 VM 前要经过统一 wrapper。

   建议抽出一个宿主无关的 `mr_app_send_event_wrapper` 等价层，复用真机逻辑：状态去重、press/release 成对、dialog/menu/localui 事件重置状态。

4. 区分“内容触摸”和“平台 UI/软键”。

   真机上软键、toolbar、编辑框、本地对话框不一定是 MRP 内容坐标命中。模拟器需要有一层平台 UI hit-test 或状态路由，把这些输入转为 `MR_KEY_*` / `MR_DIALOG_EVENT`。如果当前屏幕没有显示对应平台控件，应用内容区域的触屏点击必须保持为 `MR_MOUSE_*`。

5. wrapper/native ext 的 dispatch、timer、suspend/resume 应按机制模拟。

   如果 wrapper 模态框通过 suspend 摘掉子模块 timer、关闭后 resume 恢复 timer，模拟器应识别这类通用结构和调用序，而不是按某个游戏的 `rw+offset` 字段修复。

6. 网络、支付、下载、插件失败应能力化。

   `netpay.mrp` 不可用、服务器下线、CMWAP 代理不可达等问题，应建成插件能力表、mock server 或网络策略，不应把“某游戏访问某文件失败”硬编码在文件系统层。

## 可作为后续开发的参考点

优先级从高到低：

1. 输入事件包装器：参考 `mrp_application.c:811` 到 `mrp_application.c:859`。
2. 平台键码转换：参考 `mrp_application.c:895` 到 `mrp_application.c:1030`。
3. 触笔/坐标转换：参考 `mrp_application.c:1100` 到 `mrp_application.c:1128`、`mrp_misc.c:1550` 到 `mrp_misc.c:1625`。
4. 本地 UI dialog/menu/edit 事件：参考 `mrp_localui.cpp:434` 到 `mrp_localui.cpp:607`。
5. 触屏能力配置：参考 `mrp_setting.c:333` 到 `mrp_setting.c:343`。
6. 定时器与 pause/resume：参考 `mrp_core.c:433` 到 `mrp_core.c:451`、`src/mythroad/mythroad.c:4025` 到 `src/mythroad/mythroad.c:4248`。

## 风险和未知

- `temp` 下源码是 MTK11B/COSMOS 风格平台层，不一定覆盖所有老机型、所有 wrapper ext 或所有 SkyMobi 插件行为。
- 平台源码能证明 Mythroad 公共事件和本地 UI 行为，但不能直接证明第三方 `cfunction.ext` / wrapper 内部结构的所有内存布局。
- 当前 gxdzc 相关注释来自反汇编定位，能解释历史修复，但不应被提升为模拟器通用契约。
- 底部触屏坐标是否应全局转为软键，平台源码只能提供间接依据：平台键和本地 UI toolbar 明确产生软键/dialog 事件；普通 DSM 触笔 handler 仍会把坐标作为 `MR_MOUSE_*` 发送。因此实现时应由当前 UI 状态和设备形态决定，不应无条件改写所有底部触摸。

## 判断

可以把 `temp/.../Mythroad` 作为模拟器开发参考依据。它支持的主要结论是：模拟器应复刻 Mythroad 平台层的通用输入、UI、定时器和生命周期契约；现有按 `gxdzc` 包名、固定 offset、固定 rw 字段做补丁的代码，应逐步迁移到通用机制或显式兼容策略中。
