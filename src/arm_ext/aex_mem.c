/*
 * aex_mem.c —— guest 内存分配器与 compact 堆保护(Phase 2 拆分第 3 步)。
 *
 * 三层分配语义(LG_mem 池 first-fit / bump 后备 / wrapper compact 堆的
 * free-list 保护)与运行时不变量检查器。函数体自 arm_ext_executor.c 原样
 * 迁移;跨单元符号见 arm_ext_priv.h。分配器语义背景:
 * docs/arm-ext-abi.md 第 7 节、memory/guest-lg-mem-first-fit。
 */
#include "../include/arm_ext_priv.h"

#define ARM_EXT_LG_MEM_ALIGN(x) (((x) + 7u) & ~7u)

static uint32_t arm_ext_guest_mem_read_u32(ArmExtModule *m, uint32_t addr) {
    uint32_t v = 0;
    void *p = arm_ptr(m, addr);
    if (p) memcpy(&v, p, 4);
    return v;
}

static void arm_ext_guest_mem_write_u32(ArmExtModule *m,
                                        uint32_t addr,
                                        uint32_t v) {
    void *p = arm_ptr(m, addr);
    if (p) memcpy(p, &v, 4);
}

typedef struct ArmExtOriginArena {
    uint32_t base;
    uint32_t end;
    uint32_t head;
} ArmExtOriginArena;

typedef struct ArmExtMappedBand {
    uint32_t addr;
    uint32_t len;
} ArmExtMappedBand;

static int arm_ext_current_origin_arena(ArmExtModule *m,
                                        ArmExtOriginArena *arena) {
    if (arena) memset(arena, 0, sizeof(*arena));
    if (!m || !arena) return 0;

    uint32_t base_slot = arm_ext_guest_mem_read_u32(
        m, EXT_TABLE_ADDR + 108u * 4u);
    uint32_t end_slot = arm_ext_guest_mem_read_u32(
        m, EXT_TABLE_ADDR + 110u * 4u);
    uint32_t head = arm_ext_guest_mem_read_u32(
        m, EXT_TABLE_ADDR + 146u * 4u);
    uint32_t base = arm_ext_guest_mem_read_u32(m, base_slot);
    uint32_t end = arm_ext_guest_mem_read_u32(m, end_slot);

    /*
     * Legacy wrappers temporarily append a separately mapped arena by
     * changing LG_mem_end while keeping offset links relative to LG_mem_base.
     * Validate the endpoints and every node as it is visited; the numeric
     * interval itself may contain an unmapped gap and therefore must not be
     * required to form one host-contiguous span.
     */
    if (!base || end <= base || !head ||
        !arm_ptr_span(m, base, 8u) ||
        !arm_ptr_span(m, end - 1u, 1u) ||
        !arm_ptr_span(m, head, 8u)) {
        return 0;
    }
    arena->base = base;
    arena->end = end;
    arena->head = head;
    return 1;
}

static uint32_t arm_ext_origin_node_limit(ArmExtModule *m,
                                          const ArmExtOriginArena *arena) {
    static const ArmExtMappedBand bands[] = {
        {EXT_BASE_ADDR, EXT_MEM_SIZE},
        {EXT_PLATFORM_MEM_ADDR, EXT_PLATFORM_MEM_SIZE},
        {EXT_EXECUTOR_META_ADDR, EXT_EXECUTOR_META_SIZE},
        {EXT_PLATFORM_IO_MEM_ADDR, EXT_PLATFORM_IO_MEM_SIZE},
        {EXT_PLATFORM_ALT_MEM_ADDR, EXT_PLATFORM_ALT_MEM_SIZE},
    };
    uint64_t mapped_bytes = 0;
    for (size_t i = 0; i < sizeof(bands) / sizeof(bands[0]); ++i) {
        if (!arm_ptr(m, bands[i].addr)) continue;
        uint64_t lo = bands[i].addr;
        uint64_t band_end = (uint64_t)bands[i].addr + bands[i].len;
        uint64_t hi = arena->end < band_end ? arena->end : band_end;
        if (lo < hi) mapped_bytes += hi - lo;
    }
    /* A wrapped link may decode below base, so the structural domain is every
     * mapped byte below end.  Each valid free-list node occupies at least eight. */
    return (uint32_t)(mapped_bytes / 8u) + 2u;
}

static int arm_ext_origin_node_read(ArmExtModule *m,
                                    const ArmExtOriginArena *arena,
                                    uint32_t node,
                                    uint32_t *next,
                                    uint32_t *len) {
    if (next) *next = arena->end;
    if (len) *len = 0;
    if (!arm_ptr_span(m, node, 8u)) return 0;

    uint32_t next_off = arm_ext_guest_mem_read_u32(m, node);
    /* mem.c decodes links with `LG_mem_base + previous->next` on a 32-bit
     * target.  The subtraction used to create an offset may therefore wrap:
     * base 0x225E5C + offset 0xFFFDA360 becomes address 0x2001BC.  Validate
     * the decoded guest address, not the unsigned offset's magnitude. */
    uint32_t next_addr = arena->base + next_off;
    if (next_addr > arena->end) return 0;
    if (next) *next = next_addr;

    if (node == arena->head) return 1;
    uint32_t node_len = arm_ext_guest_mem_read_u32(m, node + 4u);
    if (!node_len || node >= arena->end ||
        node_len > arena->end - node ||
        !arm_ptr_span(m, node, node_len)) {
        return 0;
    }
    if (len) *len = node_len;
    return 1;
}

static uint32_t arm_ext_origin_stat_read(ArmExtModule *m,
                                         uint32_t slot,
                                         uint32_t fallback) {
    return slot && arm_ptr_span(m, slot, 4u)
               ? arm_ext_guest_mem_read_u32(m, slot)
               : fallback;
}

static void arm_ext_origin_stat_write(ArmExtModule *m,
                                      uint32_t slot,
                                      uint32_t value) {
    if (slot && arm_ptr_span(m, slot, 4u))
        arm_ext_guest_mem_write_u32(m, slot, value);
}

void note_origin_mem_alloc(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    len = align4(len ? len : 1);
    uint32_t left = arm_ext_origin_stat_read(
        m, m->origin_mem_left_slot, m->origin_mem_left);
    uint32_t min = arm_ext_origin_stat_read(
        m, m->origin_mem_min_slot, m->origin_mem_min);
    uint32_t top = arm_ext_origin_stat_read(
        m, m->origin_mem_top_slot, m->origin_mem_top);
    left = len < left ? left - len : 0;
    if (left < min) {
        min = left;
        /* A wrapper that attached an external arena owns LG_mem_top while its
         * dynamic end is published.  The fixed host pool length cannot derive
         * a meaningful cross-band offset and must not overwrite that value. */
        ArmExtOriginArena arena;
        if (!arm_ext_current_origin_arena(m, &arena) ||
            (arena.base == m->origin_mem_addr &&
             arena.end - arena.base == m->origin_mem_len)) {
            top = m->origin_mem_len - min;
        }
    }
    m->origin_mem_left = left;
    m->origin_mem_min = min;
    m->origin_mem_top = top;
    arm_ext_origin_stat_write(m, m->origin_mem_left_slot, left);
    arm_ext_origin_stat_write(m, m->origin_mem_min_slot, min);
    arm_ext_origin_stat_write(m, m->origin_mem_top_slot, top);
}

