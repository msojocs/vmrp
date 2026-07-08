# ARM EXT 执行器多实例化设计(P5.1,实施择机)

日期:2026-07-08
方案:`docs/arm-ext-executor-refactor-plan.md` 第八节
状态:**设计定稿,实施未开始**(收益方为 wasm 多实例与并行单测,当前无阻塞需求)

## 现状

- 执行器对宿主的符号面已在 Phase 3 收口为 `src/include/arm_ext_host.h`
  (94 条 extern:函数 + 全局变量)。
- 阻碍多实例的是其中的**可变全局变量**(函数无状态,可共享):

| 全局 | 语义 | 多实例冲突点 |
|---|---|---|
| `mr_screenBuf` / `mr_screen_w/h` | 宿主屏幕缓冲与几何 | 每实例一屏 |
| `LG_mem_base` / `LG_mem_end` | guest 堆宿主映射 | 每实例一堆 |
| `mr_gzInBuf` / `mr_gzOutBuf` / `LG_gzinptr` / `LG_gzoutcnt` | mythroad gzip 工作区 | 解压不可重入 |
| `mr_state` / `mr_timer_state` | DSM 生命周期/定时器状态 | 每实例一份 |
| unit_stubs.c 同名桩 | 单测进程内共享 | 并行单测互踩 |

另有执行器内部残留:`aex_support.c` 诊断开关缓存(进程级,只读,无冲突)、
`arm_str` 空串静态缓冲(只读,无冲突)。

## 目标形态

```c
/* arm_ext_host.h */
typedef struct ArmExtHostIface {
    uint16 **screen_buf;      /* &mr_screenBuf */
    int32 *screen_w, *screen_h;
    char **lg_mem_base, **lg_mem_end;
    uint8 **gz_in_buf, **gz_out_buf;
    unsigned *gz_in_ptr, *gz_out_cnt;
    int32 *state, *timer_state;
    /* 函数指针不做(直接链接即可);gzip 工作区须配 per-iface 互斥或
     * 改走实例私有缓冲 */
} ArmExtHostIface;
```
- `ArmExtModule` 增加 `const ArmExtHostIface *host;`,`arm_ext_load` 新变体
  `arm_ext_load_with_host(...)` 传入;现有 `arm_ext_load` 内部用指向真实全局
  的默认单例,**对外 ABI 与行为完全不变**。
- 执行器内部所有对上表全局的直接引用改为 `m->host->…` 解引用。

## 迁移步骤(每步独立提交+四道门)

1. 定义 `ArmExtHostIface` + 默认单例(指向现有全局),`ArmExtModule.host`
   在 load 时固定指向单例——**零行为变化**,纯脚手架。
2. 按单元逐个替换直接全局引用(`grep -n 'mr_screenBuf\|mr_screen_w' src/arm_ext*`
   驱动,预计 aex_screen 最多);每单元一提交。
3. mythroad gzip 工作区去共享:readFile 解压路径改用实例私有缓冲
   (或在 iface 上放互斥)——这是唯一有真实并发语义的项,放最后。
4. 单测:unit_stubs 提供第二个 iface 实例,验证两个 ArmExtModule 并存
   (make_module ×2 分配互不干扰)。
5. wasm/共享库入口暴露多实例 API(与 vmrp_api.c 协调,另行设计)。

## 风险

- 热路径解引用多一跳:screen present 与 mem 分配是热点,步骤 2 完成后跑
  一次 e2e 时长对比(基线 ~303s),劣化 >2% 则对热字段做 load 时快照。
- mythroad 层自身的全局(mr_state 等)语义是"每 DSM 一份";多实例的完整
  故事需要 mythroad 层配合(超出 executor 范围,见 CLAUDE.MD 边界约束),
  本设计只解 executor 侧引用方向。

## 不做的事

- 不把 94 条 extern 函数也搬进 iface(无状态,直接链接);
- 不在本阶段引入线程并发(主线程串行约束不变,见 CLAUDE.MD)。
