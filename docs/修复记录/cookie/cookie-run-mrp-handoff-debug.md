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
- 保留现场：artifact `/tmp/skyengine-e2e-2hb4x2`，workspace `/tmp/skyengine-ws-QAEOpj`。命名 PPM 均为 `240x320` P6；最后按键后 socket 已随进程关闭，无法再截末态图。
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
- 保留的最终 artifact 是 `/tmp/skyengine-e2e-aHIAVT`。`second-app.ppm` 为 `240x320` P6，SHA-256 是 `d3d014208fb1fac6862c4c2ab2a6479aa582a09b3a6df8908f74e9301d29c3ba`；stderr 明确出现第二次 `_mr_intra_start` 和 `dofile('start.mr')`。
- 所有图形验证都使用 `SDL_VIDEODRIVER=dummy` 和 `SDL_AUDIODRIVER=dummy`，没有使用 `xvfb`，也没有保留大范围 ARM trace。
- 最终验证：

  ```text
  cmake --build build --target skyengine -j2                         passed
  ctest --test-dir build --output-on-failure                    1/1 passed
  pnpm exec tsc --noEmit                                        passed
  pnpm exec vitest run test/e2e/cookie/run-mrp.test.ts          1/1 passed
  pnpm exec vitest run test/e2e                                 19 files, 31 tests passed
  ```

## 2026-07-15 子应用退出回退失败续查

- 当前暂存的 `test/e2e/cookie/run-mrp.test.ts` 已扩展为：在 Cookie 文件管理器中选择末项“冒泡浏览器”，启动 `wbrw.mrp`，再通过 WBRW 菜单选择“退出”，预期回到 Cookie 文件管理器启动前的末项位置。
- 目标用例无 `xvfb` 复现失败：`VMRP_E2E_KEEP_TMP=1 pnpm exec vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose`。产物目录 `/tmp/skyengine-e2e-Kn7hW0`，工作目录 `/tmp/skyengine-ws-tYKhkM`。
- 失败点是最后的 `back-to-file-manage`，断言点 `(100,216)` 实际为 `[248,252,248]`，不是文件管理器高亮 `[248,200,24]`。PPM 转 PNG 后确认末态仍是 WBRW 首页，底栏显示左软键“菜单”、右软键“退出”，不是 Cookie 文件管理器。
- 日志中 `_mr_intra_start` 出现三次，说明 Cookie、WBRW、以及一次返回旧应用的启动路径都被触发；当前首要问题不是宿主进程直接退出，而是“退出”后的旧应用/前台状态没有恢复到 Cookie 文件管理器的原位置。
- 下一步证据：用逐键截图确认 WBRW 退出确认链路是否真正调用 `mr_exit()`，再用窄口径 table[54]/`MR_STATE_STOP`/old app 文件名日志或反汇编确认 WBRW 退出时写入的返回参数。

## 2026-07-15 子应用退出后的“正在打开”帧修复

- 逐帧复查确认，返回父应用不能走冷启动，也不能只恢复最终文件列表快照。冷启动父 Cookie 会重新出现启动流程；只恢复暂停快照会直接显示最终列表，丢失文件管理器打开目标文件夹时应有的“正在打开”过渡。
- `_RL` 返回参数在 resume 前后重放也不足以生成正确过渡帧；正确边界是父应用自己在 `mr_findStart()` 成功打开目录前已经绘制过的目录打开提示。因此在 DSM `mr_findStart()` 成功后记录当前 `mr_screenBuf`，作为应用自绘的目录打开过渡帧。
- 暂停父应用用于 RESTART handoff 时，快照同时保存最终列表帧和最后一次目录打开过渡帧。子应用 STOP/退出后恢复父 VM 时，先用窄窗口 `_DispUpEx()` 回放目录打开帧，再恢复最终列表帧并调用 `resumeApp()`。这保留了“打开目标文件夹”的视觉流程，且没有 Cookie/WBRW 包名判断、文字识别或像素驱动生产逻辑。
- E2E 增加 `SCREEN_DRAW`，从 SDL draw ring 逐帧导出退出后的 PPM。`test/e2e/cookie/run-mrp.test.ts` 在最后确认退出后收集 `return-draw-*.ppm`，断言至少一帧命中“正在打开”像素签名，并断言没有命中“正在启动”签名。
- 本次无 `xvfb` 验证产物为 `/tmp/skyengine-e2e-8tsnlb`。逐帧证据：`return-draw-0046.ppm` 命中“正在打开”，关键像素为 `(194,249)=[24,96,216]`、`(150,251)=[0,76,200]`、`(100,216)=[248,252,248]`、`(8,259)=[232,240,248]`、`(42,269)=[232,240,248]`；`return-draw-0047.ppm` 起回到文件管理器最终列表，`(100,216)=[248,200,24]`。退出后采集帧未命中“正在启动”签名。
- 最终验证：

  ```text
  cmake --build build --target skyengine -j2                                      passed
  VMRP_E2E_KEEP_TMP=1 pnpm exec vitest run test/e2e/cookie/run-mrp.test.ts    1/1 passed
  pnpm exec tsc --noEmit                                                     passed
  ctest --test-dir build --output-on-failure                                 1/1 passed
  ```

