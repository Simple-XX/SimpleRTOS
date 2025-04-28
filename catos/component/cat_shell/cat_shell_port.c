/**
 * @file cat_shell_port.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-08
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2021-06-10 <td>内容
 * <tr><td>v1.1 <td>文佳源 <td>2025-03-08 <td>修改为使用串口中断, 使用信号量进行通知
 * </table>
 */

#include "catos.h"

#if (CATOS_CAT_SHELL_ENABLE == 1)

#define RECV_BUFFER_SIZE    256

static cat_task_t shell_task; /**< 任务结构体 */
static cat_u8 shell_task_env[CATOS_SHELL_STACK_SIZE]; /**< 栈空间 */

static cat_sem_t        recv_sem; /**< 用于shell利用中断读取字符的信号量 */
static cat_ringbuffer_t recv_rb;  /**< 中断接收字符的环形缓冲区 */
static cat_u8           recv_rb_space[RECV_BUFFER_SIZE]; /**< 环形缓冲区空间 */

static cat_shell_instance_t port_shell_inst_1 = {0}; /**< shell实例结构体 */
static cat_shell_config_t shell_cfg = {0};           /**< shell配置结构体 */
static cat_u8 shell_space[512];                      /**< shell占用空间 */

/* shell任务入口, 在cat_shell.c中 */
void cat_shell_task_entry(void *arg);

/**
 * @brief 创建shell任务, 组件初始化时调用
 * 
 */
void cat_shell_task_create(void)
{
    cat_i32 ret = 0;

    /* 初始化信号量 */
    cat_sem_init(&recv_sem, 0, RECV_BUFFER_SIZE-1);
    /* 初始化环形缓冲区 */
    cat_ringbuffer_init(&recv_rb, recv_rb_space, RECV_BUFFER_SIZE);

    /* 将shellspace分配到各个成员 */
    shell_cfg.buffer = shell_space;
    shell_cfg.buf_size = 512;
    if(sizeof(shell_space) < CAT_BUF_SIZE * (CAT_MAX_HISTORY + 1))
    {
        CLOG_ERROR("[cat_shell_port] shell_space is not enough !");
        while(1);
    }

    /* 初始化shell */
    ret = cat_shell_init(&port_shell_inst_1, &shell_cfg);
    if(ret)
    {
        CLOG_ERROR("[cat_shell_port] cat_shell_init fail!");
        while(1);
    }

    /* 创建任务 */
    cat_task_create(
        "shell_task", 
        &shell_task, 
        cat_shell_task_entry, 
        &port_shell_inst_1, 
        CATOS_SHELL_TASK_PRIO, 
        shell_task_env, 
        CATOS_SHELL_STACK_SIZE
    );
    CLOG_INFO("[cat_shell_port] shell task created");
}

/**
 * @brief shell获取一个字符
 * 
 * @param  data             缓冲区
 * @return cat_i16 
 */
char cat_shell_port_getc(void)
{
    char c = 0;

    cat_sem_get(&recv_sem, IPC_TIMEOUT_FOREVER);
    
    cat_i32 err = cat_ringbuffer_get(&recv_rb, (cat_u8 *)&c);
    if(CAT_EOK != err)
    {
        cat_kprintf("got c=%x", c);
        CLOG_ERROR("[shell] ERROR: got sem but no data");
    }
    
    return c;
}

void cat_shell_port_putc(char c)
{
    cat_putchar(c);
}

/**
 * @brief 通知shell收到了一个字符(在接收中断中调用)
 * 
 */
void cat_shell_recv_char_notify(cat_u8 data)
{
    cat_i32 err = cat_ringbuffer_put(&recv_rb, data);

    if(CAT_EOK != err)
    {
        CLOG_WARNING("[shell] rb us full, data lost");
    }
    else
    {
        cat_sem_post(&recv_sem);
    }
}
#else
    void cat_shell_recv_char_notify(cat_u8 data)
    {
        (void)data;
    }
    /* 不使用就空函数 */
    void cat_shell_task_create(void)
    {
        cat_kprintf("[cat_shell] shell not used\r\n");
    }
#endif /* #if (CATOS_CAT_SHELL_ENABLE == 1) */
