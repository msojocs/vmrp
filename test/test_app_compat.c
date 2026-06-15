#include <stdio.h>
#include <string.h>

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

static int write_test_mrp_header(const char *path, const char *internal_name) {
    unsigned char header[240];
    FILE *fp;
    size_t name_len;

    memset(header, 0, sizeof(header));
    memcpy(header, "MRPG", 4);
    header[4] = (unsigned char)sizeof(header);
    header[8] = (unsigned char)sizeof(header);
    name_len = strlen(internal_name);
    if (name_len > 12)
        name_len = 12;
    memcpy(header + 16, internal_name, name_len);

    fp = fopen(path, "wb");
    if (!fp)
        return -1;
    if (fwrite(header, 1, sizeof(header), fp) != sizeof(header)) {
        fclose(fp);
        return -1;
    }
    return fclose(fp) == 0 ? 0 : -1;
}

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

    printf("[TEST] app compat profile selection accepts id-prefixed filenames\n");
    ASSERT_PROFILE("prefixed gghjt filename", "018982845369-gghjt.mrp",
                   &app_compat_gghjt);

    printf("[TEST] app compat profile selection accepts id-prefixed Windows paths\n");
    ASSERT_PROFILE("prefixed Windows gghjt path",
                   "C:\\Users\\chenqiancheng\\Documents/mythroad\\018982845369-gghjt.mrp",
                   &app_compat_gghjt);

    printf("[TEST] app compat profile selection accepts renamed MRP header names\n");
    {
        const char *path = "test_vmrp_app_compat_renamed.mrp";
        if (write_test_mrp_header(path, "gghjt.mrp") != 0) {
            printf("  FAIL: unable to create test MRP header: %s\n", path);
            failures++;
        } else {
            ASSERT_PROFILE("renamed MRP with gghjt header", path,
                           &app_compat_gghjt);
        }
        remove(path);
    }

    printf("[TEST] app compat profile selection ignores unknown MRP header names\n");
    {
        const char *path = "test_vmrp_app_compat_unknown.mrp";
        if (write_test_mrp_header(path, "unknown.mrp") != 0) {
            printf("  FAIL: unable to create test MRP header: %s\n", path);
            failures++;
        } else {
            ASSERT_PROFILE("renamed MRP with unknown header", path, NULL);
        }
        remove(path);
    }

    printf("[TEST] app compat profile selection accepts mixed separators\n");
    ASSERT_PROFILE("mixed separator gghjt path",
                   "D:\\Github\\vmrp/build\\Debug/mythroad\\gghjt.mrp",
                   &app_compat_gghjt);

    printf("[TEST] app compat profile selection requires prefix separator\n");
    ASSERT_PROFILE("suffix without prefix separator",
                   "D:\\Github\\vmrp\\build\\Debug\\mythroad\\018982845369gghjt.mrp",
                   NULL);

    printf("[TEST] app compat profile selection ignores unknown apps\n");
    ASSERT_PROFILE("unknown app path", "D:\\Github\\vmrp\\foo.mrp", NULL);
    ASSERT_PROFILE("unknown prefixed app path",
                   "D:\\Github\\vmrp\\018982845369-foo.mrp", NULL);

    return failures;
}