## 2026-07-15 并行全量回归复核

- 不能把兼容性证明降级为顺序执行；默认并行全量 E2E 必须可用。一次被用户中断的顺序复核残留了后台 Vitest/skyengine 进程，已清理后重新执行默认并行命令。
- 默认并行命令 `pnpm exec vitest run test/e2e --reporter=verbose` 通过，结果为 `19 files, 31 tests passed`，总耗时 `273.01s`。这覆盖 Cookie 目标用例以及 DOTA/GGHJT/Optwar/Talkcat 等插件下载、退出、返回重进路径。
- 另用 `VMRP_E2E_KEEP_TMP=1 pnpm exec vitest run test/e2e/cookie/run-mrp.test.ts --reporter=verbose` 保留最新逐帧产物 `/tmp/skyengine-e2e-Vdk5ke`。`return-draw-0047.ppm` 命中“正在打开”签名，且视觉转换图 `return-draw-0047.png` 底部文本为“正在打开”；`return-draw-0048.ppm` 起回到文件管理器最终列表，采集帧未命中“正在启动”签名。

## 2026-07-16 直接确认键启动路径复现与快照根因

- 新增目标路径“方式二”不打开文件菜单，而是在文件管理器末项直接发送
  `ENTER`。精确命令
  `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/cookie/run-mrp.test.ts -t 方式二 --reporter=verbose`
  稳定失败于“返回帧不得出现正在启动”的断言；本轮首个保留产物为
  `/tmp/skyengine-e2e-ESL4Ry`。
- PPM 逐帧证明恢复过程不是父应用冷启动：`return-draw-0043.ppm` 正确显示
  “正在打开”，`0044.ppm`、`0045.ppm` 随后恢复文件列表却保留底部
  “正在启动，请稍候...”。生命周期诊断也只出现 Cookie、WBRW 两次
  `_mr_intra_start`，并命中 `LIFE snapshot restore target`。
- 方式一与方式二的唯一区别是启动输入路径；方式一当前通过，方式二失败。
  `src/arm_ext_executor.c::arm_ext_finish_callback_state()` 固定使用
  `snapshot_age = 1`，而 `src/arm_ext/aex_screen.c` 的 present history 只按每次
  显式提交机械入队。两个 guest 入口在发布 RESTART 前产生的提交次数不同，
  所以“倒数第二次提交”不是可依赖的暂停边界。
- 更根本的问题是 `arm_ext_restore_presented_state_by_age()` 为取得旧画面会回滚
  整段 ARM `m->mem`、low table 与 allocator 账本，却没有同步回滚 Unicorn
  寄存器、nested module/前台归属、宿主 timer 元数据以及文件或网络副作用。
  这会构造两个时间点混合的 VM；不能通过把 age 改成另一个常数解决。
- `cookie_v6110.mrp:game.ext` 反汇编再次确认 guest 协议正确：`+0x0FC2`
  调 `+0x37C1C` 登记 old pack/start，`+0x0FCC` 调 `+0x37B54`；后者写
  table[100]/[101]，在 `+0x37BB2..+0x37BBA` 经 table[31] 启动 100ms timer，
  在 `+0x37BBC..+0x37BC2` 写 timer state 1，并在
  `+0x37BC4..+0x37BCC` 写 `MR_STATE_RESTART(3)`。
- 下一步受控实验：保留历史屏幕仅作为 paused app 的视觉 `screen_image`，不再
  用它回滚 guest 内存；在 guest 完成 handoff 发布并执行 `mr_pauseApp()` 后保存
  真实执行状态。先同时验证方式一/方式二和返回后正常按键，再扩展相关回归。

