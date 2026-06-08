/*
 * test_filelib.c — 验证 fileLib.c 的文件操作
 *
 * 策略：使用真实文件系统（tmpdir），测试 my_open/close/read/write/seek/info
 * 等函数的行为，包括边界条件和错误路径。
 * 链接真实的 fileLib.c + rbtree.c + utils.c。
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define HARNESS_MULTI_FILE
#include "harness.h"
#include "../src/include/fileLib.h"

/* vmrp_config 被 utils.h 间接使用（printScreen），提供一个定义 */
#include "../src/include/vmrp.h"
VmrpConfig vmrp_config = { .screen_width = 240, .screen_height = 320 };

/* ── 辅助函数 ─────────────────────────────────────────── */
static char test_tmpdir[256];
static char test_filepath[512];

static void setup_tmpdir(void) {
    snprintf(test_tmpdir, sizeof(test_tmpdir), "/tmp/vmrp_test_filelib_XXXXXX");
    char *result = mkdtemp(test_tmpdir);
    if (!result) {
        fprintf(stderr, "FATAL: mkdtemp failed\n");
        exit(1);
    }
}

static void cleanup_tmpdir(void) {
    /* 简单清理：删除已知文件，然后删除目录 */
    char cmd[600];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", test_tmpdir);
    system(cmd);
}

static void make_test_path(const char *name) {
    snprintf(test_filepath, sizeof(test_filepath), "%s/%s", test_tmpdir, name);
}

/* 创建一个带内容的临时文件 */
static void create_test_file(const char *name, const char *content) {
    make_test_path(name);
    FILE *f = fopen(test_filepath, "wb");
    if (f) {
        fwrite(content, 1, strlen(content), f);
        fclose(f);
    }
}

/* ── 测试用例 ─────────────────────────────────────────── */

static void test_open_and_close(void) {
    TEST_BEGIN("open and close a real file");
    create_test_file("test1.txt", "hello");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);  /* my_open 返回 0 表示失败 */

    int32_t rc = my_close(fd);
    ASSERT_INT_EQ(MR_SUCCESS, rc);
    TEST_END();
}

static void test_open_nonexistent_fails(void) {
    TEST_BEGIN("open nonexistent file returns 0 (failure)");
    make_test_path("nonexistent_file.txt");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_INT_EQ(0, fd);
    TEST_END();
}

static void test_close_invalid_fd(void) {
    TEST_BEGIN("close invalid fd returns MR_FAILED");
    int32_t rc = my_close(99999);
    ASSERT_INT_EQ(MR_FAILED, rc);
    TEST_END();
}

static void test_read_file_content(void) {
    TEST_BEGIN("read returns correct file content");
    create_test_file("read_test.txt", "VMRP");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);

    char buf[16] = {0};
    int32_t n = my_read(fd, buf, 4);
    ASSERT_INT_EQ(4, n);
    ASSERT_MEM_EQ("VMRP", buf, 4);

    my_close(fd);
    TEST_END();
}

static void test_read_partial(void) {
    TEST_BEGIN("read partial content (less than file size)");
    create_test_file("partial.txt", "ABCDEFGH");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);

    char buf[4] = {0};
    int32_t n = my_read(fd, buf, 3);
    ASSERT_INT_EQ(3, n);
    ASSERT_MEM_EQ("ABC", buf, 3);

    my_close(fd);
    TEST_END();
}

static void test_read_past_eof(void) {
    TEST_BEGIN("read past EOF returns actual bytes read");
    create_test_file("short.txt", "Hi");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);

    char buf[64] = {0};
    int32_t n = my_read(fd, buf, 64);
    ASSERT_INT_EQ(2, n);
    ASSERT_MEM_EQ("Hi", buf, 2);

    my_close(fd);
    TEST_END();
}

static void test_read_invalid_fd(void) {
    TEST_BEGIN("read from invalid fd returns MR_FAILED");
    char buf[16];
    int32_t n = my_read(99999, buf, sizeof(buf));
    ASSERT_INT_EQ(MR_FAILED, n);
    TEST_END();
}

