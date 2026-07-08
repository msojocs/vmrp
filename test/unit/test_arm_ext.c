/*
 * ARM EXT 执行器单元测试基座(重构安全网 P0.2,
 * 见 docs/arm-ext-executor-refactor-plan.md)。
 *
 * arm_ext_executor.c 目前是单一翻译单元,被测函数(arm_alloc、
 * arm_ext_compact_heap_cut_range、arm_ext_guest_memcpy 等)都是 static,
 * 因此直接 #include 整个 .c 获得访问权;链接所需的 mythroad/bridge 外部
 * 符号由 unit_stubs.c 提供"一触即 abort"的桩(被测纯内存函数不应触达
 * host 桥,触达即测试失败)。Phase 2 文件拆分后应改为常规头文件+链接。
 */
#include "../../src/arm_ext_executor.c"

#include <stdio.h>

static int g_failures;
static int g_checks;
#define CHECK(cond)                                              \
    do {                                                         \
        g_checks++;                                              \
        if (!(cond)) {                                           \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            g_failures++;                                        \
        }                                                        \
    } while (0)

/* ---- 夹具:16MB 纯内存模块,不初始化 Unicorn ---- */
static ArmExtModule *make_module(void) {
    ArmExtModule *m = (ArmExtModule *)calloc(1, sizeof(*m));
    m->mem = (uint8_t *)calloc(1, EXT_MEM_SIZE);
    m->heap_top = EXT_HEAP_ADDR;
    return m;
}

static void free_module(ArmExtModule *m) {
    free(m->mem);
    free(m);
}

/* ================= arm_alloc ================= */

static void test_arm_alloc(void) {
    ArmExtModule *m = make_module();

    /* 常规分配:返回旧 heap_top,4 字节对齐推进 */
    CHECK(arm_alloc(m, 16) == EXT_HEAP_ADDR);
    CHECK(m->heap_top == EXT_HEAP_ADDR + 16);
    CHECK(arm_alloc(m, 5) == EXT_HEAP_ADDR + 16);
    CHECK(m->heap_top == EXT_HEAP_ADDR + 24); /* 5 -> 8 */
    CHECK(arm_alloc(m, 0) == EXT_HEAP_ADDR + 24);
    CHECK(m->heap_top == EXT_HEAP_ADDR + 28); /* 0 -> 1 -> 4 */

    /* B3 回归:超大长度(guest 负数)必须失败且不动 heap_top,
     * 不能靠回绕"碰巧"返回 0 */
    uint32_t before = m->heap_top;
    CHECK(arm_alloc(m, 0xFFFFFFF0u) == 0);
    CHECK(arm_alloc(m, 0xFFFFFFFFu) == 0);
    CHECK(arm_alloc(m, EXT_MEM_SIZE + 1u) == 0);
    CHECK(m->heap_top == before);

    /* 容量末端:剩余空间不足时返回 0 */
    m->heap_top = EXT_BASE_ADDR + EXT_MEM_SIZE - 8;
    CHECK(arm_alloc(m, 16) == 0);

    /* 栈/代码保留区跳过:分配越过 EXT_STACK_ADDR 时从代码区末尾继续 */
    m->code_len = 0x100;
    m->heap_top = EXT_STACK_ADDR - 8;
    uint32_t p = arm_alloc(m, 64);
    CHECK(p == EXT_CODE_ADDR + 0x100);
    CHECK(m->heap_top == EXT_CODE_ADDR + 0x100 + 64);

    /* 屏幕缓冲迁到顶部保留区后,该区对 bump 堆关闭 */
    m->screen_addr = EXT_BASE_ADDR + EXT_MEM_SIZE - EXT_SCREEN_RESERVE;
    m->heap_top = m->screen_addr - 8;
    CHECK(arm_alloc(m, 16) == 0);

    free_module(m);
}

/* ============ compact 堆 free-list cut_range ============ */

/* guest 侧布局常量(测试自建,与真实 wrapper 无关) */
#define T_CTRL 0x00300000u
#define T_BASE 0x00310000u
#define T_HEAP_LEN 0x1000u
#define T_END (T_BASE + T_HEAP_LEN)

