# mpc.mrp 启动与内部菜单无响应问题记录

## 背景

执行：

```bash
build/vmrp mythroad/mpc.mrp
```

最初会在启动后出现 ARM EXT 执行错误；修复后程序可启动，但点击应用内部菜单没有反应。

## 问题一：启动后的无效指令

### 现象

日志中出现：

```text
arm_ext_executor: uc_emu_start(0x66D7C9) failed: 10 (Invalid instruction (UC_ERR_INSN_INVALID))
pc bytes @0x66D7D4: 49 46 00 91 00 24 FF F7 FA E9 ...
```

PC 附近字节是 Thumb 指令，但执行器启动 Unicorn 时把入口地址低位清掉了，导致 Unicorn 没有按 Thumb 状态进入。

### 修复

`src/arm_ext_executor.c` 中保留入口地址低位传给 `uc_emu_start`，让 Unicorn 根据 `addr | 1` 正确进入 Thumb 模式。

## 问题二：内部菜单点击无响应

### 排查过程

添加临时 trace 后确认：

1. SDL 鼠标事件能到达 `mr_event`。
2. `mr_event` 能进入 Lua 层 `dealevent`。
3. Lua 层会通过 `_strCom(801, ...)` 把事件转给 ARM EXT。
4. 事件最初被转发到后加载的 `skyfont.ext` helper，而不是应用主逻辑 `game.ext`。
5. `skyfont.ext` 对 `code=1` 事件返回 `MR_IGNORE`，所以事件被忽略，且没有触发绘制。

`mpc.mrp` 启动过程中会加载多个嵌套 EXT：

- `game.ext`：应用主逻辑，应该接收生命周期/事件类回调。
- `skyfont.ext`：后续加载的辅助 EXT。

原执行器只有一组 `active_helper_addr/active_p_addr`，每次嵌套 EXT 加载都会覆盖它。结果事件类调用 `code=1` 被发送给最后加载的 EXT。

### 修复

在 `src/arm_ext_executor.c` 中记录第一个嵌套 EXT 为 primary EXT：

- `primary_helper_addr`
- `primary_p_addr`

对于生命周期/事件类调用 `code 0..5`，优先路由到 primary EXT；其他调用仍使用当前 active EXT。

这让 `MR_MOUSE_*`、`MR_KEY_*` 等事件重新进入 `game.ext`，内部菜单点击后会返回 `MR_SUCCESS` 并触发绘制刷新。

## 验证

构建：

```bash
cmake --build build
```

运行：

```bash
timeout 30 build/vmrp mythroad/mpc.mrp
```

验证结果：

- 不再出现 `UC_ERR_INSN_INVALID`。
- 启动成功：`vmrp_runtime_start_dsm(...): 0x0`。
- 点击内部菜单区域后，事件被 `game.ext` 处理并触发局部/全屏刷新。

## 注意

`mr_menuCreate/mr_menuShow` 等平台菜单接口仍是未实现状态，但这次的菜单无响应不是系统菜单 API 问题；`mpc.mrp` 使用的是应用内部菜单，由 `game.ext` 处理输入和绘制。
