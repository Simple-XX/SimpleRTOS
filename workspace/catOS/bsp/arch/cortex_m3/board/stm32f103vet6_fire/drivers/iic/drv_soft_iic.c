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
#include "drv_soft_iic.h"
#include "cat_delay.h"
#include "cat_stdio.h"


#define SOFT_IIC_1_CONFIG \
{ \
    .scl = PIN('A', 5), \
    .sda = PIN('A', 7), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"soft_iic1", \
}

#define SOFT_IIC_2_CONFIG \
{ \
    .scl = PIN('B', 6), \
    .sda = PIN('B', 7), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"soft_iic2", \
}

cat_iic_bus_t soft_iic_1 = SOFT_IIC_1_CONFIG;
cat_iic_bus_t soft_iic_2 = SOFT_IIC_2_CONFIG;

void cat_iic_init(cat_iic_bus_t *bus)
{
    if(CAT_FALSE == bus->is_init)
    {
        cat_pin_init(bus->scl, CAT_PIN_MODE_OUTPUT);
        cat_pin_init(bus->sda, CAT_PIN_MODE_OUTPUT);
        CAT_KPRINTF("[iic] %s init \r\n", bus->name);
    }
    else
    {
        CAT_KPRINTF("[iic] warning: %s dup init ! \r\n", bus->name);
    }
}

void cat_iic_wave(cat_iic_bus_t *bus)
{
    cat_uint8_t i = 0;

    SDA_OUT(bus);

    for(i=0; i<50; i++)
    {
        SDA_H(bus);
        SCL_L(bus);
        cat_delay_us(bus->delay_us);
        SDA_L(bus);
        SCL_H(bus);
        cat_delay_us(bus->delay_us);
    }
}

void cat_iic_start(cat_iic_bus_t *bus)
{
    SDA_OUT(bus);

    SDA_H(bus);
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    SDA_L(bus);
    cat_delay_us(bus->delay_us);
    SCL_L(bus);
}
void cat_iic_stop(cat_iic_bus_t *bus)
{
    SDA_OUT(bus);

    SDA_L(bus);
    SCL_L(bus);
    cat_delay_us(bus->delay_us);
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    SDA_H(bus);
    cat_delay_us(bus->delay_us);
}

cat_bool_t cat_iic_wait_ack(cat_iic_bus_t *bus)
{
    cat_bool_t ack = CAT_TRUE;
    cat_uint32_t delayed_us;

#if 0
    SDA_H(bus);
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    SDA_IN(bus);

    while(1 == GET_SDA(bus))
    {
        cat_delay_us(bus->delay_us);
        delayed_us += bus->delay_us;
        if(delayed_us >= bus->timeout_us)
        {
            cat_iic_stop(bus);
            ack = CAT_FALSE;
            break;
        }
    }
    if(CAT_TRUE == ack)
    {
        SCL_L(bus);
    }
#else
    SDA_IN(bus)
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    ack = GET_SDA(bus);
    SCL_L(bus);
    cat_delay_us(bus->delay_us);
#endif

    return ack;
}

cat_err_t cat_iic_send_ack(cat_iic_bus_t *bus)
{
    SDA_OUT(bus);

    SDA_L(bus); /* send 0 */
    SCL_L(bus);
    cat_delay_us(bus->delay_us);
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    SCL_L(bus);
    // cat_delay_us(bus->delay_us);

    return CAT_EOK;
}
cat_err_t cat_iic_send_nack(cat_iic_bus_t *bus)
{
    SDA_OUT(bus);

    SDA_H(bus); /* send 1 */
    SCL_L(bus);
    cat_delay_us(bus->delay_us);
    SCL_H(bus);
    cat_delay_us(bus->delay_us);
    SCL_L(bus);
    // cat_delay_us(bus->delay_us);

    return CAT_EOK;
}

cat_bool_t cat_iic_send_byte(cat_iic_bus_t *bus, cat_uint8_t byte)
{
    cat_uint8_t i, data_bit, data_byte;
    cat_bool_t ack;

    SDA_OUT(bus);

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
        cat_delay_us(bus->delay_us);

        data_byte = data_byte << 1;

        SCL_H(bus);
        SCL_L(bus);
    }
    cat_delay_us(bus->delay_us);

    ack = cat_iic_wait_ack(bus);

    return ack;
}

cat_uint8_t cat_iic_read_byte(cat_iic_bus_t *bus)
{
	cat_uint8_t i = 0;
    cat_uint8_t data = 0;

    SDA_OUT(bus);

    SDA_H(bus);

    SDA_IN(bus);
    for(i=0; i<8; i++)
    {
        data <<= 1;
        SCL_H(bus);
        cat_delay_us(bus->delay_us);
        data |= GET_SDA(bus);
        SCL_L(bus);
        cat_delay_us(bus->delay_us);
    }
    return data;
}

