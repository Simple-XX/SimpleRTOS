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
/****** 1 INCLUDE START ******/
#include "cat_drv_uart.h"
#include "cat_ringbuffer.h"
#include "cat_device.h"
#include "port.h"

#include "hal_data.h"
/****** 1 INCLUDE END ********/
//使能宏定义
#if (CATOS_ENABLE_DEVICE_MODEL == 1)
/****** 2 DECLARE START ******/

/* 获取时钟数，用于等待超时 */
#define UART_GET_SYSTICK() CATOS_GET_SYSTICK()

struct _cat_ra6m5_fire_uart_config_t
{
    const cat_uint8_t  *name;
    cat_uint32_t        aval_mode;
};

struct _cat_ra6m5_fire_uart_private_data_t
{
    sci_uart_instance_ctrl_t *inst_ctrl_ptr;
    const uart_cfg_t         *uart_cfg_ptr;
    struct _cat_ra6m5_fire_uart_config_t *config;
    cat_ringbuffer_t         *p_ringbuffer;           /* 环形缓冲区结构体指针 */
    cat_uint32_t              ringbuffer_size;        /* 环形缓冲区容量(必须为2的n次方，且实际可使用数量为size-1) */
};

/* 私有函数声明 */
/**
 * @brief 串口发送一个字符
 * 
 * @param  dev              设备指针
 * @param  timeout          超时时间
 * @param  data             要发送的字符数据
 * @return cat_int8_t       成功失败
 */
static cat_int8_t ra6m5_uart_send_char(cat_device_t*dev, cat_uint32_t timeout, cat_uint8_t data);
static cat_uint32_t ra6m5_uart_send(cat_device_t*dev, uint32_t timeout, uint8_t const * const buffer, uint32_t const size);
static cat_uint32_t ra6m5_uart_recv(cat_device_t*dev, uint32_t timeout, uint8_t *buffer, uint32_t const size);
static cat_uint8_t uart_init(cat_device_t*dev);
static cat_uint32_t uart_read(cat_device_t*dev, cat_int32_t pos, void *buffer, cat_uint32_t size);
static cat_uint32_t uart_write(cat_device_t*dev, cat_int32_t pos, const void *buffer, cat_uint32_t size);
static cat_uint8_t uart_ctrl(cat_device_t*dev, int cmd, void *args);
/****** 2 DECLARE END ********/
/****** 3 DEFINE START ******/
/* 中断相关 */
/* 用于指示发送是否完成 */
#define UART4_RINGBUFFER_SIZE            32                     /**< uart4串口接收缓冲区大小 */
volatile bool uart4_send_complete_flag = false;                 /**< uart4串口发送结束中断标志 */
volatile bool uart4_receive_char = false;                       /**< uart4串口接收到字符中断标志 */

#if 0
volatile cat_uint8_t uart4_recv_buff[UART4_RINGBUFFER_SIZE] = {0}; /**< uart4串口接收缓冲区 */
volatile cat_uint8_t uart4_recv_buff_tail_idx = 0;                 /**< uart4串口接收缓冲区访问的尾索引 */
volatile cat_uint8_t uart4_recv_buff_head_idx = 0;                 /**< uart4串口接收缓冲区访问的头索引 */
#else
cat_ringbuffer_t uart4_rb;                                      /**< uart4环形缓冲区 */
cat_uint8_t uart4_buf_space[UART4_RINGBUFFER_SIZE] = {0};       /**< uart4缓冲区空间 */
#endif

/* uart4中断回调函数 */
void debug_uart4_callback(uart_callback_args_t *pargs)
{
    switch(pargs->event)
    {
        case UART_EVENT_RX_CHAR:
        {
#if 1
            cat_ringbuffer_put(&uart4_rb, pargs->data);
#else
            if(
                ((uart4_recv_buff_head_idx - uart4_recv_buff_tail_idx) & (UART4_RINGBUFFER_SIZE - 1)) != 
                (UART4_RINGBUFFER_SIZE - 1)
            )
            {
                /* 没满就放 */
                uart4_recv_buff[uart4_recv_buff_head_idx++] = pargs->data;
                uart4_recv_buff_head_idx = (uart4_recv_buff_head_idx + 1) & (UART4_RINGBUFFER_SIZE - 1);
            }
#endif
            uart4_receive_char = true;
        }
        default:
        {
            break;
        }
    }
}

