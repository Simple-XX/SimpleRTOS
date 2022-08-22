#ifndef KEY_H
#define KEY_H

#include "stm32f1xx.h"

#define KEY1_INT_GPIO_PORT GPIOA
#define KEY1_INT_GPIO_CLK_ENABLE() __GPIOA_CLK_ENABLE();
#define KEY1_INT_GPIO_PIN GPIO_PIN_0
#define KEY1_INT_EXTI_IRQ EXTI0_IRQn
#define KEY1_IRQHandler EXTI0_IRQHandler

void EXTI_Key_Config(void);

#endif