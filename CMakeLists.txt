cmake_minimum_required(VERSION 3.11)
project(vmrp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
function(vmrp_add_link_flags target flags)
  get_target_property(_vmrp_existing_flags ${target} LINK_FLAGS)
  if(NOT _vmrp_existing_flags)
    set(_vmrp_existing_flags "")
  endif()
  set_target_properties(${target} PROPERTIES LINK_FLAGS "${_vmrp_existing_flags} ${flags}")
endfunction()

if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /J /W3 /utf-8")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /J /W3 /utf-8")
  add_definitions(-DVMRP -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN)
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -DNETWORK_SUPPORT -DVMRP -funsigned-char")
endif()

option(VMRP_BUILD_SHARED "Build libvmrp shared library (no SDL)" OFF)
option(VMRP_BUILD_SHARED_ONLY "Only build the shared library, skip SDL/test/wasm targets" OFF)
if(MSVC)
  set(VMRP_BUILD_TESTS_DEFAULT OFF)
else()
  set(VMRP_BUILD_TESTS_DEFAULT ON)
endif()
option(VMRP_BUILD_TESTS "Build unit tests" ${VMRP_BUILD_TESTS_DEFAULT})
if(VMRP_BUILD_SHARED_ONLY)
  set(VMRP_BUILD_SHARED ON CACHE BOOL "" FORCE)
endif()
if(MSVC AND VMRP_BUILD_TESTS)
  message(FATAL_ERROR "VMRP_BUILD_TESTS is not supported with MSVC yet; test/stubs.c uses POSIX mmap.")
endif()

