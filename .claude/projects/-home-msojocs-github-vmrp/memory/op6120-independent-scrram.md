---
name: op6120-independent-scrram
description: SOLVED - table[38]/MR_MALLOC_SCRRAM must use an independent guest mapping, not the LG_mem/main bump heap
metadata:
  type: project
---

OP6120 在 `--memory 2M/4M` 卡在 draw 13，1M 正常。实际 Flash 引擎是
`flaengine.ext`（SHA-256 `da955833...a6ca8b`）；其 `+0x6C4` 申请约 10 MiB
SCRRAM，table[38] code 1014 的首个请求为 `0xA00400`。1M 时旧
`arm_alloc()` 尚能在 `EXT_STACK_ADDR=0xE00000` 前容纳它；4M 时 origin pool
把 bump 顶推高约 3 MiB，请求跨过 wrapper stack/code 后失败。guest 每次减
`0x400`，要重试 8664 次才可能得到约 1.5 MiB，因此既长时间循环又容量不足。

根因是 host 内存所有权错误。真机 SCRRAM 是独立于 `LG_mem` 的平台扩展内存；
它不能计入 table[108..111]，不能调用 `arm_alloc()`，也不能加入
`arm_ext_origin_node_limit()` 的 first-fit mapped bands。通用修复是把 16 MiB
RW data band `0x50000000..0x50FFFFFF` 惰性 `uc_mem_map_ptr()` 到独立 host
backing。超过容量明确失败，不退主堆。`arm_ptr()`/`arm_addr()` 必须对称支持。

同一活动 SCRRAM 的较小请求复用稳定地址；增长只清零 `[old_len,want)`，不能
清空 live prefix。1015 对 NULL no-op，对当前首址 unmap/free，对 foreign、stale
或 double non-NULL free 返回失败。Unicorn ARM 最小诊断证明在
`UC_HOOK_CODE` 内执行 unmap/free、同址重映射和读写安全；unload 则先
`uc_close()` 再释放仍活动的 backing。当前结构是单活动 SCRRAM；若未来证据
要求多个并存分配，应实现分区或引用计数，不能静默别名或加主堆 fallback。

冷启动回归必须删除 `cache/op6120/sky.bmp`，预置 advbar，并把外部 DNS 映射
到 loopback。固定 MRP SHA-256 `611b4cd7...c8e2`。卡死 PPM 是
`56071684...4003`（1787 色、10207 非黑像素）；成功 PPM 是
`c84009d9...60d8`（1697 色、75128 非黑像素），4M 与 1M 成功帧逐字节一致。
颜色数单独不能区分两者，需用 hash/关键像素、draw > 13 且继续增长、SKY hash
`6db8e39e...56def` 和无 Unicorn/fatal 日志共同断言。最终默认并发 E2E 为
24 files / 45 tests / 295.00s。详见
`docs/修复记录/op6120-4m-startup-hang-debug.md` 与
`omx_wiki/op6120-independent-scrram-4m-startup-fix.md`。
