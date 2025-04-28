cmake_minimum_required(VERSION 3.22)

# 设置c标准
# set(CMAKE_C_STANDARD 99)            # c标准
# set(CMAKE_C_STANDARD_REQUIRED ON)   # 指明要求支持c标准
# set(CMAKE_C_EXTENSIONS ON)          # c扩展(暂时不知道干啥的)
# # 设置c++标准
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

project(unit_test)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# 设置gtest和stub项目目录
set(gtest_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../googletest-1.16.0)
set(stub_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cpp-stub)

# 设置gtest可执行文件目录
set(gtest_out_dir ${CMAKE_BINARY_DIR}/external/gtest_out)

include(CMakeDependentOption)
include(GNUInstallDirs)

# 添加gtest目录和头文件路径
message(${gtest_dir})
# add_subdirectory(${gtest_dir} ${gtest_out_dir})
link_directories(${gtest_dir}/build/lib)
include_directories(${gtest_dir}/googletest/include)
include_directories(${gtest_dir}/googlemock/include)

# 包含stub头文件路径(无源文件)
include_directories(${stub_dir}/src)
if(CMAKE_HOST_WIN32)
    include_directories(${stub_dir}/src_win)
else()
include_directories(${stub_dir}/src_linux)
endif()

# 添加被测头文件目录
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../projects/demo/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(SRCS
    ../src/cat_device.c
    ../src/cat_idle.c
    ../src/cat_init.c
    ../src/cat_intr.c
    ../src/cat_ipc.c
    ../src/cat_lib.c
    ../src/cat_shell_cmds.c
    ../src/cat_shell_port.c
    ../src/cat_shell.c
    ../src/cat_stdio.c
    ../src/cat_task.c
)

# 添加测试头文件目录（桩函数）
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/stubs")
# 添加测试源文件目录
set(TEST_SRCS
    stubs/port_stub.c
    stubs/uart_stub.c
    cat_ipc_test.cpp
)

message(${TEST_SRCS})

add_executable(gtest_out ${SRCS} ${TEST_SRCS})
target_link_options(gtest_out
    PRIVATE
    "LINKER:--script=${CMAKE_SOURCE_DIR}/link.ld"
)

target_link_libraries(gtest_out
    PRIVATE
    gtest
    gtest_main
    gmock
    gmock_main
    pthread
)
