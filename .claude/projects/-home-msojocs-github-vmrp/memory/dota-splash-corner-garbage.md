---
name: dota-splash-corner-garbage
description: SOLVED — dota.mrp 花屏根因=私有子模块 record 的 readFile 槽指向 wrapper readFile(返回挂载/索引缓冲)
metadata:
  type: project
---

**已修复（2026-06-16）。** `build/skyengine mythroad/dota.mrp` 及 `test/dota/hp.sh` 的花屏（开机角落 15×15 花屏块 + 点击后整屏 RGB 噪声带）全部消失，输出与回归前基线(9d24757)逐像素一致。

## 根因（最终定位）
- **回归提交**：`ffe4cf4 "fix: 有游戏启动报错"`（git bisect，good=9d24757 bad=2f4efa6，判据=开机"开启声音?"屏角落 softkey 色彩多样性）。它把 `table[131]` (mr_cacheSync) 的 staging 路径从「整段覆盖+word[0]=EXT_TABLE_ADDR」改成「跳过前 8 字节、保留 wrapper 写的 module record/P」(`internal_loader_staging` 分支)，本意修 gxdzc/dsm_gm 的"请稍候"卡死。
- **机制**：cfunction.ext 私有 loader 给每个子模块在 wrapper 堆里建一个 **module record**（`file_base[0]`，0x248 字节，是 EXT_TABLE 的逐槽镜像），子模块拿它当自己的 C 函数表基址。该 record 把 4 个槽重定向到 wrapper 自身实现：`[0]malloc [1]free [25]_mr_c_function_new(loader) [125]_mr_readFile`（值=0xE82xxx wrapper code）。回归前整段覆盖让 word[0]=EXT_TABLE_ADDR，子模块走宿主表；回归后保留 record，子模块改走 wrapper 的 readFile。
- **wrapper 的 readFile 在 VMRP 分裂内存模型下返回挂载/索引缓冲(0x66xxxx，含 "start.mr"/文件名/包索引字节)而非宿主 readFile 解码出的位图像素(ap, 0x71xxxx)**。dota 游戏模块用它取角标/背景位图源指针 → 拿到文件索引字节当像素画 → 花屏。这正是前几轮看到的"DrawBitmap 源=0x66–0x69、内容是 start.mr"现象，但前几轮卡在 game.ext 指针追踪、没追到 record[125]=wrapper readFile 这一步。

## 修复
`src/arm_ext_executor.c` case 131 的 `internal_loader_staging` 分支：skip-8 拷贝后，把 record 的 readFile 槽 (`record[125]`) 还原成宿主 EXT_TABLE 的桥接值（`EXT_TABLE[125]`，自指针 bridge），让子模块资源读取走宿主 readFile 得到正确解码位图。**只还原 125 槽**——malloc/free/loader(25) 仍走 wrapper，保留 wrapper 对子模块内存与子加载的管理。数据驱动、无 app 指纹、无兜底分支。

## 为什么不能用别的改法（实测）
- 整段覆盖(回归前) / V3 全 record 镜像 / 把 word[0]=EXT_TABLE_ADDR：都能清 dota，但**破坏 gxdzc**（覆盖了 record[25] wrapper loader → gxdzc 不再显示"是否开启音乐?"，nonblack 416→40，确定性复现）。
- 只还原 record[125]：dota 干净 + gxdzc 仍 416（5/5）+ dsm_gm start-app 仍加载，mpc/nes/mrpinfo 零崩溃。

## 验证方法（headless）
`VMRP_NO_MOUSE=1 VMRP_PPM=1 [VMRP_AUTO_CLICKS=...] timeout N build/skyengine mythroad/dota.mrp`，读 `/tmp/skyengine_screen.ppm`。判据：开机屏角落 18×18 区 distinct 色 <12=干净；hp.sh 全屏与 9d24757 基线 `pixdiff=0`。gxdzc 成功=点击流后 nonblack≈416("是否开启音乐?")。相关 [[app-compat-generalization]]。
