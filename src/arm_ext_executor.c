#include "./include/arm_ext_executor.h"
#include "./include/compat_msvc.h"
#include "./include/arm_ext_internal.h"
#include "./include/app_compat.h"
#include "./include/network.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/mman.h>
#else
#include <windows.h>
#endif
#include <zlib.h>

#include "./include/utils.h"
#include "./include/vmrp.h"

extern void mr_printf(const char *format, ...);
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_rand(void);
extern void mr_free(void *p, uint32 len);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern int32 mr_state;
extern int32 mr_timer_state;
extern int32 mr_stop_ex(int16 freemem);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_getUserInfo(void *info);
extern void mr_md5_init(void *pms);
extern void mr_md5_append(void *pms, const void *data, int nbytes);
extern void mr_md5_finish(void *pms, void *digest);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern const char *mr_get_pack_filename(void);
extern const char *mr_get_start_filename(void);
extern const char *mr_get_old_pack_filename(void);
extern const char *mr_get_old_start_filename(void);
extern const char *mr_get_start_fileparameter(void);
typedef void (*MR_PLAT_EX_CB)(uint8 *output, int32 output_len);
extern int32 mr_platEx(int32 code, uint8 *input, int32 input_len,
                       uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb);
extern int32 mr_ferrno(void);
extern int32 mr_open(const char *filename, uint32 mode);
extern int32 mr_close(int32 f);
extern int32 mr_info(const char *filename);
extern int32 mr_write(int32 f, void *p, uint32 l);
extern int32 mr_read(int32 f, void *p, uint32 l);
extern int32 mr_seek(int32 f, int32 pos, int method);
extern int32 mr_getLen(const char *filename);
extern int32 mr_remove(const char *filename);
extern int32 mr_rename(const char *oldname, const char *newname);
extern int32 mr_mkDir(const char *name);
extern int32 mr_rmDir(const char *name);
extern int32 mr_findStart(const char *name, char *buffer, uint32 len);
extern int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len);
extern int32 mr_findStop(int32 search_handle);
extern int32 mr_exit(void);
extern int32 mr_startShake(int32 ms);
extern int32 mr_stopShake(void);
extern int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 mr_stopSound(int type);
extern void mr_call(char *number);
extern int32 mr_sendSms(char *pNumber, char *pContent, int32 encode);
extern int32 mr_dialogCreate(const char *title, const char *text, int32 type);
extern int32 mr_dialogRelease(int32 dialog);
extern int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type);
extern int32 mr_textCreate(const char *title, const char *text, int32 type);
extern int32 mr_textRelease(int32 text);
extern int32 mr_textRefresh(int32 handle, const char *title, const char *text);
extern int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size);
extern int32 mr_editRelease(int32 edit);
extern const char *mr_editGetText(int32 edit);
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
extern int32 mr_getScreenInfo(void *s);
extern void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);
extern int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern uint16 *c2u(const char *cp, int *err, int *size);
extern int wstrlen(char *txt);

/* mrporting.h 中定义的 mr_info 返回值，此处直接使用数值避免引入额外头文件 */
#define MRP_IS_FILE 1

extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len);
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int _mr_TestCom(void *L, int input0, int input1);
extern int _mr_TestCom1(void *L, int input0, char *input1, int32 len);
extern int32 mr_socket(int32 type, int32 protocol);
extern int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 mr_closeSocket(int32 s);
extern int32 mr_getSocketState(int32 s);
typedef int32 (*MR_INIT_NETWORK_CB_t)(int32 result);
extern int32 mr_initNetwork(MR_INIT_NETWORK_CB_t cb, const char *mode);
extern int32 mr_recv(int32 s, char *buf, int len);
extern int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 mr_send(int32 s, const char *buf, int len);
extern int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);

static uint32_t arm_addr(ArmExtModule *m, const void *ptr) {
    if (ptr == NULL) return 0;
    return (uint32_t)((const uint8_t *)ptr - m->mem) + EXT_BASE_ADDR;
}

static uint32_t align4(uint32_t v) { return (v + 3u) & ~3u; }

static uint32_t arm_alloc(ArmExtModule *m, uint32_t len) {
    len = align4(len ? len : 1);
    if (m->heap_top + len >= EXT_BASE_ADDR + EXT_MEM_SIZE) return 0;
    uint32_t ret = m->heap_top;
    m->heap_top += len;
    return ret;
}

static uint32_t reg_read32(uc_engine *uc, int reg) {
    uint32_t v = 0;
    uc_reg_read(uc, reg, &v);
    return v;
}

static int arm_ext_finish_callback_state(ArmExtModule *m, uint32_t ext_chunk);
static void arm_ext_clear_foreground_screen_owner(ArmExtModule *m);
static void capture_timer_dispatches(ArmExtModule *m);

static inline void app_on_child_confirmed(ArmExtModule *m, uint32_t p, uint32_t h) {
    if (m && m->profile && m->profile->on_child_confirmed)
        m->profile->on_child_confirmed(m, m->app_state, p, h);
}
static inline int app_should_protect_got_addr(ArmExtModule *m, uint32_t addr) {
    if (m && m->profile && m->profile->should_protect_got_addr)
        return m->profile->should_protect_got_addr(m, m->app_state, addr);
    return 0;
}


static void reg_write32(uc_engine *uc, int reg, uint32_t v) {
    uc_reg_write(uc, reg, &v);
}

static void set_arm_mode_for_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
    if (addr & 1u) {
        cpsr |= (1u << 5);
    } else {
        cpsr &= ~(1u << 5);
    }
    reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
}

static uint32_t arm_exec_addr(uint32_t addr) {
    // Keep bit 0 so Unicorn enters Thumb mode for Thumb entry points.
    return addr;
}

static ArmExtNestedModule *arm_ext_find_nested_module(ArmExtModule *m, uint32_t addr) {
    if (!m) return NULL;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->file_addr && mod->file_len &&
            addr >= mod->file_addr && addr < mod->file_addr + mod->file_len) {
            return mod;
        }
    }
    return NULL;
}

/* 在所有覆盖 addr 且记录了 GOT memcpy 偏移(got_memcpy_off)的嵌套模块里，返回最近
 * 登记(最高索引)的那一个。用于在 bridge 修复时，从与被修复模块代码区间重叠的"已自
 * 重定位同族实例"借用真实的 GOT 桥块偏移。 */
static ArmExtNestedModule *arm_ext_find_got_donor(ArmExtModule *m, uint32_t lo, uint32_t hi) {
    if (!m) return NULL;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (!mod->got_memcpy_off) continue;
        if (!mod->file_addr || !mod->file_len) continue;
        uint32_t mlo = mod->file_addr, mhi = mod->file_addr + mod->file_len;
        if (mlo < hi && lo < mhi) return mod;   /* 区间重叠 */
    }
    return NULL;
}

static void arm_ext_record_nested_module(ArmExtModule *m, uint32_t file_addr,
                                         uint32_t file_len, uint32_t p_addr,
                                         uint32_t helper_addr) {
    if (!m || !file_addr || !file_len || !p_addr || !helper_addr) return;

    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->file_addr == file_addr && mod->file_len == file_len) {
            mod->p_addr = p_addr;
            mod->helper_addr = helper_addr;
            return;
        }
    }

    if (m->nested_module_count >= ARM_EXT_NESTED_MODULE_MAX) {
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: nested module registry full, file=0x%X len=%u\n",
                   file_addr, file_len);
        }
        return;
    }

    ArmExtNestedModule *slot = &m->nested_modules[m->nested_module_count++];
    slot->file_addr = file_addr;
    slot->file_len = file_len;
    slot->p_addr = p_addr;
    slot->helper_addr = helper_addr;
    slot->got_memcpy_off = 0;
}

/*
 * 通用 private-loader child GOT bridge 修复。
 *
 * 背景：少数 wrapper（如 gghjt 的 cfunction.ext）用私有 loader 加载子模块，不走标准
 * table[25] (_mr_c_function_new) 路径。私有 loader 会分配并清零一个 module record——
 * 它是 EXT_TABLE 的逐槽镜像（record[off] 对应 EXT_TABLE[off/4]）——但不填 bridge 源槽，
 * 子模块 entry 随后把空白源槽拷进自己的 RW，导致通过 GOT 调用标准函数时跳到空指针。
 * 因为这些 bridge 值从未被 ARM 写出，通用的 hook_got_write 快照机制捕获不到。
 *
 * 这里的修复完全数据驱动，无任何 app 指纹/文件名/魔数索引：
 *  (a) 扫描 record，凡 master EXT_TABLE[idx] 为自指针 bridge（== EXT_TABLE_ADDR+idx*4）
 *      且当前 record 槽为空白(0) 的，填回该 bridge 值；
 *  (b) 子模块 RW 段里 bridge 槽的目标偏移属于该子模块自身的数据段布局，无法从 EXT_TABLE
 *      推导，故以声明式描述符（纯数据，按结构化 in-bounds 拟合应用，绝不按文件名）镜像。
 * 两步都只写"空白槽"，因此对普通 app / 已正常重定位的子模块是安全 no-op。
 */
#define PRIVATE_CHILD_RECORD_SPAN 0x248u

typedef struct {
    uint32_t record_src_off; /* run 起始 record 偏移 */
    uint32_t rw_dst_off;     /* run 起始 RW 镜像偏移 */
    uint32_t count;          /* 4 字节槽数量          */
} PrivateChildRwRun;

typedef struct {
    const char *note;
    const PrivateChildRwRun *runs;
    uint32_t run_count;
} PrivateChildRwLayout;

/* verdload.ext 的 RW 数据段布局：idx26→rw+0x16C（mr_printf）、idx3..19→rw+0x170..0x1B0
 * （memcpy..strtoul）。源 record 偏移 = idx*4（record 是 EXT_TABLE 镜像）。 */
static const PrivateChildRwRun verdload_rw_runs[] = {
    { 0x68u, 0x16Cu, 1u  },
    { 0x0Cu, 0x170u, 17u },
};
static const PrivateChildRwLayout private_child_rw_layouts[] = {
    { "verdload", verdload_rw_runs, sizeof(verdload_rw_runs) / sizeof(verdload_rw_runs[0]) },
};

static void arm_ext_repair_private_child_bridges(ArmExtModule *m,
                                                 uint32 file_addr,
                                                 uint32 file_len,
                                                 uint32 rw_base) {
    if (!m || !rw_base) return;
    uint32_t record = arm_ext_read_u32_or_zero_(m, file_addr); /* file_base[0] */
    if (!record) return;

    /* (a) 数据驱动 record 源槽填值：仅空白的自指针 bridge 槽。 */
    for (uint32_t off = 0; off + 4u <= PRIVATE_CHILD_RECORD_SPAN; off += 4u) {
        uint32_t idx = off / 4u;
        if (idx >= EXT_TABLE_COUNT) break;
        if (!arm_ptr(m, record + off)) break;
        uint32_t ext = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + idx * 4u);
        if (ext != EXT_TABLE_ADDR + idx * 4u) continue;      /* 非 bridge 自指针 */
        if (arm_ext_read_u32_or_zero_(m, record + off) != 0) continue; /* 已填 */
        memcpy(arm_ptr(m, record + off), &ext, 4);
    }

    /* (b) 声明式 RW 镜像：先整体 in-bounds 拟合，再逐槽镜像空白目标。
     *
     * bridge 桥块在子模块 RW 段内的起始偏移随该模块链接结果而变，描述符里写死的
     * rw_dst_off 只对个别模块(gghjt verdload)正确，对别的模块(如 dota 社区下载库)
     * 会把桥写到模块代码实际不读取的偏移：代码按 rw+0x188 取 memcpy 桥却拿到
     * memcmp，正文逐字符拷贝失效、整片空白。
     *
     * 这里用观测到的真实偏移平移描述符：若有与本模块代码区间重叠、且已被 ARM 重定位
     * 写入过 memcpy(table[3]) 桥的同族实例(got_memcpy_off!=0)，按它的真实偏移与描述符
     * 中 idx3(memcpy) 槽偏移之差平移整张描述符；无可借用观测时按描述符原偏移(保持
     * gghjt 老行为)。验证：test/dota/community-enter.sh 正文文字显示，gghjt 下载/付费
     * 脚本与 ctest 保持通过。 */
    ArmExtNestedModule *donor =
        arm_ext_find_got_donor(m, file_addr, file_addr + file_len);
    for (size_t L = 0; L < sizeof(private_child_rw_layouts) /
                           sizeof(private_child_rw_layouts[0]); ++L) {
        const PrivateChildRwLayout *lay = &private_child_rw_layouts[L];
        /* 描述符里 idx3(memcpy，record_src_off==0x0C) 槽的偏移，作为平移基准。 */
        int32_t shift = 0;
        if (donor && donor->got_memcpy_off) {
            for (uint32_t r = 0; r < lay->run_count; ++r) {
                if (lay->runs[r].record_src_off == 0x0Cu) {
                    shift = (int32_t)donor->got_memcpy_off -
                            (int32_t)lay->runs[r].rw_dst_off;
                    break;
                }
            }
        }
        int fits = 1;
        for (uint32_t r = 0; r < lay->run_count && fits; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            uint32_t dst = run->rw_dst_off + shift;
            if (!run->count ||
                !arm_ptr(m, record + run->record_src_off + (run->count - 1u) * 4u) ||
                !arm_ptr(m, rw_base + dst + (run->count - 1u) * 4u))
                fits = 0;
        }
        if (!fits) continue;
        for (uint32_t r = 0; r < lay->run_count; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            uint32_t dst = run->rw_dst_off + shift;
            for (uint32_t i = 0; i < run->count; ++i) {
                uint32_t v = arm_ext_read_u32_or_zero_(
                    m, record + run->record_src_off + i * 4u);
                if (!v) continue;
                if (arm_ext_read_u32_or_zero_(m, rw_base + dst + i * 4u))
                    continue; /* 只写空白 RW 槽，保证 no-op 安全 */
                memcpy(arm_ptr(m, rw_base + dst + i * 4u), &v, 4);
                if (getenv("VMRP_ARM_EXT_DIAG")) {
                    printf("DIAG private-child bridge repair[%s]: shift=%d rw+0x%X = 0x%X\n",
                           lay->note, shift, dst + i * 4u, v);
                }
            }
        }
    }
}

static int arm_ext_has_internal_loader_chunk(ArmExtModule *m,
                                             uint32_t file_addr,
                                             uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + 0x38 <= m->heap_top;
         ext_chunk += 4) {
        uint32_t magic = 0, chunk_file = 0, chunk_len = 0;
        memcpy(&magic, arm_ptr(m, ext_chunk), 4);
        if (magic != EXT_CHUNK_MAGIC) continue;
        memcpy(&chunk_file, arm_ptr(m, ext_chunk + 12), 4);
        memcpy(&chunk_len, arm_ptr(m, ext_chunk + 16), 4);
        if (chunk_file == file_addr && chunk_len == file_len) {
            return 1;
        }
    }

    return 0;
}


static int arm_ext_sync_internal_nested_module(ArmExtModule *m,
                                               uint32_t file_addr,
                                               uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + 0x38 <= m->heap_top;
         ext_chunk += 4) {
        uint32_t magic = 0, chunk_file = 0, chunk_len = 0;
        memcpy(&magic, arm_ptr(m, ext_chunk), 4);
        if (magic != EXT_CHUNK_MAGIC) continue;
        memcpy(&chunk_file, arm_ptr(m, ext_chunk + 12), 4);
        memcpy(&chunk_len, arm_ptr(m, ext_chunk + 16), 4);
        if (chunk_file != file_addr || chunk_len != file_len) continue;

        uint32_t helper_addr = 0, p_addr = 0, p_ext_chunk = 0, rw_base = 0;
        memcpy(&helper_addr, arm_ptr(m, ext_chunk + 8), 4);
        memcpy(&p_addr, arm_ptr(m, ext_chunk + 28), 4);
        if (!helper_addr || !p_addr ||
            !arm_ptr(m, helper_addr & ~1u) || !arm_ptr(m, p_addr + 12)) {
            return 0;
        }
        memcpy(&rw_base, arm_ptr(m, p_addr), 4);
        memcpy(&p_ext_chunk, arm_ptr(m, p_addr + 12), 4);
        if (!rw_base || p_ext_chunk != ext_chunk) {
            return 0;
        }

        arm_ext_repair_private_child_bridges(m, file_addr, file_len, rw_base);

        /*
         * cfunction.ext's internal loader does not call table[25].  The
         * disassembly at 0xE8339C..0xE833CA writes file_base+8, file_base,
         * file_len and P into an extChunk, calls mr_cacheSync, then BLXs the
         * child entry.  The child fills extChunk[8] with its callback helper.
         * Recording that structure gives host-side R9/timer ownership the
         * same module boundaries the wrapper uses internally.
         */
        arm_ext_record_nested_module(m, file_addr, file_len, p_addr, helper_addr);
        app_on_child_confirmed(m, p_addr, helper_addr);
        /*
         * Host-visible nested loads (table[25]) make every loaded child the
         * active foreground module.  cfunction.ext's private loader builds
         * the same extChunk/node shape without crossing table[25], so mirror
         * that ownership here once the helper slot is populated.
        */
        m->active_helper_addr = helper_addr;
        m->active_p_addr = p_addr;
        arm_ext_clear_foreground_screen_owner(m);
        if (!m->primary_helper_addr) {
            /*
             * Host table[25] uses the first nested EXT as the foreground app
             * module. cfunction.ext's internal table[25] follows the same
             * wrapper layout but bypasses the host hook, so mirror only that
             * ownership metadata here. The wrapper has already BLXed the child
             * entry and issued its setup helper calls; this block must not
             * initialize or dispatch the child again.
             */
            m->primary_helper_addr = helper_addr;
            m->primary_p_addr = p_addr;
            m->primary_file_addr = file_addr;
            m->primary_file_len = file_len;
            m->primary_host_init_pending = 0;
        }
        m->pending_internal_file_addr = 0;
        m->pending_internal_file_len = 0;
        /* 不在此处清除 primary_child_reopen_timer_needed 标志。
         * 该标志由模态框关闭时设置(arm_ext_call code=1 post-processing)，
         * 表示后续需要重启定时器驱动新加载的子插件。此函数在 run_arm_with_sp
         * 内部执行（如 game 的 code=2 回调中加载子模块），此时无法启动宿主
         * 定时器。如果在这里清除标志，arm_ext_call 返回后的定时器检查就无法
         * 发现需要重启，导致子插件界面无法显示。标志由 arm_ext_call 的
         * post-processing 在确认定时器已重启后清除。 */
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            uint32_t ext_type = 0, is_pause = 0;
            memcpy(&ext_type, arm_ptr(m, p_addr + 8), 4);
            memcpy(&is_pause, arm_ptr(m, ext_chunk + 0x34), 4);
            printf("DIAG synced file=0x%X len=%u chunk=0x%X P=0x%X H=0x%X ext_type=%u pause=%u activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X\n",
                   file_addr, file_len, ext_chunk, p_addr, helper_addr, ext_type, is_pause,
                   m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
                   m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr);
        }
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: synced internal nested helper=0x%X P=0x%X file=0x%X len=%u primary=0x%X/0x%X\n",
                   helper_addr, p_addr, file_addr, file_len,
                   m->primary_helper_addr, m->primary_p_addr);
        }
        return 1;
    }

    return 0;
}