void note_origin_mem_free(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    uint32_t left = arm_ext_origin_stat_read(
        m, m->origin_mem_left_slot, m->origin_mem_left);
    uint32_t add = align4(len);
    left = UINT32_MAX - left < add ? UINT32_MAX : left + add;
    m->origin_mem_left = left;
    arm_ext_origin_stat_write(m, m->origin_mem_left_slot, left);
}

static uint32_t arm_ext_guest_mem_malloc(ArmExtModule *m, uint32_t len) {
    ArmExtOriginArena arena;
    if (!arm_ext_current_origin_arena(m, &arena)) return 0;
    uint32_t base = arena.base;
    uint32_t end = arena.end;
    uint32_t head = arena.head;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len) return 0; /* mem.c: invalid memory request */
    uint32_t nextfree = end;
    if (!arm_ext_origin_node_read(m, &arena, head, &nextfree, NULL)) {
        return 0; /* mem.c: corrupted memory */
    }
    /* previous 为头节点或空闲节点地址,两者均为 {next@+0, len@+4} 布局 */
    uint32_t previous = head;
    /* The cap is the structural maximum number of eight-byte nodes in all
     * mapped bands addressable below the current end, including wrapped links. */
    uint32_t iter_limit = arm_ext_origin_node_limit(m, &arena);
    while (nextfree < end) {
        if (!iter_limit--) return 0; /* mem.c: cyclic free list */
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, nextfree, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return 0; /* protected executable bytes cannot be free-list metadata */
        }
        uint32_t node_next = end;
        uint32_t node_len = 0;
        if (!arm_ext_origin_node_read(m, &arena, nextfree, &node_next,
                                      &node_len)) {
            return 0; /* mem.c: corrupted memory */
        }
        uint32_t alloc_addr = 0;
        if (!arm_ext_first_unprotected_subrange(m, nextfree, node_len, len,
                                                &alloc_addr)) {
            previous = nextfree;
            nextfree = node_next;
            continue;
        }
        if (alloc_addr == nextfree && node_len == len) {
            arm_ext_guest_mem_write_u32(m, previous, node_next - base);
            return nextfree;
        }
        if (alloc_addr == nextfree) {
            /* 拆分:剩余部分在 nextfree+len 处形成新空闲节点 */
            uint32_t node_end = nextfree + node_len;
            uint32_t l = nextfree + len;
            uint32_t right_len = node_end - l;
            /* split_lo/hi:与函数开头的 protected_lo/hi 区分,避免遮蔽 */
            uint32_t split_lo = 0;
            uint32_t split_hi = 0;
            if (right_len &&
                arm_ext_find_first_registered_code_overlap(
                    m, l, right_len, &split_lo, &split_hi) &&
                split_lo < l + 8u) {
                l = split_hi;
                right_len = l < node_end ? node_end - l : 0;
            }
            if (right_len) {
                arm_ext_guest_mem_write_u32(m, l, node_next - base);
                arm_ext_guest_mem_write_u32(m, l + 4, right_len);
                arm_ext_guest_mem_write_u32(m, previous, l - base);
            } else {
                arm_ext_guest_mem_write_u32(m, previous, node_next - base);
            }
            return nextfree;
        }
        {
            uint32_t node_end = nextfree + node_len;
            uint32_t alloc_end = alloc_addr + len;
            uint32_t left_len = alloc_addr - nextfree;
            uint32_t right_len = node_end - alloc_end;
            arm_ext_guest_mem_write_u32(m, nextfree + 4, left_len);
            if (right_len) {
                /* tail_lo/hi:与函数开头的 protected_lo/hi 区分,避免遮蔽 */
                uint32_t tail_lo = 0;
                uint32_t tail_hi = 0;
                if (arm_ext_find_first_registered_code_overlap(
                        m, alloc_end, right_len, &tail_lo,
                        &tail_hi) &&
                    tail_lo < alloc_end + 8u) {
                    alloc_end = tail_hi;
                    right_len = alloc_end < node_end ? node_end - alloc_end : 0;
                }
            }
            if (right_len) {
                arm_ext_guest_mem_write_u32(m, alloc_end, node_next - base);
                arm_ext_guest_mem_write_u32(m, alloc_end + 4, right_len);
                arm_ext_guest_mem_write_u32(m, nextfree, alloc_end - base);
            } else {
                arm_ext_guest_mem_write_u32(m, nextfree, node_next - base);
            }
            return alloc_addr;
        }
        /* D1:此处不再有链表推进语句——上方分支要么 return,要么在"无合适
         * 子区间"时已 continue,循环尾推进自始不可达 */
    }
    return 0; /* mem.c: no memory2(池内无足够大的块,调用方退回 bump) */
}

static void arm_ext_guest_mem_free_unprotected(ArmExtModule *m, uint32_t p,
                                               uint32_t len) {
    ArmExtOriginArena arena;
    if (!arm_ext_current_origin_arena(m, &arena)) return;
    uint32_t base = arena.base;
    uint32_t end = arena.end;
    uint32_t head = arena.head;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len || !p || p < base || p >= end || p + len > end ||
        p + len <= base || !arm_ptr_span(m, p, len)) {
        return; /* mem.c: mr_free invalid */
    }
    /* 按地址序找到插入位置:freep < p <= n */
    uint32_t freep = head;
    uint32_t n = end;
    if (!arm_ext_origin_node_read(m, &arena, freep, &n, NULL)) return;
    uint32_t iter_limit = arm_ext_origin_node_limit(m, &arena);
    while (n < end && n < p) {
        if (!iter_limit--) return; /* mem.c: cyclic free list */
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, n, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return; /* existing list node overlaps executable storage */
        }
        uint32_t next = end;
        if (!arm_ext_origin_node_read(m, &arena, n, &next, NULL))
            return;
        freep = n;
        n = next;
    }
    if (n < end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, n, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return;
        }
        if (!arm_ext_origin_node_read(m, &arena, n, NULL, NULL)) return;
    }
    if (p == freep || p == n) return; /* mem.c: already free */
    if (freep != head &&
        freep + arm_ext_guest_mem_read_u32(m, freep + 4) == p) {
        /* 与前一空闲块连续:并入 */
        arm_ext_guest_mem_write_u32(
            m, freep + 4,
            arm_ext_guest_mem_read_u32(m, freep + 4) + len);
    } else {
        /* 插入新空闲节点 */
        arm_ext_guest_mem_write_u32(m, freep, p - base);
        arm_ext_guest_mem_write_u32(m, p, n - base);
        arm_ext_guest_mem_write_u32(m, p + 4, len);
        freep = p;
    }
    if (n < end && p + len == n) {
        /* 与后一空闲块连续:并入 */
        arm_ext_guest_mem_write_u32(
            m, freep, arm_ext_guest_mem_read_u32(m, n));
        arm_ext_guest_mem_write_u32(
            m, freep + 4,
            arm_ext_guest_mem_read_u32(m, freep + 4) +
                arm_ext_guest_mem_read_u32(m, n + 4));
    }
    /* 统计由调用方的 note_origin_mem_free 记账(见分配器头注释) */
}

