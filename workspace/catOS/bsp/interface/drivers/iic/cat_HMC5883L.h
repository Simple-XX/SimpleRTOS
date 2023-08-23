/**
 * @file cat_hmc5883l.h
 * @author 文佳源 (648137125@qq.com)
 * @brief HMC5883L 驱动头文件
 * @version 0.1
 * @date 2023-08-22
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-22 <td>内容
 * </table>
 */
#ifndef CAT_HMC5883L_H
#define CAT_HMC5883L_H

#include "catos.h"

void hmc_init(cat_iic_bus_t *bus);

void hmc_self_test(cat_iic_bus_t *bus);

void hmc_get_all_data(cat_float_t *x, cat_float_t *z, cat_float_t *y);

#endif