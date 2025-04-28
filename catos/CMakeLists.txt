cmake_minimum_required(VERSION 3.22)

project(catos)
add_library(catos INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(catos INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(catos INTERFACE
    include
    component
    component/cat_shell
)

option(CATOS_TESTS "Include CatOS tests into the build" OFF)

set(catos_SOURCES
    src/cat_device.c
    src/cat_idle.c
    src/cat_init.c
    src/cat_intr.c
    src/cat_ipc.c
    src/cat_lib.c
    src/cat_log.c
    src/cat_mem.c
    src/cat_stdio.c
    src/cat_task.c

    component/cat_component.c
    component/cat_shell/cat_shell_cmds.c
    component/cat_shell/cat_shell_port.c
    component/cat_shell/cat_shell.c
    component/meoo/meoo.c
)

if(CATOS_TESTS)
    list(APPEND catos_SOURCES
        test/tests/ipc_test/basic_ipc_test.c
        test/tests/ipc_test/msg_queue_test.c
        test/tests/ipc_test/mutex_prio_reverse_test.c
        test/tests/ipc_test/mutex_test.c
        test/tests/ipc_test/sem_test.c

        test/tests/lib_test/list_test.c
        test/tests/lib_test/dlist_test.c

        test/tests/mem_test/mem_test.c
    )
endif()

target_sources(catos INTERFACE
    ${catos_SOURCES}
)

target_link_directories(catos INTERFACE
)

target_link_libraries(catos INTERFACE
)

# Validate that catos code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


