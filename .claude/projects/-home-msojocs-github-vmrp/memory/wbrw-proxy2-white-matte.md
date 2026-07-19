---
name: wbrw-proxy2-white-matte
description: Proxy2 must flatten image alpha onto opaque white before resizing and GIF quantization for WBRW
metadata:
  type: project
---

WBRW 会把 GIF 透明调色板样本当作普通 RGB 绘制。PNG 不能直接缩放到透明黑 RGBA 后再量化到不透明调色板，否则透明画布会选择黑色，半透明边缘也可能变暗。

`/home/msojocs/github/sky-engine-tool/proxy2/image.go` 的通用规则：解码后检查 alpha；存在透明或半透明像素时，先创建不透明白色 RGBA 画布，用 `draw.Over` 合成源图，再进行缩放和 GIF 量化。白色是明确输出语义，不读取 URL、域名或图片名称；透明黑输入也必须输出不透明白底。完全不透明图像不额外复制。

最终证据：`/tmp/skyengine-e2e-0crfJT/loaded.ppm` 的 `(100,43)` 为 framebuffer white `[248,252,248]`，蓝/红主体和最右字形仍在；缓存 `/tmp/skyengine-ws-51gaPf/mythroad/brw/http/cache3/6620.gif` 四角为 `[255,255,255,255]`，透明像素和纯黑像素均为 0。详见 `omx_wiki/wbrw-proxy2-white-matte-progress-2026-07-16.md`。
