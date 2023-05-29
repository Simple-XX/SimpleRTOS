
CINCLUDE_FILE += catOS/bsp/interface
CINCLUDE_FILE += catOS/bsp/interface/drivers
CINCLUDE_FILE += catOS/bsp/arch/$(CATOS_BOARD_ARCH)/$(CATOS_BOARD_BSP)/drivers


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

subdir-y += bsp
subdir-y += src
subdir-$(CATOS_ENABLE_TEST) += test

#VERIFY_MSG = catOS:$(RELATIVE_PATH)


include $(RULE_DIR)/rules.mk