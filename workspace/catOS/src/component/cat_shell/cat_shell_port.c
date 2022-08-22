/**
 * @file cat_shell_port.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-10
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-10 1.0    amoigus             内容
 */

#include "cat_task.h"
#include "cat_shell_port.h"
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

#if (CATOS_ENABLE_CAT_SHELL == 1)
struct _cat_task_t shell_task;
cat_stack_type_t shell_task_env[CATOS_SHELL_STACK_SIZE];

cat_shell_instance_t port_shell_inst_1 = {0};
cat_shell_config_t shell_cfg = {0};
uint8_t shell_space[512];



void cat_shell_task_create(void)
{
    int32_t ret = 0;

    /* 将shellspace分配到各个成员 */
    shell_cfg.buffer = shell_space;
    shell_cfg.buf_size = 512;
    if(sizeof(shell_space) < CAT_BUF_SIZE * (CAT_MAX_HISTORY + 1))
    {
        CAT_KPRINTF("[cat_shell_port:%d] shell_space is not enough !\r\n", __LINE__);
        while(1);
    }

    // for(i=0; i<CAT_MAX_HISTORY; i++)
    // {
    //     shell_cfg.historys[i] = shell_space + CAT_BUF_SIZE * i;
    // }

    ret = cat_shell_init(&port_shell_inst_1, &shell_cfg);
    if(ret)
    {
        CAT_KPRINTF("[cat_shell_port:%d] cat_shell_init fail!\r\n", __LINE__);
        while(1);
    }

    cat_sp_task_create(
        (const uint8_t *)"shell_task", 
        &shell_task, 
        cat_shell_task_entry, 
        NULL, 
        CATOS_SHELL_TASK_PRIO, 
        shell_task_env, 
        CATOS_SHELL_STACK_SIZE
    );
    CAT_SYS_PRINTF("[cat_shell_port] cat_shell init success \r\n");
}


int16_t cat_shell_port_getc(uint8_t *data)
{
    int16_t ret = 0;
    *data = CAT_SYS_GETCHAR();
    return ret;
}

int16_t cat_shell_port_putc(uint8_t data)
{
    int16_t ret = 0;

    CAT_SYS_PUTCHAR(data);

    return ret;
}
#else
    /* 不使用就空函数 */
    void cat_shell_task_create(void)
    {
        
    }
#endif /* #if (CATOS_ENABLE_CAT_SHELL == 1) */