static void arm_ext_guest_mem_free(ArmExtModule *m, uint32_t p, uint32_t len) {
    ArmExtOriginArena arena;
    if (!arm_ext_current_origin_arena(m, &arena)) return;
    uint32_t base = arena.base;
    uint32_t end = arena.end;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len || !p || p < base || p >= end || p + len > end ||
        p + len <= base || !arm_ptr_span(m, p, len)) {
        return; /* mem.c: mr_free invalid */
    }

    /*
     * Do not write LG_mem free-list nodes over registered executable images.
     * Some wrappers expose subranges of a large app arena through table[146];
     * once such a subrange becomes an EXT image, freeing the containing arena
     * must return only the non-code fragments to app allocation.
     */
    uint32_t cursor = p;
    uint32_t range_end = p + len;
    while (cursor < range_end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        uint32_t remaining = range_end - cursor;
        if (!arm_ext_find_first_registered_code_overlap(m, cursor, remaining,
                                                        &protected_lo,
                                                        &protected_hi)) {
            if (remaining >= 8u)
                arm_ext_guest_mem_free_unprotected(m, cursor, remaining);
            break;
        }
        if (cursor < protected_lo && protected_lo - cursor >= 8u) {
            arm_ext_guest_mem_free_unprotected(m, cursor,
                                               protected_lo - cursor);
        }
        if (protected_hi <= cursor) break;
        cursor = protected_hi;
    }
}

/*
 * bump 后备块回收器(宿主侧,见 arm_ext_internal.h 中 bump_live 注释)。
 * 原生设备 LG_mem 仅 512KB,应用长流程完全依赖 free 后复用;此前
 * bump 后备块的 free 是空操作,talkcat 喝水资源包安装的反复大块
 * readFile 使 bump 只增不减耗尽 16MB 并撞入 EXT 栈区。live 表登记
 * 应用可见的 bump 块;free 只回收登记过的地址(长度取登记值),
 * guest 无法借伪造 free 回收执行器内部分配。空闲表按地址有序并
 * 合并相邻块,复用时 first-fit 拆分。
 * 验证:talkcat 喝水资源包下载安装 e2e 通过,全量 e2e 回归。
 */

static int arm_ext_bump_block_append(ArmExtBumpBlock **arr,
                                     uint32_t *count,
                                     uint32_t *cap,
                                     uint32_t idx,
                                     uint32_t addr,
                                     uint32_t len) {
    if (*count == *cap) {
        uint32_t new_cap = *cap ? *cap * 2 : 32;
        ArmExtBumpBlock *p =
            (ArmExtBumpBlock *)realloc(*arr, new_cap * sizeof(**arr));
        if (!p) return 0;
        *arr = p;
        *cap = new_cap;
    }
    memmove(*arr + idx + 1, *arr + idx, (*count - idx) * sizeof(**arr));
    (*arr)[idx].addr = addr;
    (*arr)[idx].len = len;
    (*count)++;
    return 1;
}

static void arm_ext_bump_block_remove(ArmExtBumpBlock *arr,
                                      uint32_t *count,
                                      uint32_t idx) {
    memmove(arr + idx, arr + idx + 1, (*count - idx - 1) * sizeof(*arr));
    (*count)--;
}

static void arm_ext_app_alloc_track(ArmExtModule *m,
                                    uint32_t addr,
                                    uint32_t len) {
    if (!m || !addr || !len) return;
    if (!arm_ext_bump_block_append(&m->app_live_blocks,
                                   &m->app_live_count,
                                   &m->app_live_cap,
                                   m->app_live_count,
                                   addr, align4(len))) {
        return;
    }
}

static int arm_ext_app_alloc_untrack(ArmExtModule *m, uint32_t addr) {
    if (!m || !addr) return 0;
    for (uint32_t i = 0; i < m->app_live_count; ++i) {
        if (m->app_live_blocks[i].addr == addr) {
            arm_ext_bump_block_remove(m->app_live_blocks,
                                      &m->app_live_count, i);
            return 1;
        }
    }
    return 0;
}

static int arm_ext_bump_free_insert(ArmExtModule *m, uint32_t addr,
                                    uint32_t len) {
    if (!m || !addr || !len) return 0;
    uint32_t i = 0;
    while (i < m->bump_free_count && m->bump_free_blocks[i].addr < addr) i++;
    if (i > 0 && m->bump_free_blocks[i - 1].addr +
                     m->bump_free_blocks[i - 1].len == addr) {
        m->bump_free_blocks[i - 1].len += len;
        if (i < m->bump_free_count &&
            m->bump_free_blocks[i - 1].addr + m->bump_free_blocks[i - 1].len ==
                m->bump_free_blocks[i].addr) {
            m->bump_free_blocks[i - 1].len += m->bump_free_blocks[i].len;
            arm_ext_bump_block_remove(m->bump_free_blocks,
                                      &m->bump_free_count, i);
        }
        return 1;
    }
    if (i < m->bump_free_count &&
        addr + len == m->bump_free_blocks[i].addr) {
        m->bump_free_blocks[i].addr = addr;
        m->bump_free_blocks[i].len += len;
        return 1;
    }
    return arm_ext_bump_block_append(&m->bump_free_blocks,
                                     &m->bump_free_count,
                                     &m->bump_free_cap, i, addr, len);
}

static void arm_ext_bump_block_remove_range(ArmExtBumpBlock **arr,
                                            uint32_t *count,
                                            uint32_t *cap,
                                            uint32_t range_addr,
                                            uint32_t range_len) {
    if (!arr || !*arr || !count || !range_addr || !range_len) return;
    uint64_t range_start = range_addr;
    uint64_t range_end = range_start + range_len;
    for (uint32_t i = 0; i < *count;) {
        ArmExtBumpBlock *b = &(*arr)[i];
        uint64_t block_start = b->addr;
        uint64_t block_end = block_start + b->len;
        uint64_t lo = block_start > range_start ? block_start : range_start;
        uint64_t hi = block_end < range_end ? block_end : range_end;
        if (lo >= hi) {
            i++;
            continue;
        }
        if (lo <= block_start && hi >= block_end) {
            arm_ext_bump_block_remove(*arr, count, i);
            continue;
        }
        if (lo <= block_start) {
            b->addr = (uint32_t)hi;
            b->len = (uint32_t)(block_end - hi);
            i++;
            continue;
        }
        if (hi >= block_end) {
            b->len = (uint32_t)(lo - block_start);
            i++;
            continue;
        }
        uint32_t right_addr = (uint32_t)hi;
        uint32_t right_len = (uint32_t)(block_end - hi);
        b->len = (uint32_t)(lo - block_start);
        if (cap) {
            arm_ext_bump_block_append(arr, count, cap, i + 1,
                                      right_addr, right_len);
        }
        i += 2;
    }
}

