/*
 * aex_screen.c —— 屏幕缓冲、脏区跟踪、前台归属与位图呈现(Phase 2 拆分第 4 步)。
 *
 * row-span 脏区/覆盖区、present 分段提交、DispUp、draw-bitmap 镜像、
 * 屏幕上下文切换(enter/leave/push/pop)。函数体自 arm_ext_executor.c
 * 原样迁移;跨单元符号见 arm_ext_priv.h。
 */
#include "../include/arm_ext_priv.h"
#include "../include/bridge.h"
#include "../include/vmrp.h" /* vmrp_display_width/height:LCD 旋转后的显示尺寸 */

/* 宿主屏幕全局(mythroad 层导出,语义见 aex_mem/abi 文档) */
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);


int32_t arm_ext_screen_stride(ArmExtModule *m) {
    if (!m) return 0;
    return m->screen_w;
}

/*
 * B1 修复:guest 逻辑画布回拷宿主 mr_screenBuf 的统一出口。
 * 宿主缓冲按物理分辨率 mr_screen_w*mr_screen_h 分配,而 m->screen_w/h 可被
 * guest 配置成更大的逻辑画布(gtcm 的 SphinxJoy 引擎配 480x320,宿主 240x320)。
 * 此前 leave_screen_context / 模态快照恢复(arm_ext_call、call_dispatch)三处
 * 各自按 guest 尺寸整块拷贝,越过宿主堆缓冲末尾写。统一按两侧尺寸交集逐行
 * 拷贝,行距按各自真实 stride;通过 out_w/out_h 返回实际尺寸供 present 夹断。
 */
void arm_ext_copy_screen_to_host(ArmExtModule *m, uint16_t *dst,
                                        const uint16_t *src,
                                        int32_t src_stride,
                                        int32_t *out_w, int32_t *out_h) {
    int32_t w = m ? m->screen_w : 0;
    int32_t h = m ? m->screen_h : 0;
    if (mr_screen_w > 0 && w > mr_screen_w) w = mr_screen_w;
    if (mr_screen_h > 0 && h > mr_screen_h) h = mr_screen_h;
    if (out_w) *out_w = w;
    if (out_h) *out_h = h;
    if (!dst || !src || w <= 0 || h <= 0 || src_stride <= 0) return;
    for (int32_t y = 0; y < h; ++y) {
        memcpy(dst + (size_t)y * (size_t)mr_screen_w,
               src + (size_t)y * (size_t)src_stride,
               (size_t)w * sizeof(uint16_t));
    }
}

void arm_ext_free_row_spans(ArmExtRowSpans *spans) {
    if (!spans) return;
    free(spans->min_x);
    free(spans->max_x);
    spans->min_x = NULL;
    spans->max_x = NULL;
    spans->rows = 0;
}

static void arm_ext_clear_row_spans(ArmExtRowSpans *spans) {
    if (!spans || !spans->min_x || !spans->max_x) return;
    for (uint32_t y = 0; y < spans->rows; ++y) {
        spans->min_x[y] = UINT16_MAX;
        spans->max_x[y] = 0;
    }
}

static int arm_ext_ensure_row_spans(ArmExtRowSpans *spans, uint32_t rows) {
    if (!spans || rows == 0) return 0;
    if (spans->rows == rows && spans->min_x && spans->max_x) return 1;

    arm_ext_free_row_spans(spans);
    spans->min_x = (uint16_t *)malloc(rows * sizeof(uint16_t));
    spans->max_x = (uint16_t *)malloc(rows * sizeof(uint16_t));
    if (!spans->min_x || !spans->max_x) {
        arm_ext_free_row_spans(spans);
        return 0;
    }
    spans->rows = rows;
    arm_ext_clear_row_spans(spans);
    return 1;
}

static int arm_ext_has_row_spans(const ArmExtRowSpans *spans) {
    if (!spans || !spans->min_x || !spans->max_x) return 0;
    for (uint32_t y = 0; y < spans->rows; ++y) {
        if (spans->min_x[y] < spans->max_x[y]) return 1;
    }
    return 0;
}

static int arm_ext_ensure_screen_regions(ArmExtModule *m) {
    if (!m || m->screen_h <= 0) return 0;
    uint32_t rows = (uint32_t)m->screen_h;
    return arm_ext_ensure_row_spans(&m->screen_damage, rows) &&
           arm_ext_ensure_row_spans(&m->screen_present, rows);
}

static void arm_ext_clear_screen_regions(ArmExtModule *m) {
    if (!arm_ext_ensure_screen_regions(m)) return;
    arm_ext_clear_row_spans(&m->screen_damage);
    arm_ext_clear_row_spans(&m->screen_present);
}

int arm_ext_ensure_foreground_cover_regions(ArmExtModule *m) {
    if (!m || m->screen_h <= 0) return 0;
    if (!arm_ext_ensure_row_spans(&m->foreground_cover, (uint32_t)m->screen_h))
        return 0;
    if (!arm_ext_has_row_spans(&m->foreground_cover))
        arm_ext_clear_row_spans(&m->foreground_cover);
    return 1;
}

static void arm_ext_clear_foreground_cover_regions(ArmExtModule *m) {
    if (!arm_ext_ensure_foreground_cover_regions(m)) return;
    arm_ext_clear_row_spans(&m->foreground_cover);
}











static void arm_ext_mark_row_spans(ArmExtRowSpans *spans,
                                   int32_t screen_w,
                                   int32_t screen_h,
                                   int32_t x,
                                   int32_t y,
                                   int32_t w,
                                   int32_t h) {
    if (!spans || !spans->min_x || !spans->max_x || spans->rows == 0 ||
        screen_w <= 0 ||
        screen_h <= 0 || w <= 0 || h <= 0) {
        return;
    }

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > screen_w) max_x = screen_w;
    if (max_y > screen_h) max_y = screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    uint16_t ux0 = (uint16_t)min_x;
    uint16_t ux1 = (uint16_t)max_x;
    for (int32_t yy = min_y; yy < max_y && (uint32_t)yy < spans->rows; ++yy) {
        if (ux0 < spans->min_x[yy]) spans->min_x[yy] = ux0;
        if (ux1 > spans->max_x[yy]) spans->max_x[yy] = ux1;
    }
}

void arm_ext_note_screen_damage_rect(ArmExtModule *m,
                                            int32_t x,
                                            int32_t y,
                                            int32_t w,
                                            int32_t h) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return;
    arm_ext_mark_row_spans(&m->screen_damage, m->screen_w, m->screen_h,
                           x, y, w, h);
}

void arm_ext_note_screen_damage_addr_range(ArmExtModule *m,
                                                  uint64_t address,
                                                  int size) {
    if (!m || !m->screen_addr || m->screen_w <= 0 || size <= 0 ||
        address < m->screen_addr) {
        return;
    }
    uint64_t start_byte = address - m->screen_addr;
    uint64_t end_byte = start_byte + (uint64_t)size - 1u;
    uint64_t max_byte = (uint64_t)m->screen_len - 1u;
    if (start_byte > max_byte) return;
    if (end_byte > max_byte) end_byte = max_byte;

    uint64_t start_pixel = start_byte / sizeof(uint16_t);
    uint64_t end_pixel = end_byte / sizeof(uint16_t);
    uint32_t screen_stride = (uint32_t)arm_ext_screen_stride(m);
    uint32_t y0 = (uint32_t)(start_pixel / screen_stride);
    uint32_t y1 = (uint32_t)(end_pixel / screen_stride);
    uint32_t x0 = (uint32_t)(start_pixel % screen_stride);
    uint32_t x1 = (uint32_t)(end_pixel % screen_stride) + 1u;
    if ((int32_t)x0 >= m->screen_w && (int32_t)x1 > m->screen_w) return;
    if ((int32_t)x0 >= m->screen_w) x0 = (uint32_t)m->screen_w;
    if ((int32_t)x1 > m->screen_w) x1 = (uint32_t)m->screen_w;

    if (y0 == y1) {
        arm_ext_note_screen_damage_rect(m, (int32_t)x0, (int32_t)y0,
                                        (int32_t)(x1 - x0), 1);
        return;
    }
    arm_ext_note_screen_damage_rect(m, (int32_t)x0, (int32_t)y0,
                                    m->screen_w - (int32_t)x0, 1);
    for (uint32_t y = y0 + 1u; y < y1; ++y) {
        arm_ext_note_screen_damage_rect(m, 0, (int32_t)y, m->screen_w, 1);
    }
    arm_ext_note_screen_damage_rect(m, 0, (int32_t)y1, (int32_t)x1, 1);
}

