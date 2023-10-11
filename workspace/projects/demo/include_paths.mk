
CINCLUDE_FILE += catOS/include

CINCLUDE_FILE += catOS/src/component/cat_device
CINCLUDE_FILE += catOS/src/component/cat_shell
CINCLUDE_FILE += catOS/src/component/cat_stdio
CINCLUDE_FILE += catOS/src/component/cat_string

CINCLUDE_FILE += catOS/src/core/idle_task
CINCLUDE_FILE += catOS/src/core/init
CINCLUDE_FILE += catOS/src/core/interrupt
CINCLUDE_FILE += catOS/src/core/schedule
CINCLUDE_FILE += catOS/src/core/support

CINCLUDE_FILE += user

CINCLUDE_FILE += catOS/bsp/interface
CINCLUDE_FILE += catOS/bsp/interface/drivers
CINCLUDE_FILE += catOS/bsp/interface/drivers/iic

# CINCLUDE_FILE += catOS/bsp/arch/cortex_m3/board/stm32f103c8t6_min/drivers/pin

CINCLUDE_FILE += catOS/bsp/arch/$(CATOS_BOARD_ARCH)/board/$(CATOS_BOARD_BSP)/drivers

CINCLUDE_FILE += catOS/bsp/Libraries/CMSIS/Include
CINCLUDE_FILE += catOS/bsp/Libraries/CMSIS/Device/ST/STM32F1xx/Include
CINCLUDE_FILE += catOS/bsp/Libraries/STM32F1xx_HAL_Driver/Inc
CINCLUDE_FILE += catOS/bsp/Libraries/STM32F1xx_HAL_Driver