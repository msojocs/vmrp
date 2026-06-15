#include <stdio.h>

#include "../src/include/app_compat.h"

#define ASSERT_PROFILE(desc, path, expected) do { \
    const AppCompatProfile *actual = app_compat_select(path); \
    const AppCompatProfile *expected_profile = (expected); \
    if (actual != expected_profile) { \
        printf("  FAIL: %s: got %s, expected %s\n", desc, \
               actual ? actual->name : "(null)", \
               expected_profile ? expected_profile->name : "(null)"); \
        failures++; \
    } \
} while (0)

const AppCompatProfile app_compat_gghjt = {
    .name = "gghjt.mrp",
};

const AppCompatProfile app_compat_gxdzc = {
    .name = "gxdzc.mrp",
};

int test_app_compat_run_all(void) {
    int failures = 0;

    printf("[TEST] app compat profile selection accepts bare filenames\n");
    ASSERT_PROFILE("bare gghjt filename", "gghjt.mrp", &app_compat_gghjt);

    printf("[TEST] app compat profile selection accepts Unix paths\n");
    ASSERT_PROFILE("Unix gghjt path", "/tmp/mythroad/gghjt.mrp",
                   &app_compat_gghjt);

    printf("[TEST] app compat profile selection accepts Windows paths\n");
    ASSERT_PROFILE("Windows gghjt path",
                   "D:\\Github\\vmrp\\build\\Debug\\mythroad\\gghjt.mrp",
                   &app_compat_gghjt);

    printf("[TEST] app compat profile selection accepts mixed separators\n");
    ASSERT_PROFILE("mixed separator gxdzc path",
                   "D:\\Github\\vmrp/build\\Debug/mythroad\\gxdzc.mrp",
                   &app_compat_gxdzc);

    printf("[TEST] app compat profile selection ignores unknown apps\n");
    ASSERT_PROFILE("unknown app path", "D:\\Github\\vmrp\\foo.mrp", NULL);

    return failures;
}
