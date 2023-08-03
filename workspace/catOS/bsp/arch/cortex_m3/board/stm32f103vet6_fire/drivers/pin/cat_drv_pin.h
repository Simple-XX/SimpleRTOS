/**
 * @file cat_drv_pin.h
 * @brief stm32f103vet6_fire 引脚驱动头文件
 * @author jiayuanwen (648137125@qq.com)
 * @version 1.0
 * @date 2023-05-27
 * Change Logs:
 * Date           Author        Notes
 * 2023-05-27     jiayuanwen     first verion
 * 
 */
#ifndef CAT_DRV_PIN_H
#define CAT_DRV_PIN_H

#include "cat_pin.h"
#include "stm32f1xx.h"

#define DRV_PIN_NUM(_port_name, _pin) ((((_port_name) - 'A') << 8) | (_pin))
#define DRV_GET_PIN(_pin_num) ((cat_uint8_t)(1 << ((_pin_num) & 0xff)))
#define DRV_GET_PORT_OFFSET(_pin_num) ((cat_uint8_t)(_pin_num >> 8))

typedef struct _stm32f103vet6_fire_gpio_port_t
{
    GPIO_TypeDef *  addr;
    cat_bool_t      is_clk_enable;
    void            (*enable_clk)(void);
}stm32f103vet6_fire_gpio_port_t;

// typedef struct _stm32f103vet6_fire_pin_t
// {
//     cat_uint32_t      pin_num;
//     char         *port_name;
//     GPIO_TypeDef *port;
//     cat_uint16_t      gpio_pin;
// }stm32f103vet6_fire_pin_t;

#endif