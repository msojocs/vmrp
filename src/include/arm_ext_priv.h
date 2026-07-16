#ifndef __VMRP_ARM_EXT_PRIV_H__
#define __VMRP_ARM_EXT_PRIV_H__

/*
 * ARM EXT 执行器跨编译单元内部 API(Phase 2 拆分,见
 * docs/arm-ext-executor-refactor-plan.md 第五节)。
 *
 * 只允许 src/arm_ext_executor.c 与 src/arm_ext/ 下的拆分单元包含本头;
 * 对外公开接口仍然只有 arm_ext_executor.h。函数从单文件抽出时若被其它
 * 单元调用,在此登记声明并在定义处去掉 static;单元内部私有的仍保持 static
 * 不出现在这里。
 */

#include "arm_ext_internal.h"
#include "arm_ext_host.h"
#include "compat_msvc.h"

/* ---- 微工具(拆分期间上收,多个单元共用) ---- */
static inline uint32_t align4(uint32_t v) { return (v + 3u) & ~3u; }
static inline uint32_t reg_read32(uc_engine *uc, int reg) {
    uint32_t v = 0;
    uc_reg_read(uc, reg, &v);
    return v;
}
static inline void reg_write32(uc_engine *uc, int reg, uint32_t v) {
    uc_reg_write(uc, reg, &v);
}
static inline int arm_ext_addr_range_mapped(ArmExtModule *m,
                                            uint32_t addr,
                                            uint32_t len) {
    if (!m || !addr || !len) return 0;
    if (len - 1u > UINT32_MAX - addr) return 0;
    return arm_ptr(m, addr) && arm_ptr(m, addr + len - 1u) != NULL;
}

/* ---- aex_support.c:无模块状态依赖的纯工具 ---- */

/* 朴素字节序列包含判定(O(n·m),用于 MRP payload 匹配) */
int arm_ext_bytes_contain(const uint8_t *haystack, uint32_t haystack_len,
                          const uint8_t *needle, uint32_t needle_len);

/* guest memcpy/strncpy 桥的重叠区间语义实现(真机朴素前向循环;
 * 背景见 Phase 0 sanitizer 修复记录) */
void arm_ext_guest_memcpy(void *dst, const void *src, size_t n);
char *arm_ext_guest_strncpy(char *dst, const char *src, size_t n);

/* '\\'/'/' 归一化的路径比较 */
int mrp_path_equal(const char *a, const char *b);

/* FNV-1a 32 位内容指纹(P5.2 pack 归属缓存) */
uint32_t arm_ext_fnv1a(const void *data, uint32_t len);

/* 诊断输出的字节预览格式化(可打印字符直出,其余 \xNN) */
void arm_ext_diag_preview_bytes(const void *data, uint32_t len,
                                char *out, size_t out_len);

/* 诊断开关进程级缓存访问器(环境变量运行期不变) */
int arm_ext_diag_on(void);
int arm_ext_trace_on(void);
int arm_ext_trace_pc_on(void);
int arm_ext_timer_liveness_diag_on(void);
int arm_ext_timer_owner_diag_on(void);
int arm_ext_screen_diag_on(void);

/* ---- arm_ext_executor.c 暂存主体中,被拆分单元调用的符号 ---- */

uint32_t arm_alloc(ArmExtModule *m, uint32_t len);
void sync_origin_mem_slots(ArmExtModule *m);
uint32_t read_game_timer_head(ArmExtModule *m, uint32_t grw);
void write_game_timer_head(ArmExtModule *m, uint32_t grw, uint32_t val);
int arm_ext_foreground_child_has_compact_timer_queue(ArmExtModule *m);
int arm_ext_foreground_child_has_frame_timer_queue(ArmExtModule *m);
int arm_ext_primary_has_compact_timer_queue(ArmExtModule *m);
int arm_ext_wrapper_dispatch_is_busy(ArmExtModule *m);
void capture_timer_dispatches(ArmExtModule *m);
void sync_timer_state_from_arm(ArmExtModule *m);
int internal_slot_read(ArmExtModule *m, uint32_t slot, uint32_t *out);
ArmExtNestedModule *arm_ext_find_nested_module_by_p(ArmExtModule *m,
                                                    uint32_t p_addr);
