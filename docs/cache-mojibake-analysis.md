# cache 乱码文件夹分析（愤怒的小鸟VS僵尸2_v1002-3）

## 现象

```
build/skyengine build/mythroad/愤怒的小鸟VS僵尸2_v1002-3.mrp
```

运行后 `build/mythroad/cache/` 出现空目录 `鎰ゆ€掔殑灏忛笩VS鍍靛案2_v1002-3`。
该名字是「愤怒的小鸟VS僵尸2_v1002-3」的 **UTF-8 字节被当作 GBK 解码再转回
UTF-8** 的典型乱码。

## 取证

strace（`-e trace=mkdir,mkdirat`）捕获到 guest 侧发起的三次 mkdir：

```
mkdir("mythroad/cache/<乱码全名>", 0777)               = 0
mkdir("mythroad/cache/<乱码全名>{w\7/sound", 0777)     = ENOENT
mkdir("mythroad/cache/<乱码全名>{w\7/sound/sky", 0777) = ENOENT
```

第二、三次末尾的 `{w\7` 垃圾字节说明 guest 把包名拷进了**固定长度缓冲区**：
GBK 名字 27 字节放得下，UTF-8 名字 35 字节溢出且丢失 NUL 终止符。
（aex_module.c 中对 32 字节包名缓冲 ABI 的注释即描述此类风险。）

## 根因链

1. 真机上 Mythroad guest 世界（VM/EXT ARM 代码）全部使用 **GBK** 编码，
   table[100]（包名）在真机上是 GBK 字节。
2. skyengine 在 Linux 上把宿主 **UTF-8** 路径原样暴露给 guest：
   - `arm_ext_init_pack_names()`（src/arm_ext/aex_module.c:375）取
     `mr_get_pack_filename()`（宿主 UTF-8），推导 cwd 相对别名
     `mythroad/愤怒的小鸟VS僵尸2_v1002-3.mrp`，存入 `m->pack_alias`；
   - `arm_ext_set_pack_table_name()`（src/arm_ext_executor.c:1057）把该
     UTF-8 别名写进 guest 可见的 table[100]。
3. guest EXT 代码剥掉目录前缀与 `.mrp`，拼出 `cache/<包名>` 调 mr_mkDir。
4. `get_filename()`（src/mythroad/dsm.c:871）发现路径不存在，按既有规则
   （`FLAG_USE_UTF8_FS` 下 guest 路径是 GBK）做 GBK→UTF-8 转换 —— 对
   本已是 UTF-8 的字节做了二次转换，得到乱码目录名。

即：**guest→host 方向已有 GBK→UTF-8 转换（get_filename），但
host→guest 方向缺少对应的 UTF-8→GBK 转换**，编码边界不对称。

## 修复方案

保持不变量「guest 世界一律 GBK；宿主编码只存在于 host 侧」：

1. `src/mythroad/dsm.c` 新增 `dsm_host_path_to_guest()`：get_filename 编码
   规则的逆操作 —— `FLAG_USE_UTF8_FS` 时把宿主 UTF-8 路径转成 GBK 交给
   guest；非 UTF-8 文件系统（如 Windows ANSI 构建）宿主路径本身就是本地
   编码，原样拷贝。声明加入 `dsm.h`。
2. `arm_ext_init_pack_names()` 用该助手生成 `m->pack_alias`，使 ARM 可见
   的包名别名固定为 GBK 字节。
   - guest 回传该别名时 `arm_ext_pack_to_host_path()` 的精确 strcmp 命中
     （两侧同为 GBK），映射回宿主 UTF-8 路径；
   - guest 拼出的 `cache/<GBK包名>` 路径由 get_filename 正常还原为 UTF-8，
     目录名正确；
   - GBK 名字更短，也消除了 guest 固定缓冲区溢出（`{w\7` 垃圾字节）。

纯 ASCII 包名（op6120.mrp 等）转换前后字节相同，不影响既有用例。

## 状态

- [x] 定位根因（strace + 源码走读）
- [x] 实现修复（dsm_host_path_to_guest + pack_alias GBK 化）
- [x] 该游戏验证：strace 显示三次 mkdir 均为正确 UTF-8 名
      「cache/愤怒的小鸟VS僵尸2_v1002-3(/sound(/sky))」且全部成功，
      `{w\7` 溢出垃圾消失；PPM 截屏确认标题画面正常渲染；
      首个 DNS 仍为 freeads.51mrp.com（别名未退化）
- [x] 回归其它用例：`npm run test:e2e` 24 文件 46 用例全部通过
      （含 op6120「宿主名超 32 字节仍保持 MRP 身份」编码敏感用例、
      gghjt 插件下载、talkcat 资源包下载等）

## 相关但未动的同类暴露点（后续可按需处理）

- 原生 VM `_mr_c_function_table[100..103]` 直接指向 `pack_filename` 等
  宿主编码缓冲（mythroad.c:545、mythroad_mini.c:308）——纯 VM（非 EXT）
  中文名 mrp 理论上有同样问题。
- `arm_ext_guest_pack_alias_for_handoff()` 对非绝对路径的 old_pack 原样
  返回（应用切换场景 table[102]）。