static void arm_ext_note_screen_present_rect(ArmExtModule *m,
                                             int32_t x,
                                             int32_t y,
                                             int32_t w,
                                             int32_t h) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return;
    arm_ext_mark_row_spans(&m->screen_present, m->screen_w, m->screen_h,
                           x, y, w, h);
}

void arm_ext_note_foreground_cover_rect(ArmExtModule *m,
                                               int32_t x,
                                               int32_t y,
                                               int32_t w,
                                               int32_t h) {
    if (!m || !arm_ext_ensure_foreground_cover_regions(m)) return;
    arm_ext_mark_row_spans(&m->foreground_cover, m->screen_w, m->screen_h,
                           x, y, w, h);
}

int arm_ext_has_foreground_cover(ArmExtModule *m) {
    if (!m || !arm_ext_has_foreground_child(m) ||
        !arm_ext_ensure_foreground_cover_regions(m)) {
        return 0;
    }
    return arm_ext_has_row_spans(&m->foreground_cover);
}

int arm_ext_suspend_depth_for_p(ArmExtModule *m,
                                       uint32_t p_addr,
                                       uint32_t *suspend_depth) {
    uint32_t ext_chunk = 0;
    if (suspend_depth) *suspend_depth = 0;
    if (m && p_addr && arm_ptr(m, p_addr + AEX_P_EXT_CHUNK_OFF)) {
        memcpy(&ext_chunk, arm_ptr(m, p_addr + AEX_P_EXT_CHUNK_OFF), 4);
    }
    if (!ext_chunk || !arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF) || !suspend_depth) return 0;
    memcpy(suspend_depth, arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    return 1;
}

static int arm_ext_foreground_cover_is_nonmodal(ArmExtModule *m) {
    uint32_t suspend_depth = 0;
    return arm_ext_has_foreground_child(m) &&
           arm_ext_suspend_depth_for_p(m, m->active_p_addr, &suspend_depth) &&
           suspend_depth == 0;
}

static void arm_ext_retire_nonmodal_foreground_cover_rect(ArmExtModule *m,
                                                          int32_t x,
                                                          int32_t y,
                                                          int32_t w,
                                                          int32_t h) {
    if (!arm_ext_has_foreground_cover(m) ||
        !arm_ext_foreground_cover_is_nonmodal(m)) {
        return;
    }

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > m->screen_w) max_x = m->screen_w;
    if (max_y > m->screen_h) max_y = m->screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    int changed = 0;
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint16_t c0 = m->foreground_cover.min_x[yy];
        uint16_t c1 = m->foreground_cover.max_x[yy];
        if (c0 >= c1 || max_x <= c0 || min_x >= c1) continue;

        /*
         * A readable extChunk[0x34] == 0 marks a non-modal overlay, not a
         * suspended page.  When the covered layer later explicitly presents a
         * rectangle over the overlay span, the visible owner for that span has
         * moved back to the covered layer.  Unknown or modal depths keep the
         * existing cover until wrapper resume clears active ownership.
         * Compatibility impact is limited to explicit present calls; cache-only
         * writes remain invisible behind foreground cover.  Verification is the
         * opbzqe RIGHT-key PPM check plus the existing plugin and pixel e2e
         * cases listed in docs/opbzqe-advbar-black-region-debug.md.
         */
        if (min_x <= c0 && max_x >= c1) {
            m->foreground_cover.min_x[yy] = UINT16_MAX;
            m->foreground_cover.max_x[yy] = 0;
            changed = 1;
        } else if (min_x <= c0) {
            m->foreground_cover.min_x[yy] = (uint16_t)max_x;
            changed = 1;
        } else if (max_x >= c1) {
            m->foreground_cover.max_x[yy] = (uint16_t)min_x;
            changed = 1;
        }
    }

    if (changed && !arm_ext_has_foreground_cover(m)) {
        m->foreground_screen_owner_p_addr = 0;
        m->foreground_screen_owner_helper_addr = 0;
    }
}

int arm_ext_owner_is_foreground_child(ArmExtModule *m,
                                             uint32_t owner_p_addr,
                                             uint32_t owner_helper_addr) {
    return arm_ext_has_foreground_child(m) &&
           owner_p_addr &&
           (owner_p_addr == m->active_p_addr ||
            owner_helper_addr == m->active_helper_addr);
}

int arm_ext_owner_is_covered_by_foreground(ArmExtModule *m,
                                                  uint32_t owner_p_addr,
                                                  uint32_t owner_helper_addr) {
    return arm_ext_has_foreground_cover(m) &&
           !arm_ext_owner_is_foreground_child(m, owner_p_addr,
                                              owner_helper_addr);
}

static void arm_ext_clear_empty_nonmodal_foreground_owner(ArmExtModule *m) {
    if (!m || !m->foreground_screen_owner_p_addr) return;
    if (m->foreground_screen_owner_p_addr != m->active_p_addr ||
        m->foreground_screen_owner_helper_addr != m->active_helper_addr) {
        return;
    }
    if (arm_ext_has_foreground_cover(m) ||
        !arm_ext_foreground_cover_is_nonmodal(m)) {
        return;
    }

    /*
     * Non-modal overlays such as advbar keep their EXT module active after the
     * covered game presents over the overlay rows.  Cache-only writes from the
     * still-active child can refresh foreground_screen_owner_* without adding
     * any foreground_cover rows, but owner metadata alone is not a visible
     * occlusion mask.  Once the row spans are empty, the lower layer must be
     * allowed to present normally; modal children keep using suspend depth.
     */
    m->foreground_screen_owner_p_addr = 0;
    m->foreground_screen_owner_helper_addr = 0;
}

static uint32_t arm_ext_screen_caller_owner(ArmExtModule *m,
                                            uint32_t *caller_helper_addr) {
    uint32_t helper_addr = 0;
    uint32_t p_addr = 0;
    if (m) {
        p_addr = arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_LR),
                                         &helper_addr);
        if (!p_addr) {
            p_addr = m->current_p_addr;
            helper_addr = m->current_helper_addr;
        }
    }
    if (caller_helper_addr) *caller_helper_addr = helper_addr;
    return p_addr;
}

static void arm_ext_note_visible_present_rect(ArmExtModule *m,
                                              int32_t x,
                                              int32_t y,
                                              int32_t w,
                                              int32_t h) {
    arm_ext_note_screen_presented(m);
    arm_ext_note_screen_present_rect(m, x, y, w, h);
}

static uint16_t arm_ext_sample_present_pixel(ArmExtModule *m,
                                             const uint16_t *bmp,
                                             int32_t x,
                                             int32_t y) {
    if (!m || !bmp || x < 0 || y < 0 ||
        x >= m->screen_w || y >= m->screen_h) {
        return 0;
    }
    return bmp[(uint32_t)y * (uint32_t)arm_ext_screen_stride(m) +
               (uint32_t)x];
}