int arm_ext_child_has_compact_timer_walker(const uint8_t *code, uint32_t len);
int arm_ext_find_compact_timer_scheduler(const uint8_t *code, uint32_t len,
                                         uint32_t *scheduler_off,
                                         uint32_t *walker_file_off);
uint32_t arm_ext_active_rw_base(ArmExtModule *m);
int arm_ext_find_first_registered_code_overlap(ArmExtModule *m, uint32_t addr,
                                               uint32_t len,
                                               uint32_t *overlap_lo,
                                               uint32_t *overlap_hi);
int arm_ext_first_unprotected_subrange(ArmExtModule *m, uint32_t node,
                                       uint32_t node_len, uint32_t len,
                                       uint32_t *alloc_addr);

/* ---- aex_mem.c:分配器/compact 堆/不变量 ---- */

uint32_t arm_ext_app_mem_malloc(ArmExtModule *m, uint32_t len);
uint32_t arm_ext_meta_alloc(ArmExtModule *m, uint32_t len);
void arm_ext_protect_registered_module_storage(ArmExtModule *m,
                                               uint32_t file_addr,
                                               uint32_t file_len);
void arm_ext_app_mem_free(ArmExtModule *m, uint32_t p, uint32_t len);
void note_origin_mem_alloc(ArmExtModule *m, uint32_t len);
void note_origin_mem_free(ArmExtModule *m, uint32_t len);
void hook_compact_heap_free_return(uc_engine *uc, uint64_t address,
                                   uint32_t size, void *user_data);
void arm_ext_sanitize_compact_timer_heaps(ArmExtModule *m);
int arm_ext_verify_invariants(ArmExtModule *m, const char *where);
/* P4.3 统一保护集:收集(timer 节点+模块存活存储)→排序合并 */
uint32_t arm_ext_collect_protected_ranges(ArmExtModule *m,
                                          ArmExtBumpBlock *ranges);
uint32_t arm_ext_merge_protect_ranges(ArmExtBumpBlock *r, uint32_t n);
/* 单测直接调用(test/unit),因此跨单元可见 */
int arm_ext_compact_heap_cut_range(ArmExtModule *m, uint32_t ctrl,
                                   uint32_t protect_lo, uint32_t protect_hi);

/* ---- aex_screen.c:屏幕缓冲/脏区/前台归属/位图呈现 ---- */

/* 屏幕上下文(push/pop_draw_screen_context 的保存区,调用方栈上持有) */
typedef struct ArmExtScreenContext {
    uint16 *saved_screen;
    int32 saved_w;
    int32 saved_h;
    uint32_t target_addr;
    int active;
} ArmExtScreenContext;

int32_t arm_ext_screen_stride(ArmExtModule *m);
void arm_ext_copy_screen_to_host(ArmExtModule *m, uint16_t *dst,
                                 const uint16_t *src, int32_t src_stride,
                                 int32_t *out_w, int32_t *out_h);
void arm_ext_clear_foreground_screen_owner(ArmExtModule *m);
int arm_ext_has_foreground_cover(ArmExtModule *m);
void arm_ext_finish_accepted_screen_write(ArmExtModule *m,
                                          uint32_t claim_p_addr,
                                          uint32_t claim_helper_addr);
void arm_ext_note_screen_presented(ArmExtModule *m);
void arm_ext_claim_foreground_screen_rect(ArmExtModule *m,
                                          uint32_t owner_p_addr,
                                          uint32_t owner_helper_addr,
                                          int32_t x, int32_t y,
                                          int32_t w, int32_t h);
void arm_ext_restore_modal_screen_snapshot(ArmExtModule *m);
void hook_screen_dim_write(uc_engine *uc, uc_mem_type type, uint64_t address,
                           int size, int64_t value, void *user_data);
void hook_screen_dim_read(uc_engine *uc, uc_mem_type type, uint64_t address,
                          int size, int64_t value, void *user_data);
