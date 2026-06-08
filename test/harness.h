/*
 * harness.h — 自包含测试框架，无外部依赖
 *
 * 设计原则：
 *   - Header-only，直接 #include 即可使用
 *   - 非中断式断言：所有测试都会运行，最后统一报告
 *   - TEST_BEGIN/TEST_END 块结构，输出清晰
 *   - 与现有 ASSERT_EQ/ASSERT_INT 宏兼容，可逐步迁移
 *
 * 用法：
 *   #include "harness.h"
 *
 *   void test_foo(void) {
 *       TEST_BEGIN("foo returns 42");
 *       int result = foo();
 *       ASSERT_INT_EQ(42, result);
 *       TEST_END();
 *   }
 *
 *   int main(void) {
 *       test_foo();
 *       TEST_SUMMARY();
 *       return TEST_EXIT_CODE();
 *   }
 */
#ifndef VMRP_TEST_HARNESS_H
#define VMRP_TEST_HARNESS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ── 计数器 ───────────────────────────────────────────────── */
/* 单文件使用时（只有一个 .c 包含 harness.h），直接用 static 即可。
 * 多文件使用时，在 main 所在的 .c 文件 include 前定义 HARNESS_IMPL，
 * 其他文件直接 include 即可。 */
#ifdef HARNESS_IMPL
int harness_tests_run    = 0;
int harness_tests_passed = 0;
int harness_tests_failed = 0;
#elif !defined(HARNESS_MULTI_FILE)
/* 单文件模式：static 避免链接冲突 */
static int harness_tests_run    = 0;
static int harness_tests_passed = 0;
static int harness_tests_failed = 0;
#else
/* 多文件模式但非 IMPL 文件：extern 声明 */
extern int harness_tests_run;
extern int harness_tests_passed;
extern int harness_tests_failed;
#endif

/* ── TEST_BEGIN / TEST_END 块 ────────────────────────────── */
/* 每个测试用例用 TEST_BEGIN/TEST_END 包裹。
 * 内部 _test_ok 标记该测试是否全部断言通过。 */

#define TEST_BEGIN(name) \
    do { \
        harness_tests_run++; \
        const char *_test_name = (name); \
        int _test_ok = 1; \
        (void)_test_ok; \
        printf("  [RUN ] %s\n", _test_name);

#define TEST_END() \
        if (_test_ok) { \
            harness_tests_passed++; \
            printf("  [ OK ] %s\n", _test_name); \
        } else { \
            harness_tests_failed++; \
            printf("  [FAIL] %s\n", _test_name); \
        } \
    } while (0)

/* ── 断言宏 ───────────────────────────────────────────────── */
/* 所有断言失败时设置 _test_ok = 0 但不 return，
 * 保证同一个 TEST_BEGIN/TEST_END 块内的后续断言仍会执行。 */

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            printf("    ASSERT_TRUE failed: %s\n" \
                   "    at %s:%d\n", #expr, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            printf("    ASSERT_FALSE failed: %s is true\n" \
                   "    at %s:%d\n", #expr, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_INT_EQ(expected, actual) \
    do { \
        long long _e = (long long)(expected); \
        long long _a = (long long)(actual); \
        if (_e != _a) { \
            printf("    ASSERT_INT_EQ failed: expected %lld, got %lld\n" \
                   "    expr: %s == %s\n" \
                   "    at %s:%d\n", \
                   _e, _a, #expected, #actual, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_INT_NEQ(unexpected, actual) \
    do { \
        long long _u = (long long)(unexpected); \
        long long _a = (long long)(actual); \
        if (_u == _a) { \
            printf("    ASSERT_INT_NEQ failed: should not be %lld\n" \
                   "    expr: %s != %s\n" \
                   "    at %s:%d\n", \
                   _u, #unexpected, #actual, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_UINT_EQ(expected, actual) \
    do { \
        unsigned long long _e = (unsigned long long)(expected); \
        unsigned long long _a = (unsigned long long)(actual); \
        if (_e != _a) { \
            printf("    ASSERT_UINT_EQ failed: expected 0x%llX, got 0x%llX\n" \
                   "    expr: %s == %s\n" \
                   "    at %s:%d\n", \
                   _e, _a, #expected, #actual, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_STR_EQ(expected, actual) \
    do { \
        const char *_e = (expected); \
        const char *_a = (actual); \
        if (_e == NULL || _a == NULL || strcmp(_e, _a) != 0) { \
            printf("    ASSERT_STR_EQ failed:\n" \
                   "      expected: \"%s\"\n" \
                   "      actual:   \"%s\"\n" \
                   "    at %s:%d\n", \
                   _e ? _e : "(null)", _a ? _a : "(null)", \
                   __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_PTR_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("    ASSERT_PTR_NOT_NULL failed: %s is NULL\n" \
                   "    at %s:%d\n", #ptr, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_PTR_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("    ASSERT_PTR_NULL failed: %s is not NULL\n" \
                   "    at %s:%d\n", #ptr, __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

#define ASSERT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            printf("    ASSERT_MEM_EQ failed: %zu bytes differ\n" \
                   "    at %s:%d\n", (size_t)(len), __FILE__, __LINE__); \
            _test_ok = 0; \
        } \
    } while (0)

/* ── 汇总与退出码 ─────────────────────────────────────────── */

#define TEST_SUMMARY() \
    do { \
        printf("\n==== %d tests: %d passed, %d failed ====\n", \
               harness_tests_run, harness_tests_passed, \
               harness_tests_failed); \
    } while (0)

#define TEST_EXIT_CODE() (harness_tests_failed > 0 ? 1 : 0)

#endif /* VMRP_TEST_HARNESS_H */
