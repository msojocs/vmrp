# GTXZJ 启动黑屏修复进度

日期：2026-07-17

## 目标与约束

- 复现命令：`build/skyengine build/mythroad/gtxzj.mrp`。
- 目标：游戏正常启动并进入可交互画面；不得按包名、哈希或场景添加特判。
- 运行使用现有 `DISPLAY=:0`/WSLg 图形会话，不使用 Xvfb。
- trace 日志必须保持有界；画面以 PPM 的结构、像素和交互共同验证。

## 包与二进制身份

- `gtxzj.mrp` SHA-256：
  `65f41d75b54b66d9fe747c7df0b50b5d2808ff71c08bdbf4a860989d976773da`，
  文件长度 361950 字节，包头声明 240x320。
- 从跟踪 fixture 直接解压得到 `cfunction.ext` 16940 字节、SHA-256
  `5d4a25bb9aca85d4638a43acaefd1aa80267654494170b66c0b024441bbbc69c`；
  `game.ext` 101136 字节、SHA-256
  `4582afaf47b2fed745b2c75647c55dd72ff4994529b3a979ab1e34f7b273d3ab`。
  两个身份无需依赖临时文件即可复验：

  ```bash
  dd if=test/fixtures/gtxzj.mrp bs=1 skip=336656 count=11678 status=none \
    | gzip -dc | sha256sum
  dd if=test/fixtures/gtxzj.mrp bs=1 skip=273132 count=63502 status=none \
    | gzip -dc | sha256sum
  ```

  对应 entry 偏移/压缩长度为 `0x52310/0x2D9E` 和 `0x42AEC/0xF80E`。
- 当前修复构建 `build/skyengine` build-id：
  `fe38a6d32e0a1c4ba78310c0ae2adce903a51d6a`。
- 该包不是提交 `b9ecee2` 已修复并加入回归的 `sanguo_490111.mrp`；后者
  SHA-256 为 `bcbac32a...`。旧包的动态 LG_mem arena 结论和非黑 PPM
  不能作为本包已经修复的证据。

## 基线运行证据

- 当前二进制成功解压并加载 `mrc_loader.ext`（232 字节）、
  `cfunction.ext`（16940 字节）和 `game.ext`（101136 字节）。
- wrapper 离线/运行时指纹一致：timer dispatch 文件偏移 `0x3A54`，
  compact scheduler offset `0x1FC`，compact heap control offset `0x14C`，
  compact free return 文件偏移 `0x2644`，没有 chain walker。
- `start.mr` 的同步启动调用已经返回，日志到达：
  `skyengine_runtime_start_dsm returned 0x0` 和 `After app init`。因此当前黑屏
  不在旧 `490111` 卡住的同步 wrapper teardown，也不是 Lua 入口未返回。
- 运行 15 秒没有一次有效 screen draw，设置 `VMRP_PPM=1` 仍无自动 PPM。
  存活时向进程发送 `SIGUSR1` 后得到 `/tmp/gtxzj-live.ppm`：P6 240x320，
  230415 字节，SHA-256
  `1edcc2a5a6f7f596acf98d8fbd248c4175886dc8759e6d75ec652b339c2a53b6`；
  230400 字节 RGB payload 全为零。
- 黑屏稳定后进程不是忙循环：主线程睡眠在 SDL `poll`，SDL timer 线程睡眠
  在 futex，总 CPU 约 0.5%。这把直接调查边界缩小到启动完成后的
  timer/event 注册、owner 路由或 guest 初始化提前返回。

## 正常包对照与回归边界

- GTXZJ 与可正常运行的 GFHCQ 使用同族的 `start.mr`、`mrc_loader.ext`、
  `cfunction.ext` 和 `verdload.ext`；主要差异是 `game.ext`。GFHCQ 在约 1 秒、
  2 秒时分别累计约 17、33 次 draw，并生成有 996 种颜色、5137 个非黑像素的
  PPM；GTXZJ 的 timer 和 draw 始终为 0。
