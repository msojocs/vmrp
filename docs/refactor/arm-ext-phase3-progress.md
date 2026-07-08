# Phase 3 hook_table 解体与 call 路径去重 —— 执行记录

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第六节

## P3-A hook_table case 函数化 ✅(93/99)

- 框架(arm_ext_priv.h):`AexTableCtx{idx,r0-r3,ret}` + `AexTableHandler` +
  按编号索引的 `aex_table_handlers[EXT_TABLE_COUNT]`(designated initializer)。
  hook_table 序言(参数读取/trace/busy-wait 维护)后先查表分发,NULL 槽位
  回落残余 switch。
- 自动转换(脚本):选出**不含 for/while/do/switch/goto 的简单 case**共 93 个,
  `break;`→`goto aex_done;`(无循环则 break 必绑定外层 switch,语义无歧义),
  函数体原样迁入 `src/arm_ext/aex_table.c`(1991 行)。
- 残余 switch:6 个复杂 case(0 分配+嵌套加载、3/14 GOT 修复循环、38 platEx、
  44 读文件、125 readFile)+ default(未实现打印+MR_IGNORE)。
  **教训:switch 末尾的 default 块曾被自动切割吞进 case 145 的 handler,
  已剥离并在残余 switch 恢复——分批迁移时 default 必须显式处理。**
- 顺带收敛:94 条 mr_*/DSM extern 收进 `src/include/arm_ext_host.h`
  (priv 头统一引入;将来 Phase 5 多实例化在此收口为接口结构体)。

## P3-B call 路径去重 ✅

- **模态屏幕快照恢复**:arm_ext_call 与 arm_ext_call_dispatch 的逐字重复块
  (约 25 行 ×2)提取为 `arm_ext_restore_modal_screen_snapshot`(aex_screen.c)。
- **宿主 tick 保活四连段**:`mr_timerStart(50)+mr_timer_state=1+host_timer_pending
  +internal_slot_write` 在 arm_ext_call 内 4 处逐字重复,收敛为
  `arm_ext_keep_host_tick_alive`(判活条件各异保持原状,动作统一)。
- **SMS 分发拆分:不再需要**——issues doc 记录的 567 行是 2026-07-06 旧版
  测量,当前 `arm_ext_dispatch_pending_sms_result` 已是 11 行(此前的修复
  已把它简化),无拆分价值。issues doc 该项可销。

## 验证

- vmrp-unit 47 checks ✅;全量 e2e 27/27 两轮(P3-A 后、P3-B 后)✅;
  第一方告警 0 ✅;sanitizer 门(Phase 2+3 合并覆盖):**ASan/UBSan 报告
  0 条** ✅,6 个失败与既定基线同类同集合(像素时序,普通构建全绿)。
- 当前布局:executor 主体 5736 行 + src/arm_ext/ 七单元 5454 行。

## 收尾批(同日续):残余复杂 case 迁移 ✅(99/99)

- 6 个复杂 case(0 分配+嵌套加载、3/14 GOT 修复循环、38 platEx、44 读文件、
  125 readFile)手工迁入 aex_table.c(aex_t000/003/014/038/044/125),
  循环内 break 逐个人工核对(不做机械替换);hook_table 的残余 switch
  删除,NULL 槽位回落 default 语义(打印 + MR_IGNORE)保持不变。
- 迁移所需 executor 符号(GOT 保护判定、read_file 镜像/gzip 槽同步、
  retire/restore 等 15 个)经 priv 头登记;`app_should_protect_got_addr`
  上收为 priv 内联(经 profile 回调)。
- aex_table.c 现 2534 行,handler 表 99 槽全满。

## 后续

- 按域把 aex_table.c 拆成 mem/file/screen/timer/misc 子文件(方案原意,
  择机)。
- Phase 4 启发式治理衔接(指纹迁移按分级清单)。
