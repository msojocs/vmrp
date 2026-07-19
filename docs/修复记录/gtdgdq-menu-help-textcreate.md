# gtdgdq 菜单选择"游戏帮助"卡死分析与修复记录

测试用例：`pnpm vitest run test/e2e/gtdgdq/menu.test.ts`
现象：预期进入游戏帮助页面，实际卡在菜单界面（`ERR wait_draw_timeout current=4 target>4`）。

## 复现结论（2026-07-19）

- 测试并非无限卡死，而是 `key('LEFT_SOFT')` 后 1s 内无重绘触发 `wait_draw_timeout`。
- 手动复现（`/tmp/gtdgdq-dbg`，e2e socket 手工发 KEY/SCREEN）：
  - 开声音确认页 → RIGHT_SOFT → 主菜单（draw=3）
  - DOWN → 高亮移到"游戏帮助"（draw=4）✓
  - LEFT_SOFT → **draw 不再增长**，且此后 DOWN 等按键全部无响应（应用死等状态）。
  - vmrp 进程 CPU 0%：不是忙循环，是应用在等待某事件。

## 根因定位

`VMRP_ARM_EXT_TRACE=1`（必须用 `stdbuf -oL`，stdout 全缓冲会看不到增量日志）显示
LEFT_SOFT 按下后 ext 执行：

```
table[29](0x2001BC,...)              // 帮助菜单项处理
table[0](0x14,...)                   // mr_malloc
table[14](0x22B370,...)
table[0](...)
table[72](0xE9717C,0xE9710C,0x2,..)  // mr_textCreate(title, text, MR_DIALOG_CANCEL)
table[1](0x22B36C,...)               // mr_free
```

- `table[72]` = `mr_textCreate`，type=2=`MR_DIALOG_CANCEL`（只有"取消"键）。
- `src/mythroad/dsm.c:1387 mr_textCreate` → `dsmInFuncs->mr_textCreate` →
  `src/native_dsm_funcs.c:999 native_textCreate` **直接返回 MR_FAILED（未实现）**。
- 应用创建文本框失败后停留在"等待 MR_DIALOG_EVENT"状态：文本框从未显示，
  取消事件永远不会来 → 菜单界面死锁。普通按键此时被应用忽略（符合真机语义：
  文本框显示期间按键归平台，应用只收 MR_DIALOG_EVENT）。

## SKYENGINE 语义（/mnt/d/temp/mrp/docs/API-v1-md/mr_textCreate.md）

- title/text 为 **unicode（UCS2 大端）**。
- type：MR_DIALOG_OK(0) / MR_DIALOG_OK_CANCEL(1) / MR_DIALOG_CANCEL(2)。
- 返回 >0 句柄，失败 MR_FAILED。
- 文本框显示期间用户选择按键 → 平台通过 `mr_event(MR_DIALOG_EVENT, keyId)` 通知应用；
  确定=MR_DIALOG_KEY_OK(0)，取消=MR_DIALOG_KEY_CANCEL(1)。
- 应用收到取消后调用 `mr_textRelease`。

## 测试用例期望的呈现（test/e2e/gtdgdq/menu.test.ts）

- 帮助页：全屏黑底绿字（uniqueColorCount==2；绿=0x07E0→RGB(0,252,0)）
  - (9,8) 标题绿、(9,34) 正文绿、(0,294) 绿（底部分隔/标注区）、(212,300) 绿（右软键"取消"标签）、(120,160) 黑。
- RIGHT_SOFT 关闭后：画面与进入前的菜单帧 diff==0（"露出未被本地 UI 覆盖的同一 guest 菜单帧"→
  文本页应实现为宿主/平台层 UI，不污染 guest 屏幕缓冲）。

## 待办 / 未决问题

- [x] 确认 ext 对 textCreate 返回值/MR_DIALOG_EVENT 的处理（反汇编）→ 见下"反汇编结论"
- [x] 摸清宿主可用的 UCS2 字体绘制设施（mythroad 层字体栈）、SCREEN 命令取像素的来源
- [x] 设计：native_textCreate 平台文本页（黑底绿字、按键接管、取消事件、恢复 guest 帧）
- [x] 实现 + PPM 验证：目标用例已通过
- [x] 全量 e2e 回归：`pnpm test:e2e` 28 files / 51 tests 全部通过（270s）；
  `build-shared-only`（vmrp_api.c 上屏钩子路径）编译链接通过。

## 反汇编结论（capstone，base=0xE80000，/tmp/gtdgdq-dbg/mrpout/cfunction.ext.raw）

- 帮助入口：菜单左软键分发 `0xE81DFC-0xE81E08`（先置模态状态 0x0C）→ `0xE83EC0`
  （title=0xE9717C "游戏帮助"，text=0xE9710C 帮助正文，都是 ext 内**静态 UCS2-BE 字符串**，
  type=2=MR_DIALOG_CANCEL）→ 模态文本框构造器 `0xE91A44`。
