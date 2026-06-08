/*
 * test_filelib_main.c — fileLib 测试运行器
 */
#include <stdio.h>
#define HARNESS_IMPL
#define HARNESS_MULTI_FILE
#include "harness.h"

extern int test_filelib_run_all(void);

int main(void) {
    printf("=== FileLib Tests ===\n");
    test_filelib_run_all();

    TEST_SUMMARY();
    return TEST_EXIT_CODE();
}