static int arm_ext_bump_track(ArmExtModule *m, uint32_t addr, uint32_t len) {
    return arm_ext_bump_block_append(&m->bump_live, &m->bump_live_count,
                                     &m->bump_live_cap, m->bump_live_count,
                                     addr, align4(len ? len : 1));
}

static uint32_t arm_ext_bump_reuse(ArmExtModule *m, uint32_t len) {
    len = align4(len ? len : 1);
    for (uint32_t i = 0; i < m->bump_free_count; ++i) {
        ArmExtBumpBlock *b = &m->bump_free_blocks[i];
        if (b->len < len) continue;
        uint32_t addr = b->addr;
        /* 剩余不足以成块时整块给出(与池分配器拆分粒度一致) */
        int split = (b->len - len >= 8);
        uint32_t give = split ? len : b->len;
        /* B6:先登记 live 成功再改空闲表。此前先摘链后登记,登记失败
         * (宿主 OOM)时块既不在空闲表也不在 live 表,永久泄漏 */
        if (!arm_ext_bump_track(m, addr, give)) return 0;
        if (split) {
            b->addr += give;
            b->len -= give;
        } else {
            arm_ext_bump_block_remove(m->bump_free_blocks,
                                      &m->bump_free_count, i);
        }
        return addr;
    }
    return 0;
}

static int arm_ext_bump_recycle(ArmExtModule *m, uint32_t addr) {
    uint32_t live_idx = 0;
    uint32_t len = 0;
    for (; live_idx < m->bump_live_count; ++live_idx) {
        if (m->bump_live[live_idx].addr == addr) {
            len = m->bump_live[live_idx].len;
            break;
        }
    }
    if (!len) return 0; /* 未登记的地址:不属于应用的 bump 块,忽略 */
    arm_ext_bump_block_remove(m->bump_live, &m->bump_live_count, live_idx);

    /* 按地址有序插入空闲表并与前后相邻块合并,但剔除已登记 EXT 映像。
     * bump 后备块也可能承载 readFile 得到的子模块;注册为代码后不能再
     * 因应用 free 而复用成像素/资源缓冲。 */
    uint32_t cursor = addr;
    uint32_t range_end = addr + len;
    while (cursor < range_end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        uint32_t remaining = range_end - cursor;
        if (!arm_ext_find_first_registered_code_overlap(m, cursor, remaining,
                                                        &protected_lo,
                                                        &protected_hi)) {
            arm_ext_bump_free_insert(m, cursor, remaining);
            break;
        }
        if (cursor < protected_lo)
            arm_ext_bump_free_insert(m, cursor, protected_lo - cursor);
        if (protected_hi <= cursor) break;
        cursor = protected_hi;
    }
    return 1;
}

void arm_ext_protect_registered_module_storage(ArmExtModule *m,
                                                      uint32_t file_addr,
                                                      uint32_t file_len) {
    if (!m || !file_addr || !file_len) return;
    /*
     * A registered module's file range is instruction/literal storage, even if
     * it originally came from an app-visible readFile allocation.  Drop that
     * interval from bump live/free tracking so later app frees cannot recycle
     * executable bytes as ordinary data.
     */
    arm_ext_bump_block_remove_range(&m->bump_live, &m->bump_live_count,
                                    &m->bump_live_cap, file_addr, file_len);
    arm_ext_bump_block_remove_range(&m->bump_free_blocks, &m->bump_free_count,
                                    &m->bump_free_cap, file_addr, file_len);
    arm_ext_bump_block_remove_range(&m->app_live_blocks, &m->app_live_count,
                                    &m->app_live_cap, file_addr, file_len);
}

/* 应用可见内存分配/释放的统一入口(table[0]/[2]/[125] 输出):
 * 池 first-fit(地址预测成立)→ bump 空闲块复用 → bump 新块。 */
uint32_t arm_ext_app_mem_malloc(ArmExtModule *m, uint32_t len) {
    uint32_t ret = arm_ext_guest_mem_malloc(m, len);
    if (!ret) {
        ret = arm_ext_bump_reuse(m, len);
    }
    if (!ret) {
        ret = arm_alloc(m, len);
        if (ret && !arm_ext_bump_track(m, ret, len)) {
            /* 登记失败(宿主内存不足)时块仍有效,只是不可回收 */
        }
    }
    if (ret) arm_ext_app_alloc_track(m, ret, len);
    return ret;
}

void arm_ext_app_mem_free(ArmExtModule *m, uint32_t p, uint32_t len) {
    if (!m || !p) return;
    /* A host bump block may lie numerically between a dynamically extended
     * LG_mem base/end without belonging to that guest free list.  Ownership
     * tracking therefore takes precedence over the ABI range test. */
    if (arm_ext_bump_recycle(m, p)) {
        arm_ext_app_alloc_untrack(m, p);
        return;
    }
    int app_owned = arm_ext_app_alloc_untrack(m, p);
    ArmExtOriginArena arena;
    if (arm_ext_current_origin_arena(m, &arena) &&
        p >= arena.base && p < arena.end &&
        arm_ptr_span(m, p, ARM_EXT_LG_MEM_ALIGN(len))) {
        /* The cross-band numeric interval also contains host-private mapped
         * storage.  Only table allocations may be returned while it is live;
         * registered module ranges are removed from app_live_blocks. */
        int dynamic = arena.base != m->origin_mem_addr ||
                      arena.end - arena.base != m->origin_mem_len;
        if (dynamic && !app_owned) return;
        arm_ext_guest_mem_free(m, p, len);
        return;
    }
}


#define ARM_EXT_COMPACT_TIMER_PROTECT_MAX 128u

static int arm_ext_compact_timer_magic_node_is_valid(ArmExtModule *m,
                                                     uint32_t node) {
    if (!m || !node || !arm_ext_addr_range_mapped(m, node, 0x20u))
        return 0;
    return arm_ext_read_u32_or_zero_(m, node) == ARM_EXT_COMPACT_TIMER_MAGIC;
}

