/*
 * test_main.c — 最小测试运行器
 */
#include <stdio.h>

extern void test_dsm_init(void);
extern int test_got_snapshot_run_all(void);
extern int test_parser_run_all(void);
extern int test_dns_map_run_all(void);

int main(void) {
    int total_failures = 0;

    /* 初始化 DSM 层——Lua VM 的内存分配器依赖它 */
    test_dsm_init();

    printf("=== GOT Snapshot Tests ===\n");
    total_failures += test_got_snapshot_run_all();

    printf("\n=== Parser Tests ===\n");
    total_failures += test_parser_run_all();

    printf("\n=== DNS Map Tests ===\n");
    total_failures += test_dns_map_run_all();

    printf("\n");
    if (total_failures == 0) {
        printf("All tests PASSED\n");
    } else {
        printf("%d test(s) FAILED\n", total_failures);
    }
    return total_failures ? 1 : 0;
}