void enter_screen_context(ArmExtModule *m, uint16 **saved_screenBuf,
                          uint32_t *saved_present_depth);
void leave_screen_context(ArmExtModule *m, uint16 *saved_screenBuf,
                          uint32_t saved_present_depth);


/* screen 单元其余跨单元符号(diag dump 随物理区间迁入,后续归位 aex_diag) */
int arm_ext_push_draw_screen_context(ArmExtModule *m, ArmExtScreenContext *ctx);
void arm_ext_pop_draw_screen_context(ArmExtScreenContext *ctx);
int arm_ext_should_accept_screen_write(ArmExtModule *m, uint32_t *claim_p_addr, uint32_t *claim_helper_addr);
void arm_ext_finish_screen_cache_write(ArmExtModule *m, const ArmExtScreenContext *ctx, uint32_t claim_p_addr, uint32_t claim_helper_addr);
void arm_ext_diag_dump_layer_state(ArmExtModule *m, const char *tag);
int arm_ext_owner_is_covered_by_foreground(ArmExtModule *m, uint32_t owner_p_addr, uint32_t owner_helper_addr);
void arm_ext_diag_dump_rw_timer_state(ArmExtModule *m, const char *tag, uint32_t rw_base);
uint16_t *arm_ext_snapshot_screen(ArmExtModule *m);
void arm_ext_note_screen_damage_diff(ArmExtModule *m, const uint16_t *before);
void arm_ext_free_row_spans(ArmExtRowSpans *spans);
void arm_ext_diag_dump_wrapper_compact_timer_nodes(ArmExtModule *m, const char *tag);
void arm_ext_diag_dump_primary_compact_timer_nodes(ArmExtModule *m, const char *tag);
void arm_ext_claim_foreground_screen_diff(ArmExtModule *m, uint32_t owner_p_addr, uint32_t owner_helper_addr, const uint16_t *before);
int arm_ext_should_accept_visible_present(ArmExtModule *m, uint32_t *claim_p_addr, uint32_t *claim_helper_addr);
int arm_ext_screen_context_targets_primary(ArmExtModule *m, const ArmExtScreenContext *ctx);
int arm_ext_owner_is_foreground_child(ArmExtModule *m, uint32_t owner_p_addr, uint32_t owner_helper_addr);
void arm_ext_note_screen_damage_rect(ArmExtModule *m, int32_t x, int32_t y, int32_t w, int32_t h);
int arm_ext_ensure_foreground_cover_regions(ArmExtModule *m);
void arm_ext_note_foreground_cover_rect(ArmExtModule *m, int32_t x, int32_t y, int32_t w, int32_t h);
void arm_ext_diag_dump_wrapper_timer_state(ArmExtModule *m, const char *tag);
int arm_ext_suspend_depth_for_p(ArmExtModule *m, uint32_t p_addr, uint32_t *suspend_depth);
int arm_ext_present_bitmap_rect(ArmExtModule *m, uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h, int32_t source_stride, int32_t source_x, int32_t source_y, int covered_by_foreground);
void arm_ext_note_screen_damage_addr_range(ArmExtModule *m, uint64_t address, int size);
void arm_ext_mirror_draw_bitmap_to_screen(ArmExtModule *m, uint32_t bmp_addr, int16_t x, int16_t y, uint16_t w, uint16_t h, int32_t source_stride, int32_t source_x, int32_t source_y);
void arm_ext_draw_bitmap_from_guest(ArmExtModule *m,
                                    const ArmExtScreenContext *screen_ctx,
                                    uint32_t p_addr, int16_t x, int16_t y,
                                    uint16_t w, uint16_t h, uint16_t rop,
                                    uint16_t transcoler, int16_t sx,
                                    int16_t sy, int16_t mw);
int arm_ext_dispup_rect(ArmExtModule *m, int32_t x, int32_t y, int32_t w, int32_t h, int covered_by_foreground);
int arm_ext_bitmap_source_uses_screen_stride(ArmExtModule *m, uint32_t bmp_addr);

