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

typedef signed char     cat_int8_t;
typedef short int       cat_int16_t;
typedef int             cat_int32_t;

typedef unsigned char           cat_uint8_t;
typedef unsigned short int      cat_uint16_t;
typedef unsigned int            cat_uint32_t;

typedef cat_uint8_t cat_stack_type_t;

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
    (((cat_uint32_t)addr) & ~(((cat_uint32_t)align) - 1))

/************** struct type*********************/
/* cat_list.c */
typedef struct _cat_node_t     cat_node_t;
typedef struct _cat_list_t     cat_list_t;

/* cat_device.c */
typedef enum _cat_device_type_t         cat_device_type_t;      /**< 设备类型 */
typedef struct _cat_device_t            cat_device_t;           /**< 设备结构体 */


#endif