static cat_int8_t ra6m5_uart_send_char(cat_device_t*dev, cat_uint32_t timeout, cat_uint8_t data)
{
    cat_int8_t ret = CAT_ERROR;
    struct _cat_ra6m5_fire_uart_private_data_t *private_data = NULL;

    /* 获取设备实例数据 */
    private_data = (struct _cat_ra6m5_fire_uart_private_data_t *)(dev->pri_data);

    sci_uart_instance_ctrl_t *p_ctrl = private_data->inst_ctrl_ptr;

    /* 将要发送的数据放进数据寄存器 */
    p_ctrl->p_reg->TDR = data;

    /* 等待发送完成或超时 */
    while(
        ((p_ctrl->p_reg->SSR_b.TEND) == 0) &&
        (0 != timeout)
    )
    {
        timeout--;
    }

    /* 未超时才成功 */
    if(0 != timeout)
    {
        ret = CAT_EOK;
    }

    return ret;
}

static cat_uint32_t ra6m5_uart_send(cat_device_t*dev, uint32_t timeout, uint8_t const * const buffer, uint32_t const size)
{
    (void)timeout;
    cat_uint32_t cnt = 0;
    cat_int8_t err = CAT_EOK;
    
    while(
        (CAT_EOK == err) &&
        (cnt < size)
    )
    {
        err = ra6m5_uart_send_char(dev, 0xffff, buffer[cnt]);
        cnt++;
    }

    /* 因为前面在一次 while 循环中无论发送是否成功 cnt 都会无条件加一，所以如果失败了就有一个多加上的计数 */
    if(CAT_ERROR == err)
    {
        cnt--;
    }

    return cnt;
}

static cat_uint32_t ra6m5_uart_recv(cat_device_t*dev, uint32_t timeout, uint8_t *buffer, uint32_t const size)
{
    cat_uint32_t  recv_buffer_idx = 0; /**< 串口接收缓冲区访问索引 */
    cat_uint32_t  err = CAT_ERROR;
    struct _cat_ra6m5_fire_uart_private_data_t *private_data = NULL;

    /* 获取设备实例数据 */
    private_data = (struct _cat_ra6m5_fire_uart_private_data_t *)(dev->pri_data);
    assert(NULL != private_data);

    /* 读取 */
    while(recv_buffer_idx != size)
    {
        while(
            (false == uart4_receive_char) &&
            (0 != timeout)
        )
        {
            timeout--;
        };

        if(0 == timeout)
        {
            break;
        }
        
        while(
            (cat_ringbuffer_is_empty(private_data->p_ringbuffer) == 0)
        )
        {
            /* 获取接收到的字符 */
#if 1
            err = cat_ringbuffer_get(private_data->p_ringbuffer, &(buffer[recv_buffer_idx++]));

            if(CAT_ERROR == err)
            {
                /* 获取失败，因为while条件判断过非空，所以出大问题 */
                while(1);
            }
#else
            if(uart4_recv_buff_tail_idx != uart4_recv_buff_head_idx)
            {
                buffer[recv_buffer_idx++] =  uart4_recv_buff[uart4_recv_buff_tail_idx++];
                uart4_recv_buff_tail_idx = (uart4_recv_buff_tail_idx + 1) & (UART4_RINGBUFFER_SIZE - 1);
            }
            
#endif

            if(recv_buffer_idx == size)
            {
                break;
            }
        }

        if(cat_ringbuffer_is_empty(private_data->p_ringbuffer))
        {
            /* 取完才改遍flag */
            uart4_receive_char = false;
        }
    }
    
    return recv_buffer_idx;
}

