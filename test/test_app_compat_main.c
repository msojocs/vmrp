#include <stdio.h>

extern int test_app_compat_run_all(void);

int main(void) {
    int failures;

    printf("=== App Compat Tests ===\n");
    failures = test_app_compat_run_all();

    if (failures == 0) {
        printf("All app compat tests PASSED\n");
    } else {
        printf("%d app compat test(s) FAILED\n", failures);
    }
    return failures ? 1 : 0;
}
