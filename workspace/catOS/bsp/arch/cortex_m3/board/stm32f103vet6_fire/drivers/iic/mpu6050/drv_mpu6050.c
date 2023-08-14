/**
 * @file drv_mpu6050.c
 * @author 文佳源 (648137125@qq.com)
 * @brief mpu6050驱动
 * @version 0.1
 * @date 2023-08-10
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-10 <td>创建
 * </table>
 */
#include "drv_mpu6050.h"
#include "cat_delay.h"

#define MPU6050_BUS (&soft_iic_2)

void cat_mpu6050_init(void)
{

}

cat_uint32_t cat_mpu6050_get_data(cat_uint8_t reg_addr)
{
    
}