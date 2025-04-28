
# 1. 架构选择(必需)，这个主要是指定编译参数
set(USER_ARCH "cortex_m3")

# 2. 可移植包选择(必需)，这个是架构相关移植代码
set(PORT_PACKAGE cortex_m3)

# 3. 用户宏定义(可选)，例如stm32使用hal库需要定义 USE_HAL_DRIVER STM32F103xB 等
set(USER_COMPILE_DEFINITIONS
    USE_HAL_DRIVER 
    STM32F103xB
)


