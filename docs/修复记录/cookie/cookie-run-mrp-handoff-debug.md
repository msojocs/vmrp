# cookie 应用列表 MRP 接力启动调试记录

## 2026-07-15 基线与目标

- 目标用例：`test/e2e/cookie/run-mrp.test.ts`。
- 已完成的前置修复是短按键与 guest timer generation 的同步边界；当前 `HEAD=22f92a9`，工作区在本轮开始时干净。
- 用例末尾在“文件管理”中选中“应用列表”，打开菜单并选择“进入”；当前代码随后只有 `delay(10_000)` 和“应用没有启动”的 TODO，尚未用 PPM 或进程状态证明接力结果。
- 本轮目标是定位为何选择另一个 MRP 后整个运行时退出，并实现通用的生命周期修复，使下一个应用在同一宿主进程中成功启动。

## 约束

- 不使用 `xvfb`；E2E 使用 SDL dummy 驱动或当前真实显示环境。
- 不采集无边界 ARM trace；先用生命周期日志、候选地址 hook、反汇编和 PPM 差异缩小范围。
- 生产代码不判断 cookie、特定 MRP 文件名、像素或测试步骤，不增加失败后继续运行的兜底分支。
- 所有生产代码改动必须有解释不直观生命周期语义的注释。
- 先修复目标流程，再运行其它 E2E 兼容回归。

## 初始假设（均待证据验证）

1. guest 可能按平台协议先请求退出当前应用，再通过 DSM 状态/启动参数请求下一个应用；宿主把前者错误解释为进程终止。
2. `mr_state`、restart 参数或 DSM 启动队列可能在 ARM EXT 到 native Mythroad 再到 `main.c` 的边界丢失。
3. guest 也可能正确请求启动，但当前 app 的迟到退出事件覆盖了新 app 的状态。

## 下一步证据

- 复现末尾退出并保留 stdout/stderr、socket 状态和前后 PPM。
- 从 cookie 包提取相关 EXT，反汇编应用列表“进入”处理、平台启动调用和当前应用退出调用。
- 对照 `src/mythroad` 的 restart/stop 状态机与 `src/main.c` 主循环，找到 guest 指令到宿主进程退出的完整调用链。

## 2026-07-15 首次确定性复现

- 标准命令 `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose` 返回 1 passed，但当前用例末尾只有固定 delay，没有末态断言，因此这是测试假绿。
- 使用同一个 `VmrpE2e` helper 逐步执行且不修改测试文件，六个现有 PPM 像素全部命中；最后一条 `KEY LEFT_SOFT` 返回 `OK key 61 exited`，此前 `draw_count=17`，进程在 1 秒内退出。
- 保留现场：artifact `/tmp/vmrp-e2e-2hb4x2`，workspace `/tmp/vmrp-ws-QAEOpj`。命名 PPM 均为 `240x320` P6；最后按键后 socket 已随进程关闭，无法再截末态图。
- stdout/stderr 共 27 行，没有启用 trace。关键顺序是一次 `native dsm_init success`，随后解析 `applist2.sky-mobi.net` 失败，最终打印 `mythroad exit.`；不存在第二次 DSM/MRP 初始化。
- 工作区中存在 `mythroad/dsm_gm.mrp`，所以不能把退出归因于默认返回包缺失。

## 当前 native 生命周期线索

- `src/mythroad/dsm.c::mr_exit()` 先调用 `mr_restart_old_app()`；只有 `old_pack_filename` 非空才进入 `MR_STATE_RESTART`，否则调用平台 `exit`。
- `src/mythroad/mythroad.c::mr_start_dsm()` 会无条件清空 `old_pack_filename` 和 `old_start_filename`。当前命令行直接启动 cookie，因此若 guest 的“应用列表”路径没有另行登记返回应用，末尾 `mr_exit()` 必然落到宿主退出。
- 下一步需要用反汇编确认 cookie 在按下“进入”后实际调用了哪个 Mythroad ABI，以及它按原平台协议期望由谁写入返回应用/下一个应用参数；不能仅凭现象给 `old_pack_filename` 填默认值。