void cat_iic_write_reg(cat_iic_bus_t *bus, cat_uint8_t device_addr, cat_uint8_t reg_addr, cat_uint8_t data)
{
    cat_iic_start(bus);
    cat_iic_send_byte(bus, device_addr);
    cat_iic_send_byte(bus, reg_addr);
    cat_iic_send_byte(bus, data);
    cat_iic_stop(bus);
}
cat_uint8_t cat_iic_read_reg(cat_iic_bus_t *bus, cat_uint8_t device_addr, cat_uint8_t reg_addr)
{
    cat_uint8_t data;

    cat_iic_start(bus);
    cat_iic_send_byte(bus, device_addr);
    cat_iic_send_byte(bus, reg_addr);
    cat_iic_start(bus);
    cat_iic_send_byte(bus, device_addr+1);
    data = cat_iic_read_byte(bus);
    cat_iic_send_ack(bus);
    cat_iic_stop(bus);

    return data;
}

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_string.h"
#include "cat_error.h"
void *do_iic_read(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint8_t data;
    cat_uint32_t device_addr, reg_addr;

    
    if(inst->buffer.arg_num != 2)
    {
        CAT_SYS_PRINTF("[iic_read] usage:iic_read [device_addr] [reg_addr]\r\n");
    }
    else
    {
        cat_htoi(&device_addr, inst->buffer.args[0]);
        cat_htoi(&reg_addr, inst->buffer.args[1]);

        data = cat_iic_read_reg(&soft_iic_2, device_addr, reg_addr);

        CAT_KPRINTF("iic[%x](%x) -> %x\r\n", device_addr, reg_addr, data);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_read, "", do_iic_read);

void *do_iic_write(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint8_t data;
    cat_uint32_t device_addr, reg_addr, val;

    
    if(inst->buffer.arg_num != 3)
    {
        CAT_SYS_PRINTF("[iic_read] usage:iic_read [device_addr] [reg_addr] [val]\r\n");
    }
    else
    {
        cat_htoi(&device_addr, inst->buffer.args[0]);
        cat_htoi(&reg_addr, inst->buffer.args[1]);
        cat_htoi(&val, inst->buffer.args[2]);

        cat_iic_write_reg(&soft_iic_2, device_addr, reg_addr, val);

        CAT_KPRINTF("%x -> iic[%x](%x) \r\n", val, device_addr, reg_addr);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_write, "", do_iic_write);

void *do_iic_start(void *arg)
{
    CAT_ASSERT(arg);

    cat_iic_start(&soft_iic_2);

    CAT_KPRINTF("[iic] start \r\n");

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_start, "", do_iic_start);

void *do_iic_stop(void *arg)
{
    CAT_ASSERT(arg);

    cat_iic_stop(&soft_iic_2);

    CAT_KPRINTF("[iic] stop \r\n");

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_stop, "", do_iic_stop);

/* send byte */
void *do_iic_sb(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t byte = 0x0;

    cat_htoi(&byte, inst->buffer.args[0]);

    
    if(inst->buffer.arg_num != 1)
    {
        CAT_SYS_PRINTF("[iic_send_byte] usage:iic_send_byte [byte]\r\n");
    }
    else
    {
        cat_iic_start(&soft_iic_2);
        cat_iic_send_byte(&soft_iic_2, byte);
        cat_iic_stop(&soft_iic_2);

        CAT_KPRINTF("[iic] send %x \r\n", byte);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_sb, "", do_iic_sb);

/* receive byte */
void *do_iic_rb(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint8_t byte = 0x0;

    if(inst->buffer.arg_num != 0)
    {
        CAT_SYS_PRINTF("[iic_send_byte] usage:iic_send_byte [byte]\r\n");
    }
    else
    {
        cat_iic_start(&soft_iic_2);
        byte = cat_iic_read_byte(&soft_iic_2);
        cat_iic_stop(&soft_iic_2);

        CAT_KPRINTF("[iic] receive %x \r\n", byte);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(iic_rb, "", do_iic_rb);

/* when sda high send byte */
void *do_h_sb(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t byte = 0x0;

    cat_htoi(&byte, inst->buffer.args[0]);

    SDA_OUT(&soft_iic_2);
    SDA_H(&soft_iic_2);

    SDA_IN(&soft_iic_2);
    
    if(inst->buffer.arg_num != 1)
    {
        CAT_SYS_PRINTF("[h_sb] usage:h_sb [byte]\r\n");
    }
    else
    {
        cat_iic_start(&soft_iic_2);
        cat_iic_send_byte(&soft_iic_2, byte);
        cat_iic_stop(&soft_iic_2);

        CAT_KPRINTF("[h_sb] send %x \r\n", byte);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(h_sb, "", do_h_sb);

/* when sda low send byte */
void *do_l_sb(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t byte = 0x0;

    cat_htoi(&byte, inst->buffer.args[0]);

    SDA_OUT(&soft_iic_2);
    SDA_L(&soft_iic_2);

    SDA_IN(&soft_iic_2);
    
    if(inst->buffer.arg_num != 1)
    {
        CAT_SYS_PRINTF("[l_sb] usage:l_sb [byte]\r\n");
    }
    else
    {
        cat_iic_start(&soft_iic_2);
        cat_iic_send_byte(&soft_iic_2, byte);
        cat_iic_stop(&soft_iic_2);

        CAT_KPRINTF("[l_sb] send %x \r\n", byte);
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(l_sb, "", do_l_sb);

#endif
