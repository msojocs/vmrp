# gghjt.mrp 支付流程崩溃与卡死修复报告

## 问题描述

执行 `build/skyengine mythroad/gghjt.mrp` 后按特定步骤进入付费流程，点击付费后程序崩溃（UC_MEM_READ_UNMAPPED）。

复现脚本：`test/bug.sh`
```bash
export VMRP_AUTO_CLICKS="0,0,5000;227,308,2000;227,308,2000;227,308,2000;227,308,2000;227,308,4000;160,290,3000;121,291,1000;121,291,3000;28,308,2000"
build/skyengine mythroad/gghjt.mrp
```

## 分析方法

使用 `arm-none-eabi-objdump` 反汇编 crash PC 附近的 ARM Thumb 代码，结合 Unicorn 内存写入钩子（`UC_HOOK_MEM_WRITE`）追踪 GOT 条目的写入历史，定位被覆盖的函数指针。

## 根因分析

支付流程涉及三个相互关联的 Bug：

### Bug 1：GOT 函数指针被 dump0 恢复覆盖（崩溃）

**现象**：crash PC=0x65BD16，R3=0x1C6D24EE（垃圾地址），实际是 `blx r3` 调用了一个被覆写的 memset 函数指针。

**机制**：
1. gghjt 启动时，ARM 代码将 bridge 函数地址（如 memset=0x10038）写入 R9 数据区的 GOT 表
2. 支付时，gghjt 将模块内存保存到 `dump0` 文件，然后加载 netpay 插件（覆写模块内存）
3. 支付完成后，从 `dump0` 读回模块内存恢复状态
4. **问题**：宿主侧 `memcpy(arm_ptr(...))` / `memset(arm_ptr(...))` 绕过 Unicorn 写钩子，直接操作 mmap 内存。dump0 读回的原始数据覆盖了已重定位的 bridge 指针，GOT 条目变成文件中的原始字节（如 0x1C6D24EE）
5. ARM 代码通过 GOT 调用 memset 时跳转到垃圾地址，触发 `UC_MEM_READ_UNMAPPED`

**修复**（`src/arm_ext_executor.c`）：
- **`hook_got_write`**：注册 `UC_HOOK_MEM_WRITE` 监听整个堆区，当 ARM 代码向 R9 数据区写入 bridge 范围 `[EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT*4)` 内的值时记录到快照；当写入非 bridge 值时清除该槽位（避免误恢复动态回调指针）
- **case 3/14/44（memcpy/memset/mr_read）**：宿主侧内存操作后，扫描快照中被覆盖区域的 bridge 条目并回写正确值

### Bug 2：Unicorn 翻译缓存未失效（执行旧代码崩溃）

**现象**：dump0 恢复后执行到 netpay 遗留的翻译块（Translation Block），PC 处的字节已被 dump0 覆盖为 gghjt 代码，但 Unicorn JIT 缓存仍是旧的 netpay 翻译结果。

**修复**（`src/arm_ext_executor.c` case 44）：
```c
if ((int32_t)ret > 0 && (uint32_t)ret > 0x1000) {
    uc_ctl_remove_cache(m->uc, r1, r1 + (uint32_t)ret);
    // ...GOT 修复...
}
```
大块 `mr_read` 后调用 `uc_ctl_remove_cache` 清除目标区域的翻译缓存。

### Bug 3：netpay 覆写 wrapper 路径字符串导致支付后卡死

**现象**：支付完成后游戏卡在"请稍等"界面不推进。

**机制**：
1. wrapper 的路径字符串 `"c:/mythroad/"` 存储在 R9 偏移 ~0x150 处（地址约 0x645D90）
2. 该地址位于 dump0 保存范围（0x646100+）之外的间隙区域
3. netpay 加载时覆写此区域，字符串变为 `"c:/mythrD/"` 等残余数据
4. dump0 恢复不覆盖此间隙，路径保持损坏状态
5. 后续文件操作使用错误路径，`sms.sav` 写入失败（fd=0），支付结果无法保存