static void test_write_and_readback(void) {
    TEST_BEGIN("write data and read it back");
    make_test_path("write_test.txt");

    int32_t fd = my_open(test_filepath, MR_FILE_CREATE | MR_FILE_RDWR);
    ASSERT_TRUE(fd > 0);

    const char *data = "test data 123";
    int32_t w = my_write(fd, (void *)data, strlen(data));
    ASSERT_INT_EQ((int32_t)strlen(data), w);

    my_close(fd);

    /* 重新打开并读回 */
    fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);

    char buf[32] = {0};
    int32_t r = my_read(fd, buf, 32);
    ASSERT_INT_EQ((int32_t)strlen(data), r);
    ASSERT_MEM_EQ(data, buf, strlen(data));

    my_close(fd);
    TEST_END();
}

static void test_seek_and_read(void) {
    TEST_BEGIN("seek then read from offset");
    create_test_file("seek_test.txt", "ABCDEFGH");

    int32_t fd = my_open(test_filepath, MR_FILE_RDONLY);
    ASSERT_TRUE(fd > 0);

    int32_t rc = my_seek(fd, 4, MR_SEEK_SET);
    ASSERT_INT_EQ(MR_SUCCESS, rc);

    char buf[8] = {0};
    int32_t n = my_read(fd, buf, 4);
    ASSERT_INT_EQ(4, n);
    ASSERT_MEM_EQ("EFGH", buf, 4);

    my_close(fd);
    TEST_END();
}

static void test_seek_invalid_fd(void) {
    TEST_BEGIN("seek on invalid fd returns MR_FAILED");
    int32_t rc = my_seek(99999, 0, MR_SEEK_SET);
    ASSERT_INT_EQ(MR_FAILED, rc);
    TEST_END();
}

static void test_info_file(void) {
    TEST_BEGIN("my_info returns MR_IS_FILE for regular file");
    create_test_file("info_test.txt", "x");

    int32_t info = my_info(test_filepath);
    ASSERT_INT_EQ(MR_IS_FILE, info);
    TEST_END();
}

static void test_info_dir(void) {
    TEST_BEGIN("my_info returns MR_IS_DIR for directory");
    int32_t info = my_info(test_tmpdir);
    ASSERT_INT_EQ(MR_IS_DIR, info);
    TEST_END();
}

static void test_info_nonexistent(void) {
    TEST_BEGIN("my_info returns MR_IS_INVALID for nonexistent path");
    make_test_path("no_such_file");
    int32_t info = my_info(test_filepath);
    ASSERT_INT_EQ(MR_IS_INVALID, info);
    TEST_END();
}

static void test_getLen(void) {
    TEST_BEGIN("my_getLen returns file size");
    create_test_file("len_test.txt", "12345");

    int32_t len = my_getLen(test_filepath);
    ASSERT_INT_EQ(5, len);
    TEST_END();
}

static void test_getLen_nonexistent(void) {
    TEST_BEGIN("my_getLen returns -1 for nonexistent file");
    make_test_path("no_such_file");
    int32_t len = my_getLen(test_filepath);
    ASSERT_INT_EQ(-1, len);
    TEST_END();
}

static void test_remove_file(void) {
    TEST_BEGIN("my_remove deletes a file");
    create_test_file("to_delete.txt", "bye");

    int32_t rc = my_remove(test_filepath);
    ASSERT_INT_EQ(MR_SUCCESS, rc);

    /* 确认文件不存在了 */
    int32_t info = my_info(test_filepath);
    ASSERT_INT_EQ(MR_IS_INVALID, info);
    TEST_END();
}

static void test_remove_nonexistent(void) {
    TEST_BEGIN("my_remove on nonexistent file returns MR_FAILED");
    make_test_path("ghost_file.txt");
    int32_t rc = my_remove(test_filepath);
    ASSERT_INT_EQ(MR_FAILED, rc);
    TEST_END();
}

static void test_rename_file(void) {
    TEST_BEGIN("my_rename moves a file");
    create_test_file("old_name.txt", "rename me");
    char old_path[512];
    snprintf(old_path, sizeof(old_path), "%s", test_filepath);

    make_test_path("new_name.txt");
    int32_t rc = my_rename(old_path, test_filepath);
    ASSERT_INT_EQ(MR_SUCCESS, rc);

    /* 新路径应该存在，旧路径不存在 */
    ASSERT_INT_EQ(MR_IS_FILE, my_info(test_filepath));
    ASSERT_INT_EQ(MR_IS_INVALID, my_info(old_path));
    TEST_END();
}

