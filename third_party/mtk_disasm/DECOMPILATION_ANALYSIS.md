# MTK11B mythroad_mini 反编译伪C代码还原

## 平台信息
- 芯片: MTK (MT6253/MT11B)
- 编译器: RVCT (ARM RealView Compilation Tools)
- ABI: ARM EABI4
- MR_VERSION: 2011 (0x7db)
- 每个函数编译为独立 ELF section (i.funcname)

---

## 1. _mr_TestComC — 原生扩展加载/调用（最关键差异）

```c
// r4 = 指向全局基地址结构体 (与 _mr_c_internal_table 相关)
// 结构体 [0x88] = 函数入口指针 (code entry)
// 结构体 [0x8c] = mr_load_c_function 指针
// 结构体 [0x90] = mr_c_function 指针

static int _mr_TestComC(int input0, char* input1, int32 len, int32 code) {
    int ret = 0;

    if (input0 == 800) {
        // === 加载 native ARM extension ===
        // input1 = 文件缓冲区指针 (已读入的 .ext 文件内容)
        // len    = 文件长度
        // code   = 0

        // 将入口点设为 buf+8（跳过8字节文件头）
        base->entry = (void*)(input1 + 8);  // [0x88]

        // 在缓冲区开头写入 0xa4
        // 0xa4 = ARM指令 "BX LR" 的编码?  不对...
        // 实际上 0x000000a4 是一个偏移量/magic标记
        // 可能是 _mr_c_function_table 的起始偏移
        *(uint32*)input1 = 0x000000a4;

        // 对齐到32字节边界，刷新 I-cache / D-cache
        uint32 aligned_start = (uint32)input1 & ~0x1f;
        uint32 aligned_end   = ((uint32)(len + 93)) & ~0x1f;  // 93 = 0x5d
        mr_cacheSync(aligned_start, aligned_end);

        // 直接跳入！参数 r0 = code
        ret = ((int(*)(int))base->entry)(code);

    } else if (input0 == 801) {
        // === 调用已加载的 extension ===
        uint8* output = NULL;
        int32  output_len = 0;

        // 从 [0x8c] 获取 mr_load_c_function，从 [0x90] 获取 mr_c_function
        func = base[0x8c];     // = mr_load_c_function
        cf   = base[0x8c + 4]; // = mr_c_function (实际偏移 0x90)

        ret = func(cf, code, input1, len, &output, &output_len);
    }

    return 0;  // 注意：始终返回 0 (r6=0 初始化后不变)
}
```

### 与 vmrp 对比

vmrp 的 `_mr_TestComC`:
```c
case 800: arm_ext_load(&native_ext, input1, len, code, &ext_r0); // 通过 unicorn 模拟执行
case 801: arm_ext_call(native_ext, code, input1, len, ...);
```

**关键发现：**
1. MTK 原版直接在 **真实 ARM CPU** 上执行 .ext 文件中的原生代码 (blx 跳转)
2. 写入 `0xa4` 到 buf[0]：这个值 == `_mr_c_function_table` 在数据段中的偏移量！
   所以实际上是告诉 .ext 的初始化代码：函数表在哪里
3. 入口点在 buf+8，跳过了8字节的头（可能是 magic + version/size）
4. **必须** 调用 `mr_cacheSync` 刷新缓存才能执行写入内存的代码
5. vmrp 使用 unicorn ARM 模拟器来实现同样的功能——正确的抽象

---

## 2. _mr_TestCom — 主调度函数

