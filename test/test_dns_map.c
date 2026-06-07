/*
 * DNS map tests use network.c directly but avoid sockets. The stubs below
 * satisfy symbols from code paths that these tests do not exercise.
 */
#include "../src/include/network.h"

#include <stdio.h>

static int failures = 0;

#define ASSERT_INT(desc, actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("  FAIL: %s: got %d, expected %d\n", desc, (actual), (expected)); \
        failures++; \
    } \
} while (0)

uIntMap *uIntMap_search(struct rb_root *root, uint32_t key) {
    (void)root;
    (void)key;
    return NULL;
}

int uIntMap_insert(struct rb_root *root, uIntMap *obj) {
    (void)root;
    (void)obj;
    return 0;
}

uIntMap *uIntMap_delete(struct rb_root *root, uint32_t key) {
    (void)root;
    (void)key;
    return NULL;
}

struct rb_node *rb_first(const struct rb_root *root) {
    (void)root;
    return NULL;
}

int32_t bridge_dsm_network_cb(uc_engine *uc, uint32_t addr, int32_t p0, uint32_t p1) {
    (void)uc;
    (void)addr;
    (void)p0;
    (void)p1;
    return MR_FAILED;
}

static void test_dns_map_arrow(void) {
    printf("[TEST] DNS map with arrow separator\n");
    ASSERT_INT("configure arrow DNS map",
               my_configureDnsMap("Original.Example. -> 127.0.0.1"), MR_SUCCESS);
    ASSERT_INT("mapped domain resolves through fake domain",
               my_getHostByName(NULL, "original.example", NULL, NULL), 0x7F000001);
}

static void test_dns_map_equals_and_multiple_entries(void) {
    printf("[TEST] DNS map with equals separator and multiple entries\n");
    ASSERT_INT("configure multiple DNS maps",
               my_configureDnsMap("unused.example=unused.invalid;alias.example=127.0.0.1"),
               MR_SUCCESS);
    ASSERT_INT("mapped alias resolves through fake domain",
               my_getHostByName(NULL, "ALIAS.EXAMPLE.", NULL, NULL), 0x7F000001);
}

static void test_dns_map_rejects_invalid_entry(void) {
    printf("[TEST] DNS map rejects invalid entry\n");
    ASSERT_INT("invalid DNS map fails", my_configureDnsMap("missing-separator"), MR_FAILED);
    ASSERT_INT("empty DNS map clears entries", my_configureDnsMap(""), MR_SUCCESS);
}

int test_dns_map_run_all(void) {
    int before = failures;
    test_dns_map_arrow();
    test_dns_map_equals_and_multiple_entries();
    test_dns_map_rejects_invalid_entry();
    return failures - before;
}
