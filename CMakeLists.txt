cmake_minimum_required(VERSION 3.0.0)
project(vmrp VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-m32 -g -Wall -DNETWORK_SUPPORT -DVMRP")


add_executable(vmrp
  ./src/main.c
  ./src/bridge.c
  ./src/vmrp.c
  ./src/utils.c
  ./src/rbtree.c
  ./src/network.c
  ./src/memory.c
  ./src/fileLib.c
)

target_link_libraries(vmrp unicorn)
target_link_libraries(vmrp SDL2main SDL2 m pthread)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