static int arm_ext_ranges_overlap(uint32_t a, uint32_t a_len,
                                  uint32_t b, uint32_t b_len) {
    uint64_t a_start = a;
    uint64_t a_end = a_start + a_len;
    uint64_t b_start = b;
    uint64_t b_end = b_start + b_len;
    return a_start < b_end && b_start < a_end;
}

static int arm_ext_range_contains(uint32_t outer, uint32_t outer_len,
                                  uint32_t inner, uint32_t inner_len) {
    uint64_t outer_start = outer;
    uint64_t outer_end = outer_start + outer_len;
    uint64_t inner_start = inner;
    uint64_t inner_end = inner_start + inner_len;
    return outer_start <= inner_start && inner_end <= outer_end;
}

static void arm_ext_restore_primary_mapping_after_dump0(ArmExtModule *m,
                                                       uint32_t read_addr,
                                                       uint32_t read_len) {
    if (!m || !m->primary_file_addr || !m->primary_file_len ||
        !m->primary_p_addr || !m->primary_helper_addr) {
        return;
    }
    if (!arm_ext_range_contains(read_addr, read_len,
                                m->primary_file_addr, m->primary_file_len)) {
        return;
    }
    /*
     * gxdzc/netpay 取消路径反汇编定位：
     *   0xE836A7 通过 table[44] 把 plugins/dump0 读回 0x6460D4，长度 0x96000；
     *   随后 game.ext 在 0x6643CD/0x6643D7 重新 stop/start 自己的 timer。
     * 读回 dump0 会把 0x6460F8 上临时加载的 netpay.ext 覆盖回 game.ext，
     * 但执行器的 nested_modules 仍把同一地址范围登记为 netpay，导致
     * table[31] 把 timer owner 记录成 netpay helper(0x64D531)，下一次
     * code=2 进入已被覆盖的插件，最终在 wrapper 队列 0xE83B48..0xE83B56
     * 弹出坏 PC=0x9A000004。这里按真实内存恢复结果重建代码归属，不在
     * run_arm_with_sp() 里吞异常；兼容性：只在大块读回完整覆盖 primary
     * ext 映像时触发，普通插件 staging 仍由 table[131]/mr_cacheSync 处理。
     * 验证：test/gxdzc/pay-cancel.sh 不再打印 invalid memory，ctest 通过。
     */
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule mod = m->nested_modules[i];
        int is_primary = mod.file_addr == m->primary_file_addr &&
                         mod.file_len == m->primary_file_len &&
                         mod.helper_addr == m->primary_helper_addr;
        if (!is_primary &&
            arm_ext_ranges_overlap(mod.file_addr, mod.file_len, read_addr, read_len)) {
            continue;
        }
        m->nested_modules[out++] = mod;
    }
    m->nested_module_count = out;
    arm_ext_record_nested_module(m, m->primary_file_addr, m->primary_file_len,
                                 m->primary_p_addr, m->primary_helper_addr);

    if (m->active_helper_addr &&
        m->active_helper_addr != m->primary_helper_addr &&
        arm_ext_range_contains(read_addr, read_len, m->active_helper_addr & ~1u, 2)) {
        m->active_helper_addr = m->primary_helper_addr;
        m->active_p_addr = m->primary_p_addr;
        arm_ext_clear_foreground_screen_owner(m);
    }
    if (m->timer_helper_addr &&
        m->timer_helper_addr != m->primary_helper_addr &&
        arm_ext_range_contains(read_addr, read_len, m->timer_helper_addr & ~1u, 2)) {
        m->timer_helper_addr = m->primary_helper_addr;
        m->timer_p_addr = m->primary_p_addr;
    }
    if (m->dispatch_timer_start &&
        arm_ext_range_contains(read_addr, read_len, m->dispatch_timer_start & ~1u, 2)) {
        m->dispatch_timer_start = 0;
    }
    if (m->dispatch_timer_stop &&
        arm_ext_range_contains(read_addr, read_len, m->dispatch_timer_stop & ~1u, 2)) {
        m->dispatch_timer_stop = 0;
    }
}

static uint32_t arm_ext_p_for_code_addr(ArmExtModule *m, uint32_t addr,
                                        uint32_t *helper_addr) {
    uint32_t pc = addr & ~1u;
    if (helper_addr) *helper_addr = 0;
    if (!m) return 0;

    if (pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
        if (helper_addr) *helper_addr = m->helper_addr;
        return m->p_addr;
    }

    ArmExtNestedModule *mod = arm_ext_find_nested_module(m, pc);
    if (mod) {
        if (helper_addr) *helper_addr = mod->helper_addr;
        return mod->p_addr;
    }

    return 0;
}

static void arm_ext_sync_r9_for_code_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t pc = addr & ~1u;
    uint32_t p_addr = 0;
    if (pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
        uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
        for (int i = 0; i < m->nested_module_count; ++i) {
            uint32_t nested_rw = 0;
            if (arm_ptr(m, m->nested_modules[i].p_addr)) {
                memcpy(&nested_rw, arm_ptr(m, m->nested_modules[i].p_addr), 4);
            }
            if (nested_rw && current_r9 == nested_rw) {
                p_addr = m->p_addr;
                break;
            }
        }
    } else {
        ArmExtNestedModule *mod = arm_ext_find_nested_module(m, pc);
        if (mod) {
            p_addr = mod->p_addr;
            /*
             * 同一份 EXT 代码被 wrapper 实例化多次时（dota 的 screen 模块
             * 0x661AC0 同时有 loading / menu 等多个实例，file_addr 相同、rw 各异），
             * arm_ext_find_nested_module 只能按 PC 区间匹配，返回“最后登记”的实例。
             * 当正在执行的是一个 wrapper 刚 BLX 进去、但尚未登记进 nested_modules
             * 的新实例时（game 自己已经把 sb 装成新实例的 rw），按“最后登记实例”纠
             * R9 会把 game 设好的正确值覆盖成旧实例的 rw，使该模块所有 sb 相对全局
             * 访问错位。dota 实证：menu 屏消息处理器以 loading 实例的 R9 运行，
             * memset(sb-0xA44) 落到 primary 共享的 cg_1 背景条结构、清 0 其指针，
             * 主界面 y=80..159 变黑带。
             *
             * 判据（数据驱动，无 app 指纹）：仅当该代码段被同 file_addr 的多个实例
             * 共享（歧义），且当前 R9 不等于任何“已知上下文”的 rw —— 既不是任何
             * 已登记 nested 实例、也不是 primary、也不是 wrapper(m->p_addr) 的 rw ——
             * 时，认定它是 game 刚设好的“新建未登记实例”的 sb，予以信任、不覆盖。
             * 残留的旧 R9 必然等于某个已知上下文的 rw，仍走原纠正逻辑；单实例模块
             * （same_code==1）完全不受影响。验证：test/dota/hp.sh 黑带消失，
             * ctest 与 gghjt/gxdzc 回归通过。
             */
            int same_code = 0;
            for (int i = 0; i < m->nested_module_count; ++i) {
                if (m->nested_modules[i].file_addr == mod->file_addr &&
                    m->nested_modules[i].file_len)
                    same_code++;
            }
            if (same_code > 1) {
                uint32_t cur_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                int recognized = 0;
                if (cur_r9 == arm_ext_read_u32_or_zero_(m, m->primary_p_addr) ||
                    cur_r9 == arm_ext_read_u32_or_zero_(m, m->p_addr))
                    recognized = 1;
                for (int i = 0; !recognized && i < m->nested_module_count; ++i) {
                    if (cur_r9 ==
                        arm_ext_read_u32_or_zero_(m, m->nested_modules[i].p_addr))
                        recognized = 1;
                }
                if (cur_r9 && !recognized) return; /* 新建未登记实例的 sb，信任 game */
            }
        }
    }
    if (!p_addr || !arm_ptr(m, p_addr)) return;

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (!rw_base) return;

    uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
    if (current_r9 != rw_base) {
        reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    }
}

static void trace_pc(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pos = m->pc_ring_pos++ % EXT_TRACE_PC_RING;
    m->pc_ring[pos] = (uint32_t)address;
    m->cpsr_ring[pos] = reg_read32(uc, UC_ARM_REG_CPSR);
}

static void dump_pc_ring(ArmExtModule *m) {
    if (!getenv("VMRP_ARM_EXT_TRACE_PC")) return;
    uint32_t total = m->pc_ring_pos < EXT_TRACE_PC_RING ? m->pc_ring_pos : EXT_TRACE_PC_RING;
    printf("arm_ext_executor: recent PCs:\n");
    for (uint32_t i = 0; i < total; ++i) {
        uint32_t idx = (m->pc_ring_pos - total + i) % EXT_TRACE_PC_RING;
        printf("  #%02u pc=0x%X cpsr=0x%X %s\n",
               i, m->pc_ring[idx], m->cpsr_ring[idx],
               (m->cpsr_ring[idx] & (1u << 5)) ? "thumb" : "arm");
    }
}

static uint32_t arg_read(ArmExtModule *m, unsigned n) {
    if (n < 4) return reg_read32(m->uc, UC_ARM_REG_R0 + n);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    uint32_t v = 0;
    uc_mem_read(m->uc, sp + (n - 4) * 4, &v, 4);
    return v;
}

static int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp) {
    set_arm_mode_for_addr(m, start);
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    reg_write32(m->uc, UC_ARM_REG_LR, EXT_STOP_ADDR);
    uc_err err = uc_emu_start(m->uc, arm_exec_addr(start), EXT_STOP_ADDR, 0, 0);
    /*
     * Some nested .mrp plug-ins (e.g. netpay.mrp loaded by gghjt.mrp) keep
     * function pointers without the Thumb bit, so a BLX into them lands in
     * ARM mode while the bytes are Thumb-2.  Do the mode correction once per
     * callback entry.  If the same PC still raises UC_ERR_INSN_INVALID after
     * being retried in Thumb mode, it is real invalid code/data, not a missing
     * Thumb bit.  gghjt's 60s timeout-return path exposes this after dump0
     * restore: a copied timer node (e.g. table[3] to 0x738B34) is data, and an
     * unbounded retry here pins the UI on “请稍等”.  Let the normal invalid-PC
     * handling below stop the callback cleanly instead of spinning forever.
     */
    if (err == UC_ERR_INSN_INVALID) {
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
        if (pc != EXT_STOP_ADDR && pc != 0) {
            uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
            if ((cpsr & (1u << 5)) == 0) {
                cpsr |= (1u << 5);
                reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: retrying in Thumb mode at pc=0x%X\n", pc);
                }
                err = uc_emu_start(m->uc, pc, EXT_STOP_ADDR, 0, 0);
            }
        }
    }
    if (err != UC_ERR_OK) {
        if (reg_read32(m->uc, UC_ARM_REG_PC) == EXT_STOP_ADDR) return MR_SUCCESS;
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
        uint32_t cur_sp = reg_read32(m->uc, UC_ARM_REG_SP);
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
            /*
             * ARM 侧栈漂进了代码段后无法继续安全解释。这里只结束当前
             * callback，让平台层保持原状态继续调度；退出/重启必须来自
             * ARM 程序自己的 mr_exit/mr_state 路径，不能在异常处理里推断。
             */
            if (getenv("VMRP_ARM_EXT_TRACE")) {
                printf("arm_ext_executor: ARM callback stopped at non-instruction pc=0x%X sp=0x%X\n", pc, cur_sp);
                dump_pc_ring(m);
            }
            reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
            return MR_SUCCESS;
        }
        /*
         * Nested plug-ins loaded into the heap (e.g. netpay.mrp inside
         * gghjt.mrp) sometimes encode return addresses that drop us a
         * halfword into a 32-bit Thumb-2 instruction. The fetched bytes
         * are then undefined, but a quick check confirms PC-2 starts a
         * valid 32-bit Thumb-2 op -- treat it as a clean stop instead of
         * spamming the console; the caller already handles MR_SUCCESS the
         * same way it handles a normal return.
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc + 2 < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint8_t *p = (uint8_t *)arm_ptr(m, pc - 2);
            if (p) {
                uint16_t prev_hw = (uint16_t)(p[0] | (p[1] << 8));
                uint32_t top5 = prev_hw >> 11;
                if (top5 == 0x1d || top5 == 0x1e || top5 == 0x1f) {
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: plugin returned to mid-Thumb2 pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                        dump_pc_ring(m);
                    }
                    reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                    return MR_SUCCESS;
                }
            }
        }
        /*
         * gghjt 60s 超时返回路径：wrapper 的定时器内联分发跳转到堆上的
         * 定时器/链表节点（如 0x738B58 含 FB FF FF FF …）而非代码。
         *
         * 正常流程中 wrapper 在分发前会先完成 dump0 读取（0x96000 字节
         * → 0x646100），还原游戏内存后分发到的回调指向有效游戏代码。
         * 超时路径中 dump0 虽已打开但大块读取尚未发生就崩溃了。
         *
         * 修复：从宿主侧完成 dump0 读取 + RW 区清理（与正常路径中
         * table[14] memset(0x645C70,0,0x80) / memset(0x645CF0,0,0x20) 等
         * 效），然后终止本次 ARM 执行。下一次定时器触发时 wrapper 会
         * 基于已恢复的游戏状态分发到有效游戏代码，继续加载 pak 资源，
         * 实现「返回游戏菜单」而非重启。
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            /* LR 在 wrapper 代码范围内，说明是 wrapper 内联分发到了堆上
             * 的无效回调地址——netpay 超时返回的标志性崩溃模式 */
            int lr_in_wrapper = (lr & ~1u) >= EXT_CODE_ADDR &&
                                (lr & ~1u) < EXT_CODE_ADDR + m->code_len;
            /* wrapper 内部 loader BLX 到 game code 时 LR 自然指向 wrapper，
             * 不能把这种合法跳转当作无效回调。game code 在 pending file 范围内
             * 时排除 lr_in_wrapper 判定，让后续 "fallback to LR" 继续执行。 */
            if (lr_in_wrapper && m->pending_internal_file_addr &&
                pc >= m->pending_internal_file_addr &&
                pc < m->pending_internal_file_addr + m->pending_internal_file_len) {
                lr_in_wrapper = 0;
            }
            uint8_t *pcp = (uint8_t *)arm_ptr(m, pc);
            if (pcp && ((pcp[0] == 0xFF && pcp[1] == 0xFF) ||
                       (pcp[0] == 0x00 && pcp[1] == 0x00 && pcp[2] == 0x00 && pcp[3] == 0x00) ||
                       lr_in_wrapper)) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: plugin returned to heap data pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
        }
        /*
         * Some plug-in callbacks (e.g. the netpay timer event) corrupt
         * their own stack in our emulator, popping a return address that
         * lands inside ARM ext data (commonly a copy of the mrp pathname).
         * Detect that pattern -- PC sitting on the stack page near SP --
         * and treat it as a clean stop so the caller can carry on; we'd
         * otherwise just spam an Unhandled CPU exception line every tick.
         */
        if (err == UC_ERR_EXCEPTION) {
            uint32_t sp_diff = (pc > cur_sp) ? pc - cur_sp : cur_sp - pc;
            if (sp_diff < 0x4000) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: stack-corrupt plugin callback pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
        }
        printf("arm_ext_executor: uc_emu_start(0x%X) failed: %u (%s)\n", start, err, uc_strerror(err));
        uint8_t *pc_mem = arm_ptr(m, pc);
        if (pc_mem) {
            printf("arm_ext_executor: pc bytes @0x%X:", pc);
            for (uint32_t i = 0; i < 16; ++i) printf(" %02X", pc_mem[i]);
            printf("\n");
        }
        dump_pc_ring(m);
        dumpREG(m->uc);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int run_arm(ArmExtModule *m, uint32_t start) {
    return run_arm_with_sp(m, start, EXT_STACK_ADDR + EXT_STACK_SIZE);
}

static void restore_ext_r9(ArmExtModule *m) {
    if (!m) return;
    uint32_t p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    if (!p_addr) return;
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
}

static char *arm_str(ArmExtModule *m, uint32_t addr) {
    char *p = (char *)arm_ptr(m, addr);
    return p ? p : (char *)"";
}