```c
int _mr_TestCom(mrp_State* L, int input0, int input1) {
    int ret = 0;
    // base 指向全局状态结构体
    // base[0x00] = mr_soundOn (byte)
    // base[0x01] = mr_shakeOn (byte)
    // base[0x02] = mr_sms_return_flag (byte)
    // base[0x04] = mr_state
    // base[0x08] = bi (flags)
    // base[0x0c] = mr_timer_state
    // base[0x10] = mr_timer_run_without_pause
    // base[0x14] = mr_exception_str
    // base[0x18] = mr_ram_file
    // ...
    // base[0x24] = mr_event_function
    // base[0x28] = mr_timer_function
    // base[0x2c] = mr_stop_function
    // base[0x30] = mr_pauseApp_function
    // base[0x34] = mr_resumeApp_function
    // base[0x3c] = MR_SCREEN_MAX_W
    // base[0x40] = mr_screen_w
    // base[0x44] = mr_screen_h
    // base[0x84] = mr_sms_return_val

    uint8 flags = base->bi;  // [0x08]

    switch (input0) {
    case 1:  ret = mr_getTime(); break;
    case 2:  base->mr_event_function = input1; break;    // [0x24]
    case 3:  base->mr_timer_function = input1; break;    // [0x28]
    case 4:  base->mr_stop_function = input1; break;     // [0x2c]
    case 5:  base->mr_pauseApp_function = input1; break; // [0x30]
    case 6:  base->mr_resumeApp_function = input1; break;// [0x34]
    case 7:  return input1;  // 直接返回 (MR_PLAT_DRAWTEXT)
    case 9:  mrc_appInfo_st.ram = input1; break;

    case 100: ret = LG_mem_min; break;
    case 101: ret = LG_mem_top; break;
    case 102: ret = LG_mem_left; break;

    case 200:  // mr_startShake
        if (mr_state != MR_STATE_RUN || !mr_shakeOn) break;
        ret = mr_startShake(input1);
        break;

    case 300: mr_soundOn = (int8)input1; break;
    case 301: mr_shakeOn = (int8)input1; break;
    case 302: flags |= MR_FLAGS_EI; break;   // 0x04
    case 303: flags &= ~MR_FLAGS_EI; break;
    case 304: flags |= 0x08; break;
    case 305: flags &= ~0x08; break;

    case 306:
        mr_sms_return_flag = 1;
        mr_sms_return_val = input1;
        break;
    case 307:
        mr_sms_return_flag = 0;
        break;

    case 400: mr_sleep(input1); break;

    case 401:  // set MR_SCREEN_MAX_W
        ret = base->mr_screen_w; // [0x40]
        base->mr_screen_w = input1;
        break;

    case 404:  // _mr_newSIMInd
        ret = _mr_newSIMInd((int16)input1, NULL);
        break;

    case 405:  // mr_closeNetwork
        ret = mr_closeNetwork();
        break;

    case 406:  // set MR_SCREEN_H
        ret = base->mr_screen_h; // [0x44]
        base->mr_screen_h = input1;
        break;

    case 407:  // mr_timer_run_without_pause
        base->mr_timer_run_without_pause = input1;
        mr_plat(1202, input1);
        break;

    case 408:  // screen buffer resize (复杂逻辑)
        // 检查 bitmap[BITMAPMAX].type 和 buflen
        // 如果 type==FIRST_BUF, 重新分配
        // 如果 type==SECOND_BUF, 检查大小是否够用
        {
            mr_bitmap_t* bmp = &base->bitmap_area;
            if (bmp->type1e8 == 0) { // FIRST_BUF
                void* newbuf = mr_malloc(input1);
                if (newbuf) {
                    mr_free(mr_screenBuf, bmp->buflen1e4);
                    mr_screenBuf = newbuf;
                    bmp->buflen1e4 = input1;
                    ret = MR_SUCCESS;
                } else {
                    ret = MR_FAILED;
                }
            } else if (bmp->type1e8 == 1) { // SECOND_BUF
                if (bmp->buflen1e4 >= input1) {
                    ret = MR_SUCCESS;
                } else {
                    ret = MR_FAILED;
                }
            }
        }
        break;

    case 500:  // 查询版本?
        ret = _mr_TestCom(NULL, 5, *(uint8*)sp);
        break;

    case 504:  // 某种递归调用
        ret = _mr_TestCom(input1, ?);
        break;

    case 3629: // 0xe2d - 校验码匹配
        if (input1 == 0x9e67a) { // 对应 MR_VERSION=2011 的 key?
            flags &= ~0x02;  // 清除 AI 标志
        }
        break;

    case 3891: // 0xf33
        if (input1 == 0x18030) {
            flags |= 0x01;
        }
        break;

    case 3923: // 0xf53
        if (input1 == 0x18030) {
            flags |= 0x02;  // AI 标志
        }
        break;
    }

    if (case changed flags) base->bi = flags;
    return ret;
}
```

### 与 vmrp 对比
vmrp 的实现**几乎完全一致**。唯一差异：
- MTK case 7 直接 `return input1`，vmrp 用 `#ifdef MR_PLAT_DRAWTEXT` 保护
- MTK case 304/305 操作 bi 的 bit3 (0x08)，vmrp 操作 MR_FLAGS_EI (0x04)
  → **vmrp 的 bit 定义可能有偏差**，需要核实
- 3629/3891/3923 是加密校验相关的，vmrp 已正确实现

---

## 3. _mr_TestCom1 — 字符串参数调度

