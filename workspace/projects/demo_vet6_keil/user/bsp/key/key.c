
#include "key.h"
#include "bsp_board_led.h"

void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 开启按键 GPIO 口的时钟 */
    KEY1_INT_GPIO_CLK_ENABLE();
    /* 选择按键 1 的引脚 */
    GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);
    /* 配置 EXTI 中断源 到 key1 引脚、配置中断优先级 */
    HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);
}

// void KEY1_IRQHandler(void)
// {
//     // 确保是否产生了 EXTI Line 中断
//     if (__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET) {
//         // LED1 取反
//         LED1_TOGGLE;
//         // 清除中断标志位
//         __HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);
//     }
// }