**修复**（`src/mythroad/dsm.c` `SetDsmWorkPath`）：
```c
if (strncmp2(dsmWorkPath, "mythroad/", 9) != 0 &&
    strncmp2(dsmWorkPath, "mythr", 5) == 0) {
    // 检测到被 netpay 破坏的路径前缀，修正为 "mythroad/"
    snprintf_(fixed, sizeof(fixed), "%s%s", MYTHROAD_PATH, dsmWorkPath + strlen2("mythroad/"));
    strncpy2(dsmWorkPath, fixed, sizeof(dsmWorkPath) - 1);
}
```

### 附加修复：`mr_plat` 未实现的平台接口

- **`mr_plat(1011)`**：netpay 短信查询/触发接口。必须返回 `MR_IGNORE`（不是 `MR_SUCCESS`），否则 netpay 走错误分支跳过 `pyone.sys` 删除，导致支付结果丢失
- **`mr_plat(1214)` = `MR_SET_KEY_END`**：启用/禁用按键结束事件，返回 `MR_SUCCESS`

## GOT 快照的误恢复问题

初始实现中 `hook_got_write` 只记录 bridge 值写入但不清除。当 ARM 代码先写入 bridge 值（初始化 GOT）后又写入游戏回调指针（动态更新槽位）时，快照仍保留旧的 bridge 值。dump0 恢复后快照把动态回调指针错误地覆写为 bridge 地址（例如 strncmp 的 bridge 地址 0x1002C），导致游戏定时器回调变成 `strncmp(NULL, NULL, 0)` 无法分发游戏逻辑。

修复：写入非 bridge 值时清除对应快照槽位：
```c
} else {
    /* 非 bridge 值覆盖 → 清除快照（该槽位是动态回调，不应被恢复） */
    m->got_snapshot[idx] = 0;
}
```

## 修改文件清单

| 文件 | 修改内容 |
|------|---------|
| `src/arm_ext_executor.c` | GOT 快照机制（hook_got_write + 三处恢复点）；TB 缓存失效；增强 crash handler（前置字节、二进制导出、栈内容） |
| `src/mythroad/dsm.c` | 路径损坏检测修正；mr_plat(1011) 返回 MR_IGNORE；MR_SET_KEY_END(1214) 实现 |
| `src/main.c` | 自动点击支持每步自定义延迟（`x,y,delay_ms`） |

## 验证

```bash
# 支付流程测试（应看到 sms.sav 和 dat.sav 被写入，无 crash）
VMRP_AUTO_CLICKS="0,0,5000;227,308,2000;227,308,2000;227,308,2000;227,308,2000;227,308,4000;160,290,3000;121,291,1000;121,291,3000;28,308,2000" \
  timeout 90 build/skyengine mythroad/gghjt.mrp 2>&1 | grep "sms\.sav\|dat\.sav\|NETPAY\|invalid memory"

# 兼容性测试
timeout 10 build/skyengine mythroad/nes.mrp 2>&1 | grep -c "invalid memory"     # 应为 0
timeout 10 build/skyengine mythroad/dota.mrp 2>&1 | grep -c "invalid memory"    # 应为 0
timeout 10 build/skyengine mythroad/mrpinfo.mrp 2>&1 | grep -c "invalid memory" # 应为 0
```

## 内存布局示意

```
0x010000 ┌─────────────────┐ EXT_TABLE_ADDR (bridge 函数表)
         │ table[0..149]   │ ← hook_table 拦截 ARM 函数调用
0x010258 ├─────────────────┤
         │                 │
0x080000 ├─────────────────┤ EXT_CODE_ADDR (wrapper 代码)
         │ wrapper .text   │
0x180000 ├─────────────────┤ EXT_STACK_ADDR
         │ ARM stack       │
0x200000 ├─────────────────┤ EXT_HEAP_ADDR (arm_alloc bump 分配)
         │ 各种 u32 slot   │
         │ ...             │
0x645C40 │ wrapper R9/P    │ ← wrapper 的数据基址
0x645C70 │ (memset 清除区) │
0x645CF0 │ ...             │
0x645D50 │ 路径字符串等    │ ← netpay 覆写此间隙 → 路径损坏 BUG
0x646100 ├─────────────────┤ dump0 保存/恢复起始
         │ gghjt 代码+数据 │
0x66D5AC │  └ R9 GOT 区域  │ ← bridge 指针被覆盖 → 崩溃 BUG
         │                 │
0x6DC100 ├─────────────────┤ dump0 保存/恢复结束
         │ ...             │
0x880000 └─────────────────┘
```
