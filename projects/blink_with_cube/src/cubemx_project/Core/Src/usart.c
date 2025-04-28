/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
static uint8_t _uart_char_buffer = 0; 
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static cat_u8 uart1_init(cat_device_t *dev)
{
  (void)dev;

  /* 开始接受uart中断接收 */
  HAL_UART_Receive_IT(&huart1, &_uart_char_buffer, 1);

  return CAT_EOK;
}
static cat_u8 uart1_open(cat_device_t *dev, cat_u16 oflag)
{
  (void)dev;
  (void)oflag;
  return CAT_EOK;
}
static cat_u8 uart1_close(cat_device_t *dev)
{
  (void)dev;
  return CAT_EOK;
}
static cat_u32 uart1_read(cat_device_t *dev, cat_i32 pos, void *buffer, cat_u32 size)
{
  (void)dev;
  (void)pos;

  cat_u32 ret = HAL_UART_Receive(
      &huart1,
      (cat_u8 *)buffer,
      size,
      0x1000);

  return ret;
}
static cat_u32 uart1_write(cat_device_t *dev, cat_i32 pos, const void *buffer, cat_u32 size)
{
  (void)dev;
  (void)pos;
#if 0
    cat_u32 ret = HAL_UART_Transmit(
        &huart1,
        (cat_u8 *)buffer,
        size,
        0x1000);
    return ret;
#else
    cat_u32 i;
    for(i=0; i<size; i++)
    {
        cat_irq_disable();
        while((USART1->SR & USART_SR_TXE) == 0);

        USART1->DR = ((cat_u8 *)buffer)[i];
        cat_irq_enable();
    }

    return i;
#endif
}
static cat_u8 uart1_ctrl(cat_device_t *dev, int cmd, void *args)
{
  (void)dev;
  (void)cmd;
  (void)args;
  return CAT_EOK;
}
static cat_device_t uart1_dev =
{
  .type = CAT_DEVICE_TYPE_CHAR,
  .init = uart1_init,
  .open = uart1_open,
  .close = uart1_close,
  .read = uart1_read,
  .write = uart1_write,
  .ctrl = uart1_ctrl,

  .pri_data = CAT_NULL
};
cat_u8 cat_debug_uart_register(void)
{
  cat_u8 err = cat_device_register(
    &uart1_dev,
    "debug_uart",
    CAT_DEVICE_MODE_RDWR
  );

  CAT_ASSERT(CAT_EOK == err);

  return err;
}

/**
 * @brief 通知shell收到了一个字符(在接收中断中调用)
 * 
 */
void cat_shell_recv_char_notify(cat_u8 data);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(USART1 == huart->Instance)
    {

        cat_shell_recv_char_notify(_uart_char_buffer);

        HAL_UART_Receive_IT(huart, &_uart_char_buffer, 1);
    }
}
/* USER CODE END 1 */
