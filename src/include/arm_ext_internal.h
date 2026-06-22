#ifndef __VMRP_ARM_EXT_INTERNAL_H__
#define __VMRP_ARM_EXT_INTERNAL_H__

#include "app_compat.h"
#include <unicorn/unicorn.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define EXT_BASE_ADDR 0x00010000u
#define EXT_MEM_SIZE  (16u * 1024u * 1024u)
#define EXT_TABLE_ADDR EXT_BASE_ADDR
#define EXT_TABLE_COUNT 150u
#define EXT_STOP_ADDR 0x0007FFF0u
#define EXT_HEAP_ADDR 0x00200000u
#define EXT_STACK_ADDR 0x00E00000u
#define EXT_STACK_SIZE (512u * 1024u)
#define EXT_CODE_ADDR  0x00E80000u
#define EXT_WRAPPER_STACK_SIZE 0x20000u
#define EXT_CHUNK_MAGIC 0x7FD854EBu
#define EXT_LOW_TABLE_SIZE 0x2000u
#define EXT_TRACE_PC_RING 64u
#define GAME_TIMER_HEAD_OFFSET 0x008Cu

#define MRP_CACHE_MAX 16
#define MRP_VFD_MAX 4
#define MRP_VFD_BASE 0x7FFF0000u
#define ARM_EXT_NESTED_MODULE_MAX 64

typedef struct mr_c_function_P_t {
    uint32 start_of_ER_RW;
    uint32 ER_RW_Length;
    int32 ext_type;
    uint32 mrc_extChunk;
    int32 stack;
} mr_c_function_P_t;

typedef struct MrpCacheEntry {
    char name[128];
    uint8_t *data;
    uint32_t data_len;
} MrpCacheEntry;

typedef struct MrpVirtualFd {
    int in_use;
    const uint8_t *data;
    uint32_t data_len;
    uint32_t pos;
} MrpVirtualFd;

typedef struct ArmExtNestedModule {
    uint32_t file_addr;
    uint32_t file_len;
    uint32_t p_addr;
    uint32_t helper_addr;
    /* 观测到的该模块 GOT 桥块中 memcpy(table[3]=EXT_TABLE_ADDR+0xC) 桥所在的
     * R9 相对偏移（ARM 重定位写入该桥时记录，0=未观测）。私有 loader 子模块的
     * GOT 桥块基址随模块链接结果而变，写死会错位；用同族已自重定位实例观测到的
     * 真实偏移来平移 bridge 修复描述符，避免把桥写到模块代码实际不读取的位置。 */
    uint32_t got_memcpy_off;
} ArmExtNestedModule;

typedef struct ArmExtRowSpans {
    uint16_t *min_x;
    uint16_t *max_x;
    uint32_t rows;
} ArmExtRowSpans;

