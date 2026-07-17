---
name: wxmdld-wrapped-lg-mem-first-fit
description: SOLVED — WXMDLD 99% crash needed both nested table[125] owner recovery and 32-bit wrapped LG_mem link decoding
metadata:
  type: project
---

WXMDLD 的 99% 崩溃有两个串联根因。第一，shared wrapper thunk 让 table bridge
的直接 LR 看不到 nested caller；`arm_ext_resource_owner_for_lr()` 只在 direct LR
属于 wrapper 且 direct owner 失败时扫描最多 16 个栈 word，候选必须是 Thumb、
属于已登记 child，且返回地址前为 `BLX Rm`。不要退回 active/foreground 猜 owner。

第二，`flaengine.ext` 依赖 Mythroad first-fit 的地址预测，调用 `table[125]` 后会
丢弃返回指针。运行值 `LG_mem_base=0x225E5C`、`next=0xFFFDA360` 在 32 位加法中
回绕为合法且已映射的低地址节点 `0x2001BC`。原生 `mem.c` 的
`LG_mem_base + previous->next` 接受它；host 旧 guard 在加法前以
`next_off > end-base` 拒绝并退 bump，导致后续预测 buffer 未填充、parser
`+0x48` count 为 0，最后缺少 `hash31("pet")=0x1B11F`。`pet` 是症状，禁止补 key、
修改 lookup 或按应用回收 table[125]。

通用修复是先按 `uint32_t` 计算 decoded address，再验证 `<= end`、完整映射、
非零长度、不溢出和 executable overlap；malloc 节点可低于 base，但 `mr_free`
的 `p < base` 拒绝必须保留。node traversal cap 要统计 end 以下全部映射 band。

最终全新运行树生成 219848-byte `avm_cache`，SHA-256
`d9d91908442aad3e766ba357bfa5f1a430a5bb9baf691a2db5b054d1f75d572f`；冷、暖
启动均进入 240x320 标题且无 Unicorn 错误。标题 PPM hash `098e2b47...5536cb`；
DOWN 改变菜单区 1138 pixels，UP 后 hash 精确恢复。详见
`omx_wiki/wxmdld-wrapped-lg-mem-first-fit-fix.md` 和
`docs/修复记录/wxmdld-99-percent-crash-debug.md`。

默认 `build/` work-dir 曾保留旧崩溃生成的同长度坏 SKY：hash
`34664c5a...e8792`、图像区大面积为 0，导致无 Unicorn 错误但只有 8 色/33
非黑像素。隔离验证只删除该生成物、保留 avm/exdet/其它状态即可让应用正常重建
`2d2708d8...9565`。这是调试工作区遗留物，不要为它增加应用名/路径/hash 特判或
全局清缓存；新冷启动已证明当前代码会直接生成完整 SKY。