- GFHCQ 的 primary sync 之后还会继续读包、同步 child EXT，并在 helper code 0
  内启动 timer；GTXZJ 在 primary sync 后直接返回，没有后续 bridge activity。
- 使用提交 `b9ecee2` 之前的 `651ba7c` 构建运行 GTXZJ 仍然全黑。因此这不是
  动态 LG_mem arena 修复引入的回归，不能通过撤销该修复解决。
- `--memory 1M/2M/4M/8M` 的结果完全一致；ENTER、LEFT_SOFT 输入也不能触发
  draw，排除了内存档位不足和仅缺少首次按键两种解释。

## Primary helper code 0 调用链复核

- wrapper 私有 loader 先 BLX child entry，取得并发布 P/RW/helper；随后只向
  child helper 发送 setup code 6、8、10，没有发送 code 0。
- GTXZJ child helper 运行时地址为 `0x238099`（`game.ext` 文件偏移
  `0x11D91`）。其 code 0 分支依次调用 `0x2269D0`、`0x237F08`，然后进入
  游戏 `mrc_init`（`0x22C124`）。
- 仅检查私有 loader 会漏掉它的 caller。wrapper outer code 0 在私有 loader 返回
  后调用 `0xE82E88`；该函数在 `cfunction.ext+0x2EA4` 取 child P，`+0x2EA6`
  取 child helper，`+0x2EAA` 执行 `movs r1,#0`，并在 `+0x2EAC` 执行
  `blx helper`。因此 committed 基线已经调用 child code 0 和 `mrc_init`。
- 把 private primary 的 `primary_host_init_pending` 从 0 改为 1 会在 wrapper 返回后
  再次调用同一个 child code 0。运行日志确认实验构建出现 wrapper 内部调用和宿主
  pending 调用两次初始化；第二次仍无 timer/draw。该实验已从主工作树撤销，不能
  作为修复。

## GTXZJ 日期门禁

- GTXZJ `mrc_init` 入口 `0x22C124` 首先调用 `0x23A5B8`，比较返回值与 1；
  不等于 1 时跳到 `0x22C16E` 直接返回，尚未注册 timer 或执行任何绘制。
- `0x23A5B8`（`game.ext` 文件偏移 `0x142B0`）调 `0x23A1E0` 获取并展开
  `year/month/day/hour/minute/second`。2012-06-20 及之前确定返回 1；
  2012-06-21 至 06-24 还会用时间播种 PRNG，并把 `abs(rand) % 100` 与
  `20 * (day - 20)` 比较，因而通过概率依次约为 79%、59%、39%、19%；
  2012-06-25 起以及 2012 年之后确定返回 0。
- 修复前宿主 `src/utils.c` 使用本地系统时间，年份为 `tm_year + 1900`、月份为
  `tm_mon + 1`。当前日期 2026-07-17 必然使门禁返回 0，这解释了基线没有
  timer、draw 和 PPM。
- ABI 链已逐级确认：`0x23A1E0` 调 `0x237C0C`，后者取 function table 字节偏移
  `0x88`，即 slot 34；源码中 table[34] 为 `asm_mr_getDatetime`，最终转发到
  `utils.c:getDatetime`。`mr_datetime` 的 year 为完整年份，宿主原字段换算本身
  没有 ABI 错误。
- MRP header 只有 appid、version、vendor、description 和 reserved 字段，没有正式
  构建日期。最终方案没有从包元数据猜日期，也没有按 GTXZJ 包名、哈希、截止日
  特判或在失败后重试。

## 隔离日期验证与字库分叉

- 隔离构建只把设备日期设为 2012-06-20，保持 committed 单次 child code 0。
  watch 依次命中 `mrc_init 0x22C124`、日期 guard、成功分支 `0x22C130`；随后加载
  `arrow.bmp/rect*.bmp/talk5.bmp`，注册 80 ms timer，并持续 full-frame present。
