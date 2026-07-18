# LCD 旋转（横屏自动翻转）支持

## 需求

`build/vmrp build/mythroad/gtcm.mrp --screen 320x480`（真机竖屏面板规格）：
gtcm（SphinxJoy 引擎）启动时调 `mr_plat(101, MR_LCD_ROTATE_270)` 请求横屏。
要求 guest 请求旋转时 SDL 窗口自动翻转为 480x320 正确显示，并为 Flutter
嵌入端提供对接 API。

## 修改前行为（实证）

- plat(101) 仅回 ACK（dsm.c），无状态；
- aex_screen 转置拒绝保护以 `m->screen_w/h`（=面板 320x480）为基准，把
  gtcm 合法的横屏画布写入 (480,320) 当作「转置撤销请求」拒绝；
- 窗口固定 320x480，游戏按 480x320 布局绘制 → 右侧 160px 裁剪
  （PPM 实证：声音提示画面残缺）；
- 旧 e2e 用 `--screen 480x320` 预旋转窗口绕过。

## 显示模型（用户视角，无像素级旋转）

真机 plat(101,3) 后应用绘制转置画布，LCD 控制器旋转扫描，用户横握手机所
见 = 画布原样。模拟器只做尺寸簿记：奇数旋转（90°/270°）时显示尺寸 = 面板
尺寸转置；帧内容直接 1:1 呈现。鼠标/触摸坐标与画布 1:1，无需变换。

## 实现

状态由 **DSM 层持有**（真机上这是 LCD 驱动状态；且 `src/mythroad/*` 编译
时显式 `-UVMRP`（CMakeLists.txt:292），不能访问 `vmrp_config` —— 首版实现
误用 `#ifdef VMRP` 分支导致状态从未设置，已实证并纠正）：

1. `src/mythroad/dsm.c`
   - `dsmLcdRotation` 静态状态 + `dsm_get/set_lcd_rotation()`（dsm.h 导出）；
   - `mr_plat` case 101：param 0..3 记录状态返回 MR_SUCCESS（gtcm 反汇编
     证明非 0 返回会进错误分支黑屏），范围外 MR_IGNORE；
   - `dsm_init()` 归零（每次 runtime 启动）。
2. `src/vmrp.c` + `src/include/vmrp.h`
   - `vmrp_display_width/height()`：奇数旋转返回面板转置，否则面板尺寸。
     `vmrp_config.screen_width/height` 始终是 `--screen` 面板尺寸不变。
3. `src/arm_ext/aex_screen.c` + `aex_table.c`（`arm_ext_priv.h` 登记）
   - `arm_ext_apply_lcd_rotation(m)`：把 `m->screen_w/h/len` 与 ARM 可见
     `mr_screen_w/h`（table[92/93] 槽）更新为显示尺寸（真机 LCD 驱动行
     为）。画布字节数转置不变，无迁移；宿主 memcpy 不触发 unicorn 钩子。
   - `aex_t037`（plat 桥）：`code==101 && ret==MR_SUCCESS` 后调用之。
   - 此后既有转置拒绝/读恢复保护以旋转后的基准工作：gtcm 横屏画布
     (480,320)==基准被平凡采纳；付费框的 (320,480) 撤销写照常防护。
4. `src/main.c`（SDL 展示）
   - `guiDrawBitmapWithStride`：取 surface 前窗口尺寸 != 显示尺寸时
     `SDL_SetWindowSize`（VM 全在 SDL 主线程，安全）；裁剪按显示尺寸；
   - `guiDrawBitmap` wrapper 行宽 → `vmrp_display_width()`。
   - PPM dump / e2e 帧捕获 / SCREEN 命令取 surface->w/h，自动正确。
5. `src/vmrp_api.c` + `vmrp_api.h`（Flutter 共享库）
   - 裁剪/行宽按显示尺寸；`vmrp_api_get_screen_width/height` 返回显示尺寸；
   - 新导出 `vmrp_api_get_screen_rotation()`（0..3，轮询风格与
     get_screen_dirty 一致）；`vmrp_api_init` 归零旋转;
   - screen_buf/rgba 像素总数转置不变，无需重分配。
6. `src/utils.c` printScreen 调试转储按显示尺寸。
7. `docs/flutter-integration.md`：新增「屏幕旋转」小节 + Dart FFI 绑定与
   VmrpEngine 轮询示例。

## 发现的真机行为（gtcm 付费流程）

gtcm 进入 netpay 下载/付费界面前调 `plat(101,0)` **撤销旋转**（竖屏 UI，
用户竖握手机），退出后再 `plat(101,3)` 恢复横屏。旋转支持后付费界面以
320x480 竖屏完整呈现（旧行为是强制横屏布局）。e2e「付费界面修正」断言
更新为竖屏坐标（标题栏/底部软键栏深灰 40,40,40）。

## 验证

- [x] gtcm `--screen 320x480`：窗口自动翻转 480x320，声音提示/主界面/游戏
      完整横屏渲染（PPM 实证）；付费界面自动翻回 320x480 竖屏完整呈现
- [x] gtcm e2e 2/2 通过（screen 改为 320x480 真机规格,断言坐标横屏不变;
      付费界面断言更新为竖屏坐标）
- [x] 全量 e2e：24 文件 46 用例全部通过（其余用例不调 plat(101)，
      rotation==0 显示尺寸恒等面板尺寸，路径行为不变）
- [x] 共享库 `-DVMRP_BUILD_SHARED=ON` 构建通过，`vmrp_api_get_screen_rotation`
      已导出（nm 验证）

## 兼容性说明

- 旧用法 `--screen 480x320` 跑 gtcm：plat(101,3) 会将显示转置为 320x480
  （对已横置面板再旋转的字面语义，与真机一致）。推荐用法即 320x480。
- 复现脚本：`temp/repro-gtcm-pay.sh <outdir> [WxH]`（e2e 付费流程手动版）。