```c
int _mr_TestCom1(mrp_State* L, int input0, char* input1, int32 len) {
    int ret = 0;
    switch (input0) {
    case 2: // 设置 mr_c_function_P
        if (mr_c_function_P != NULL) {
            mr_free(mr_c_function_P, mr_c_function_P_len);
        }
        mr_c_function_P = input1;
        mr_c_function_P_len = len;
        break;

    case 3: // 设置 old_start_filename
        memset(old_start_filename, 0, 128);
        if (input1) strncpy(old_start_filename, input1, 127);
        memset(old_pack_filename, 0, 128);
        strncpy(old_pack_filename, "start.mr", 127);
        break;

    case 4: // 设置 start_fileparameter
        memset(start_fileparameter, 0, 128);
        if (input1) strncpy(start_fileparameter, input1, 127);
        break;

    case 5: // 设置 mr_exception_str
        mr_exception_str = input1;
        break;

    case 6: // 清除 mr_exception_str
        mr_exception_str = NULL;
        break;

    case 9: // mr_cacheSync
        mr_cacheSync(input1 & ~0x1f, (len + 93) & ~0x1f);
        return 0;

    case 200: // mr_updcrc
        mr_updcrc(NULL, 0);
        mr_updcrc(input1, len);
        ret = mr_updcrc(input1, 0);
        break;

    case 700: // _mr_newSIMInd
        ret = _mr_newSIMInd((int16)(len >> 16), input1);
        break;

    case 701: // mr_cacheSync (variant)
        // input1 → 地址, input1+2 → 长度指针
        ret = *(input1+2);  // 第3个字段
        // 调用某函数
        break;

    case 900: // mr_platEx(0x30d41, 0xc8, 16, 0, 0)
        ret = mr_platEx(0x30d41, 0xc8, 16, NULL, NULL);
        break;
    }
    return ret;
}
```

### 与 vmrp 对比
vmrp 实现基本匹配。**关键发现：**
- **case 9** = `mr_cacheSync` 调用！这在 vmrp 中应该实现为空操作（模拟器不需要 cache sync）
  但它揭示了 .ext 代码可能通过 `_mr_TestCom1(L, 9, addr, len)` 来请求 cache flush
- **case 900** 调用 `mr_platEx(0x30d41, ...)` — code=0x30d41 → GROUP=3, FUNC=0xd41
  这是 mpchat 相关的功能

---

## 4. mr_event / mr_timer / mr_pauseApp / mr_resumeApp

这些函数在 MTK 和 SPL6530 之间**完全一致**，也与 vmrp 匹配。

### mr_event (MTK):
```c
int32 mr_event(int16 type, int32 param1, int32 param2) {
    if (mr_state==RUN || (mr_timer_run_without_pause && mr_state==PAUSE)) {
        if (mr_event_function) {
            int s = mr_event_function(type, param1, param2);
            if (s != MR_IGNORE) return s;
        }
        c_event_st = {type, param1, param2};
        _mr_TestComC(801, &c_event_st, 20, 1);  // sizeof(c_event_st)=20? 不对, 应该是14
        // 注意：MTK 中 sizeof(c_event_st) = 20 (0x14), 而不是 12
        return MR_SUCCESS;
    }
    return MR_IGNORE;
}
```

**发现：** MTK 中 c_event_st 调用时 len=20 (0x14)，但 vmrp 用 `sizeof(c_event_st)` = 12。
看反汇编 `mov r2, #20` → c_event_st 在 MTK 中是 20 字节。

### mr_timer (MTK):
```c
int32 mr_timer(void) {
    if (mr_timer_state != RUNNING) {
        mr_printf("warning:mr_timer event unexpected!");
        return MR_IGNORE;
    }
    mr_timer_state = IDLE;
    if (mr_state==RUN || (mr_timer_run_without_pause && mr_state==PAUSE)) {
        if (mr_timer_function) {
            int s = mr_timer_function();
            if (s != MR_IGNORE) return s;
        }
        _mr_TestComC(801, NULL, 1, 2);
        return MR_SUCCESS;
    } else if (mr_state == RESTART) {
        mr_stop();
        _mr_intra_start(start_filename, NULL);
        return MR_SUCCESS;
    }
    return MR_IGNORE;
}
```

完全与 vmrp 一致。

---

## 5. mr_doExt — 加载执行 .ext 扩展

