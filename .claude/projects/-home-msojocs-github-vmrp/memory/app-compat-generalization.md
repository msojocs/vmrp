---
name: app-compat-generalization
description: How to generalize src/app_compat* hardcoded hooks; mtk_disasm is the authoritative loader reference
metadata:
  type: project
---

目标：把 src/app_compat.c + src/app_compat_gghjt.c 的硬编码 hook 改成通用代码（最终删掉 profile）。

关键发现：gghjt 的 hook 不是"应用专属逻辑"，而是 skyengine 通用 EXT 加载/重定位机制的补丁。`on_child_synced` 硬编码的 `table[3..19]`/`table[26]` 经 `third_party/mtk_disasm` 的 `_mr_c_function_table` 解码后，就是 memcpy/strcpy/sprintf…/mr_printf 这些**标准 C 运行时函数表槽位**——任何 ext 子模块都要重定位的，可数据驱动通用化。

**third_party/mtk_disasm 是权威参考**（不是闭源黑盒）：含 mythroad VM 反汇编、`mythroad_mini_relocations.txt`（148 项 _mr_c_function_table + 16 项 _mr_c_internal_table 完整布局，table 从数据段 offset 0xa4 起，index=(offset-0xa4)/4）、以及已有伪 C 还原 `DECOMPILATION_ANALYSIS.md`。table[25]=_mr_c_function_new=标准子模块加载器入口。

**进度（2026-06）：bridge 路径已通用化（低风险版完成）。** `gghjt_on_child_synced` + 指纹（0x47E4/0x36DC/prologue）+ 魔数索引 + 死代码（wrapper-state 4 函数 + on_child_confirmed）已删；执行器新增 `arm_ext_repair_private_child_bridges()`（在 `arm_ext_sync_internal_nested_module` 调用），record 填值由 master EXT_TABLE 自指针谓词数据驱动，RW 镜像由声明式 `verdload_rw_runs` 描述符（`{0x68→0x16C×1, 0x0C→0x170×17}`，纯数据、结构化拟合、不按文件名）驱动。实测 6 个游戏（gghjt/mpc/gxdzc/nes/dota/mrpinfo）共享该 wrapper 全部免费修复、18 槽逐字命中基线；4 个（applist/dsm_gm/ydqtwo/mssj）通用 pass 为 no-op；11 游戏零崩溃。详见 docs/app-compat-bridge-slot-verification.md。

**仍未做（残留硬编码，下一步可选）：** (1) `should_protect_got_addr` 仍按 gghjt profile 的 wrapper_rw+0x190 偏移；(2) gzip/netpay 提取 hook（intercept_write/post_read_hook，magic 0x2001BC/0xBE）仍 app 专属；(3) `app_compat_select` 仍按文件名 "gghjt.mrp" 选 profile（为上述两项服务）。(4) "根因修复"（让私有 loader 自己填 record，从而删整个 profile）未做——属高风险，留待评估。

**Why:** 之前误以为加载器源码闭源不可参考，实际 mtk_disasm 提供了完整机制。
**How to apply:** 通用化时遍历子模块 GOT，凡值落在函数表区间的槽统一回填 table_base+index*4，删掉 on_child_synced/should_protect_got_addr 的魔数；read/write 的 gzip 补丁对照 mr_iolib/mr_inflate 与 internal table[7..10] (mr_gzInBuf/LG_gzinptr) 语义。验证用 skyengine_app_compat_tests + test/gghjt/download-plugin-enter.sh。死代码 save/restore_wrapper_state 直接删。相关 [[gghjt-is-game]]。