# --- zlib ---
if(MSVC)
  include(FetchContent)
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()

  macro(vmrp_fetchcontent_make_available name)
    if(COMMAND FetchContent_MakeAvailable)
      FetchContent_MakeAvailable(${name})
    else()
      FetchContent_GetProperties(${name})
      string(TOLOWER "${name}" _vmrp_fc_name)
      if(NOT ${_vmrp_fc_name}_POPULATED)
        FetchContent_Populate(${name})
        if(EXISTS "${${_vmrp_fc_name}_SOURCE_DIR}/CMakeLists.txt")
          add_subdirectory("${${_vmrp_fc_name}_SOURCE_DIR}" "${${_vmrp_fc_name}_BINARY_DIR}")
        endif()
      endif()
    endif()
  endmacro()

  FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
    GIT_SHALLOW TRUE
  )
  set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  vmrp_fetchcontent_make_available(zlib)
  FetchContent_GetProperties(zlib)
  # Make zlib headers findable for targets
  if(TARGET zlibstatic)
    target_include_directories(zlibstatic PUBLIC ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    if(NOT TARGET ZLIB::ZLIB)
      add_library(ZLIB::ZLIB ALIAS zlibstatic)
    endif()
  elseif(TARGET zlib)
    target_include_directories(zlib PUBLIC ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    if(NOT TARGET ZLIB::ZLIB)
      add_library(ZLIB::ZLIB ALIAS zlib)
    endif()
  endif()

  if(NOT VMRP_BUILD_SHARED_ONLY)
    # --- SDL2 (pre-built MSVC/VC package) ---
    FetchContent_Declare(
      SDL2_VC
      URL "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip"
    )
    vmrp_fetchcontent_make_available(SDL2_VC)
    FetchContent_GetProperties(SDL2_VC)
    set(SDL2_DIR "${sdl2_vc_SOURCE_DIR}/cmake")
    if(DEFINED CMAKE_WARN_DEPRECATED)
      set(_VMRP_CMAKE_WARN_DEPRECATED_DEFINED TRUE)
      set(_VMRP_CMAKE_WARN_DEPRECATED "${CMAKE_WARN_DEPRECATED}")
    else()
      set(_VMRP_CMAKE_WARN_DEPRECATED_DEFINED FALSE)
    endif()
    set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
    find_package(SDL2 REQUIRED CONFIG)
    if(_VMRP_CMAKE_WARN_DEPRECATED_DEFINED)
      set(CMAKE_WARN_DEPRECATED "${_VMRP_CMAKE_WARN_DEPRECATED}" CACHE BOOL "" FORCE)
    else()
      unset(CMAKE_WARN_DEPRECATED CACHE)
    endif()
    unset(_VMRP_CMAKE_WARN_DEPRECATED_DEFINED)
    unset(_VMRP_CMAKE_WARN_DEPRECATED)
  endif()
endif()

# --- Unicorn (submodule) ---
# 只编译 ARM 架构，静态链接，不构建示例和测试
set(UNICORN_ARCH "arm" CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(UNICORN_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(UNICORN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(UNICORN_INSTALL OFF CACHE BOOL "" FORCE)
if(ANDROID_ABI)
  get_filename_component(VMRP_ANDROID_TOOLCHAIN_BIN "${CMAKE_C_COMPILER}" DIRECTORY)
  find_program(VMRP_STRINGS_TOOL llvm-strings
    PATHS "${VMRP_ANDROID_TOOLCHAIN_BIN}"
    NO_DEFAULT_PATH)
  if(NOT VMRP_STRINGS_TOOL)
    find_program(VMRP_STRINGS_TOOL strings)
  endif()
  if(VMRP_STRINGS_TOOL)
    set(ENV{STRINGS} "${VMRP_STRINGS_TOOL}")
  endif()

  find_program(VMRP_PKG_CONFIG_TOOL pkg-config)
  if(VMRP_PKG_CONFIG_TOOL)
    set(ENV{PKG_CONFIG} "${VMRP_PKG_CONFIG_TOOL}")
  else()
    set(ENV{PKG_CONFIG} "true")
  endif()
endif()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/unicorn EXCLUDE_FROM_ALL)

set(VMRP_CORE_SOURCES
  ./src/vmrp.c
  ./src/utils.c
  ./src/rbtree.c
  ./src/network.c
  ./src/fileLib.c
)

set(VMRP_PLATFORM_SDL_SOURCES
  ./src/main.c
)

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
  ./src/mythroad/src/parser/mr_parser.c
  ./src/mythroad/src/parser/mr_lex.c
  ./src/mythroad/src/parser/mr_code.c
  ./src/mythroad/src/lib/mr_auxiliar.c
  ./src/mythroad/src/lib/mr_auxlib.c
  ./src/mythroad/src/lib/mr_baselib.c
  ./src/mythroad/src/lib/mr_iolib_target.c
  ./src/mythroad/src/lib/mr_socket_target.c
  ./src/mythroad/src/lib/mr_strlib.c
  ./src/mythroad/src/lib/mr_tablib.c
  ./src/mythroad/src/lib/mr_tcp_target.c
)
if(MSVC)
  set(VMRP_UNDEFINE_VMRP_OPTION "/UVMRP")
else()
  set(VMRP_UNDEFINE_VMRP_OPTION "-UVMRP")
endif()
set_source_files_properties(${VMRP_MYTHROAD_SOURCES} PROPERTIES COMPILE_OPTIONS "${VMRP_UNDEFINE_VMRP_OPTION}")

if(NOT VMRP_BUILD_SHARED_ONLY)
# --- Native 构建 ---
add_executable(vmrp
  ${VMRP_PLATFORM_SDL_SOURCES}
  ${VMRP_CORE_SOURCES}
  ./src/native_platform_memory.c
  ./src/native_dsm_funcs.c
  ./src/runtime_native_mythroad.c
  ./src/arm_ext_executor.c
  ${VMRP_MYTHROAD_SOURCES}
)
target_compile_definitions(vmrp PRIVATE VMRP_NATIVE VMRP_SDL_AUDIO TARGET_MOD MTK_MOD MR_PLAT_DRAWTEXT DSM_FULL TRUST_BINARIES)
target_include_directories(vmrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/unicorn/include)
if(MSVC)
  vmrp_add_link_flags(vmrp "/FORCE:MULTIPLE")
  target_include_directories(vmrp PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(vmrp unicorn ZLIB::ZLIB SDL2::SDL2main SDL2::SDL2 ws2_32)
  # Copy SDL2.dll next to the executable after build
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_SDL2_DLL "${sdl2_vc_SOURCE_DIR}/lib/x64/SDL2.dll")
  else()
    set(_SDL2_DLL "${sdl2_vc_SOURCE_DIR}/lib/x86/SDL2.dll")
  endif()
  add_custom_command(TARGET vmrp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_SDL2_DLL}" $<TARGET_FILE_DIR:vmrp>
  )
else()
  vmrp_add_link_flags(vmrp "-Wl,--allow-multiple-definition")
  # --no-as-needed 确保 libm 不被 GNU ld 按需跳过——unicorn 的内部静态库
  # （arm-softmmu）在链接顺序上排在 -lm 之后，需要其中的 sqrtf 等符号
  target_link_libraries(vmrp unicorn "-Wl,--no-as-needed" SDL2main SDL2 m pthread z)
endif()

# --- Wasm 构建（emcc）---
# 用法: cmake --build build --target vmrp-wasm
# 需要 emcc 在 PATH 中
find_program(EMCC emcc)
if(EMCC)
  set(VMRP_WASM_OUTPUT_DIR ${CMAKE_BINARY_DIR}/wasm)
  set(VMRP_WASM_OUTPUT ${VMRP_WASM_OUTPUT_DIR}/vmrp.js)

  set(VMRP_WASM_SOURCES
    ${VMRP_PLATFORM_SDL_SOURCES}
    ${VMRP_CORE_SOURCES}
    ./src/native_platform_memory.c
    ./src/native_dsm_funcs.c
    ./src/runtime_native_mythroad.c
    ./src/arm_ext_executor.c
    ${VMRP_MYTHROAD_SOURCES}
  )
  # 将相对路径转为绝对路径
  set(VMRP_WASM_ABS_SOURCES "")
  foreach(src ${VMRP_WASM_SOURCES})
    get_filename_component(abs_src ${src} ABSOLUTE)
    list(APPEND VMRP_WASM_ABS_SOURCES ${abs_src})
  endforeach()

  add_custom_command(
    OUTPUT ${VMRP_WASM_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${VMRP_WASM_OUTPUT_DIR}
    COMMAND ${EMCC} -Os -Wall
      -DNETWORK_SUPPORT -DVMRP -DVMRP_NATIVE -DVMRP_SDL_AUDIO
      -DTARGET_MOD -DMTK_MOD -DMR_PLAT_DRAWTEXT -DDSM_FULL -DTRUST_BINARIES
      -funsigned-char
      -I${CMAKE_CURRENT_SOURCE_DIR}/wasm/unicorn
      ${VMRP_WASM_ABS_SOURCES}
      ${CMAKE_CURRENT_SOURCE_DIR}/wasm/unicorn/libunicorn.a
      -s EXPORTED_FUNCTIONS="['_main','_malloc','_free']"
      -s ALLOW_MEMORY_GROWTH=1
      -s ALLOW_TABLE_GROWTH
      -s USE_SDL=2
      -s USE_ZLIB=1
      -s WASM=1
      -s FORCE_FILESYSTEM=1
      -o ${VMRP_WASM_OUTPUT}
    DEPENDS ${VMRP_WASM_ABS_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/wasm/unicorn/libunicorn.a
    COMMENT "Building vmrp for WebAssembly with emcc"
    VERBATIM
  )
  add_custom_target(vmrp-wasm DEPENDS ${VMRP_WASM_OUTPUT})
else()
  message(STATUS "emcc not found, vmrp-wasm target disabled")
endif()
endif() # NOT VMRP_BUILD_SHARED_ONLY

# --- Shared library (for Android/Flutter via dart:ffi) ---
if(VMRP_BUILD_SHARED)
  add_library(vmrp-shared SHARED
    ./src/vmrp_api.c
    ${VMRP_CORE_SOURCES}
    ./src/native_platform_memory.c
    ./src/native_dsm_funcs.c
    ./src/runtime_native_mythroad.c
    ./src/arm_ext_executor.c
    ${VMRP_MYTHROAD_SOURCES}
  )
  target_compile_definitions(vmrp-shared PRIVATE VMRP_NATIVE TARGET_MOD MTK_MOD MR_PLAT_DRAWTEXT DSM_FULL TRUST_BINARIES)
  target_include_directories(vmrp-shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/unicorn/include)
  set_target_properties(vmrp-shared PROPERTIES OUTPUT_NAME "vmrp")
  if(MSVC)
    vmrp_add_link_flags(vmrp-shared "/FORCE:MULTIPLE")
    target_link_libraries(vmrp-shared ZLIB::ZLIB unicorn ws2_32)
  else()
    vmrp_add_link_flags(vmrp-shared "-Wl,--allow-multiple-definition")
    target_link_libraries(vmrp-shared unicorn "-Wl,--no-as-needed" m z)
    set_target_properties(vmrp-shared PROPERTIES C_VISIBILITY_PRESET hidden)
  endif()
endif()

if(NOT VMRP_BUILD_SHARED_ONLY)
# --- 测试 ---
enable_testing()
if(VMRP_BUILD_TESTS)
  set(TEST_MYTHROAD_SOURCES ${VMRP_MYTHROAD_SOURCES})
  add_executable(test_vmrp
    test/test_main.c
    test/test_got_snapshot.c
    test/test_parser.c
    test/stubs.c
    ${TEST_MYTHROAD_SOURCES}
  )
  set_source_files_properties(${TEST_MYTHROAD_SOURCES} PROPERTIES COMPILE_OPTIONS "${VMRP_UNDEFINE_VMRP_OPTION}")
  target_compile_definitions(test_vmrp PRIVATE VMRP_NATIVE TARGET_MOD MTK_MOD MR_PLAT_DRAWTEXT DSM_FULL TRUST_BINARIES)
  target_include_directories(test_vmrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/unicorn/include)
  if(MSVC)
    target_link_libraries(test_vmrp ZLIB::ZLIB ws2_32)
    vmrp_add_link_flags(test_vmrp "/FORCE:MULTIPLE")
  else()
    target_link_libraries(test_vmrp m pthread z)
    vmrp_add_link_flags(test_vmrp "-Wl,--allow-multiple-definition")
  endif()
  add_test(NAME vmrp_unit_tests COMMAND test_vmrp)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
endif() # NOT VMRP_BUILD_SHARED_ONLY
