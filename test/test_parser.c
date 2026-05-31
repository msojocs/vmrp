/*
 * test_parser.c — 验证 MRP Lua 解析器能在运行时编译源码
 *
 * 背景：之前构建使用了 mr_noparser.c，_loads() 返回 "parser not loaded"，
 * 导致服务器下发的 Lua 源码命令帧无法编译执行，网络更新失败。
 * 修复后使用了真正的解析器（mr_parser.c + mr_lex.c + mr_code.c）。
 */
/* type.h 会重定义 size_t/ptrdiff_t/uintptr_t 为 32-bit 类型，
 * 与 x86_64 系统头冲突。先 include 系统头，再跳过 type.h 中的
 * 冲突定义，手动 typedef MRP 需要的整型 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#define _M_TYPE__
#include "../src/mythroad/include/mr.h"
#include "../src/mythroad/include/mr_auxlib.h"

static int failures = 0;

#define ASSERT_INT(desc, actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("  FAIL: %s: got %d, expected %d\n", desc, (actual), (expected)); \
        failures++; \
    } \
} while(0)

#define ASSERT_NEQ(desc, actual, unexpected) do { \
    if ((actual) == (unexpected)) { \
        printf("  FAIL: %s: got %d, should not be %d\n", desc, (actual), (unexpected)); \
        failures++; \
    } \
} while(0)

/* ---------- 测试用例 ---------- */

int test_parser_compile_simple(void) {
    printf("[TEST] compile simple source\n");
    mrp_State *L = mrp_open();
    if (!L) { printf("  FAIL: mrp_open returned NULL\n"); failures++; return 1; }

    int status = mr_L_loadbuffer(L, "x = 1", 5, "test");
    ASSERT_INT("simple assignment compiles", status, 0);

    mrp_close(L);
    return 0;
}

int test_parser_compile_def_keyword(void) {
    printf("[TEST] compile 'def' keyword (MRP extension for 'function')\n");
    mrp_State *L = mrp_open();
    if (!L) { printf("  FAIL: mrp_open returned NULL\n"); failures++; return 1; }

    const char *src = "def foo(a, b)\n  return a + b\nend";
    int status = mr_L_loadbuffer(L, src, strlen(src), "test_def");
    ASSERT_INT("def keyword compiles", status, 0);

    mrp_close(L);
    return 0;
}

int test_parser_compile_server_payload(void) {
    printf("[TEST] compile server code payload (applist-server.js format)\n");
    mrp_State *L = mrp_open();
    if (!L) { printf("  FAIL: mrp_open returned NULL\n"); failures++; return 1; }

    /* 与 tool/applist-server.js getCodePayload() 生成的载荷一致 */
    const char *src =
        "print(\"code frame received\")\n"
        "def progress(data)\n"
        "  p = tonumber(data)\n"
        "  if p == nil then\n"
        "    p = 0\n"
        "  end\n"
        "end\n";

    int status = mr_L_loadbuffer(L, src, strlen(src), "server_payload");
    ASSERT_INT("server payload compiles", status, 0);

    mrp_close(L);
    return 0;
}

int test_parser_syntax_error(void) {
    printf("[TEST] syntax error returns non-zero\n");
    mrp_State *L = mrp_open();
    if (!L) { printf("  FAIL: mrp_open returned NULL\n"); failures++; return 1; }

    const char *bad = "if then end";
    int status = mr_L_loadbuffer(L, bad, strlen(bad), "bad");
    ASSERT_NEQ("syntax error detected", status, 0);

    mrp_close(L);
    return 0;
}

int test_parser_run_all(void) {
    int before = failures;
    test_parser_compile_simple();
    test_parser_compile_def_keyword();
    test_parser_compile_server_payload();
    test_parser_syntax_error();
    return failures - before;
}
