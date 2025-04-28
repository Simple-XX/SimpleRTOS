/**
 * @file catos_types.h
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

#include <stdint.h>

/* 基本数据类型 */
typedef signed char     cat_i8;
typedef short int       cat_i16;
typedef int             cat_i32;

typedef unsigned char           cat_u8;
typedef unsigned short int      cat_u16;
typedef unsigned int            cat_u32;

typedef float                   cat_float;
typedef double                  cat_double;


/* 最高位宽数据类型 */
typedef intptr_t                cat_base;
typedef uintptr_t               cat_ubase;

/* 布尔值定义 */
typedef cat_u8             cat_bool; /* 布尔值 */
#define CAT_TRUE                (1)
#define CAT_FALSE               (0)


/* 错误代码定义 */
typedef enum _cat_err
{
    CAT_EOK = 0,  /**< 成功 */
    CAT_ERROR,    /**< 失败 */
    CAT_EINVAL,   /**< 非法值 */
    CAT_ENPTR,  /**< 空指针 */
    CAT_ETIMEOUT, /**< 超时 */
    CAT_EOUTOF,   /**< 资源已耗尽 */
    CAT_EFULL,    /**< 资源已满 */
    CAT_EUNEXIST, /**< 资源不存在 */
} cat_err;  /* 错误类型 */ 

/* 空指针 */
#define CAT_NULL                ((void *)0)


/* 对齐 */
/**
 * @brief 向上取整对齐(align必须是2的幂)
 */
#define CAT_ALIGN(addr, align) \
    (((addr) + (align) - 1) & ~((align) - 1))
/**
 * @brief 向下取整对齐(align必须是2的幂)
 */
#define CAT_ALIGN_DOWN(addr, align) \
    (((cat_u32)addr) & ~(((cat_u32)align) - 1))

/************** struct type*********************/
/* cat_device.c */
typedef enum _cat_device_type_t
{
    CAT_DEVICE_TYPE_CHAR = 0,                         /**< 字符(以字节作为io单位) */
    CAT_DEVICE_TYPE_BLK,                              /**< 块(可多字节io) */
    CAT_DEVICE_TYPE_RTC,                              /**< 实时时钟 */
    CAT_DEVICE_TYPE_GRAPHIC,                          /**< 图形和显示 */
    CAT_DEVICE_TYPE_IIC,                              /**< I2C 总线 */

    CAT_DEVICE_TYPE_UNDEF                             /**< 未定义设备 */
} cat_device_type_t;      /**< 设备类型 */
typedef struct _cat_device_t            cat_device_t;           /**< 设备结构体 */

/* 一些前向声明， 防止头文件循环依赖，当只需要使用某类型T的指针时，不需要包含T类型的完整定义所在头文件*/
typedef struct _cat_task_t cat_task_t; /* 任务结构体 */

#endif


