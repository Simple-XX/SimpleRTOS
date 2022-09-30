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

uint8_t cat_drv_uart_register(void);

#if 0
uint32_t cat_bsp_uart_init(void);

uint32_t cat_bsp_uart_transmit(uint8_t *data, uint32_t size);
uint32_t cat_bsp_uart_receive(uint8_t *data, uint32_t size);

uint8_t cat_bsp_uart_transmit_byte(uint8_t *ch);
uint8_t cat_bsp_uart_receive_byte(uint8_t *ch);
#endif
