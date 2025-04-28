/**
 * @file cat_stdio.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 标准输入输出头文件
 * @version 2.0
 * @date 2025-04-03
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2022-07-16 <td>内容
 * <tr><td>v2.0 <td>文佳源 <td>2025-04-03 <td>重构
 * </table>
 */

#ifndef CAT_STDIO_H
#define CAT_STDIO_H

#include <stdarg.h>

#include "catos_config.h"
#include "catos_types.h"

#if (CATOS_STDIO_ENABLE == 1)

#if (CATOS_DEVICE_MODEL_ENABLE == 1)
/**
 * @brief 设置stdio输入输出设备
 * 
 * @param  name             设备名称
 * @return cat_u8          成功失败
 */
cat_u8 cat_stdio_set_device(const char *name);

/**
 * @brief 检查stdio设备是否设置
 * 
 * @return cat_u8 0：未设置
 *                 1：已设置
 */
cat_u8 cat_stdio_is_device_is_set(void);
#endif

char    cat_getchar(void);
cat_i32 cat_putchar(char c);
cat_i32 cat_vprintf(const char *format, va_list args);
cat_i32 cat_printf(const char *format, ...);
cat_i32 cat_sprintf(char *str, const char *format, ...);

cat_i32 cat_kprintf(const char *format, ...);

#else

#define cat_getchar()
#define cat_putchar(c)
#define cat_vprintf(fmt, args)
#define cat_printf(fmt, ...)
#define cat_sprintf(str, fmt, ...)

#define cat_kprintf(fmt, ...)

#endif /* #define CATOS_STDIO_ENABLE 1 */

#endif /* #define CAT_STDIO_H */
