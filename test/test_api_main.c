/*
 * test_api_main.c — vmrp_api 测试运行器
 */
#include <stdio.h>
#define HARNESS_IMPL
#define HARNESS_MULTI_FILE
#include "harness.h"

extern int test_api_lifecycle_run_all(void);

int main(void) {
    printf("=== API Lifecycle Tests ===\n");
    test_api_lifecycle_run_all();

    TEST_SUMMARY();
    return TEST_EXIT_CODE();
}