static void arm_ext_diag_visible_present(ArmExtModule *m,
                                         const char *tag,
                                         const uint16_t *bmp,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w,
                                         int32_t h,
                                         int covered_by_foreground) {
    if (!m || !arm_ext_screen_diag_on()) return;
    if (!arm_ext_has_foreground_child(m) &&
        !(w >= m->screen_w && h >= m->screen_h)) {
        return;
    }

    uint32_t owner_helper = 0;
    uint32_t owner_p = arm_ext_screen_caller_owner(m, &owner_helper);
    uint16_t c_tl = arm_ext_sample_present_pixel(m, bmp, x, y);
    uint16_t c_mid = arm_ext_sample_present_pixel(
        m, bmp, m->screen_w / 2, m->screen_h / 2);
    uint16_t c_probe = arm_ext_sample_present_pixel(
        m, bmp, (m->screen_w * 2) / 3, (m->screen_h * 9) / 20);
    /*
     * Narrow visible-present signature for foreground handoff bugs.  It logs
     * only explicit host-visible submits, not every ARM framebuffer write.
     */
    printf("DIAG visible_present %s x=%d y=%d w=%d h=%d ownerP=0x%X ownerH=0x%X activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X fgOwnerP=0x%X fgOwnerH=0x%X covered=%d samples=%04X/%04X/%04X\n",
           tag ? tag : "?", x, y, w, h, owner_p, owner_helper,
           m->active_p_addr, m->active_helper_addr,
           m->primary_p_addr, m->primary_helper_addr,
           m->foreground_screen_owner_p_addr,
           m->foreground_screen_owner_helper_addr,
           covered_by_foreground, c_tl, c_mid, c_probe);
}

typedef int (*ArmExtPresentSegmentFn)(ArmExtModule *m,
                                      void *ctx,
                                      int32_t x,
                                      int32_t y,
                                      int32_t w);

typedef struct ArmExtBitmapPresentCtx {
    uint16_t *bmp;
    int32_t dst_x;
    int32_t dst_y;
    int32_t source_stride;
    int32_t source_x;
    int32_t source_y;
} ArmExtBitmapPresentCtx;

typedef struct ArmExtDispupPresentCtx {
    uint16_t *screen;
    int32_t source_stride;
} ArmExtDispupPresentCtx;

static int arm_ext_submit_uncovered_present_segments(
    ArmExtModule *m,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    ArmExtPresentSegmentFn submit,
    void *submit_ctx) {
    if (!m || !submit || w <= 0 || h <= 0) return 0;

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > m->screen_w) max_x = m->screen_w;
    if (max_y > m->screen_h) max_y = m->screen_h;
    if (min_x >= max_x || min_y >= max_y) return 0;

    int drew = 0;
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint16_t c0 = m->foreground_cover.min_x[yy];
        uint16_t c1 = m->foreground_cover.max_x[yy];
        if (c0 >= c1) {
            if (submit(m, submit_ctx, min_x, yy, max_x - min_x)) {
                arm_ext_note_visible_present_rect(m, min_x, yy,
                                                  max_x - min_x, 1);
                drew = 1;
            }
            continue;
        }
        if (min_x < c0) {
            int32_t sx1 = c0 < max_x ? c0 : max_x;
            if (min_x < sx1 &&
                submit(m, submit_ctx, min_x, yy, sx1 - min_x)) {
                arm_ext_note_visible_present_rect(m, min_x, yy,
                                                  sx1 - min_x, 1);
                drew = 1;
            }
        }
        if (c1 < max_x) {
            int32_t sx0 = c1 > min_x ? c1 : min_x;
            if (sx0 < max_x &&
                submit(m, submit_ctx, sx0, yy, max_x - sx0)) {
                arm_ext_note_visible_present_rect(m, sx0, yy,
                                                  max_x - sx0, 1);
                drew = 1;
            }
        }
    }
    return drew;
}

static int arm_ext_submit_bitmap_segment(ArmExtModule *m,
                                         void *ctx,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w) {
    (void)m;
    ArmExtBitmapPresentCtx *bitmap_ctx = (ArmExtBitmapPresentCtx *)ctx;
    if (!bitmap_ctx || !bitmap_ctx->bmp) return 0;
    guiDrawBitmapWithStride(
        bitmap_ctx->bmp, x, y, w, 1, bitmap_ctx->source_stride,
        bitmap_ctx->source_x + (x - bitmap_ctx->dst_x),
        bitmap_ctx->source_y + (y - bitmap_ctx->dst_y));
    return 1;
}

static int arm_ext_submit_dispup_segment(ArmExtModule *m,
                                         void *ctx,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w) {
    (void)m;
    ArmExtDispupPresentCtx *dispup_ctx = (ArmExtDispupPresentCtx *)ctx;
    if (!dispup_ctx || !dispup_ctx->screen || dispup_ctx->source_stride <= 0) {
        return 0;
    }
    guiDrawBitmapWithStride(dispup_ctx->screen, x, y, w, 1,
                            dispup_ctx->source_stride, x, y);
    return 1;
}

int arm_ext_present_bitmap_rect(ArmExtModule *m,
                                       uint16_t *bmp,
                                       int32_t x,
                                       int32_t y,
                                       int32_t w,
                                       int32_t h,
                                       int32_t source_stride,
                                       int32_t source_x,
                                       int32_t source_y,
                                       int covered_by_foreground) {
    if (!bmp || w <= 0 || h <= 0 || source_stride <= 0) return 0;
    if (covered_by_foreground) {
        arm_ext_retire_nonmodal_foreground_cover_rect(m, x, y, w, h);
        covered_by_foreground = arm_ext_has_foreground_cover(m);
    }
    if (!covered_by_foreground || !arm_ext_has_foreground_cover(m)) {
        arm_ext_diag_visible_present(m, "bitmap", bmp, x, y, w, h,
                                     covered_by_foreground);
        guiDrawBitmapWithStride(bmp, x, y, w, h, source_stride,
                                source_x, source_y);
        arm_ext_note_visible_present_rect(m, x, y, w, h);
        return 1;
    }

    ArmExtBitmapPresentCtx bitmap_ctx = {
        .bmp = bmp,
        .dst_x = x,
        .dst_y = y,
        .source_stride = source_stride,
        .source_x = source_x,
        .source_y = source_y,
    };
    return arm_ext_submit_uncovered_present_segments(
        m, x, y, w, h, arm_ext_submit_bitmap_segment, &bitmap_ctx);
}

int arm_ext_dispup_rect(ArmExtModule *m,
                               int32_t x,
                               int32_t y,
                               int32_t w,
                               int32_t h,
                               int covered_by_foreground) {
    if (w <= 0 || h <= 0) return 0;
    if (covered_by_foreground) {
        arm_ext_retire_nonmodal_foreground_cover_rect(m, x, y, w, h);
        covered_by_foreground = arm_ext_has_foreground_cover(m);
    }
    if (!covered_by_foreground || !arm_ext_has_foreground_cover(m)) {
        uint16_t *screen = (uint16_t *)arm_ptr(m, m->screen_addr);
        arm_ext_diag_visible_present(m, "dispup", screen,
                                     x, y, w, h, covered_by_foreground);
        guiDrawBitmapWithStride(screen, x, y, w, h,
                                arm_ext_screen_stride(m), x, y);
        arm_ext_note_visible_present_rect(m, x, y, w, h);
        return MR_SUCCESS;
    }

    ArmExtDispupPresentCtx dispup_ctx = {
        .screen = (uint16_t *)arm_ptr(m, m->screen_addr),
        .source_stride = arm_ext_screen_stride(m),
    };
    /* D1:MR_SUCCESS==0,原三目两分支同值不可区分;分段提交本就无失败
     * 语义,统一返回 MR_SUCCESS,guest 可见行为不变 */
    arm_ext_submit_uncovered_present_segments(
        m, x, y, w, h, arm_ext_submit_dispup_segment, &dispup_ctx);
    return MR_SUCCESS;
}

