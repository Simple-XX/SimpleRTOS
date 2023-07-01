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

#include "cat_pin.h"
#include "uart/cat_drv_uart.h"

#include "cat_init.h"

/**** 三方应用头文件 ****/
#if (CATOS_ENABLE_CMBACKTRACE == 1)
    #include "cm_backtrace.h"
#endif
/**
 * @brief 系统初始化
 */
void catos_init(void)
{
    /**********硬件相关初始化 */
    /* 板级硬件初始化 */
    cat_hw_init();

    /* 初始化设备框架 */
    cat_device_module_init();

    /* 注册串口 */
    cat_drv_uart_register();

    /* 设置标准输入输出使用的串口 */
    cat_stdio_set_device((cat_uint8_t *)CATOS_STDIO_DEVICE_NAME);

    /* 初始化引脚设备 */
    cat_pin_device_init();

    /********内核相关初始化 */
    /* 初始化调度器 */
    cat_scheduler_init();
    
    /* 初始化系统计数器 */
    cat_systick_init();

    /* 创建空闲任务 */
    cat_idle_task_create();

    /* 创建shell任务 */
    cat_shell_task_create();

    /********三方应用相关初始化 */
#if (CATOS_ENABLE_CMBACKTRACE == 1)
    cm_backtrace_init("catos", "0.0", "0.1");
#endif

    /* 禁止调度，若用户调用catos_hw_start_sched()，则在其中打开调度锁 */
    cat_sp_task_sched_disable();

#if 0 /* 为了兼容armcc的流程，由main主动调用catos_init()函数 
       * note: armcc的标准启动函数中先调用_main，在_main中处理一部分内容再跳转到main函数
       **/
    /* 跳转到用户主函数 */
    main();

    /* 若用户主函数未启动调度器则可能会返回，正常项目不应该运行到该循环中 */
    CAT_KPRINTF("user do not start the scheduler, main() returned\r\n");
    while(1)
    {

    }
#endif /* #if 0 */
    
}