static void w32(ArmExtModule *m, uint32_t addr, uint32_t v) {
    memcpy(arm_ptr(m, addr), &v, 4);
}
static uint32_t r32(ArmExtModule *m, uint32_t addr) {
    uint32_t v;
    memcpy(&v, arm_ptr(m, addr), 4);
    return v;
}

/* 建一个含两个空闲节点的 compact 堆:
 *   node A: off 0x100, len 0x200  ->  node B: off 0x800, len 0x100 -> 结束
 * free-list 节点格式 {next_off, len},头槽在 ctrl+0x18,off>=heap_len 终止 */
static ArmExtModule *make_compact_heap(void) {
    ArmExtModule *m = make_module();
    w32(m, T_CTRL + 0x08, T_BASE);
    w32(m, T_CTRL + 0x0C, 0x300); /* 空闲总字节 = 0x200+0x100 */
    w32(m, T_CTRL + 0x10, T_END);
    w32(m, T_CTRL + 0x18, 0x100); /* 头节点偏移 */
    w32(m, T_BASE + 0x100, 0x800);       /* A.next */
    w32(m, T_BASE + 0x100 + 4, 0x200);   /* A.len  */
    w32(m, T_BASE + 0x800, T_HEAP_LEN);  /* B.next = 结束 */
    w32(m, T_BASE + 0x800 + 4, 0x100);   /* B.len  */
    return m;
}

static void test_cut_range_middle_split(void) {
    /* 保护段落在 A 中部 -> A 拆成左右两段,B 不动 */
    ArmExtModule *m = make_compact_heap();
    uint32_t lo = T_BASE + 0x180, hi = T_BASE + 0x1C0;
    CHECK(arm_ext_compact_heap_cut_range(m, T_CTRL, lo, hi) == 1);
    CHECK(r32(m, T_CTRL + 0x18) == 0x100);        /* 头仍指向 A */
    CHECK(r32(m, T_BASE + 0x100 + 4) == 0x80);    /* A 左段 len=0x180-0x100 */
    CHECK(r32(m, T_BASE + 0x100) == 0x1C0);       /* A.next -> 右段 */
    CHECK(r32(m, T_BASE + 0x1C0 + 4) == 0x140);   /* 右段 len=0x300-0x1C0 */
    CHECK(r32(m, T_BASE + 0x1C0) == 0x800);       /* 右段.next -> B */
    CHECK(r32(m, T_CTRL + 0x0C) == 0x300 - 0x40); /* 空闲字节减去被裁的 0x40 */
    free_module(m);
}

static void test_cut_range_trim_head(void) {
    /* 保护段覆盖 A 开头 -> 只留右段,头槽改指右段 */
    ArmExtModule *m = make_compact_heap();
    uint32_t lo = T_BASE + 0x0F0, hi = T_BASE + 0x200;
    CHECK(arm_ext_compact_heap_cut_range(m, T_CTRL, lo, hi) == 1);
    CHECK(r32(m, T_CTRL + 0x18) == 0x200);      /* 头 -> 右段 */
    CHECK(r32(m, T_BASE + 0x200 + 4) == 0x100); /* 右段 len=0x300-0x200 */
    CHECK(r32(m, T_BASE + 0x200) == 0x800);     /* 右段.next -> B */
    free_module(m);
}

static void test_cut_range_trim_tail(void) {
    /* 保护段覆盖 A 末尾 -> 只留左段,链不变 */
    ArmExtModule *m = make_compact_heap();
    uint32_t lo = T_BASE + 0x280, hi = T_BASE + 0x400;
    CHECK(arm_ext_compact_heap_cut_range(m, T_CTRL, lo, hi) == 1);
    CHECK(r32(m, T_CTRL + 0x18) == 0x100);
    CHECK(r32(m, T_BASE + 0x100 + 4) == 0x180); /* A len=0x280-0x100 */
    CHECK(r32(m, T_BASE + 0x100) == 0x800);     /* A.next 仍 -> B */
    free_module(m);
}

static void test_cut_range_remove_whole(void) {
    /* 保护段整体吞掉 A -> A 从链上摘除,头槽 -> B */
    ArmExtModule *m = make_compact_heap();
    uint32_t lo = T_BASE + 0x0F8, hi = T_BASE + 0x308;
    CHECK(arm_ext_compact_heap_cut_range(m, T_CTRL, lo, hi) == 1);
    CHECK(r32(m, T_CTRL + 0x18) == 0x800);
    CHECK(r32(m, T_CTRL + 0x0C) == 0x100);      /* 只剩 B 的 0x100 */
    free_module(m);
}

