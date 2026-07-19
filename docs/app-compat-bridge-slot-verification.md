# app_compat_gghjt bridge 槽位映射验证报告

目的：在改动 EXT 重定位通用路径之前，逐槽位验证 `app_compat_gghjt.c::gghjt_on_child_synced`
硬编码填入的 18 个 bridge 值，是否等于"对 `_mr_c_function_table` 做标准重定位"应得的值。
若一致，则证明这些 hook 在做通用引擎职责（标准函数表重定位），可被数据驱动通用化。

## 证据来源（三个独立来源互证）

1. **硬编码常量** — `src/app_compat_gghjt.c` 的 `VERDLOAD_*` 枚举与 `gghjt_on_child_synced()`。
   填入公式：`bridge = EXT_TABLE_ADDR + index*4`，`EXT_TABLE_ADDR = 0x10000`。
2. **平台函数表布局** — `third_party/mtk_disasm/mythroad_mini_relocations.txt` 的
   148 项 `.rel.data`（`_mr_c_function_table`），表起始于数据段 offset `0xa4`，
   `index = (offset - 0xa4) / 4`。
3. **verdload 运行时逆向** — `docs/gghjt-continue-reopen-fix.md` §6，对 `verdload.ext`
   文件偏移 `0x308` 起 bridge copy 函数及消费点（`0x24DC` memset / `0x2582` memcpy）的反汇编。

## 逐槽位对照表

| child RW 目标 | record 源槽 | table idx | 填入值 (源1) | 函数名 (源2: mtk_disasm) | 运行时消费点 (源3) |
|---|---|---|---|---|---|
| rw+0x16C | record+0x68 | 26 | 0x10068 | mr_printf | — |
| rw+0x170 | record+0x0C | 3  | 0x1000C | memcpy   | off 0x2582 调 memcpy ✓ |
| rw+0x174 | record+0x10 | 4  | 0x10010 | memmove  | |
| rw+0x178 | record+0x14 | 5  | 0x10014 | strcpy   | |
| rw+0x17C | record+0x18 | 6  | 0x10018 | mr_strncpy | |
| rw+0x180 | record+0x1C | 7  | 0x1001C | strcat   | |
| rw+0x184 | record+0x20 | 8  | 0x10020 | strncat  | |
| rw+0x188 | record+0x24 | 9  | 0x10024 | memcmp   | |
| rw+0x18C | record+0x28 | 10 | 0x10028 | strcmp   | |
| rw+0x190 | record+0x2C | 11 | 0x1002C | strncmp  | |
| rw+0x194 | record+0x30 | 12 | 0x10030 | strcoll  | |
| rw+0x198 | record+0x34 | 13 | 0x10034 | memchr   | |
| rw+0x19C | record+0x38 | 14 | 0x10038 | memset   | off 0x24DC 调 memset ✓ |
| rw+0x1A0 | record+0x3C | 15 | 0x1003C | strlen   | |
| rw+0x1A4 | record+0x40 | 16 | 0x10040 | strstr   | |
| rw+0x1A8 | record+0x44 | 17 | 0x10044 | sprintf  | |
| rw+0x1AC | record+0x48 | 18 | 0x10048 | atoi     | |
| rw+0x1B0 | record+0x4C | 19 | 0x1004C | strtoul  | |

## 关键一致性校验

- **公式自洽**：`gghjt_on_child_synced` 用 `EXT_TABLE_ADDR + (FIRST_INDEX + i)*4`，
  `FIRST_INDEX=3, COUNT=17` → table[3..19] = 0x1000C..0x1004C；`EXTRA_TABLE_INDEX=26` →
  0x10068。与表中"填入值"列逐行吻合。
- **偏移自洽**：源3 文档独立给出 `record[0x0C..0x4C]→rw+0x170..0x1B0 = table[3..19]`、
  `record[0x68]→rw+0x16C = table[26]`，与源1 的 `VERDLOAD_BRIDGE_SRC_OFF=0x0C /
  DST_OFF=0x170 / EXTRA_SRC_OFF=0x68 / EXTRA_DST_OFF=0x16C` 逐字一致。
