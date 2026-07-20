# app_compat 硬编码去除 —— 进度文档

目标:去掉 `src/app_compat_gghjt.c` / `src/app_compat.c` 的按应用名硬编码逻辑
(netpay chk 解压替换 workaround + GOT 事件槽保护),改成通用逻辑;
禁止 `if(is_xxx_app())` 式代码与兜底逻辑。

状态:**已完成**(2026-07-20,全量 e2e 28 文件/51 用例全部通过)

## 最终结论(指令级根因)

gghjt 的"netpay 提取死循环 / chk 解压失败"与 app 的 inflate 实现无关,
guest 端 inflate 完全正常。真正根因是一处 **模拟器与真机的栈死区语义差异**:

1. 提取链(game.ext,加载于 0x234D50,r9=0x24D7BC):
   `extractOne @0x22e708` → `readChunk @0x242658(mode=0)` →
   解压包装 `@0x2434ac` → 位读 inflate `@0x241fdc/@0x2421d8`(纯 guest 代码)。
2. `extractOne` 把输出缓冲设为 **屏幕缓冲**(经 `getScreenBuf @0x24719c`,
   即 table[91] 槽 = 0x2001BC),但 **容量槽 [sp+8] 从不初始化**(0x22e70e
   `sub sp,#12` 后只写了 [sp+0]/[sp+4])。
3. 解压包装 `@0x2434ac` 的容量检查(0x2434f6-0x2434fc):
   `*out != 0` 时要求 `*outlen >= 解压尺寸`,否则返回 -1 且不更新 out/outlen。
4. 真机上 table 系统调用是固件里真实 ARM 实现,调用后 SP 之下死区残留
   返回地址/帧指针等**大数值**,该未初始化容量槽因此恒 ≥ 解压尺寸,检查
   总能通过(应用带着这个未初始化 bug 在真机上"碰巧"工作);
   模拟器宿主实现不触碰 guest 栈,槽里残留 res.list 成功后写入的 **0xBE**,
   后续 10 条 chk(解压 ~67KB)全部容量判定失败 → readChunk 返回 -1 →
   `mr_write(fd, 0x2001BC, 0xBE)` 把陈旧 res.list 文本写进 pak(10×190=1900)。

## 通用修复(无任何按应用判断)

`src/arm_ext_executor.c` `hook_table()` 入口:每次 table 调用前,把
**本次调用返回地址(lr)与调用方 sp** 交替填充到 guest SP 之下 64 字节,
恢复真机"被调用者压栈后死区残留地址类数值"的语义。对遵守 AAPCS 的代码
无可观察差异(SP 之下本就是被调用者可随意使用的死区)。

注:第一版尝试按 `push {r4-r11,lr}` 填充真实寄存器值失败——本应用调用点
r11=0,恰好落在容量槽上;地址类数值(lr/sp)才是真机死区的普遍形态。

## 删除内容

- `src/app_compat.c`、`src/app_compat_gghjt.c`、`src/include/app_compat.h` 整体删除。
- `ArmExtModule.profile/app_state` 字段及 select/init/cleanup 调用。
- `aex_t043` 的 intercept_write/post_write_cleanup(host zlib 解压替换写入)。
- `aex_t044` 的 post_read_hook(gzip 读取缓存解压)。
- `app_should_protect_got_addr`(GOT 事件槽保护)及 4 处调用
  (hook_got_write / aex_t003 / aex_t014 / aex_t044)。
- `aex_module.c` 的 app_on_child_confirmed。
- CMakeLists 4 个 target 的 8 处源文件条目。
- `arm_ext_internal.h` 补回原经 app_compat.h 间接引入的 types.h 与
  `ArmExtModule` 前置声明。

## 验证

- 冷启动(profile 删除 + 栈死区修复):`mythroad/gghjt/gghjt.pak` 673448 字节,
  与 fixture **逐字节一致**(guest 原生 inflate 输出 == 旧 host zlib 替换输出);
  PPM 像素 (227,308)=RGB(0,0,0) 与 e2e bgm-select 断言一致。
- `pnpm vitest run test/e2e/gghjt/`:7/7 通过(game-start 含支付+dump0 恢复
  +场景渲染;download-plugin 全部连续进入/返回场景)——GOT 事件槽保护删除后
  亦无回归。
- 全量回归:`pnpm vitest run test/e2e/` 28 文件 / 51 用例全部通过;
  `vmrp-unit` 94 checks 0 failures。

## 排查方法备忘

- 复现脚本 /tmp/run_gghjt.sh(冷启动+LEFT_SOFT 自动按键+PPM);
  判定:pak==673448 成功 / 1900 失败(陈旧写)/ 0(容量槽被 0 覆盖)。
- 关键定位手段:hook_table 临时 UC_HOOK_CODE 观察 guest 函数入口
  (readChunk/解压包装/容量失败分支 0x2434fc),一次性内存 dump +
  arm-none-eabi-objdump -M force-thumb 静态反汇编,BL 目标扫描找调用者。
