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


cat_uint32_t cat_bsp_uart_init(void);

cat_uint32_t cat_bsp_uart_transmit(cat_uint8_t *data, cat_uint32_t size);
cat_uint32_t cat_bsp_uart_receive(cat_uint8_t *data, cat_uint32_t size);

cat_uint8_t cat_bsp_uart_transmit_byte(cat_uint8_t *ch);
cat_uint8_t cat_bsp_uart_receive_byte(cat_uint8_t *ch);

