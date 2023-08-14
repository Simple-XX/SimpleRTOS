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

#define DBG_PRINT_WHEN_PIN_MODE_CHANGED
#undef  DBG_PRINT_WHEN_PIN_MODE_CHANGED

// #define PIN_CONFIG \
// { \
//     {0,      "GPIOB", GPIOB,  GPIO_PIN_5}, \
//     {0xffff, "NONE" , NULL,   0xff}, \
}

void gpioa_clk_enable(void);
void gpiob_clk_enable(void);
void gpioc_clk_enable(void);
void gpiod_clk_enable(void);
void gpioe_clk_enable(void);

#define PORT_CONFIG \
{ \
    {GPIOA, CAT_FALSE, gpioa_clk_enable}, \
    {GPIOB, CAT_FALSE, gpiob_clk_enable}, \
    {GPIOC, CAT_FALSE, gpioc_clk_enable}, \
    {GPIOD, CAT_FALSE, gpiod_clk_enable}, \
    {GPIOE, CAT_FALSE, gpioe_clk_enable}, \
}

// stm32f103vet6_fire_pin_t pin_map[] = PIN_CONFIG;

stm32f103vet6_fire_gpio_port_t port_map[] = PORT_CONFIG;

void cat_pin_device_init(void)
{
// #if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
//     __HAL_RCC_GPIOB_CLK_ENABLE();
//     CAT_KPRINTF("[pin init] GPIOB_CLK is enable\r\n");
// #endif
// #if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
//     __HAL_RCC_GPIOC_CLK_ENABLE();
//     CAT_KPRINTF("[pin init] GPIOC_CLK is enable\r\n");
// #endif
}

cat_err_t cat_pin_init(cat_uint32_t pin_num, cat_uint8_t mode)
{
    cat_err_t ret = CAT_ERROR;

#if 0
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
#else
            cat_uint8_t str[2] = "a";
            cat_uint32_t pin_offset = 0;

            GPIO_InitTypeDef  GPIO_InitStruct;

            /* 如果时钟没开先使能时钟 */
            if(CAT_FALSE == port_map[DRV_GET_PORT_OFFSET(pin_num)].is_clk_enable)
            {
                port_map[DRV_GET_PORT_OFFSET(pin_num)].enable_clk();
                port_map[DRV_GET_PORT_OFFSET(pin_num)].is_clk_enable = CAT_TRUE;
            }

            /*选择要控制的GPIO引脚*/															   
            GPIO_InitStruct.Pin = DRV_GET_PIN(pin_num);	

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
                HAL_GPIO_Init(
                    port_map[DRV_GET_PORT_OFFSET(pin_num)].addr, 
                    &GPIO_InitStruct
                );
                ret = CAT_EOK;

                str[0] = 'A' + DRV_GET_PORT_OFFSET(pin_num);

                for(pin_offset=0; pin_offset<=0xff; pin_offset++)
                {
                    if((1 << pin_offset) == DRV_GET_PIN(pin_num))
                    {
                        break;
                    }
                }

                CAT_KPRINTF("[pin] port_addr = %x\r\n", (cat_ubase_t)(port_map[DRV_GET_PORT_OFFSET(pin_num)].addr));
                CAT_KPRINTF("[pin] (pin_num:%x)GPIO%s->%d init\r\n", pin_num, str, pin_offset);
            }
#endif


    return ret;
}

