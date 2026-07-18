/*
 * aex_table.c —— mr_table 系统调用 handler(Phase 3 case 函数化第 1 批)。
 *
 * 由 hook_table(arm_ext_executor.c)经 aex_table_handlers 表分发;本批
 * 收纳所有不含循环/内层 switch 的简单 case,函数体自原 switch 原样迁移
 * (break → goto aex_done)。复杂 case(3/14/44/125/131 等)仍在残余
 * switch,后续批次迁移并按域拆文件。
 */
#include "../include/arm_ext_priv.h"
#include "../include/network.h"
#include "../include/fileLib.h"
#include "../include/utils.h"

#include <ctype.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif

static void aex_t001(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;

            /* 原生 table[1] = mr_free(p,len):池内块插回空闲链表并合并
             * ("先 free 再让 readFile 复用该块"的原生模式据此成立);
             * bump 后备块进宿主侧回收器(只认登记过的地址)。 */
            arm_ext_app_mem_free(m, r0, r1);
            note_origin_mem_free(m, r1);
            ret = MR_SUCCESS;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t002(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* 原生 table[2] = mr_realloc(p,oldlen,newlen),mem.c:
             * p==NULL 等价 malloc;newlen==0 等价 free 返回 NULL;
             * 其余 malloc→拷贝 min(oldlen,newlen)→free(p,oldlen)。
             * 分配同 table[0] 池优先、bump 后备;统计与修复前一致仅记
             * 新块(note_origin_mem_alloc)。 */
            if (!r0) {
                ret = arm_ext_app_mem_malloc(m, r2);
                if (ret) note_origin_mem_alloc(m, r2);
                goto aex_done;
            }
            if (!r2) { arm_ext_app_mem_free(m, r0, r1); ret = 0; goto aex_done; }
            uint32_t p = arm_ext_app_mem_malloc(m, r2);
            if (p) {
                note_origin_mem_alloc(m, r2);
                /* B2:旧块指针非法(guest 状态损坏)时跳过拷贝,不解引用 NULL */
                uint32_t n = r1 < r2 ? r1 : r2;
                void *dst2 = arm_ptr_span(m, p, n);
                void *src2 = arm_ptr_span(m, r0, n);
                if (n && dst2 && src2) memmove(dst2, src2, n);
                arm_ext_app_mem_free(m, r0, r1);
            }
            ret = p;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t004(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            void *mv_dst = arm_ptr_span(m, r0, r2);
            void *mv_src = arm_ptr_span(m, r1, r2);
            if (r2 && mv_dst && mv_src) memmove(mv_dst, mv_src, r2);
            ret = r0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t005(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            const char *cp_src = arm_str(m, r1);
            uint32_t n = (uint32_t)strlen(cp_src) + 1u;
            char *cp_dst = (char *)arm_ptr_span(m, r0, n);
            if (cp_dst) memcpy(cp_dst, cp_src, n); /* 等价 strcpy(含 NUL) */
            ret = r0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t006(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            char *ncp_dst = (char *)arm_ptr_span(m, r0, r2);
            if (r2 && ncp_dst)
                arm_ext_guest_strncpy(ncp_dst, arm_str(m, r1), r2);
            ret = r0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t007(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            const char *cat_src = arm_str(m, r1);
            char *cat_dst0 = (char *)arm_ptr(m, r0);
            if (cat_dst0) {
                uint32_t need = (uint32_t)strlen(cat_dst0) +
                                (uint32_t)strlen(cat_src) + 1u;
                char *cat_dst = (char *)arm_ptr_span(m, r0, need);
                if (cat_dst) strcat(cat_dst, cat_src);
            }
            ret = r0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t008(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            const char *ncat_src = arm_str(m, r1);
            char *ncat_dst0 = (char *)arm_ptr(m, r0);
            if (ncat_dst0) {
                uint32_t sl = (uint32_t)strlen(ncat_src);
                if (sl > r2) sl = r2;
                uint32_t need = (uint32_t)strlen(ncat_dst0) + sl + 1u;
                char *ncat_dst = (char *)arm_ptr_span(m, r0, need);
                if (ncat_dst) strncat(ncat_dst, ncat_src, r2);
            }
            ret = r0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t009(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            const void *cmp_a = arm_ptr_span(m, r0, r2);
            const void *cmp_b = arm_ptr_span(m, r1, r2);
            if (r2 && cmp_a && cmp_b) ret = memcmp(cmp_a, cmp_b, r2);
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t010(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = strcmp(arm_str(m, r0), arm_str(m, r1)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t011(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = strncmp(arm_str(m, r0), arm_str(m, r1), r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t013(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* B2:区间可映射才搜索 */
            void *hay = arm_ptr_span(m, r0, r2);
            void *p = (hay && r2) ? memchr(hay, (int)r1, r2) : NULL;
            ret = p ? arm_addr(m, p) : 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t012(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = strcoll(arm_str(m, r0), arm_str(m, r1)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t015(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = strlen(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t016(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            char *hay = (char *)arm_ptr(m, r0);
            char *nee = (char *)arm_ptr(m, r1);
            if (!hay || !nee) { ret = 0; goto aex_done; }
            char *p = strstr(hay, nee);
            ret = p ? arm_addr(m, p) : 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t017(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            char buf[1024];
            ret = format_arm(m, buf, sizeof(buf), arm_str(m, r1), 2);
            /* B2:输出区可映射才写回 */
            char *fmt_dst =
                (char *)arm_ptr_span(m, r0, (uint32_t)strlen(buf) + 1u);
            if (fmt_dst) memcpy(fmt_dst, buf, strlen(buf) + 1u);
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t018(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = atoi(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t019(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = (uint32_t)strtoul(arm_str(m, r0), NULL, r1); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t020(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = mr_rand(); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t022(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;

            ret = mr_stop_ex((int16)r0);
            internal_slot_write(m, m->mr_state_slot, 0);
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t025(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            uint32_t nested_file_addr = 0;
            uint32_t nested_file_len = 0;
            int nested = arm_ext_nested_exec_range_for_lr(
                m, lr, &nested_file_addr, &nested_file_len);
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
                memcpy(arm_ptr(m, nested_file_addr + 4), &p_addr, 4);
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
                arm_ext_record_nested_module(m, nested_file_addr,
                                             nested_file_len, p_addr, r0);
                if (!m->primary_helper_addr) {
                    m->primary_helper_addr = r0;
                    m->primary_p_addr = p_addr;
                    m->primary_file_addr = nested_file_addr;
                    m->primary_file_len = nested_file_len;
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
                if (arm_ext_trace_on()) {
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
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t026(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            char buf[1024];
            const char *fmt = arm_str(m, r0);
            format_arm(m, buf, sizeof(buf), fmt, 1);
            mr_printf("%s", buf);
            ret = 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t027(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            uint32_t base = m->origin_mem_addr;
            uint32_t len = m->origin_mem_len;
            if (r0) memcpy(arm_ptr(m, r0), &base, 4);
            if (r1) memcpy(arm_ptr(m, r1), &len, 4);
            ret = base ? MR_SUCCESS : MR_FAILED;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t028(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = MR_SUCCESS; goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t029(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 {
            void *bmp = arm_ptr(m, r0);
            uint16_t h = (uint16_t)arg_read(m, 4);
            uint32_t claim_p = 0, claim_helper = 0;
            if (bmp) {
                int screen_stride_source =
                    arm_ext_bitmap_source_uses_screen_stride(m, r0);
                int32_t source_stride = screen_stride_source ?
                    arm_ext_screen_stride(m) : (int32_t)(uint16)r3;
                int32_t source_x = screen_stride_source ? (int16)r1 : 0;
                int32_t source_y = screen_stride_source ? (int16)r2 : 0;
                int accept = arm_ext_should_accept_visible_present(m, &claim_p,
                                                                   &claim_helper);
                if (arm_ext_diag_on()) {
                    int covered_diag = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    printf("DIAG present29 x=%d y=%d w=%u h=%u accept=%d claimP=0x%X claimH=0x%X covered=%d\n",
                           (int16)r1, (int16)r2, (uint16)r3, h, accept,
                           claim_p, claim_helper, covered_diag);
                    arm_ext_diag_dump_layer_state(m, "present29");
                }
                if (accept) {
                    int covered = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    arm_ext_present_bitmap_rect(m, bmp, (int16)r1, (int16)r2,
                                                (uint16)r3, h,
                                                source_stride, source_x,
                                                source_y, covered);
                }
                /*
                 * A rejected foreground present is still a write into the
                 * shared backing screen cache when its source is a bitmap.
                 * Foreground overlays can later proxy uncovered regions from
                 * that cache; skipping this mirror leaves those regions black
                 * even though the visible submit was correctly blocked.
                 */
                arm_ext_mirror_draw_bitmap_to_screen(m, r0, (int16)r1,
                                                     (int16)r2,
                                                     (uint16)r3, h,
                                                     source_stride, source_x,
                                                     source_y);
                if (accept) {
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                    arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                         claim_helper,
                                                         (int16)r1,
                                                         (int16)r2,
                                                         (uint16)r3, h);
                    if (arm_ext_owner_is_foreground_child(m, claim_p,
                                                          claim_helper)) {
                        arm_ext_note_foreground_cover_rect(m, (int16)r1,
                                                           (int16)r2,
                                                           (uint16)r3, h);
                    }
                }
            }
            ret = MR_SUCCESS;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t030(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 {
            int width = 0;
            int height = 0;
            const char *bitmap = mr_getCharBitmap((uint16)r0, (uint16)r1, &width, &height);
            // 根据字体实际尺寸计算位图大小：gb12=18字节(12行×12位紧凑), gb16=32字节(16行×2字节)
            int bitmap_size = ((width * height) + 7) >> 3;
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &width, 4);
            if (r3 && arm_ptr(m, r3)) memcpy(arm_ptr(m, r3), &height, 4);
            if (bitmap) {
                if (!m->char_bitmap_addr) m->char_bitmap_addr = arm_alloc(m, 32);
                /* B5:缓冲固定 32 字节,当前 sky12/sky16 字体最大恰为 32;
                 * 若将来字体返回更大位图,夹断并告警而不是越界写 ARM 堆 */
                if (bitmap_size > 32) {
                    printf("arm_ext_executor: char bitmap %dx%d size %d exceeds 32-byte slot, truncated\n",
                           width, height, bitmap_size);
                    bitmap_size = 32;
                }
                if (m->char_bitmap_addr && bitmap_size > 0) {
                    memcpy(arm_ptr(m, m->char_bitmap_addr), bitmap, bitmap_size);
                    ret = m->char_bitmap_addr;
                } else {
                    ret = 0;
                }
            } else {
                ret = 0;
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t031(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0; /* r1-r3 仅出现在 DIAG 格式串文本中,不作变量使用 */
    uint32_t ret = MR_SUCCESS;
 {
            uint32_t pc_diag = 0;
            uint32_t lr_diag = 0;
            if (arm_ext_diag_on()) {
                pc_diag = reg_read32(m->uc, UC_ARM_REG_PC);
                lr_diag = reg_read32(m->uc, UC_ARM_REG_LR);
                printf("DIAG table31_pre t=%u pc=0x%X lr=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X r4=0x%X r5=0x%X r6=0x%X r7=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X timerP=0x%X timerH=0x%X\n",
                       (uint16)r0, pc_diag, lr_diag,
                       reg_read32(m->uc, UC_ARM_REG_R0),
                       reg_read32(m->uc, UC_ARM_REG_R1),
                       reg_read32(m->uc, UC_ARM_REG_R2),
                       reg_read32(m->uc, UC_ARM_REG_R3),
                       reg_read32(m->uc, UC_ARM_REG_R4),
                       reg_read32(m->uc, UC_ARM_REG_R5),
                       reg_read32(m->uc, UC_ARM_REG_R6),
                       reg_read32(m->uc, UC_ARM_REG_R7),
                       reg_read32(m->uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr,
                       m->timer_p_addr, m->timer_helper_addr);
            }
            ret = mr_timerStart((uint16)r0);
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            arm_ext_record_timer_owner(m);
            if (arm_ext_diag_on()) {
                printf("DIAG table31_post ret=0x%X pc=0x%X lr=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X r4=0x%X r5=0x%X r6=0x%X r7=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X timerP=0x%X timerH=0x%X\n",
                       ret, reg_read32(m->uc, UC_ARM_REG_PC),
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       reg_read32(m->uc, UC_ARM_REG_R0),
                       reg_read32(m->uc, UC_ARM_REG_R1),
                       reg_read32(m->uc, UC_ARM_REG_R2),
                       reg_read32(m->uc, UC_ARM_REG_R3),
                       reg_read32(m->uc, UC_ARM_REG_R4),
                       reg_read32(m->uc, UC_ARM_REG_R5),
                       reg_read32(m->uc, UC_ARM_REG_R6),
                       reg_read32(m->uc, UC_ARM_REG_R7),
                       reg_read32(m->uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr,
                       m->timer_p_addr, m->timer_helper_addr);
            }
            goto aex_done;
        }
aex_done:
    c->ret = ret;
}

static void aex_t032(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 {
            ret = mr_timerStop();
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            mr_timer_state = 0;
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t033(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 {
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
        } goto aex_done;
        /* case 34/35(B2):输出结构体指针非法时返回失败,宿主不向 NULL 写 */
aex_done:
    c->ret = ret;
}

static void aex_t034(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            void *dt = arm_ptr(m, r0);
            ret = dt ? (uint32_t)mr_getDatetime(dt) : (uint32_t)MR_FAILED;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t035(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            void *ui = arm_ptr(m, r0);
            ret = ui ? (uint32_t)mr_getUserInfo(ui) : (uint32_t)MR_FAILED;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t036(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_sleep(r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t037(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;

            ret = mr_plat((int32)r0, (int32)r1);
            /* plat(101)=LCD 旋转:设置成功后把模块画布基准与 ARM 可见
             * mr_screen_w/h 同步为旋转后的显示尺寸(真机 LCD 驱动行为),
             * 之后 guest 写入的横屏画布尺寸与基准相等即被采纳。 */
            if (r0 == 101u && ret == MR_SUCCESS) {
                arm_ext_apply_lcd_rotation(m);
            }
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0;
                uint32_t owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table37 code=%d param=0x%X ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r1, ret,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t039(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = mr_ferrno(); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t040(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            const char *open_name = arm_str(m, r0);
            ret = mr_open(arm_ext_pack_to_host_path(m, open_name), r1);
            /* 磁盘上找不到时尝试从 MRP 缓存提供虚拟 fd */
            if (ret == 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, open_name);
                if (ce) {
                    ret = mrp_vfd_open(m, ce->data, ce->data_len);
                }
            }
            if ((int32_t)ret > 0) {
                arm_ext_diag_fd_set(m, (int32_t)ret, open_name);
            }
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0;
                ArmExtNestedModule *owner =
                    arm_ext_resource_owner_for_lr(m, &owner_p, &owner_h);
                printf("DIAG table40 name='%s' flags=0x%X ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       open_name, r1, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, m->active_p_addr,
                       m->primary_p_addr);
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t041(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            const char *diag_name = arm_ext_diag_fd_name(m, (int32_t)r0);
            MrpVirtualFd *vf = mrp_vfd_get(m, r0);
            if (vf) { vf->in_use = 0; ret = MR_SUCCESS; }
            else {
                ret = mr_close((int32)r0);
            }
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table41 fd=%d name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, diag_name, ret,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            if (ret == MR_SUCCESS) {
                arm_ext_diag_fd_clear(m, (int32_t)r0);
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t042(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            const char *info_name = arm_str(m, r0);
            ret = mr_info(arm_ext_pack_to_host_path(m, info_name));
            /* 磁盘上不存在时检查 MRP 缓存 */
            if (ret != MRP_IS_FILE && m->mrp_cache_count > 0) {
                if (mrp_cache_find(m, info_name))
                    ret = MRP_IS_FILE;
            }
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0;
                ArmExtNestedModule *owner =
                    arm_ext_resource_owner_for_lr(m, &owner_p, &owner_h);
                printf("DIAG table42 name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       info_name, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, m->active_p_addr,
                       m->primary_p_addr);
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t043(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* mr_write consumes the complete guest range.  Validating only the
             * first byte lets a wrapped/oversized length make the host write(2)
             * read past the 16MB guest backing and report a misleading partial
             * success.  Match table[44]/mr_read: an unmapped span is an ABI
             * failure and must never be dereferenced by the host. */
            void *src = arm_ptr_span(m, r1, r2 ? r2 : 1u);
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
            ret = src ? mr_write((int32)r0, src, len) : MR_FAILED;
            if (substituted) {
                if (m->profile && m->profile->post_write_cleanup)
                    m->profile->post_write_cleanup(m->app_state);
                /* ret 为 uint32_t,与 len 直接比较即位模式相等判定(与原
                 * (int32_t) 强转比较数值等价),消除符号比较告警 */
                if (ret == len) ret = (int32_t)r2;
            }
            if (arm_ext_diag_on()) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_preview_bytes(src, len, preview, sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table43 fd=%d name='%s' src=0x%X len=%u ret=0x%X subst=%d preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, arm_ext_diag_fd_name(m, (int32_t)r0),
                       r1, r2, ret, substituted, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t045(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
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
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t046(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            const char *len_name = arm_str(m, r0);
            ret = mr_getLen(arm_ext_pack_to_host_path(m, len_name));
            /* 原有 "ret < 0 时查 MRP 缓存" 回退块因 ret 为 uint32_t 恒不成立,
             * 属从未生效的死代码(-Wtype-limits 证实),按"禁止兜底逻辑"规范
             * 删除;删除即保持现行为,getLen 失败语义与真机一致由调用方处理 */
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table46 name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       len_name, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t047(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_remove(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t048(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            const char *old_name = arm_str(m, r0);
            const char *new_name = arm_str(m, r1);
            ret = mr_rename(old_name, new_name);
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t049(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_mkDir(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t050(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_rmDir(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t051(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_findStart(arm_str(m, r0), arm_ptr(m, r1), r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t052(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_findGetNext((int32)r0, arm_ptr(m, r1), r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t053(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_findStop((int32)r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t054(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = mr_exit(); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t055(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_startShake((int32)r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t056(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = mr_stopShake(); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t057(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 ret = mr_playSound((int)r0, arm_ptr(m, r1), r2, (int32)r3); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t058(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_stopSound((int)r0); goto aex_done;
        /* table[59] mr_sendSms(pNumber, pContent, encode) */
aex_done:
    c->ret = ret;
}

static void aex_t059(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            int32 flags = (int32)r2;
            ret = mr_sendSms(arm_str(m, r0), arm_str(m, r1), flags);
            if (ret == MR_SUCCESS && (flags & ARM_EXT_MR_SMS_RESULT_FLAG)) {
                m->pending_sms_result = 1;
                m->pending_sms_result_value = ret;
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t060(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 mr_call(arm_str(m, r0)); ret = MR_SUCCESS; goto aex_done;
        /* table[61] mr_getNetworkID()：返回 0 表示移动网络（MR_NET_ID_MOBILE） */
aex_done:
    c->ret = ret;
}

static void aex_t061(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = 0; goto aex_done;
        /* table[81] mr_initNetwork(cb, mode)：读取 mode 并设置 isCMWAP 标志，
         * 同步返回成功。桌面端没有真实 GPRS 网络，但需要 isCMWAP 控制后续
         * send/recv 的代理模拟逻辑。 */
aex_done:
    c->ret = ret;
}

static void aex_t081(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            const char *mode_str = arm_str(m, r1);
            ret = mr_initNetwork(NULL, mode_str);
        } goto aex_done;
        /* table[82] mr_closeNetwork()：return success，跟 mr_initNetwork 配对。 */
aex_done:
    c->ret = ret;
}

static void aex_t082(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = MR_SUCCESS; goto aex_done;
        /* table[83] mr_getHostByName(name, cb)。
         * 异步回调 cb 是 ARM 虚拟地址，不能在宿主线程调用（会把 ARM
         * 地址当宿主函数指针，段错误）。绕过 DSM 回调链直接调用底层
         * my_getHostByName(cb=NULL) 走同步 DNS 解析路径，返回 IP。 */
aex_done:
    c->ret = ret;
}

static void aex_t083(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            const char *host = arm_str(m, r0);
            ret = my_getHostByName(NULL, host, NULL, NULL);
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t084(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = mr_socket((int32)r0, (int32)r1); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t085(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 ret = mr_connect((int32)r0, (int32)r1, (uint16)r2, (int32)r3); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t086(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;

            ret = mr_closeSocket((int32)r0);
            if (arm_ext_diag_on()) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table86 socket=%d ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t087(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            void *dst = arm_ptr(m, r1);
            ret = mr_recv((int32)r0, dst, (int)r2);
            if (arm_ext_diag_on()) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                uint32_t preview_len = (int32_t)ret > 0 ? (uint32_t)ret : 0;
                arm_ext_diag_preview_bytes(dst, preview_len, preview,
                                           sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table87 socket=%d want=%u ret=0x%X dst=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X openSockets=%d\n",
                       (int32_t)r0, r2, ret, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr,
                       my_openSocketCount());
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t088(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 { uint32_t a4 = arg_read(m, 4);
                   ret = mr_recvfrom((int32)r0, arm_ptr(m, r1), (int)r2,
                                    (int32 *)arm_ptr(m, r3), (uint16 *)arm_ptr(m, a4));
                 } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t089(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            void *src = arm_ptr(m, r1);
            if (arm_ext_diag_on()) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_preview_bytes(src, r2, preview, sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table89_pre socket=%d len=%u src=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r2, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            ret = mr_send((int32)r0, src, (int)r2);
            if (arm_ext_diag_on()) {
                printf("DIAG table89_post socket=%d len=%u ret=0x%X openSockets=%d\n",
                       (int32_t)r0, r2, ret, my_openSocketCount());
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t090(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 { uint32_t a4 = arg_read(m, 4);
                   ret = mr_sendto((int32)r0, arm_ptr(m, r1), (int)r2, (int32)r3, (uint16)a4);
                 } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t069(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_dialogCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t070(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_dialogRelease((int32)r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t071(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 ret = mr_dialogRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2), (int32)r3); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t072(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_textCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t073(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_textRelease((int32)r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t074(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_textRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t075(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 ret = mr_editCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2, (int32)r3); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t076(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = mr_editRelease((int32)r0); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t077(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            const char *text = mr_editGetText((int32)r0);
            /* mr_editGetText returns UCS2-BE text.  ASCII therefore contains a
             * zero high byte before every character, so strlen() would copy only
             * the first byte and make ARM browser editors see an empty string. */
            ret = text ? alloc_bytes(m, text, (uint32_t)wstrlen((char *)text) + 2) : 0;
        } goto aex_done;
        /* table[78] mr_winCreate / table[79] mr_winRelease: 窗口功能不支持 */
aex_done:
    c->ret = ret;
}

static void aex_t078(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = MR_IGNORE; goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t079(ArmExtModule *m, AexTableCtx *c) {
    (void)m;

    uint32_t ret = MR_SUCCESS;
 ret = MR_IGNORE; goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t080(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            /* B2:同 case 34/35 */
            void *si = arm_ptr(m, r0);
            ret = si ? (uint32_t)mr_getScreenInfo(si) : (uint32_t)MR_FAILED;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t113(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 {
            void *p = arm_ptr(m, r0);
            if (p) mr_md5_init(p);
            ret = 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t114(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            void *p = arm_ptr(m, r0);
            void *d = arm_ptr(m, r1);
            if (p && d) mr_md5_append(p, d, (int)r2);
            ret = 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t115(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 {
            void *p = arm_ptr(m, r0);
            void *digest = arm_ptr(m, r1);
            if (p && digest) mr_md5_finish(p, digest);
            ret = 0;
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t118(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

            if (arm_ext_diag_on()) {
                printf("DIAG DispUpEx x=%d y=%d w=%u h=%u currentP=0x%X currentH=0x%X activeP=0x%X activeH=0x%X\n",
                       (int16)r0, (int16)r1, (uint16)r2, (uint16)r3,
                       m->current_p_addr, m->current_helper_addr,
                       m->active_p_addr, m->active_helper_addr);
            }
            {
                uint32_t claim_p = 0, claim_helper = 0;
                int accept = arm_ext_should_accept_visible_present(m, &claim_p,
                                                                   &claim_helper);
                if (arm_ext_diag_on()) {
                    int covered_diag = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    printf("DIAG present118 x=%d y=%d w=%u h=%u accept=%d claimP=0x%X claimH=0x%X covered=%d\n",
                           (int16)r0, (int16)r1, (uint16)r2, (uint16)r3,
                           accept, claim_p, claim_helper, covered_diag);
                    arm_ext_diag_dump_layer_state(m, "present118");
                }
                if (accept) {
                    ArmExtScreenContext screen_ctx;
                    if (arm_ext_push_draw_screen_context(m, &screen_ctx) &&
                        arm_ext_screen_context_targets_primary(m, &screen_ctx)) {
                        int covered = arm_ext_owner_is_covered_by_foreground(
                            m, claim_p, claim_helper);
                        ret = arm_ext_dispup_rect(m, (int16)r0, (int16)r1,
                                                  (uint16)r2, (uint16)r3,
                                                  covered);
                        arm_ext_finish_accepted_screen_write(m, claim_p,
                                                             claim_helper);
                        arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                             claim_helper,
                                                             (int16)r0,
                                                             (int16)r1,
                                                             (uint16)r2,
                                                             (uint16)r3);
                        if (arm_ext_owner_is_foreground_child(m, claim_p,
                                                              claim_helper)) {
                            arm_ext_note_foreground_cover_rect(m, (int16)r0,
                                                               (int16)r1,
                                                               (uint16)r2,
                                                               (uint16)r3);
                        }
                    } else {
                        ret = 0;
                    }
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                } else {
                    ret = 0;
                }
            }
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t119(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;

            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    _DrawPoint((int16)r0, (int16)r1, (uint16)r2);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_rect(m, (int16)r0,
                                                    (int16)r1, 1, 1);
                    arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                         claim_helper,
                                                         (int16)r0,
                                                         (int16)r1, 1, 1);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t120(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

            {
                uint32_t claim_p = 0, claim_helper = 0;
                uint16_t h = (uint16_t)arg_read(m, 4);
                uint16_t rop = (uint16_t)arg_read(m, 5);
                uint16_t trans = (uint16_t)arg_read(m, 6);
                int16_t sx = (int16)arg_read(m, 7);
                int16_t sy = (int16)arg_read(m, 8);
                int16_t mw = (int16)arg_read(m, 9);
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    arm_ext_draw_bitmap_from_guest(
                        m, &screen_ctx, r0, (int16)r1, (int16)r2,
                        (uint16)r3, h, rop, trans, sx, sy, mw);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    /* The draw helper records only primary-screen pixels whose
                     * RGB565 value changed in its existing per-pixel loop. This
                     * preserves the old diff semantics for off-screen targets,
                     * transparency and rejected sources without an O(screen)
                     * snapshot/scan per blit. Verify with the focused
                     * performance/paired-PPM test, then off-screen and golden
                     * E2E cases before the full suite. */
                    arm_ext_claim_foreground_screen_rect(
                        m, claim_p, claim_helper,
                        (int16)r1, (int16)r2, (uint16)r3, h);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t122(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

            {
                uint32_t claim_p = 0, claim_helper = 0;
                int16_t x = (int16)r0;
                int16_t y = (int16)r1;
                int16_t w = (int16)r2;
                int16_t h = (int16)r3;
                uint8_t cr = (uint8)arg_read(m, 4);
                uint8_t cg = (uint8)arg_read(m, 5);
                uint8_t cb = (uint8)arg_read(m, 6);
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    int full_black_clear =
                        arm_ext_screen_context_targets_primary(m, &screen_ctx) &&
                        x <= 0 && y <= 0 &&
                        (int32_t)x + (int32_t)w >= m->screen_w &&
                        (int32_t)y + (int32_t)h >= m->screen_h &&
                        cr == 0 && cg == 0 && cb == 0;
                    DrawRect(x, y, w, h, cr, cg, cb);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    /*
                     * A primary full-screen black DrawRect is commonly a cache
                     * reset before partial repaint/present.  Treat it as a
                     * backing write only so callback-exit damage synthesis does
                     * not expose pixels the app never explicitly presented.
                     */
                    if (!full_black_clear) {
                        arm_ext_note_screen_damage_rect(m, x, y, w, h);
                        arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                             claim_helper,
                                                             x, y, w, h);
                        arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                          claim_p,
                                                          claim_helper);
                    }
                }
            }
            ret = 0;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t123(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    ret = _DrawText(arm_str(m, r0), (int16)r1, (int16)r2,
                                    (uint8)r3, (uint8)arg_read(m, 4),
                                    (uint8)arg_read(m, 5),
                                    (int)arg_read(m, 6),
                                    (uint16)arg_read(m, 7));
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_diff(m, before);
                    arm_ext_claim_foreground_screen_diff(m, claim_p,
                                                         claim_helper,
                                                         before);
                    free(before);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                } else {
                    ret = 0;
                }
            }
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t124(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 {
            ArmExtScreenContext screen_ctx;
            if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                ret = _BitmapCheck(arm_ptr(m, r0), (int16)r1, (int16)r2,
                                   (uint16)r3, (uint16)arg_read(m, 4),
                                   (uint16)arg_read(m, 5),
                                   (uint16)arg_read(m, 6));
                arm_ext_pop_draw_screen_context(&screen_ctx);
            } else {
                ret = 0;
            }
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t126(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;
 ret = wstrlen(arm_str(m, r0)); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t127(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = mr_registerAPP(arm_ptr(m, r0), (int32)r1, (int32)r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t130(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 ret = _mr_TestCom(NULL, (int)r1, (int)r2); goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t131(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

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
            if (arm_ext_diag_on()) {
                uint32_t record_addr_diag = 0;
                uint32_t p_addr_diag = 0;
                if (r2 && arm_ptr(m, r2))
                    record_addr_diag = arm_ext_read_u32_or_zero_(m, r2);
                if (r2 && arm_ptr(m, r2 + 4u))
                    p_addr_diag = arm_ext_read_u32_or_zero_(m, r2 + 4u);
                printf("DIAG table131 cmd=%u data=0x%X len=%u ret=0x%X internalLoader=%d record=0x%X P=0x%X lastFile=0x%X/%u lastPack='%s' lr=0x%X activeP=0x%X primaryP=0x%X\n",
                       r1, r2, r3, ret, internal_loader_staging,
                       record_addr_diag, p_addr_diag, m->last_file_addr,
                       m->last_file_len, m->last_file_pack_host_path,
                       reg_read32(m->uc, UC_ARM_REG_LR), m->active_p_addr,
                       m->primary_p_addr);
            }
            if (r1 == 9 && internal_loader_staging &&
                m->last_file_copy && r2 && r3 <= m->last_file_len &&
                r3 > 8 && arm_ptr(m, r2 + 8)) {
                memcpy(arm_ptr(m, r2 + 8), m->last_file_copy + 8, r3 - 8);
                uint32_t record_addr = 0;
                uint32_t p_addr = arm_ext_read_u32_or_zero_(m, r2 + 4u);
                memcpy(&record_addr, arm_ptr(m, r2), 4);
                /*
                 * The private loader writes the runtime header before
                 * mr_cacheSync, but several wrappers leave the body in a
                 * transient scratch form.  Copy only the immutable payload tail
                 * from the host-decoded readFile result; keep file_base[0]/[4]
                 * as the wrapper's record/P metadata.
                 */
                arm_ext_repair_private_child_record_bridges(m, record_addr,
                                                            r2, r3);
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
                if (record_addr && arm_ptr(m, record_addr + 125 * 4)) {
                    memcpy(arm_ptr(m, record_addr + 125 * 4),
                           arm_ptr(m, EXT_TABLE_ADDR + 125 * 4), 4);
                }
                arm_ext_apply_short_pack_alias_for_private_child(m, r2, r3,
                                                                 p_addr);
                /*
                 * From this point on the executable owner is the runtime image
                 * at r2, not the raw buffer returned by _mr_readFile.  Keep
                 * last_file_copy as immutable package provenance, but make
                 * later generic nested-load ownership and P-header writes use
                 * the same file_base that the private loader will BLX into.
                 */
                m->last_file_addr = r2;
                m->last_file_len = r3;
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
                if (arm_ext_trace_on()) {
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
                arm_ext_drop_overlapping_stale_nested_modules(m, r2, r3);
                m->pending_internal_file_addr = r2;
                m->pending_internal_file_len = r3;
                arm_ext_sync_internal_nested_module(m, r2, r3);
                uc_err cerr = uc_ctl_remove_cache(m->uc, r2, r2 + r3);
                if (cerr != UC_ERR_OK && arm_ext_trace_on()) {
                    printf("arm_ext_executor: uc_ctl_remove_cache(0x%X, 0x%X) failed: %u\n",
                           r2, r2 + r3, cerr);
                }
            }
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t132(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
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
        } goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t133(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = ((int32)r1 == 0) ? 0 : (int32)r0 / (int32)r1; goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t134(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t ret = MR_SUCCESS;
 ret = ((int32)r1 == 0) ? 0 : (int32)r0 % (int32)r1; goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t145(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;

            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    mr_platDrawChar((uint16)r0, (int32)r1, (int32)r2,
                                    (uint32)r3);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_diff(m, before);
                    arm_ext_claim_foreground_screen_diff(m, claim_p,
                                                         claim_helper,
                                                         before);
                    free(before);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            goto aex_done;
aex_done:
    c->ret = ret;
}

static void aex_t000(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t ret = MR_SUCCESS;

            /* 原生 table[0] = mr_malloc(first-fit,mem.c)。先走 guest
             * 空闲链表使地址预测成立,池内无合适块再走 bump 空闲块
             * 复用/新块(见分配器头注释与 bump 回收器注释)。 */
            ret = arm_ext_app_mem_malloc(m, r0);
            m->last_alloc_addr = ret;
            m->last_alloc_len = r0;
            if (ret) note_origin_mem_alloc(m, r0);
            {
                uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
                uint32_t nested_file_addr = 0;
                uint32_t nested_file_len = 0;
                if (m->nested_loading &&
                    arm_ext_nested_exec_range_for_lr(m, lr, &nested_file_addr,
                                                     &nested_file_len) &&
                    r0 > 0x1000) {
                    if (!m->outer_r9) {
                        m->outer_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                    }
                    if (!m->nested_return_addr) {
                        uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
                        uint32_t saved_lr = 0;
                        if (arm_ptr(m, sp + 12)) {
                            uc_mem_read(m->uc, sp + 12, &saved_lr, 4);
                            /*
                             * The private loader temporarily switches R9 from
                             * wrapper RW to the child RW while child startup
                             * allocates its own data.  Restore the wrapper R9
                             * only at the real wrapper return site.  A saved LR
                             * inside the staged child is just an internal
                             * Thumb return and restoring there makes the child
                             * run with wrapper globals.
                             */
                            uint32_t ret_pc = saved_lr & ~1u;
                            if (ret_pc >= EXT_CODE_ADDR &&
                                ret_pc < EXT_CODE_ADDR + m->code_len) {
                                m->nested_return_addr = saved_lr;
                            }
                        }
                    }
                    reg_write32(m->uc, UC_ARM_REG_R9, ret + 4);
                    m->nested_loading = 0;
                    if (arm_ext_trace_on()) {
                        printf("arm_ext_executor: nested R9=0x%X after malloc 0x%X outer=0x%X return=0x%X\n",
                               ret + 4, r0, m->outer_r9, m->nested_return_addr);
                    }
                }
            }
            goto aex_done;
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

static void aex_t003(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* B2:两端都完整可映射才执行,guest 传坏指针时不解引用 NULL */
            void *cpy_dst = arm_ptr_span(m, r0, r2);
            const void *cpy_src = arm_ptr_span(m, r1, r2);
            if (r2 && cpy_dst && cpy_src)
                arm_ext_guest_memcpy(cpy_dst, cpy_src, r2);
        }
            /* memcpy 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t cpy_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= cpy_end &&
                        !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; goto aex_done;
        /* case 4-9(B2):指针非法时跳过宿主操作;dst 类调用返回值仍是 r0,
         * 比较类返回 0(ret 初值)。只消除宿主 NULL 解引用,不改合法路径 */
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

static void aex_t014(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* B2:区间可映射才清写 */
            void *set_dst = arm_ptr_span(m, r0, r2);
            if (r2 && set_dst) memset(set_dst, (int)r1, r2);
        }
            /* memset 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t set_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= set_end &&
                        !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; goto aex_done;
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

static uint32_t arm_ext_scrram_acquire(ArmExtModule *m, uint32_t want) {
    if (!m || !want || want > EXT_SCRRAM_SIZE) return 0;
    if (!m->scrram_mem) {
        uint8_t *mem = (uint8_t *)calloc(1, EXT_SCRRAM_SIZE);
        if (!mem) return 0;
        /* Native MR_MALLOC_SCRRAM allocates outside LG_mem.  Mapping the
         * platform scratch band lazily preserves that ownership boundary and
         * avoids reserving 16 MiB for applications that never request it. */
        uc_err err = uc_mem_map_ptr(m->uc, EXT_SCRRAM_ADDR,
                                    EXT_SCRRAM_SIZE,
                                    UC_PROT_READ | UC_PROT_WRITE, mem);
        if (err != UC_ERR_OK) {
            free(mem);
            return 0;
        }
        m->scrram_mem = mem;
    }
    if (!m->exram_addr || want > m->exram_len) {
        /* The platform keeps a stable allocation while callers probe larger
         * sizes.  Preserve the live prefix and initialize only newly exposed
         * bytes; clearing [0, want) would corrupt data retained by the guest. */
        memset(m->scrram_mem + m->exram_len, 0, want - m->exram_len);
        m->exram_addr = EXT_SCRRAM_ADDR;
        m->exram_len = want;
    }
    return m->exram_addr;
}

static int32_t arm_ext_scrram_release(ArmExtModule *m, uint32_t addr) {
    if (!m || !addr) return MR_SUCCESS; /* Native free(NULL) is a no-op. */
    if (!m->exram_addr || addr != m->exram_addr || !m->scrram_mem)
        return MR_FAILED;
    /* SCRRAM is data-only and cannot contain the executing table bridge, so
     * unmapping it here gives stale guest pointers the same invalid lifetime
     * as the native free instead of retaining a hidden mapped fallback. */
    if (uc_mem_unmap(m->uc, EXT_SCRRAM_ADDR, EXT_SCRRAM_SIZE) != UC_ERR_OK)
        return MR_FAILED;
    free(m->scrram_mem);
    m->scrram_mem = NULL;
    m->exram_addr = 0;
    m->exram_len = 0;
    return MR_SUCCESS;
}

static void aex_t038(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t r3 = c->r3;
    uint32_t ret = MR_SUCCESS;
 {
            int diag_enabled = arm_ext_diag_on();
            uint32_t diag_lr = 0;
            uint32_t diag_owner_p = 0, diag_owner_h = 0;
            uint32_t diag_owner_file = 0, diag_owner_len = 0;
            char diag_input_preview[192];
            diag_input_preview[0] = '\0';
            if (diag_enabled) {
                diag_lr = reg_read32(m->uc, UC_ARM_REG_LR);
                arm_ext_diag_owner_for_lr(m, &diag_owner_p, &diag_owner_h,
                                          &diag_owner_file, &diag_owner_len);
                arm_ext_diag_preview_bytes(arm_ptr(m, r1), r2,
                                           diag_input_preview,
                                           sizeof(diag_input_preview));
            }
            /* SCRRAM is a separate platform allocation, not an LG_mem/bump
             * block.  Sharing the 16 MiB main map made a 10 MiB request fit
             * with a 1 MiB app heap but fail after --memory 2M/4M moved the
             * bump top across the wrapper stack reservation. */
            if (r0 == 1014) {
                uint32_t outp = r3, outlenp = arg_read(m, 4);
                uint32_t want = (uint32_t)r2;
                uint32_t a = arm_ext_scrram_acquire(m, want);
                if (a) {
                    if (outp) uc_mem_write(m->uc, outp, &a, 4);
                    if (outlenp) uc_mem_write(m->uc, outlenp, &want, 4);
                }
                ret = a ? MR_SUCCESS : MR_FAILED;
                if (diag_enabled) {
                    printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x%X outLen=%u inputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                           (int32_t)r0, r1, r2, r3, arg_read(m, 4), ret,
                           a, want, diag_input_preview, diag_lr,
                           diag_owner_p, diag_owner_h, diag_owner_file,
                           diag_owner_len, m->active_p_addr,
                           m->primary_p_addr);
                }
                goto aex_done;
            }
            if (r0 == 1015) {
                /* Native MR_FREE_SCRRAM frees the pointer supplied in input;
                 * foreign and repeated non-NULL frees are explicit errors. */
                ret = arm_ext_scrram_release(m, r1);
                if (diag_enabled) {
                    printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x0 outLen=0 inputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                           (int32_t)r0, r1, r2, r3, arg_read(m, 4), ret,
                           diag_input_preview, diag_lr, diag_owner_p,
                           diag_owner_h, diag_owner_file, diag_owner_len,
                           m->active_p_addr, m->primary_p_addr);
                }
                goto aex_done;
            }
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
                void *old_arm_ptr = arm_output ? arm_ptr(m, arm_output) : NULL;
                if (host_output) {
                    /* mr_platEx can write into a caller-owned buffer without
                     * updating output_len (MR_UCS2GB is one documented ABI).
                     * Pointer identity proves that no host-to-guest copy is
                     * needed, so retain the original ARM address even when the
                     * reported length is zero. */
                    if (host_output != old_arm_ptr && host_output_len > 0) {
                        new_arm_output = arm_alloc(m, (uint32_t)host_output_len + 1);
                        if (new_arm_output) {
                            memcpy(arm_ptr(m, new_arm_output), host_output, (uint32_t)host_output_len);
                            ((uint8 *)arm_ptr(m, new_arm_output))[host_output_len] = '\0';
                        }
                    } else if (host_output != old_arm_ptr) {
                        new_arm_output = 0;
                    }
                } else {
                    new_arm_output = 0;
                }
                uc_mem_write(m->uc, outputp_addr, &new_arm_output, 4);
            }
            if (output_lenp_addr) {
                uc_mem_write(m->uc, output_lenp_addr, &host_output_len, 4);
            }
            if (diag_enabled) {
                uint32_t new_arm_output = 0;
                int32_t new_output_len = 0;
                char output_preview[192];
                output_preview[0] = '\0';
                if (outputp_addr)
                    uc_mem_read(m->uc, outputp_addr, &new_arm_output, 4);
                if (output_lenp_addr)
                    uc_mem_read(m->uc, output_lenp_addr, &new_output_len, 4);
                arm_ext_diag_preview_bytes(arm_ptr(m, new_arm_output),
                                           new_output_len > 0 ?
                                           (uint32_t)new_output_len : 0,
                                           output_preview,
                                           sizeof(output_preview));
                printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x%X outLen=%d inputPreview='%s' outputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r1, r2, outputp_addr,
                       output_lenp_addr, ret, new_arm_output,
                       new_output_len, diag_input_preview, output_preview,
                       diag_lr, diag_owner_p, diag_owner_h,
                       diag_owner_file, diag_owner_len, m->active_p_addr,
                       m->primary_p_addr);
            }
        } goto aex_done;
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

static void aex_t044(ArmExtModule *m, AexTableCtx *c) {
    (void)m;
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            /* B2:整个读缓冲区可映射才交给宿主写(对照 vfd 分支的 hp 判空,
             * 真实 mr_read 分支此前无保护) */
            void *hp = arm_ptr_span(m, r1, r2 ? r2 : 1u);
            MrpVirtualFd *vf44 = mrp_vfd_get(m, r0);
            if (vf44) {
                uint32_t avail = vf44->pos < vf44->data_len ? vf44->data_len - vf44->pos : 0;
                uint32_t n = r2 < avail ? r2 : avail;
                if (n && hp) memcpy(hp, vf44->data + vf44->pos, n);
                vf44->pos += n;
                ret = (int32_t)n;
            } else if (hp) {
                ret = mr_read((int32)r0, hp, r2);
            } else {
                ret = MR_FAILED;
            }
            if (arm_ext_diag_on()) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                uint32_t overlap_lo = 0, overlap_hi = 0;
                uint32_t preview_len = (int32_t)ret > 0 ? (uint32_t)ret : 0;
                arm_ext_diag_preview_bytes(hp, preview_len, preview,
                                           sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                arm_ext_find_first_registered_code_overlap(
                    m, r1, preview_len, &overlap_lo, &overlap_hi);
                printf("DIAG table44 fd=%d name='%s' want=%u ret=0x%X dst=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X codeOverlap=0x%X..0x%X\n",
                       (int32_t)r0, arm_ext_diag_fd_name(m, (int32_t)r0),
                       r2, ret, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr,
                       overlap_lo, overlap_hi);
            }
            if (m->profile && m->profile->post_read_hook)
                m->profile->post_read_hook(m, m->app_state, r1, r2, ret, hp);
            if ((int32_t)ret > 0)
                arm_ext_retire_modules_overwritten_by_data_read(
                    m, r1, (uint32_t)ret);
            /* dump/restore 读回整块模块内存后，需要：
             * 1) 修复 GOT 中的 bridge 函数指针（文件数据中是原始未重定位的值）
             * 2) invalidate Unicorn TB cache（否则执行旧翻译） */
            if ((int32_t)ret > 0 && (uint32_t)ret > 0x1000) {
                /* 仅在 dump0 恢复时（目标地址匹配）才恢复间隙数据 */
                int read_covers_primary =
                    m->primary_file_addr && m->primary_file_len &&
                    arm_ext_range_contains(r1, (uint32_t)ret,
                                           m->primary_file_addr,
                                           m->primary_file_len);
                uc_ctl_remove_cache(m->uc, r1, r1 + (uint32_t)ret);
                arm_ext_restore_primary_mapping_after_dump0(m, r1, (uint32_t)ret);
                /*
                 * table[44] also reads downloaded packages and RAM staging
                 * buffers.  Only rewrite bridge GOT slots when this read is
                 * proven to be an executable dump/restore of the primary
                 * image; otherwise compressed payload bytes can be corrupted.
                 */
                if (read_covers_primary && m->got_snapshot_base) {
                    uint32_t got_base = m->got_snapshot_base;
                    uint32_t read_end = r1 + (uint32_t)ret;
                    for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                        uint32_t addr = got_base + i * 4;
                        if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                            m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                            addr >= r1 && addr + 4 <= read_end &&
                            !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                            !app_should_protect_got_addr(m, addr)) {
                            memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                        }
                    }
                }
            }
        } goto aex_done;
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

static void aex_t125(ArmExtModule *m, AexTableCtx *c) {
    uint32_t r0 = c->r0;
    uint32_t r1 = c->r1;
    uint32_t r2 = c->r2;
    uint32_t ret = MR_SUCCESS;
 {
            int fl = 0;
            uint32_t packp = 0, ramp_slot = 0, raml_slot = 0, ramp = 0, raml = 0;
            uint32_t read_pack_ram_addr = 0, read_pack_ram_len = 0;
            char read_pack_host_path[PATH_MAX];
            read_pack_host_path[0] = '\0';
            const char *read_name = arm_str(m, r0);
            ArmExtNestedModule *owner =
                arm_ext_resource_owner_for_lr(m, NULL, NULL);
            uint32_t table_addr = arm_ext_read_file_table_context(m, owner);
            if (!table_addr) goto aex_done;
            /*
             * A private child invokes this host bridge through its own module
             * record.  Its loader can replace package/RAM slots independently
             * of the primary EXT table, so read all three from the LR-selected
             * ABI context.
             */
            packp = arm_ext_read_u32_or_zero_(m, table_addr + 100u * 4u);
            ramp_slot = arm_ext_read_u32_or_zero_(m, table_addr + 104u * 4u);
            raml_slot = arm_ext_read_u32_or_zero_(m, table_addr + 105u * 4u);
            if (ramp_slot) uc_mem_read(m->uc, ramp_slot, &ramp, 4);
            if (raml_slot) uc_mem_read(m->uc, raml_slot, &raml, 4);

            void *hp = NULL;
            const char *pack = arm_str(m, packp);
            const char *host_pack = arm_ext_pack_to_host_path(m, pack);
            uint32_t owner_p_diag = owner ? owner->p_addr : 0;
            uint32_t owner_h_diag = owner ? owner->helper_addr : 0;
            int pack_is_root =
                pack[0] && host_pack && m->pack_host_path[0] &&
                strcmp(host_pack, m->pack_host_path) == 0;
            int child_scoped_pack =
                arm_ext_child_has_package_owner(m, owner) &&
                (!pack[0] || pack_is_root);
            if (child_scoped_pack) {
                hp = arm_ext_read_child_resource(
                    m, owner, read_name, &fl, (int)r2,
                    read_pack_host_path, &read_pack_ram_addr,
                    &read_pack_ram_len);
            }
            if (!hp && !child_scoped_pack && pack[0] == '$' && ramp && raml) {
                hp = mr_readFile_from_ram(read_name, &fl, (int)r2, arm_ptr(m, ramp), (int)raml);
                if (hp) {
                    read_pack_ram_addr = ramp;
                    read_pack_ram_len = raml;
                    const char *last_mrp_path = mr_get_last_written_mrp_path();
                    uint32 last_mrp_len = 0;
                    const uint8 *last_mrp =
                        mr_get_last_written_mrp_data(&last_mrp_len);
                    if (last_mrp && last_mrp_len == raml &&
                        last_mrp_path && last_mrp_path[0] &&
                        memcmp(arm_ptr(m, ramp), last_mrp, raml) == 0) {
                        snprintf(read_pack_host_path,
                                 sizeof(read_pack_host_path), "%s",
                                 last_mrp_path);
                    }
                }
            } else if (!hp && !child_scoped_pack && !pack[0]) {
                /*
                 * Native pack_filename[128] is writable, and private child
                 * helpers may clear it after their loader has installed the
                 * child.  Once the LR maps to a registered child module, that
                 * module's package is the ABI context for table[125]; otherwise
                 * a blank table[100] means the VM-global package.
                 */
                hp = _mr_readFile(read_name, &fl, (int)r2);
            } else if (!hp && !child_scoped_pack) {
                /*
                 * EXT modules expose their current MRP through table[100].
                 * Nested download/install helpers may switch that slot to a
                 * transient package while the VM-level pack_filename still
                 * names the outer app. Bind the host read to the ARM-visible
                 * package for this bridge call, matching the native C table ABI.
                 */
                hp = mr_readFile_from_pack(host_pack, read_name, &fl, (int)r2);
                if (hp && host_pack && host_pack[0]) {
                    snprintf(read_pack_host_path, sizeof(read_pack_host_path),
                             "%s", host_pack);
                }
            }
            if (arm_ext_diag_on()) {
                char ram_preview[192];
                char out_preview[192];
                ram_preview[0] = '\0';
                out_preview[0] = '\0';
                if (ramp && raml && arm_ptr(m, ramp)) {
                    arm_ext_diag_preview_bytes(arm_ptr(m, ramp), raml,
                                               ram_preview,
                                               sizeof(ram_preview));
                }
                if (hp && fl > 0) {
                    arm_ext_diag_preview_bytes(hp, (uint32_t)fl,
                                               out_preview,
                                               sizeof(out_preview));
                }
                printf("DIAG table125 name='%s' lookfor=%u pack='%s' host_pack='%s' childScoped=%d ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u ramp=0x%X raml=%u found=%d fl=%d readPack='%s' readRam=0x%X/%u lr=0x%X\n",
                       read_name, r2, pack, host_pack ? host_pack : "",
                       child_scoped_pack, owner_p_diag, owner_h_diag,
                       owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, ramp, raml,
                       hp != NULL, fl, read_pack_host_path,
                       read_pack_ram_addr, read_pack_ram_len,
                       reg_read32(m->uc, UC_ARM_REG_LR));
            }
            if (!hp) {
                ret = 0;
                goto aex_done;
            }
            /* 原生 asm_mr_readFile 的输出缓冲来自 mr_malloc(LG_mem
             * first-fit),应用会预先 free 一块自有内存构造空闲链表,让
             * 该分配按 first-fit 落在预测地址,并丢弃返回值直接读预测
             * 地址(talkcat 图标花屏根因)。先走 guest 空闲链表;池内
             * 无合适块时与 table[0] 一样退回 bump(修复前行为)。
             * 若该缓冲随后经 mr_cacheSync 注册为 EXT 映像,注册路径会把
             * 文件范围标记为不可再分配的代码区。 */
            uint32_t ap = 0;
            int direct_ram_package_ptr = 0;
            uint32_t mapped_hp = fl > 0 ? arm_addr(m, hp) : 0;
            if (mapped_hp && read_pack_ram_addr && read_pack_ram_len &&
                arm_ext_range_contains(read_pack_ram_addr, read_pack_ram_len,
                                       mapped_hp, (uint32_t)fl)) {
                /*
                 * Native _mr_readFile returns uncompressed '*'/'$' package
                 * entries as pointers inside the package buffer.  Do the same
                 * when the host result already maps to ARM-visible package
                 * RAM; copying it to a new mr_malloc block gives the caller a
                 * shorter lifetime than native and lets later readFile calls
                 * recycle storage still referenced by cached bitmap records.
                 */
                ap = mapped_hp;
                direct_ram_package_ptr = 1;
            } else {
                ap = arm_ext_app_mem_malloc(m, (uint32_t)fl);
                if (!ap) {
                    ret = 0;
                    goto aex_done;
                }
                /*
                 * Compressed package entries and filesystem reads return
                 * native mr_malloc-owned buffers (mr_gzOutBuf/filebuf).  The
                 * bridge allocation is therefore app-visible LG_mem and must
                 * update the same counters as table[0].
                 */
                note_origin_mem_alloc(m, (uint32_t)fl);
                memcpy(arm_ptr(m, ap), hp, fl);
            }
            if (r1 && arm_ptr(m, r1)) {
                memcpy(arm_ptr(m, r1), &fl, 4);
            }
            arm_ext_sync_read_file_gzip_slots(m, hp, ap);
            free(m->last_file_copy);
            m->last_file_copy = malloc((size_t)fl);
            if (m->last_file_copy) {
                memcpy(m->last_file_copy, hp, (size_t)fl);
                m->last_file_addr = ap;
                m->last_file_len = (uint32_t)fl;
                snprintf(m->last_file_pack_host_path,
                         sizeof(m->last_file_pack_host_path), "%s",
                         read_pack_host_path);
                m->last_file_pack_ram_addr = read_pack_ram_addr;
                m->last_file_pack_ram_len = read_pack_ram_len;
            }
            uint32_t mirrored_slot = 0;
            int mirrored_to_adjacent_slot =
                m->last_file_copy &&
                arm_ext_mirror_read_file_to_adjacent_slot(
                    m, r1, m->last_file_copy, (uint32_t)fl, ap,
                    &mirrored_slot);
            if (arm_ext_diag_on()) {
                char ret_preview[192];
                ret_preview[0] = '\0';
                arm_ext_diag_preview_bytes(arm_ptr(m, ap), (uint32_t)fl,
                                           ret_preview,
                                           sizeof(ret_preview));
                printf("DIAG table125_ret name='%s' ret=0x%X fl=%d lenSlot=0x%X/%u mirror=%d mirrorSlot=0x%X lr=0x%X preview='%s'\n",
                       read_name, ap, fl, r1,
                       r1 ? arm_ext_read_u32_or_zero_(m, r1) : 0,
                       mirrored_to_adjacent_slot,
                       mirrored_slot, reg_read32(m->uc, UC_ARM_REG_LR),
                       ret_preview);
            }
            if (!direct_ram_package_ptr)
                arm_ext_release_host_read_file_buffer(hp, (uint32_t)fl);
            ret = ap;
        } goto aex_done;
    goto aex_done; /* 复杂 case:循环内 break 原样保留,此处显式收敛 */
aex_done:
    c->ret = ret;
}

const AexTableHandler aex_table_handlers[EXT_TABLE_COUNT] = {
    [125] = aex_t125,
    [44] = aex_t044,
    [38] = aex_t038,
    [14] = aex_t014,
    [3] = aex_t003,
    [0] = aex_t000,
    [1] = aex_t001,
    [2] = aex_t002,
    [4] = aex_t004,
    [5] = aex_t005,
    [6] = aex_t006,
    [7] = aex_t007,
    [8] = aex_t008,
    [9] = aex_t009,
    [10] = aex_t010,
    [11] = aex_t011,
    [12] = aex_t012,
    [13] = aex_t013,
    [15] = aex_t015,
    [16] = aex_t016,
    [17] = aex_t017,
    [18] = aex_t018,
    [19] = aex_t019,
    [20] = aex_t020,
    [22] = aex_t022,
    [25] = aex_t025,
    [26] = aex_t026,
    [27] = aex_t027,
    [28] = aex_t028,
    [29] = aex_t029,
    [30] = aex_t030,
    [31] = aex_t031,
    [32] = aex_t032,
    [33] = aex_t033,
    [34] = aex_t034,
    [35] = aex_t035,
    [36] = aex_t036,
    [37] = aex_t037,
    [39] = aex_t039,
    [40] = aex_t040,
    [41] = aex_t041,
    [42] = aex_t042,
    [43] = aex_t043,
    [45] = aex_t045,
    [46] = aex_t046,
    [47] = aex_t047,
    [48] = aex_t048,
    [49] = aex_t049,
    [50] = aex_t050,
    [51] = aex_t051,
    [52] = aex_t052,
    [53] = aex_t053,
    [54] = aex_t054,
    [55] = aex_t055,
    [56] = aex_t056,
    [57] = aex_t057,
    [58] = aex_t058,
    [59] = aex_t059,
    [60] = aex_t060,
    [61] = aex_t061,
    [69] = aex_t069,
    [70] = aex_t070,
    [71] = aex_t071,
    [72] = aex_t072,
    [73] = aex_t073,
    [74] = aex_t074,
    [75] = aex_t075,
    [76] = aex_t076,
    [77] = aex_t077,
    [78] = aex_t078,
    [79] = aex_t079,
    [80] = aex_t080,
    [81] = aex_t081,
    [82] = aex_t082,
    [83] = aex_t083,
    [84] = aex_t084,
    [85] = aex_t085,
    [86] = aex_t086,
    [87] = aex_t087,
    [88] = aex_t088,
    [89] = aex_t089,
    [90] = aex_t090,
    [113] = aex_t113,
    [114] = aex_t114,
    [115] = aex_t115,
    [118] = aex_t118,
    [119] = aex_t119,
    [120] = aex_t120,
    [122] = aex_t122,
    [123] = aex_t123,
    [124] = aex_t124,
    [126] = aex_t126,
    [127] = aex_t127,
    [130] = aex_t130,
    [131] = aex_t131,
    [132] = aex_t132,
    [133] = aex_t133,
    [134] = aex_t134,
    [145] = aex_t145,
};
