# Phase 1 Bug 修复与低风险清理 —— 执行记录

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第四节;问题清单:`docs/arm_ext_executor-issues.md`

## P1-A 真实 bug 修复 ✅

- **B1 屏幕回拷越界**:新增 `arm_ext_copy_screen_to_host`(按宿主
  mr_screen_w/h 容量与 guest 逻辑画布尺寸的交集逐行拷贝,返回实际宽高供
  present 夹断),替换三处各自整块拷贝:`leave_screen_context`、
  `arm_ext_call` 与 `arm_ext_call_dispatch` 的模态快照恢复。gtcm 480x320
  逻辑画布用例回归通过。
- **B2 arm_ptr NULL 解引用**:`arm_ext_internal.h` 新增 `arm_ptr_span(m,addr,need)`
  (长度校验+跨映射区校验)。加固点:hook_table case 2(realloc 拷贝)、
  3/4(memcpy/memmove)、5-8(str 族,按实际写入长度校验)、9/13/14
  (memcmp/memchr/memset)、17(format 写回)、34/35/80(结构体输出,失败
  返回 MR_FAILED)、44(真实 mr_read 分支,此前完全无保护)、
  `write_game_timer_head`(写侧对齐读侧的判空)。
  策略:指针无效时跳过宿主操作、返回语义不变——只消除宿主段错误,不发明
  guest 可见新行为。
- **B4**:`arm_ext_host_cache_sync` 改按实际拷入的 copy_len 解析 nested 模块。
- **B5**:case 30 char_bitmap 超 32 字节时夹断+无条件告警(此前隐式依赖字体上限)。
- **B6**:`arm_ext_call` 的 outp/outl 分配失败返回 MR_FAILED;init_table 屏幕
  分配失败无条件 FATAL 告警;nested 注册表满从 TRACE 门控改为无条件告警
  (该状态=事件会派发给错误 owner);`arm_ext_bump_reuse` 改为先登记 live
  再改空闲表(消除宿主 OOM 时块永久丢失)。

## P1-B 死代码与临时诊断清理 ✅

- **D1**:删除 `arm_ext_call_dispatch` 恒零的 depth_patched 恢复块;
  `arm_ext_guest_mem_malloc` 不可达的循环尾链表推进;`leave_screen_context`
  恒真分支(随 B1 合并消除);`arm_ext_dispup_rect` 两分支同值的三目
  (MR_SUCCESS==0,统一返回 MR_SUCCESS)。
- **D2**:整体删除 watch 临时诊断子系统(注释自标"修复后删除"):
  `hook_watch_write`/`arm_ext_watch_sentinel_check`/`arm_ext_watch_module_event`/
  `arm_ext_watch_alloc_report`/hash/hex 工具、`ArmExtWatchState slots[8]`
  (含 used 只增不减+悬空 m 的 ABA 隐患)、VMRP_WATCH_WRITE/SENTINEL/ALLOC
  环境变量入口、table[125] 内 3 处 WATCH_READFILE 取证块及 hook 安装点。
  对应历史 bug(talkcat 代码区覆写等)已由通用修复+不变量检查器覆盖。

## P1-C 诊断开关缓存 + 魔数常量化 ✅(纯机械,行为不变)

- 5 个 VMRP_ARM_EXT_* 开关(DIAG 45 处/TRACE 19 处/TRACE_PC/TIMER_LIVENESS/
  SCREEN_DIAG)统一改经进程级缓存访问器(`arm_ext_diag_on()` 等),热路径
  (hook_table 每次 table 调用)不再反复 getenv。
- `arm_ext_internal.h` 新增命名偏移宏并全量替换字面量:
  - extChunk:`AEX_CHUNK_*`(INIT/HELPER/FILE_BASE/FILE_LEN/RW_BASE/RW_LEN/
    P_ADDR/P_LEN/EVENT_DATA/EVENT_FUNC/RECORD/SUSPEND(0x34)/HEAP_TOP);
  - P 结构:`AEX_P_ER_RW_OFF`/`ER_RW_LEN`/`EXT_TYPE`/`EXT_CHUNK`(+12 共 16 处);
  - compact 堆 ctrl:`AEX_COMPACT_CTRL_BASE/FREE/END/HEAD`。
  布局与证据来源见 `docs/arm-ext-abi.md`。ext_chunk+0x37 保留字面量
  (仅边界探测,非字段)。

## 验证

- 每批之后:`skyengine-unit` 47 checks 全过 + 全量 e2e 16 文件/27 用例全绿
  (P1-A、P1-B、P1-C 三轮)。
- 第一方编译告警保持 0。
- sanitizer 门:ASan/UBSan **报告 0 条**(达标)。5 个像素时序失败与 Phase 0
  基线同集合(减速固定延时类,普通构建全绿),符合既定验证纪律。

## 未在本阶段处理(移交)

- B7(bridge_dsm_network_cb 截断 host 函数指针):需改 bridge 接口签名,
  与 native/共享库路径联动,单独安排。
- issues doc 第四节 30+ 应用指纹函数:Phase 4 启发式治理统一处理。
- hook_table/arm_ext_call 超大函数拆分:Phase 2/3。
