# 动感芯片（加速度传感器）接口支持

## 需求与依据

示例游戏 gtdgdq（test/fixtures/gtdgdq.mrp，e2e：test/e2e/gtdgdq/
motion-support.test.ts）启动时经 `mr_plat(4002)` 探测动感芯片；实现前该码走
default 返回 MR_IGNORE，游戏判定不支持，主菜单「新的游戏」后直接进关卡菜
单，无动感提示屏（实证：LEFT_SOFT 后 21 色关卡菜单）。

接口规范来自 SKYENGINE API 参考手册（`D:\temp\mrp\docs\API-v2-md\`，本机
挂载于 /mnt/d/...）：

| 接口 | 语义 | 返回 |
|---|---|---|
| mr_plat(4001) | 停止监听 | MR_SUCCESS |
| mr_plat(4002) | 芯片上电 | MR_SUCCESS |
| mr_plat(4003) | 芯片断电 | MR_SUCCESS |
| mr_plat(4004) | 监听晃动模式 | MR_SUCCESS |
| mr_plat(4005) | 监听倾斜模式 | MR_SUCCESS |
| mr_plat(4006) | 量程查询 | A>1000，范围 ±(A-1000) |

事件上送契约（文档 Remarks）：`mr_event(MR_MOTION_EVENT(18),
MR_MOTION_EVENT_SHAKE(0)|TILT(1), T_MOTION_ACC*)`，其中第三参即现有
mr_event 的 param2 —— **无需扩展事件参数个数**；`T_MOTION_ACC{int32
x,y,z}` 是指针语义。坐标系：平放 +Z 最大、屏幕向左横立 +X 最大、屏幕背向
自己竖立 +Y 最大（《动感芯片接口.md》）。

## 实现

1. **DSM 层状态机**（src/mythroad/dsm.c + dsm.h，真机上这是传感器驱动状
   态；注意 mythroad 源码 `-UVMRP` 编译，不能访问 vmrp_config）：
   `dsmMotionPowered/Listening/Mode` + plat 4001~4006 分支；
   `dsm_motion_listening_mode()`（-1 未监听 / 0 晃动 / 1 倾斜）；
   `DSM_MOTION_ACC_MAX=1000`（4006 返回 2000）；`dsm_init` 归零。
2. **guest 槽位**（arm_ext）：`ArmExtModule.motion_acc_addr` 持久 12 字节
   槽位（懒分配复用，arm_alloc 是 bump 分配器不能按事件分配），
   `arm_ext_host_motion_acc_slot(m,x,y,z)`（arm_ext_executor.c，
   arm_ext_executor.h 导出）写样本返回 guest 地址。
3. **注入链**：`mr_motion_input(x,y,z)`（mythroad.c）查监听状态→写槽位→
   `mr_event(18, mode, slot)`；`vmrp_runtime_motion`（runtime 层）→
   `vmrp_motion_input`（vmrp.c/vmrp.h，宿主入口）。
4. **e2e 通道**：`MOTION x y z` 命令（src/e2e_control.c，复用 SCREEN 的主
   线程回投+条件变量通道，guest 事件入口非线程安全）；回应
   `OK motion delivered|ignored` 区分 guest 是否在监听。hook 经
   main.c `e2e_motion_input_hook` 接入。
5. **Flutter API**（vmrp_api.c/h）：`vmrp_api_motion(x,y,z)`（异步队列
   API_CMD_MOTION）、`vmrp_api_motion_active()`（-1/0/1，嵌入端据此开关平
   台传感器省电）；docs/flutter-integration.md 新增「动感芯片」小节（FFI
   绑定+sensors_plus 示例+轴向换算）。

## 验证

- [x] gtdgdq 探测链闭环：plat(4002)=SUCCESS → 主菜单出现绿字黑底提示
      「您的手机支持动感功能，是否使用？」（2 色）；选确定后游戏调
      plat(4005, param=2)（param 为文档未提及的附加参数，按文档忽略）
- [x] `MOTION 0 0 1000` 上送返回 delivered，游戏正常运行无崩溃
- [x] e2e test/e2e/gtdgdq/motion-support.test.ts 通过（游戏启动很快，
      整测 ~430ms 是真实通过：VMRP_E2E_KEEP_TMP=1 产物截图核验为真实
      bgm-select 与动感提示画面）
- [x] 全量回归 25 文件 47 用例通过。⚠️ 教训：**跑全量 e2e 时不要并行运行
      手动 vmrp 探测进程**——曾因资源竞争造成 11 文件假失败
- [x] 共享库导出 `vmrp_api_motion`/`vmrp_api_motion_active`（nm 验证）

## 遗留观察（不影响本次验收）

进入教程关卡后注入持续 ±700 倾斜样本，栏板未见移动（截图 diff 仅计时器数
字）。反汇编（cfunction.ext，ARM 模式）确认 wrapper 事件链存在 code==18
分支且不拒绝（0x1c94/0x1de0/0x1f98 `cmp #0x12`），事件已进入游戏侧；教程
文字明确要求「按住左右方向」（按键），推测教程阶段按设计只响应按键，动感
生效于正式玩法或需完成教程标定。如需深究，从 0xf0a0 的事件入口
（`cmp r7,#0x12` 后 fall-through 的处理路径）与 plat(4005,2) 的 param=2
语义入手。

## 工具备注

- mrp 提取/反汇编：temp/disasm/extract_mrp.py、disasm_range.py（注意
  disasm_range.py 按 Thumb 解码；cfunction.ext 的 mrc 库段是 **ARM 模式**，
  需用 capstone CS_MODE_ARM）。
- 动感手动注入：e2e socket `MOTION x y z`（先 KEY 流程开启游戏监听）。