static void test_cut_range_no_overlap(void) {
    /* 无重叠 -> 不改动,返回 0 */
    ArmExtModule *m = make_compact_heap();
    CHECK(arm_ext_compact_heap_cut_range(m, T_CTRL, T_BASE + 0x400,
                                         T_BASE + 0x700) == 0);
    CHECK(r32(m, T_CTRL + 0x18) == 0x100);
    CHECK(r32(m, T_BASE + 0x100 + 4) == 0x200);
    CHECK(r32(m, T_CTRL + 0x0C) == 0x300);
    free_module(m);
}

/* ============ guest memcpy/strncpy 重叠语义 ============ */

static void test_guest_memcpy(void) {
    /* 不重叠:等价 memcpy */
    char a[16] = "ABCDEFGH";
    arm_ext_guest_memcpy(a + 9, a, 4);
    CHECK(memcmp(a + 9, "ABCD", 4) == 0);

    /* dst<src 重叠(左移):前向拷贝等价 memmove */
    char b[16] = "ABCDEFGH";
    arm_ext_guest_memcpy(b, b + 2, 6);
    CHECK(memcmp(b, "CDEFGH", 6) == 0);

    /* dst>src 重叠(右移):前向拷贝产生周期性"涂抹",与真机朴素
     * memcpy 一致(而 memmove 会得到 "ABABCD",两者不同) */
    char c[16] = "ABCDEFGH";
    arm_ext_guest_memcpy(c + 2, c, 4);
    CHECK(memcmp(c, "ABABAB", 6) == 0);
}

static void test_guest_strncpy(void) {
    /* dst==src(talkcat 实测形态):内容不变,长度内补零不越界 */
    char a[16] = "hello\0XXXXXXX";
    arm_ext_guest_strncpy(a, a, 10);
    CHECK(memcmp(a, "hello\0\0\0\0\0", 10) == 0);
    CHECK(a[10] == 'X'); /* n 之外不动 */

    /* 不重叠:等价 strncpy(含补零) */
    char b[16];
    memset(b, 'Y', sizeof(b));
    arm_ext_guest_strncpy(b, "hi", 6);
    CHECK(memcmp(b, "hi\0\0\0\0", 6) == 0);
    CHECK(b[6] == 'Y');
}

/* ============ 不变量检查器(P0.3)============ */

static void test_verify_invariants(void) {
    /* 场景复刻 gzwdzjs 无 netpay 根因:compact free-list 里有一个空闲段
     * 覆盖已注册模块的映像/ER_RW。检查器必须报出 violation。 */
    ArmExtModule *m = make_module();
    m->wrapper_compact_heap_ctrl_off = 0x40;

    /* 注册一个 nested 模块:映像 0x310200+0x100,P 结构声明 RW 0x320000+0x80 */
    m->nested_module_count = 1;
    m->nested_modules[0].file_addr = T_BASE + 0x200;
    m->nested_modules[0].file_len = 0x100;
    m->nested_modules[0].p_addr = 0x00330000;
    w32(m, 0x00330000, 0x00320000);     /* start_of_ER_RW */
    w32(m, 0x00330000 + 4, 0x80);       /* ER_RW_Length   */

    /* 该模块 RW 上挂 compact 堆 ctrl,free-list 头节点 A(off 0x100,len
     * 0x200 → 0x310100..0x310300)与模块映像 0x310200..0x310300 重叠 */
    w32(m, 0x00320000 + 0x40, T_CTRL);
    w32(m, T_CTRL + 0x08, T_BASE);
    w32(m, T_CTRL + 0x0C, 0x200);
    w32(m, T_CTRL + 0x10, T_END);
    w32(m, T_CTRL + 0x18, 0x100);
    w32(m, T_BASE + 0x100, T_HEAP_LEN);
    w32(m, T_BASE + 0x100 + 4, 0x200);

    CHECK(arm_ext_verify_invariants(m, "unit-negative") > 0);

    /* 把空闲段挪到不重叠位置(off 0x400..0x500),应零报警 */
    w32(m, T_CTRL + 0x18, 0x400);
    w32(m, T_BASE + 0x400, T_HEAP_LEN);
    w32(m, T_BASE + 0x400 + 4, 0x100);
    CHECK(arm_ext_verify_invariants(m, "unit-positive") == 0);

    /* owner 指向未注册 P 也要报 */
    m->active_p_addr = 0x00340000;
    CHECK(arm_ext_verify_invariants(m, "unit-owner") > 0);
    m->active_p_addr = m->nested_modules[0].p_addr;
    CHECK(arm_ext_verify_invariants(m, "unit-owner-ok") == 0);

    free_module(m);
}