static int arm_ext_has_screen_damage(ArmExtModule *m) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return 0;
    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        if (m->screen_damage.min_x[y] < m->screen_damage.max_x[y]) {
            return 1;
        }
    }
    return 0;
}

void arm_ext_note_screen_damage_diff(ArmExtModule *m,
                                            const uint16_t *before) {
    if (!m || !before || !m->screen_addr || !arm_ext_ensure_screen_regions(m) ||
        m->screen_w <= 0 || m->screen_h <= 0) {
        return;
    }
    const uint16_t *after = (const uint16_t *)arm_ptr(m, m->screen_addr);
    if (!after) return;

    for (int32_t y = 0; y < m->screen_h; ++y) {
        int32_t min_x = m->screen_w;
        int32_t max_x = -1;
        const uint16_t *before_row =
            before + (uint32_t)y * (uint32_t)arm_ext_screen_stride(m);
        const uint16_t *after_row =
            after + (uint32_t)y * (uint32_t)arm_ext_screen_stride(m);
        for (int32_t x = 0; x < m->screen_w; ++x) {
            if (before_row[x] != after_row[x]) {
                if (x < min_x) min_x = x;
                max_x = x;
            }
        }
        if (max_x >= min_x) {
            arm_ext_note_screen_damage_rect(m, min_x, y, max_x - min_x + 1, 1);
        }
    }
}

static void arm_ext_present_uncovered_screen_damage(ArmExtModule *m,
                                                    uint16_t *screen) {
    if (!m || !screen || !m->screen_dirty || !m->screen_presented_in_callback ||
        !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    /*
     * Native EXT code can compose through host screen-cache APIs, then present
     * only a smaller dirty rectangle.  The ARM framebuffer is copied back to
     * the host shadow buffer at callback exit, but SDL has only seen the
     * explicit present rectangles.  Submit the row spans that were changed by
     * host-executed drawing APIs and were not covered by those presents.
     */
    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 >= d1) continue;

        uint16_t p0 = m->screen_present.min_x[y];
        uint16_t p1 = m->screen_present.max_x[y];
        if (p0 >= p1) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(d1 - d0), 1);
            continue;
        }
        if (d0 < p0) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(p0 - d0), 1);
        }
        if (p1 < d1) {
            mr_drawBitmap(screen, p1, (int16)y, (uint16)(d1 - p1), 1);
        }
    }
}

static void arm_ext_present_screen_damage(ArmExtModule *m, uint16_t *screen) {
    if (!m || !screen || !m->screen_dirty || !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 < d1) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(d1 - d0), 1);
        }
    }
}

static void arm_ext_merge_uncovered_screen_damage(ArmExtModule *m,
                                                  uint32_t owner_p_addr,
                                                  uint32_t owner_helper_addr) {
    if (!m || !m->screen_dirty || !m->screen_presented_in_callback ||
        !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 >= d1) continue;

        uint16_t p0 = m->screen_present.min_x[y];
        uint16_t p1 = m->screen_present.max_x[y];
        if (p0 >= p1) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 d0, (int32_t)y,
                                                 d1 - d0, 1);
            continue;
        }
        if (d0 < p0) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 d0, (int32_t)y,
                                                 p0 - d0, 1);
        }
        if (p1 < d1) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 p1, (int32_t)y,
                                                 d1 - p1, 1);
        }
    }
}

void enter_screen_context(ArmExtModule *m,
                                 uint16 **saved_screen,
                                 uint32_t *present_depth_before) {
    *saved_screen = mr_screenBuf;
    if (m) {
        if (present_depth_before) {
            *present_depth_before = m->screen_present_depth;
        }
        m->screen_presented_in_callback = 0;
        arm_ext_clear_screen_regions(m);
    } else if (present_depth_before) {
        *present_depth_before = 0;
    }
    if (m->screen_addr) mr_screenBuf = (uint16 *)arm_ptr(m, m->screen_addr);
}

void arm_ext_note_screen_presented(ArmExtModule *m) {
    if (m) {
        m->screen_presented_in_callback = 1;
        m->screen_present_depth++;
    }
}

uint16_t *arm_ext_snapshot_screen(ArmExtModule *m) {
    if (!m || !m->screen_addr || !m->screen_len || !arm_ptr(m, m->screen_addr))
        return NULL;
    uint16_t *snapshot = (uint16_t *)malloc(m->screen_len);
    if (!snapshot) return NULL;
    memcpy(snapshot, arm_ptr(m, m->screen_addr), m->screen_len);
    return snapshot;
}

void leave_screen_context(ArmExtModule *m,
                                 uint16 *saved_screen,
                                 uint32_t present_depth_before) {
    if (m->screen_addr && saved_screen && m->screen_len) {
        uint16_t *arm_screen = (uint16_t *)arm_ptr(m, m->screen_addr);
        /* B1:统一经宿主容量夹断的回拷;顺带消除旧代码里 stride 恒等于
         * screen_w(arm_ext_screen_stride 实现如此)导致的不可达逐行分支 */
        arm_ext_copy_screen_to_host(m, saved_screen, arm_screen,
                                    arm_ext_screen_stride(m), NULL, NULL);
    }
    mr_screenBuf = saved_screen;
    if (saved_screen && m && m->screen_dirty && m->screen_presented_in_callback) {
        arm_ext_merge_uncovered_screen_damage(m, m->foreground_screen_owner_p_addr,
                                              m->foreground_screen_owner_helper_addr);
        arm_ext_present_uncovered_screen_damage(m, saved_screen);
        m->screen_dirty = 0;
    }
    /*
     * Keep foreground screen ownership as visible-layer metadata only.
     * A foreground overlay may present its own covered rows and later proxy
     * still-visible rows from the shared ARM screen cache.  Auto-restoring a
     * host-side copy into that cache would erase the producer's next frame.
     */
    /*
     * Native apps often clear the whole screen buffer, repaint only dirty
     * regions, then explicitly present those regions with mr_drawBitmap or
     * DispUpEx.  A forced full-screen present at callback exit would expose
     * cleared-but-not-presented pixels, so callback-exit synthesis is limited
     * to row spans that were explicitly tracked as screen damage.
     */
    if (saved_screen && m && m->screen_dirty &&
        m->screen_present_depth == present_depth_before &&
        arm_ext_has_screen_damage(m)) {
        arm_ext_present_screen_damage(m, saved_screen);
    }
    m->screen_dirty = 0;
    if (m) m->screen_presented_in_callback = 0;
}

static int arm_ext_read_table_u32_slot(ArmExtModule *m, uint32_t idx,
                                       uint32_t *value) {
    if (value) *value = 0;
    if (!m || idx >= EXT_TABLE_COUNT) return 0;

    uint32_t screen_slot = 0;
    void *slotp = arm_ptr(m, EXT_TABLE_ADDR + idx * 4);
    if (!slotp) return 0;
    memcpy(&screen_slot, slotp, 4);
    if (!screen_slot || !arm_ptr(m, screen_slot)) return 0;

    uint32_t slot_value = 0;
    memcpy(&slot_value, arm_ptr(m, screen_slot), 4);
    if (value) *value = slot_value;
    return 1;
}

