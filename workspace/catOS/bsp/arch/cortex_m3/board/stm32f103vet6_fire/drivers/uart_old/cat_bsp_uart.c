/**
 * @file cat_bsp_uart.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-05 1.0    amoigus             内容
 */

#include "cat_bsp_uart.h"
#include "../../../../../../Libraries/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include "../../../interface/port.h"


//串口1波特率
#define USART_1_BAUDRATE 115200

//名称
#define USART_1_NAME                    USART1

//引脚
#define USART_1_RX_GPIO_PORT            GPIOA
#define USART_1_RX_PIN                  GPIO_PIN_10
#define USART_1_TX_GPIO_PORT            GPIOA
#define USART_1_TX_PIN                  GPIO_PIN_9

//中断
#define USART_1_IRQHandler              USART1_IRQHandler
#define USART_1_IRQ                     USART1_IRQn

//使能宏定义
#define USART_1_CLK_ENABLE()            __HAL_RCC_USART1_CLK_ENABLE()
#define USART_1_RX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART_1_TX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

UART_HandleTypeDef UartHandle;

cat_uint32_t cat_bsp_uart_init(void)
{
    cat_uint32_t ret = CAT_EOK;

    UartHandle.Instance          = USART_1_NAME;
  
    UartHandle.Init.BaudRate     = USART_1_BAUDRATE;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
    HAL_UART_Init(&UartHandle);
   
    //__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
    //cat_bsp_uart_transmit("[bsp] uart init succeed!!\r\n", 27);
    

    return ret;
}

cat_uint32_t cat_bsp_uart_transmit(cat_uint8_t *data, cat_uint32_t size)
{
    cat_uint32_t ret = CAT_EOK;

    ret = HAL_UART_Transmit(&UartHandle, data, size, 0xffff);

    return ret;
}

cat_uint32_t cat_bsp_uart_receive(cat_uint8_t *data, cat_uint32_t size)
{
    cat_uint32_t ret = CAT_EOK;

    ret = HAL_UART_Receive(&UartHandle, data, size, 0x1000);

    return ret;
}

cat_uint8_t cat_bsp_uart_transmit_byte(cat_uint8_t *ch)
{
    HAL_UART_Transmit(&UartHandle, (cat_uint8_t *)ch, 1, 1000);
    return *ch;
}

cat_uint8_t cat_bsp_uart_receive_byte(cat_uint8_t *ch)
{
    HAL_UART_Receive(&UartHandle, (cat_uint8_t *)ch, 1, 1000);
    return *ch;
}

//会在HAL_UART_Init中被调用
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  USART_1_CLK_ENABLE();
  USART_1_RX_GPIO_CLK_ENABLE();
  USART_1_TX_GPIO_CLK_ENABLE();
  
/**USART1 GPIO Configuration    
  PA9     ------> USART1_TX
  PA10    ------> USART1_RX 
  */
  /* 设置tx引脚为复用  */
  GPIO_InitStruct.Pin = USART_1_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(USART_1_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 设置rx引脚为复用 */
  GPIO_InitStruct.Pin = USART_1_RX_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;
  HAL_GPIO_Init(USART_1_RX_GPIO_PORT, &GPIO_InitStruct); 
 
  //HAL_NVIC_SetPriority(USART_1_IRQ ,0,1);	
  //HAL_NVIC_EnableIRQ(USART_1_IRQ );		   
}