/* ============ 保护集区间合并(P4.3)============ */

static void test_merge_protect_ranges(void) {
    /* 乱序 + 重叠 + 相邻 + 重复 -> 合并为有序不相交集合 */
    ArmExtBumpBlock r1[] = {
        {0x300, 0x100}, /* 0x300..0x400 */
        {0x100, 0x080}, /* 0x100..0x180 */
        {0x3F0, 0x020}, /* 与第一段重叠 -> 并入 0x300..0x410 */
        {0x180, 0x040}, /* 与第二段相邻 -> 并入 0x100..0x1C0 */
        {0x100, 0x080}, /* 完全重复 */
    };
    uint32_t n = arm_ext_merge_protect_ranges(r1, 5);
    CHECK(n == 2);
    CHECK(r1[0].addr == 0x100 && r1[0].len == 0xC0);
    CHECK(r1[1].addr == 0x300 && r1[1].len == 0x110);

    /* 不相交保持原样(排序后) */
    ArmExtBumpBlock r2[] = {{0x500, 0x10}, {0x100, 0x10}};
    n = arm_ext_merge_protect_ranges(r2, 2);
    CHECK(n == 2);
    CHECK(r2[0].addr == 0x100 && r2[1].addr == 0x500);

    /* 单区间与空集 */
    ArmExtBumpBlock r3[] = {{0x100, 0x10}};
    CHECK(arm_ext_merge_protect_ranges(r3, 1) == 1);
    CHECK(arm_ext_merge_protect_ranges(r3, 0) == 0);

    /* 大区间吞并小区间 */
    ArmExtBumpBlock r4[] = {{0x200, 0x10}, {0x100, 0x400}};
    n = arm_ext_merge_protect_ranges(r4, 2);
    CHECK(n == 1);
    CHECK(r4[0].addr == 0x100 && r4[0].len == 0x400);
}

/* ============ bytes_contain / fnv1a(P5.2)============ */

static int naive_contains(const uint8_t *h, uint32_t hl,
                          const uint8_t *n, uint32_t nl) {
    if (!h || !n || !nl || nl > hl) return 0;
    for (uint32_t off = 0; off + nl <= hl; ++off)
        if (memcmp(h + off, n, nl) == 0) return 1;
    return 0;
}

static void test_bytes_contain(void) {
    /* memchr 跳跃版必须与朴素版逐一等价(含首字节高频重复的构造) */
    const uint8_t hay[] = "aaaaabaaaaabaacaab";
    struct { const char *n; } cases[] = {
        {"aab"}, {"aac"}, {"caa"}, {"aaaaab"}, {"b"}, {"x"},
        {"aaaaabaaaaabaacaab"}, {"aabx"},
    };
    for (unsigned i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        const uint8_t *n = (const uint8_t *)cases[i].n;
        uint32_t nl = (uint32_t)strlen(cases[i].n);
        CHECK(arm_ext_bytes_contain(hay, sizeof(hay)-1, n, nl) ==
              naive_contains(hay, sizeof(hay)-1, n, nl));
    }
    /* 边界:needle 等长/超长/空 */
    CHECK(arm_ext_bytes_contain(hay, 3, (const uint8_t *)"aaa", 3) == 1);
    CHECK(arm_ext_bytes_contain(hay, 2, (const uint8_t *)"aaa", 3) == 0);
    CHECK(arm_ext_bytes_contain(hay, 5, (const uint8_t *)"", 0) == 0);

    /* FNV-1a 已知向量 */
    CHECK(arm_ext_fnv1a("", 0) == 2166136261u);
    CHECK(arm_ext_fnv1a("a", 1) == 0xE40C292Cu);
}