static int format_arm(ArmExtModule *m, char *dst, size_t dst_size, const char *fmt, unsigned first_arg) {
    size_t out = 0;
    unsigned ai = first_arg;
    for (const char *p = fmt; *p && out + 1 < dst_size; ++p) {
        if (*p != '%') {
            dst[out++] = *p;
            continue;
        }
        p++;
        if (*p == '%') { dst[out++] = '%'; continue; }
        char spec[32];
        size_t si = 0;
        spec[si++] = '%';
        while (*p && strchr("-+ #0", *p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        while (*p && isdigit((unsigned char)*p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (*p == 'l' && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (!*p) break;
        spec[si++] = *p;
        spec[si] = '\0';
        uint32_t av = arg_read(m, ai++);
        char tmp[512];
        switch (*p) {
            case 's': snprintf(tmp, sizeof(tmp), spec, arm_str(m, av)); break;
            case 'c': snprintf(tmp, sizeof(tmp), spec, (int)av); break;
            case 'p': snprintf(tmp, sizeof(tmp), "0x%X", av); break;
            default: snprintf(tmp, sizeof(tmp), spec, av); break;
        }
        size_t l = strlen(tmp);
        if (l > dst_size - out - 1) l = dst_size - out - 1;
        memcpy(dst + out, tmp, l);
        out += l;
    }
    dst[out] = '\0';
    return (int)out;
}

/* ---- MRP 文件缓存实现 ---- */

static uint32_t mrp_rd32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static const char *path_basename(const char *path) {
    const char *p = strrchr(path, '/');
    const char *q = strrchr(path, '\\');
    if (q && (!p || q > p)) p = q;
    return p ? p + 1 : path;
}

/* 解析 MRP 并缓存所有条目（解压 gzip） */
static void parse_mrp_cache(ArmExtModule *m, const char *mrp_path) {
    m->mrp_cache_count = 0;
    if (!mrp_path || !*mrp_path) return;

    FILE *fp = fopen(mrp_path, "rb");
    if (!fp) return;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 240) { fclose(fp); return; }

    uint8_t *raw = (uint8_t *)malloc((size_t)sz);
    if (!raw) { fclose(fp); return; }
    if (fread(raw, 1, (size_t)sz, fp) != (size_t)sz) { free(raw); fclose(fp); return; }
    fclose(fp);

    if (memcmp(raw, "MRPG", 4) != 0) { free(raw); return; }

    uint32_t pos = 240;
    while (pos + 16 <= (uint32_t)sz && m->mrp_cache_count < MRP_CACHE_MAX) {
        uint32_t name_len = mrp_rd32le(raw + pos);
        pos += 4;
        if (name_len == 0 || name_len > 255 || pos + name_len + 12 > (uint32_t)sz) break;
        char entry_name[256];
        uint32_t copy_len = name_len < sizeof(entry_name) - 1 ? name_len : sizeof(entry_name) - 1;
        memcpy(entry_name, raw + pos, copy_len);
        entry_name[copy_len] = '\0';
        /* 去除尾部 null（MRP 条目名含 null 终止符算在 name_len 中） */
        if (copy_len > 0 && entry_name[copy_len - 1] == '\0') copy_len--;
        pos += name_len;
        uint32_t off = mrp_rd32le(raw + pos);
        uint32_t packed_len = mrp_rd32le(raw + pos + 4);
        pos += 12;
        if (off >= (uint32_t)sz || packed_len > (uint32_t)sz - off) continue;

        MrpCacheEntry *e = &m->mrp_cache[m->mrp_cache_count];
        strncpy(e->name, entry_name, sizeof(e->name) - 1);
        e->name[sizeof(e->name) - 1] = '\0';

        /* gzip 压缩的条目在宿主侧解压 */
        if (packed_len >= 2 && raw[off] == 0x1f && raw[off + 1] == 0x8b) {
            z_stream zs;
            memset(&zs, 0, sizeof(zs));
            if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) continue;
            uint32_t cap = packed_len * 4 + 4096;
            uint8_t *out = (uint8_t *)malloc(cap);
            if (!out) { inflateEnd(&zs); continue; }
            zs.next_in = raw + off;
            zs.avail_in = packed_len;
            int zret;
            do {
                if (zs.total_out == cap) {
                    uint32_t new_cap = cap * 2;
                    uint8_t *nb = (uint8_t *)realloc(out, new_cap);
                    if (!nb) { free(out); out = NULL; break; }
                    out = nb;
                    cap = new_cap;
                }
                zs.next_out = out + zs.total_out;
                zs.avail_out = cap - (uint32_t)zs.total_out;
                zret = inflate(&zs, Z_NO_FLUSH);
            } while (zret == Z_OK);
            if (!out || zret != Z_STREAM_END) { free(out); inflateEnd(&zs); continue; }
            e->data = out;
            e->data_len = (uint32_t)zs.total_out;
            inflateEnd(&zs);
        } else {
            e->data = (uint8_t *)malloc(packed_len);
            if (!e->data) continue;
            memcpy(e->data, raw + off, packed_len);
            e->data_len = packed_len;
        }
        m->mrp_cache_count++;
    }
    free(raw);
}

/* 按 basename 在缓存中查找条目 */
static MrpCacheEntry *mrp_cache_find(ArmExtModule *m, const char *filename) {
    const char *base = path_basename(filename);
    for (int i = 0; i < m->mrp_cache_count; i++) {
        if (strcmp(path_basename(m->mrp_cache[i].name), base) == 0)
            return &m->mrp_cache[i];
    }
    return NULL;
}

/* 分配一个虚拟 fd 并绑定到指定数据 */
static uint32_t mrp_vfd_open(ArmExtModule *m, const uint8_t *data, uint32_t len) {
    for (int i = 0; i < MRP_VFD_MAX; i++) {
        if (!m->mrp_vfds[i].in_use) {
            m->mrp_vfds[i].in_use = 1;
            m->mrp_vfds[i].data = data;
            m->mrp_vfds[i].data_len = len;
            m->mrp_vfds[i].pos = 0;
            return MRP_VFD_BASE + (uint32_t)i + 1;
        }
    }
    return 0;
}

static MrpVirtualFd *mrp_vfd_get(ArmExtModule *m, uint32_t fd) {
    if (fd <= MRP_VFD_BASE || fd > MRP_VFD_BASE + MRP_VFD_MAX) return NULL;
    MrpVirtualFd *v = &m->mrp_vfds[fd - MRP_VFD_BASE - 1];
    return v->in_use ? v : NULL;
}

static void mrp_cache_free(ArmExtModule *m) {
    for (int i = 0; i < m->mrp_cache_count; i++) {
        free(m->mrp_cache[i].data);
        m->mrp_cache[i].data = NULL;
    }
    m->mrp_cache_count = 0;
}

/* ---- 结束 MRP 文件缓存 ---- */

static void write_table_entry(ArmExtModule *m, uint32_t index, uint32_t value) {
    memcpy((uint8_t *)arm_ptr(m, EXT_TABLE_ADDR + index * 4), &value, 4);
}

static uint32_t alloc_u32_slot(ArmExtModule *m, uint32_t value) {
    uint32_t slot = arm_alloc(m, 4);
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
    return slot;
}

static uint32_t alloc_string(ArmExtModule *m, const char *value) {
    if (!value) value = "";
    size_t len = strlen(value) + 1;
    uint32_t slot = arm_alloc(m, (uint32_t)len);
    if (slot) memcpy(arm_ptr(m, slot), value, len);
    return slot;
}

static void internal_slot_write(ArmExtModule *m, uint32_t slot, uint32_t value) {
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
}

static int internal_slot_read(ArmExtModule *m, uint32_t slot, uint32_t *value) {
    if (!m || !slot || !value || !arm_ptr(m, slot)) return 0;
    memcpy(value, arm_ptr(m, slot), 4);
    return 1;
}

static void sync_internal_state_to_arm(ArmExtModule *m) {
    if (!m) return;
    internal_slot_write(m, m->mr_state_slot, (uint32_t)mr_state);
    internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
}

static void sync_timer_state_from_arm(ArmExtModule *m) {
    enum { ARM_TIMER_IDLE = 0, ARM_TIMER_RUNNING = 1 };
    uint32_t arm_timer_state = 0;
    if (!internal_slot_read(m, m ? m->mr_timer_state_slot : 0, &arm_timer_state)) {
        return;
    }
    if (arm_timer_state <= 3) {
        if (m->host_timer_pending &&
            mr_timer_state == ARM_TIMER_RUNNING &&
            arm_timer_state == ARM_TIMER_IDLE) {
            internal_slot_write(m, m->mr_timer_state_slot, ARM_TIMER_RUNNING);
            return;
        }
        mr_timer_state = (int32)arm_timer_state;
        if (mr_timer_state != ARM_TIMER_RUNNING) {
            m->host_timer_pending = 0;
        }
    }
}

static void arm_ext_record_timer_owner(ArmExtModule *m) {
    if (!m) return;
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
    uint32_t owner_p = 0;
    uint32_t owner_helper = 0;

    owner_p = arm_ext_p_for_code_addr(m, lr, &owner_helper);
    if (!owner_p || !owner_helper) {
        /*
         * table[31] owner is normally identified from LR's code range. The
         * current/active path is kept for host-synthetic entries whose LR is
         * outside every loaded EXT image. Verification: test/gxdzc/pay.sh and
         * ctest.
         */
        owner_p = m->current_p_addr ? m->current_p_addr :
                  (m->active_p_addr ? m->active_p_addr : m->p_addr);
        owner_helper = m->current_helper_addr ? m->current_helper_addr :
                       (m->active_helper_addr ? m->active_helper_addr : m->helper_addr);
    }

    if (owner_p && owner_helper) {
        m->timer_p_addr = owner_p;
        m->timer_helper_addr = owner_helper;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG timer_owner lr=0x%X ownerP=0x%X ownerH=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X\n",
                   lr, owner_p, owner_helper, m->active_p_addr,
                   m->active_helper_addr, m->current_p_addr,
                   m->current_helper_addr);
        }
    }
}

/* origin_mem 池由 ext 自身的 ARM 内存管理器维护，ext 直接更新
 * table[111/135/136] 对应的 slot 值。宿主侧不应覆写这些 slot，
 * 否则 ext 读到的 origin_mem_left 等统计会与实际不符。
 * 这里仅维护 m->origin_mem_left 用于其他宿主逻辑（如 arm_alloc
 * 的上限检查），不再调用 sync_origin_mem_stats。 */
static void note_origin_mem_alloc(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    len = align4(len ? len : 1);
    m->origin_mem_left = len < m->origin_mem_left ? m->origin_mem_left - len : 0;
    if (m->origin_mem_left < m->origin_mem_min) {
        m->origin_mem_min = m->origin_mem_left;
        m->origin_mem_top = m->origin_mem_len - m->origin_mem_min;
    }
}

static void note_origin_mem_free(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    m->origin_mem_left += align4(len);
    if (m->origin_mem_left > m->origin_mem_len) m->origin_mem_left = m->origin_mem_len;
}

static void enter_screen_context(ArmExtModule *m, uint16 **saved_screen) {
    *saved_screen = mr_screenBuf;
    if (m->screen_addr) mr_screenBuf = (uint16 *)arm_ptr(m, m->screen_addr);
}

static void leave_screen_context(ArmExtModule *m, uint16 *saved_screen) {
    if (m && m->foreground_screen_snapshot_valid &&
        m->foreground_screen_snapshot &&
        m->foreground_screen_snapshot_len == m->screen_len &&
        m->screen_addr && arm_ptr(m, m->screen_addr)) {
        memcpy(arm_ptr(m, m->screen_addr),
               m->foreground_screen_snapshot, m->screen_len);
    }
    if (m->screen_addr && saved_screen && m->screen_len) {
        memcpy(saved_screen, arm_ptr(m, m->screen_addr), m->screen_len);
    }
    mr_screenBuf = saved_screen;
    /* 真机上 ext 写屏幕缓冲区后由 OS 的 VSync/刷新机制送显。模拟器里
     * ARM 缓冲区已 memcpy 到宿主 screenBuf，直接调 mr_drawBitmap 送显。 */
    if (saved_screen) {
        mr_drawBitmap(saved_screen, 0, 0, (uint16)m->screen_w, (uint16)m->screen_h);
    }
    m->screen_dirty = 0;
}

static void arm_ext_clear_foreground_screen_owner(ArmExtModule *m) {
    if (!m) return;
    m->foreground_screen_owner_p_addr = 0;
    m->foreground_screen_owner_helper_addr = 0;
    m->foreground_screen_snapshot_valid = 0;
}

static void arm_ext_note_foreground_screen_write(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr)) {
        return;
    }
    if (m->foreground_screen_snapshot_len != m->screen_len) {
        free(m->foreground_screen_snapshot);
        m->foreground_screen_snapshot = NULL;
        m->foreground_screen_snapshot_len = 0;
        m->foreground_screen_snapshot_valid = 0;
    }
    if (!m->foreground_screen_snapshot) {
        m->foreground_screen_snapshot = (uint8_t *)malloc(m->screen_len);
        if (m->foreground_screen_snapshot)
            m->foreground_screen_snapshot_len = m->screen_len;
    }
    if (!m->foreground_screen_snapshot) return;

    memcpy(m->foreground_screen_snapshot, arm_ptr(m, m->screen_addr),
           m->screen_len);
    m->foreground_screen_owner_p_addr = owner_p_addr;
    m->foreground_screen_owner_helper_addr = owner_helper_addr;
    m->foreground_screen_snapshot_valid = 1;
}

static int arm_ext_should_accept_screen_write(ArmExtModule *m,
                                              uint32_t *claim_p_addr,
                                              uint32_t *claim_helper_addr) {
    if (claim_p_addr) *claim_p_addr = 0;
    if (claim_helper_addr) *claim_helper_addr = 0;
    if (!m) return 1;
    if (!m->primary_p_addr || !m->active_p_addr || !m->active_helper_addr) return 1;
    if (m->active_p_addr == m->primary_p_addr ||
        m->active_helper_addr == m->primary_helper_addr ||
        m->active_p_addr == m->p_addr ||
        m->active_helper_addr == m->helper_addr) {
        return 1;
    }

    uint32_t caller_helper_addr = 0;
    uint32_t caller_p_addr =
        arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_LR),
                                &caller_helper_addr);
    if (!caller_p_addr) {
        caller_p_addr = m->current_p_addr;
        caller_helper_addr = m->current_helper_addr;
    }
    if (!caller_p_addr) return 1;
    if (caller_p_addr == m->active_p_addr) {
        if (claim_p_addr) *claim_p_addr = caller_p_addr;
        if (claim_helper_addr) *claim_helper_addr = caller_helper_addr;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG screen_claim callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   m->foreground_screen_snapshot_valid);
        }
        return 1;
    }
    if (m->foreground_screen_owner_p_addr != m->active_p_addr ||
        m->foreground_screen_owner_helper_addr != m->active_helper_addr) {
        return 1;
    }

    /*
     * cfunction.ext suspend/resume manages extChunk[0x34]:
     *   0xE83098/0xE83514 increment it and detach extChunk[0x24].
     *   0xE83590/0xE835FC decrement it and relink extChunk[0x24].
     * The field may be incremented after the child has already rendered, so
     * use the first actual child screen write as the host-side foreground
     * claim.  gghjt's verdload path shows the child rendering the download UI,
     * then the wrapper returns to game.ext and redraws the menu in the same
     * timer callback.  run_arm_with_sp() is still executing the wrapper helper
     * in that nested call, so classify each draw by LR's code range instead of
     * current_p_addr.  The host has no real window stack, so keep the active
     * child framebuffer authoritative until the wrapper resumes the primary
     * layer or another active child claims the screen.
    */
    if (caller_p_addr == m->primary_p_addr) {
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG screen_reject primary callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   m->foreground_screen_snapshot_valid);
        }
        return 0;
    }
    if (caller_p_addr == m->p_addr &&
        caller_helper_addr == m->helper_addr) {
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG screen_reject wrapper callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   m->foreground_screen_snapshot_valid);
        }
        return 0;
    }
    return 1;
}

static void arm_ext_finish_accepted_screen_write(ArmExtModule *m,
                                                 uint32_t claim_p_addr,
                                                 uint32_t claim_helper_addr) {
    if (!claim_p_addr) return;
    /*
     * A foreground child is claimed by executable owner, but the framebuffer
     * must be captured after the accepted draw completes.  Capturing before the
     * draw preserves the old primary/menu pixels and lets leave_screen_context()
     * flush those stale pixels over the child UI.
     */
    arm_ext_note_foreground_screen_write(m, claim_p_addr, claim_helper_addr);
}

static void arm_ext_mirror_draw_bitmap_to_screen(ArmExtModule *m,
                                                 uint32_t bmp_addr,
                                                 int16_t x,
                                                 int16_t y,
                                                 uint16_t w,
                                                 uint16_t h) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr) || !arm_ptr(m, bmp_addr)) {
        return;
    }

    int32_t screen_w = m->screen_w;
    int32_t screen_h = m->screen_h;
    if (screen_w <= 0 || screen_h <= 0) return;

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = (int32_t)x + (int32_t)w;
    int32_t max_y = (int32_t)y + (int32_t)h;
    if (max_x > screen_w) max_x = screen_w;
    if (max_y > screen_h) max_y = screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    uint16_t *dst_screen = (uint16_t *)arm_ptr(m, m->screen_addr);
    uint32_t row_pixels = (uint32_t)(max_x - min_x);
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint32_t offset_pixels = (uint32_t)yy * (uint32_t)screen_w +
                                 (uint32_t)min_x;
        uint64_t row_addr64 = (uint64_t)bmp_addr +
                              (uint64_t)offset_pixels * sizeof(uint16_t);
        uint64_t row_end64 = row_addr64 +
                             (uint64_t)row_pixels * sizeof(uint16_t) - 1u;
        if (row_end64 > 0xFFFFFFFFu) return;
        uint16_t *src_row = (uint16_t *)arm_ptr(m, (uint32_t)row_addr64);
        if (!src_row || !arm_ptr(m, (uint32_t)row_end64)) return;
        /*
         * host mr_drawBitmap() flushes a rectangle from a full screen-stride
         * source buffer; mirror that same visible rectangle into the ARM
         * framebuffer so later context exit cannot restore the covered layer.
         */
        memcpy(dst_screen + offset_pixels, src_row,
               (size_t)row_pixels * sizeof(uint16_t));
    }
    m->screen_dirty = 1;
}

