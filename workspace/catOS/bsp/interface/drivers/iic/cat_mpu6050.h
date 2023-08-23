/**
 * @file cat_iic_mpu6050.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-10
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-10 <td>内容
 * </table>
 */
#ifndef CAT_MPU6050_H
#define CAT_MPU6050_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_iic.h"


void cat_mpu6050_init(cat_iic_bus_t *bus);

cat_int32_t cat_mpu6050_get_data(cat_uint8_t reg_addr);
cat_err_t cat_mpu6050_get_angle(cat_float_t *pitch_ptr, cat_float_t *roll_ptr, cat_float_t *yaw_ptr);

#endif