static void test_mkdir_and_rmdir(void) {
    TEST_BEGIN("my_mkDir + my_rmDir");
    make_test_path("sub_dir");

    int32_t rc = my_mkDir(test_filepath);
    ASSERT_INT_EQ(MR_SUCCESS, rc);
    ASSERT_INT_EQ(MR_IS_DIR, my_info(test_filepath));

    rc = my_rmDir(test_filepath);
    ASSERT_INT_EQ(MR_SUCCESS, rc);
    ASSERT_INT_EQ(MR_IS_INVALID, my_info(test_filepath));
    TEST_END();
}

static void test_mkdir_already_exists(void) {
    TEST_BEGIN("my_mkDir on existing dir returns MR_SUCCESS");
    make_test_path("existing_dir");
    my_mkDir(test_filepath);

    /* 重复创建不应失败 */
    int32_t rc = my_mkDir(test_filepath);
    ASSERT_INT_EQ(MR_SUCCESS, rc);

    my_rmDir(test_filepath);
    TEST_END();
}

static void test_opendir_readdir_closedir(void) {
    TEST_BEGIN("opendir/readdir/closedir lists directory contents");
    /* 在 tmpdir 里创建几个文件 */
    create_test_file("dir_a.txt", "a");
    create_test_file("dir_b.txt", "b");

    int32_t dh = my_opendir(test_tmpdir);
    ASSERT_TRUE(dh > 0);

    /* 遍历目录，至少应该看到 . .. 和我们创建的文件 */
    int count = 0;
    int found_a = 0, found_b = 0;
    char *name;
    while ((name = my_readdir(dh)) != NULL) {
        count++;
        if (strcmp(name, "dir_a.txt") == 0) found_a = 1;
        if (strcmp(name, "dir_b.txt") == 0) found_b = 1;
    }
    ASSERT_TRUE(count >= 2);
    ASSERT_TRUE(found_a);
    ASSERT_TRUE(found_b);

    int32_t rc = my_closedir(dh);
    ASSERT_INT_EQ(MR_SUCCESS, rc);
    TEST_END();
}

static void test_closedir_invalid(void) {
    TEST_BEGIN("closedir with invalid handle returns MR_FAILED");
    int32_t rc = my_closedir(99999);
    ASSERT_INT_EQ(MR_FAILED, rc);
    TEST_END();
}

static void test_vmrp_writeFile_and_readFile(void) {
    TEST_BEGIN("vmrp_writeFile + readFile round-trip");
    make_test_path("roundtrip.txt");

    const char *data = "round trip test data";
    vmrp_writeFile(test_filepath, (void *)data, strlen(data));

    /* 验证文件存在且大小正确 */
    ASSERT_INT_EQ(MR_IS_FILE, my_info(test_filepath));
    ASSERT_INT_EQ((int32_t)strlen(data), my_getLen(test_filepath));

    char *read_back = readFile(test_filepath);
    ASSERT_PTR_NOT_NULL(read_back);
    ASSERT_MEM_EQ(data, read_back, strlen(data));
    free(read_back);
    TEST_END();
}

/* ── 运行所有测试 ─────────────────────────────────────── */
int test_filelib_run_all(void) {
    int before = harness_tests_failed;

    setup_tmpdir();

    test_open_and_close();
    test_open_nonexistent_fails();
    test_close_invalid_fd();
    test_read_file_content();
    test_read_partial();
    test_read_past_eof();
    test_read_invalid_fd();
    test_write_and_readback();
    test_seek_and_read();
    test_seek_invalid_fd();
    test_info_file();
    test_info_dir();
    test_info_nonexistent();
    test_getLen();
    test_getLen_nonexistent();
    test_remove_file();
    test_remove_nonexistent();
    test_rename_file();
    test_mkdir_and_rmdir();
    test_mkdir_already_exists();
    test_opendir_readdir_closedir();
    test_closedir_invalid();
    test_vmrp_writeFile_and_readFile();

    cleanup_tmpdir();

    return harness_tests_failed - before;
}