static void capture_timer_dispatches(ArmExtModule *m) {
    if (!m || !m->primary_helper_addr) return;
    uint32_t t31a = EXT_TABLE_ADDR + 31 * 4;
    uint32_t t32a = EXT_TABLE_ADDR + 32 * 4;
    uint32_t t31v = 0, t32v = 0;
    memcpy(&t31v, arm_ptr(m, t31a), 4);
    memcpy(&t32v, arm_ptr(m, t32a), 4);
    uint32_t primary_start = m->primary_file_addr;
    uint32_t primary_end = primary_start + m->primary_file_len;
    int t31_primary = primary_start && t31v >= primary_start && t31v < primary_end;
    int t32_primary = primary_start && t32v >= primary_start && t32v < primary_end;

    if (t31v != t31a && arm_ptr(m, t31v)) {
        /* 嵌套 EXT 会把 SDK 的 timerStart 表项改成自己的 dispatch veneer
         * （mpc/game.ext 反汇编为 str ..., [table,#0x7c]）。模拟器必须保留
         * table hook，同时记录真实 veneer，用 wrapper R9 转调。 */
        int captured = (!m->dispatch_timer_start || t31_primary);
        if (captured) {
            m->dispatch_timer_start = t31v;
        }
        memcpy(arm_ptr(m, t31a), &t31a, 4);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: %s timerStart dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t31v);
        }
    }
    if (t32v != t32a && arm_ptr(m, t32v)) {
        /* 同上，timerStop 也可能被嵌套 EXT 替换为 stop dispatch veneer。 */
        int captured = (!m->dispatch_timer_stop || t32_primary);
        if (captured) {
            m->dispatch_timer_stop = t32v;
        }
        memcpy(arm_ptr(m, t32a), &t32a, 4);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: %s timerStop dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t32v);
        }
    }
}

static void cb_ret(ArmExtModule *m, uint32_t ret) {
    reg_write32(m->uc, UC_ARM_REG_R0, ret);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    set_arm_mode_for_addr(m, lr);
    reg_write32(m->uc, UC_ARM_REG_PC, lr);
}

/* ARM ext 跳转到低地址区（0x0 ~ EXT_LOW_TABLE_SIZE）时的处理。
 *
 * 两种场景会到这里：
 * a) 未经重定位的嵌套 ext 用低地址 table 指针（如 0x8 = table[2]），
 *    发生在宿主侧预解压后跳过了 ARM 侧的重定位。此时应将执行重定向
 *    到主表对应条目，由 hook_table 正常分发。
 * b) 无效跳转（如 0x1000），按 return-to-LR 兜底。 */
static void hook_low_zero(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    /* 场景 a：未经重定位的 ext 通过低地址调用 table 函数。
     * 不做真实分发（参数可能是给嵌套 ext 函数而非 table 函数的），
     * 安全地返回 0（NULL/失败），让调用方走错误处理路径。 */
    if (address < EXT_TABLE_COUNT * 4) {
        reg_write32(uc, UC_ARM_REG_R0, 0);
    }
    /* 场景 b：超出 table 范围 → return to LR */
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    if (lr) {
        set_arm_mode_for_addr(m, lr);
        reg_write32(uc, UC_ARM_REG_PC, lr);
    } else {
        reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
        uc_emu_stop(uc);
    }
}

