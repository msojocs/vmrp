---
name: wbrw-html2sky-line-wrap
description: WBRW br uses FlowID/0x4E while hr requires a visible 0x60 separator; retained PPM verifies both
metadata:
  type: project
---

WBRW 的自动折行、HTML 强制换行和水平分隔线是三套语义。同一段落中没有显式 break 的链接/文本应共享 `FlowID`，由客户端在同一 `0x4E` 行内按屏宽自动折行；`<br>` 后的内容必须获得新的 `FlowID`，使 `generateSkyDisplayList` 创建新的 `0x4E` 行。只 `flush()` 而不切换 flow 会吞掉 HTML break，例如 `mrp.gddhy.net` 的 `<a>mynes [顶]</a><br/><br/><a>RX管理器 [顶]</a>` 会挤在同一基线上。

`<hr>` 不能再与 `<br>` 共用分支。它必须保留为独立的 `ItemKindHorizontalRule`，绕过空文本过滤，并按 DOM 顺序直接发射 `parent=当前内容/样式容器`、`e2=14`、`op=0x60`、`payload=00`，不要外包合成 `0x4E` 文本行。当前固定 `skyStylePrelude` 的 element 14 是灰色分隔线样式；历史页面出现的 element 8 属于另一张样式表，数字不可跨表照搬。通用修复位于 `/home/msojocs/github/sky-engine-tools/html2sky/html2sky.go`。

不要把 `\n` 塞入 `Item.Text`，因为 `collapseText` 会将其压成空格。连续 `<br>` 会确保后续内容另起一行，但不会为无内容的 break 单独生成空白 spacer 行。

验证：`go test -count=1 ./...` 通过；热重载后聚焦运行 `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/temp.test.ts -t 'mrp.gddhy.net' --reporter=verbose`。最终 `/tmp/vmrp-e2e-dpBTag/loaded.ppm`（SHA-256 `403468815271d872cc2965d2c7758fcd8681c7516f42a389df83d8f7e9403731`）清楚显示 `mynes [顶]` 后换行，下一行的 `RX管理器 [顶]` 从左侧开始。详见 `omx_wiki/wbrw-html2sky-line-wrap-progress-2026-07-16.md`。

水平线最终证据：`/tmp/vmrp-e2e-yQ9666/loaded.ppm`（240x320，SHA-256 `2dfee34fc01fe11f5cb1069466c79c7744a7415a52f4f10b677df82d0fd31331`）在 `软件列表` 下方显示横跨内容区的浅灰线；`y=84` 和 `y=86` 各有 230 个 `[232,236,232]` 像素，旧图对应行各只有 2 个边框像素。

表单背景规则（2026-07-16）：`0x61` 的样式元素不能无条件写成 9。真机缓存 SKY 证明，普通顶层表单使用中性 element 3，只有处于 element-7 显式背景容器下的表单才使用 element 9。生成器现在根据 `ContainerID`/`ContainerElement`（来自实际 CSS）选择 3 或 9，不根据主机、URL、标题或表单名判断。最终 task 页面保留 32,394 个源 CSS 绿色像素和黄色页脚；gddhy 底部页面从 3,248 个错误绿色像素降为 0，两个表单仍完整显示。证据为 `/tmp/vmrp-e2e-1UrKUI/loaded.ppm` 与 `/tmp/vmrp-e2e-IyUwMO/loaded.ppm`。