## 2026-07-16 distinct visible state 修复与目标验证

- 受控实验否定了“只恢复历史像素，不恢复配套 guest 状态”：即使把稳定列表
  像素写回 parent framebuffer，`resume(code=5)` 仍会根据较新的 guest UI
  globals 重新绘制“正在启动”。因此 paused checkpoint 必须保持历史屏幕和产生
  该屏幕的 ARM memory 同代；问题不是“完全不应恢复 guest state”，而是原代码
  选择的代际没有视觉状态语义。
- `VMRP_LIFECYCLE_DIAG=1` 的全帧 FNV 对齐证明，方式二发布 RESTART 时
  `age=0`、`age=1` 都是同一启动提示，指纹均为 `65B71FBC96D967BB`；
  `age=2` 为启动前文件列表，指纹 `658F0762D29F633C` 与
  `file-manage-bottom.ppm` 由 RGB888 还原为 RGB565 后完全一致。原固定
  `snapshot_age=1` 因而恢复了重复提交的同一启动状态。
- `arm_ext_note_screen_presented()` 现在只推进不同的可见状态：如果整张
  framebuffer 与最新历史项逐字节相同，则在原槽更新配套的最新 ARM memory 和
  allocator 状态，不增加 history generation。这样 history age 1 表示“上一张
  不同的显式可见状态”，不再表示“上一次 draw 调用”。
- RESTART 保存逻辑不再以缺失历史时的当前屏幕兜底；上一代屏幕和配套 memory
  任一不可用就不发布不完整 paused checkpoint。实现不检查应用名、包名、按键、
  文本或像素，不包含 Cookie/WBRW 场景分支。
- 测试先在 workspace 准备 `wbrw.mrp` 再启动 Cookie，移除初始枚举竞态；返回
  断言比较 `y=0..295` 整个应用内容区与启动前 PPM，状态栏时钟区域除外。该区域
  完整覆盖标题、列表、选择位置和底部启动/打开提示。
- 最终 distinct-history 实现的无 `xvfb` 保留产物 `/tmp/skyengine-e2e-uhqu1M` 中，方式二
  `return-draw-0043.ppm` 为“正在打开”；`0044.ppm`、`0045.ppm` 的 SHA-256
  `a7c2a05df997b006d006f7dbd3e27d699daa8838c6b2a9f344492e774c267847`
  与启动前 `file-manage-bottom.ppm` 完全相同。返回后 `RIGHT_SOFT` 仍能进入
  Cookie 主界面，证明恢复的是可运行父 VM，而不是静态截图。
- 已完成验证：

  ```text
  cmake --build build --target skyengine -j2                         passed
  pnpm vitest run test/e2e/cookie/run-mrp.test.ts -t 方式二     1 passed
  pnpm vitest run test/e2e/cookie/run-mrp.test.ts -t 方式一     1 passed
  pnpm exec tsc --noEmit                                        passed
  ctest --test-dir build --output-on-failure                    1/1 passed
  pnpm exec vitest run test/e2e --reporter=verbose              19 files, 32 tests passed
  ```

- 默认并行全量 e2e 未使用 `xvfb`，总耗时 `315.18s`。除 Cookie 两种路径外，
  DOTA/GGHJT/Optwar/Talkcat 的下载、退出、返回重进、付费超时和取消循环，
  Gzwdzjs/GTCM/GXDZC 以及 golden 全图检查均通过。

## 2026-07-16 方案复审：present history / 快照回放方案被否决

- 基线复测：当前工作树 `pnpm vitest run test/e2e/cookie/run-mrp.test.ts -t 方式二`
  通过（产物 `/tmp/skyengine-e2e-NY0sLP`），但通过方式是伪造的：
  `return-draw-0044/0045.ppm` 与启动前 `file-manage-bottom.ppm` SHA-256 完全相同
  （`cddd8822f93803b6…`），`return-draw-0043.ppm` 的“正在打开”是
  `mr_findStart()` 时截存的旧帧回放。返回过程中文件管理器并没有真正重新打开
  目录、重新加载文件列表——与预期“返回后重新加载文件列表”矛盾。
