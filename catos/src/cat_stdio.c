/**
 * @file cat_stdio.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 标准输入输出源文件
 * @version 2.0
 * @date 2025-04-03
 *
 * Copyright (c) 2023
 *
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2022-07-16 <td>支持十进制和字符串输入输出
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-04 <td>支持16进制输出
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-09 <td>修复cat_sprintf()使用时width大于零时无限循环写内存的bug
 * <tr><td>v2.0 <td>文佳源 <td>2025-04-03 <td>重构
 * </table>
 */
#include "catos_config.h"
#include "catos_types.h"

#include "cat_lib.h"
#include "cat_intr.h"

#include "cat_assert.h"

#if (CATOS_STDIO_ENABLE == 1)

static cat_i32 _print_padded(const char *src, cat_i32 len, cat_i32 width, cat_bool is_left_align);

static cat_i32 _sprint_padded(char *dst, const char *src, cat_i32 len, cat_i32 width, cat_bool is_left_align);
static cat_i32 _ftoa(char *buf, cat_double num, cat_i32 precision);

#if (CATOS_DEVICE_MODEL_ENABLE == 1)
#include "cat_device.h"

static cat_device_t *_stdio_dev = CAT_NULL;

cat_u8 cat_stdio_set_device(const char *name)
{
    cat_u8 ret = CAT_EOK;

    _stdio_dev = cat_device_get(name);
    CAT_ASSERT(CAT_NULL != _stdio_dev);

    if(CAT_NULL != _stdio_dev)
    {
        ret = cat_device_init(_stdio_dev);
    }
    if(CAT_EOK == ret)
    {
        ret = cat_device_open(_stdio_dev, CAT_DEVICE_MODE_RDWR);
    }

    return ret;
}

cat_u8 cat_stdio_is_device_is_set(void)
{
    cat_u8 ret = 0;

    if(CAT_NULL != _stdio_dev)
    {
        ret = 1;
    }

    return ret;
}

/**
 * @brief 获取字符
 * 
 * @return char 得到的字符
 */
char cat_getchar(void)
{
    char c;

    cat_device_read(_stdio_dev, 0, &c, 1);

    return c;
}

/**
 * @brief 输出字符
 *
 * @param[in] ch       字符
 * @return cat_i32     成功则返回字符数值(以 i32 格式)
 *                     小于0: 失败
 */
cat_i32 cat_putchar(char c)
{
    cat_device_write(_stdio_dev, 0, &c, 1);

    return 0;
}
#else /* #if (CATOS_DEVICE_MODEL_ENABLE == 1) */
#error not implied!!
/**
 * @brief 获取字符
 * 
 * @return char 得到的字符
 */
char cat_getchar(void)
{
    char c = 0;



    return c;
}

/**
 * @brief 输出字符
 *
 * @param[in] ch       字符
 * @return cat_i32     成功则返回字符数值(以 i32 格式)
 *                     小于0: 失败
 */
cat_i32 cat_putchar(char c)
{


    return 0;
}
#endif /* #if (CATOS_DEVICE_MODEL_ENABLE == 1) */