static void hook_table(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address < EXT_TABLE_ADDR || address >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) return;
    uint32_t idx = (uint32_t)((address - EXT_TABLE_ADDR) / 4);
    if (m->pending_internal_file_addr) {
        arm_ext_sync_internal_nested_module(m, m->pending_internal_file_addr,
                                            m->pending_internal_file_len);
    }
    capture_timer_dispatches(m);
    uint32_t r0 = arg_read(m, 0), r1 = arg_read(m, 1), r2 = arg_read(m, 2), r3 = arg_read(m, 3);
    uint32_t ret = MR_SUCCESS;
    int trace_table = getenv("VMRP_ARM_EXT_TRACE") != NULL;
    if (trace_table) {
        uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
        printf("arm_ext_executor: table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X\n", idx, r0, r1, r2, r3, lr);
    }
    /* ARM ext 常用 mr_getTime 忙等等待动画/定时器间隔。timerStart/Stop
     * 可能夹在同一段等待循环里，因此与 getTime 一起保留连续计数。 */
    if (idx != 33 && idx != 31 && idx != 32) {
        m->busy_wait_count = 0;
        m->busy_wait_start_ms = 0;
    }

    switch (idx) {
        case 0:
            ret = arm_alloc(m, r0);
            m->last_alloc_addr = ret;
            m->last_alloc_len = r0;
            if (ret) note_origin_mem_alloc(m, r0);
            {
                uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
                if (m->nested_loading && m->last_file_addr && lr >= m->last_file_addr && lr < m->last_file_addr + m->last_file_len && r0 > 0x1000) {
                    if (!m->outer_r9) {
                        m->outer_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                    }
                    if (!m->nested_return_addr) {
                        uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
                        uint32_t saved_lr = 0;
                        if (arm_ptr(m, sp + 12)) {
                            uc_mem_read(m->uc, sp + 12, &saved_lr, 4);
                            m->nested_return_addr = saved_lr;
                        }
                    }
                    reg_write32(m->uc, UC_ARM_REG_R9, ret + 4);
                    m->nested_loading = 0;
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: nested R9=0x%X after malloc 0x%X outer=0x%X return=0x%X\n",
                               ret + 4, r0, m->outer_r9, m->nested_return_addr);
                    }
                }
            }
            break;
        case 1:
            note_origin_mem_free(m, r1);
            ret = MR_SUCCESS;
            break;
        case 2: {
            uint32_t p = arm_alloc(m, r2);
            if (p) note_origin_mem_alloc(m, r2);
            if (p && r0 && r1) memcpy(arm_ptr(m, p), arm_ptr(m, r0), r1 < r2 ? r1 : r2);
            ret = p;
        } break;
        case 3:
            memcpy(arm_ptr(m, r0), arm_ptr(m, r1), r2);
            /* memcpy 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t cpy_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= cpy_end &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 4: memmove(arm_ptr(m, r0), arm_ptr(m, r1), r2); ret = r0; break;
        case 5: strcpy(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 6: strncpy(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 7: strcat(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 8: strncat(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 9: ret = memcmp(arm_ptr(m, r0), arm_ptr(m, r1), r2); break;
        case 10: ret = strcmp(arm_str(m, r0), arm_str(m, r1)); break;
        case 11: ret = strncmp(arm_str(m, r0), arm_str(m, r1), r2); break;
        case 13: { void *p = memchr(arm_ptr(m, r0), (int)r1, r2); ret = p ? arm_addr(m, p) : 0; } break;
        case 14:
            memset(arm_ptr(m, r0), (int)r1, r2);
            /* memset 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t set_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= set_end &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 12: ret = strcoll(arm_str(m, r0), arm_str(m, r1)); break;
        case 15: ret = strlen(arm_str(m, r0)); break;
        case 16: {
            char *hay = (char *)arm_ptr(m, r0);
            char *nee = (char *)arm_ptr(m, r1);
            if (!hay || !nee) { ret = 0; break; }
            char *p = strstr(hay, nee);
            ret = p ? arm_addr(m, p) : 0;
        } break;
        case 17: { char buf[1024]; ret = format_arm(m, buf, sizeof(buf), arm_str(m, r1), 2); strcpy(arm_ptr(m, r0), buf); } break;
        case 18: ret = atoi(arm_str(m, r0)); break;
        case 19: ret = (uint32_t)strtoul(arm_str(m, r0), NULL, r1); break;
        case 20: ret = mr_rand(); break;
        case 22:
            ret = mr_stop_ex((int16)r0);
            internal_slot_write(m, m->mr_state_slot, 0);
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
            break;
        case 25: {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            int nested = m->last_file_addr && lr >= m->last_file_addr && lr < m->last_file_addr + m->last_file_len;
            uint32_t p_addr = nested ? m->nested_p_addr : 0;
            int reuse_nested_p = p_addr && r1 == sizeof(mr_c_function_P_t) && arm_ptr(m, p_addr);
            uint32_t p_len = r1;
            /* wrapper ext 的 dispatch 入口会将 SP 设为 P + 0x20000（见 gxdzc 的
             * 0x800FC: mov sp, P+0x20000），无论 ext 请求的 P 大小是多少都假定
             * 有 128KB 栈空间。确保分配至少 EXT_WRAPPER_STACK_SIZE 避免栈溢出
             * 到代码段导致数据损坏。 */
            if (!nested && p_len < EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t)) {
                p_len = EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t);
            }
            if (!p_addr || r1 != sizeof(mr_c_function_P_t) || !arm_ptr(m, p_addr)) {
                p_addr = arm_alloc(m, p_len);
            }
            if (p_addr && !reuse_nested_p) memset(arm_ptr(m, p_addr), 0, p_len);
            if (nested) {
                memcpy(arm_ptr(m, m->last_file_addr + 4), &p_addr, 4);
                uint32_t wrapper_rw = 0;
                memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
                if (wrapper_rw) {
                    uint32_t callback_slot = wrapper_rw + 0x1A0u;
                    if (arm_ptr(m, callback_slot))
                        memcpy(arm_ptr(m, callback_slot), &r0, 4);
                }
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                arm_ext_clear_foreground_screen_owner(m);
                arm_ext_record_nested_module(m, m->last_file_addr,
                                             m->last_file_len, p_addr, r0);
                if (!m->primary_helper_addr) {
                    m->primary_helper_addr = r0;
                    m->primary_p_addr = p_addr;
                    m->primary_file_addr = m->last_file_addr;
                    m->primary_file_len = m->last_file_len;
                    /* Host table[25] only registers the child helper. Unlike
                     * cfunction.ext's internal loader, it has not called the
                     * child lifecycle handler yet, so mythroad.c must issue
                     * one foreground code=0 call after the wrapper returns. */
                    m->primary_host_init_pending = 1;
                    /* wrapper 的 suspend/resume 通过 extChunk[8] 向 game
                     * 派发 pauseApp/resumeApp。真机上 wrapper 在加载 nested
                     * ext 时设置此字段，VMRP 下可能未被初始化（保持为 0），
                     * 导致 suspend/resume 的 blx extChunk[8] 跳转到 0。
                     * 此处在首次设置 primary_helper 时补写。 */
                    uint32_t np = p_addr;
                    uint32_t ec = 0;
                    if (np && arm_ptr(m, np + 12))
                        memcpy(&ec, arm_ptr(m, np + 12), 4);
                    /* （extChunk[8] 修复移到 modal dispatch 路由中动态执行，
                     * 因为 wrapper ARM 代码会在运行中覆盖此字段） */
                } else if (m->primary_file_addr) {
                    memcpy(arm_ptr(m, m->primary_file_addr + 4), &p_addr, 4);
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: nested helper=0x%X P=0x%X len=%u primary=0x%X/0x%X\n",
                           r0, p_addr, r1, m->primary_helper_addr, m->primary_p_addr);
                }
                m->nested_loading = 1;
                m->nested_p_addr = 0;
            } else {
                m->helper_addr = r0;
                m->p_addr = p_addr;
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                arm_ext_clear_foreground_screen_owner(m);
                memcpy(arm_ptr(m, EXT_CODE_ADDR + 4), &m->p_addr, 4);
            }
            ret = p_addr ? MR_SUCCESS : MR_FAILED;
        } break;
        case 26: {
            char buf[1024];
            format_arm(m, buf, sizeof(buf), arm_str(m, r0), 1);
            mr_printf("%s", buf);
            ret = 0;
        } break;
        case 27: {
            uint32_t base = m->origin_mem_addr;
            uint32_t len = m->origin_mem_len;
            if (r0) memcpy(arm_ptr(m, r0), &base, 4);
            if (r1) memcpy(arm_ptr(m, r1), &len, 4);
            ret = base ? MR_SUCCESS : MR_FAILED;
        } break;
        case 28: ret = MR_SUCCESS; break;
        case 29: {
            void *bmp = arm_ptr(m, r0);
            uint16_t h = (uint16_t)arg_read(m, 4);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG drawBitmap bmp=0x%X x=%d y=%d w=%u h=%u currentP=0x%X currentH=0x%X activeP=0x%X activeH=0x%X\n",
                       r0, (int16)r1, (int16)r2, (uint16)r3,
                       h, m->current_p_addr,
                       m->current_helper_addr, m->active_p_addr,
                       m->active_helper_addr);
            }
            uint32_t claim_p = 0, claim_helper = 0;
            if (bmp && arm_ext_should_accept_screen_write(m, &claim_p,
                                                          &claim_helper)) {
                mr_drawBitmap(bmp, (int16)r1, (int16)r2, (uint16)r3, h);
                arm_ext_mirror_draw_bitmap_to_screen(m, r0, (int16)r1,
                                                     (int16)r2,
                                                     (uint16)r3, h);
                arm_ext_finish_accepted_screen_write(m, claim_p,
                                                     claim_helper);
            }
            ret = MR_SUCCESS;
        } break;
        case 30: {
            int width = 0;
            int height = 0;
            const char *bitmap = mr_getCharBitmap((uint16)r0, (uint16)r1, &width, &height);
            // 根据字体实际尺寸计算位图大小：gb12=18字节(12行×12位紧凑), gb16=32字节(16行×2字节)
            int bitmap_size = ((width * height) + 7) >> 3;
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &width, 4);
            if (r3 && arm_ptr(m, r3)) memcpy(arm_ptr(m, r3), &height, 4);
            if (bitmap) {
                if (!m->char_bitmap_addr) m->char_bitmap_addr = arm_alloc(m, 32);
                if (m->char_bitmap_addr) {
                    memcpy(arm_ptr(m, m->char_bitmap_addr), bitmap, bitmap_size);
                    ret = m->char_bitmap_addr;
                } else {
                    ret = 0;
                }
            } else {
                ret = 0;
            }
        } break;
        case 31:
            ret = mr_timerStart((uint16)r0);
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            arm_ext_record_timer_owner(m);
            break;
        case 32: {
            ret = mr_timerStop();
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            mr_timer_state = 0;
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
        } break;
        case 33: {
            ret = mr_getTime();
            /* 部分 ext 用忙等循环反复调 mr_getTime() 等待真实时间流逝（如
             * 菜单切换动画、定时器主循环）。真机上 OS 抢占让时钟自然推进；
             * 模拟器里需 usleep 让宿主时间走动。连续 200 次无其它 table 调
             * 用即视为忙等，每轮 usleep 5ms；超 3 秒强制停止 ARM 执行，
             * 让宿主侧定时器有机会触发再重新进入。 */
            m->busy_wait_count++;
            if (m->busy_wait_count == 1) {
                m->busy_wait_start_ms = ret;
            }
            if (m->busy_wait_count >= 200) {
                usleep(5 * 1000);
                m->busy_wait_count = 100;
                uint32_t elapsed_ms = ret - m->busy_wait_start_ms;
                if (elapsed_ms >= 3000) {
                    m->busy_wait_count = 0;
                    m->busy_wait_start_ms = 0;
                    reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                    uc_emu_stop(m->uc);
                }
            }
        } break;
        case 34: ret = mr_getDatetime(arm_ptr(m, r0)); break;
        case 35: ret = mr_getUserInfo(arm_ptr(m, r0)); break;
        case 36: ret = mr_sleep(r0); break;
        case 37: ret = mr_plat((int32)r0, (int32)r1); break;
        case 38: {
            uint32_t outputp_addr = r3;
            uint32_t output_lenp_addr = arg_read(m, 4);
            uint32_t arm_output = 0;
            int32 host_output_len = 0;
            uint8 *host_output = NULL;
            uint8 *input = (uint8 *)arm_ptr(m, r1);

            if (outputp_addr) {
                uc_mem_read(m->uc, outputp_addr, &arm_output, 4);
                host_output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
            }
            if (output_lenp_addr) {
                uc_mem_read(m->uc, output_lenp_addr, &host_output_len, 4);
            }

            ret = mr_platEx((int32)r0, input, (int32)r2,
                            outputp_addr ? &host_output : NULL,
                            output_lenp_addr ? &host_output_len : NULL,
                            NULL);

            if (outputp_addr) {
                uint32_t new_arm_output = arm_output;
                if (host_output && host_output_len > 0) {
                    void *old_arm_ptr = arm_output ? arm_ptr(m, arm_output) : NULL;
                    if (host_output != old_arm_ptr) {
                        new_arm_output = arm_alloc(m, (uint32_t)host_output_len + 1);
                        if (new_arm_output) {
                            memcpy(arm_ptr(m, new_arm_output), host_output, (uint32_t)host_output_len);
                            ((uint8 *)arm_ptr(m, new_arm_output))[host_output_len] = '\0';
                        }
                    }
                } else {
                    new_arm_output = 0;
                }
                uc_mem_write(m->uc, outputp_addr, &new_arm_output, 4);
            }
            if (output_lenp_addr) {
                uc_mem_write(m->uc, output_lenp_addr, &host_output_len, 4);
            }
        } break;
        case 39: ret = mr_ferrno(); break;
        case 40: {
            const char *open_name = arm_str(m, r0);
            ret = mr_open(open_name, r1);
            /* 磁盘上找不到时尝试从 MRP 缓存提供虚拟 fd */
            if (ret == 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, open_name);
                if (ce) {
                    ret = mrp_vfd_open(m, ce->data, ce->data_len);
                }
            }
        } break;
        case 41: {
            MrpVirtualFd *vf = mrp_vfd_get(m, r0);
            if (vf) { vf->in_use = 0; ret = MR_SUCCESS; }
            else ret = mr_close((int32)r0);
        } break;
        case 42: {
            const char *info_name = arm_str(m, r0);
            ret = mr_info(info_name);
            /* 磁盘上不存在时检查 MRP 缓存 */
            if (ret != MRP_IS_FILE && m->mrp_cache_count > 0) {
                if (mrp_cache_find(m, info_name))
                    ret = MRP_IS_FILE;
            }
        } break;
        case 43: {
            void *src = arm_ptr(m, r1);
            uint32_t len = r2;
            int substituted = 0;
            void *new_src = NULL;
            uint32_t new_len = 0;
            if (m->profile && m->profile->intercept_write &&
                m->profile->intercept_write(m, m->app_state, r0, r1, r2,
                                            &new_src, &new_len)) {
                src = new_src;
                len = new_len;
                substituted = 1;
            }
            ret = mr_write((int32)r0, src, len);
            if (substituted) {
                if (m->profile && m->profile->post_write_cleanup)
                    m->profile->post_write_cleanup(m->app_state);
                if (ret == (int32_t)len) ret = (int32_t)r2;
            }
        } break;
        case 44: {
            void *hp = arm_ptr(m, r1);
            MrpVirtualFd *vf44 = mrp_vfd_get(m, r0);
            if (vf44) {
                uint32_t avail = vf44->pos < vf44->data_len ? vf44->data_len - vf44->pos : 0;
                uint32_t n = r2 < avail ? r2 : avail;
                if (n && hp) memcpy(hp, vf44->data + vf44->pos, n);
                vf44->pos += n;
                ret = (int32_t)n;
            } else {
                ret = mr_read((int32)r0, hp, r2);
            }
            if (m->profile && m->profile->post_read_hook)
                m->profile->post_read_hook(m, m->app_state, r1, r2, ret, hp);
            /* dump/restore 读回整块模块内存后，需要：
             * 1) 修复 GOT 中的 bridge 函数指针（文件数据中是原始未重定位的值）
             * 2) invalidate Unicorn TB cache（否则执行旧翻译） */
            if ((int32_t)ret > 0 && (uint32_t)ret > 0x1000) {
                /* 仅在 dump0 恢复时（目标地址匹配）才恢复间隙数据 */
                uc_ctl_remove_cache(m->uc, r1, r1 + (uint32_t)ret);
                arm_ext_restore_primary_mapping_after_dump0(m, r1, (uint32_t)ret);
                /* 修复 GOT：只恢复已记录的 bridge 函数指针 */
                if (m->got_snapshot_base) {
                    uint32_t got_base = m->got_snapshot_base;
                    uint32_t read_end = r1 + (uint32_t)ret;
                    for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                        uint32_t addr = got_base + i * 4;
                        if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                            m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                            addr >= r1 && addr + 4 <= read_end &&
                            !app_should_protect_got_addr(m, addr)) {
                            memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                        }
                    }
                }
            }
        } break;
        case 45: {
            MrpVirtualFd *vf45 = mrp_vfd_get(m, r0);
            if (vf45) {
                /* SEEK_SET=0, SEEK_CUR=1, SEEK_END=2 */
                int32_t new_pos;
                if ((int)r2 == 0) new_pos = (int32_t)r1;
                else if ((int)r2 == 1) new_pos = (int32_t)vf45->pos + (int32_t)r1;
                else new_pos = (int32_t)vf45->data_len + (int32_t)r1;
                if (new_pos < 0) new_pos = 0;
                if ((uint32_t)new_pos > vf45->data_len) new_pos = (int32_t)vf45->data_len;
                vf45->pos = (uint32_t)new_pos;
                ret = MR_SUCCESS;
            } else {
                ret = mr_seek((int32)r0, (int32)r1, (int)r2);
            }
        } break;
        case 46: {
            ret = mr_getLen(arm_str(m, r0));
            if (ret < 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, arm_str(m, r0));
                if (ce) ret = (int32_t)ce->data_len;
            }
        } break;
        case 47: ret = mr_remove(arm_str(m, r0)); break;
        case 48: ret = mr_rename(arm_str(m, r0), arm_str(m, r1)); break;
        case 49: ret = mr_mkDir(arm_str(m, r0)); break;
        case 50: ret = mr_rmDir(arm_str(m, r0)); break;
        case 51: ret = mr_findStart(arm_str(m, r0), arm_ptr(m, r1), r2); break;
        case 52: ret = mr_findGetNext((int32)r0, arm_ptr(m, r1), r2); break;
        case 53: ret = mr_findStop((int32)r0); break;
        case 54: ret = mr_exit(); break;
        case 55: ret = mr_startShake((int32)r0); break;
        case 56: ret = mr_stopShake(); break;
        case 57: ret = mr_playSound((int)r0, arm_ptr(m, r1), r2, (int32)r3); break;
        case 58: ret = mr_stopSound((int)r0); break;
        /* table[59] mr_sendSms(pNumber, pContent, encode) */
        case 59: ret = mr_sendSms(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 60: mr_call(arm_str(m, r0)); ret = MR_SUCCESS; break;
        /* table[61] mr_getNetworkID()：返回 0 表示移动网络（MR_NET_ID_MOBILE） */
        case 61: ret = 0; break;
        /* table[81] mr_initNetwork(cb, mode)：读取 mode 并设置 isCMWAP 标志，
         * 同步返回成功。桌面端没有真实 GPRS 网络，但需要 isCMWAP 控制后续
         * send/recv 的代理模拟逻辑。 */
        case 81: {
            const char *mode_str = arm_str(m, r1);
            ret = mr_initNetwork(NULL, mode_str);
        } break;
        /* table[82] mr_closeNetwork()：return success，跟 mr_initNetwork 配对。 */
        case 82: ret = MR_SUCCESS; break;
        /* table[83] mr_getHostByName(name, cb)。
         * 异步回调 cb 是 ARM 虚拟地址，不能在宿主线程调用（会把 ARM
         * 地址当宿主函数指针，段错误）。绕过 DSM 回调链直接调用底层
         * my_getHostByName(cb=NULL) 走同步 DNS 解析路径，返回 IP。 */
        case 83: {
            const char *host = arm_str(m, r0);
            ret = my_getHostByName(NULL, host, NULL, NULL);
        } break;
        case 84: ret = mr_socket((int32)r0, (int32)r1); break;
        case 85: ret = mr_connect((int32)r0, (int32)r1, (uint16)r2, (int32)r3); break;
        case 86: ret = mr_closeSocket((int32)r0); break;
        case 87: ret = mr_recv((int32)r0, arm_ptr(m, r1), (int)r2); break;
        case 88: { uint32_t a4 = arg_read(m, 4);
                   ret = mr_recvfrom((int32)r0, arm_ptr(m, r1), (int)r2,
                                    (int32 *)arm_ptr(m, r3), (uint16 *)arm_ptr(m, a4));
                 } break;
        case 89: ret = mr_send((int32)r0, arm_ptr(m, r1), (int)r2); break;
        case 90: { uint32_t a4 = arg_read(m, 4);
                   ret = mr_sendto((int32)r0, arm_ptr(m, r1), (int)r2, (int32)r3, (uint16)a4);
                 } break;
        case 69: ret = mr_dialogCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 70: ret = mr_dialogRelease((int32)r0); break;
        case 71: ret = mr_dialogRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2), (int32)r3); break;
        case 72: ret = mr_textCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 73: ret = mr_textRelease((int32)r0); break;
        case 74: ret = mr_textRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2)); break;
        case 75: ret = mr_editCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2, (int32)r3); break;
        case 76: ret = mr_editRelease((int32)r0); break;
        case 77: {
            const char *text = mr_editGetText((int32)r0);
            ret = alloc_string(m, text ? text : "");
        } break;
        /* table[78] mr_winCreate / table[79] mr_winRelease: 窗口功能不支持 */
        case 78: ret = MR_IGNORE; break;
        case 79: ret = MR_IGNORE; break;
        case 80: ret = mr_getScreenInfo(arm_ptr(m, r0)); break;
        case 113: {
            void *p = arm_ptr(m, r0);
            if (p) mr_md5_init(p);
            ret = 0;
        } break;
        case 114: {
            void *p = arm_ptr(m, r0);
            void *d = arm_ptr(m, r1);
            if (p && d) mr_md5_append(p, d, (int)r2);
            ret = 0;
        } break;
        case 115: {
            void *p = arm_ptr(m, r0);
            void *digest = arm_ptr(m, r1);
            if (p && digest) mr_md5_finish(p, digest);
            ret = 0;
        } break;
        case 118:
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG DispUpEx x=%d y=%d w=%u h=%u currentP=0x%X currentH=0x%X activeP=0x%X activeH=0x%X\n",
                       (int16)r0, (int16)r1, (uint16)r2, (uint16)r3,
                       m->current_p_addr, m->current_helper_addr,
                       m->active_p_addr, m->active_helper_addr);
            }
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    ret = _DispUpEx((int16)r0, (int16)r1,
                                    (uint16)r2, (uint16)r3);
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                } else {
                    ret = 0;
                }
            }
            break;
        case 119:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    _DrawPoint((int16)r0, (int16)r1, (uint16)r2);
                    m->screen_dirty = 1;
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                }
            }
            ret = 0;
            break;
        case 120:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    _DrawBitmap(arm_ptr(m, r0), (int16)r1, (int16)r2,
                                (uint16)r3, (uint16)arg_read(m, 4),
                                (uint16)arg_read(m, 5),
                                (uint16)arg_read(m, 6),
                                (int16)arg_read(m, 7),
                                (int16)arg_read(m, 8),
                                (int16)arg_read(m, 9));
                    m->screen_dirty = 1;
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                }
            }
            ret = 0;
            break;
        case 122:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    DrawRect((int16)r0, (int16)r1, (int16)r2, (int16)r3,
                             (uint8)arg_read(m, 4),
                             (uint8)arg_read(m, 5),
                             (uint8)arg_read(m, 6));
                    m->screen_dirty = 1;
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                }
            }
            ret = 0;
            break;
        case 123:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    ret = _DrawText(arm_str(m, r0), (int16)r1, (int16)r2,
                                    (uint8)r3, (uint8)arg_read(m, 4),
                                    (uint8)arg_read(m, 5),
                                    (int)arg_read(m, 6),
                                    (uint16)arg_read(m, 7));
                    m->screen_dirty = 1;
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                } else {
                    ret = 0;
                }
            }
            break;
        case 124: ret = _BitmapCheck(arm_ptr(m, r0), (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4), (uint16)arg_read(m, 5), (uint16)arg_read(m, 6)); break;
        case 125: {
            int fl = 0;
            uint32_t packp = 0, ramp_slot = 0, raml_slot = 0, ramp = 0, raml = 0;
            const char *read_name = arm_str(m, r0);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 100 * 4, &packp, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 104 * 4, &ramp_slot, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 105 * 4, &raml_slot, 4);
            if (ramp_slot) uc_mem_read(m->uc, ramp_slot, &ramp, 4);
            if (raml_slot) uc_mem_read(m->uc, raml_slot, &raml, 4);

            void *hp = NULL;
            const char *pack = arm_str(m, packp);
            if (pack[0] == '$' && ramp && raml) {
                hp = mr_readFile_from_ram(read_name, &fl, (int)r2, arm_ptr(m, ramp), (int)raml);
            } else {
                hp = _mr_readFile(read_name, &fl, (int)r2);
            }
            if (!hp) { ret = 0; break; }
            uint32_t ap = arm_alloc(m, (uint32_t)fl);
            memcpy(arm_ptr(m, ap), hp, fl);
            free(m->last_file_copy);
            m->last_file_copy = malloc((size_t)fl);
            if (m->last_file_copy) {
                memcpy(m->last_file_copy, hp, (size_t)fl);
                m->last_file_addr = ap;
                m->last_file_len = (uint32_t)fl;
            }
            if (r1) memcpy(arm_ptr(m, r1), &fl, 4);
            ret = ap;
        } break;
        case 126: ret = wstrlen(arm_str(m, r0)); break;
        case 127: ret = mr_registerAPP(arm_ptr(m, r0), (int32)r1, (int32)r2); break;
        case 130: ret = _mr_TestCom(NULL, (int)r1, (int)r2); break;
        case 131:
            ret = _mr_TestCom1(NULL, (int)r1, arm_ptr(m, r2), (int32)r3);
            /*
             * cfunction.ext has a private child loader that never calls host
             * table[25].  Its disassembly at 0xE8339C..0xE833C8 proves the
             * loader writes file_base[0]=module record, file_base[4]=child P,
             * extChunk[12]=file_base and extChunk[16]=len before mr_cacheSync.
             * Once that extChunk exists, the staging image is already the
             * wrapper-owned runtime form and must not be overwritten with the
             * raw bytes cached by _mr_readFile.
             */
            int internal_loader_staging =
                (r1 == 9 && arm_ext_has_internal_loader_chunk(m, r2, r3));
            if (r1 == 9 && internal_loader_staging &&
                m->last_file_copy && r2 && r3 <= m->last_file_len &&
                r3 > 8 && arm_ptr(m, r2 + 8)) {
                memcpy(arm_ptr(m, r2 + 8), m->last_file_copy + 8, r3 - 8);
                /*
                 * file_base[0] 是私有 loader 写入的 module record 指针——它是
                 * EXT_TABLE 的逐槽镜像，被子模块当作自己的 C 函数表基址使用。
                 * 该 record 把 table[125] (_mr_readFile) 重定向到 wrapper 自身的
                 * readFile (0xE819xx)。wrapper 的 readFile 工作在 wrapper 的内存
                 * 模型里，返回的是挂载/索引缓冲(0x66xxxx 区)而非宿主 readFile 解码
                 * 出的位图像素；子模块据此取位图源指针时拿到的是文件索引/文件名字节，
                 * 绘制即花屏。把 record 的 readFile 槽还原成宿主 EXT_TABLE 的桥接值，
                 * 让子模块的资源读取走宿主 readFile，得到正确解码的位图。
                 * 仅还原 readFile 槽：malloc/free/loader(table[25]) 仍走 wrapper，
                 * 以保留 wrapper 对子模块内存与子加载的管理。
                 */
                uint32_t record_addr = 0;
                memcpy(&record_addr, arm_ptr(m, r2), 4);
                if (record_addr && arm_ptr(m, record_addr + 125 * 4)) {
                    memcpy(arm_ptr(m, record_addr + 125 * 4),
                           arm_ptr(m, EXT_TABLE_ADDR + 125 * 4), 4);
                }
            }
            if (r1 == 9 && !internal_loader_staging &&
                m->last_file_copy && r2 && r3 <= m->last_file_len &&
                arm_ptr(m, r2)) {
                memcpy(arm_ptr(m, r2), m->last_file_copy, r3);
                uint32_t table_addr = EXT_TABLE_ADDR;
                memcpy(arm_ptr(m, r2), &table_addr, 4);
                m->last_file_addr = r2;
                m->last_file_len = r3;
                if (m->last_alloc_len == sizeof(mr_c_function_P_t) && arm_ptr(m, m->last_alloc_addr)) {
                    m->nested_p_addr = m->last_alloc_addr;
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: staged nested ext at 0x%X len=%u P=0x%X\n",
                           r2, r3, m->nested_p_addr);
                }
            }
            /*
             * 关键修复：case 131 (r1=9) 是 mr_cacheSync。原意是同步指令缓存，
             * 因为 wrapper 刚把新的 .ext 二进制写到了 [r2, r2+r3) 范围。在真机
             * 上这会清掉 D-cache 并 invalidate I-cache，让后续取指看到新指令。
             *
             * 我们用 Unicorn 跑 ARM，Unicorn 通过 QEMU TCG 做 JIT 翻译并 cache
             * translation blocks（TB）。当 gghjt 把 netpay 的 smcheck/advsms
             * 等子插件 staging 到同一段内存时，TB cache 仍然是上一个插件留下的
             * 翻译结果，结果就是 PC 进了新 .ext 的代码区，执行的却是旧代码。
             *
             * 表现为 r3 不被 LDR 重新加载、movs r1/r2 立即数被跳过等"鬼指令"，
             * 最后 BLX r3 跳到一个看着像数据/字符串的地址，崩在 0x627A60 一类
             * 位置。修复方式就是 mimic 真机的 mr_cacheSync：在 staging 完成后
             * 通过 uc_ctl_remove_cache 让 Unicorn 把这段地址的 TB 翻译丢掉，
             * 下次执行强制重新翻译现在的字节。
             */
            if (r1 == 9 && r2 && r3 > 0 && arm_ptr(m, r2) && arm_ptr(m, r2 + r3 - 1)) {
                /*
                 * The internal loader overwrites the MRPGCMAP header before
                 * mr_cacheSync: file_base[0] becomes the module record and
                 * file_base[4] becomes the child P pointer.  The authoritative
                 * check is therefore the extChunk magic/file/length tuple.
                 */
                m->pending_internal_file_addr = r2;
                m->pending_internal_file_len = r3;
                arm_ext_sync_internal_nested_module(m, r2, r3);
                uc_err cerr = uc_ctl_remove_cache(m->uc, r2, r2 + r3);
                if (cerr != UC_ERR_OK && getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: uc_ctl_remove_cache(0x%X, 0x%X) failed: %u\n",
                           r2, r2 + r3, cerr);
                }
            }
            break;
        case 132: {
            int err_pos = -1;
            int out_size = 0;
            uint16 *host = c2u(arm_str(m, r0), r1 ? &err_pos : NULL, &out_size);
            if (r1 && arm_ptr(m, r1)) memcpy(arm_ptr(m, r1), &err_pos, 4);
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &out_size, 4);
            if (host && out_size > 0) {
                uint32_t out_addr = arm_alloc(m, (uint32_t)out_size);
                if (out_addr) {
                    memcpy(arm_ptr(m, out_addr), host, (uint32_t)out_size);
                }
                mr_free(host, (uint32)out_size);
                ret = out_addr;
            } else {
                ret = 0;
            }
        } break;
        case 133: ret = ((int32)r1 == 0) ? 0 : (int32)r0 / (int32)r1; break;
        case 134: ret = ((int32)r1 == 0) ? 0 : (int32)r0 % (int32)r1; break;
        case 145:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (arm_ext_should_accept_screen_write(m, &claim_p,
                                                       &claim_helper)) {
                    mr_platDrawChar((uint16)r0, (int32)r1, (int32)r2,
                                    (uint32)r3);
                    m->screen_dirty = 1;
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                }
            }
            ret = 0;
            break;
        default:
            printf("arm_ext_executor: table[%u] not implemented (r0=0x%X r1=0x%X r2=0x%X r3=0x%X)\n", idx, r0, r1, r2, r3);
            ret = MR_IGNORE;
            break;
    }
    cb_ret(m, ret);
}