/* ============ wrapper 指纹字节资产(P4.2 迁移安全网)============ */

#include "wrapper_assets.h"

/* 解出资产中某条 Thumb LDR literal 的 literal pool 窗口内偏移 */
static uint32_t asset_ldr_lit_off(const uint8_t *code, uint32_t insn_off) {
    uint16_t insn = (uint16_t)(code[insn_off] | (code[insn_off + 1] << 8));
    return ((insn_off + 4u) & ~3u) + (insn & 0xFFu) * 4u;
}

static void test_find_wrapper_timer_dispatch(void) {
    uint8_t buf[sizeof(asset_timer_dispatch_gxdzc)];
    uint32_t chain = 1, sched = 1;

    /* 正样本 gxdzc(16984 字节 SDK wrapper):命中 + sched_off=0x1FC */
    uint32_t ret = find_wrapper_timer_dispatch(
        asset_timer_dispatch_gxdzc, sizeof(asset_timer_dispatch_gxdzc),
        &chain, &sched);
    CHECK(ret == EXT_CODE_ADDR + asset_timer_dispatch_gxdzc_match_off + 1u);
    CHECK(chain == 0);
    CHECK(sched == 0x1FCu);

    /* 正样本 wbrw(20272 字节变体):同形指令流,literal 布局不同 */
    ret = find_wrapper_timer_dispatch(
        asset_timer_dispatch_wbrw, sizeof(asset_timer_dispatch_wbrw),
        &chain, &sched);
    CHECK(ret == EXT_CODE_ADDR + asset_timer_dispatch_wbrw_match_off + 1u);
    CHECK(sched == 0x2A8u);

    /* 通配位(i==0/16 为 LDR literal 立即数)扰动不影响命中 */
    memcpy(buf, asset_timer_dispatch_gxdzc, sizeof(buf));
    buf[asset_timer_dispatch_gxdzc_match_off + 0] ^= 0xFF;
    ret = find_wrapper_timer_dispatch(buf, sizeof(buf), &chain, &sched);
    CHECK(ret == EXT_CODE_ADDR + asset_timer_dispatch_gxdzc_match_off + 1u);

    /* 负样本:固定指令字节扰动 -> 不命中 */
    memcpy(buf, asset_timer_dispatch_gxdzc, sizeof(buf));
    buf[asset_timer_dispatch_gxdzc_match_off + 2] ^= 0x01;
    CHECK(find_wrapper_timer_dispatch(buf, sizeof(buf), &chain, &sched) == 0);
    CHECK(chain == 0 && sched == 0);

    /* sched literal 越界(>=0x1000)/未对齐 -> 地址仍命中但不提取偏移 */
    uint32_t lit = asset_ldr_lit_off(
        asset_timer_dispatch_gxdzc, asset_timer_dispatch_gxdzc_match_off + 16u);
    memcpy(buf, asset_timer_dispatch_gxdzc, sizeof(buf));
    buf[lit] = 0x00; buf[lit + 1] = 0x20; /* 0x2000 */
    ret = find_wrapper_timer_dispatch(buf, sizeof(buf), &chain, &sched);
    CHECK(ret != 0 && sched == 0);
    memcpy(buf, asset_timer_dispatch_gxdzc, sizeof(buf));
    buf[lit] = 0xFE; /* 0x1FE:未 4 对齐 */
    ret = find_wrapper_timer_dispatch(buf, sizeof(buf), &chain, &sched);
    CHECK(ret != 0 && sched == 0);
}

static void test_find_wrapper_chain_thunk(void) {
    /* gghjt/gwkdl 19428 字节 wrapper:链式 thunk 只记录地址供 code=2
     * 补发,绝不能作为宿主 timer dispatcher(返回值必须为 0) */
    uint8_t buf[sizeof(asset_chain_thunk_gghjt)];
    uint32_t chain = 1, sched = 1;
    uint32_t ret = find_wrapper_timer_dispatch(
        asset_chain_thunk_gghjt, sizeof(asset_chain_thunk_gghjt),
        &chain, &sched);
    CHECK(ret == 0);
    CHECK(chain == EXT_CODE_ADDR + asset_chain_thunk_gghjt_match_off + 1u);
    CHECK(sched == 0);

    /* 零通配指纹:任何一字节扰动都不命中 */
    memcpy(buf, asset_chain_thunk_gghjt, sizeof(buf));
    buf[asset_chain_thunk_gghjt_match_off + 4] ^= 0x01;
    ret = find_wrapper_timer_dispatch(buf, sizeof(buf), &chain, &sched);
    CHECK(ret == 0 && chain == 0);
}