static cat_uint8_t uart_init(cat_device_t*dev)
{
    cat_uint8_t ret = CAT_EOK;

    /* 获取该设备私有数据 */
    struct _cat_ra6m5_fire_uart_private_data_t *private_data = (struct _cat_ra6m5_fire_uart_private_data_t *)(dev->pri_data);

#if 1
    /* 初始化设备的环形缓冲区 */
    cat_ringbuffer_init(
        (private_data->p_ringbuffer),
        uart4_buf_space,
        (private_data->ringbuffer_size)
    );
#else
    uart4_recv_buff_tail_idx = 0;
    uart4_recv_buff_head_idx = 0;
#endif

    /* 使用fsp库初始化串口 */
    fsp_err_t err = R_SCI_UART_Open(private_data->inst_ctrl_ptr, private_data->uart_cfg_ptr);

    assert(FSP_SUCCESS == err);

#if 0
    R_SCI_UART_Write(&g_uart4_ctrl, (const uint8_t *)"uart init\r\n", 11);
    while(false == uart4_send_complete_flag);
    uart4_send_complete_flag = false;
#else
    ra6m5_uart_send(dev, 0, "[uart] uart init\r\n", 18);
#endif

    return ret;
}

static cat_uint32_t uart_read(cat_device_t*dev, cat_int32_t pos, void *buffer, cat_uint32_t size)
{
    (void)pos;
    
    cat_uint32_t ret = ra6m5_uart_recv(dev, 0xffff, buffer, size);

    return ret;
}

static cat_uint32_t uart_write(cat_device_t*dev, cat_int32_t pos, const void *buffer, cat_uint32_t size)
{
    (void)pos;
    cat_uint32_t ret;
    
    ret = ra6m5_uart_send(dev, 0xffff, buffer, size);

    return ret;
}
static cat_uint8_t uart_ctrl(cat_device_t*dev, int cmd, void *args)
{
    return CAT_EOK;
}
/* 私有函数定义 END */


/* uart1 */
#define UART4_CONFIG \
{ \
    .name               = (const cat_uint8_t *)"debug_uart", \
    .aval_mode          = CAT_DEVICE_MODE_RDWR, \
}

struct _cat_ra6m5_fire_uart_config_t uart4_cfg_data = UART4_CONFIG;
struct _cat_ra6m5_fire_uart_private_data_t uart4_pri_data = {
    .inst_ctrl_ptr = &g_uart4_ctrl,
    .uart_cfg_ptr = &g_uart4_cfg,
    .p_ringbuffer   = &uart4_rb,
};

cat_device_t uart4_dev = {
    .type = CAT_DEVICE_TYPE_CHAR,
    .init = uart_init,
    .open = NULL,
    .close = NULL,
    .read = uart_read,
    .write = uart_write,
    .ctrl = uart_ctrl,

    .pri_data = &uart4_pri_data,
};

/* 挂载所有uart设备 */
cat_uint8_t cat_drv_uart_register(void)
{
    cat_uint8_t err = CAT_EOK;

    err = cat_device_register(
            &uart4_dev,
            uart4_cfg_data.name,
            uart4_cfg_data.aval_mode
            );


    if(CAT_EOK != err)
    {
        while(1);
    }

    return err;
}
/****** 3 DEFINE END ********/

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "cat_error.h"
void *do_test_uart4(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;
    cat_uint8_t       *p = NULL;
    cat_uint32_t       len = 0;

    if(inst->buffer.arg_num == 0)
    {
        uart_write(&uart4_dev, 0, "use: uart4 [str]\r\n", 18);
    }
    else
    {
        p = inst->buffer.args[0];
        while(*p != '\0')
        {
            uart_write(&uart4_dev, 0, p, 1);
            p++;
        }  
        uart_write(&uart4_dev, 0, "\r\n", 2);
    }

    return NULL;
}
CAT_DECLARE_CMD(uart4, use uart pring, do_test_uart4);
#endif
#endif /* #if (CATOS_ENABLE_DEVICE_MODEL == 1) */
