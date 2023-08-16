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
#include "drv_soft_iic.h"
#include "cat_delay.h"
#include "cat_stdio.h"

#define MPU6050_BUS (&soft_iic_2)

#define MPU_WRITE_REG(reg_addr, data) \
    cat_iic_write_reg(MPU6050_BUS, SlaveAddress, reg_addr, data)

#define MPU_READ_REG(reg_addr) \
    cat_iic_read_reg(MPU6050_BUS, SlaveAddress, reg_addr)

void cat_mpu6050_init(void)
{
    cat_uint8_t whoami = 0;

    cat_iic_init(MPU6050_BUS);

    cat_delay_ms(100);

    cat_iic_wave(MPU6050_BUS);
    
    MPU_WRITE_REG(PWR_MGMT_1, 0x00);	
    MPU_WRITE_REG(SMPLRT_DIV, 0x07);
    MPU_WRITE_REG(CONFIG, 0x06);
    MPU_WRITE_REG(GYRO_CONFIG, 0x18);
    MPU_WRITE_REG(ACCEL_CONFIG, 0x01);

    whoami = MPU_READ_REG(WHO_AM_I);

    CAT_KPRINTF("mpu6050 init, reg WHO_AM_I=%x\r\n", whoami);
}

cat_int32_t cat_mpu6050_get_data(cat_uint8_t reg_addr)
{
    cat_uint32_t data;
    cat_uint8_t data_h, data_l;

    data_h = MPU_READ_REG(reg_addr);
    data_l = MPU_READ_REG(reg_addr+1);

    data = ((data_h << 8) + data_l);

    if((data & 0x00008000) == 0x00008000)
    {
        data |= 0xffff0000;
    }

    return (cat_int32_t)data;
}