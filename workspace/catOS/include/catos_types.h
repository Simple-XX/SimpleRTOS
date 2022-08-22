/**
 * @file catos_defs.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-17 1.0    amoigus             基本类型定义
 */

#ifndef CATOS_DEF_H
#define CATOS_DEF_H

#include <stdio.h>

/************** MACROS*********************/
#define U32_MAX_VAL 0xffffffff

typedef signed char     int8_t;
typedef short int       int16_t;
#if defined(__CC_ARM)
    typedef int             int32_t;
#endif
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
#if 1
    #if defined(__CC_ARM)
        typedef unsigned int            uint32_t;
    #endif
#else
    typedef unsigned int            uint32_t;
#endif

typedef uint8_t cat_stack_type_t;

/* 错误代码定义 */
#define CAT_EOK                 (0)     /**< 成功 */
#define CAT_ERROR               (1)     /**< 失败 */
#define CAT_EINVAL              (2)     /**< 非法值 */

/* 对齐 */
/**
 * @brief 向上取整对齐
 */
#define CAT_ALIGN(addr, align) \
    (((addr) + (align) - 1) & ~((align) - 1))
/**
 * @brief 向下取整对齐
 */
#define CAT_ALIGN_DOWN(addr, align) \
    ((addr) & ~((align) - 1))

/************** struct type*********************/
/* cat_list.c */
typedef struct _cat_node_t     cat_node_t;
typedef struct _cat_list_t     cat_list_t;

/* cat_device.c */
typedef enum _cat_device_type_t         cat_device_type_t;      /**< 设备类型 */
typedef struct _cat_device_t            cat_device_t;           /**< 设备结构体 */


#endif


