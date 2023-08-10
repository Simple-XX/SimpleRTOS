/**
 * @file cat_iic.h
 * @author 文佳源 (648137125@qq.com)
 * @brief iic总线驱动头文件
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
#ifndef CAT_IIC_H
#define CAT_IIC_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_pin.h"

/* SET_SCL(&bus1, CAT_PIN_HIGH); */
#define SET_SCL(bus, val)    cat_pin_write(bus->scl, val)
#define SET_SDA(bus, val)    cat_pin_write(bus->sda, val)
#define GET_SCL(bus)    cat_pin_read(bus->scl)
#define GET_SDA(bus)    cat_pin_read(bus->sda)

#define SCL_L(bus)  SET_SCL(bus, CAT_PIN_LOW)
#define SDA_L(bus)  SET_SDA(bus, CAT_PIN_LOW)
#define SDA_H(bus)  SET_SDA(bus, CAT_PIN_HIGH)

typedef struct _cat_iic_bus_t
{
    cat_uint32_t scl;
    cat_uint32_t sda;
    cat_bool_t   is_init;
}cat_iic_bus_t;

typedef struct _cat_iic_client_t
{
    cat_uint32_t reserve;
}cat_iic_client_t;

void SCL_H(cat_iic_bus_t *bus);

void cat_iic_delay_us(cat_uint32_t us);

void cat_iic_init(cat_iic_bus_t *bus);

void cat_iic_start(cat_iic_bus_t *bus);
void cat_iic_stop(cat_iic_bus_t *bus);

void cat_iic_wait_ack(cat_iic_bus_t *bus);
void cat_iic_send_ack(cat_iic_bus_t *bus);
void cat_iic_send_nack(cat_iic_bus_t *bus);

void cat_iic_send_byte(cat_iic_bus_t *bus, cat_uint8_t byte);

cat_uint8_t cat_iic_read_byte(cat_iic_bus_t *bus);

#endif