static void arm_ext_compact_timer_protect_add(ArmExtModule *m,
                                              ArmExtBumpBlock *ranges,
                                              uint32_t *count,
                                              uint32_t node,
                                              int include_alloc_header) {
    if (!ranges || !count || *count >= ARM_EXT_COMPACT_TIMER_PROTECT_MAX)
        return;
    if (!arm_ext_compact_timer_magic_node_is_valid(m, node))
        return;

    uint32_t lo = node;
    uint32_t hi = node + 0x20u;
    if (hi < node) return;
    if (include_alloc_header) {
        if (node < 4u || node + 0x24u < node) return;
        /*
         * game.ext's SDK malloc fallback asks table[0] for len+4, stores the
         * size header at user-4, then returns the timer node at user.  The
         * compact allocator rounds that 0x24 request to 0x28, so the protected
         * range must include the header and trailing alignment bytes.
         */
        lo = node - 4u;
        hi = node + 0x24u;
    }

    for (uint32_t i = 0; i < *count; ++i) {
        if (ranges[i].addr == lo && ranges[i].len == hi - lo)
            return;
    }
    ranges[*count].addr = lo;
    ranges[*count].len = hi - lo;
    (*count)++;
}

static void arm_ext_collect_compact_timer_chain(ArmExtModule *m,
                                                ArmExtBumpBlock *ranges,
                                                uint32_t *count,
                                                uint32_t node,
                                                uint32_t next_off,
                                                int include_alloc_header) {
    for (uint32_t i = 0; i < ARM_EXT_COMPACT_TIMER_PROTECT_MAX && node; ++i) {
        uint32_t next = 0;
        if (!arm_ext_compact_timer_magic_node_is_valid(m, node))
            return;
        arm_ext_compact_timer_protect_add(m, ranges, count, node,
                                          include_alloc_header);
        next = arm_ext_read_u32_or_zero_(m, node + next_off);
        if (next == node) return;
        for (uint32_t seen = 0; seen < *count; ++seen) {
            uint32_t seen_node = ranges[seen].addr;
            if (include_alloc_header && seen_node + 4u >= seen_node)
                seen_node += 4u;
            if (next == seen_node) return;
        }
        node = next;
    }
}

static void arm_ext_collect_compact_timer_queue_pair(ArmExtModule *m,
                                                     ArmExtBumpBlock *ranges,
                                                     uint32_t *count,
                                                     uint32_t queued,
                                                     uint32_t current,
                                                     int include_alloc_header) {
    arm_ext_collect_compact_timer_chain(m, ranges, count, queued, 0x18u,
                                        include_alloc_header);
    arm_ext_collect_compact_timer_chain(m, ranges, count, current, 0x1Cu,
                                        include_alloc_header);
}

static void arm_ext_collect_primary_compact_timer_nodes(ArmExtModule *m,
                                                       ArmExtBumpBlock *ranges,
                                                       uint32_t *count) {
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!rw) return;

    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->primary_p_addr);
    if (mod && mod->compact_timer_scheduler_off) {
        uint32_t off = mod->compact_timer_scheduler_off;
        if (arm_ext_addr_range_mapped(m, rw + off, 0x14u)) {
            /* Protect the scheduler discovered from this module's own walker,
             * including allocator headers used by primary game EXT nodes. */
            arm_ext_collect_compact_timer_queue_pair(
                m, ranges, count,
                arm_ext_read_u32_or_zero_(m, rw + off + 0x08u),
                arm_ext_read_u32_or_zero_(m, rw + off + 0x0Cu), 1);
        }
    }
    arm_ext_collect_compact_timer_chain(m, ranges, count,
                                        read_game_timer_head(m, rw),
                                        0x18u, 1);
}

static void arm_ext_collect_wrapper_compact_timer_nodes(ArmExtModule *m,
                                                       ArmExtBumpBlock *ranges,
                                                       uint32_t *count) {
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t off = m ? m->wrapper_compact_timer_scheduler_off : 0;
    if (!wrapper_rw || !off ||
        !arm_ext_addr_range_mapped(m, wrapper_rw + off, 0x18u)) {
        return;
    }

    arm_ext_collect_compact_timer_queue_pair(
        m, ranges, count,
        arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x0Cu),
        arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x10u), 0);
}

static void arm_ext_collect_active_compact_timer_nodes(ArmExtModule *m,
                                                      ArmExtBumpBlock *ranges,
                                                      uint32_t *count) {
    if (!m || !m->active_p_addr || m->active_p_addr == m->primary_p_addr ||
        m->active_p_addr == m->p_addr) {
        return;
    }
    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->active_p_addr);
    if (!mod || !mod->file_addr || !mod->file_len)
        return;
    if (!mod->compact_timer_scheduler_off)
        return;

    uint32_t rw = arm_ext_active_rw_base(m);
    uint32_t off = mod->compact_timer_scheduler_off;
    if (arm_ext_addr_range_mapped(m, rw + off, 0x14u)) {
        arm_ext_collect_compact_timer_queue_pair(
            m, ranges, count,
            arm_ext_read_u32_or_zero_(m, rw + off + 0x08u),
            arm_ext_read_u32_or_zero_(m, rw + off + 0x0Cu), 0);
    }
}

int arm_ext_compact_heap_cut_range(ArmExtModule *m,
                                          uint32_t ctrl,
                                          uint32_t protect_lo,
                                          uint32_t protect_hi) {
    if (!m || !ctrl || protect_lo >= protect_hi ||
        !arm_ext_addr_range_mapped(m, ctrl, 0x1Cu)) {
        return 0;
    }

    uint32_t base = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_BASE_OFF);
    uint32_t end = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_END_OFF);
    if (end <= base || !arm_ext_addr_range_mapped(m, base, end - base))
        return 0;

    uint32_t heap_len = end - base;
    uint32_t prev_slot = ctrl + AEX_COMPACT_CTRL_HEAD_OFF;
    uint32_t off = arm_ext_read_u32_or_zero_(m, prev_slot);
    uint32_t iter_limit = heap_len / 8u + 4u;
    int changed = 0;

    while (off < heap_len && iter_limit--) {
        uint32_t node = base + off;
        if (!arm_ext_addr_range_mapped(m, node, 8u))
            break;
        uint32_t next_off = arm_ext_read_u32_or_zero_(m, node);
        uint32_t len = arm_ext_read_u32_or_zero_(m, node + 4u);
        if (len < 8u || len > end - node)
            break;

        uint32_t node_end = node + len;
        uint32_t lo = node > protect_lo ? node : protect_lo;
        uint32_t hi = node_end < protect_hi ? node_end : protect_hi;
        if (lo >= hi) {
            prev_slot = node;
            off = next_off;
            continue;
        }

        uint32_t left_len = lo > node ? lo - node : 0;
        uint32_t right_start = hi;
        uint32_t right_len = node_end > right_start ? node_end - right_start : 0;
        int keep_left = left_len >= 8u;
        int keep_right = right_len >= 8u;
        uint32_t removed = len -
            (keep_left ? left_len : 0u) -
            (keep_right ? right_len : 0u);

        if (keep_left && keep_right) {
            uint32_t right_off = right_start - base;
            arm_ext_guest_mem_write_u32(m, node + 4u, left_len);
            arm_ext_guest_mem_write_u32(m, node, right_off);
            arm_ext_guest_mem_write_u32(m, right_start, next_off);
            arm_ext_guest_mem_write_u32(m, right_start + 4u, right_len);
            prev_slot = right_start;
            off = next_off;
        } else if (keep_left) {
            arm_ext_guest_mem_write_u32(m, node + 4u, left_len);
            arm_ext_guest_mem_write_u32(m, node, next_off);
            prev_slot = node;
            off = next_off;
        } else if (keep_right) {
            uint32_t right_off = right_start - base;
            arm_ext_guest_mem_write_u32(m, right_start, next_off);
            arm_ext_guest_mem_write_u32(m, right_start + 4u, right_len);
            arm_ext_guest_mem_write_u32(m, prev_slot, right_off);
            prev_slot = right_start;
            off = next_off;
        } else {
            arm_ext_guest_mem_write_u32(m, prev_slot, next_off);
            off = next_off;
        }

        if (removed) {
            uint32_t free_bytes = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_FREE_OFF);
            free_bytes = removed < free_bytes ? free_bytes - removed : 0;
            arm_ext_guest_mem_write_u32(m, ctrl + AEX_COMPACT_CTRL_FREE_OFF, free_bytes);
        }
        changed = 1;
    }
    return changed;
}