/* executor 主体中被 screen 单元调用 */
uint32_t arm_ext_p_for_code_addr(ArmExtModule *m, uint32_t addr, uint32_t *helper_addr);
int arm_ext_has_foreground_child(ArmExtModule *m);
int arm_ext_wrapper_has_timer_queue(ArmExtModule *m);
ArmExtNestedModule *arm_ext_find_nested_module_by_rw(ArmExtModule *m, uint32_t rw_base);

/* ---- aex_guard.c:table return 守卫 ---- */

void arm_ext_note_table_return_guard(ArmExtModule *m, uint32_t lr, uint32_t sp);
int arm_ext_guard_table_return_block(uc_engine *uc, ArmExtModule *m,
                                     uint32_t address);

#define MRP_IS_FILE 1
#define ARM_EXT_MR_SMS_RESULT_FLAG 16

/* executor 主体中被 table handler 调用的符号(自动登记) */
uint32_t alloc_bytes(ArmExtModule *m, const void *value, uint32_t len);
uint32_t arm_addr(ArmExtModule *m, const void *ptr);
void arm_ext_apply_short_pack_alias_for_private_child(ArmExtModule *m, uint32_t file_addr, uint32_t file_len, uint32_t p_addr);
void arm_ext_diag_fd_clear(ArmExtModule *m, int32_t handle);
const char *arm_ext_diag_fd_name(ArmExtModule *m, int32_t handle);
void arm_ext_diag_fd_set(ArmExtModule *m, int32_t handle, const char *name);
/* VMRP_ARM_EXT_WATCH_PC: 模块相对文件偏移观察点,嵌套模块注册时安装(aex_diag.c) */
void arm_ext_install_pc_watches(ArmExtModule *m, uint32_t base, uint32_t len);
void arm_ext_drop_overlapping_stale_nested_modules(ArmExtModule *m, uint32_t file_addr, uint32_t file_len);
int arm_ext_has_internal_loader_chunk(ArmExtModule *m, uint32_t file_addr, uint32_t file_len);
const char *arm_ext_pack_to_host_path(ArmExtModule *m, const char *pack);
const char *arm_ext_register_short_pack_alias(ArmExtModule *m,
                                              const char *host_path);
void arm_ext_record_nested_module(ArmExtModule *m, uint32_t file_addr, uint32_t file_len, uint32_t p_addr, uint32_t helper_addr);
void arm_ext_record_timer_owner(ArmExtModule *m);
void arm_ext_repair_private_child_record_bridges(ArmExtModule *m, uint32_t record, uint32_t file_addr, uint32_t file_len);
ArmExtNestedModule *arm_ext_resource_owner_for_lr(ArmExtModule *m, uint32_t *owner_p, uint32_t *owner_helper);
uint32_t arm_ext_read_file_table_context(ArmExtModule *m,
                                         ArmExtNestedModule *owner);
int arm_ext_sync_internal_nested_module(ArmExtModule *m, uint32_t file_addr, uint32_t file_len);
void internal_slot_write(ArmExtModule *m, uint32_t slot, uint32_t value);
void arm_ext_diag_owner_for_lr(ArmExtModule *m, uint32_t *owner_p, uint32_t *owner_h, uint32_t *owner_file, uint32_t *owner_len);

/* 复杂 case(0/3/14/38/44/125)迁移所需的 executor 符号 */
static inline int app_should_protect_got_addr(ArmExtModule *m, uint32_t addr) {
    if (m && m->profile && m->profile->should_protect_got_addr)
        return m->profile->should_protect_got_addr(m, m->app_state, addr);
    return 0;
}
int arm_ext_child_has_package_owner(ArmExtModule *m, ArmExtNestedModule *owner);
int arm_ext_mirror_read_file_to_adjacent_slot(ArmExtModule *m, uint32_t len_slot, const void *data, uint32_t len, uint32_t ret_addr, uint32_t *slot_out);
int arm_ext_range_contains(uint32_t outer, uint32_t outer_len, uint32_t inner, uint32_t inner_len);
void *arm_ext_read_child_resource(ArmExtModule *m, ArmExtNestedModule *owner, const char *read_name, int *file_len, int lookfor, char *read_pack_host_path, uint32_t *read_pack_ram_addr, uint32_t *read_pack_ram_len);
void arm_ext_release_host_read_file_buffer(const void *hp, uint32_t len);
void arm_ext_restore_primary_mapping_after_dump0(ArmExtModule *m, uint32_t read_addr, uint32_t read_len);
void arm_ext_retire_modules_overwritten_by_data_read(ArmExtModule *m, uint32_t read_addr, uint32_t read_len);
int arm_ext_should_skip_got_snapshot_restore(ArmExtModule *m, uint32_t addr);
void arm_ext_sync_read_file_gzip_slots(ArmExtModule *m, const void *read_file_ret, uint32_t arm_ret);

