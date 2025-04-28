
# set (ARCH_LIST "")

set(TARGET_ARCH_DIR ${WORKSPACE_DIR}/cmake/target_arch)
set(TOOL_CHAIN_DIR ${WORKSPACE_DIR}/cmake/toolchain)

set(USER_ARCH "" CACHE STRING "Choose arch" FORCE)

if(NOT USER_ARCH)
    message(FATAL_ERROR "Please provide a value for USER_ARCH ")
endif()

message("use arch ${USER_ARCH}")

include("${TARGET_ARCH_DIR}/${USER_ARCH}/${USER_ARCH}.cmake")