/**
 * @file drv_soft_iic.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 软件iic驱动实现源文件
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

#include "cat_delay.h"
#include "drv_soft_iic.h"

#define SOFT_IIC_1_CONFIG \
{ \
    .scl = PIN('A', 5), \
    .sda = PIN('A', 7), \
    .is_init = CAT_FALSE, \
}

cat_iic_bus_t soft_iic_1 = SOFT_IIC_1_CONFIG;


void cat_iic_delay_us(cat_uint32_t us)
{
    cat_delay_us(us);
}

void cat_iic_init(cat_iic_bus_t *bus)
{
    cat_pin_init(bus->scl, CAT_PIN_MODE_OUTPUT);
    cat_pin_init(bus->sda, CAT_PIN_MODE_OUTPUT);
}

void cat_iic_start(cat_iic_bus_t *bus)
{
    SCL_H(bus);
    SDA_H(bus);
    SDA_L(bus);
    SCL_L(bus);
}
void cat_iic_stop(cat_iic_bus_t *bus)
{
    SCL_H(bus);
    SDA_L(bus);
    SDA_H(bus);
}

void cat_iic_wait_ack(cat_iic_bus_t *bus)
{
    SCL_H(bus);
    SCL_L(bus);
}
void cat_iic_send_ack(cat_iic_bus_t *bus)
{

}
void cat_iic_send_nack(cat_iic_bus_t *bus)
{

}

void cat_iic_send_byte(cat_iic_bus_t *bus, cat_uint8_t byte)
{
    cat_uint8_t i, data_bit, data_byte;

    data_byte = byte;
    SCL_L(bus);

    for(i=0; i<8; i++)
    {
        data_bit = data_byte;
        data_bit &= 0x80;      /* 取得最高位 */

        if(0x80 == data_bit)
        {
            SDA_H(bus);
        }
        else
        {
            SDA_L(bus);
        }

        data_byte = data_byte << 1;

        SCL_H(bus);
        SCL_L(bus);
    }
}

cat_uint8_t cat_iic_read_byte(cat_iic_bus_t *bus)
{
	return 0;
}