int arm_ext_push_draw_screen_context(ArmExtModule *m,
                                            ArmExtScreenContext *ctx) {
    if (!ctx) return 0;
    memset(ctx, 0, sizeof(*ctx));
    ctx->saved_screen = mr_screenBuf;
    ctx->saved_w = mr_screen_w;
    ctx->saved_h = mr_screen_h;
    if (!m) return 0;

    uint32_t screen_addr = 0;
    uint32_t screen_w = 0;
    uint32_t screen_h = 0;
    if (!arm_ext_read_table_u32_slot(m, 91, &screen_addr) ||
        !screen_addr || !arm_ptr(m, screen_addr)) {
        return 0;
    }
    if (!arm_ext_read_table_u32_slot(m, 92, &screen_w) ||
        !arm_ext_read_table_u32_slot(m, 93, &screen_h) ||
        screen_w == 0 || screen_h == 0) {
        return 0;
    }

    /*
     * guest 可改写 ARM 可见 mr_screen_w/h(table[92/93])配置逻辑画布(gtcm
     * 横屏 480x320):hook_screen_dim_write 已在写入瞬间把缓冲迁移到顶部
     * 保留区。仅当缓冲确实已迁移(screen_addr 落在保留区)时才把新尺寸
     * 持久采纳进 m->screen_w/h/len——present 的 stride/行数与 damage 跟踪
     * 必须与迁移后的绘制一致。未迁移时保持原有语义:guest 尺寸只作用于
     * 本次绘制(函数末尾的 mr_screen_w/h 赋值),但超出宿主缓冲容量的
     * 尺寸必须回退,避免宿主绘制越界抹掉缓冲之后的模块镜像(gtcm 黑屏
     * 根因);写成宿主画布转置的尺寸同样回退——那是 gtcm 撤销 LCD 旋转
     * 的竖屏请求,hook_screen_dim_read 会在 guest 读回前恢复宿主尺寸,
     * 宿主绘制若在这个窗口内采纳转置 stride 会交错花屏。guest 把
     * table[91] 指向自有缓冲时(滚动背景离屏合成)不受影响。
     */
    if (screen_addr == m->screen_addr) {
        uint32_t cap = m->screen_cap ? m->screen_cap : m->screen_len;
        int migrated = m->screen_addr ==
                       EXT_BASE_ADDR + EXT_MEM_SIZE - EXT_SCREEN_RESERVE;
        int transposed = screen_w == (uint32_t)m->screen_h &&
                         screen_h == (uint32_t)m->screen_w &&
                         screen_w != screen_h;
        if (transposed || screen_w > 2048 || screen_h > 2048 ||
            screen_w * screen_h * 2u > cap) {
            /* 越界/转置保护:退回宿主已知尺寸 */
            screen_w = (uint32_t)m->screen_w;
            screen_h = (uint32_t)m->screen_h;
        } else if (migrated &&
                   ((int32_t)screen_w != m->screen_w ||
                    (int32_t)screen_h != m->screen_h)) {
            m->screen_w = (int32_t)screen_w;
            m->screen_h = (int32_t)screen_h;
            m->screen_len = screen_w * screen_h * 2u;
        }
    }

    /*
     * table[91/92/93] are the ARM-visible mr_screenBuf/mr_screen_w/mr_screen_h
     * variables. Native code may repoint them to an off-screen cache with a
     * wider stride while composing a scrolling background, so host-rendered
     * screen-cache APIs must use the current ARM values for the duration of
     * each call.
     */
    mr_screenBuf = (uint16 *)arm_ptr(m, screen_addr);
    mr_screen_w = (int32)screen_w;
    mr_screen_h = (int32)screen_h;
    ctx->target_addr = screen_addr;
    ctx->active = 1;
    return 1;
}

void arm_ext_pop_draw_screen_context(ArmExtScreenContext *ctx) {
    if (!ctx || !ctx->active) return;
    mr_screenBuf = ctx->saved_screen;
    mr_screen_w = ctx->saved_w;
    mr_screen_h = ctx->saved_h;
}

int arm_ext_screen_context_targets_primary(ArmExtModule *m,
                                                  const ArmExtScreenContext *ctx) {
    return m && ctx && ctx->active && ctx->target_addr == m->screen_addr;
}

static int arm_ext_should_track_screen_cache_damage(ArmExtModule *m) {
    /*
     * The screen cache is backing storage.  A foreground child protects lower
     * layers only after it has explicitly presented cover rows; table[25] alone
     * can also register non-visual support helpers, and suppressing cache
     * damage in that state leaves later primary redraws invisible.
     */
    return !arm_ext_has_foreground_cover(m);
}

void arm_ext_finish_screen_cache_write(ArmExtModule *m,
                                              const ArmExtScreenContext *ctx,
                                              uint32_t claim_p_addr,
                                              uint32_t claim_helper_addr) {
    if (!arm_ext_screen_context_targets_primary(m, ctx)) return;
    if (!arm_ext_should_track_screen_cache_damage(m)) return;
    m->screen_dirty = 1;
    arm_ext_finish_accepted_screen_write(m, claim_p_addr, claim_helper_addr);
}

void arm_ext_clear_foreground_screen_owner(ArmExtModule *m) {
    if (!m) return;
    m->foreground_screen_owner_p_addr = 0;
    m->foreground_screen_owner_helper_addr = 0;
    arm_ext_clear_foreground_cover_regions(m);
}

static int arm_ext_claim_foreground_screen_owner(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr) {
    if (!m || !owner_p_addr) {
        return 0;
    }
    m->foreground_screen_owner_p_addr = owner_p_addr;
    m->foreground_screen_owner_helper_addr = owner_helper_addr;
    return 1;
}

void arm_ext_claim_foreground_screen_rect(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr,
                                                 int32_t x,
                                                 int32_t y,
                                                 int32_t w,
                                                 int32_t h) {
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    arm_ext_claim_foreground_screen_owner(m, owner_p_addr,
                                          owner_helper_addr);
}

void arm_ext_claim_foreground_screen_diff(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr,
                                                 const uint16_t *before) {
    (void)before;
    arm_ext_claim_foreground_screen_owner(m, owner_p_addr,
                                          owner_helper_addr);
}

int arm_ext_should_accept_screen_write(ArmExtModule *m,
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
        if (arm_ext_diag_on()) {
            static int screen_claim_diag_count = 0;
            if (screen_claim_diag_count++ < 40) {
                printf("DIAG screen_claim callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                       caller_p_addr, caller_helper_addr,
                       m->active_p_addr, m->active_helper_addr,
                       (m->foreground_screen_owner_p_addr != 0));
            }
        }
        return 1;
    }
    arm_ext_clear_empty_nonmodal_foreground_owner(m);
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
        if (arm_ext_diag_on()) {
            printf("DIAG screen_reject primary callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   (m->foreground_screen_owner_p_addr != 0));
        }
        return 0;
    }
    if (caller_p_addr == m->p_addr &&
        caller_helper_addr == m->helper_addr) {
        if (arm_ext_diag_on()) {
            printf("DIAG screen_reject wrapper callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   (m->foreground_screen_owner_p_addr != 0));
        }
        return 0;
    }
    return 1;
}

int arm_ext_should_accept_visible_present(ArmExtModule *m,
                                                 uint32_t *claim_p_addr,
                                                 uint32_t *claim_helper_addr) {
    if (arm_ext_should_accept_screen_write(m, claim_p_addr,
                                           claim_helper_addr)) {
        return 1;
    }

    uint32_t owner_helper_addr = 0;
    uint32_t owner_p_addr = arm_ext_screen_caller_owner(m, &owner_helper_addr);
    /*
     * A foreground child can own only part of the visible screen, for example
     * a top overlay while the primary game remains visible
     * underneath.  Explicit presents from the covered layer are therefore not
     * discarded; the draw path clips them against the child-owned cover rows.
     * Cache-only writes still use arm_ext_should_accept_screen_write() above
     * and remain invisible until an explicit present reaches this path.
     */
    if (!arm_ext_owner_is_covered_by_foreground(m, owner_p_addr,
                                                owner_helper_addr)) {
        return 0;
    }
    if (claim_p_addr) *claim_p_addr = owner_p_addr;
    if (claim_helper_addr) *claim_helper_addr = owner_helper_addr;
    return 1;
}

