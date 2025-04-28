cmake_minimum_required(VERSION 3.22)

project(port)
add_library(port INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(port INTERFACE 
    USE_HAL_DRIVER 
    STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(port INTERFACE
    include
    ${LIB_DIR}/CMSIS/Core/Include
    ${LIB_DIR}/CMSIS/Device/ST/STM32F1xx/Include
    ${LIB_DIR}/STM32F1xx_HAL_Driver
    ${LIB_DIR}/STM32F1xx_HAL_Driver/Inc
)

target_sources(port INTERFACE
    arch_xx_hw_c.c
    arch_xx_hw_s_gnu.s
    arch_xx_stack.c
    arch_xx_start.s # 如果用了cubemx的就不用编译这个
    # debug_uart.c    # cubemx开启uart1就不用编译这个
)

target_link_directories(port INTERFACE
)

target_link_libraries(port INTERFACE
)

# Validate that port code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


