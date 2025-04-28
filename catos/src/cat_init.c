/**
 * @file cat_init.c
 * @author mio (648137125@qq.com)
 * @brief 初始化
 * @version 0.1
 * @date 2022-07-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "port.h"

#include "cat_device.h"
#include "cat_shell.h"
#include "cat_stdio.h"

#include "cat_task.h"
#include "cat_intr.h"
#include "cat_idle.h"

/* 用户实现调试串口设备并注册 */
cat_u8 cat_debug_uart_register(void);

cat_err cat_mem_init(void);
void cat_task_scheduler_init(void);
void cat_component_init(void);

/**
 * @brief 系统初始化
 */
void catos_init(void)
{
    /**********硬件相关初始化 */
    /* 动态内存堆初始化 */
    cat_mem_init();

    /* 初始化设备框架 */
    cat_device_module_init();

    /* 注册串口 */
    cat_debug_uart_register();

#if (CATOS_STDIO_ENABLE == 1)
    /* 设置标准输入输出使用的串口 */
    cat_stdio_set_device(CATOS_STDIO_DEVICE_NAME);
#endif

    /********内核相关初始化 */
    /* 初始化调度器 */
    cat_task_scheduler_init();
    
    /* 初始化系统计数器 */
    cat_systick_init();

    /* 创建空闲任务 */
    cat_idle_task_create();

    /* 组件初始化 */
    cat_component_init();

    /* 禁止调度，若用户调用catos_hw_start_sched()，则在其中打开调度锁 */
    cat_task_sched_lock();

#if 0 /* 为了兼容armcc的流程，由main主动调用catos_init()函数 
       * note: armcc的标准启动函数中先调用_main，在_main中处理一部分内容再跳转到main函数
       **/
    /* 跳转到用户主函数 */
    main();

    /* 若用户主函数未启动调度器则可能会返回，正常项目不应该运行到该循环中 */
    cat_kprintf("user do not start the scheduler, main() returned\r\n");
    while(1)
    {

    }
#endif /* #if 0 */
    
}