void arm_ext_finish_accepted_screen_write(ArmExtModule *m,
                                                 uint32_t claim_p_addr,
                                                 uint32_t claim_helper_addr) {
    if (!claim_p_addr) return;
    /*
     * Foreground ownership is metadata only.  Visible pixels are protected by
     * foreground_cover row spans; keeping an extra full-screen snapshot made
     * the advbar fix harder to audit and no longer feeds any restore path.
     * Compatibility impact: cache-only writes still stay hidden behind cover,
     * while explicit presents update cover/present spans as before.  Verified
     * with the opbzqe RIGHT-key PPM/vitest flow and related plugin e2e cases.
     */
    arm_ext_claim_foreground_screen_owner(m, claim_p_addr,
                                          claim_helper_addr);
}

void arm_ext_mirror_draw_bitmap_to_screen(ArmExtModule *m,
                                                 uint32_t bmp_addr,
                                                 int16_t x,
                                                 int16_t y,
                                                 uint16_t w,
                                                 uint16_t h,
                                                 int32_t source_stride,
                                                 int32_t source_x,
                                                 int32_t source_y) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr) || !arm_ptr(m, bmp_addr) ||
        source_stride <= 0) {
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
        uint32_t offset_pixels = (uint32_t)yy *
                                 (uint32_t)arm_ext_screen_stride(m) +
                                 (uint32_t)min_x;
        int32_t sx = source_x + (min_x - x);
        int32_t sy = source_y + (yy - y);
        if (sx < 0 || sy < 0 ||
            sx + (int32_t)row_pixels > source_stride) {
            return;
        }
        uint64_t source_offset_pixels =
            (uint64_t)(uint32_t)sy * (uint64_t)(uint32_t)source_stride +
            (uint64_t)(uint32_t)sx;
        uint64_t row_addr64 = (uint64_t)bmp_addr +
                              source_offset_pixels * sizeof(uint16_t);
        uint64_t row_end64 = row_addr64 +
                             (uint64_t)row_pixels * sizeof(uint16_t) - 1u;
        if (row_end64 > 0xFFFFFFFFu) return;
        uint16_t *src_row = (uint16_t *)arm_ptr(m, (uint32_t)row_addr64);
        if (!src_row || !arm_ptr(m, (uint32_t)row_end64)) return;
        /*
         * Host-visible bitmap presents can use either local bitmap rows or the
         * full screen cache as their source. Mirror the exact submitted source
         * rows into the ARM framebuffer so later context exit cannot restore the
         * covered layer with stale pixels.
         * Do not mark it dirty here: the explicit present already reached SDL,
         * and screen_dirty is reserved for cache writes that still need a
         * synthesized submit.
         */
        memcpy(dst_screen + offset_pixels, src_row,
               (size_t)row_pixels * sizeof(uint16_t));
    }
}

int arm_ext_bitmap_source_uses_screen_stride(ArmExtModule *m,
                                                    uint32_t bmp_addr) {
    if (!m || !bmp_addr) return 0;
    if (m->screen_addr && bmp_addr == m->screen_addr) return 1;

    uint32_t current_screen_addr = 0;
    if (arm_ext_read_table_u32_slot(m, 91, &current_screen_addr) &&
        current_screen_addr && bmp_addr == current_screen_addr) {
        return 1;
    }
    return 0;
}

enum {
    ARM_EXT_BM_OR = 0,
    ARM_EXT_BM_XOR = 1,
    ARM_EXT_BM_COPY = 2,
    ARM_EXT_BM_NOT = 3,
    ARM_EXT_BM_MERGENOT = 4,
    ARM_EXT_BM_ANDNOT = 5,
    ARM_EXT_BM_TRANSPARENT = 6,
    ARM_EXT_BM_AND = 7,
    ARM_EXT_BM_GRAY = 8,
    ARM_EXT_BM_REVERSE = 9,
    ARM_EXT_SPRITE_INDEX_MASK = 0x03FF,
    ARM_EXT_SPRITE_TRANSPARENT = 0x0400,
    ARM_EXT_TILE_SHIFT = 11,
    ARM_EXT_ROTATE_0 = 0,
    ARM_EXT_ROTATE_90 = 1,
    ARM_EXT_ROTATE_180 = 2,
    ARM_EXT_ROTATE_270 = 3,
};

static int arm_ext_bitmap_source_bounds(ArmExtModule *m,
                                        uint32_t p_addr,
                                        uint32_t *lo,
                                        uint32_t *hi) {
    uint32_t bitmap_array =
        arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 95u * 4u);
    if (lo) *lo = 0;
    if (hi) *hi = 0;
    if (!m || !p_addr) return 0;

    if (bitmap_array) {
        for (uint32_t i = 0; i < 31u; ++i) {
            uint32_t desc = bitmap_array + i * 16u;
            uint32_t len = arm_ext_read_u32_or_zero_(m, desc + 4u);
            uint32_t base = arm_ext_read_u32_or_zero_(m, desc + 12u);
            uint64_t end = (uint64_t)base + (uint64_t)len;
            if (!base || !len || end > 0x100000000ull) continue;
            if (p_addr >= base && (uint64_t)p_addr < end) {
                if (lo) *lo = base;
                if (hi) *hi = (uint32_t)end;
                return 1;
            }
        }
    }

    for (uint32_t i = 0; i < m->app_live_count; ++i) {
        uint32_t base = m->app_live_blocks[i].addr;
        uint32_t len = m->app_live_blocks[i].len;
        uint64_t end = (uint64_t)base + (uint64_t)len;
        if (!base || !len || end > 0x100000000ull) continue;
        if (p_addr >= base && (uint64_t)p_addr < end) {
            if (lo) *lo = base;
            if (hi) *hi = (uint32_t)end;
            return 1;
        }
    }
    return 0;
}

static int arm_ext_bitmap_read_source_pixel(ArmExtModule *m,
                                            uint64_t addr64,
                                            int bounded,
                                            uint32_t lo,
                                            uint32_t hi,
                                            uint16_t *color) {
    if (color) *color = 0;
    if (addr64 > 0xFFFFFFFEull) return 0;
    uint32_t addr = (uint32_t)addr64;
    if (bounded && (addr < lo || addr + sizeof(uint16_t) > hi)) return 0;
    void *p = arm_ptr(m, addr);
    if (!p || !arm_ptr(m, addr + 1u)) return 0;
    memcpy(color, p, sizeof(uint16_t));
    return 1;
}

static void arm_ext_bitmap_apply_rop(uint16_t *dst,
                                     uint16_t src,
                                     uint16_t rop,
                                     uint16_t transcoler) {
    if (!dst) return;
    switch (rop) {
        case ARM_EXT_BM_TRANSPARENT:
            if (src != transcoler) *dst = src;
            break;
        case ARM_EXT_BM_COPY:
            *dst = src;
            break;
        case ARM_EXT_BM_GRAY:
            if (src != transcoler) {
                uint32_t r = (src & 0xF800u) >> 11;
                uint32_t g = (src & 0x07E0u) >> 6;
                uint32_t b = src & 0x001Fu;
                uint32_t gray = (r * 60u + g * 118u + b * 22u) / 25u;
                *dst = MAKERGB565(gray, gray, gray);
            }
            break;
        case ARM_EXT_BM_REVERSE:
            if (src != transcoler) *dst = (uint16_t)~src;
            break;
        case ARM_EXT_BM_OR:
            *dst = (uint16_t)(src | *dst);
            break;
        case ARM_EXT_BM_XOR:
            *dst = (uint16_t)(src ^ *dst);
            break;
        case ARM_EXT_BM_NOT:
            *dst = (uint16_t)~src;
            break;
        case ARM_EXT_BM_MERGENOT:
            *dst = (uint16_t)((uint16_t)~src | *dst);
            break;
        case ARM_EXT_BM_ANDNOT:
            *dst = (uint16_t)((uint16_t)~src & *dst);
            break;
        case ARM_EXT_BM_AND:
            *dst = (uint16_t)(src & *dst);
            break;
        default:
            break;
    }
}

