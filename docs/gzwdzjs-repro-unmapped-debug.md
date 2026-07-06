# gzwdzjs repro.sh UC_MEM_WRITE_UNMAPPED 调试记录

## 2026-07-06 复现基线

- 目标命令:`test/e2e/gzwdzjs/repro.sh`。按要求不使用 xvfb,复现时使用 `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy`。
- 构建命令:`cmake --build build --target vmrp -j2` 通过。
- 复现产物:`/tmp/gzwdzjs-repro-unmapped-current/stdout.log` 和 `stderr.log`。
- 结果:脚本自己的 `timeout 90s` 返回 `124`,stdout 尾部持续重复:
  - `arm_ext_executor: uc_emu_start(0xE800B0) failed: 7 (Invalid memory write (UC_ERR_WRITE_UNMAPPED))`
  - `arm_ext_executor: invalid memory UC_MEM_WRITE_UNMAPPED addr=0xFFFFFFF5 size=4 value=0x23`
  - crash PC 为 Thumb `0x23A880`,LR 为 `0x23209D`,SP 已变成 `0x00000009`。
- 初步判断:崩溃点是 `0x23A880` 函数入口的栈写入,写地址 `0xFFFFFFF5` 来自 Thumb prologue 在坏 SP 上做 push/sub;问题重点不是该函数写错地址,而是上游以错误的 SP/调用帧进入该回调。

## 下一步

- 反汇编 `0x23A880`、`0x23209D` 和日志里的最后活跃 child 文件范围 `0x2F5384..0x2F614A`。
- 用窄口径诊断确认进入 `0x23A880` 前的 bridge/table 分派参数、R9/R10、wrapper 栈恢复路径,避免全量 trace。
- 修复必须保持通用 ARM EXT/桥表语义,不能按 gzwdzjs 名称或固定应用流程分支。

## 2026-07-06 21:18 最终定位与修复

- 反汇编取证:
  - 从 `build/mythroad/gzwdzjs.mrp` 抽取 `game.ext`,解压后 116492 字节,
    仍按旧记录装载基址 `0x226118` 反汇编。
  - `0x23A880` 字节为 `f0 b5 83 b0 ...`,Thumb 指令是
    `push {r4,r5,r6,r7,lr}; sub sp,#12`;旧日志里的 `SP=0x9`
    会让入口 push 写到 `0xFFFFFFF5`,所以该点是坏栈受害者。
  - `0x232098 bl 0x23A880`,旧日志 `LR=0x23209D` 说明坏 SP 已经带入
    调用者后继续调用该绘制/回调函数。
  - 关键桥接点 `0x23B5AE blx r7` 后紧跟 `0x23B5B0 pop {r3,r4,r5,r6,r7,pc}`。
    宿主 table bridge 在 `hook_table()` 中通过写 `PC=LR` 模拟 native 返回;
    如果同一个 `blx reg; pop {...,pc}` epilogue 在 pop 已经消费栈帧后被再次进入,
    就会重复 pop 并把后续回调的 SP 带坏。
- 通用修复:
  - `src/arm_ext_executor.c` 在 table bridge 返回时记录 LR 指向的 Thumb
    `blx reg; pop {...,pc}` 返回点和当时 SP。
  - `hook_restore_r9()` 的 block hook 只在首次正常进入该 epilogue 后,
    计算 pop 后 SP;若后续又以同一 PC、同一 pop 后 SP、LR 仍指向该 epilogue
    的形态重复进入,则停止当前 ARM 执行到 `EXT_STOP_ADDR`。
  - 判据只依赖指令形状和运行时 SP/LR,不使用包名、应用名、固定 gzwdzjs 地址,
    也不把任意异常当作成功兜底。
- 目标脚本验证:
  - 命令:`SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy VMRP_E2E_SOCKET=/tmp/gzwdzjs-final-drawcount/e2e.sock VMRP_PPM=1 VMRP_PPM_PATH=/tmp/gzwdzjs-final-drawcount/latest.ppm ... build/vmrp build/mythroad/gzwdzjs.mrp`
    使用 `test/e2e/gzwdzjs/repro.sh` 的同一自动按键序列。
  - `timeout 95s` 返回 124 属观察窗口到期;stdout/stderr 合计仅 1796 字节,
    `grep 'UC_ERR|UC_MEM|invalid memory|failed|crash'` 无命中。
  - `DRAW_COUNT` 采样:5s=98、15s=176、30s=283、55s=283、85s=283,
    每次 E2E socket 响应约 0.1-0.2ms,无错误刷屏导致的卡顿。
  - 最新 PPM:`/tmp/gzwdzjs-final-drawcount/latest.ppm`,240x320,
    sha256 前缀 `66d382b4f7105779`,unique=17,nonzero=88954。
- 回归:
  - `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/gzwdzjs/game-start.test.ts --reporter=verbose`
    通过,2 个用例全部通过。
  - `pnpm vitest run test/e2e --reporter=verbose` 通过,16 个测试文件、
    27 个用例全部通过。