static bool hook_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (type == UC_MEM_FETCH_UNMAPPED && address == 0) {
        uint32_t lr = 0;
        uc_reg_read(uc, UC_ARM_REG_LR, &lr);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: treated fetch from 0 as return to 0x%X\n", lr);
            dumpREG(uc);
        }
        if (lr) {
            uint32_t cpsr = 0;
            uc_reg_read(uc, UC_ARM_REG_CPSR, &cpsr);
            if (lr & 1u) cpsr |= (1u << 5);
            else cpsr &= ~(1u << 5);
            uc_reg_write(uc, UC_ARM_REG_CPSR, &cpsr);
            uc_reg_write(uc, UC_ARM_REG_PC, &lr);
        } else {
            uint32_t pc = EXT_STOP_ADDR;
            uc_reg_write(uc, UC_ARM_REG_PC, &pc);
            uc_emu_stop(uc);
        }
        return true;
    }
    {
        uint32_t pc = reg_read32(uc, UC_ARM_REG_PC);
        uint32_t sp = reg_read32(uc, UC_ARM_REG_SP);
        uint32_t cpsr = reg_read32(uc, UC_ARM_REG_CPSR);
        int thumb = (cpsr >> 5) & 1;
        printf("arm_ext_executor: invalid memory %s addr=0x%llX size=%d value=0x%llX\n",
               memTypeStr(type), (unsigned long long)address, size, (unsigned long long)value);
        printf("  crash PC=0x%X (%s) last_file=0x%X..0x%X\n",
               pc, thumb ? "thumb" : "arm",
               m->last_file_addr, m->last_file_addr + m->last_file_len);
        /* 打印 crash PC 前32字节和后16字节，便于反汇编定位上下文 */
        {
            uint32_t pre_start = (pc > 32) ? (pc - 32) : 0;
            uint32_t pre_len = pc - pre_start;
            uint8_t pre_bytes[32];
            if (pre_len > 0 && uc_mem_read(uc, pre_start, pre_bytes, pre_len) == UC_ERR_OK) {
                printf("  bytes @0x%X (before PC):", pre_start);
                for (uint32_t i = 0; i < pre_len; i++) printf(" %02X", pre_bytes[i]);
                printf("\n");
            }
        }
        uint8_t bytes[16];
        if (uc_mem_read(uc, pc, bytes, 16) == UC_ERR_OK) {
            printf("  bytes @0x%X (at PC):", pc);
            for (int i = 0; i < 16; i++) printf(" %02X", bytes[i]);
            printf("\n");
        }
        /* 导出 crash PC 前后各256字节的二进制，供 arm-none-eabi-objdump 反汇编 */
        {
            uint32_t dump_start = (pc > 256) ? (pc - 256) : 0;
            uint32_t dump_end = pc + 256;
            uint32_t dump_len = dump_end - dump_start;
            uint8_t *dump_buf = malloc(dump_len);
            if (dump_buf && uc_mem_read(uc, dump_start, dump_buf, dump_len) == UC_ERR_OK) {
                FILE *df = fopen("/tmp/vmrp_crash.bin", "wb");
                if (df) {
                    fwrite(dump_buf, 1, dump_len, df);
                    fclose(df);
                    printf("  [CRASH_DUMP] saved %u bytes [0x%X..0x%X] to /tmp/vmrp_crash.bin\n",
                           dump_len, dump_start, dump_end);
                    printf("  [CRASH_DUMP] disassemble: arm-none-eabi-objdump -D -b binary "
                           "-m arm -M force-thumb --adjust-vma=0x%X /tmp/vmrp_crash.bin\n",
                           dump_start);
                }
            }
            free(dump_buf);
        }
        /* 导出栈内容（SP 向上64字节） */
        {
            uint8_t stack_buf[64];
            if (uc_mem_read(uc, sp, stack_buf, sizeof(stack_buf)) == UC_ERR_OK) {
                printf("  stack @0x%X:", sp);
                for (int i = 0; i < 64; i += 4) {
                    uint32_t val = stack_buf[i] | (stack_buf[i+1]<<8)
                                 | (stack_buf[i+2]<<16) | (stack_buf[i+3]<<24);
                    printf(" 0x%08X", val);
                }
                printf("\n");
            }
        }
    }
    dumpREG(uc);
    return false;
}

/* GOT 写监控 */
static void hook_got_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (size != 4) return;
    uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
    uint32_t got_size = EXT_TABLE_COUNT * 4;
    if (!r9 || (uint32_t)address < r9 || (uint32_t)address >= r9 + got_size) return;
    uint32_t idx = ((uint32_t)address - r9) / 4;
    if (idx >= EXT_TABLE_COUNT) return;
    if ((uint32_t)value >= EXT_TABLE_ADDR &&
        (uint32_t)value < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) {
        /*
         * 观测真实重定位偏移：ARM 代码把 memcpy 桥(table[3]=EXT_TABLE_ADDR+0xC)
         * 写入某个 rw_base 的 GOT 时，记录其 R9 相对偏移到对应嵌套模块记录上。
         * 这是私有 loader 子模块 GOT 桥块真实基址的可靠信号——bridge 修复据此把
         * 描述符平移到模块代码实际读取的偏移，而不是写死的 app 专用偏移。 */
        if ((uint32_t)value == EXT_TABLE_ADDR + 0xCu) {
            for (int gi = 0; gi < m->nested_module_count; ++gi) {
                uint32_t nrw = 0;
                if (arm_ptr(m, m->nested_modules[gi].p_addr))
                    memcpy(&nrw, arm_ptr(m, m->nested_modules[gi].p_addr), 4);
                if (nrw && nrw == r9) {
                    m->nested_modules[gi].got_memcpy_off = (uint32_t)address - r9;
                    break;
                }
            }
        }
        if (app_should_protect_got_addr(m, (uint32_t)address)) {
            return;
        }
        /* bridge 函数指针写入 → 记录快照。只在 got_snapshot_base 尚未
         * 设置或与当前 R9 一致时更新 base，防止嵌套插件（如 netpay）
         * 的 GOT 初始化覆盖 wrapper 的 snapshot base */
        if (!m->got_snapshot_base || m->got_snapshot_base == r9) {
            m->got_snapshot_base = r9;
        }
        /* 只记录属于 snapshot base 所属 GOT 的写入 */
        if (m->got_snapshot_base == r9) {
            m->got_snapshot[idx] = (uint32_t)value;
        }
    }
    /* 非 bridge 值覆盖时保留已有的快照不清除——netpay 等嵌套插件会将
     * GOT 中的 bridge 指针临时替换为自己的回调地址，但 dump0 restore
     * 读回整块内存后需要把这些槽位恢复为原始 bridge 指针 */
}

static void hook_screen_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    m->screen_write_count++;
    m->screen_dirty = 1;
    if (getenv("VMRP_ARM_EXT_TRACE") && m->screen_write_count <= 20) {
        printf("arm_ext_executor: screen write #%u addr=0x%llX size=%d value=0x%llX\n",
               m->screen_write_count, (unsigned long long)address, size, (unsigned long long)value);
    }
}

static void hook_restore_r9(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (m->outer_r9 && m->nested_return_addr && (uint32_t)address == (m->nested_return_addr & ~1u)) {
        uc_reg_write(uc, UC_ARM_REG_R9, &m->outer_r9);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: restored outer R9=0x%X at 0x%X\n",
                   m->outer_r9, (uint32_t)address);
        }
        m->outer_r9 = 0;
        m->nested_return_addr = 0;
        return;
    }
    /*
     * wrapper dispatch can BLX into callbacks retained by older nested EXTs
     * (gxdzc netpay: 0xE83B40 -> smsend.ext 0x66FFBD). Those callbacks use
     * R9-relative globals, so R9 must follow the code range currently being
     * executed rather than the last active helper. The hook runs per basic
     * block and only writes R9 when PC enters a known EXT image. Verification:
     * test/gxdzc/pay.sh reaches the payment UI and ctest stays green.
     */
    arm_ext_sync_r9_for_code_addr(m, (uint32_t)address);
}

static uint32_t find_wrapper_timer_dispatch(const uint8_t *code, uint32_t len) {
    static const uint8_t pat[] = {
        0x00, 0x48, 0xF8, 0xB5, 0x78, 0x44, 0x80, 0x6B,
        0x80, 0x30, 0x40, 0x68, 0x80, 0x47, 0x00, 0x25,
        0x00, 0x4E, 0x4E, 0x44, 0x71, 0x69, 0x75, 0x61,
        0x41, 0x1A, 0xF0, 0x68, 0x0B, 0x1C, 0x00, 0x28,
    };

    if (!code || len < sizeof(pat)) return 0;
    for (uint32_t off = 0; off + sizeof(pat) <= len; off += 2) {
        int match = 1;
        for (uint32_t i = 0; i < sizeof(pat); ++i) {
            /* gxdzc cfunction.ext 反汇编确认 0xE83A80 是 wrapper timer
             * queue dispatcher：它从 wrapper RW+0x1FC 取队列并在 0xE83B46
             * BLX 节点回调。Thumb LDR literal 的立即数字段随代码布局变，
             * 其余指令序列稳定。兼容性：只在扫描到该队列消费函数时启用
             * wrapper timer dispatch；验证 test/gxdzc/pay.sh 与 ctest。 */
            if (i == 0 || i == 16) continue;
            if (code[off + i] != pat[i]) {
                match = 0;
                break;
            }
        }
        if (match) return EXT_CODE_ADDR + off + 1u;
    }
    return 0;
}

static void patch_wrapper_stack_size(ArmExtModule *m) {
    struct PatchWord {
        uint32_t old_value;
        uint32_t new_value;
    };
    static const struct PatchWord patches[] = {
        {0xe2806d80u, 0xe2806802u}, /* add r6, r0, #0x2000 -> #0x20000 */
        {0xe280bd80u, 0xe280b802u}, /* add fp, r0, #0x2000 -> #0x20000 */
        {0xe2806dc0u, 0xe2806802u}, /* add r6, r0, #0x3000 -> #0x20000 */
        {0xe280bdc0u, 0xe280b802u}, /* add fp, r0, #0x3000 -> #0x20000 */
    };
    uint32_t patch_len = m->code_len < 0x200u ? m->code_len : 0x200u;
    for (uint32_t off = 0; off + 4 <= patch_len; off += 4) {
        uint32_t value = 0;
        memcpy(&value, arm_ptr(m, EXT_CODE_ADDR + off), 4);
        for (size_t i = 0; i < sizeof(patches) / sizeof(patches[0]); ++i) {
            if (value == patches[i].old_value) {
                memcpy(arm_ptr(m, EXT_CODE_ADDR + off), &patches[i].new_value, 4);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: patched wrapper stack at 0x%X\n", EXT_CODE_ADDR + off);
                }
                break;
            }
        }
    }
}

