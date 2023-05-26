#!/bin/bash
#altera-usb-blaster.cfg   cmsis-dap.cfg  nds32-aice.cfg          raspberrypi2-native.cfg    ulink.cfg
#altera-usb-blaster2.cfg  dummy.cfg      opendous.cfg            rlink.cfg                  usb-jtag.cfg
#arm-jtag-ew.cfg          estick.cfg     openjtag.cfg            stlink-v1.cfg              usbprog.cfg
#at91rm9200.cfg           flashlink.cfg  osbdm.cfg               stlink-v2-1.cfg            vsllink.cfg
#buspirate.cfg            ftdi           parport.cfg             stlink-v2.cfg
#calao-usb-a9260.cfg      jlink.cfg      parport_dlc5.cfg        sysfsgpio-raspberrypi.cfg
#chameleon.cfg            jtag_vpi.cfg   raspberrypi-native.cfg  ti-icdi.cfg
#openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg


