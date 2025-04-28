cmake_minimum_required(VERSION 3.22)

project(user_project)
add_library(user_project INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

add_subdirectory(src/port_${PORT_PACKAGE} port)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(user_project INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(user_project INTERFACE
    include
)

target_sources(user_project INTERFACE
    src/main.c
)

target_link_directories(user_project INTERFACE
)

target_link_libraries(user_project INTERFACE
    port
)

# Validate that user_project code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


