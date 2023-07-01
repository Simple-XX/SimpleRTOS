/**
 * @file cat_drv_uart.c
 * @brief uart串口驱动程序
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2022-07-31
 * Change Logs:
 * Date           Author        Notes
 * 2022-07-31     mio     first verion
 * 
 */

#include "cat_drv_uart.h"
#include "cat_device.h"
#include "port.h"

#include "stm32f1xx.h"




struct _cat_stm32f103vet6_uart_config_t
{
    const cat_uint8_t  *name;
    cat_uint32_t        aval_mode;

    cat_uint32_t        instance_addr;

    cat_uint32_t        baudrate;
    cat_uint32_t        wordlen;
    cat_uint32_t        stopbits;
    cat_uint32_t        parity;
    cat_uint32_t        mode;
    cat_uint32_t        hw_flowctrl;
    cat_uint32_t        over_sampling;
};

struct _cat_stm32f103vet6_uart_private_data_t
{
    UART_HandleTypeDef                      *handle;
    struct _cat_stm32f103vet6_uart_config_t *config;
};

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

#if (CATOS_ENABLE_DEVICE_MODEL == 1)

/* 公共函数 */
cat_uint8_t uart_init(cat_device_t*dev)
{
    cat_uint8_t ret = CAT_EOK;

    /* 获取sthal库的uart初始化句柄 */
    UART_HandleTypeDef *handle_ptr = 
        ((struct _cat_stm32f103vet6_uart_private_data_t *)(dev->pri_data))->handle;
    /* 获取配置结构体 */
    struct _cat_stm32f103vet6_uart_config_t *config_ptr = 
        ((struct _cat_stm32f103vet6_uart_private_data_t *)(dev->pri_data))->config;

    handle_ptr->Instance          = (USART_TypeDef *)(config_ptr->instance_addr);
  
    handle_ptr->Init.BaudRate     = config_ptr->baudrate;
    handle_ptr->Init.WordLength   = config_ptr->wordlen;
    handle_ptr->Init.StopBits     = config_ptr->stopbits;
    handle_ptr->Init.Parity       = config_ptr->parity;
    handle_ptr->Init.Mode         = config_ptr->mode;
    handle_ptr->Init.HwFlowCtl    = config_ptr->hw_flowctrl;
    
    HAL_UART_Init(handle_ptr);
   
    //__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
    //cat_bsp_uart_transmit("[bsp] uart init succeed!!\r\n", 27);
    

    return ret;
}
// cat_uint8_t uart_open(cat_device_t*dev, cat_uint16_t oflag)
// {

// }
// cat_uint8_t uart_close(cat_device_t*dev)
// {

// }
cat_uint32_t uart_read(cat_device_t*dev, cat_int32_t pos, void *buffer, cat_uint32_t size)
{
    (void)pos;
    cat_uint32_t ret = HAL_UART_Receive(
                    ((struct _cat_stm32f103vet6_uart_private_data_t *)(dev->pri_data))->handle,
                    (cat_uint8_t *)buffer, 
                    size, 
                    0x1000
                );
    return ret;
}
cat_uint32_t uart_write(cat_device_t*dev, cat_int32_t pos, const void *buffer, cat_uint32_t size)
{
    (void)pos;
    UART_HandleTypeDef *handle_ptr = ((struct _cat_stm32f103vet6_uart_private_data_t *)(dev->pri_data))->handle;
    cat_uint32_t ret = HAL_UART_Transmit(
                    handle_ptr, 
                    (cat_uint8_t *)buffer, 
                    size, 
                    0x1000
                );
    return ret;
}
cat_uint8_t uart_ctrl(cat_device_t*dev, int cmd, void *args)
{
    return CAT_EOK;
}

/* uart1 */
#define UART1_CONFIG \
{ \
    .name               = (const cat_uint8_t *)"debug_uart", \
    .aval_mode          = CAT_DEVICE_MODE_RDWR, \
    .instance_addr      = (cat_uint32_t)USART1, \
    \
    .baudrate           = 115200, \
    .wordlen            = UART_WORDLENGTH_8B, \
    .stopbits           = UART_STOPBITS_1, \
    .parity             = UART_PARITY_NONE, \
    .mode               = UART_MODE_TX_RX, \
    .hw_flowctrl        = UART_HWCONTROL_NONE, \
    .over_sampling      = UART_OVERSAMPLING_16, \
}

struct _cat_stm32f103vet6_uart_config_t uart1_cfg_data = UART1_CONFIG;
struct _cat_stm32f103vet6_uart_private_data_t uart1_pri_data = {
    .handle = &UartHandle,
    .config = &uart1_cfg_data
};

cat_device_t uart1_dev = {
    .type = CAT_DEVICE_TYPE_CHAR,
    .init = uart_init,
    .open = NULL,
    .close = NULL,
    .read = uart_read,
    .write = uart_write,
    .ctrl = uart_ctrl,

    .pri_data = &uart1_pri_data
};

/* 挂载所有uart设备 */
cat_uint8_t cat_drv_uart_register(void)
{
    cat_uint8_t err = CAT_EOK;

    err = cat_device_register(
            &uart1_dev,
            uart1_cfg_data.name,
            uart1_cfg_data.aval_mode
            );

#if 0
    if(CAT_EOK == err)
    {
        err = cat_device_init(&uart1_dev);
    }
    if(CAT_EOK == err)
    {
        err = cat_device_open(&uart1_dev, CAT_DEVICE_MODE_RDWR);
    }
#endif

    if(CAT_EOK != err)
    {
        while(1);
    }

    return err;
}
#else /* #if (CATOS_ENABLE_DEVICE_MODEL == 1) */

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
#endif /* #if (CATOS_ENABLE_DEVICE_MODEL == 1) */

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
