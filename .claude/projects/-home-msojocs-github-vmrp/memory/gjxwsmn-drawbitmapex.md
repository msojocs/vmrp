---
name: gjxwsmn-drawbitmapex
description: SOLVED - gjxwsmn scene background was missing because ARM table[121] DrawBitmapEx had no handler
metadata:
  type: project
---

`gjxwsmn.mrp` 的音乐页和菜单正常、首场景只剩文字/少量角色时，SDL present
不是根因。失败日志在场景阶段丢弃 94 次 `table[121]` 调用；Mythroad 原生表将
121 定义为 `_DrawBitmapEx`。`game.ext+0x1BA38` 包装函数在
`+0x1BAF2` 读取 `255+193+36=484=121*4`，`+0x1BAFA` BLX。它传两个
12-byte guest bitmap descriptor `{u32 p,u16 w,h,x,y}`、10-byte matrix
`{i16 A,B,C,D,u16 rop}`、w/h 和透明色。矩阵既有 identity，也有旋转/翻转。

通用修复位于 `src/arm_ext/aex_table.c`：新增 slot 121 handler，按 ARM ABI 偏移
逐字段解码，验证描述符、完整缓冲、源矩形与非零行列式，再通过
`mr_drawBitmapExHost()` 构造 host descriptor 并复用唯一的 `_DrawBitmapEx`
算法。主 framebuffer 继续记录 damage/foreground ownership，离屏目标仅更新
guest 缓冲。禁止把 32-bit guest descriptor 强转成 64-bit host struct，也不要
用普通 copy blit 代替 transform。

同包第二个 3,549-byte `start.mr` 虽含 `hsman=="sdk"` 门禁，但运行只读取首个
2,592-byte `start.mr`。`opense/sdk` A/B 的缺图完全相同，而 `sdk` 会改变网络
POST 和授权输入，所以不要为此把全局设备身份伪装成 `sdk`。

最终 `opense` PPM `/tmp/skyengine-e2e-aJLLb0/first-scene.ppm`：240x320、
64 色、62,522 非黑像素，`(174,42)=[216,228,232]`，目视为完整雪山场景。
目标用例通过；CTest 1/1、全量 E2E 30 files / 53 tests（`--retry=0`）全过。
详细证据见 `docs/修复记录/gjxwsmn/gjxwsmn-render-progress.md`。