static void init_table(ArmExtModule *m) {
    for (uint32_t i = 0; i < EXT_TABLE_COUNT; ++i) {
        write_table_entry(m, i, EXT_TABLE_ADDR + i * 4);
    }
    /* Shadow storage for table data slots used by C EXT code. */
    for (uint32_t i = 91; i <= 112; ++i) {
        write_table_entry(m, i, alloc_u32_slot(m, 0));
    }
    write_table_entry(m, 135, alloc_u32_slot(m, 0));
    write_table_entry(m, 136, alloc_u32_slot(m, 0));
    write_table_entry(m, 139, alloc_u32_slot(m, 0));
    write_table_entry(m, 140, alloc_u32_slot(m, 0));
    write_table_entry(m, 142, alloc_u32_slot(m, 0));
    write_table_entry(m, 143, alloc_u32_slot(m, 0));
    write_table_entry(m, 146, alloc_u32_slot(m, 0));

    m->mr_m0_files_addr = arm_alloc(m, 50 * 4);
    m->vm_state_slot = alloc_u32_slot(m, 0);
    m->mr_state_slot = alloc_u32_slot(m, 1); /* MR_STATE_RUN */
    m->bi_slot = alloc_u32_slot(m, 0);
    m->mr_timer_p_slot = alloc_u32_slot(m, 0);
    m->mr_timer_state_slot = alloc_u32_slot(m, 0); /* MR_TIMER_STATE_IDLE */
    m->mr_timer_run_without_pause_slot = alloc_u32_slot(m, 0);
    m->mr_gzin_buf_slot = alloc_u32_slot(m, 0);
    m->mr_gzout_buf_slot = alloc_u32_slot(m, 0);
    m->lg_gzinptr_slot = alloc_u32_slot(m, 0);
    m->lg_gzoutcnt_slot = alloc_u32_slot(m, 0);
    m->mr_sms_cfg_need_save_slot = alloc_u32_slot(m, 0);
    m->internal_table_addr = arm_alloc(m, 17 * 4);
    m->port_table_addr = arm_alloc(m, 4 * 4);
    if (m->internal_table_addr) {
        uint32_t internal[17] = {
            m->mr_m0_files_addr,
            m->vm_state_slot,
            m->mr_state_slot,
            m->bi_slot,
            m->mr_timer_p_slot,
            m->mr_timer_state_slot,
            m->mr_timer_run_without_pause_slot,
            m->mr_gzin_buf_slot,
            m->mr_gzout_buf_slot,
            m->lg_gzinptr_slot,
            m->lg_gzoutcnt_slot,
            m->mr_sms_cfg_need_save_slot,
            EXT_TABLE_ADDR + 12 * 4,
            EXT_TABLE_ADDR + 13 * 4,
            EXT_TABLE_ADDR + 14 * 4,
            EXT_TABLE_ADDR + 15 * 4,
            0,
        };
        memcpy(arm_ptr(m, m->internal_table_addr), internal, sizeof(internal));
        write_table_entry(m, 23, m->internal_table_addr);
    }
    if (m->port_table_addr) {
        write_table_entry(m, 24, m->port_table_addr);
    }

    int32_t sw = mr_screen_w > 0 ? mr_screen_w : vmrp_config.screen_width;
    int32_t sh = mr_screen_h > 0 ? mr_screen_h : vmrp_config.screen_height;
    int32_t bit = 16;
    m->screen_w = sw;
    m->screen_h = sh;
    m->screen_len = (uint32_t)(sw * sh * 2);
    m->screen_addr = arm_alloc(m, m->screen_len);
    if (m->screen_addr && mr_screenBuf) {
        memcpy(arm_ptr(m, m->screen_addr), mr_screenBuf, m->screen_len);
    }
    m->screen_dirty = 0;

    write_table_entry(m, 91, alloc_u32_slot(m, m->screen_addr));
    write_table_entry(m, 92, alloc_u32_slot(m, (uint32_t)sw));
    write_table_entry(m, 93, alloc_u32_slot(m, (uint32_t)sh));
    write_table_entry(m, 94, alloc_u32_slot(m, (uint32_t)bit));

    const uint32_t bitmap_count = 31;
    const uint32_t bitmap_stride = 16;
    const uint32_t bitmapmax = 30;
    uint32_t bitmap_array = arm_alloc(m, bitmap_count * bitmap_stride);
    if (bitmap_array) {
        uint8_t *bm = (uint8_t *)arm_ptr(m, bitmap_array + bitmapmax * bitmap_stride);
        uint16_t sw16 = (uint16_t)sw;
        uint16_t sh16 = (uint16_t)sh;
        uint32_t type = 0;
        memcpy(bm + 0, &sw16, 2);
        memcpy(bm + 2, &sh16, 2);
        memcpy(bm + 4, &m->screen_len, 4);
        memcpy(bm + 8, &type, 4);
        memcpy(bm + 12, &m->screen_addr, 4);
        write_table_entry(m, 95, bitmap_array);
    }

    write_table_entry(m, 100, alloc_string(m, mr_get_pack_filename()));
    write_table_entry(m, 101, alloc_string(m, mr_get_start_filename()));
    write_table_entry(m, 102, alloc_string(m, mr_get_old_pack_filename()));
    write_table_entry(m, 103, alloc_string(m, mr_get_old_start_filename()));
    write_table_entry(m, 138, alloc_string(m, mr_get_start_fileparameter()));

    m->origin_mem_len = 4u * 1024u * 1024u;
    m->origin_mem_left = m->origin_mem_len;
    m->origin_mem_min = m->origin_mem_len;
    m->origin_mem_top = 0;
    /* 先分配统计 slot 和 free_head，再分配 origin_mem 池。ext 的 ARM
     * 分配器会在 origin_mem 池末尾写入元数据，如果 slot 紧跟池后面就会被覆盖。 */
    uint32_t slot108 = alloc_u32_slot(m, 0);
    uint32_t slot109 = alloc_u32_slot(m, 0);
    uint32_t slot110 = alloc_u32_slot(m, 0);
    uint32_t slot111 = alloc_u32_slot(m, 0);
    uint32_t slot135 = alloc_u32_slot(m, 0);
    uint32_t slot136 = alloc_u32_slot(m, 0);
    uint32_t free_head = arm_alloc(m, 8);
    m->origin_mem_addr = arm_alloc(m, m->origin_mem_len);
    if (m->origin_mem_addr) {
        uint32_t free_next = m->origin_mem_len;
        uint32_t free_len = m->origin_mem_len;
        memcpy(arm_ptr(m, m->origin_mem_addr), &free_next, 4);
        memcpy((uint8_t *)arm_ptr(m, m->origin_mem_addr) + 4, &free_len, 4);

        if (slot108) { uint32_t v = m->origin_mem_addr; memcpy(arm_ptr(m, slot108), &v, 4); }
        if (slot109) { uint32_t v = m->origin_mem_len; memcpy(arm_ptr(m, slot109), &v, 4); }
        if (slot110) { uint32_t v = m->origin_mem_addr + m->origin_mem_len; memcpy(arm_ptr(m, slot110), &v, 4); }
        if (slot111) { uint32_t v = m->origin_mem_left; memcpy(arm_ptr(m, slot111), &v, 4); }
        if (slot135) { uint32_t v = m->origin_mem_min; memcpy(arm_ptr(m, slot135), &v, 4); }
        if (slot136) { uint32_t v = m->origin_mem_top; memcpy(arm_ptr(m, slot136), &v, 4); }
        write_table_entry(m, 108, slot108);
        write_table_entry(m, 109, slot109);
        write_table_entry(m, 110, slot110);
        write_table_entry(m, 111, slot111);
        write_table_entry(m, 135, slot135);
        write_table_entry(m, 136, slot136);
        if (free_head) write_table_entry(m, 146, free_head);
    }
}

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code, int32 *ext_ret) {
    if (!out || !code || len < 12) return MR_FAILED;
    ArmExtModule *m = calloc(1, sizeof(*m));
    if (!m) return MR_FAILED;
    /* 在 EXT_BASE_ADDR 固定地址分配 ARM 内存，使 ARM 虚拟地址等于宿主指针，
     * 与 unicorn 模式下 uc_mem_map_ptr 的行为一致——ext 返回的指针可被 Lua 直接使用 */
#ifdef _MSC_VER
    m->mem = VirtualAlloc((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (m->mem == NULL) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#else
    m->mem = mmap((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m->mem == MAP_FAILED) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#endif
    if (!m->mem) goto fail;
    m->low_table = calloc(1, EXT_LOW_TABLE_SIZE);
    if (!m->low_table) goto fail;
    m->heap_top = EXT_HEAP_ADDR;
    m->code_len = len;
    m->host_code = code;

    m->profile = app_compat_select(mr_get_pack_filename());
    if (m->profile && m->profile->init)
        m->app_state = m->profile->init(m);

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &m->uc);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_BASE_ADDR, EXT_MEM_SIZE, UC_PROT_ALL, m->mem);
    if (err != UC_ERR_OK) goto fail;
    init_table(m);
    /* 预解析 MRP，缓存所有条目，避免 ARM ext 做极慢的 MRP 索引扫描 */
    parse_mrp_cache(m, mr_get_pack_filename());
    memcpy(m->low_table, m->mem, EXT_TABLE_COUNT * 4);
    err = uc_mem_map_ptr(m->uc, 0, EXT_LOW_TABLE_SIZE, UC_PROT_ALL, m->low_table);
    if (err != UC_ERR_OK) goto fail;
    void *code_dst = arm_ptr(m, EXT_CODE_ADDR);
    if (!code_dst) goto fail;
    memcpy(code_dst, code, len);
    m->wrapper_timer_dispatch_addr = find_wrapper_timer_dispatch(code, len);
    patch_wrapper_stack_size(m);
    uint32_t table = EXT_TABLE_ADDR;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4);
    err = uc_hook_add(m->uc, &m->hook, UC_HOOK_CODE, hook_table, m, EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4);
    if (err != UC_ERR_OK) goto fail;
    uc_hook low_zero_hook;
    err = uc_hook_add(m->uc, &low_zero_hook, UC_HOOK_CODE, hook_low_zero, m, 0, EXT_LOW_TABLE_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    if (getenv("VMRP_ARM_EXT_TRACE_PC")) {
        uc_hook pc_hook;
        err = uc_hook_add(m->uc, &pc_hook, UC_HOOK_CODE, trace_pc, m,
                          EXT_CODE_ADDR, EXT_CODE_ADDR + len);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook restore_hook;
    err = uc_hook_add(m->uc, &restore_hook, UC_HOOK_BLOCK, hook_restore_r9, m,
                      EXT_BASE_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook screen_hook;
    if (m->screen_addr && m->screen_len) {
        err = uc_hook_add(m->uc, &screen_hook, UC_HOOK_MEM_WRITE, hook_screen_write, m,
                          m->screen_addr, m->screen_addr + m->screen_len - 1);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook invalid_hook;
    err = uc_hook_add(m->uc, &invalid_hook, UC_HOOK_MEM_INVALID, hook_invalid, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;
    /* GOT bridge 指针追踪：ARM 代码向 R9 数据区写入 bridge 地址时记录，
     * 后续 dump/restore 覆盖时自动修复 */
    {
        uc_hook got_hook;
        uc_hook_add(m->uc, &got_hook, UC_HOOK_MEM_WRITE, hook_got_write, m,
                    EXT_HEAP_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    }

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int load_ret = run_arm(m, EXT_CODE_ADDR + 8);
    leave_screen_context(m, saved_screenBuf);
    if (load_ret != MR_SUCCESS) goto fail;
    capture_timer_dispatches(m);
    /* 将 ARM 代码 mr_c_function_load() 的返回值 (R0) 传给调用者，
     * 与非 native 路径中 mr_load_c_function(code) 的返回值语义一致 */
    if (ext_ret) *ext_ret = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
    *out = m;
    return MR_SUCCESS;
fail:
    arm_ext_unload(m);
    return MR_FAILED;
}


static int arm_ext_finish_callback_state(ArmExtModule *m, uint32_t ext_chunk) {
    if (!m) return 0;

    /* ARM ext 的退出/重启逻辑会直接写 mr_state_slot 设置
     * MR_STATE_RESTART(3) 或 MR_STATE_STOP(4)。真机上 ARM 代码与宿主共享
     * 同一全局变量；Unicorn 下 ARM 内存独立，所以回调结束后要同步一次。 */
    if (m->mr_state_slot) {
        uint32_t arm_state = 0;
        memcpy(&arm_state, arm_ptr(m, m->mr_state_slot), 4);
        if (arm_state >= 3) {
            mr_exit();
            return 1;
        }
    }

    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x34)) {
        return 0;
    }

    /* wrapper 反汇编确认 extChunk[0x34] 是 suspend 深度：
     *   0xE831A4 suspend: [0x34]++，首次进入时摘掉 wrapper timer 节点
     *   0xE83220 resume:  [0x34]--，回到 0 时重新挂回 timer 节点
     * 普通 wrapper 模态/子流程会把 active_helper_addr 切到后加载的子模块；
     * gxdzc 的资源下载提示则只摘掉 game timer，并把旧链表头保存在
     * saved_game_timer_head，取消时还要恢复。真正的“返回上层/停止自驱”
     * 路径没有前台子模块、没有可恢复的 game timer，且宿主定时器空闲。
     * 模拟器没有真机平台的上层调度器，因此只在这些条件都满足时调用
     * mr_exit()，把 wrapper 状态同步到宿主平台层。 */
    enum { MR_TIMER_STATE_IDLE = 0 };
    uint32_t suspend_depth = 0;
    memcpy(&suspend_depth, arm_ptr(m, ext_chunk + 0x34), 4);
    int foreground_child =
        m->primary_helper_addr &&
        m->active_helper_addr &&
        m->active_helper_addr != m->primary_helper_addr;
    if (suspend_depth > 0 &&
        mr_timer_state == MR_TIMER_STATE_IDLE &&
        !foreground_child &&
        m->saved_game_timer_head == 0) {
        mr_exit();
        return 1;
    }
    return 0;
}

static int arm_ext_save_modal_screen_snapshot(ArmExtModule *m) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr)) {
        return 0;
    }
    if (m->modal_screen_snapshot_len != m->screen_len) {
        free(m->modal_screen_snapshot);
        m->modal_screen_snapshot = NULL;
        m->modal_screen_snapshot_len = 0;
        m->modal_screen_snapshot_valid = 0;
    }
    if (!m->modal_screen_snapshot) {
        m->modal_screen_snapshot = (uint8_t *)malloc(m->screen_len);
        if (m->modal_screen_snapshot) m->modal_screen_snapshot_len = m->screen_len;
    }
    if (!m->modal_screen_snapshot) return 0;
    memcpy(m->modal_screen_snapshot, arm_ptr(m, m->screen_addr), m->screen_len);
    m->modal_screen_snapshot_valid = 0;
    return 1;
}

/* wrapper RW 前台分发区快照范围：覆盖 wrapper 在 RW 内维护的前台模块分发表
 * 与前台 helper 槽。偏移与长度来自对 cfunction.ext 事件分发路径的反汇编与
 * 运行时 RW 对比（dota 浏览器插件下载：进入子模块时 wrapper 改写 +0xF4 分发
 * 表项及 +0x18C/+0x1A0 helper 槽，关闭后未还原导致事件仍走子模块）。 */
#define MODAL_FG_SNAPSHOT_OFF 0xE0u
#define MODAL_FG_SNAPSHOT_LEN 0xD0u

static int arm_ext_save_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->p_addr || !arm_ptr(m, m->p_addr)) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (!rw || !arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        return 0;
    }
    if (!m->modal_fg_snapshot) {
        m->modal_fg_snapshot = (uint8_t *)malloc(MODAL_FG_SNAPSHOT_LEN);
        if (!m->modal_fg_snapshot) return 0;
    }
    memcpy(m->modal_fg_snapshot, arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF),
           MODAL_FG_SNAPSHOT_LEN);
    m->modal_fg_snapshot_rw = rw;
    m->modal_fg_snapshot_valid = 0;
    return 1;
}

/* 模态子模块关闭时还原 wrapper 前台分发区，使事件路由回到下层页面。
 * 仅当 wrapper RW 基址未变（同一 wrapper 实例）时还原，避免误写。 */
static void arm_ext_restore_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->modal_fg_snapshot || !m->modal_fg_snapshot_valid) return;
    uint32_t rw = 0;
    if (m->p_addr && arm_ptr(m, m->p_addr))
        memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (rw && rw == m->modal_fg_snapshot_rw &&
        arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        memcpy(arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF), m->modal_fg_snapshot,
               MODAL_FG_SNAPSHOT_LEN);
    }
    m->modal_fg_snapshot_valid = 0;
}




int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len) {
    if (output) *output = NULL;
    if (output_len) *output_len = 0;
    if (!m || !m->helper_addr || !m->p_addr) return MR_FAILED;
    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    uint32_t input_addr = 0;
    if (input != NULL) {
        if (input == m->host_code) {
            input_addr = EXT_CODE_ADDR;
        } else if (code == 3 || code == 6) {
            /*
             * Some start.mr files pass non-pointer scalar/table markers for
             * these bootstrap messages.  The ARM helper does not dereference
             * them, so keep the raw 32-bit value instead of memcpy'ing from an
             * invalid host address such as 0x1.
             */
            input_addr = (uint32_t)(uintptr_t)input;
        } else {
            input_addr = arm_alloc(m, input_len ? input_len : 1);
            if (!input_addr) return MR_FAILED;
            if (input_len) memcpy(arm_ptr(m, input_addr), input, input_len);
        }
    }
    uint32_t outp_addr = arm_alloc(m, 4);
    uint32_t outl_addr = arm_alloc(m, 4);
    /* 真机上事件(code=1)始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过 mrc_event 的内部回调链
     * 分发给 game。code=0/3-5 为生命周期调用，路由到 primary game helper。
     * code=2 路由到当前定时器所有者（wrapper 或 game）。 */
    int has_separate_wrapper = m->helper_addr && m->primary_helper_addr &&
                               m->helper_addr != m->primary_helper_addr;
    uint32_t call_p_addr, call_helper_addr;
    if (code == 1 && has_separate_wrapper) {
        call_p_addr = m->p_addr;
        call_helper_addr = m->helper_addr;
    } else if (code >= 0 && code <= 5 && m->primary_p_addr) {
        call_p_addr = m->primary_p_addr;
        call_helper_addr = m->primary_helper_addr;
    } else {
        call_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
        call_helper_addr = m->active_helper_addr ? m->active_helper_addr : m->helper_addr;
    }
    if (code == 2 && m->timer_p_addr && m->timer_helper_addr) {
        call_p_addr = m->timer_p_addr;
        call_helper_addr = m->timer_helper_addr;
    }
    int wrapper_modal_event_routed = 0;
    int reopen_set_this_call = 0;
    uint32_t modal_ext_chunk = 0;
    uint32_t modal_suspend_depth_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12))
        memcpy(&modal_ext_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + 0x34))
        memcpy(&modal_suspend_depth_pre, arm_ptr(m, modal_ext_chunk + 0x34), 4);
    int foreground_child_active =
        m->active_p_addr &&
        m->active_helper_addr &&
        m->active_p_addr != m->primary_p_addr &&
        m->active_helper_addr != m->primary_helper_addr &&
        m->active_p_addr != m->p_addr &&
        m->active_helper_addr != m->helper_addr;
    /* 真机上 code=1 事件始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过内部回调链分发给 game
     * 或前台子模块。不绕过 wrapper，否则 wrapper 的 suspend/resume
     * 机制无法正确管理模态框生命周期。
     *
     * 预处理：
     * - 修复 extChunk[8]（game helper 指针），确保 wrapper 的 resume
     *   (0xE83220) 能通过 blx extChunk[8] 正确调用 game 的 resumeApp。
     *   cfunction.ext ARM 代码运行中会将该槽位清零（gxdzc 反汇编证实）。 */
    if (code == 1 && has_separate_wrapper) {
        uint32_t _ec = modal_ext_chunk;
        if (_ec && m->primary_helper_addr && arm_ptr(m, _ec + 8)) {
            memcpy(arm_ptr(m, _ec + 8), &m->primary_helper_addr, 4);
        }
        if (input_len >= 12 && input_addr) {
            wrapper_modal_event_routed = 1;
        }
    }

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, call_p_addr), 4);
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG arm_ext_call code=%d input_len=%u callP=0x%X callH=0x%X rw=0x%X activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X modalDepth=%u\n",
               (int)code, input_len, call_p_addr, call_helper_addr, rw_base,
               m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
               m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr,
               modal_suspend_depth_pre);
    }
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    reg_write32(m->uc, UC_ARM_REG_R0, call_p_addr);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)code);
    reg_write32(m->uc, UC_ARM_REG_R2, input_addr);
    reg_write32(m->uc, UC_ARM_REG_R3, input_len);
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 16;
    uint32_t helper_args[4] = {outp_addr, outl_addr, 0, 0};
    uc_mem_write(m->uc, sp, helper_args, sizeof(helper_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 arm_ext_call 前的 game state[8] */
    uint32_t _ac_grw = 0, _ac_s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_ac_grw, arm_ptr(m, m->primary_p_addr), 4);
    if (_ac_grw && arm_ptr(m, _ac_grw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&_ac_s8_pre, arm_ptr(m, _ac_grw + GAME_TIMER_HEAD_OFFSET), 4);
    int call_modal_snapshot_candidate = 0;
    /* 非模态状态下的 code=1 事件可能触发子模块加载，需要事先保存 game
     * 的快照以便子模块关闭后恢复。事件经 wrapper 路由时 call_p_addr
     * 指向 wrapper 而非 primary，但语义不变：game 仍然会收到该事件。 */
    /* gxdzc 的下载提示可能在普通事件回调或 primary timer(code=2) 中
     * 直接触发 wrapper suspend，而不是等到 wrapper dispatch；回调前
     * 保存，回调后确认 [0x34] 0->1 才允许 cancel 使用这张底层页面截图。 */
    if ((code == 1 || code == 2) && modal_suspend_depth_pre == 0) {
        if (_ac_s8_pre != 0)
            call_modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区，供模态子模块关闭后还原事件路由。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    sync_internal_state_to_arm(m);
    int was_host_timer_pending = m->host_timer_pending;
    if (code == 2) {
        m->host_timer_pending = 0;
    }
    m->current_p_addr = call_p_addr;
    m->current_helper_addr = call_helper_addr;
    int call_ret = run_arm_with_sp(m, call_helper_addr, sp);
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    if (code == 2 && was_host_timer_pending && m->host_timer_pending) {
        internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
    }
    leave_screen_context(m, saved_screenBuf);
    capture_timer_dispatches(m);

    /* wrapper 无 primary module 时（如"请稍后"加载阶段），wrapper 的
     * code=2 处理完后不会通过 table[31] 重启宿主 timer。宿主需要持续
     * 给 wrapper 发送 timer tick，直到 wrapper 完成 game 模块的加载并
     * 注册 primary module。 */
    if (code == 2 && !m->host_timer_pending &&
        !m->primary_p_addr && m->wrapper_timer_dispatch_addr) {
        mr_timerStart(50);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        internal_slot_write(m, m->mr_timer_state_slot, 1);
    }

    /* arm_ext_call 后检查 game timer head (state[8]) 变化：wrapper 的
     * suspend 会清零 game_rw[0x8C]，保存旧值以便 resume 时恢复。 */
    if (_ac_grw && _ac_s8_pre) {
        uint32_t _ac_s8_post = 0;
        memcpy(&_ac_s8_post, arm_ptr(m, _ac_grw + GAME_TIMER_HEAD_OFFSET), 4);
        if (_ac_s8_post == 0) {
            m->saved_game_timer_head = _ac_s8_pre;
        }
    }
    uint32_t modal_suspend_depth_post = modal_suspend_depth_pre;
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + 0x34))
        memcpy(&modal_suspend_depth_post, arm_ptr(m, modal_ext_chunk + 0x34), 4);
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        call_modal_snapshot_candidate) {
        m->modal_screen_snapshot_valid = 1;
    }
    /* 进入模态（0->1）：标记前台分发区快照有效，供关闭时还原事件路由。 */
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        m->modal_fg_snapshot) {
        m->modal_fg_snapshot_valid = 1;
    }
    /* 通用模态框关闭处理（suspend depth >0 → 0）。
     * 真机上 wrapper 的 resume(0xE83220) 恢复 game 状态（定时器链表头、
     * pauseApp/resumeApp 回调），模拟器只需做模块级清理：
     *  - 将前台归还给 primary game
     *  - 清除子模块残留的定时器归属
     *  - 恢复 game timer head（wrapper 的 suspend 清零了它）
     *  - 恢复模态前屏幕快照
     *  - 补启宿主定时器让 wrapper 完成 resume 队列 */
    if (modal_suspend_depth_pre > 0 && modal_suspend_depth_post == 0) {
        m->active_helper_addr = m->primary_helper_addr;
        m->active_p_addr = m->primary_p_addr;
        arm_ext_clear_foreground_screen_owner(m);
        /* 还原 wrapper 前台分发区，使事件路由回到下层页面（修复返回后
         * 无法二次进入子模块界面）。 */
        arm_ext_restore_modal_fg_snapshot(m);
        if (foreground_child_active) {
            arm_ext_reset_child_modules(m);
            if (m->timer_p_addr &&
                m->timer_p_addr != m->primary_p_addr &&
                m->timer_p_addr != m->p_addr) {
                m->timer_p_addr = 0;
                m->timer_helper_addr = 0;
            }
        }
        if (m->saved_game_timer_head) {
            uint32_t grw = 0;
            if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
                memcpy(&grw, arm_ptr(m, m->primary_p_addr), 4);
            if (grw && arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET))
                memcpy(arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET),
                       &m->saved_game_timer_head, 4);
            m->saved_game_timer_head = 0;
        }
        if (m->modal_screen_snapshot_valid &&
            m->modal_screen_snapshot &&
            m->modal_screen_snapshot_len == m->screen_len &&
            m->screen_addr && arm_ptr(m, m->screen_addr)) {
            memcpy(arm_ptr(m, m->screen_addr),
                   m->modal_screen_snapshot, m->screen_len);
            extern uint16 *mr_screenBuf;
            extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y,
                                      uint16 w, uint16 h);
            if (mr_screenBuf) {
                memcpy(mr_screenBuf, m->modal_screen_snapshot,
                       m->screen_len);
                mr_drawBitmap(mr_screenBuf, 0, 0,
                              (uint16)m->screen_w, (uint16)m->screen_h);
            }
            m->modal_screen_snapshot_valid = 0;
        }
        m->primary_child_reopen_timer_needed = 1;
        reopen_set_this_call = 1;
    }

    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (arm_ext_finish_callback_state(m, modal_ext_chunk)) {
        /* mr_exit() 在存在 old app 时会重启上层并返回；当前 ARM 回调的
         * 控制权已经交给平台层，不能继续读取旧 helper 的 output/R0。 */
        return MR_SUCCESS;
    }

    /* 模态框关闭后的定时器恢复。
     * 真机上 game 的 table[31/32] 经 mrc_extTimerStart/Stop 路由到
     * wrapper 的定时器队列，不影响宿主定时器。模拟器里 game 的
     * table[32] 直接停掉宿主定时器，需要反复补启直到 game 完成
     * 内部状态转换。仅在新模态进入时清除标志。 */
    if (m->primary_child_reopen_timer_needed &&
        !m->host_timer_pending && mr_timer_state == 0) {
        mr_timerStart(100);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
    }
    if (m->primary_child_reopen_timer_needed && !reopen_set_this_call) {
        /* 新模态进入：child 加载成功并进入前台层 */
        if (modal_suspend_depth_post > 0 && modal_suspend_depth_pre == 0)
            m->primary_child_reopen_timer_needed = 0;
    }

    uint32_t arm_output = 0;
    uint32_t arm_output_len = 0;
    uc_mem_read(m->uc, outp_addr, &arm_output, 4);
    uc_mem_read(m->uc, outl_addr, &arm_output_len, 4);
    if (output_len) *output_len = (int32)arm_output_len;
    if (output) *output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;

    /* 模态层的原始事件已经由 wrapper 控件树处理。这里向宿主返回
     * MR_SUCCESS，避免同一个 mouse-up 在模态关闭后继续落到 game 菜单。 */
    if (wrapper_modal_event_routed) return MR_SUCCESS;
    return (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
}

