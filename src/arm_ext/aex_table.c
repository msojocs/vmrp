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
#include <unistd.h>

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
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    arm_ext_draw_bitmap_from_guest(
                        m, r0, (int16)r1, (int16)r2,
                        (uint16)r3, h, rop, trans, sx, sy, mw);
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

const AexTableHandler aex_table_handlers[EXT_TABLE_COUNT] = {
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