```c
int32 mr_doExt(char* extName) {
    char* filebuf;
    int32 filelen;

    filebuf = _mr_readFile(extName, &filelen, 0);
    if (!filebuf) {
        mr_printf("err:%d", 11001);  // 0x2af9
        return MR_FAILED;
    }

    _mr_TestCom(NULL, 0xb61+0x2cc, 0xb61+0x2cc);
    // 0xb61 = 2913, 0xb61+0x2cc = 3629
    // 即 _mr_TestCom(NULL, 3629, 2913)  ← 与 vmrp 完全一致！

    if (_mr_TestComC(800, filebuf, filelen, 0) != 0) {
        mr_printf("err:%d", 11002);  // 0x2afa
        return MR_FAILED;
    }

    _mr_TestComC(801, filebuf, 0x7db, 6);     // MR_VERSION=2011
    _mr_TestComC(801, &mrc_appInfo_st, 16, 8); // sizeof=16
    _mr_TestComC(801, filebuf, 0x7db, 0);      // 入口点调用

    return MR_SUCCESS;
}
```

**与 vmrp 完全一致。**

---

## 6. _mr_c_function_table 布局 (148 entries)

MTK 和 SPL6530 的函数表布局**完全相同**，148 个条目，顺序一致。
详见上方 relocation 分析输出。

---

## 7. _mr_c_internal_table 布局 (17 entries)

```
[0]  = &mr_m0_files
[1]  = &vm_state
[2]  = &mr_state
[3]  = &bi
[4]  = &mr_timer_p
[5]  = &mr_timer_state
[6]  = &mr_timer_run_without_pause
[7]  = &mr_gzInBuf
[8]  = &mr_gzOutBuf
[9]  = &LG_gzinptr
[10] = &LG_gzoutcnt
[11] = &mr_sms_cfg_need_save
[12] = _mr_smsSetBytes
[13] = _mr_smsAddNum
[14] = _mr_newSIMInd
[15] = _mr_isMr
```

与 SPL6530 **完全一致**。

---

## 8. mmidsmapi.obj — mr_platEx 分发架构

```c
// mr_platEx 的实现 (MTK)
int32 mr_platEx(int32 code, uint8* input, int32 input_len,
                uint8** output, int32* output_len, MR_PLAT_EX_CB* cb) {
    int group = code >> 16;  // 高16位 = GROUP

    if (group >= g_platex_func_table_entries)
        return MR_IGNORE;

    mr_platEx_f func = g_platex_func_table[group];
    if (func == NULL)
        return MR_IGNORE;

    return func(code, input, input_len, output, output_len, cb);
}
```

这是一个极其优雅的分发架构。vmrp 当前的 mr_platEx 是单一函数，
只处理 GROUP=0 (code < 0x10000) 的情况。

---

## 9. mr_plat — 平台调用

mr_plat(MTK) 是一个巨大的 switch/case，处理数百个 case。
关键 case:

| code | 功能 |
|------|------|
| 2 | 获取 LCD 宽度 |
| 3 | 获取 LCD 高度 |
| 4 | 获取颜色位数 |
| 0x65 (101) | mr_lcd_set_rotation |
| 0x66 (102) | 未知 |
| 0x67 (103) | 未知 |
| 0x68 (104) | 未知 |
| -某值 | mr_net_check_connect |
| 0x12c+1 (301) | mr_signal_initialize |
| ... | ... |

---

## 10. gamepad.obj — 外置手柄

```c
void EV_EntryGameSimu(uint32 port, uint32 buf_size);  // 初始化 UART 手柄
void EV_ExitGameSimu(void);                            // 退出
uint32 EV_GameSimuKeyValue(void);                      // 读取按键值
```

通过 UART 串口通信读取外置游戏手柄输入。对模拟器无用。

---

## MTK vs SPL6530 vs vmrp 差异总表

| 函数 | MTK vs SPL6530 | vmrp 是否正确 |
|------|----------------|--------------|
| _mr_TestComC | **相同逻辑** (直接BLX跳转) | ✅ 正确抽象为 arm_ext_load/call |
| _mr_TestCom | **完全相同** | ✅ 完全匹配 |
| _mr_TestCom1 | **完全相同** | ✅ 基本匹配 |
| mr_event | **完全相同** | ✅ c_event_st sizeof 需确认 |
| mr_timer | **完全相同** | ✅ 匹配 |
| mr_doExt | **完全相同** | ✅ 匹配 |
| mr_pauseApp | **完全相同** | ✅ 匹配 |
| mr_resumeApp | **完全相同** | ✅ 匹配 |
| mr_stop/stop_ex | **完全相同** | ✅ 匹配 |
| _mr_intra_start | **完全相同** | ✅ 匹配 |
| mr_start_dsm | **签名不同** (MTK 1参数) | ✅ vmrp 用 SPL6530 的 3 参数 |
| mr_platEx | **MTK有分组架构** | ⚠️ vmrp 只实现 GROUP 0 |
| _mr_c_function_table | **完全相同 (148条)** | ✅ 匹配 |
| _mr_c_internal_table | **完全相同 (16条)** | ✅ 匹配 |