static void arm_ext_sanitize_compact_heap_for_rw(ArmExtModule *m,
                                                 uint32_t rw,
                                                 const ArmExtBumpBlock *ranges,
                                                 uint32_t count) {
    if (!m || !rw || !m->wrapper_compact_heap_ctrl_off ||
        !ranges || !count) {
        return;
    }
    uint32_t ctrl_slot = rw + m->wrapper_compact_heap_ctrl_off;
    if (!arm_ext_addr_range_mapped(m, ctrl_slot, 4u))
        return;
    uint32_t ctrl = arm_ext_read_u32_or_zero_(m, ctrl_slot);
    if (!ctrl) return;

    /*
     * The compact heap control block is per-module RW state, but the allocator
     * routine itself is shared wrapper code and reads it through R9.  Sanitize
     * each discovered control block structurally; no application name or MRP
     * path participates in the decision.
     */
    for (uint32_t i = 0; i < count; ++i) {
        arm_ext_compact_heap_cut_range(m, ctrl, ranges[i].addr,
                                       ranges[i].addr + ranges[i].len);
    }
}

static void arm_ext_collect_protect_range_(ArmExtBumpBlock *ranges,
                                           uint32_t *count,
                                           uint32_t addr, uint32_t len) {
    if (!addr || !len || *count >= ARM_EXT_COMPACT_TIMER_PROTECT_MAX)
        return;
    ranges[*count].addr = addr;
    ranges[*count].len = len;
    (*count)++;
}

/*
 * 已注册 EXT 模块的存活存储(文件映像 + ER_RW 静态段)不允许出现在 compact
 * free-list 里,否则后续 malloc 会把精灵/位图缓冲切进正在执行的代码或静态
 * 变量区。gzwdzjs 删除 plugins/netpay.mrp 后的错误路径实测:
 *   - 启动时 wrapper 把 gzip 代码段读进 RAM pack 缓冲 0x2314C0(len 72570,
 *     尾部到 0x24303A),readFile('abc') 解压出 game.ext 到 0x226118+0x1C70C,
 *     其映像与随后的 ER_RW(0x24282C+0x4DF4)都落在 RAM pack 区间内;
 *   - RAM pack 其后被 free 进 compact free-list,free-list 便获得覆盖模块
 *     存活存储的空闲段;
 *   - 只保护文件映像时,精灵缓存仍会分配到 0x242828 附近,RGB565 像素
 *     (0xF81F 等)覆写 ER_RW 头部,帧回调指针变成像素垃圾 0x9359B3BB,
 *     事件回调 blx 到未映射地址崩溃一次后定时器泵瘫痪、画面冻结;
 *   - 只保护代码前的版本则是像素覆写 0x231550 指令→SP=0x61/0x9 崩溃循环。
 * 与 LG_mem 侧 arm_ext_find_first_registered_code_overlap 的保护语义一致,
 * 这里把当前注册模块的文件映像和 ER_RW 区间一并加入 compact 堆保护集。
 * ER_RW 基址/长度读自各模块 P 结构(start_of_ER_RW / ER_RW_Length)。模块被
 * 替换/丢弃后不再注册,区间自然恢复可分配,不影响"先 free 再 readFile
 * 复用"的行为。
 */
static void arm_ext_collect_registered_module_ranges(ArmExtModule *m,
                                                     ArmExtBumpBlock *ranges,
                                                     uint32_t *count) {
    if (!m || !ranges || !count) return;
    /* 每个模块条目:文件映像 {file_addr,file_len} + P 结构地址(取 ER_RW) */
    struct { uint32_t addr, len, p_addr; } mod[2 + ARM_EXT_NESTED_MODULE_MAX];
    uint32_t n = 0;
    mod[n].addr = m->primary_file_addr;
    mod[n].len = m->primary_file_len;
    mod[n].p_addr = m->p_addr;
    n++;
    /* pending_internal 尚未 sync,还没有 P 结构,只保护文件映像 */
    mod[n].addr = m->pending_internal_file_addr;
    mod[n].len = m->pending_internal_file_len;
    mod[n].p_addr = 0;
    n++;
    for (int i = 0; i < m->nested_module_count && n < 2 + ARM_EXT_NESTED_MODULE_MAX; ++i) {
        mod[n].addr = m->nested_modules[i].file_addr;
        mod[n].len = m->nested_modules[i].file_len;
        mod[n].p_addr = m->nested_modules[i].p_addr;
        n++;
    }
    for (uint32_t i = 0; i < n; ++i) {
        if (!mod[i].addr || !mod[i].len)
            continue;
        arm_ext_collect_protect_range_(ranges, count, mod[i].addr, mod[i].len);
        if (!mod[i].p_addr ||
            !arm_ext_addr_range_mapped(m, mod[i].p_addr, 8u))
            continue;
        /* mr_c_function_P_t: +0=start_of_ER_RW,+4=ER_RW_Length */
        uint32_t rw = arm_ext_read_u32_or_zero_(m, mod[i].p_addr);
        uint32_t rw_len = arm_ext_read_u32_or_zero_(m, mod[i].p_addr + AEX_P_ER_RW_LEN_OFF);
        if (!rw || !rw_len || !arm_ext_addr_range_mapped(m, rw, rw_len))
            continue;
        arm_ext_collect_protect_range_(ranges, count, rw, rw_len);
    }
}