- 首阶段 PPM 正常显示“我要冒泡”和 Moshi Logo。约 6 秒后进入状态索引 10，
  对应 `0x23C8F8` 的“是否开启音效？”文本菜单；若工作目录没有基础字库，
  `mr_open(mythroad/system/gb12.uc2/gb16.uc2)` 都返回 0，该文本菜单会再次全黑。
- `build/skyengine` 默认以可执行文件目录 `build/` 为 work-dir，但修复前
  `build/mythroad/system` 只有字体安装 MRP，没有解包后的 `.uc2`。因此用户原命令
  还需要构建系统部署基础字库，不能只修 RTC。

## 最终通用修复

- 新增设备 RTC 配置：默认日期为 2011-01-01，适合旧 MRP 的确定性回放；
  时分秒仍随宿主时钟。CLI `--device-date YYYY-MM-DD|host` 和环境变量
  `SKYENGINE_DEVICE_DATE` 可选择任意合法日期或恢复真实宿主日期。
- `getDatetime` 只读取公共 `SkyEngineConfig`，没有任何应用身份或失败状态分支。
  日期解析校验固定格式、月份天数和闰年，非法日期在启动前明确失败。
- 共享库通过 `skyengine_api_set_device_date("YYYY-MM-DD"|"host")` 复用同一校验并在
  `skyengine_api_start()` 传播配置，非法调用不会覆盖上一次有效模式。
- CMake 的 `skyengine-system-assets` 依赖在每次请求构建 `skyengine` 时使用
  `copy_if_different` 把 `gb12.uc2`、`gb16.uc2` 复制到
  `$<TARGET_FILE_DIR:skyengine>/mythroad/system`，无需重链接即可恢复缺失或过期字库。
- 新增 `test/fixtures/gtxzj.mrp` 和 `test/e2e/gtxzj/boot-to-title.test.ts`。测试等待
  稳定像素而不是固定启动补调用，覆盖音效菜单、RIGHT_SOFT、标题画面、DOWN 选择
  和 ENTER 打开“无存档”对话框。
- 新增 `test/e2e/gtxzj/device-date.test.ts`，自动覆盖六种非法日历输入、
  `SKYENGINE_DEVICE_DATE`、CLI 覆盖环境变量和 `host` 模式，共 9 个日期契约用例。

## 最终目标验证

- 真实 `DISPLAY=:0` 下按原命令运行，6 秒 SIGUSR1 截图
  `/tmp/gtxzj-final-real.ppm` 为 P6 240x320、230415 字节，SHA-256
  `3286614596ea0f61c9d59a119e8ffec1ba754fc1678c8e81192111b7e48aa1b0`；
  RGB payload 有 2406 个非零字节、3 种字节值，内容为音效选择菜单。
- 真实显示交互产物在 `/tmp/gtxzj-final-real-e2e-ULrgCl/`：sound menu、title、
  load-selected、no-save-dialog 四帧哈希各不相同；DOWN 前后有 1506 个 PPM 字节
  变化，ENTER 前后有 57656 个字节变化，`DRAW_COUNT=218`，最后 `QUIT` 干净退出。
- 新增 Vitest 在 SDL dummy 下独立复验同一工作流，耗时约 7.7 秒并通过；真实显示
  验证未使用 Xvfb。