## 2026-07-15 根因反汇编

- `cookie_v6110.mrp:game.ext` 解压长度为 `0x40B24`。其 `+0x37C1C` 函数通过 `mr_table + 102*4` 和 `mr_table + 103*4` 取得 `old_pack_filename` / `old_start_filename`，分别先清零 32 字节，再最多复制 31 字节。`0x198=102*4`、`0x19C=103*4` 的指令偏移与原生 `mr_helper_st` 布局完全一致。
- 调用点 `game.ext+0x0FC2` 登记旧应用后，紧接着调用 `game.ext+0x37B54`。后者写 `table[100]/[101]` 的目标 pack/start，调用 `table[31]` 启动 100ms timer，把 internal timer state 设为 RUNNING，并把 internal `mr_state` 设为 `3`（`MR_STATE_RESTART`）。guest 已按标准 ABI 完整发起接力。
- 原生 `src/mythroad/mythroad.c` 的 table 100..103 都指向 128 字节可写数组；当前 Unicorn 表只有 table[100] 使用 128 字节可写缓冲，table[101..103] 使用 `alloc_string()` 按初值实际长度分配。旧包初值为空时槽位只有 1 字节，guest 的标准 32 字节清零不仅不会同步给 host，还会越界覆盖相邻 guest 元数据。
- `arm_ext_finish_callback_state()` 的注释区分 RESTART(3) 与 STOP(4)，实现却把所有 `arm_state >= 3` 合并为 `mr_exit()`。这绕过了 `src/mythroad/mythroad.c::mr_timer()` 中唯一正确的 RESTART 消费路径。

## 修复设计

1. table[100..103] 全部使用与原生数组同容量的 128 字节 guest 缓冲，保存明确地址，不再按初值长度分配。
2. guest 返回 RESTART/STOP 时，把四个缓冲有界同步回 native Mythroad；这是共享内存 ABI 的等价实现，不在普通回调中猜测包名。
3. RESTART 只把 host `mr_state` 同步为 3，让 guest 已启动的 100ms timer 自然进入 `mr_timer()` 的 stop/start 分支；STOP 才调用 `mr_exit()` 执行旧应用恢复或平台退出。
4. 修正 table[131] 的 cmd=3 在 `L == NULL` 时仍访问 Lua 栈的问题：ARM bridge 没有 Lua 第三参数，按 mini Mythroad 的标准行为使用 `start.mr`。

## 2026-07-15 生命周期修复后的第二层失败

- 四槽和 RESTART 修复后，最后一次左软键不再令宿主退出。动态值为：

  ```text
  pack      = dsm_gm.mrp
  start     = cfunction.ext
  old_pack  = cookie_v6110.mrp
  old_start = logo.ext
  mr_state  = RESTART(3)
  ```

- 100ms timer 随后进入既有 `mr_timer() -> mr_stop() -> _mr_intra_start()` 分支，证明生命周期接力已经恢复。但是启动目标是 368 字节 `cfunction.ext`，把它直接交给 Lua 会返回语法错误；按 MINI 顺序执行 EXT 的 code 6/8/0 虽全部返回 0，仍没有新画面。
- 定向反汇编证明该 EXT 只在 code 2 时轮询 `0x80110000` 的 NES 按键位图。它没有绘制 table 调用、timer 注册或 UI 回调，所以 code 6/8/0 成功只证明桥调用完成，不能证明第二应用启动。
- 失败 PPM 在 1 秒和 3 秒完全相同，SHA-256 都是 `e4f29b52626d22591eddfdb27758c90aa9e18cf44933f83e955c3984fa597c41`；画面仍是 Cookie 文件管理页的“正在启动，请稍候...”残帧。这排除了“再等待一会儿会绘制”的解释。

## 入口选择与版本契约根因