void arm_ext_draw_bitmap_from_guest(ArmExtModule *m,
                                    const ArmExtScreenContext *screen_ctx,
                                    uint32_t p_addr,
                                    int16_t x,
                                    int16_t y,
                                    uint16_t w,
                                    uint16_t h,
                                    uint16_t rop,
                                    uint16_t transcoler,
                                    int16_t sx,
                                    int16_t sy,
                                    int16_t mw) {
    if (!m || !p_addr || !mr_screenBuf || mr_screen_w <= 0 ||
        mr_screen_h <= 0) {
        return;
    }

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = (int32_t)x + (int32_t)w;
    int32_t max_y = (int32_t)y + (int32_t)h;
    if (max_x > mr_screen_w) max_x = mr_screen_w;
    if (max_y > mr_screen_h) max_y = mr_screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    uint32_t src_lo = 0;
    uint32_t src_hi = 0;
    int bounded = arm_ext_bitmap_source_bounds(m, p_addr, &src_lo, &src_hi);
    int32_t primary_stride = arm_ext_screen_stride(m);
    size_t primary_pixels = m->screen_w > 0 && m->screen_h > 0
        ? (size_t)m->screen_w * (size_t)m->screen_h
        : 0;
    uint64_t primary_lo = m->screen_addr;
    uint64_t primary_hi = primary_lo + primary_pixels * sizeof(uint16_t);
    uint64_t draw_lo = screen_ctx && screen_ctx->active
        ? (uint64_t)screen_ctx->target_addr +
              ((uint64_t)(uint32_t)min_y * (uint32_t)mr_screen_w +
               (uint32_t)min_x) * sizeof(uint16_t)
        : 0;
    uint64_t draw_hi = screen_ctx && screen_ctx->active
        ? (uint64_t)screen_ctx->target_addr +
              ((uint64_t)(uint32_t)(max_y - 1) * (uint32_t)mr_screen_w +
               (uint32_t)max_x) * sizeof(uint16_t)
        : 0;
    int track_primary_damage = primary_stride > 0 && primary_pixels > 0 &&
        draw_lo < primary_hi && draw_hi > primary_lo &&
        arm_ext_ensure_screen_regions(m);

    /*
     * table[120] passes guest bitmap pointers into host C. The native helper
     * indexes from that pointer without a byte length, but the emulator can see
     * table[95] bitmap descriptors and live app allocations. Keep source reads
     * inside that owner so a bad source rectangle cannot turn unrelated guest
     * heap/code bytes into pixels; pixels outside the owner are not drawn.
     */
    for (int32_t dy = min_y; dy < max_y; ++dy) {
        for (int32_t dx = min_x; dx < max_x; ++dx) {
            int64_t src_offset = -1;
            uint16_t src = 0;
            uint16_t draw_rop = rop;
            uint16_t *dst =
                mr_screenBuf + (size_t)dy * (size_t)mr_screen_w + (size_t)dx;

            if (rop > ARM_EXT_SPRITE_TRANSPARENT) {
                uint16_t bitmap_rop = rop & ARM_EXT_SPRITE_INDEX_MASK;
                uint16_t mode = (rop >> ARM_EXT_TILE_SHIFT) & 0x3u;
                uint16_t flip = (rop >> ARM_EXT_TILE_SHIFT) & 0x4u;
                int64_t rel_x = (int64_t)dx - (int64_t)x;
                int64_t rel_y = (int64_t)dy - (int64_t)y;
                int64_t row = 0;
                int64_t col = 0;
                if (bitmap_rop != ARM_EXT_BM_TRANSPARENT &&
                    bitmap_rop != ARM_EXT_BM_COPY) {
                    continue;
                }
                switch (mode) {
                    case ARM_EXT_ROTATE_0:
                        row = flip ? (int64_t)h - 1 - rel_y : rel_y;
                        col = rel_x;
                        break;
                    case ARM_EXT_ROTATE_90:
                        row = flip ? (int64_t)h - 1 - rel_x : rel_x;
                        col = (int64_t)w - 1 - rel_y;
                        break;
                    case ARM_EXT_ROTATE_180:
                        row = flip ? rel_y : (int64_t)h - 1 - rel_y;
                        col = (int64_t)w - 1 - rel_x;
                        break;
                    case ARM_EXT_ROTATE_270:
                        row = flip ? rel_x : (int64_t)h - 1 - rel_x;
                        col = rel_y;
                        break;
                    default:
                        continue;
                }
                src_offset = row * (int64_t)w + col;
                draw_rop = bitmap_rop;
            } else {
                src_offset =
                    ((int64_t)dy - (int64_t)y + (int64_t)sy) *
                        (int64_t)mw +
                    ((int64_t)dx - (int64_t)x + (int64_t)sx);
            }

            if (src_offset < 0) continue;
            uint64_t src_addr =
                (uint64_t)p_addr + (uint64_t)src_offset * sizeof(uint16_t);
            if (!arm_ext_bitmap_read_source_pixel(m, src_addr, bounded,
                                                  src_lo, src_hi, &src)) {
                continue;
            }
            uint16_t before = track_primary_damage ? *dst : 0;
            arm_ext_bitmap_apply_rop(dst, src, draw_rop, transcoler);
            if (track_primary_damage && *dst != before) {
                /* The old full-screen diff used the primary framebuffer's
                 * physical stride, not table[92]'s temporary draw stride. Map
                 * the changed destination address the same way so off-screen
                 * caches and transparent/no-op pixels remain damage-free. */
                uint64_t dst_addr = (uint64_t)screen_ctx->target_addr +
                    ((uint64_t)(uint32_t)dy * (uint32_t)mr_screen_w +
                     (uint32_t)dx) * sizeof(uint16_t);
                if (dst_addr >= primary_lo && dst_addr < primary_hi &&
                    ((dst_addr - primary_lo) & 1u) == 0) {
                    size_t pixel_offset =
                        (size_t)((dst_addr - primary_lo) / sizeof(uint16_t));
                    uint32_t damage_y =
                        (uint32_t)(pixel_offset / (uint32_t)primary_stride);
                    uint16_t damage_x =
                        (uint16_t)(pixel_offset % (uint32_t)primary_stride);
                    ArmExtRowSpans *damage = &m->screen_damage;
                    if (damage_y < damage->rows) {
                        if (damage_x < damage->min_x[damage_y])
                            damage->min_x[damage_y] = damage_x;
                        if ((uint16_t)(damage_x + 1u) >
                            damage->max_x[damage_y]) {
                            damage->max_x[damage_y] =
                                (uint16_t)(damage_x + 1u);
                        }
                    }
                }
            }
        }
    }
}


/*
 * 模态子模块(下载 UI/支付窗)关闭时恢复被其覆盖的主画面快照并 present。
 * Phase 3 去重:此前 arm_ext_call 与 arm_ext_call_dispatch 各有一份逐字
 * 相同的实现,修一处漏一处(issues doc M1);统一为唯一出口,行为不变。
 */
void arm_ext_restore_modal_screen_snapshot(ArmExtModule *m) {
    if (m->modal_screen_snapshot_valid &&
        m->modal_screen_snapshot &&
        m->modal_screen_snapshot_len == m->screen_len &&
        m->screen_addr && arm_ptr(m, m->screen_addr)) {
        memcpy(arm_ptr(m, m->screen_addr),
               m->modal_screen_snapshot, m->screen_len);
        if (mr_screenBuf) {
            /* B1:回拷与 present 都按宿主容量夹断 */
            uint16_t *snapshot = (uint16_t *)m->modal_screen_snapshot;
            int32_t pw = 0, ph = 0;
            arm_ext_copy_screen_to_host(m, mr_screenBuf, snapshot,
                                        arm_ext_screen_stride(m), &pw, &ph);
            if (pw > 0 && ph > 0)
                mr_drawBitmap(mr_screenBuf, 0, 0, (uint16)pw, (uint16)ph);
        }
        m->modal_screen_snapshot_valid = 0;
    }
}

