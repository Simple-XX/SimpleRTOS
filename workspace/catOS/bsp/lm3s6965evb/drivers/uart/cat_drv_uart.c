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

#define LM3S_HSI_FREQ            12000000UL

//volatile uint32_t * const UART0DR = (uint32_t *)0x4000C000;

typedef struct _lm2s_uart_t
{
    volatile uint16_t DR;
    uint16_t RESERVED0;
    volatile uint16_t RSR_ECR;
    uint16_t RESERVED1;
    volatile uint16_t FR;
    uint16_t RESERVED2;
    volatile uint16_t ILPR;
    uint16_t RESERVED3;
    volatile uint16_t IBRD;
    uint16_t RESERVED4;
    volatile uint16_t LCRH;
    uint16_t RESERVED5;
    volatile uint16_t CTL;
    uint16_t RESERVED6;
    volatile uint16_t IFLS;
    uint16_t RESERVED7;
    volatile uint16_t IM;
    uint16_t RESERVED8;
    volatile uint16_t RIS;
    uint16_t RESERVED9;
    volatile uint16_t MIS;
    uint16_t RESERVED10;
    volatile uint16_t ICR;
    uint16_t RESERVED11;
    volatile uint16_t PeriphID4;
    uint16_t RESERVED12;
    volatile uint16_t PeriphID5;
    uint16_t RESERVED13;
    volatile uint16_t PeriphID6;
    uint16_t RESERVED14;
    volatile uint16_t PeriphID7;
    uint16_t RESERVED15;
    volatile uint16_t PeriphID0;
    uint16_t RESERVED16;
    volatile uint16_t PeriphID1;
    uint16_t RESERVED17;
    volatile uint16_t PeriphID2;
    uint16_t RESERVED18;
    volatile uint16_t PeriphID3;
    uint16_t RESERVED19;
    volatile uint16_t PCellID0;
    uint16_t RESERVED20;
    volatile uint16_t PCellID1;
    uint16_t RESERVED21;
    volatile uint16_t PCellID2;
    uint16_t RESERVED22;
    volatile uint16_t PCellID3;
    uint16_t RESERVED23;
}lm2s_uart_t;

volatile lm2s_uart_t *const UART0DR = (lm2s_uart_t *)0x4000C000;

struct _cat_lm2s_uart_config_t
{
    const uint8_t  *name;
    uint32_t        aval_mode;

    uint32_t        instance_addr;

    uint32_t        baudrate;
    uint32_t        wordlen;
    uint32_t        stopbits;
    uint32_t        parity;
    uint32_t        mode;
    uint32_t        hw_flowctrl;
    uint32_t        over_sampling;
};

