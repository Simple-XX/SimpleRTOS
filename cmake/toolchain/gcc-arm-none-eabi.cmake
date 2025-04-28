
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
# set(TOOLCHAIN_PREFIX                E:/code/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-)
set(TOOLCHAIN_PREFIX                  arm-none-eabi-)

if(CMAKE_HOST_WIN32)
    set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc.exe)
    set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
    set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++.exe)
    set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++.exe)
    set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy.exe)
    set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size.exe)
else()
    set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
    set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
    set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)
endif()

message("toolchain is set to gcc-arm-none-eabi")

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
