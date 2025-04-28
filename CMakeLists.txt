
# 要求的最小版本号
cmake_minimum_required(VERSION 3.22)

# 设置编译器
set(CMAKE_C_STANDARD 99)            # c标准
set(CMAKE_C_STANDARD_REQUIRED ON)   # 指明要求支持c标准
set(CMAKE_C_EXTENSIONS ON)          # c扩展(暂时不知道干啥的)

# 设置构建类型(默认调试模式)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 设置各个目录名称
set(WORKSPACE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})
set(CATOS_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/catos)
set(TEST_DIR        ${CATOS_DIR}/test)
set(CMAKE_RULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
set(OUTPUT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(PROJECTS_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/projects)

# 要编译的项目
message("building userproj ${USER_PROJECT}")
# 设置项目名称
# set(CMAKE_PROJECT_NAME CACHE STRING ${p}) # 好像传参进来后面会消失
set(CMAKE_PROJECT_NAME ${USER_PROJECT})

# 包含用户配置文件
# include("cmake/gcc-arm-none-eabi.cmake")

include ("${PROJECTS_DIR}/${USER_PROJECT}/user.cmake")
include ("${CMAKE_RULES_DIR}/config_parse.cmake")

# 重要！！ 这一步必须放在包含用户配置后面, 因为
# user.cmake中最终会包含定义工具链的配置, 如果
# 设置project()后再include(), 那么回来之后会认
# 为修改了项目原来的工具链配置, 就会重新执行, 然
# 而 用户定义的变量就会消失, 导致错误

# 设置项目信息
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

message("CMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME}")

# 导出编译过程用到的指令
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 使能c语言和汇编
enable_language(C ASM)

# 可执行文件
add_executable(${CMAKE_PROJECT_NAME})

# 添加子目录
add_subdirectory(${CATOS_DIR} catos)
add_subdirectory(${PROJECTS_DIR}/${CMAKE_PROJECT_NAME})

# 置链接库目录
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# 添加源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# 添加包含目录
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# 添加编译时宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)
# 添加用户宏定义，例如stm32使用hal库时的 USE_HAL_DRIVER 和 STM32F103xB
# message(user defined: ${USER_COMPILE_DEFINITIONS})
add_compile_definitions(${USER_COMPILE_DEFINITIONS})

# 添加链接库
target_link_libraries(${CMAKE_PROJECT_NAME}
    catos
    user_project

    # Add user defined libraries
)
