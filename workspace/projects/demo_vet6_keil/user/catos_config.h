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
#define CATOS_VERSION               "v1.0.2"
/* 构建工具信息 */
#if defined(__CC_ARM)
    #define CATOS_BUILD_COMPILER    "armcc under 6"
#elif (defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))
    #define CATOS_BUILD_COMPILER    "armcc6(AC6)"
#elif defined(__GNUC__)
    #define CATOS_BUILD_COMPILER    "gcc(GNU)"
#endif

/** 系统相关 **/
#define CATOS_SYSTICK_FRQ           (100)   /**< 系统时钟频率，每秒的tick数(周期的倒数)*/
#define CATOS_SYSTICK_MS            ((1000 * 1) / CATOS_SYSTICK_FRQ) /**< 每个tick经过的毫秒数*/

/** 任务相关 **/
#define CATOS_MAX_TASK_PRIO         (32)    /**< 任务最大优先级 */
#define CATOS_MAX_SLICE             (10)    /**< 最大时间片 */

#define CATOS_IDLE_STACK_SIZE       (1024)  /**< 空闲任务栈大小*/

/** component **/
/* cat_device */
#define CATOS_ENABLE_DEVICE_MODEL           1           /**< 使用设备驱动框架 */

/* cat_shell */
#define CATOS_ENABLE_CAT_SHELL              1           /**< 使用shell */
#if (CATOS_ENABLE_CAT_SHELL == 1)
    #define CATOS_SHELL_TASK_PRIO  (CATOS_MAX_TASK_PRIO - 2)  /**< 任务优先级*/
    #define CATOS_SHELL_STACK_SIZE (4096)                     /**< 任务栈空间大小*/
#endif

/* cat_stdio */
#define CATOS_ENABLE_SYS_PRINTF             1           /**< 系统输出 */
#if (CATOS_ENABLE_SYS_PRINTF == 1)
    #define CATOS_ENABLE_DEBUG_PRINTF       1                /**< 调试打印功能 */
    #define CATOS_STDIO_DEVICE_NAME         "debug_uart"     /**< 标准输入输出使用设备 */
#endif /* #if (CATOS_ENABLE_SYS_PRINTF == 1) */

/* third_party 三方代码 */
#define CATOS_ENABLE_CMBACKTRACE            0           /**< 使用cm_backtrace追踪 */

#endif/* #ifndef CATOS_CONFIG_H */
