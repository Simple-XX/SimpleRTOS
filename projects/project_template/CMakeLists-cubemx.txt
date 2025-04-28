cmake_minimum_required(VERSION 3.22)

project(user_project)
add_library(user_project INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

set(CUBE_DIR src/cubemx_project)

add_subdirectory(src/port_${PORT_PACKAGE} port)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(user_project INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(user_project INTERFACE
    include
    ${CUBE_DIR}/Core/Inc
    ${CUBE_DIR}/Drivers/CMSIS/Include
    ${CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CUBE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
)

aux_source_directory(${CUBE_DIR}/Core/Src MX_Application_Src)
aux_source_directory(${CUBE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/ STM32_Drivers_Src)

target_sources(user_project INTERFACE
    ${MX_Application_Src}
    ${STM32_Drivers_Src}

    # ${TEST_DIR}/tests/ipc_test/basic_ipc_test.c
    # ${TEST_DIR}/tests/ipc_test/sem_test.c
    # ${TEST_DIR}/tests/ipc_test/mutex_test.c
    # ${TEST_DIR}/tests/ipc_test/mutex_prio_reverse_test.c
    # ${TEST_DIR}/tests/ipc_test/msg_queue_test.c
    # ${TEST_DIR}/tests/mem_test/mem_test.c

    # ${TEST_DIR}/tests/lib_test/list_test.c
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


