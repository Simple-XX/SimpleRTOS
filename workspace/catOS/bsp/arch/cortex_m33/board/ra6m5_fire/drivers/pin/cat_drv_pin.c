/**
 * @file cat_drv_pin.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 瑞萨ra6m5引脚驱动源文件
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
#include "cat_drv_pin.h"
#include "cat_stdio.h"

#define PIN_CONFIG \
{ \
    {0,      &g_ioport_ctrl,  BSP_IO_PORT_04_PIN_00}, \
    {1,      &g_ioport_ctrl,  BSP_IO_PORT_04_PIN_03}, \
    {2,      &g_ioport_ctrl,  BSP_IO_PORT_04_PIN_04}, \
    {0xffff, NULL,           0xff}, \
}

ra6m5_fire_pin_t pin_map[] = PIN_CONFIG;

void cat_pin_device_init(void)
{
    R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
}

cat_uint8_t cat_pin_init(cat_uint32_t pin_num, cat_uint8_t mode)
{
    cat_uint8_t ret = CAT_ERROR; 
    cat_pin_write(pin_num, CAT_PIN_LOW);
    return ret;
}

cat_uint8_t cat_pin_set_mode(cat_uint32_t pin_num, cat_uint8_t mode)
{
    cat_uint8_t ret = CAT_ERROR;
    CAT_KPRINTF("[pin] set mode now not support, abort!\r\n");
    return ret;
}


cat_int8_t  cat_pin_read(cat_uint32_t pin_num)
{
    cat_int8_t ret = CAT_ERROR;
    CAT_KPRINTF("[pin] read func now not support, abort!\r\n");
    return ret;
}


cat_uint8_t cat_pin_write(cat_uint32_t pin_num, cat_uint8_t val)
{
    cat_uint8_t ret = CAT_ERROR;
    ra6m5_fire_pin_t *p = &(pin_map[0]);
    
    /* 遍历pin_map数组 */
    for(; p->pin_num!=0xffff; p++)
    {
        if(p->pin_num == pin_num)
        {
            if(0 == val)
            {
                R_IOPORT_PinWrite(p->p_ctrl_ptr, p->gpio_pin, BSP_IO_LEVEL_LOW);
                ret = CAT_EOK;
            }
            else if(1 == val)
            {
                R_IOPORT_PinWrite(p->p_ctrl_ptr, p->gpio_pin, BSP_IO_LEVEL_HIGH);
                ret = CAT_EOK;
            }
            else
            {
                /* 非法值，之后打印错误信息 */
                ret = CAT_ERROR;
                while(1);
            }
            
            /* 写入结束结束循环 */
            break;
        } /* if */
    } /* for */

    return ret;
}

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "cat_string.h"
#include "cat_error.h"
void *do_led_ctrl(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t led_idx = 0;
    cat_uint32_t val     = 0;

    cat_int8_t  err = CAT_ERROR;

    if(inst->buffer.arg_num != 2)
    {
        CAT_SYS_PRINTF("[led] usage:led [LED] [VAL]\r\n");
        CAT_SYS_PRINTF("      LED = 1,2 (led 0 on use)\r\n");
        CAT_SYS_PRINTF("      VAL = 0,1\r\n");
    }
    else
    {
        err = cat_atoi(&led_idx, inst->buffer.args[0]);
        err = cat_atoi(&val,     inst->buffer.args[1]);

        if(CAT_EOK == err)
        {
            err = cat_pin_write(led_idx, val);

            if(CAT_EOK == err)
            {
                CAT_SYS_PRINTF("[led] success to write led%d to val%d\r\n", led_idx, val);
            }
            else
            {
                CAT_SYS_PRINTF("[led] fail to write led%d to val%d\r\n", led_idx, val);
            }
        }
        else
        {
            CAT_SYS_PRINTF("[led] invalid par led%s and val%s\r\n", inst->buffer.args[0], inst->buffer.args[1]);
        }
    }

    

    return NULL;
}
CAT_DECLARE_CMD(led, ctrl led, do_led_ctrl);
#endif
