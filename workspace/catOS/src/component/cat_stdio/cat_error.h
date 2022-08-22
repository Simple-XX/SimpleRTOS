/**
 * @file cat_error.h
 * @brief 错误处理等
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-12
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-12 1.0    amoigus             内容
 */

#ifndef CAT_ERROR_H
#define CAT_ERROR_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_stdio.h"

#define CAT_ASSERT(_expr) \
    do{ \
        if(!(_expr)) \
        { \
            CAT_KPRINTF("%s:%d %s assert failed !\r\n", __FILE__, __LINE__, #_expr); \
            while(1); \
        } \
    }while(0)

#define CAT_FALTAL_ERROR(_msg) \
    do{ \
        CAT_KPRINTF("%s:%d faltal error:%s !\r\n", __FILE__, __LINE__, _msg); \
        while(1); \
    }while(0)

#endif
