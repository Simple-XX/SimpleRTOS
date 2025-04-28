/**
 * @file cat_assert.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 断言头文件
 * @version 0.1
 * @date 2025-04-03
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-04-03 <td>内容
 * </table>
 */
#ifndef CAT_ASSERT_H
#define CAT_ASSERT_H

#include "catos_config.h"

#include "cat_stdio.h"

#if (CATOS_ASSERT_ENABLE == 1)
/**
 * @brief 断言
 * 
 * @param _expr 表达式
 */
#define CAT_ASSERT(_expr) \
    do{ \
        if(!(_expr)) \
        { \
            cat_printf("%s:%d %s assert failed !\r\n", __FILE__, __LINE__, #_expr); \
            while(1); \
        } \
    }while(0)
#else /* #if (CATOS_ASSERT_ENABLE == 1) */
#define CAT_ASSERT(_expr)
#endif /* #if (CATOS_ASSERT_ENABLE == 1) */

#endif