- **运行时锚点自洽**：源3 实测 verdload 在 `rw+0x170` 取值调 memcpy、`rw+0x19C` 取值调 memset。
  按表：rw+0x170 = table[3] = memcpy ✓；rw+0x19C 距 rw+0x170 为 0x2C = 11 槽 →
  table[3+11] = table[14] = memset ✓。两个独立锚点都落在 mtk_disasm 命名的正确函数上。

**结论：三方对照零冲突。** 这 18 个硬编码 bridge 值就是"对标准 C 运行时函数表
（memcpy/…/strtoul 块 + mr_printf）做标准 GOT 重定位"的结果，不含任何 gghjt 业务语义。

## 对通用化的直接含义（务必区分两层）

1. **值（VALUES）是通用的，已证。** `table[index] = EXT_TABLE_ADDR + index*4`，索引 3..19/26
   对应的是平台标准函数表槽位。任何走标准 `_mr_c_function_new`(table[25]) 的子模块都会被
   通用重定位正确填好——gghjt 之所以要手填，仅因 `cfunction.ext` 私有 loader 绕过了 table[25]
   （见 gghjt-jit-cache-fix.md / continue-reopen-fix.md §4-5）。
2. **位置（OFFSETS）是 verdload 专属布局，mtk_disasm 不能证明。** `record+0x0C`、`rw+0x170`、
   `record+0x68`、`rw+0x16C` 这些来自 `verdload.ext` 自身反汇编，不是平台 ABI。
   **因此通用实现不能照搬 0x170/0x16C。** 真正通用的做法必须"结构化发现需要重定位的 RW 槽"：
   - 方案 A：在子模块 RW 段扫描，凡槽位值落在"未重定位的函数表相对值"区间的，按其隐含 index
     回填 `EXT_TABLE_ADDR + index*4`；
   - 方案 B：解析子模块自带的重定位记录（若 .ext 头保留），按记录驱动。

   换言之：删 `gghjt_on_child_synced` 的前提，是让通用子模块同步路径
   （`arm_ext_sync_internal_nested_module`）对 private-loader child 也执行结构化 GOT 重定位，
   而不是依赖记忆偏移。

## 下一步

- 通用重定位落地后，保留本表作为回归基准：private child 跑完后 dump `rw+0x16C..0x1B0`，
  断言 == 本表"填入值"列（0x10068, 0x1000C..0x1004C）。
- 验证脚本：`test/gghjt/download-plugin-enter.sh`（触发 netpay/verdload 下载 UI 路径）。

## 运行时验证结果（通用化落地后）

通用化已落地：`arm_ext_repair_private_child_bridges()`（`src/arm_ext_executor.c`）取代了
`gghjt_on_child_synced`，record 填值由 master `EXT_TABLE` 自指针谓词数据驱动，RW 镜像由
声明式 `verdload_rw_runs` 描述符驱动（无文件名/指纹/魔数索引）。`VMRP_ARM_EXT_DIAG=1` 跑
`build/skyengine mythroad/gghjt.mrp`（删 netpay.mrp 触发下载路径）实测 18 个 RW 槽逐字等于本表
"填入值"列：`rw+0x16C=0x10068`、`rw+0x170..0x1B0=0x1000C..0x1004C`，零崩溃。

跨 11 个游戏回归（`VMRP_ARM_EXT_DIAG=1`，各跑数秒）：
- **复用 verdload wrapper、18 槽逐字命中、干净启动**：gghjt, mpc, gxdzc, nes, dota, mrpinfo。
  （旧代码只按文件名修 gghjt 一个；通用化后这 5 个共享 wrapper 的游戏免费获得同样修复。）
- **通用 pass 为 no-op（0 修复行）、干净启动**：applist, dsm_gm, ydqtwo, mssj。证明按结构化
  拟合 + 空白槽守卫是选择性的，不会对无该 private child 的游戏误触发。
- 全部 11 个游戏零崩溃 / 零 Invalid instruction。`skyengine_app_compat_tests`、`skyengine_filelib_tests` 绿；
  `skyengine_api_tests` 的 2 个 event/timer 转发失败是先于本改动存在的、与本路径无关（该测试不链接
  `arm_ext_executor.c`/`app_compat_gghjt.c`）。
