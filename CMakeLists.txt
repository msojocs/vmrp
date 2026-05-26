cmake_minimum_required(VERSION 3.5)
project(vmrp VERSION 0.1.0 LANGUAGES C CXX)

option(VMRP_USE_NATIVE_MYTHROAD "Use native x86 Mythroad instead of cfunction.ext" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -DNETWORK_SUPPORT -DVMRP -funsigned-char")

# --- Unicorn (submodule) ---
# 只编译 ARM 架构，静态链接，不构建示例和测试
set(UNICORN_ARCH "arm" CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(UNICORN_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(UNICORN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(UNICORN_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/unicorn EXCLUDE_FROM_ALL)

set(VMRP_COMMON_SOURCES
  ./src/main.c
  ./src/vmrp.c
  ./src/utils.c
  ./src/rbtree.c
  ./src/network.c
  ./src/fileLib.c
)

if(VMRP_USE_NATIVE_MYTHROAD)
  set(VMRP_MYTHROAD_SOURCES
    ./src/mythroad/mythroad.c
    ./src/mythroad/encode.c
    ./src/mythroad/mr_unzip.c
    ./src/mythroad/mr_base64.c
    ./src/mythroad/mr_inflate.c
    ./src/mythroad/mr_graphics.c
    ./src/mythroad/string.c
    ./src/mythroad/printf.c
    ./src/mythroad/other.c
    ./src/mythroad/strtol.c
    ./src/mythroad/strtoul.c
    ./src/mythroad/dsm.c
    ./src/mythroad/fixR9.c
    ./src/mythroad/md5.c
    ./src/mythroad/mem.c
    ./src/mythroad/mr_pluto.c
    ./src/mythroad/src/mr_api.c
    ./src/mythroad/src/mr_debug.c
    ./src/mythroad/src/mr_do.c
    ./src/mythroad/src/mr_dump.c
    ./src/mythroad/src/mr_func.c
    ./src/mythroad/src/mr_gc.c
    ./src/mythroad/src/mr_mem.c
    ./src/mythroad/src/mr_opcodes.c
    ./src/mythroad/src/mr_object.c
    ./src/mythroad/src/mr_state.c
    ./src/mythroad/src/mr_string.c
    ./src/mythroad/src/mr_table.c
    ./src/mythroad/src/mr_tm.c
    ./src/mythroad/src/mr_undump.c
    ./src/mythroad/src/mr_vm.c
    ./src/mythroad/src/mr_zio.c
    ./src/mythroad/src/parser/mr_noparser.c
    ./src/mythroad/src/lib/mr_auxiliar.c
    ./src/mythroad/src/lib/mr_auxlib.c
    ./src/mythroad/src/lib/mr_baselib.c
    ./src/mythroad/src/lib/mr_iolib_target.c
    ./src/mythroad/src/lib/mr_socket_target.c
    ./src/mythroad/src/lib/mr_strlib.c
    ./src/mythroad/src/lib/mr_tablib.c
    ./src/mythroad/src/lib/mr_tcp_target.c
  )
  set_source_files_properties(${VMRP_MYTHROAD_SOURCES} PROPERTIES COMPILE_OPTIONS "-UVMRP")
  add_executable(vmrp
    ${VMRP_COMMON_SOURCES}
    ./src/native_platform_memory.c
    ./src/native_dsm_funcs.c
    ./src/runtime_native_mythroad.c
    ./src/arm_ext_executor.c
    ${VMRP_MYTHROAD_SOURCES}
  )
  target_compile_definitions(vmrp PRIVATE VMRP_USE_NATIVE_MYTHROAD=1 VMRP_NATIVE TARGET_MOD MTK_MOD MR_PLAT_DRAWTEXT DSM_FULL TRUST_BINARIES)
else()
  add_executable(vmrp
    ${VMRP_COMMON_SOURCES}
    ./src/bridge.c
    ./src/memory.c
    ./src/runtime_arm_cfunction.c
  )
  target_compile_definitions(vmrp PRIVATE VMRP_USE_NATIVE_MYTHROAD=0)
endif()

target_include_directories(vmrp PRIVATE ${CMAKE_SOURCE_DIR}/third_party/unicorn/include)
if(VMRP_USE_NATIVE_MYTHROAD)
  target_link_options(vmrp PRIVATE -Wl,--allow-multiple-definition)
endif()
# --no-as-needed 确保 libm 不被 GNU ld 按需跳过——unicorn 的内部静态库
# （arm-softmmu）在链接顺序上排在 -lm 之后，需要其中的 sqrtf 等符号
target_link_libraries(vmrp unicorn "-Wl,--no-as-needed" SDL2main SDL2 m pthread z)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