uint32 arm_ext_helper_addr(ArmExtModule *m) {
    return m ? m->helper_addr : 0;
}

uint32 arm_ext_primary_helper(ArmExtModule *m) {
    if (!m) return 0;
    uint32_t ret = 0;
    uint32_t ext_chunk = 0, dispatch = 0;
    if (m->timer_p_addr && m->timer_helper_addr &&
        !(m->timer_p_addr == m->p_addr && m->timer_helper_addr == m->helper_addr)) {
        /* gxdzc/netpay 的“请稍后...”路径由 wrapper 自己在 0xE83A69
         * 启动 10ms timer；该 owner 不能屏蔽宿主侧 wrapper timer 路由。
         * 只有 timer 属于后加载子插件时，才跳过 wrapper timer，避免把
         * 独立子插件 timer 当成 wrapper tick。 */
        goto out;
    }
    if (!(m->primary_helper_addr && m->primary_helper_addr != m->helper_addr)) {
        goto out;
    }
    if (!m->primary_p_addr || !arm_ptr(m, m->primary_p_addr + 12)) {
        goto out;
    }
    memcpy(&ext_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);
    if (ext_chunk && arm_ptr(m, ext_chunk + 0x28)) {
        memcpy(&dispatch, arm_ptr(m, ext_chunk + 0x28), 4);
    }
    uint32_t dispatch_addr = dispatch & ~1u;
    /* 只有指向外层 wrapper 代码段、且已定位到 wrapper timer queue consumer
     * 的 extChunk dispatch 才需要宿主从 arm_ext_call_dispatch() 直接驱动。
     * 另一些 wrapper（gghjt cfunction.ext 反汇编：helper code=2 分支进入
     * 0xE83E6C）把 timer consumer 收在 helper switch 里；这类应走普通
     * code=2 路由，由当前 timer owner 选择 wrapper helper。mpc/game.ext 的
     * extChunk[0x28] 还可指向后续嵌套 EXT 的 code dispatcher，它的 timer
     * 已通过 table[31]/[32] veneer 接入，不能按 wrapper tick 调用。 */
    ret = (dispatch && (dispatch & 1u) &&
           dispatch_addr >= EXT_CODE_ADDR &&
           dispatch_addr < EXT_CODE_ADDR + m->code_len &&
           m->wrapper_timer_dispatch_addr)
          ? m->primary_helper_addr : 0;
out:
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG primary_helper ret=0x%X timerP=0x%X timerH=0x%X wrapperP=0x%X wrapperH=0x%X primaryP=0x%X primaryH=0x%X chunk=0x%X dispatch=0x%X wrapperTimer=0x%X\n",
               ret, m->timer_p_addr, m->timer_helper_addr, m->p_addr, m->helper_addr,
               m->primary_p_addr, m->primary_helper_addr, ext_chunk, dispatch,
               m->wrapper_timer_dispatch_addr);
    }
    return ret;
}

int arm_ext_consume_primary_host_init(ArmExtModule *m) {
    if (!m || !m->primary_host_init_pending) return 0;
    m->primary_host_init_pending = 0;
    return 1;
}

void arm_ext_reset_child_modules(ArmExtModule *m) {
    if (!m) return;
    /* 保留 primary 模块，清除所有其他嵌套模块注册。
     * child 流程关闭后 nested_modules 里残留的子插件条目会干扰
     * hook_restore_r9 的 R9 同步和 arm_ext_p_for_code_addr 的
     * 代码归属判定，导致 wrapper 内部分发走错路径。 */
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        int is_primary = m->primary_file_addr &&
                         mod->file_addr == m->primary_file_addr &&
                         mod->file_len == m->primary_file_len;
        if (is_primary) {
            m->nested_modules[out++] = *mod;
        }
    }
    m->nested_module_count = out;
    m->active_p_addr = m->primary_p_addr;
    m->active_helper_addr = m->primary_helper_addr;
    arm_ext_clear_foreground_screen_owner(m);


}

uint32 arm_ext_read_timer_queue(ArmExtModule *m) {
    if (!m || !m->primary_p_addr) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->primary_p_addr), 4);
    if (!rw) return 0;
    uint32_t val = 0;
    if (arm_ptr(m, rw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&val, arm_ptr(m, rw + GAME_TIMER_HEAD_OFFSET), 4);
    return val;
}


int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32_t timer_interval) {
    if (!m || !m->p_addr) return MR_FAILED;
    uint32_t nested_p = m->primary_p_addr ? m->primary_p_addr : 0;
    if (!nested_p && m->last_file_addr) {
        void *p4 = arm_ptr(m, m->last_file_addr + 4);
        if (p4) memcpy(&nested_p, p4, 4);
    }
    if (!nested_p || !arm_ptr(m, nested_p + 12)) return MR_FAILED;
    /* 读取 P->mrc_extChunk (offset 12) */
    uint32_t ext_chunk = 0;
    memcpy(&ext_chunk, arm_ptr(m, nested_p + 12), 4);
    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x28)) return MR_FAILED;
    /* 读取目标函数 extChunk[0x28] 和参数 extChunk[0x24] */
    uint32_t func = 0, param = 0;
    memcpy(&func, arm_ptr(m, ext_chunk + 0x28), 4);
    memcpy(&param, arm_ptr(m, ext_chunk + 0x24), 4);
    int wrapper_timer_owner =
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        m->wrapper_timer_dispatch_addr != 0;
    if (wrapper_timer_owner) {
        /*
         * wrapper 自己的 table[31] timerStart 来自 timer queue 管理函数
         * 0xE83A34，下一次宿主 timer 必须进入 0xE83A80 的队列消费逻辑；
         * extChunk[0x28] 是事件/控件分发入口 0xE830BD，会把节点重新挂回
         * 队列而不是执行到 0xE83B46 的回调。验证：gxdzc 付费点击后
         * 0x66FFBD 回调被消费并显示 netpay 付费界面，ctest 仍通过。
         */
        func = m->wrapper_timer_dispatch_addr;
    }
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG call_dispatch nestedP=0x%X chunk=0x%X func=0x%X param=0x%X wrapperOwner=%d timerP=0x%X timerH=0x%X\n",
               nested_p, ext_chunk, func, param, wrapper_timer_owner,
               m->timer_p_addr, m->timer_helper_addr);
    }
    if (!func) return MR_FAILED;
    /* 以 wrapper 的 R9 调用目标函数:
     * R0=extChunk, R1=is_stop, R2=param, R3=timer_interval */
    uint32_t wrapper_rw = 0;
    memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
    /* wrapper 的 RW 段基址由 ARM ext 在 mr_c_function_load 期间设置。
     * 部分 wrapper（如 mrpinfo 的 cfunction.ext）在 Unicorn 环境下
     * 不初始化 R9，导致 P[0]=0。此时 dispatch 无法正确访问 wrapper 数据，
     * 跳过即可——primary ext 的定时器仍可通过 arm_ext_call(code=2) 驱动。 */
    if (!wrapper_rw) return MR_FAILED;

    reg_write32(m->uc, UC_ARM_REG_R9, wrapper_rw);
    reg_write32(m->uc, UC_ARM_REG_R0, ext_chunk);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)is_stop);
    reg_write32(m->uc, UC_ARM_REG_R2, param);
    reg_write32(m->uc, UC_ARM_REG_R3, timer_interval);
    /* dispatch 的栈参数全为0 */
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 32;
    uint32_t stack_args[8] = {0};
    uc_mem_write(m->uc, sp, stack_args, sizeof(stack_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 dispatch 前的 game timer head，用于模态框取消时恢复 */
    uint32_t _game_rw = 0, _s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_game_rw, arm_ptr(m, m->primary_p_addr), 4);
    if (_game_rw && arm_ptr(m, _game_rw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&_s8_pre, arm_ptr(m, _game_rw + GAME_TIMER_HEAD_OFFSET), 4);
    uint32_t suspend_depth_pre = 0;
    if (arm_ptr(m, ext_chunk + 0x34))
        memcpy(&suspend_depth_pre, arm_ptr(m, ext_chunk + 0x34), 4);
    int modal_snapshot_candidate = 0;
    /* cfunction.ext 的 suspend(0xE831A4) 在同一次 dispatch 内把
     * extChunk[0x34] 从 0 加到 1 并绘制模态层。这里只在尚未 suspend
     * 时抓取 framebuffer，dispatch 后确认 0->1 再标记为可用于 cancel 恢复。 */
    if (suspend_depth_pre == 0) {
        if (_s8_pre != 0)
            modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区（timer 内进入模态子模块的路径）。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->p_addr;
    m->current_helper_addr = m->helper_addr;
    int ret = run_arm_with_sp(m, func, sp);
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf);
    capture_timer_dispatches(m);
    if (wrapper_timer_owner) {
        uint32_t queue_head = 0;
        uint32_t queue_base_ms = 0;
        uint32_t q = wrapper_rw + 0x1FCu;
        if (arm_ptr(m, q + 20)) {
            memcpy(&queue_head, arm_ptr(m, q + 12), 4);
            memcpy(&queue_base_ms, arm_ptr(m, q + 20), 4);
        }
        if (queue_head) {
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            /*
             * 0xE83A80 可能只消费到期节点并留下后续节点；若 ARM 侧没有
             * 再次调用 table[31]，宿主仍要保持 timer running，下一 tick
             * 才能继续推进 wrapper 队列。验证：gxdzc pay.sh 不再停在
             * 标题/“请稍后”，同时 gghjt pay-normal-back.sh 仍能返回。
             */
            if (queue_base_ms == 0) {
                uint32_t now = mr_getTime();
                memcpy(arm_ptr(m, q + 20), &now, 4);
            }
        }
    }

    /* wrapper dispatch 可能清零 game timer head（模态框进入时），保存旧值 */
    /* dispatch 后重新读 game_rw（ARM 代码可能修改了 primary_p_addr[0]） */
    {
        uint32_t _grw2 = 0;
        if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
            memcpy(&_grw2, arm_ptr(m, m->primary_p_addr), 4);
        uint32_t _s8_post = 0;
        if (_grw2 && arm_ptr(m, _grw2 + GAME_TIMER_HEAD_OFFSET))
            memcpy(&_s8_post, arm_ptr(m, _grw2 + GAME_TIMER_HEAD_OFFSET), 4);
        if (_s8_post == 0 && _s8_pre != 0) {
            m->saved_game_timer_head = _s8_pre;
        }
        uint32_t suspend_depth_post = 0;
        if (arm_ptr(m, ext_chunk + 0x34))
            memcpy(&suspend_depth_post, arm_ptr(m, ext_chunk + 0x34), 4);
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && modal_snapshot_candidate) {
            m->modal_screen_snapshot_valid = 1;
        }
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && m->modal_fg_snapshot) {
            m->modal_fg_snapshot_valid = 1;
        }
        /* 通用模态框关闭：与 arm_ext_call 中相同的通用清理 */
        if (suspend_depth_pre > 0 && suspend_depth_post == 0) {
            m->active_helper_addr = m->primary_helper_addr;
            m->active_p_addr = m->primary_p_addr;
            arm_ext_clear_foreground_screen_owner(m);
            /* 还原 wrapper 前台分发区，使事件路由回到下层页面。 */
            arm_ext_restore_modal_fg_snapshot(m);
            arm_ext_reset_child_modules(m);
            if (m->saved_game_timer_head) {
                if (_grw2 && arm_ptr(m, _grw2 + GAME_TIMER_HEAD_OFFSET))
                    memcpy(arm_ptr(m, _grw2 + GAME_TIMER_HEAD_OFFSET),
                           &m->saved_game_timer_head, 4);
                m->saved_game_timer_head = 0;
            }
            if (m->modal_screen_snapshot_valid &&
                m->modal_screen_snapshot &&
                m->modal_screen_snapshot_len == m->screen_len &&
                m->screen_addr && arm_ptr(m, m->screen_addr)) {
                memcpy(arm_ptr(m, m->screen_addr),
                       m->modal_screen_snapshot, m->screen_len);
                extern uint16 *mr_screenBuf;
                extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y,
                                          uint16 w, uint16 h);
                if (mr_screenBuf) {
                    memcpy(mr_screenBuf, m->modal_screen_snapshot,
                           m->screen_len);
                    mr_drawBitmap(mr_screenBuf, 0, 0,
                                  (uint16)m->screen_w, (uint16)m->screen_h);
                }
                m->modal_screen_snapshot_valid = 0;
            }
            m->primary_child_reopen_timer_needed = 1;
        }
    }

    if (arm_ext_finish_callback_state(m, ext_chunk)) {
        /* dispatch 已经触发平台退出/重启语义；返回成功让外层停止继续解释
         * 本次 wrapper timer 的旧返回值。 */
        return MR_SUCCESS;
    }

    return ret;
}

int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, 0);
    reg_write32(m->uc, UC_ARM_REG_R1, 0);
    reg_write32(m->uc, UC_ARM_REG_R2, 0);
    reg_write32(m->uc, UC_ARM_REG_R3, 0);

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int call_ret = run_arm(m, func);
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    return MR_SUCCESS;
}

int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, arg0);
    reg_write32(m->uc, UC_ARM_REG_R1, arg1);
    reg_write32(m->uc, UC_ARM_REG_R2, arg2);

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int call_ret = run_arm(m, func);
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    return MR_SUCCESS;
}

void arm_ext_host_cache_sync(ArmExtModule *m, const void *host_data, uint32 len) {
    if (!m || !host_data || !len) return;
    if (!m->pending_internal_file_addr || !m->pending_internal_file_len) return;
    uint32_t addr = m->pending_internal_file_addr;
    uint32_t flen = m->pending_internal_file_len;
    uint32_t copy_len = len < flen ? len : flen;
    void *dst = arm_ptr(m, addr);
    if (!dst) return;
    memcpy(dst, host_data, copy_len);
    uc_ctl_remove_cache(m->uc, addr, addr + copy_len);
    arm_ext_sync_internal_nested_module(m, addr, flen);
}

void arm_ext_unload(ArmExtModule *m) {
    if (!m) return;
    if (m->profile && m->profile->cleanup)
        m->profile->cleanup(m->app_state);
    m->app_state = NULL;
    mrp_cache_free(m);
    if (m->uc) uc_close(m->uc);
    free(m->last_file_copy);
    free(m->modal_screen_snapshot);
    free(m->modal_fg_snapshot);
    free(m->foreground_screen_snapshot);
    free(m->low_table);
    if (m->mem_is_mmap) {
#ifdef _MSC_VER
        VirtualFree(m->mem, 0, MEM_RELEASE);
#else
        munmap(m->mem, EXT_MEM_SIZE);
#endif
    } else {
        free(m->mem);
    }
    free(m);
}
