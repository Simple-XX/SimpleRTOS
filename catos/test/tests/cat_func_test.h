/**
 * @file cat_func_test.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 功能测试辅助接口
 * @version 0.1
 * @date 2025-03-03
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-03 <td>内容
 * </table>
 */
#ifndef CAT_FUNC_TEST_H
#define CAT_FUNC_TEST_H

#include "catos.h"

static cat_u32 test_cnt = 0; /* 测试序号，每调用一次CAT_TEST_INFO就加一 */

#define CAT_EXPECT(_expr) \
    do { \
        if(_expr) \
        { \
            cat_printf("[TEST_EXPECT]%s:%d %s(%s) == false%s\r\n", __FILE__, __LINE__, COLOR_RED, #_expr, COLOR_NONE); \
        } \
        else \
        { \
            cat_printf("[TEST_EXPECT]%s:%d %s(%s) == true%s\r\n", __FILE__, __LINE__, COLOR_GREEN, #_expr, COLOR_NONE); \
        } \
    }while(0)

#define CAT_TEST_INFO(_func, _test_name) \
    do { \
        cat_printf("== test-%d == \"%s\" %s\r\n", test_cnt++, #_func, #_test_name); \
    }while(0)

#endif
