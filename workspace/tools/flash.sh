#!/bin/bash
#openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c init -c halt -c "flash write_image erase output/demo/bin/demo.elf" -c reset -c shutdown
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c init -c halt -c "flash write_image erase ../output/demo/bin/demo.elf" -c reset -c shutdown