/* ---- aex_table.c:mr_table 系统调用分发(Phase 3 case 函数化) ---- */

/* 单次 table 调用的上下文:r0-r3 为 guest 参数,ret 为回写 R0 的返回值。
 * handler 只读参数、写 ret;寄存器/内存副作用仍经 m。 */
typedef struct AexTableCtx {
    uint32_t idx;
    uint32_t r0, r1, r2, r3;
    uint32_t ret;
} AexTableCtx;

typedef void (*AexTableHandler)(ArmExtModule *m, AexTableCtx *c);

/* 按 table 编号索引的 handler 表;NULL 槽位回落到 hook_table 的残余
 * switch(未转换 case,逐批迁移直至 switch 清空)。 */
extern const AexTableHandler aex_table_handlers[EXT_TABLE_COUNT];

/* ---- aex_exec.c:执行引擎与 guest 字符串 ---- */

void set_arm_mode_for_addr(ArmExtModule *m, uint32_t addr);
uint32_t arg_read(ArmExtModule *m, unsigned n);
void trace_pc(uc_engine *uc, uint64_t address, uint32_t size, void *user_data);
void hook_intr(uc_engine *uc, uint32_t intno, void *user_data);
int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp);
int run_arm(ArmExtModule *m, uint32_t start);
void restore_ext_r9(ArmExtModule *m);
char *arm_str(ArmExtModule *m, uint32_t addr);
int format_arm(ArmExtModule *m, char *dst, size_t dst_size, const char *fmt,
               unsigned first_arg);
int arm_ext_nested_exec_range_for_lr(ArmExtModule *m, uint32_t lr,
                                     uint32_t *file_addr, uint32_t *file_len);

/* ---- aex_module.c:模块注册/生命周期/指纹扫描 ---- */

ArmExtNestedModule *arm_ext_find_nested_module(ArmExtModule *m, uint32_t addr);
int arm_ext_has_suspended_foreground_child(ArmExtModule *m,
                                           uint32_t primary_ext_chunk);
void arm_ext_init_pack_names(ArmExtModule *m);
uint32_t find_wrapper_compact_heap_free_return(const uint8_t *code,
                                               uint32_t len,
                                               uint32_t *ctrl_off_out);
uint32_t find_wrapper_timer_dispatch(const uint8_t *code, uint32_t len,
                                     uint32_t *chain_thunk_out,
                                     uint32_t *sched_off_out);
void arm_ext_set_pack_table_name(ArmExtModule *m, const char *name);
void arm_ext_set_child_record_pack_name(ArmExtModule *m, uint32_t file_addr,
                                        const char *name);

/* ---- aex_pack.c:MRP 包缓存与虚拟 fd ---- */

/* 解析宿主 MRP 文件,把全部条目(解压后)缓存进 m->mrp_cache */
void parse_mrp_cache(ArmExtModule *m, const char *mrp_path);
/* 按名(含 basename 退化匹配)查缓存条目 */
MrpCacheEntry *mrp_cache_find(ArmExtModule *m, const char *filename);
/* 分配/查询只读虚拟 fd(MRP_VFD_BASE 起) */
uint32_t mrp_vfd_open(ArmExtModule *m, const uint8_t *data, uint32_t len);
MrpVirtualFd *mrp_vfd_get(ArmExtModule *m, uint32_t fd);
void mrp_cache_free(ArmExtModule *m);

#endif