- `dsm_gm.mrp` 的索引读取正常，包内同时存在 37,230 字节 `start.mr` 和 368 字节 `cfunction.ext`。`start.mr` 的字符串包含 `c_load`、`_strCom` 和 `cfunction.ext`，说明 Lua UI 会自行加载该按键 helper。
- Cookie `game.ext+0x0E40` 的启动选择先读取平台版本并与 2000 比较。版本小于 2000 时保留 `start.mr`（再按存在性选择 `shell.mr` / `startqq.mr`）；版本大于等于 2000 时才探测 `cfunction.ext`，探测成功便明确选择它。该读取通过 table[40]/[41]/[44]/[45] 完成，不经过 table[125]，因此不能在文件 bridge 中伪造失败来改变入口。
- `git show 87fa2d3^:src/mythroad/mythroad.c` 显示重构前 FULL runtime 的 `MR_VERSION` 是 `1968`；同一时期的 MINI runtime 才是 `2011`。提交 `87fa2d3` 为 FULL 增加 Unicorn EXT 支持时把版本改成了 `2011`，但并未把完整 Lua 启动器替换成 MINI 的纯 EXT 启动器。
- 因此根因是把“能执行 EXT”误当成“采用 MINI 启动 ABI”。Cookie 按宿主声明选择 `cfunction.ext` 是正确行为，不能在接力 bridge 中把 guest 明确请求的入口偷换成 `start.mr`。

## 最终实现

1. ARM table[100..103] 全部映射为 128 字节固定容量 guest 数组，并保存四个稳定地址。
2. guest 发布 RESTART 或 STOP 时，四个数组才有界同步回 native Mythroad；普通包别名切换不触发 handoff。
3. RESTART 只发布 `MR_STATE_RESTART`，由已经 armed 的 timer 执行标准 stop/start；STOP 才走 `mr_exit()` 的旧应用恢复或宿主退出语义。
4. table[131] 的无 Lua 栈调用按 SDK 默认登记 `start.mr`。
5. FULL `mythroad.c` 恢复对外版本 `1968`，MINI `mythroad_mini.c` 保持 `2011`。Cookie 因而请求 `dsm_gm.mrp:start.mr`，Lua 主程序再按自身协议加载辅助 EXT。
6. E2E 合成键使用专用 `windowID` 标记，并把唯一 token 放在 SDL 不会重写的 `keysym.scancode`；主线程在 DOWN/UP guest 回调完成后发布精确回执，避免最后一次左软键的同步边界依赖 SDL `timestamp`。

未保留直接 EXT 入口 bootstrap、按包名/文件名改入口、无绘制 fallback、像素驱动的生产逻辑或强制空屏绘制。它们要么违背 guest 的明确选择，要么只能掩盖“helper 被当成应用”的错误。

## 最终画面与回归

- `test/e2e/cookie/run-mrp.test.ts` 删除固定 `delay(10_000)` 和 TODO，改为等待应用列表主体像素 `(120,180)=[24,160,200]`，再断言标题栏 `(120,10)=[24,104,136]`。旧等待页对应两点分别为 `[200,224,248]` 和 `[0,24,112]`，不会误通过。
- 保留的最终 artifact 是 `/tmp/vmrp-e2e-aHIAVT`。`second-app.ppm` 为 `240x320` P6，SHA-256 是 `d3d014208fb1fac6862c4c2ab2a6479aa582a09b3a6df8908f74e9301d29c3ba`；stderr 明确出现第二次 `_mr_intra_start` 和 `dofile('start.mr')`。
- 所有图形验证都使用 `SDL_VIDEODRIVER=dummy` 和 `SDL_AUDIODRIVER=dummy`，没有使用 `xvfb`，也没有保留大范围 ARM trace。
- 最终验证：

  ```text
  cmake --build build --target vmrp -j2                         passed
  ctest --test-dir build --output-on-failure                    1/1 passed
  pnpm exec tsc --noEmit                                        passed
  pnpm exec vitest run test/e2e/cookie/run-mrp.test.ts          1/1 passed
  pnpm exec vitest run test/e2e                                 19 files, 31 tests passed
  ```
