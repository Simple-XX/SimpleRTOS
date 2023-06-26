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
    {0,      "GPIOB", GPIOB,  GPIO_PIN_5}, \
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
    cat_uint8_t ret = CAT_ERROR;
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

    return ret;
}
