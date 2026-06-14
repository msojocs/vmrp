# 修复：DSM 应用列表启动应用卡在"请稍候"

## 问题描述

运行 `test/dsm_gm/start-app.sh`，从 DSM 应用列表选择应用并点击"启动应用"后，
界面一直停在"请稍候..."状态，应用无法加载。

直接启动 MRP（如 `build/vmrp mythroad/gxdzc.mrp`）也有同样问题——黑屏。

## 排查思路

### 1. 确认自动点击生效

首先验证 `VMRP_AUTO_CLICKS` 注入的点击是否正确触发：

```bash
export VMRP_AUTO_CLICKS="0,0,1000;75,176,500;75,176,500;59,58,500;59,58,500"
export VMRP_PPM=1
timeout 25 build/vmrp > stdout.log 2> stderr.log
```

通过 `grep "CLICK" stdout.log` 确认 5 个点击全部触发。PPM 截屏确认界面停在
"请稍候..."。

### 2. 分析 ARM ext 加载流程

使用 `VMRP_ARM_EXT_DIAG=1` 查看 ARM 扩展调用：

```
DIAG arm_ext_call code=0 ... primaryP=0x0 primaryH=0x0
DIAG arm_ext_call_post code=0 ... armTimer=0 hostTimer=0 pending=0
```

关键发现：code=0（init）执行后 **primaryP=0x0**——没有注册 game 模块。
与 gghjt 对比，gghjt 在 code=0 期间通过 `table[25]`（mr_c_function_load）
注册了 game 模块，而 gxdzc 的 cfunction.ext 使用了延迟加载路径。

### 3. 追踪 game.ext 加载过程

使用 `VMRP_ARM_EXT_TRACE=1` 查看 table 函数调用，发现 code=0 期间的完整流程：

1. `table[125]`（_mr_readFile）读取 game.ext → 解压 168012 字节 → `arm_alloc` 分配到地址 A
2. wrapper 解析 MRP 索引，通过 `table[44]` 读取索引数据到另一个地址 B
3. wrapper 内部 loader 创建 extChunk，但 **file 字段指向地址 B（MRP 索引），而非地址 A（game 代码）**
4. `table[131]`（mr_cacheSync）同步地址 B
5. wrapper 尝试 BLX 跳转到 B+0x34 执行 game 入口 → **执行到 MRP 索引文本数据 → UC_ERR_INSN_INVALID**
6. 错误处理器检测到 `lr_in_wrapper` 条件成立 → 当作"跳到堆数据"处理 → 安静退出 code=0

### 4. 确认根因：内存空间不一致

关键对比：
- **真机**：Lua VM 和 ARM ext 共享同一块物理内存，`_mr_readFile` 返回的指针
  ARM 代码可直接访问
- **模拟器**：ARM ext 使用 Unicorn 独立的内存空间（`mmap` 在 `EXT_BASE_ADDR`），
  与 Lua VM 的 `LG_mem_base` 分离

wrapper 的 ARM 代码基于共享内存假设计算 game 数据地址（从 LG_mem_base 偏移），
但在模拟器中该地址包含的是 MRP 索引数据而非 game 代码。

通过 hex dump 验证：
```
pc bytes @0x646114: 6D 65 72 3D ...  # ASCII "mer=" (WML文本，非ARM指令)
```

而 game.ext 原始文件在同一偏移处：
```
offset 0x34: 00 00 50 e3  # CMP R0, #0 (有效ARM指令)
```

## 修复方案

### 核心修复：`internal_loader_staging` 数据拷贝

**文件**：`src/arm_ext_executor.c`，`table[131]`（mr_cacheSync）处理逻辑

当 wrapper 内部 loader 已创建 extChunk（`internal_loader_staging=1`）时，
从 `last_file_copy`（table[125] 缓存的正确解压数据）拷贝代码部分到 staging 地址，
跳过前 8 字节（wrapper 已写入的模块元数据 "MRPGCMAP" 头被替换为 module record + P 指针）：

```c
if (r1 == 9 && internal_loader_staging &&
    m->last_file_copy && r2 && r3 <= m->last_file_len &&
    r3 > 8 && arm_ptr(m, r2 + 8)) {
    memcpy(arm_ptr(m, r2 + 8), m->last_file_copy + 8, r3 - 8);
}
```

### 辅助修复

1. **`lr_in_wrapper` 误判排除**（`run_arm_with_sp` 错误处理）：
   当 PC 在 `pending_internal_file` 范围内时，不将合法的 wrapper→game BLX 跳转
   误判为"跳到堆数据"而强制退出

2. **无 primary module 时保持 timer 运行**（`arm_ext_call` code=2 后处理）：
   wrapper 加载阶段 code=2 处理完后不通过 table[31] 重启 timer，宿主主动补启

3. **`arm_ext_host_cache_sync`**（新函数）：
   Lua 层调用 `mr_cacheSync` 时，将数据同步到 ARM ext 内存的 pending 地址

## 涉及文件

| 文件 | 修改 |
|------|------|
| `src/arm_ext_executor.c` | 核心修复 + lr_in_wrapper + timer restart + host_cache_sync |
| `src/include/arm_ext_executor.h` | 新增 `arm_ext_host_cache_sync` 声明 |
| `src/mythroad/mythroad.c` | `_mr_TestCom1` case 9 调用 host_cache_sync |
| `test/stubs.c` | 新增 `arm_ext_host_cache_sync` 桩函数 |

## 验证

```bash
# 1. 从 DSM 应用列表启动（原始问题场景）
export VMRP_AUTO_CLICKS="0,0,1000;75,176,500;75,176,500;59,58,500;59,58,500"
export VMRP_PPM=1
timeout 20 build/vmrp
# 截屏应显示"是否开启音乐?"而非"请稍候..."

# 2. 直接启动 gxdzc
export VMRP_AUTO_CLICKS="0,0,500;228,309,500;76,291,500;123,291,500"
export VMRP_PPM=1
timeout 20 build/vmrp mythroad/gxdzc.mrp
# 截屏应显示游戏画面

# 3. 回归测试 gghjt
export VMRP_AUTO_CLICKS="0,0,500;228,309,500;158,293,1000;-3,0,500;133,295,3000"
export VMRP_PPM=1
timeout 20 build/vmrp mythroad/gghjt.mrp
# 截屏应显示"罪恶都市 - 开始游戏"

# 4. 单元测试
./build/test_vmrp        # All tests PASSED
./build/test_vmrp_api    # 16/16 passed
```

## 已知遗留问题

gxdzc 启动后 wrapper timer dispatch 处理过程中会报一次
`UC_MEM_WRITE_UNMAPPED addr=0xFFFFFFE2` 错误。这是 gxdzc 自身 wrapper
定时器队列中包含的过期 WML 回调地址导致的**预存 bug**，之前因为游戏从未成功
加载而未暴露。该错误不影响游戏运行（错误后游戏继续正常显示主菜单）。
