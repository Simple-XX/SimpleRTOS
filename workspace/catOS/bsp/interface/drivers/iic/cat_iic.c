/**
 * @file cat_iic.c
 * @author 文佳源 (648137125@qq.com)
 * @brief iic总线驱动
 * @version 0.1
 * @date 2023-08-05
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-05 <td>新建
 * </table>
 */
#include "cat_iic.h"

void SCL_H(cat_iic_bus_t *bus)
{
    SET_SCL(bus, 1);
}