static void test_find_wrapper_compact_heap_free_return(void) {
    uint8_t buf[sizeof(asset_free_return_gxdzc)];
    uint32_t ctrl = 1;

    /* 正样本 gxdzc:hook 点在 pop(+0x8C),ctrl_off=0x14C */
    uint32_t ret = find_wrapper_compact_heap_free_return(
        asset_free_return_gxdzc, sizeof(asset_free_return_gxdzc), &ctrl);
    CHECK(ret == EXT_CODE_ADDR + asset_free_return_gxdzc_match_off + 0x8Cu);
    CHECK(ctrl == 0x14Cu);

    /* 正样本 optwar(23492 字节变体):ctrl_off=0x270 */
    ret = find_wrapper_compact_heap_free_return(
        asset_free_return_optwar, sizeof(asset_free_return_optwar), &ctrl);
    CHECK(ret == EXT_CODE_ADDR + asset_free_return_optwar_match_off + 0x8Cu);
    CHECK(ctrl == 0x270u);

    /* 负样本:17 个固定半字任意扰动 -> 不命中 */
    memcpy(buf, asset_free_return_gxdzc, sizeof(buf));
    buf[asset_free_return_gxdzc_match_off + 0x8E] ^= 0x01; /* bx lr */
    CHECK(find_wrapper_compact_heap_free_return(buf, sizeof(buf), &ctrl) == 0);
    CHECK(ctrl == 0);

    /* 候选 LDR 必须是 r2:改写 rd -> 不命中 */
    memcpy(buf, asset_free_return_gxdzc, sizeof(buf));
    buf[asset_free_return_gxdzc_match_off + 1] = 0x49; /* ldr r1,[pc,...] */
    CHECK(find_wrapper_compact_heap_free_return(buf, sizeof(buf), &ctrl) == 0);

    /* ctrl literal 越界(<0x80 / >=0x1000)与未对齐 -> 不命中 */
    uint32_t lit = asset_ldr_lit_off(asset_free_return_gxdzc,
                                     asset_free_return_gxdzc_match_off);
    memcpy(buf, asset_free_return_gxdzc, sizeof(buf));
    buf[lit] = 0x7C; buf[lit + 1] = 0x00;
    CHECK(find_wrapper_compact_heap_free_return(buf, sizeof(buf), &ctrl) == 0);
    memcpy(buf, asset_free_return_gxdzc, sizeof(buf));
    buf[lit] = 0x00; buf[lit + 1] = 0x10; /* 0x1000 */
    CHECK(find_wrapper_compact_heap_free_return(buf, sizeof(buf), &ctrl) == 0);
    memcpy(buf, asset_free_return_gxdzc, sizeof(buf));
    buf[lit] = 0x4E; /* 0x14E:未对齐 */
    CHECK(find_wrapper_compact_heap_free_return(buf, sizeof(buf), &ctrl) == 0);

    /* 长度不足 0x90 -> 直接拒绝 */
    CHECK(find_wrapper_compact_heap_free_return(
              asset_free_return_gxdzc, 0x80u, &ctrl) == 0);
}

int main(void) {
    /* 不变量检查器由环境变量门控(进程内缓存),测试进程先行开启 */
    setenv("VMRP_ARM_EXT_INVARIANTS", "1", 1);
    test_arm_alloc();
    test_verify_invariants();
    test_merge_protect_ranges();
    test_bytes_contain();
    test_cut_range_middle_split();
    test_cut_range_trim_head();
    test_cut_range_trim_tail();
    test_cut_range_remove_whole();
    test_cut_range_no_overlap();
    test_guest_memcpy();
    test_guest_strncpy();
    test_find_wrapper_timer_dispatch();
    test_find_wrapper_chain_thunk();
    test_find_wrapper_compact_heap_free_return();
    printf("%s: %d checks, %d failures\n",
           g_failures ? "FAIL" : "PASS", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