void cat_pin_set_mode(cat_uint32_t pin_num, cat_uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.Pin = DRV_GET_PIN(pin_num);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (mode == CAT_PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    }
    // else if (mode == CAT_PIN_MODE_INPUT)
    else
    {
        /* input setting: not pull. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    }


    HAL_GPIO_Init(port_map[DRV_GET_PORT_OFFSET(pin_num)].addr, &GPIO_InitStruct);

    cat_uint8_t str[2] = "a";
    str[0] = 'A' + DRV_GET_PORT_OFFSET(pin_num);

    cat_uint32_t pin_offset = 0;
    for(pin_offset=0; pin_offset<=0xff; pin_offset++)
    {
        if((1 << pin_offset) == DRV_GET_PIN(pin_num))
        {
            break;
        }
    }

#ifdef DBG_PRINT_WHEN_PIN_MODE_CHANGED
    if (mode == CAT_PIN_MODE_OUTPUT)
    {
        CAT_KPRINTF("[pin_set_mode] set GPIO%s->%d to OUTPUT\r\n", str, pin_offset);
    }
    else
    {
        CAT_KPRINTF("[pin_set_mode] set GPIO%s->%d to INPUT \r\n", str, pin_offset);
    }
#endif

}


cat_uint8_t  cat_pin_read(cat_uint32_t pin_num)
{
    cat_uint8_t val;

#if 0
    cat_uint32_t idr = (port_map[DRV_GET_PORT_OFFSET(pin_num)].addr)->IDR;
    cat_uint32_t pin_mask = DRV_GET_PIN(pin_num);

    CAT_KPRINTF("pin_mask=%x, idr(%x)=%x\r\n", pin_mask, &(port_map[DRV_GET_PORT_OFFSET(pin_num)].addr)->IDR, idr);
#endif    

    if (((port_map[DRV_GET_PORT_OFFSET(pin_num)].addr)->IDR & DRV_GET_PIN(pin_num)) != 0)
    {
        val = CAT_PIN_HIGH;
    }
    else
    {
        val = CAT_PIN_LOW;
    }

    return val;
}


void cat_pin_write(cat_uint32_t pin_num, cat_uint8_t val)
{
#if 0
    stm32f103vet6_fire_pin_t *p = &(pin_map[0]);
    
    /* 遍历pin_map数组 */
    for(; p->pin_num!=0xffff; p++)
    {
        if(p->pin_num == pin_num)
        {
            HAL_GPIO_WritePin(p->port, p->gpio_pin, (GPIO_PinState)val);

            /* 写入结束结束循环 */
            break;
        } /* if */
    } /* for */
#elif 0
    HAL_GPIO_WritePin(
        (GPIO_TypeDef *)(port_map[DRV_GET_PORT_OFFSET(pin_num)].addr), 
        DRV_GET_PIN(pin_num),
        (GPIO_PinState)val
    );
#else
    if (val != CAT_PIN_LOW)
    {
        (port_map[DRV_GET_PORT_OFFSET(pin_num)].addr)->BSRR = DRV_GET_PIN(pin_num);
    }
    else
    {
        (port_map[DRV_GET_PORT_OFFSET(pin_num)].addr)->BSRR = (cat_uint32_t)DRV_GET_PIN(pin_num) << 16u;
    }
#endif
}

/* 使能时钟 */
void gpioa_clk_enable(void)
{
#if defined(__HAL_RCC_GPIOA_CLK_ENABLE)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOA_CLK is enable\r\n");
#endif
}
void gpiob_clk_enable(void)
{
#if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
    __HAL_RCC_GPIOB_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOB_CLK is enable\r\n");
#endif
}
void gpioc_clk_enable(void)
{
#if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
    __HAL_RCC_GPIOC_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOC_CLK is enable\r\n");
#endif
}
void gpiod_clk_enable(void)
{
#if defined(__HAL_RCC_GPIOD_CLK_ENABLE)
    __HAL_RCC_GPIOD_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOD_CLK is enable\r\n");
#endif
}
void gpioe_clk_enable(void)
{
#if defined(__HAL_RCC_GPIOE_CLK_ENABLE)
    __HAL_RCC_GPIOE_CLK_ENABLE();
    CAT_KPRINTF("[pin init] GPIOE_CLK is enable\r\n");
#endif
}

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "cat_string.h"
#include "cat_error.h"
void *do_rpin(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint8_t port;
    cat_int32_t pin_from_arg;
    cat_uint32_t pin_num;

    cat_uint8_t pin_val;

    cat_int32_t ret = CAT_EOK;

    
    if(inst->buffer.arg_num != 2)
    {
        CAT_SYS_PRINTF("[rpin] usage:rpin [PORT(A)] [PIN(0)]\r\n");
    }
    else
    {
        port = inst->buffer.args[0][0];

        if(
            (port > 'Z') ||
            (port < 'A')
        )
        {
            CAT_SYS_PRINTF("[rpin] invalid port:%s\r\n", inst->buffer.args[0]);
        }
        else
        {
            ret = cat_atoi(&pin_from_arg, inst->buffer.args[1]);

            if(
                (pin_from_arg < 0) ||
                (pin_from_arg > 0xff)
            )
            {
                CAT_SYS_PRINTF("[rpin] invalid pin:%s\r\n", inst->buffer.args[1]);
            }
            else
            {
                /* 获取pin_num */
                pin_num = DRV_PIN_NUM(port, (cat_uint8_t)pin_from_arg);

                /* 设置为输入模式 */
                cat_pin_set_mode(pin_num, CAT_PIN_MODE_INPUT);

                /* 读取引脚值 */
                pin_val = cat_pin_read(pin_num);

                if(pin_val != 0)
                {
                    CAT_KPRINTF("P%s%s -> HIGH\r\n", inst->buffer.args[0], inst->buffer.args[1]);
                }
                else
                {
                    CAT_KPRINTF("P%s%s -> LOW \r\n", inst->buffer.args[0], inst->buffer.args[1]);
                }
                
            }
        }
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(rpin, read pin, do_rpin);

void *do_wpin(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint8_t port;
    cat_int32_t pin_from_arg;
    cat_uint32_t pin_num;

    cat_int32_t ret = CAT_EOK;

    
    if(inst->buffer.arg_num != 3)
    {
        CAT_SYS_PRINTF("[wpin] usage:wpin [PORT(A)] [PIN(0)] [VAL(0/1)]\r\n");
    }
    else
    {
        port = inst->buffer.args[0][0];

        if(
            (port > 'Z') ||
            (port < 'A')
        )
        {
            CAT_SYS_PRINTF("[rpin] invalid port:%s\r\n", inst->buffer.args[0]);
        }
        else
        {
            ret = cat_atoi(&pin_from_arg, inst->buffer.args[1]);

            if(
                (pin_from_arg < 0) ||
                (pin_from_arg > 0xff)
            )
            {
                CAT_SYS_PRINTF("[rpin] invalid pin:%s\r\n", inst->buffer.args[1]);
            }
            else
            {
                /* 获取pin_num */
                pin_num = DRV_PIN_NUM(port, (cat_uint8_t)pin_from_arg);

                /* 设置引脚模式为输出 */
                cat_pin_set_mode(pin_num, CAT_PIN_MODE_OUTPUT);

                /* 判断参数中要写入的值是否是0 */
                if(inst->buffer.args[2][0] != '0')
                {
                    cat_pin_write(pin_num, CAT_PIN_HIGH);
                    CAT_KPRINTF("HIGH -> P%s%s\r\n", inst->buffer.args[0], inst->buffer.args[1]);
                }
                else
                {
                    cat_pin_write(pin_num, CAT_PIN_LOW);
                    CAT_KPRINTF("LOW  -> P%s%s \r\n", inst->buffer.args[0], inst->buffer.args[1]);
                }
                
            }
        }
    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(wpin, write pin, do_wpin);
#endif