- 判定为不健全、需移除的机制：
  1. `present_screen_history` 内存时间旅行（`aex_screen.c`）：把整段 ARM 内存与
     像素配对存档，RESTART 时按硬编码 `snapshot_age=1` 回滚 `m->mem`/low_table/
     allocator 账本。只回滚内存不回滚 Unicorn 寄存器、宿主 timer 元数据、
     nested module、文件/网络副作用，构造两个时间点混合的 VM；“相同帧不推进
     代际”的去重启发式只是让 age=1 恰好落在 Cookie 的绘制序列上，换一个在
     发布 RESTART 前多画几帧的应用即失效。真机上不存在“宿主替应用保存可见
     状态历史”的机制。
  2. `mr_note_directory_open` 帧回放（`dsm.c`/`mythroad.c`）：宿主在返回时经
     `_DispUpEx` 回放截图，“正在打开”成为幽灵帧。
  3. 为支撑上述机制的绕行：`mr_present_restored_screen` 临时篡改 `mr_state`、
     `arm_ext_finish_callback_state` 中“清状态槽→mr_pauseApp→写回”的序列。
- 反汇编证据（提取自 `test/fixtures/cookie_v6110.mrp`，`game.ext` 共 0x40B24 字节）：
  字面量池含 `reload.flag`(0x3E464)、`mulreload.flag`(0x3E470)、
  `mrparam.bak`(0x3E454)、`cookie.inf`(0x3E48C)、参数令牌 `_RL`(0x3E094/0x3E460)、
  `_CH`、`*J,`、`_1X`、拼接格式 `"%s&%s&%s&%d"`(0x3E0E8)；`res_lang0.rc`
  （UTF-16BE）同时含“正在打开”(0x21CE) 与“正在启动”(0x5AC)。Cookie 自带跨启动
  的 reload 协议，宿主职责是忠实传递 handoff 状态，而不是回放画面。
- 下一步（Phase 1）：反汇编 `reload.flag`/`mrparam.bak`/`_RL` 的使用方，确定
  Cookie 的返回重载协议走“冷重启 + 自持久化状态”还是“父 VM 存活 resume”，
  据此重做实现。

## 2026-07-16 Phase 1 运行时证据：launch 前持久化 + `_RL` 冷启动实测

- `SKYENGINE_LOG=1 VMRP_LIFECYCLE_DIAG=1` 逐事件追踪（产物 `/tmp/skyengine-e2e-ombWvn`）确认协议：
  - Cookie 从文件管理器启动 wbrw 前，先写 `mythroad/fm.sav`（13 字节，
    `00 00 00 01 | 00 00 00 00 | 01 00 00 00 | 06`，末字节 0x06 与选中项
    序号 6 吻合；字面量 0x3F094 紧邻 `..\appmgr\filemanager\fm_view.c`），
    并更新 `dl.dlm`/`nfy.dlm`/`fpd.sav`/`rollmsg.dat`；随后经 wrapper 写
    table[100..103]（old=自身）并把 table[138] 置为 `_RL`，发布 RESTART。
  - wbrw 以 `param='_RL'` 冷启动；wbrw 退出时其 wrapper 直接把
    old_pack/old_start 拷回 pack/start（`pack='~p0'` 短别名）再次发布
    RESTART，param 仍为 `_RL`。`reload.flag`/`mulreload.flag`/`mrparam.bak`
    在该流程中从未被访问。
- 受控实验：临时移除快照拦截（`mr_timer` RESTART 分支回归 `mr_stop()+_mr_intra_start()`，
  `arm_ext_finish_callback_state` RESTART 分支回归“同步文件名+发布”），
  产物 `/tmp/skyengine-e2e-aqDs8w`：返回时 Cookie 真实冷重启，**跳过了开机 splash
  直接进主页图标网格**（`_RL` 参数被 Cookie 识别），但没有回到文件管理器，
  也从未尝试读 `fm.sav`。断言 `isOpeningFolderFrame` 失败。
- 结论：宿主侧 handoff（参数同步、包名别名、冷重启）已经把 `_RL` 送达；
  缺口在于 Cookie 启动路径进入“恢复上次视图（文件管理器）”分支所需的
  条件还未满足。已把 `fm.sav` 读取方、`_RL` 解析分支、fm.sav 字段布局列为
  反汇编优先问题（后台 Agent 进行中）。

## 2026-07-16 Phase 2 前置清理（时间旅行机制移除）