/*
 * P4.3:保护集区间排序 + 合并(重叠或相邻区间并为一段)。
 * 四个收集器(primary/wrapper/active timer 节点 + 注册模块映像/ER_RW)
 * 各自独立追加且可能重复登记同一节点(issues doc "无去重"),cut_range
 * 对重复区间只是空跑;合并后语义不变,遍历次数确定且便于诊断输出。
 * 非 static:单元测试直接覆盖合并逻辑。
 */
uint32_t arm_ext_merge_protect_ranges(ArmExtBumpBlock *r, uint32_t n) {
    if (n < 2) return n;
    /* 区间数 ≤128,插入排序足够 */
    for (uint32_t i = 1; i < n; ++i) {
        ArmExtBumpBlock key = r[i];
        uint32_t j = i;
        while (j > 0 && r[j - 1].addr > key.addr) {
            r[j] = r[j - 1];
            --j;
        }
        r[j] = key;
    }
    uint32_t out = 0;
    for (uint32_t i = 1; i < n; ++i) {
        uint32_t cur_end = r[out].addr + r[out].len;
        if (r[i].addr <= cur_end) { /* 重叠或相邻 */
            uint32_t new_end = r[i].addr + r[i].len;
            if (new_end > cur_end) r[out].len = new_end - r[out].addr;
        } else {
            r[++out] = r[i];
        }
    }
    return out + 1;
}

/* P4.3:统一保护集收集入口 —— live timer 节点 + 注册模块存活存储,
 * 收集后排序合并。sanitize 与后续诊断/分配期检查共用同一集合。 */
uint32_t arm_ext_collect_protected_ranges(ArmExtModule *m,
                                          ArmExtBumpBlock *ranges) {
    uint32_t count = 0;
    arm_ext_collect_primary_compact_timer_nodes(m, ranges, &count);
    arm_ext_collect_wrapper_compact_timer_nodes(m, ranges, &count);
    arm_ext_collect_active_compact_timer_nodes(m, ranges, &count);
    /* 注册模块的映像与 ER_RW 静态段与 live timer node 一样属于不可分配区间 */
    arm_ext_collect_registered_module_ranges(m, ranges, &count);
    return arm_ext_merge_protect_ranges(ranges, count);
}

void arm_ext_sanitize_compact_timer_heaps(ArmExtModule *m) {
    if (!m || !m->wrapper_compact_heap_ctrl_off)
        return;

    ArmExtBumpBlock ranges[ARM_EXT_COMPACT_TIMER_PROTECT_MAX];
    uint32_t count = arm_ext_collect_protected_ranges(m, ranges);
    if (!count) return;

    arm_ext_sanitize_compact_heap_for_rw(m, arm_ext_primary_rw_base_(m),
                                         ranges, count);
    arm_ext_sanitize_compact_heap_for_rw(m, arm_ext_wrapper_rw_base_(m),
                                         ranges, count);
    arm_ext_sanitize_compact_heap_for_rw(m, arm_ext_active_rw_base(m),
                                         ranges, count);
    arm_ext_sanitize_compact_heap_for_rw(
        m, reg_read32(m->uc, UC_ARM_REG_R9), ranges, count);
    for (int i = 0; i < m->nested_module_count; ++i) {
        uint32_t rw = arm_ext_read_u32_or_zero_(m,
                                                m->nested_modules[i].p_addr);
        arm_ext_sanitize_compact_heap_for_rw(m, rw, ranges, count);
    }
}

void hook_compact_heap_free_return(uc_engine *uc, uint64_t address,
                                          uint32_t size, void *user_data) {
    (void)uc;
    (void)address;
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    /*
     * The hook is installed only on the discovered compact mr_free() return
     * instruction.  At this point the ARM allocator has finished inserting or
     * merging the free block, so removing live timer allocations preserves the
     * allocator's own ordering instead of masking the callback that caused it.
     */
    arm_ext_sanitize_compact_timer_heaps(m);
}

/*
 * ============ 运行时不变量检查器(重构安全网 P0.3)============
 *
 * VMRP_ARM_EXT_INVARIANTS=1 时,在 arm_ext_call / arm_ext_call_dispatch 的
 * 出入口做结构一致性检查。目的:把"修 A 坏 B"从像素级症状(冻结/花屏)
 * 提前到状态级报警——gzwdzjs 无 netpay 冻结(free-list 覆盖模块存活区)
 * 这类问题在覆写发生前就能打印出来。只读检查,不修改任何 guest 状态;
 * 违反时打印 "INVARIANT ..." 一行并计数,不中断执行(由观察者决定处置)。
 */

static int arm_ext_invariants_enabled_(void) {
    static int cached = -1;
    if (cached < 0) cached = getenv("VMRP_ARM_EXT_INVARIANTS") ? 1 : 0;
    return cached;
}

/* 单个 compact 堆 free-list 与保护区间(模块映像+ER_RW+live timer)求交,
 * 遍历逻辑与 arm_ext_compact_heap_cut_range 相同但只读 */
static int arm_ext_invariant_check_freelist_overlap_(
    ArmExtModule *m, const char *where, uint32_t rw,
    const ArmExtBumpBlock *ranges, uint32_t count) {
    int violations = 0;
    if (!m || !rw || !m->wrapper_compact_heap_ctrl_off) return 0;
    uint32_t ctrl_slot = rw + m->wrapper_compact_heap_ctrl_off;
    if (!arm_ext_addr_range_mapped(m, ctrl_slot, 4u)) return 0;
    uint32_t ctrl = arm_ext_read_u32_or_zero_(m, ctrl_slot);
    if (!ctrl || !arm_ext_addr_range_mapped(m, ctrl, 0x1Cu)) return 0;
    uint32_t base = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_BASE_OFF);
    uint32_t end = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_END_OFF);
    if (end <= base || !arm_ext_addr_range_mapped(m, base, end - base))
        return 0;
    uint32_t heap_len = end - base;
    uint32_t off = arm_ext_read_u32_or_zero_(m, ctrl + AEX_COMPACT_CTRL_HEAD_OFF);
    uint32_t iter_limit = heap_len / 8u + 4u;
    while (off < heap_len && iter_limit--) {
        uint32_t node = base + off;
        if (!arm_ext_addr_range_mapped(m, node, 8u)) break;
        uint32_t next_off = arm_ext_read_u32_or_zero_(m, node);
        uint32_t len = arm_ext_read_u32_or_zero_(m, node + 4u);
        if (len < 8u || len > end - node) break;
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t lo = ranges[i].addr;
            uint32_t hi = ranges[i].addr + ranges[i].len;
            uint32_t olo = node > lo ? node : lo;
            uint32_t ohi = node + len < hi ? node + len : hi;
            if (olo < ohi) {
                printf("INVARIANT %s: compact free node 0x%X+0x%X overlaps protected 0x%X..0x%X (rw=0x%X)\n",
                       where, node, len, lo, hi, rw);
                violations++;
            }
        }
        off = next_off;
    }
    return violations;
}

