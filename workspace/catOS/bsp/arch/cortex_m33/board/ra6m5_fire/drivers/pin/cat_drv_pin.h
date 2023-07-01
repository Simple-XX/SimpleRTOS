/**
 * @file cat_drv_pin.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 瑞萨ra6m5引脚驱动头文件
 * @version 0.1
 * @date 2023-06-06
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-06 <td>创建
 * </table>
 */
#ifndef CAT_DRV_PIN_H
#define CAT_DRV_PIN_H

#include "cat_pin.h"
#include "hal_data.h"

typedef struct _ra6m5_fire_pin_t
{
    cat_uint32_t             pin_num;
    ioport_instance_ctrl_t  *p_ctrl_ptr;
    const bsp_io_port_pin_t  gpio_pin;
}ra6m5_fire_pin_t;

#endif