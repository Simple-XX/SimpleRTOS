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

typedef struct _stm32f103vet6_fire_pin_t
{
    cat_uint32_t      pin_num;
    char         *port_name;
    GPIO_TypeDef *port;
    cat_uint16_t      gpio_pin;
}stm32f103vet6_fire_pin_t;

#endif