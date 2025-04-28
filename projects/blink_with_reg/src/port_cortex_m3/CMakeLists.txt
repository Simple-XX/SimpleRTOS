cmake_minimum_required(VERSION 3.22)

project(port)
add_library(port INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(port INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(port INTERFACE
    include
)

target_sources(port INTERFACE
    arch_cm3_hw_c.c
    arch_cm3_hw_s_gnu.s
    arch_cm3_stack.c
    arch_cm3_start.s
)

target_link_directories(port INTERFACE
)

target_link_libraries(port INTERFACE
)

# Validate that port code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