struct _cat_lm2s_uart_private_data_t
{
    struct _cat_lm2s_uart_config_t *config;
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

// UART_HandleTypeDef UartHandle;

#if (CATOS_ENABLE_DEVICE_MODEL == 1)

/* 公共函数 */
uint8_t uart_init(cat_device_t*dev)
{
    uint8_t ret = CAT_EOK;

    /** INIT **/
    /* 1. disable uart by clear UARTEN in UARTCTL */
    //UART0DR->CTL &= ~(1 << 0);
    /* 2. write integer portion of BRD to UARTIBRD */
    //uint32_t baud_rate = LM3S_HSI_FREQ / (16 * 115200);

    /* 3. write fractional portion to UARTFBRD */

    /* 4. write the disired serial para to UARTLCRH */

    /* 5. enable UART by set UARTEN in UARTCTL */

    /* 启用fifo */
    // UART0DR->LCRH |= (1 << 4);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    return ret;
}
// uint8_t uart_open(cat_device_t*dev, uint16_t oflag)
// {

// }
// uint8_t uart_close(cat_device_t*dev)
// {

// }
uint32_t uart_read(cat_device_t*dev, int32_t pos, void *buffer, uint32_t size)
{
    (void)pos;
    uint32_t ret = 0;

    if(1 != size)
    {
        while(1);
    }

    while(
        UART0DR->FR & (1 << 4)
    );

    *((uint8_t *)buffer) = (uint8_t)(UART0DR->DR & 0xff);

    return ret;
}
uint32_t uart_write(cat_device_t*dev, int32_t pos, const void *buffer, uint32_t size)
{
    (void)pos;
    uint32_t ret = 0;
    
    if(1 != size)
    {
        while(1);
    }

    while(
        UART0DR->FR & (1 << 5) /* trans reg is full */
    );

    // uint32_t status = cat_hw_irq_disable();
    UART0DR->DR = *((uint8_t *)buffer);
    // while(
    //     (0 != (UART0DR->FR & (1 << 3)))    /* UART is busy in transmiting */
    // );
    // cat_hw_irq_enable(status);


    return ret;
}
uint8_t uart_ctrl(cat_device_t*dev, int cmd, void *args)
{
    return CAT_EOK;
}

/* uart1 */
#define UART1_CONFIG \
{ \
    .name               = (const uint8_t *)"uart1", \
    .aval_mode          = 0, \
    .instance_addr      = (uint32_t)0, \
    \
    .baudrate           = 115200, \
    .wordlen            = 0, \
    .stopbits           = 0, \
    .parity             = 0, \
    .mode               = 0, \
    .hw_flowctrl        = 0, \
    .over_sampling      = 0, \
}

struct _cat_lm2s_uart_config_t uart1_cfg_data = UART1_CONFIG;
struct _cat_lm2s_uart_private_data_t uart1_pri_data = {};

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
uint8_t cat_drv_uart_register(void)
{
    uint8_t err = CAT_EOK;

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

uint32_t cat_bsp_uart_init(void)
{
    uint32_t ret = CAT_EOK;

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

uint32_t cat_bsp_uart_transmit(uint8_t *data, uint32_t size)
{
    uint32_t ret = CAT_EOK;

    ret = HAL_UART_Transmit(&UartHandle, data, size, 0xffff);

    return ret;
}

uint32_t cat_bsp_uart_receive(uint8_t *data, uint32_t size)
{
    uint32_t ret = CAT_EOK;

    ret = HAL_UART_Receive(&UartHandle, data, size, 0x1000);

    return ret;
}

uint8_t cat_bsp_uart_transmit_byte(uint8_t *ch)
{
    HAL_UART_Transmit(&UartHandle, (uint8_t *)ch, 1, 1000);
    return *ch;
}

uint8_t cat_bsp_uart_receive_byte(uint8_t *ch)
{
    HAL_UART_Receive(&UartHandle, (uint8_t *)ch, 1, 1000);
    return *ch;
}
#endif /* #if (CATOS_ENABLE_DEVICE_MODEL == 1) */

// //会在HAL_UART_Init中被调用
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
//   GPIO_InitTypeDef  GPIO_InitStruct;
  
//   USART_1_CLK_ENABLE();
//   USART_1_RX_GPIO_CLK_ENABLE();
//   USART_1_TX_GPIO_CLK_ENABLE();
  
// /**USART1 GPIO Configuration    
//   PA9     ------> USART1_TX
//   PA10    ------> USART1_RX 
//   */
//   /* 设置tx引脚为复用  */
//   GPIO_InitStruct.Pin = USART_1_TX_PIN;
//   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
//   GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
//   HAL_GPIO_Init(USART_1_TX_GPIO_PORT, &GPIO_InitStruct);
  
//   /* 设置rx引脚为复用 */
//   GPIO_InitStruct.Pin = USART_1_RX_PIN;
//   GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;
//   HAL_GPIO_Init(USART_1_RX_GPIO_PORT, &GPIO_InitStruct); 
 
//   //HAL_NVIC_SetPriority(USART_1_IRQ ,0,1);	
//   //HAL_NVIC_EnableIRQ(USART_1_IRQ );		   
// }