cat_i32 cat_vprintf(const char *format, va_list args)
{
    cat_i32 count = 0;

    /* 检查参数是否为空 */
    if (CAT_NULL == format)
    {
        return CAT_EINVAL;
    }

    /* 开始处理不定长参数 */

    /* 循环到格式字符串结尾 */
    while (*format)
    {
        /* 如果是%则开始处理格式 */
        if (*format == '%')
        {
            format++;

            /* 对齐方式，默认右对齐，如果是 %- 则是左对齐 */
            cat_bool is_left_align = CAT_FALSE;
            if('-' == *format)
            {
                is_left_align = CAT_TRUE;
                format++; /* 格式字符串指针后移 */
            }

            /* 输出宽度，当此时出现数字才处理 */
            cat_u32 width = 0;
            while((*format >= '0') && (*format <= '9'))
            {
                width = width * 10 + (*format - '0');
                format++;
            }

            /* 精度，仅浮点可用，默认6位 */
            cat_i32 precision = -1;
            if('.' == *format)
            {
                format++;
                precision = 0;

                while((*format >= '0') && (*format <= '9'))
                {
                    precision = precision * 10 + (*format - '0');
                    format++;
                }
            }

            /* 需要使用_sprint_padded输出到buffer并且移动str指针 */
            cat_bool need_print_and_move = CAT_FALSE;
            char num_str_buffer[CAT_STDIO_NUM_STR_BUF_SIZE];
            cat_i32 num_strlen; /* 数字转换为字符串后的长度 */
            switch (*format)
            {
                case 'd':
                {
                    cat_i32 num = va_arg(args, cat_i32);

                    /* 将i32转换为字符串 */
                    num_strlen = cat_itoa(num_str_buffer, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'x':
                {
                    cat_u32 num = va_arg(args, cat_u32);

                    /* 将i32转换为字符串 */
                    num_strlen = cat_itoh(num_str_buffer, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'f':
                {
                    /* 浮点作为可变参传进来会变为double */
                    cat_double num = va_arg(args, cat_double);

                    /* 未指定则改为默认精度6位 */
                    if(precision < 0)
                    {
                        precision = 6;
                    }

                    /* 将浮点转换为字符串 */
                    num_strlen = _ftoa(num_str_buffer, num, precision);
                    need_print_and_move = CAT_TRUE;

                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(args, cat_i32);
                    cat_putchar(c);
                    count += 1;

                    break;
                }
                case 's':
                {
                    char *s = va_arg(args, char *);

                    const char *p = s;
                    while('\0' != *p)
                    {
                        p++;
                    }
                    cat_i32 len = _print_padded(s, p - s, width, is_left_align);

                    count += len;

                    break;
                }
                case '%':
                {
                    cat_putchar('%');
                    count += 1;
                    break;
                }
                default:
                {
                    cat_putchar('%');
                    cat_putchar(*format);
                    count += 2;
                    break;
                }
            } /* switch */

            if(CAT_TRUE == need_print_and_move)
            {
                /* 以指定宽度输出 */
                cat_i32 len = _print_padded(num_str_buffer, num_strlen, width, is_left_align);

                /* 缓冲区指针前移 */
                count += len;
            }
        }
        else
        {
            cat_putchar(*format);
        }
        format++;
    }

    return count;
}

cat_i32 cat_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cat_i32 count = cat_vprintf(format, args);
    va_end(args);
    return count;
}

cat_i32 cat_sprintf(char *str, const char *format, ...)
{
    /* 检查参数是否为空 */
    if ((CAT_NULL == str) || (CAT_NULL == format))
    {
        return CAT_EINVAL;
    }

    /* 开始处理不定长参数 */
    va_list args;
    va_start(args, format);

    char *start = str;
    /* 循环到格式字符串结尾 */
    while (*format)
    {
        /* 如果是%则开始处理格式 */
        if (*format == '%')
        {
            format++;

            /* 对齐方式，默认右对齐，如果是 %- 则是左对齐 */
            cat_bool is_left_align = CAT_FALSE;
            if('-' == *format)
            {
                is_left_align = CAT_TRUE;
                format++; /* 格式字符串指针后移 */
            }

            /* 输出宽度，当此时出现数字才处理 */
            cat_u32 width = 0;
            while((*format >= '0') && (*format <= '9'))
            {
                width = width * 10 + (*format - '0');
                format++;
            }

            /* 精度，仅浮点可用，默认6位 */
            cat_i32 precision = -1;
            if('.' == *format)
            {
                format++;
                precision = 0;

                while((*format >= '0') && (*format <= '9'))
                {
                    precision = precision * 10 + (*format - '0');
                    format++;
                }
            }

            /* 需要使用_sprint_padded输出到buffer并且移动str指针 */
            cat_bool need_print_and_move = CAT_FALSE;
            char num_str_buffer[CAT_STDIO_NUM_STR_BUF_SIZE];
            cat_i32 num_strlen; /* 数字转换为字符串后的长度 */
            switch (*format)
            {
                case 'd':
                {
                    cat_i32 num = va_arg(args, cat_i32);

                    /* 将i32转换为字符串 */
                    num_strlen = cat_itoa(num_str_buffer, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'x':
                {
                    cat_u32 num = va_arg(args, cat_u32);

                    /* 将i32转换为字符串 */
                    num_strlen = cat_itoh(num_str_buffer, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'f':
                {
                    /* 浮点作为可变参传进来会变为double */
                    cat_double num = va_arg(args, cat_double);

                    /* 未指定则改为默认精度6位 */
                    if(precision < 0)
                    {
                        precision = 6;
                    }

                    /* 将浮点转换为字符串 */
                    num_strlen = _ftoa(num_str_buffer, num, precision);
                    need_print_and_move = CAT_TRUE;

                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(args, cat_i32);
                    *str++ = c;
                    break;
                }
                case 's':
                {
                    char *s = va_arg(args, char *);

                    const char *p = s;
                    while('\0' != *p)
                    {
                        p++;
                    }
                    cat_i32 len = _sprint_padded(str, s, p - s, width, is_left_align);

                    str += len;

                    break;
                }
                case '%':
                {
                    *str++ = '%';
                    break;
                }
                default:
                {
                    *str++ = '%';
                    *str++ = *format;
                    break;
                }
            } /* switch */

            if(CAT_TRUE == need_print_and_move)
            {
                /* 以指定宽度输出 */
                cat_i32 len = _sprint_padded(str, num_str_buffer, num_strlen, width, is_left_align);

                /* 缓冲区指针前移 */
                str += len;
            }
        }
        else
        {
            *str++ = *format;
        }
        format++;
    }

    *str = '\0';
    va_end(args);
    return str - start;
}

cat_i32 cat_kprintf(const char *format, ...)
{
    cat_printf("[%d] ", catos_get_systick());

    va_list args;
    va_start(args, format);
    cat_i32 count = cat_vprintf(format, args);
    va_end(args);
    return count;
}

/* STATIC FUNCS */

/**
 * @brief 将字符串按指定宽度输出到控制台
 * 
 * @param[in] src      源字符串
 * @param[in] len      字符串长度
 * @param[in] width    宽度
 * @param[in] is_left_align CAT_TRUE : 左对齐
 *                          CAT_FALSE: 右对齐 
 * @return cat_i32  输出字符数
 */
static cat_i32 _print_padded(const char *src, cat_i32 len, cat_i32 width, cat_bool is_left_align)
{
    cat_i32 total_len = 0; /* 字符串总长度 */

    /* 取最大的 */
    total_len = width > len ? width : len;

    if (!is_left_align && width > len)
    {
        /* 右对齐填充 */
        width = width - len; /* 复用一下width局部变量 */
        while(width > 0)
        {
            cat_putchar(' ');
            width--;
        }

    }

    /* 输出内容 */
    const char *p = CAT_NULL;
    for (p = src; *p; p++)
    {
        cat_putchar(*p);
    }

    if (is_left_align && width > len)
    {
        /* 左对齐填充 */
        width = width - len;
        while(width > 0)
        {
            cat_putchar(' ');
            width--;
        }
    }

    return total_len;
}

/**
 * @brief 将字符串按指定宽度输出到缓冲区
 * 
 * @param[in] dst      目标缓冲区
 * @param[in] src      源字符串
 * @param[in] len      字符串长度
 * @param[in] width    宽度
 * @param[in] is_left_align CAT_TRUE : 左对齐
 *                          CAT_FALSE: 右对齐 
 * @return cat_i32 输出字符数
 */
static cat_i32 _sprint_padded(char *dst, const char *src, cat_i32 len, cat_i32 width, cat_bool is_left_align)
{
    cat_i32 total_len = 0; /* 字符串总长度 */

    /* 取最大的 */
    total_len = width > len ? width : len;

    if (!is_left_align && width > len)
    {
        /* 右对齐填充 */
        width = width - len; /* 复用一下width局部变量 */
        while(width > 0)
        {
            *(dst++) = ' ';
            width--;
        }

    }

    /* 输出内容 */
    const char *p = CAT_NULL;
    for (p = src; *p; p++)
    {
        *(dst++) = *p;
    }

    if (is_left_align && width > len)
    {
        /* 左对齐填充 */
        width = width - len;
        while(width > 0)
        {
            *(dst++) = ' ';
            width--;
        }
    }

    return total_len;
}

/* 辅助函数：输出浮点数 */
static cat_i32 _ftoa(char *buf, cat_double num, cat_i32 precision)
{
    cat_i32 idx = 0;
    
    /* 处理负数 */
    if (num < 0) {
        buf[idx++] = '-';
        num = -num;
    }
    
    /* 整数部分 */
    cat_i32 int_part = (cat_i32)num;
    double frac_part = num - int_part;
    
    /* 转换整数部分 */
    if (int_part == 0) {
        buf[idx++] = '0';
    } else {
        cat_i32 start = idx;
        while (int_part > 0) {
            buf[idx++] = '0' + (int_part % 10);
            int_part /= 10;
        }
        /* 反转整数部分 */
        cat_i32 end = idx - 1;
        while (start < end) {
            char tmp = buf[start];
            buf[start] = buf[end];
            buf[end] = tmp;
            start++;
            end--;
        }
    }
    
    /* 小数部分 */
    if (precision > 0) {
        buf[idx++] = '.';
        for (cat_i32 i = 0; i < precision; i++) {
            frac_part *= 10;
            cat_i32 digit = (cat_i32)frac_part;
            buf[idx++] = '0' + digit;
            frac_part -= digit;
        }
    }

    buf[idx] = '\0';
    
    return idx;
}

#endif /* #if (CATOS_STDIO_ENABLE == 1) */

/* 感觉vprintf和sprintf中很多部分可以重用，但考虑到可能影响一丢丢性能，之后再测试吧 */
#if 0
/**
 * @brief vprintf和sprintf底层实现
 * 
 * @param[in] is_printf CAT_TRUE:  用于vprintf
 *                      CAT_FALSE: 用于sprintf
 * @param[in] str       sprintf 输出到的字符串，用于vprintf时可以为空
 * @param[in] format    格式字符串
 * @param[in] args      可变参数
 */
static inline cat_i32 _vsprintf_impl(cat_bool is_vprintf, char *str, const char *format, va_list args)
{
    /* 检查参数是否为空 */
    if ((CAT_FALSE == is_vprintf) && (CAT_NULL == str) || (CAT_NULL == format))
    {
        return CAT_EINVAL;
    }

    /* 开始处理不定长参数 */

    char *start = str;
    /* 循环到格式字符串结尾 */
    while (*format)
    {
        /* 如果是%则开始处理格式 */
        if (*format == '%')
        {
            format++;

            /* 对齐方式，默认右对齐，如果是 %- 则是左对齐 */
            cat_bool is_left_align = CAT_FALSE;
            if('-' == *format)
            {
                is_left_align = CAT_TRUE;
                format++; /* 格式字符串指针后移 */
            }

            /* 输出宽度，当此时出现数字才处理 */
            cat_u32 width = 0;
            while((*format >= '0') && (*format <= '9'))
            {
                width = width * 10 + (*format - '0');
                format++;
            }

            /* 精度，仅浮点可用，默认6位 */
            cat_u32 precision = -1;
            if('.' == *format)
            {
                format++;
                precision = 0;

                while((*format >= '0') && (*format <= '9'))
                {
                    precision = precision * 10 + (*format - '0');
                    format++;
                }
            }

            /* 需要使用_sprint_padded输出到buffer并且移动str指针 */
            cat_bool need_print_and_move = CAT_FALSE;
            char out_str_buf[CAT_STDIO_NUM_STR_BUF_SIZE];
            cat_i32 out_str_len = 0; /* 数字转换为字符串后的长度 */
            switch (*format)
            {
                case 'd':
                {
                    cat_i32 num = va_arg(args, cat_i32);

                    /* 将i32转换为字符串 */
                    out_str_len = cat_itoa(out_str_buf, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'x':
                {
                    cat_u32 num = va_arg(args, cat_u32);

                    /* 将i32转换为字符串 */
                    out_str_len = cat_itoh(out_str_buf, num);
                    need_print_and_move = CAT_TRUE;
                    
                    break;
                }
                case 'f':
                {
                    /* 浮点作为可变参传进来会变为double */
                    cat_double num = va_arg(args, cat_double);

                    /* 未指定则改为默认精度6位 */
                    if(precision < 0)
                    {
                        precision = 6;
                    }

                    /* 将浮点转换为字符串 */
                    out_str_len = _ftoa(out_str_buf, num, precision);
                    need_print_and_move = CAT_TRUE;

                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(args, cat_i32);
                    *str++ = c;
                    break;
                }
                case 's':
                {
                    char *s = va_arg(args, char *);

                    char *p = out_str_buf;
                    while('\0' != *s)
                    {
                        *(p++) = *(s++);
                    }
                    out_str_len = p - str;
                    need_print_and_move = CAT_TRUE;

                    break;
                }
                case '%':
                {
                    *str++ = '%';
                    break;
                }
                default:
                {
                    *str++ = '%';
                    *str++ = *format;
                    break;
                }
            } /* switch */

            if(CAT_TRUE == need_print_and_move)
            {
                /* 以指定宽度输出 */
                if(is_vprintf)
                {
                    cat_i32 len = _print_padded(out_str_buf, out_str_len, width, is_left_align);
                }
                else
                {
                    cat_i32 len = _sprint_padded(str, out_str_buf, out_str_len, width, is_left_align);

                    /* 缓冲区指针前移 */
                    str += len;
                }
            }
        }
        else
        {
            if(is_vprintf)
            {
                cat_putchar(*format);
            }
            else
            {
                *str++ = *format;
            }
        }
        format++;
    }

    if(!is_vprintf)
    {
        *str = '\0';
    }

    return str - start;
}
#endif
