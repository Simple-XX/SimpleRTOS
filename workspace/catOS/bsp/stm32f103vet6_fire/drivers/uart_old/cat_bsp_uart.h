/**
 * @file cat_bsp_uart.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-05 1.0    amoigus             目前只留一个调试用串口，之后再扩展
 */

#include "catos_types.h"


uint32_t cat_bsp_uart_init(void);

uint32_t cat_bsp_uart_transmit(uint8_t *data, uint32_t size);
uint32_t cat_bsp_uart_receive(uint8_t *data, uint32_t size);

uint8_t cat_bsp_uart_transmit_byte(uint8_t *ch);
uint8_t cat_bsp_uart_receive_byte(uint8_t *ch);