struct ArmExtModule {
    uc_engine *uc;
    uint8_t *mem;
    uint8_t *low_table;
    uint32_t heap_top;
    uint32_t code_len;
    const uint8_t *host_code;
    uint32_t helper_addr;
    uint32_t p_addr;
    uint32_t screen_addr;
    uint32_t screen_len;
    uint32_t char_bitmap_addr;
    int32_t screen_w;
    int32_t screen_h;
    uint32_t origin_mem_addr;
    uint32_t origin_mem_len;
    uint32_t origin_mem_left;
    uint32_t origin_mem_min;
    uint32_t origin_mem_top;
    /* MR_MALLOC_SCRRAM/MR_FREE_SCRRAM return ARM-addressable scratch RAM.
     * Native code uses that RAM as ordinary pixel/data storage, so the host
     * must keep the allocation inside Unicorn's mapped address space. */
    uint32_t exram_addr;
    uint32_t exram_len;
    uint32_t internal_table_addr;
    uint32_t port_table_addr;
    uint32_t mr_m0_files_addr;
    uint32_t vm_state_slot;
    uint32_t mr_state_slot;
    uint32_t bi_slot;
    uint32_t mr_timer_p_slot;
    uint32_t mr_timer_state_slot;
    uint32_t mr_timer_run_without_pause_slot;
    uint32_t mr_gzin_buf_slot;
    uint32_t mr_gzout_buf_slot;
    uint32_t lg_gzinptr_slot;
    uint32_t lg_gzoutcnt_slot;
    uint32_t mr_sms_cfg_need_save_slot;
    uint32_t last_file_addr;
    uint32_t last_file_len;
    uint8_t *last_file_copy;
    uint32_t last_alloc_addr;
    uint32_t last_alloc_len;
    uint32_t nested_p_addr;
    uint32_t active_p_addr;
    uint32_t active_helper_addr;
    uint32_t foreground_screen_owner_p_addr;
    uint32_t foreground_screen_owner_helper_addr;
    ArmExtRowSpans foreground_cover;
    uint32_t current_p_addr;
    uint32_t current_helper_addr;
    uint32_t primary_p_addr;
    uint32_t primary_helper_addr;
    uint32_t primary_file_addr;
    uint32_t primary_file_len;
    int primary_host_init_pending;
    uint32_t wrapper_timer_dispatch_addr;
    ArmExtNestedModule nested_modules[ARM_EXT_NESTED_MODULE_MAX];
    int nested_module_count;
    uint32_t outer_r9;
    uint32_t nested_return_addr;
    uint32_t screen_write_count;
    uint32_t thumb_fix_count;
    uint32_t pc_ring[EXT_TRACE_PC_RING];
    uint32_t cpsr_ring[EXT_TRACE_PC_RING];
    uint32_t pc_ring_pos;
    uint32_t busy_wait_count;
    uint32_t busy_wait_start_ms;
    int nested_loading;
    int screen_dirty;
    int screen_presented_in_callback;
    uint32_t screen_present_depth;
    ArmExtRowSpans screen_damage;
    ArmExtRowSpans screen_present;
    int mem_is_mmap;
    uint32_t got_snapshot_base;
    uint32_t got_snapshot[EXT_TABLE_COUNT];
    uint32_t dispatch_timer_start;
    uint32_t dispatch_timer_stop;
    uint32_t timer_p_addr;
    uint32_t timer_helper_addr;
    uint32_t pending_internal_file_addr;
    uint32_t pending_internal_file_len;
    int primary_child_reopen_timer_needed;
    int host_timer_pending;
    int in_dispatch;
    uint32_t saved_game_timer_head;
    uint8_t *modal_screen_snapshot;
    uint32_t modal_screen_snapshot_len;
    int modal_screen_snapshot_valid;
    /* 模态框（wrapper suspend）进入前 wrapper RW 前台分发区的快照。
     * wrapper 在 RW 内维护一张前台模块分发表（事件先派发到该表项指向的
     * 模块 helper）。进入下载提示等模态子模块时，wrapper 的私有 loader 会把
     * 子模块写进该表并改写若干 helper 槽；模拟器用 reset_child_modules 仅做
     * 模块级清理，并不会像真机那样在子模块退出时让 wrapper 还原这张表，
     * 导致返回后事件仍被派发给已关闭的子模块（dota 浏览器插件下载返回后无法
     * 二次进入即此因）。进入模态前快照、关闭时还原该区域即可恢复事件路由。
     * 仅覆盖前台分发表与 helper 槽所在的小段 RW，避免影响其它状态。 */
    uint8_t *modal_fg_snapshot;
    uint32_t modal_fg_snapshot_rw;
    int modal_fg_snapshot_valid;
    uc_hook hook;
    MrpCacheEntry mrp_cache[MRP_CACHE_MAX];
    int mrp_cache_count;
    MrpVirtualFd mrp_vfds[MRP_VFD_MAX];

    const AppCompatProfile *profile;
    void *app_state;
};

static inline void *arm_ptr(ArmExtModule *m, uint32_t addr) {
    if (addr < EXT_BASE_ADDR || addr >= EXT_BASE_ADDR + EXT_MEM_SIZE) return NULL;
    return m->mem + (addr - EXT_BASE_ADDR);
}

static inline uint32_t arm_ext_read_u32_or_zero_(ArmExtModule *m, uint32_t addr) {
    uint32_t value = 0;
    if (m && addr && arm_ptr(m, addr))
        memcpy(&value, arm_ptr(m, addr), 4);
    return value;
}

static inline uint8_t arm_ext_read_u8_or_zero_(ArmExtModule *m, uint32_t addr) {
    uint8_t value = 0;
    if (m && addr && arm_ptr(m, addr))
        memcpy(&value, arm_ptr(m, addr), 1);
    return value;
}

static inline uint32_t arm_ext_wrapper_rw_base_(ArmExtModule *m) {
    uint32_t wrapper_rw = 0;
    if (m && m->p_addr && arm_ptr(m, m->p_addr))
        memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
    return wrapper_rw;
}

static inline uint32_t arm_ext_primary_rw_base_(ArmExtModule *m) {
    uint32_t rw = 0;
    if (m && m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&rw, arm_ptr(m, m->primary_p_addr), 4);
    return rw;
}

extern int32 mr_timer_state;
extern int32 mr_timerStart(uint16 t);

#endif
