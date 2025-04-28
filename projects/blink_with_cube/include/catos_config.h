/**
 * @file cat_config.h
 * @brief the config MACROs
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-19
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-19 1.0    amoigus             内容
 */

#ifndef CATOS_CONFIG_H
#define CATOS_CONFIG_H

/** 版本和构建信息 **/
/* catos版本 */
#define CATOS_VERSION               "v3.0.0"
/* 构建工具信息 */
#if defined(__CC_ARM)
    #define CATOS_BUILD_COMPILER    "armcc under 6"
#elif (defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))
    #define CATOS_BUILD_COMPILER    "armcc6(AC6)"
#elif defined(__GNUC__)
    #define CATOS_BUILD_COMPILER    "gcc(GNU)"
#endif

/** 系统相关 **/
#define CATOS_CLOCK_FRQ              72000000 /**< 系统时钟频率(systick挂在总线的时钟频率, 用于设置时钟中断重装载值 */
#define CATOS_SYSTICK_FRQ            1000     /**< OS时钟中断频率，每秒的tick数(周期的倒数)*/
#define CATOS_SYSTICK_MS \
    ((1000 * 1) / CATOS_SYSTICK_FRQ)          /**< 每个tick经过的毫秒数*/

#define CATOS_MEM_HEAP_SIZE          2048     /**< 动态内存空间大小 */

/** 任务相关 **/
#define CATOS_TASK_PRIO_MIN          32       /**< 任务最大优先级+1 (该值无意义，只是提供上限) */
#define CATOS_MAX_SLICE              10       /**< 最大时间片 */

#define CATOS_IDLE_STACK_SIZE        1024     /**< 空闲任务栈大小*/
/** 可选系统功能 */
#define CATOS_ENABLE_CPU_USAGE       1

/* 标准输入输出 */
#define CATOS_STDIO_ENABLE           1        /**< 使用标准输入输出 */
#define CAT_STDIO_NUM_STR_BUF_SIZE   64       /**< 数字缓冲区长度 */ 
#define CATOS_STDIO_DEVICE_NAME \
    "debug_uart"                              /**< 标准输入输出使用设备 */

/* 设备驱动框架 */
#define CATOS_DEVICE_MODEL_ENABLE     1       /**< 使用设备驱动框架 */

#define CATOS_ASSERT_ENABLE           1       /**< 使用断言 */
#define CATOS_CLOG_ENABLE             1       /**< 使用日志 */
#define CATOS_CLOG_LEVEL              4       /* 输出的最高日志等级，等级更低(数值小)的日志不打印 */
                                              /**
                                               * 可选值：
                                               * 0x0 : CLOG_LEVEL_DISABLE,
                                               * 0x1 : CLOG_LEVEL_ERROR  
                                               * 0x2 : CLOG_LEVEL_WARNING
                                               * 0x3 : CLOG_LEVEL_INFO   
                                               * 0x4 : CLOG_LEVEL_DEBUG  
                                               * 0x5 : CLOG_LEVEL_TRACE
                                               * 注意：频繁的log会使用大量栈空间，还没想到解决方案
                                               */

/** 组件 **/
/* 命令行 */
#define CATOS_CAT_SHELL_ENABLE        0       /**< 使用命令行 */
#define CATOS_SHELL_TASK_PRIO \
    (CATOS_TASK_PRIO_MIN - 2)                 /**< 命令行任务优先级(默认倒数第二低)*/
#define CATOS_SHELL_STACK_SIZE        4096    /**< 命令行任务栈空间大小*/
#define CATOS_SHELL_USE_HISTORY       1       /**< 使用历史命令功能 */

/* third_party 三方代码 */


/* 处理依赖的功能（不要修改这里） */
#if (CATOS_STDIO_ENABLE == 0)
    #if (CATOS_ASSERT_ENABLE == 1)
    #error "assert relies on stdio"
    #endif

    #if (CATOS_CLOG_ENABLE == 1)
    #error "cat_log relies on stdio"
    #endif

    #if (CATOS_CAT_SHELL_ENABLE == 1)
    #error "shell relies on stdio"
    #endif
#endif

#endif/* #ifndef CATOS_CONFIG_H */
