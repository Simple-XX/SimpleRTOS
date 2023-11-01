
# This file is a part of Simple-XX/SimpleRTOS
# (https://github.com/Simple-XX/SimpleRTOS).
#
# CMakeLists.txt for Simple-XX/SimpleRTOS.

# 设置项目名与版本
project(
    unit-test
    VERSION 0.0.1
)

add_executable(unit_test
    example.cpp
)

target_compile_options(unit_test PRIVATE
    ${DEFAULT_TEST_COMPILE_OPTIONS}
)

target_link_options(unit_test PRIVATE
    ${DEFAULT_TEST_LINK_OPTIONS}
)

target_link_libraries(unit_test PRIVATE
    ${DEFAULT_TEST_LINK_LIB}
)

gtest_discover_tests(unit_test)
