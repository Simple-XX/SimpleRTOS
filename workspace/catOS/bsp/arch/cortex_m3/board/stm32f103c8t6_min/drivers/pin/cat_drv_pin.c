/**
 * @file cat_drv_pin.c
 * @brief stm32f103vet6_fire 引脚驱动源文件
 * @author jiayuanwen (648137125@qq.com)
 * @version 1.0
 * @date 2023-05-27
 * Change Logs:
 * Date           Author        Notes
 * 2023-05-27     jiayuanwen     first verion
 * 
 */
#include "cat_drv_pin.h"
#include "cat_stdio.h"

#define PIN_CONFIG \
{ \
    {0,      "GPIOC", GPIOC,  GPIO_PIN_13}, \
    {0xffff, "NONE" , NULL,   0xff}, \
}

stm32f103vet6_fire_pin_t pin_map[] = PIN_CONFIG;

void cat_pin_device_init(void)
{
#if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
    __HAL_RCC_GPIOB_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOB_CLK is enable\r\n");
#endif
#if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
    __HAL_RCC_GPIOC_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOC_CLK is enable\r\n");
#endif
}

cat_uint8_t cat_pin_init(cat_uint32_t pin_num, cat_uint8_t mode)
{
    cat_uint8_t ret = CAT_ERROR;
    stm32f103vet6_fire_pin_t *p = &(pin_map[0]);
    
    /* 遍历pin_map数组 */
    for(; p->pin_num!=0xffff; p++)
    {
        if(p->pin_num == pin_num)
        {
            GPIO_InitTypeDef  GPIO_InitStruct;

            /*选择要控制的GPIO引脚*/															   
            GPIO_InitStruct.Pin = p->gpio_pin;	

            switch (mode)
            {
            case CAT_PIN_MODE_OUTPUT:
                /*设置引脚的输出类型为推挽输出*/
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                /*设置引脚为上拉模式*/
                GPIO_InitStruct.Pull  = GPIO_PULLUP;
                /*设置引脚速率为高速 */   
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                break;
            // case CAT_PIN_MODE_INPUT:
            //     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            //     break;
            default:
                ret = CAT_EINVAL;
                CAT_KPRINTF("[pin] invalid pin mode specified, cancel init\r\n");
                break;
            }
            
            /* 配置没问题就可以用了 */
            if(ret != CAT_EINVAL)
            {
                /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
                HAL_GPIO_Init(p->port, &GPIO_InitStruct);
                ret = CAT_EOK;
            }

            /* 初始化结束结束循环 */
            break;
        } /* if */
    } /* for */

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
    cat_uint8_t ret = CAT_EINVAL;
    stm32f103vet6_fire_pin_t *p = &(pin_map[0]);
    
    /* 遍历pin_map数组 */
    for(; p->pin_num!=0xffff; p++)
    {
        if(p->pin_num == pin_num)
        {
            HAL_GPIO_WritePin(p->port, p->gpio_pin, (GPIO_PinState)val);

            ret = CAT_EOK;

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

void *do_write_pin(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t pin_num;
    cat_uint8_t val;

    cat_int32_t ret = 0;

    
    if(inst->buffer.arg_num == 0)
    {
        CAT_SYS_PRINTF("[pin] usage:write_pin [pin_num] [val:0/1]\r\n");
    }
    else
    {
        /* 从buffer获取引脚号和待写入值的字符串并转换为整型 */
        ret = cat_atoi(&pin_num, inst->buffer.args[0]);
        ret = cat_atoi(&val, inst->buffer.args[1]);

        CAT_KPRINTF("[pin] pin=%s, val=%s\r\n", inst->buffer.args[0], inst->buffer.args[1]);
        CAT_KPRINTF("[pin] pin=%d, val=%d\r\n", pin_num, val);

        /* 调用pin_write函数写入 */
        ret = cat_pin_write(pin_num, val);
        if(ret != CAT_EOK)
        {
            CAT_SYS_PRINTF("[pin] fail to write !!\r\n");
        }
        else
        {
            CAT_KPRINTF("[pin] wrote %d -> pin%d success\r\n", val, pin_num);
        }
    }

    return NULL;
}
CAT_DECLARE_CMD(write_pin, write pin val, do_write_pin);
#endif