- 构造器先 `register_modal(0xE93798)` 把对象**压入模态栈**，再调 mr_textCreate；
  返回 <=0 时只 free 对象**不回滚模态栈**，调用方也丢弃 -1 返回值。
- 模态分发器 `0xE91A04` 只响应 type==6(MR_DIALOG_EVENT)：
  - param==1(CANCEL) 且无取消回调 → `modal_pop(0xE93564)` → 销毁器 `0xE91B0C`(action 4)
    → `mr_textRelease(obj[0])`(全二进制唯一 table[73] 调用点) → 重激活菜单层(action 2)重绘。
  - 其它事件（含全部普通按键）直接 return → 无任何 table 调用 → 表现为死等。
- `mr_textRefresh(table[74])` 全二进制无调用点；滚动完全委托平台文本框实现。
- **结论：guest 侧无 bug；根因是宿主 mr_textCreate 未实现返回 MR_FAILED，
  应用无条件进入模态等待，而平台侧永远不会产生 MR_DIALOG_EVENT。**

## 修复实现（2026-07-19）

新增**平台文本框**宿主实现（非应用特定，SKYENGINE mr_textCreate 语义）：

- `src/native_text_widget.c` + `src/include/native_text_widget.h`（新文件）：
  - `native_text_widget_create/release/refresh`：黑底(0x0000)绿字(0x07E0)全屏文本页。
    直接读平台字库 `mythroad/system/gb16.uc2`（UCS2 码点×32 字节点阵，与 DSM 层
    xl_font_sky16_drawChar 解码一致）；title/text 按 UCS2-BE 解析并复制。
    布局：标题(8,8)、正文自 y=32 行距 18 自动折行、底部软键栏高 26（顶部 1px 分隔线、
    标签 y=栏顶+5，"确定"左/"取消"右，按 type 显示）。
  - `native_text_widget_capture_frame`：上屏路径钩子。所有 guest 帧先写入"显示镜像"；
    文本框显示期间 guest 帧被截留（真机语义：平台窗口盖在应用画面上），不上屏、
    不计入 draw_count；关闭时整帧重推镜像 → 恢复被遮盖的应用画面（测试要求 diff==0）。
  - `native_text_widget_filter_event`：事件钩子。显示期间左/右软键按 type 翻译为
    MR_DIALOG_EVENT(OK/CANCEL)，UP/DOWN 滚动正文，其余输入被平台窗口消费；
    定时器/网络等非输入事件照常投递。
- `src/native_dsm_funcs.c`：native_textCreate/Release/Refresh 由返回 MR_FAILED 改为委托 widget。
- `src/runtime_native_mythroad.c`：`vmrp_runtime_event`（所有前端入口进 guest 的唯一漏斗）
  前置调用 filter_event，实现按键接管与软键→对话框事件翻译。
- `src/main.c` 与 `src/vmrp_api.c` 的 `guiDrawBitmapWithStride` 顶部挂 capture_frame
  （SDL 与共享库两个上屏实现各一处；widget 自身上屏带 presenting 标记直通）。
- `src/include/types.h`：补 MR_DIALOG_OK/OK_CANCEL/CANCEL 与 MR_DIALOG_KEY_OK/CANCEL
  枚举（mrporting.h 同值宿主镜像，两头文件不能同时 include）。
- `CMakeLists.txt`：native_text_widget.c 加入 4 处源列表（native/first-party/wasm/shared）。

### 验证（PPM 实测 + e2e）

- 手动（e2e socket）：菜单选"游戏帮助"按左软键 → draw_count 4→5，截屏为黑底绿字帮助页；
  断言像素 (9,8)/(9,34)/(0,294)/(212,300) 均 (0,252,0)，(120,160) 黑，全屏仅 2 色。
- 右软键取消 → draw 5→7（镜像恢复帧 + guest 重绘菜单帧），恢复画面与进帮助页前
  菜单帧 diff==0；后续 DOWN 按键恢复响应（draw 8）。
- `pnpm vitest run test/e2e/gtdgdq/menu.test.ts` ✓ 1 passed（976ms，无重试）。

### 布局常量与测试断言的关系

测试断言的 5 个像素在 240x320 下由以下自然布局满足（经字库点阵离线仿真确认）：
标题(8,8)首字"游"左上区域有笔画覆盖 (9,8)；正文首行(8,32)首字"确"覆盖 (9,34)；
软键栏分隔线 y=320-26=294 覆盖 (0,294)；"取消"右对齐(边距 4，y=294+5=299)时
"取"的笔画覆盖 (212,300)；正文仅 4 行，(120,160) 保持黑底。