- 最终 build-id `fe38a6d32e0a1c4ba78310c0ae2adce903a51d6a` 再次在 WSLg
  `DISPLAY=:0` 下按相同二进制参数启动；环境 transcript 记录
  `WSL2_GUI_APPS_ENABLED=1`、`WAYLAND_DISPLAY=wayland-0`、`DISPLAY=:0`，且
  `/tmp/.X11-unix/X0` 存在、没有 Xvfb 进程。依次执行 `RIGHT_SOFT`、`DOWN`、
  `ENTER` 后四帧 SHA-256 为：

  ```text
  sound:  3286614596ea0f61c9d59a119e8ffec1ba754fc1678c8e81192111b7e48aa1b0
  title:  000e9612464ce4cb6f0935265b62dba423afeeb3b3ffc6ddf6cd7784d4602246
  load:   9a56df148f398760ff7ac61c0a7bb3845851f0fffb2552a73a10e6e5d1acc290
  dialog: 2610ebf7f4471dff91780f2fcd5025713b71424fdf5a120d8c7c153cc8c29a20
  ```

  标题到加载选择全屏变化 520 像素、底部菜单区域变化 301 像素，加载选择到对话框
  变化 19426 像素；`DRAW_COUNT=1289`，`QUIT` 返回 OK，进程以状态 0 退出。
  `/tmp/gtxzj-final-current-transcript.log` SHA-256 为
  `ca153d758d4af83538709d21eee1ae8a58c3eb66a32fc998dc5b09c891c82a8e`。

## 兼容回归与最终状态

已完成定向兼容回归：

- GFHCQ：最终 build-id 定向 1/1 通过，draw-count 为
  `116,274,440,571,680,795,892`，首尾变化 347189 像素；保留日志 SHA-256
  `1b60cbba0819a36193a67010cb31b4041b362ccbff9ed368ef4095cf04406967`。
- Talkcat：启动、资源包下载安装、循环取消 3/3 通过。
- Sanguo：加载、网络失败提示、ENTER 退出 1/1 通过。
- GTCM：主界面与付费界面 2/2 通过。
- Golden：`gzwdzjs-bgm-dialog`、`gxdzc-boot` 两张全图逐像素一致。
- GGHJT：正式开始不花屏、直接开始不缺渲染 2/2 通过。
- OPGLQA：字体下载/切换后的顶部渲染 1/1 通过。

- `cmake --build build --target skyengine -j2` 和 `pnpm exec tsc --noEmit` 均通过。
- 删除已部署的 `build/mythroad/system/gb12.uc2` 后再次增量构建，没有重链接也会
  执行 `skyengine-system-assets` 并恢复相同 SHA-256 的字库。
- 使用 `-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/tmp/skyengine-runtime-dir-audit` 的独立配置
  验证 link 输出和两份字库都位于该目录，构建目录本身没有误放的 `mythroad/`。
- `SKYENGINE_BUILD_SHARED_ONLY=ON` 的 `skyengine-shared` 构建通过；公共日期 API 对
  `host`、合法闰日和多种非法日期返回值符合契约。以独立共享库进程启动 GTXZJ，
  `2011-01-01` 在 7 秒后有 1134 个非零 RGB565 像素，`host` 在当前日期 2 秒后
  仍为 0，证明配置确实穿过 `skyengine_api_start()` 到 table[34]；活动运行期间改为
  `host` 会返回 `-1`，原固定日期继续产生 1134 个非零像素。
- 第一次完整回归发现源码模板下残留了本次诊断生成且被 Git 忽略的
  `wasm/dist/fs/mythroad/gtxzj/`，它给 Cookie 文件管理器增加一个根目录项，导致
  两个用例的固定末行坐标失效。显式 `--device-date host` 结果不变，排除了 RTC
  回归；删除这个 2026-07-17 09:56 生成的诊断副产物后，原失败点恢复。
- 最终 build-id 下重新执行 `pnpm exec vitest run test/e2e --reporter=verbose`：
  22/22 测试文件、43/43 用例通过，耗时 304.05 秒；同一次运行中 GTXZJ 耗时
  7.18 秒，新增日期契约 9/9、Cookie 两种子应用返回路径也均通过。完整输出保留在
  `/tmp/gtxzj-final-full-e2e.log`，SHA-256 为
  `fb21f3191c2b9edf44bbc1f930d4b39026469a3b2d733644819ab075b06e0dc0`。
- 最终修复已写入 `omx_wiki/gtxzj-date-guard-and-default-runtime-assets.md`。
  主工作树没有 primary child 重复初始化实验、临时 framebuffer 探针或
  `wasm/dist/fs/mythroad/gtxzj/` 诊断运行数据。
