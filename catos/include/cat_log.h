/**
 * @file cat_log.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 日志
 * @version 0.1
 * @date 2025-03-31
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-31 <td>内容
 * </table>
 */
#ifndef CAT_LOG_H
#define CAT_LOG_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_stdio.h"

#define CLOG_LEVEL_DEFAULT   0xff /* 默认日志等级 */

/* 日志等级，数值越小级别越高 */
typedef enum
{
    CLOG_LEVEL_DISABLE = 0x0,
    CLOG_LEVEL_ERROR  ,
    CLOG_LEVEL_WARNING,
    CLOG_LEVEL_INFO   ,
    CLOG_LEVEL_DEBUG  ,
    CLOG_LEVEL_TRACE
} clog_level_t;

#if (CATOS_CLOG_ENABLE == 1)
/* 输出日志，级别：错误（0x1，最高）*/
#define CLOG_ERROR(fmt, ...)   clog_out(CLOG_LEVEL_ERROR,   fmt, ##__VA_ARGS__)
/* 输出日志，级别：警告（0x2）*/
#define CLOG_WARNING(fmt, ...) clog_out(CLOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
/* 输出日志，级别：普通信息（0x3）*/
#define CLOG_INFO(fmt, ...)    clog_out(CLOG_LEVEL_INFO,    fmt, ##__VA_ARGS__)
/* 输出日志，级别：调试信息（0x4）*/
#define CLOG_DEBUG(fmt, ...)   clog_out(CLOG_LEVEL_DEBUG,   fmt, ##__VA_ARGS__)

/* 输出日志，级别：系统跟踪（0xf1）*/
#define CLOG_TRACE(fmt, ...)   clog_out(CLOG_LEVEL_TRACE, "%s:%4d " fmt, __func__, __LINE__ , ##__VA_ARGS__);

/**
 * @brief 日志输出统一接口
 * 
 * @param  level   日志等级
 * @param  fmt     格式输出字符串
 * @param  ...     格式输出参数列表
 */
void clog_out(clog_level_t level, const char *fmt, ...);
#else /* #if (CATOS_CLOG_ENABLE == 1) */
#define CLOG_ERROR(fmt, ...)
#define CLOG_WARNING(fmt, ...)
#define CLOG_INFO(fmt, ...)
#define CLOG_DEBUG(fmt, ...)
#define CLOG_TRACE(fmt, ...)
#endif /* #if (CATOS_CLOG_ENABLE == 1) */

#endif