/*
 * guest 经 plat(101) 设置 LCD 旋转成功后调用(aex_t037),等价于真机 LCD
 * 驱动旋转后更新平台屏幕全局:把模块画布基准 m->screen_w/h 与 ARM 可见的
 * mr_screen_w/h(table[92/93] 槽)更新为旋转后的显示尺寸(vmrp_config 面板
 * 尺寸的转置)。画布字节数在转置下不变(w*h*2 相等),无需迁移缓冲。
 *
 * 之后转置拒绝保护(arm_ext_push_draw_screen_context)与
 * hook_screen_dim_read 以旋转后的尺寸为基准工作:gtcm 随后写入的横屏画布
 * (480,320) 与基准相等被平凡采纳,付费框写入的 (320,480) 才是转置的撤销
 * 请求照常被拒绝——与旋转前以 --screen 480x320 预转窗口运行的行为一致。
 *
 * 宿主侧 memcpy 写 guest 内存不会触发 unicorn 内存钩子,钩子按地址注册也
 * 无需重挂。
 */
void arm_ext_apply_lcd_rotation(ArmExtModule *m) {
    if (!m || !m->screen_w_slot || !m->screen_h_slot) return;
    uint32_t dw = (uint32_t)vmrp_display_width();
    uint32_t dh = (uint32_t)vmrp_display_height();
    if (!dw || !dh) return;
    m->screen_w = (int32_t)dw;
    m->screen_h = (int32_t)dh;
    m->screen_len = dw * dh * 2u;
    memcpy(arm_ptr(m, m->screen_w_slot), &dw, 4);
    memcpy(arm_ptr(m, m->screen_h_slot), &dh, 4);
}

void hook_screen_dim_write(uc_engine *uc, uc_mem_type type,
                                  uint64_t address, int size, int64_t value,
                                  void *user_data) {
    (void)uc;
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (!m || size <= 0) return;

    /* 仅当 ARM 可见 mr_screenBuf(table[91])仍指向宿主屏幕缓冲时才迁移:
     * gtcm 在 init 配置 480x320 逻辑画布时 buf 未动,需要迁移扩容;gghjt
     * 滚屏离屏合成先把 table[91] 指到自有缓冲再写更宽的 w(272),此时
     * 迁移宿主缓冲反而使恢复后的 buf 与 screen_addr 失配导致花屏。 */
    {
        uint32_t slot91_ptr = arm_ext_read_u32_or_zero_(
            m, EXT_TABLE_ADDR + 91u * 4u);
        uint32_t cur_buf = arm_ext_read_u32_or_zero_(m, slot91_ptr);
        if (cur_buf != m->screen_addr) return;
    }

    uint32_t addr = (uint32_t)address;
    /* 写入尚未落盘:被写维度取 value,另一维读当前内存 */
    uint32_t w = (addr == m->screen_w_slot)
                     ? (uint32_t)value
                     : arm_ext_read_u32_or_zero_(m, m->screen_w_slot);
    uint32_t h = (addr == m->screen_h_slot)
                     ? (uint32_t)value
                     : arm_ext_read_u32_or_zero_(m, m->screen_h_slot);
    if (w == 0 || h == 0 || w > 2048 || h > 2048) return;

    uint32_t need = w * h * 2u;
    uint32_t top_addr = EXT_BASE_ADDR + EXT_MEM_SIZE - EXT_SCREEN_RESERVE;
    if (need <= m->screen_cap || m->screen_addr == top_addr ||
        need > EXT_SCREEN_RESERVE) {
        return;
    }

    uint8_t *dst = (uint8_t *)arm_ptr(m, top_addr);
    const uint8_t *src = (const uint8_t *)arm_ptr(m, m->screen_addr);
    if (!dst || !src) return;
    memset(dst, 0, need);
    /* 旧内容按行搬运(stride 变化),保留已绘制画面 */
    uint32_t copy_w = ((uint32_t)m->screen_w < w ? (uint32_t)m->screen_w : w) * 2u;
    uint32_t copy_h = (uint32_t)m->screen_h < h ? (uint32_t)m->screen_h : h;
    for (uint32_t yy = 0; yy < copy_h; ++yy) {
        memcpy(dst + yy * w * 2u, src + yy * (uint32_t)m->screen_w * 2u, copy_w);
    }
    /* 同步 ARM 可见 mr_screenBuf 变量(slot91 指向的 u32) */
    uint32_t slot91 = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 91u * 4u);
    if (slot91 && arm_ptr(m, slot91)) {
        memcpy(arm_ptr(m, slot91), &top_addr, 4);
    }
    /* 同步 table[95] 屏幕位图描述项(第 30 项: w,h,len,type,addr) */
    uint32_t bmp_array = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 95u * 4u);
    if (bmp_array && arm_ptr(m, bmp_array + 30u * 16u + 15u)) {
        uint8_t *bm = (uint8_t *)arm_ptr(m, bmp_array + 30u * 16u);
        uint16_t w16 = (uint16_t)w;
        uint16_t h16 = (uint16_t)h;
        memcpy(bm + 0, &w16, 2);
        memcpy(bm + 2, &h16, 2);
        memcpy(bm + 4, &need, 4);
        memcpy(bm + 12, &top_addr, 4);
    }
    m->screen_addr = top_addr;
    m->screen_cap = EXT_SCREEN_RESERVE;
    /* screen_w/h/len 不在此处更新:绘制入口(arm_ext_push_draw_screen_context)
     * 读取 guest 变量后在容量内统一采纳,保证与后续尺寸调整走同一路径 */
}

/* guest 读回 ARM 可见 mr_screen_w/h 时触发。gtcm 进下载/付费提示前把逻辑
 * 画布写成宿主画布的转置(按 LCD 原生竖屏 320x480,真机上伴随撤销
 * plat(101) 旋转)。展示层没有旋转能力、窗口方向固定;若放任转置尺寸被
 * 读回,提示 UI 会按竖屏 320 宽布局,横屏窗口右侧 160px 永远不被绘制。
 * 写钩子在写指令落盘前触发无法当场改写,改为在 guest 读回前把两个变量
 * 恢复成宿主尺寸,guest 按窗口方向(480x320)重新布局。仅当两值恰为宿主
 * 尺寸的转置时生效,不影响 gghjt 滚屏离屏合成等改写更宽 stride 的场景。 */
void hook_screen_dim_read(uc_engine *uc, uc_mem_type type,
                                 uint64_t address, int size, int64_t value,
                                 void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (!m || m->screen_w == m->screen_h) return;

    uint32_t w = arm_ext_read_u32_or_zero_(m, m->screen_w_slot);
    uint32_t h = arm_ext_read_u32_or_zero_(m, m->screen_h_slot);
    if (w != (uint32_t)m->screen_h || h != (uint32_t)m->screen_w) return;

    /* 仅主屏幕缓冲:table[91] 指向自有离屏缓冲时不干预 */
    uint32_t slot91 = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 91u * 4u);
    uint32_t cur_buf = arm_ext_read_u32_or_zero_(m, slot91);
    if (cur_buf != m->screen_addr) return;

    uint32_t hw = (uint32_t)m->screen_w;
    uint32_t hh = (uint32_t)m->screen_h;
    memcpy(arm_ptr(m, m->screen_w_slot), &hw, 4);
    memcpy(arm_ptr(m, m->screen_h_slot), &hh, 4);
}

