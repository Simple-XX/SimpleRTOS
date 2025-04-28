/**
 * @file cat_log.c
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
#include "cat_ansi_code.h"
#include "cat_log.h"

#include <stdarg.h>

/* 定义每个等级的日志输出字体*/
#define _CLOG_ERROR_PREFIX_STYLE CA_STYLE_BOLD CA_FG_RED
#define _CLOG_WARN__PREFIX_STYLE CA_STYLE_BOLD CA_FG_YELLOW
#define _CLOG_INFO__PREFIX_STYLE CA_FG_GREEN
#define _CLOG_DEBUG_PREFIX_STYLE 
#define _CLOG_TRACE_PREFIX_STYLE CA_FG_BLUE

/* 各等级日志输出前缀 */
static char *_clog_prefix[] = {
    "[UNDEFINED_LOG_LEVEL!]",
    _CLOG_ERROR_PREFIX_STYLE "[CLOG_ERROR]",
    _CLOG_WARN__PREFIX_STYLE "[CLOG_WARN ]",
    _CLOG_INFO__PREFIX_STYLE "[CLOG_INFO ]",
    _CLOG_DEBUG_PREFIX_STYLE "[CLOG_DEBUG]",
    _CLOG_TRACE_PREFIX_STYLE "[CLOG_TRACE]"
};

void clog_out(clog_level_t level, const char *fmt, ...)
{
    va_list args;

    /* 控制日志输出等级 */
    if(level <= CATOS_CLOG_LEVEL)
    {
        /* 输出前缀 */
        cat_kprintf("%s ", _clog_prefix[level]);

        va_start(args, fmt);

        cat_vprintf(fmt, args);

        va_end(args);

        cat_printf(CA_STYLE_RESET "\r\n");
    }
}

#if (CATOS_CAT_SHELL_ENABLE == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_test_log(void *arg)
{
    (void)arg;

    CLOG_ERROR  ("%s", "just test error");
    CLOG_WARNING("%s", "just test warning");
    CLOG_INFO   ("%s", "just test info");
    CLOG_DEBUG  ("%s", "just test debug");
    CLOG_TRACE  ("%s", "just test trace");

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_log, test log system, do_test_log);
#endif /* #if (CATOS_CAT_SHELL_ENABLE == 1) */