- 已按方案复审结论移除：`aex_screen.c` 整体回退到 HEAD（present history 环、
  配对 ARM 内存 checkpoint、`arm_ext_restore_presented_state_by_age` 全部删除）；
  `arm_ext_internal.h` 删除 history 字段与 `ARM_EXT_PRESENT_HISTORY_MAX`；
  `arm_ext_executor.c` 删除 `arm_ext_snapshot_pre_handoff_screen`、unload 释放环、
  RESTART 分支回归“同步 handoff 文件名/参数 + 发布 RESTART”；`mythroad.c` 删除
  `MrPausedAppSnapshot` 全套（421 行）、目录打开帧截存/回放、
  `mr_present_restored_screen`；`dsm.c` 删除 `mr_note_directory_open` 调用；
  相关头文件声明同步清理。构建通过（仅存量警告）。
- 保留（判定为符合真机共享内存 ABI 的修正）：table[138] 参数槽固定 128 字节
  并纳入 handoff 同步、包名短别名 `~pN`（32 字节固定缓冲约束）、
  `_mr_intra_start` 重置 DSM work path、pause/resume Lua 优先顺序、
  e2e SCREEN_DRAW 抓帧设施、`VMRP_LIFECYCLE_DIAG` 诊断。
- 当前状态：返回路径为真实冷重启，`_RL` 已送达 Cookie 且被识别（跳过 splash
  直达主页），待反汇编查明自动重入文件管理器的门槛条件。

## 2026-07-16 反汇编成果：Cookie 返回重载协议全貌（根因确认）

以下偏移均为 `game.ext` 文件偏移（= 运行地址 − 0xE80000；`arm_ext_load` 把整个
文件 memcpy 到 EXT_CODE_ADDR，无头部跳过）。完整反汇编锚点见
`/tmp/cookie-analysis/notes.md`（临时目录，要点已并入本节）。

### 根因

**Cookie 的续传状态是存放在 table[138]（start_fileparameter[128]）里的
128 字节二进制记录，不是 C 字符串**：`"_RL\0"` 前缀，中段全零，`+0x6C` 为
大端 u32 的 “fm 状态已保存” 标志（=1），`+0x70/+0x74/+0x78/+0x7C` 为四个
大端 u32 视图恢复字段（w0 低字节 = 视图 id，位于缓冲区最后一个字节 +0x7F）。
模拟器在两处按 C 字符串处理该缓冲：

1. `arm_ext_executor.c` `alloc_filename_table_slot()` 用 `snprintf "%s"` 给重启
   实例的 table[138] 播种 —— NUL 后的字段全部丢失；
2. handoff 同步 `arm_ext_sync_filename_slot()` → `mr_set_start_fileparameter()`
   → `mr_set_filename()`（STRNCPY）—— 发布时截断；且通用槽同步会给
   value[127] 补 NUL，恰好踩掉视图 id 低字节。

于是重启后 Cookie 仍匹配到 `"_RL"`（跳过 splash，与实测一致），但 +0x6C 与
视图字段读回为 0 → 落在主页图标网格、永不读取 `fm.sav`（与实测一致）。

### 协议全流程（反汇编证据）

- 子应用启动函数 0xE40：`0x3528` 收集 4 个视图状态字 → `0x338D0`
  save_reload_state → `0x37C1C` 写 old_pack/old_start（table[102]/[103]）→
  `0x37B54` 写 pack/start（table[100]/[101]）+ table[31] 100ms timer +
  mr_state=RESTART(3)。
- `save_reload_state@0x338D0`（平台版本 ≥1946 路径）：备份旧参数到
  `mrparam.bak`（若以 `*` 开头或已有 `_CH` 记录）→ `memset(param,0,16)` →
  `strcpy(param,"_RL")` → 四个视图字大端写入 +0x7C/+0x78/+0x74/+0x70 →
  写 `l1XrunState.sav` → `fm_save@0xC36C`。版本 <1946 的旧路径才把这些字写进
  `reload.flag` 文件（`mulreload.flag` 只用于 `_CH` 渠道返回内嵌 `_RL` 的场景）。
- `fm_save@0xC36C`：删除并重写根目录 `fm.sav`（格式：BE32 magic=1 | u8 目录名长 |
  目录串 | BE32 深度 | BE32 选中项…；实测 13 字节 = magic1/len0/depth1/sel6），
  并在 param+0x6C 写大端 1。
- 冷启动检查器 `0x337BC`（仅 init/code=0 路径调用，经 `0x37078` 事件分发 →
  `0x36708` → `0x337bc`）：`strncmp(param,"_RL")` 命中 → 置全局 reload 标志 →
  读并删除 `l1XrunState.sav`；`"_1X"` 是备选令牌。消费者：跳过 splash 动画
  （0x33386/0x3343a）、跳过主页横幅（0x33614）、视图恢复分发器 `0x4354`。
