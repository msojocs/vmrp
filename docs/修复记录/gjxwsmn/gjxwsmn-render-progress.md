# gjxwsmn 游戏界面渲染缺失修复进度

## 目标与约束

- 目标用例：`pnpm vitest run test/e2e/gjxwsmn/boot-to-menu.test.ts`。
- 修复后必须渲染真实的游戏菜单与场景，不能绕过像素断言。
- 使用 SDL dummy 驱动，不使用 xvfb；避免全量 ARM trace。
- 必须根据 MRP 字节码与 ARM EXT 反汇编定位根因。
- 产品逻辑不得按应用名、包名、测试像素或场景硬编码，也不得增加失败兜底路径。
- 行为修改必须带说明原因的注释；最后回归其它用例。

## 2026-07-21 当前工作树审计

- 当前 fixture `test/fixtures/gjxwsmn.mrp` 为 403,772 字节，SHA-256 为
  `413eebf0b3e3d96562a0110e8756d79eb4260d8f77a9fec4a4060f6054bd765c`。
- 工作树原有 `gjxwszy` 进度文档把第二个 `start.mr` 单独当成启动入口并提出
  `opense -> sdk` 候选修改；本轮重新审计实际装载链与 A/B 结果后已移除。

## 2026-07-21 当前包 start.mr 反汇编纠偏

- MRP 索引从 `0xF0` 开始，包内有两个同名 `start.mr`。首项 packed 数据位于
  包偏移 `0x4ADF`，gzip 长度 `0x418`；第 8 项位于 `0x27858`，gzip 长度
  `0x58D`。两者解压后都是 `version=0x80,endian=1` 的 MRP bytecode。
- 首项根 proto 位于解压流 `0x0006`，有 43 条指令和 5 个子 proto。
- PC 17..19 调用 `GetSysInfo()` 并把结果保存为全局 `sysinfo`；PC 20..23 随即
  调用 `_mr_c_load()` 并检查返回值。根 proto 没有 `hsman`、`hstype`、`sdk`、
  `sdk_key.dat` 或 `cann\`t find sdk key!` 常量。
- 第二个 `start.mr` 解压为 3,549 字节、根 proto 132 条指令，确实包含
  `hsman == "sdk"` 与 `sdk_key.dat` 门禁，但详细运行日志只读取首项的 2,592
  字节，未读取第二项的 3,549 字节。
- 独立构建的 `opense` 与 `sdk` 二进制都能到达相同的音乐页、菜单和缺图场景；
  前三张 PPM hash 完全相同。`sdk` 反而把网络 POST Content-Length 从 559 改为
  553，证明身份字段进入线上协议。故恢复稳定的 `opense` 身份；该全局 ABI
  改动不是本缺图故障的修复。

## 2026-07-21 当前候选构建的 PPM 基线

- `cmake --build build --target skyengine --parallel` 成功；二进制 SHA-256 为
  `64ee1ec417a2fd908d3187c07c892305a89cc650f621605bd09525cbba17ee7d`。
- `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gjxwsmn/boot-to-menu.test.ts
  --retry=0` 失败，产物目录 `/tmp/skyengine-e2e-3S9rkf`。
- `bgm-select.ppm` 与 `menu.ppm` 的关键像素全部通过。`first-scene.ppm` 为
  240x320、27 色、8,302 个非黑像素；`(173,156)=[248,220,144]` 通过，
  `(174,42)=[0,0,0]` 失败。目视只有标题、对话框和少量角色，背景图层缺失。
- 场景期间 stdout 高频出现 `table[121] not implemented`；原生函数表明确把
  slot 121 定义为 `_DrawBitmapEx`。参数 `r0/r1` 稳定为两个相邻 guest 结构体
  地址，`r2/r3` 反复为 2、16、32 等贴图块尺寸。这是当前最强根因候选；下一步
  通过 ARM 调用点反汇编恢复结构与矩阵实参，再实现通用 bridge handler。

## 2026-07-21 DrawBitmapEx 反汇编与修复

- `graphics.ext` 解压后 8,148 字节，`game.ext` 解压后 174,452 字节。
- `game.ext+0x1BA38` 是 BitmapDraw 包装函数。`+0x1BA98` 与 `+0x1BAE8`
  分别把表基址加 255 和 193，`+0x1BAF2` 再取偏移 36，合计
  `255+193+36=484=121*4`；`+0x1BAFA` 执行 `BLX`。
- 包装函数把 `BitmapDraw(di,dx,dy,si,sx,sy,w,h,A,B,C,D,rop)` 转成
  `DrawBitmapEx(src,dst,w,h,matrix,transcolor)`。两个位图描述符各 12 字节：
  `{u32 p,u16 w,u16 h,u16 x,u16 y}`；矩阵 10 字节：
  `{i16 A,i16 B,i16 C,i16 D,u16 rop}`。运行时两个描述符地址相差正好 12 字节。
- 失败基线共丢弃 94 次调用：65 次 16x16、21 次 2x16、4 次 16x2、2 次
  32x32、2 次 2x2。反汇编同时证明既有 identity 矩阵，也有负数/交换系数的
  旋转与翻转矩阵，不能用普通 copy blit 代替。
- 新增通用 table[121] handler：显式解码 32 位 guest 结构，验证描述符、矩阵、
  完整源/目标缓冲与非零行列式，再通过标量 host bridge 构造原生结构并调用唯一
  的 `_DrawBitmapEx` 算法。目标是主 framebuffer 时沿用快照 diff、damage 与
  前台层归属跟踪；离屏目标只更新其自身缓冲。实现无应用/包名/像素硬编码，也无
  失败兜底路径。

## 2026-07-21 目标与兼容验证

- 恢复 `opense` 后重新构建成功，目标用例通过：1 文件、1 用例，耗时 11.82
  秒。保留目录 `/tmp/skyengine-e2e-aJLLb0`。
- `first-scene.ppm` 为 240x320、64 色、62,522 个非黑像素，SHA-256
  `d8c0f62f796905a2d3e58dda39c60c987a173457d8f1642cef1199f2353f60c0`。
  `(173,156)=[248,220,144]`，原失败点 `(174,42)=[216,228,232]`。
- 与失败基线相比，场景从 27 色/约 8.3k 非黑像素提升到 64 色/62.5k 非黑像素，
  背景覆盖到目标上方区域；stdout 不再出现 `table[121] not implemented`，也无
  Unicorn 执行错误。转换后的 PNG 目视为完整雪山、树木、道路、对话框与角色，
  不是只满足单个像素的空白/色块画面。
- `ctest --test-dir build --output-on-failure`：1/1 通过。
- `pnpm vitest run test/e2e --retry=0 --reporter=verbose`：30 个测试文件、53 个
  用例全部通过，耗时 314.52 秒。覆盖 golden 全图、普通/仿射位图、横屏、前台
  子模块、插件下载/返回、付费超时、网络、输入和冷暖启动；无重试掩盖失败。
- 最终工作树重建后再次以 `--retry=0` 运行目标用例：1/1 通过，耗时
  11.64 秒，产物目录 `/tmp/skyengine-e2e-dKaznk`。`first-scene.ppm` 的
  SHA-256 仍为 `d8c0f62f796905a2d3e58dda39c60c987a173457d8f1642cef1199f2353f60c0`，
  且日志中无 table[121] fallback 或 Unicorn 错误。