static int arm_ext_invariant_owner_is_known_(ArmExtModule *m, uint32_t p) {
    if (!p || p == m->p_addr) return 1;
    for (int i = 0; i < m->nested_module_count; ++i) {
        if (m->nested_modules[i].p_addr == p) return 1;
    }
    return 0;
}

int arm_ext_verify_invariants(ArmExtModule *m, const char *where) {
    if (!m || !arm_ext_invariants_enabled_()) return 0;
    int violations = 0;

    /* 1. bump 堆顶界内 */
    if (m->heap_top < EXT_HEAP_ADDR ||
        m->heap_top > EXT_BASE_ADDR + EXT_MEM_SIZE) {
        printf("INVARIANT %s: heap_top 0x%X out of range\n", where,
               m->heap_top);
        violations++;
    }

    /* 2. 注册表容量与模块映像可映射 */
    if (m->nested_module_count < 0 ||
        m->nested_module_count > ARM_EXT_NESTED_MODULE_MAX) {
        printf("INVARIANT %s: nested_module_count %d out of range\n", where,
               m->nested_module_count);
        violations++;
    }
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->file_addr && mod->file_len &&
            !arm_ext_addr_range_mapped(m, mod->file_addr, mod->file_len)) {
            printf("INVARIANT %s: nested[%d] file 0x%X+0x%X unmapped\n",
                   where, i, mod->file_addr, mod->file_len);
            violations++;
        }
    }

    /* 3. active/timer owner 必须指向 wrapper 或已注册模块的 P */
    if (!arm_ext_invariant_owner_is_known_(m, m->active_p_addr)) {
        printf("INVARIANT %s: active_p 0x%X not a registered module P\n",
               where, m->active_p_addr);
        violations++;
    }
    if (!arm_ext_invariant_owner_is_known_(m, m->timer_p_addr)) {
        printf("INVARIANT %s: timer_p 0x%X not a registered module P\n",
               where, m->timer_p_addr);
        violations++;
    }

    /* 4. compact free-list 不得覆盖模块存活存储/live timer(gzwdzjs 根因类) */
    ArmExtBumpBlock ranges[ARM_EXT_COMPACT_TIMER_PROTECT_MAX];
    uint32_t count = 0;
    arm_ext_collect_registered_module_ranges(m, ranges, &count);
    if (count) {
        violations += arm_ext_invariant_check_freelist_overlap_(
            m, where, arm_ext_primary_rw_base_(m), ranges, count);
        violations += arm_ext_invariant_check_freelist_overlap_(
            m, where, arm_ext_wrapper_rw_base_(m), ranges, count);
        violations += arm_ext_invariant_check_freelist_overlap_(
            m, where, arm_ext_active_rw_base(m), ranges, count);
        for (int i = 0; i < m->nested_module_count; ++i) {
            uint32_t rw = arm_ext_read_u32_or_zero_(
                m, m->nested_modules[i].p_addr);
            violations += arm_ext_invariant_check_freelist_overlap_(
                m, where, rw, ranges, count);
        }
    }

    if (violations) {
        printf("INVARIANT %s: %d violation(s)\n", where, violations);
    }
    return violations;
}


uint32_t arm_alloc(ArmExtModule *m, uint32_t len) {
    /* 长度上限守卫:len 接近 UINT32_MAX 时 align4 与 heap_top+len 均会回绕,
     * 绕过下方容量检查返回"成功"地址(guest 负长度经 case 132/25 等传入,
     * issues doc B3)。EXT 总内存只有 16MB,超过即明确失败,合法调用不受影响 */
    if (len > EXT_MEM_SIZE) return 0;
    len = align4(len ? len : 1);
    /* bump 堆从 EXT_HEAP_ADDR(0x200000) 向上生长,必须跳过
     * EXT 栈区 [EXT_STACK_ADDR, EXT_CODE_ADDR) 和 wrapper 代码区
     * [EXT_CODE_ADDR, +code_len):此前只检查 16MB 总界,长流程
     * (talkcat 喝水资源包下载安装)累计分配越过 0xE00000 后,
     * SCRRAM/大缓冲直接落在 wrapper 栈里,整屏位图把返回地址踩成
     * 像素值(PC=0x6246834A 一类 FETCH_UNMAPPED)。跳过保留区后
     * [代码区末, 16MB) 仍可继续分配。 */
    uint32_t reserved_lo = EXT_STACK_ADDR;
    uint32_t reserved_hi = EXT_CODE_ADDR + align4(m->code_len);
    if (m->heap_top < reserved_hi && m->heap_top + len > reserved_lo) {
        m->heap_top = reserved_hi;
    }
    /* 屏幕缓冲迁移到顶部保留区后(hook_screen_dim_write),该区对 bump 堆
     * 关闭,否则宿主绘制会覆写堆上分配。未迁移时保持完整 16MB 可用,
     * 与旧版本布局/容量完全一致(gghjt 等长流程依赖)。 */
    uint32_t mem_end = EXT_BASE_ADDR + EXT_MEM_SIZE;
    if (m->screen_addr == mem_end - EXT_SCREEN_RESERVE) {
        mem_end -= EXT_SCREEN_RESERVE;
    }
    if (m->heap_top + len >= mem_end) return 0;
    uint32_t ret = m->heap_top;
    m->heap_top += len;
    return ret;
}

/* guest 改写 ARM 可见 mr_screen_w/h 变量(table[92/93] 指向的 u32)时触发。
 * 部分 C SDK 游戏(gtcm 的 SphinxJoy 引擎)在 init 用这两个变量配置比物理
 * 屏更大的逻辑画布(横屏 480x320):真机上 DSM framebuffer 与配置一致,而
 * 模拟器的 EXT 屏幕缓冲按物理分辨率分配在 bump 堆首,若不处理,宿主
 * DrawRect/DrawBitmap 按新尺寸写入旧缓冲会越界抹掉紧随其后的模块镜像
 * (gtcm 黑屏根因)。超出容量时把缓冲迁移到 16MB 空间顶部的专属保留区:
 * 迁移发生在写变量的瞬间,早于 graphics.ext 缓存 framebuffer 指针,guest
 * 后续读 table[91] 拿到的即是新地址;bump 堆布局保持与旧版本一致,不影响
 * 依赖既有堆地址的应用(gghjt netpay dump0 恢复等)。 */

uint32_t arm_ext_meta_alloc(ArmExtModule *m, uint32_t len) {
    if (!m) return 0;
    len = align4(len ? len : 1);
    if (len > EXT_EXECUTOR_META_SIZE ||
        m->executor_meta_top > EXT_EXECUTOR_META_SIZE - len) {
        return 0;
    }
    uint32_t ret = EXT_EXECUTOR_META_ADDR + m->executor_meta_top;
    m->executor_meta_top += len;
    return ret;
}