- `0x4354`：reload 标志为 0 → 正常主页流程；为 1 → `read_reload_state@0x33680`
  读回四个视图字 → `0x2780C` 分发进保存的视图。`read_reload_state` 中
  `BE32(param+0x6C)==1` 才调用 `fm_load@0xC098`（读 `fm.sav`、恢复目录串/深度/
  选中项后删除文件）；随后 `memset(param,0,16)`，若存在 128 字节的
  `mrparam.bak` 则复制回 param 并删除。
- suspend(code=4)@0x36CE8 / resume(code=5)@0x37B2C 只做关句柄/重绘消息，
  **与返回重载无关——重载是纯冷启动路径**，印证平台“子应用退出 →
  mr_restart_old_app → 全新 VM”的真机语义。

### 修复（已实施）

- `mythroad.c` `mr_set_start_fileparameter()` 改为整块 `MEMCPY(128)`；
- `arm_ext_executor.c` 新增 `alloc_start_parameter_table_slot()`（memcpy 128
  播种 table[138]，不再 snprintf）与 `arm_ext_sync_start_parameter_slot()`
  （memcpy 128 回宿主，不补 NUL）；
- 其余文件名槽（table[100..103]）保持字符串语义不变。

### 修复后的实测（产物 /tmp/skyengine-e2e-FzVoYp）

- 返回路径日志出现 `mr_open(mythroad/fm.sav,1)`（读）→ `mr_remove(fm.sav)` →
  末尾 `mr_findStart(mythroad/)` 重开根目录：文件管理器真实重载，
  “正在打开”由 Cookie 自绘，`isOpeningFolderFrame`/无“正在启动”断言通过。
- 剩余差异（diffPixelCount=7805）：wbrw 运行时在根目录创建了自己的数据目录
  `brw/`，重载后的列表多出一项且按序号(6)恢复选中位置落在 `disk`。这是真机
  上同样会发生的应用行为，不是模拟器缺陷。处理：测试 workspace 预创建
  `mythroad/brw`，使启动前后根目录列表一致（已改 `run-mrp.test.ts` 两个用例的
  准备步骤）；预创建后末项高亮行位移（8 项列表，末项高亮 y≈244-252），
  需要重定标测试中的像素坐标。

## 2026-07-16 Phase 3 目标用例验证通过

- 测试准备预创建 `mythroad/brw`（两个用例），使子应用 wbrw 运行时新建的根目录
  在启动前后一致；文件列表稳定为 8 项，`fm.sav` 保存/恢复的选中序号(7=末项
  冒泡浏览器)前后指向同一项。末项高亮随之下移，`file-manage-bottom` 断言坐标
  从 (100,216) 调整为 (100,248)。“正在打开”覆盖层是居中对话框，屏幕位置与
  列表内容无关，`isOpeningFolderFrame`/`isStartupOverlayFrame` 像素签名无需改动。
- 结果：
  ```text
  cmake --build build --target skyengine -j            passed
  pnpm vitest run ... -t 方式二                    1 passed (产物 /tmp/skyengine-e2e-tZHWPH)
  pnpm vitest run ... -t 方式一                    1 passed (产物 /tmp/skyengine-e2e-ZLESpM)
  ```
- 关键区别于旧方案：返回时日志出现 `mr_open(mythroad/fm.sav,1)` 读 →
  `mr_remove(fm.sav)` → `mr_findStart(mythroad/)` 重开根目录，文件列表**真实
  重新加载**，“正在打开”由 Cookie 自绘；宿主不再保存/回放任何截图或内存快照。

## 2026-07-16 Phase 4 全量兼容通过

```text
pnpm exec tsc --noEmit                          passed
ctest --test-dir build --output-on-failure      1/1 (skyengine-unit) passed
pnpm exec vitest run test/e2e                    19 files, 32 tests passed (308.50s, 无 xvfb)
```

覆盖 Cookie 两种启动路径、DOTA/GGHJT/Optwar/Talkcat 的下载/退出/返回重进/付费
超时/取消循环、Gzwdzjs/GTCM/GXDZC、wbrw 输入与 golden 全图检查。pause/resume
Lua 优先顺序与真机冷重启 handoff 未引入回归。
