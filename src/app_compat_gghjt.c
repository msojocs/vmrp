#include "./include/arm_ext_internal.h"
#include "./include/arm_ext_executor.h"
#include <zlib.h>

typedef struct {
    uint32_t pending_chk_arm_buf;
    uint32_t pending_chk_len;
    uint8_t *pending_chk_decomp;
    uint32_t pending_chk_decomp_len;
} GghjtState;

/* --- lifecycle --- */

static void *gghjt_init(ArmExtModule *m) {
    (void)m;
    GghjtState *s = calloc(1, sizeof(GghjtState));
    return s;
}

static void gghjt_cleanup(void *app_state) {
    GghjtState *s = app_state;
    if (!s) return;
    free(s->pending_chk_decomp);
    free(s);
}

/* --- GOT protection --- */

static int gghjt_should_protect_got_addr(ArmExtModule *m, void *app_state,
                                         uint32 addr) {
    (void)app_state;
    if (!m) return 0;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t event_base = wrapper_rw + 0x190u;
    uint32_t event_end = event_base + 5u * 8u;
    return wrapper_rw && addr >= event_base && addr < event_end;
}

/* --- write/read interception (netpay extraction workaround) --- */

static int gghjt_intercept_write(ArmExtModule *m, void *app_state,
                                 uint32 fd, uint32 src_addr, uint32 len,
                                 void **new_src, uint32 *new_len) {
    (void)fd;
    GghjtState *gs = app_state;
    if (!gs) return 0;

    /* 0xBE 字节的 chk 提取源是 EXT 屏幕缓冲基址。此前硬编码 0x2001BC
     * (screen buffer 位于堆首时的地址);screen buffer 迁移到顶部保留区
     * (arm_ext_internal.h EXT_SCREEN_RESERVE)后按 m->screen_addr 判定,
     * 语义不变、与堆布局解耦。 */
    if (m && src_addr == m->screen_addr && len == 0xBEu && gs->pending_chk_len > 14000u) {
        size_t out_cap = (size_t)gs->pending_chk_decomp_len;
        if (gs->pending_chk_decomp && out_cap > 0) {
            *new_src = gs->pending_chk_decomp;
            *new_len = (uint32_t)out_cap;
            return 1;
        }
    }
    return 0;
}

static void gghjt_post_write_cleanup(void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return;
    free(gs->pending_chk_decomp);
    gs->pending_chk_decomp = NULL;
    gs->pending_chk_decomp_len = 0;
    gs->pending_chk_len = 0;
}

static void gghjt_post_read_hook(ArmExtModule *m, void *app_state,
                                 uint32 dst_addr, uint32 requested,
                                 int32 actual_read, void *host_buf) {
    (void)dst_addr;
    (void)requested;
    GghjtState *gs = app_state;
    if (!gs || !m) return;

    if (actual_read > 256 && host_buf) {
        const unsigned char *bp = (const unsigned char *)host_buf;
        if (bp[0] == 0x1f && bp[1] == 0x8b) {
            gs->pending_chk_arm_buf = dst_addr;
            gs->pending_chk_len = requested;
            free(gs->pending_chk_decomp);
            gs->pending_chk_decomp = NULL;
            gs->pending_chk_decomp_len = 0;

            z_stream zs = {0};
            zs.next_in = (Bytef *)bp;
            zs.avail_in = (uInt)actual_read;
            if (inflateInit2(&zs, 16 + MAX_WBITS) == Z_OK) {
                size_t cap = (size_t)actual_read * 4 + 4096;
                for (int tries = 0; tries < 6; ++tries) {
                    uint8_t *out = (uint8_t *)malloc(cap);
                    if (!out) break;
                    zs.next_out = out;
                    zs.avail_out = (uInt)cap;
                    int z = inflate(&zs, Z_FINISH);
                    if (z == Z_STREAM_END) {
                        gs->pending_chk_decomp = out;
                        gs->pending_chk_decomp_len = (uint32_t)zs.total_out;
                        break;
                    } else if (z == Z_BUF_ERROR) {
                        free(out);
                        inflateReset2(&zs, 16 + MAX_WBITS);
                        zs.next_in = (Bytef *)bp;
                        zs.avail_in = (uInt)actual_read;
                        cap *= 2;
                        continue;
                    } else {
                        free(out);
                        break;
                    }
                }
                inflateEnd(&zs);
            }
        }
    }
}

/* --- profile definition --- */

const AppCompatProfile app_compat_gghjt = {
    .name = "gghjt.mrp",
    .init = gghjt_init,
    .cleanup = gghjt_cleanup,
    .should_protect_got_addr = gghjt_should_protect_got_addr,
    .intercept_write = gghjt_intercept_write,
    .post_write_cleanup = gghjt_post_write_cleanup,
    .post_read_hook = gghjt_post_read_hook,
};
