/**
 * @file cat_drv_uart.h
 * @brief uart串口驱动程序
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2022-07-31
 * Change Logs:
 * Date           Author        Notes
 * 2022-07-31     mio     first verion
 * 
 */

#include "catos_config.h"
#include "catos_types.h"

#include "cat_intr.h"

cat_uint8_t cat_drv_uart_register(void);

#if 0
cat_uint32_t cat_bsp_uart_init(void);

cat_uint32_t cat_bsp_uart_transmit(cat_uint8_t *data, cat_uint32_t size);
cat_uint32_t cat_bsp_uart_receive(cat_uint8_t *data, cat_uint32_t size);

cat_uint8_t cat_bsp_uart_transmit_byte(cat_uint8_t *ch);
cat_uint8_t cat_bsp_uart_receive_byte(cat_uint8_t *ch);
#endif
