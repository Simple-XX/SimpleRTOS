/**
 * @file cat_stdio.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 标准输入输出
 * @version 0.1
 * @date 2022-07-16
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2022-07-16 <td>支持十进制和字符串输入输出
 * <tr><td>v1.0 <td>文佳源 <td>2023-6-4 <td>支持16进制输出
 * </table>
 */

#include "cat_stdio.h"
#include "cat_string.h"

/************* macros and defines **************/
#define CAT_INT_LONG    10
#define MAX_STRNG_LEN   300
#define CAT_FLOAT_LONG  42

typedef enum
{
    CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE = 0,
    CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT,
    CAT_ERROR_END,
} cat_error_type_t;

/************* local vars **************/
static const cat_uint8_t *_cat_error_msg[] = 
{
    (const cat_uint8_t *)"[cat_stdio] no match error type !!\r\n",
    (const cat_uint8_t *)"[cat_stdio:cat_print]type not support !!\r\n",
};

/************* static **************/
static cat_int32_t _cat_print_string(const cat_uint8_t *str, cat_int32_t width);
static cat_int32_t _cat_print_error(cat_error_type_t type);
static cat_int32_t _cat_print_int(cat_int32_t num, cat_int32_t width);
static cat_err_t   _cat_print_float(cat_float_t num, cat_uint8_t keep, cat_int32_t width);

static cat_int32_t _cat_sprint_string(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, const cat_uint8_t *str);
static cat_int32_t _cat_sprint_int(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, cat_int32_t num, cat_int32_t width);
static cat_err_t _cat_sprint_float(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, cat_float_t num, cat_uint8_t keep, cat_int32_t width);

static cat_int32_t _cat_scan_string(cat_uint8_t *str_dest, cat_uint32_t buf_len);
static cat_int32_t _cat_scan_int(cat_int32_t *dest);


static cat_int32_t _cat_print_string(const cat_uint8_t *str, cat_int32_t width)
{
    cat_int32_t ret = CAT_EOK;
    cat_uint8_t i = 0;

    if(NULL == str)
    {
        ret = CAT_ERROR;
    }
    else
    {
        while('\0' != *str)
        {
            cat_putchar(*str);
            str++;
            i++;
        }
    }

    /* 用空字符填充 */
    width = width - i;
    while(width > 0)
    {
        cat_putchar(' ');
        width--;
    }

    return ret;
}

static cat_int32_t _cat_print_error(cat_error_type_t type)
{
    cat_int32_t ret = CAT_EOK;
    cat_putchar('\r');
    cat_putchar('\n');

    /* 检查参数type范围 */
    if(
        (CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE >= type) ||
        (CAT_ERROR_END                      <= type)    
    )
    {
        _cat_print_string(_cat_error_msg[CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE], 0);
    }
    else
    {
        _cat_print_string(_cat_error_msg[type], 0);
    }

    return ret;
}

static cat_int32_t _cat_print_int(cat_int32_t num, cat_int32_t width)
{
    cat_int32_t ret = CAT_EOK;
    /* cat_uint32_t max val = 4294967295 */
    cat_uint8_t buf[CAT_INT_LONG] = {0};
    cat_uint8_t i = 0;

    if(num < 0)
    {
        cat_putchar('-');
        num = -num;
    }

    do
    {
        buf[i++] = (cat_uint8_t)(num % 10);
        num = num / 10;

    }while((0 != num) && (i < CAT_INT_LONG));

    /* 如果有对齐要求则先输出空格 */
    width = width - i;
    while(width > 0)
    {
        cat_putchar(' ');
        width--;
    }

    while(i > 0)
    {
        cat_putchar(buf[--i] + '0');
    }

    return ret;
}

/**
 * @brief 打印浮点数
 * 
 * @param  num              要打印的浮点数
 * @param  keep             要保留的小数位数
 * @param  width            输出宽度
 * @return cat_err_t 
 */
static cat_err_t _cat_print_float(cat_float_t num, cat_uint8_t keep, cat_int32_t width)
{
    cat_err_t ret = CAT_EOK;
    /* cat_float_t : [+/- 1.18 * 10^-38, +/- 3.4 * 10^38], 算上符号, 小数点和末尾符号一共 42 个字符 */
    cat_uint8_t buf[CAT_FLOAT_LONG] = {0};
    cat_uint8_t i = 0;

    /* 最少保留一位小数 */
    if(0 == keep)
    {
        keep = 1;
    }
    

    if(num < 0)
    {
        cat_putchar('-');
        num = -num;
    }

    cat_int32_t int_part = (cat_int32_t)num;
    num -= int_part;
    if(0 == int_part)
    {
        buf[i++] = 0;
    }
    else
    {
        do
        {
            buf[i++] = (cat_uint8_t)(int_part % 10);
            int_part = int_part / 10;
        }while((0 != int_part) && (i < CAT_FLOAT_LONG));
    }

    /* 计算保留位数的截止处(还要多计算一位) */
    cat_uint32_t keep_end = i + keep + 1;
    
    if(i < CAT_INT_LONG - 1)
    {
        buf[i++] = '.';

        if(0.0f == num)
        {
            buf[i++] = 0;
        }
        else
        {
            do
            {
                num *= 10;
                int_part = (cat_int32_t)num;
                buf[i++] = int_part;
                num -= int_part;
            }while((num != 0.0f) && (i <= keep_end)); /* 多算一位是为了四舍五入 */

            while(i <= keep_end)
            {
                buf[i++] = 0;
            }

            /* 减掉用于四舍五入的那一位 */
            i--;

            if(buf[keep_end] >= 5)
            {

                do
                {
                    keep_end--;
                    buf[keep_end] = (buf[keep_end] + 1) % 10;
                }while(
                    (0   == buf[keep_end]) &&
                    ('.' != buf[keep_end])
                );

                if('.' == buf[keep_end])
                {
                    /* 跳过小数点 */
                    keep_end--;

                    if(0 == buf[keep_end + 2])
                    {
                        do
                        {
                            keep_end--;
                            buf[keep_end] = (buf[keep_end] + 1) % 10;
                        }while(
                            (0 == buf[keep_end]) &&
                            (0 != keep_end)
                        );
                    }
                }
            } /* buf[keep_end] > 5 */
        }
    }
    else
    {
        CAT_SYS_PRINTF("[printf-float] ERROR: buf overflow!\r\n");
    }

    /* 如果有对齐要求则先输出空格 */
    width = width - i;
    while(width > 0)
    {
        cat_putchar(' ');
        width--;
    }

    /* 找到小数点位置, 整数部分 buf 要倒序输出, 小数部分 buf 要正序输出 */
    cat_uint8_t int_idx = 0, dec_idx = 0;
    while(('.' != buf[int_idx]) && (int_idx < CAT_INT_LONG))
    {
        int_idx++;
    }

    /* 小数点位置下一个就是小数开始的位置 */
    dec_idx = int_idx + 1;

    /* 输出整数部分 */
    while(int_idx > 0)
    {
        cat_putchar(buf[--int_idx] + '0');
    }

    cat_putchar('.');

    /* 输出小数部分 */
    while(dec_idx < i)
    {
        cat_putchar(buf[dec_idx++] + '0');
    }

    return ret;
}

static cat_int32_t _cat_sprint_string(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, const cat_uint8_t *str)
{
    cat_int32_t ret = CAT_EOK;

    if(NULL == str)
    {
        ret = CAT_ERROR;
    }
    else
    {
        while('\0' != *str)
        {
            //cat_putchar(*str);
            buf[(*buf_idx_ptr)++] = *str;
            str++;
        }
    }

    return ret;
}
static cat_int32_t _cat_sprint_int(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, cat_int32_t num, cat_int32_t width)
{
    cat_int32_t ret = CAT_EOK;
    /* cat_uint32_t max val = 4294967295 */
    cat_uint8_t int_buf[CAT_INT_LONG] = {0};
    cat_uint8_t i = 0;

    if(num < 0)
    {
        //cat_putchar('-');
        buf[(*buf_idx_ptr)++] = '-';
        num = -num;
    }

    do
    {
        int_buf[i++] = (cat_uint8_t)(num % 10);
        num = num / 10;

    }while((0 != num) && (i < CAT_INT_LONG));

    /* 如果有对齐要求则先输出空格 */
    width = width - i - 1;
    while(width > 0)
    {
        //cat_putchar(' ');
        buf[(*buf_idx_ptr)++] = ' ';
    }

    while(i > 0)
    {
        //cat_putchar(int_buf[--i] + '0');
        buf[(*buf_idx_ptr)++] = int_buf[--i] + '0';
    }

    return ret;
}

static cat_err_t _cat_sprint_float(cat_uint8_t *buf, cat_uint32_t *buf_idx_ptr, cat_float_t num, cat_uint8_t keep, cat_int32_t width)
{
    cat_err_t ret = CAT_EOK;
    /* cat_float_t : [+/- 1.18 * 10^-38, +/- 3.4 * 10^38], 算上符号, 小数点和末尾符号一共 42 个字符 */
    cat_uint8_t float_buf[CAT_FLOAT_LONG] = {0};
    cat_uint8_t i = 0;

    /* 最少保留一位小数 */
    if(0 == keep)
    {
        keep = 1;
    }
    

    if(num < 0)
    {
        //cat_putchar('-');
        buf[(*buf_idx_ptr)++] = '-';
        num = -num;
    }

    cat_int32_t int_part = (cat_int32_t)num;
    num -= int_part;
    if(0 == int_part)
    {
        float_buf[i++] = 0;
    }
    else
    {
        do
        {
            float_buf[i++] = (cat_uint8_t)(int_part % 10);
            int_part = int_part / 10;
        }while((0 != int_part) && (i < CAT_FLOAT_LONG));
    }

    /* 计算保留位数的截止处(还要多计算一位) */
    cat_uint32_t keep_end = i + keep + 1;
    
    if(i < CAT_INT_LONG - 1)
    {
        float_buf[i++] = '.';

        if(0.0f == num)
        {
            float_buf[i++] = 0;
        }
        else
        {
            do
            {
                num *= 10;
                int_part = (cat_int32_t)num;
                float_buf[i++] = int_part;
                num -= int_part;
            }while((num != 0.0f) && (i <= keep_end)); /* 多算一位是为了四舍五入 */

            while(i <= keep_end)
            {
                float_buf[i++] = 0;
            }

            /* 减掉用于四舍五入的那一位 */
            i--;

            if(float_buf[keep_end] >= 5)
            {

                do
                {
                    keep_end--;
                    float_buf[keep_end] = (float_buf[keep_end] + 1) % 10;
                }while(
                    (0   == float_buf[keep_end]) &&
                    ('.' != float_buf[keep_end])
                );

                if('.' == float_buf[keep_end])
                {
                    /* 跳过小数点 */
                    keep_end--;

                    if(0 == float_buf[keep_end + 2])
                    {
                        do
                        {
                            keep_end--;
                            float_buf[keep_end] = (float_buf[keep_end] + 1) % 10;
                        }while(
                            (0 == float_buf[keep_end]) &&
                            (0 != keep_end)
                        );
                    }
                }
            } /* float_buf[keep_end] > 5 */
        }
    }
    else
    {
        CAT_SYS_PRINTF("[printf-float] ERROR: float_buf overflow!\r\n");
    }

    /* 如果有对齐要求则先输出空格 */
    width = width - i;
    while(width > 0)
    {
        // cat_putchar(' ');
        buf[(*buf_idx_ptr)++] = ' ';
        width--;
    }

    /* 找到小数点位置, 整数部分 buf 要倒序输出, 小数部分 buf 要正序输出 */
    cat_uint8_t int_idx = 0, dec_idx = 0;
    while(('.' != float_buf[int_idx]) && (int_idx < CAT_INT_LONG))
    {
        int_idx++;
    }

    /* 小数点位置下一个就是小数开始的位置 */
    dec_idx = int_idx + 1;

    /* 输出整数部分 */
    while(int_idx > 0)
    {
        // cat_putchar(buf[--int_idx] + '0');
        buf[(*buf_idx_ptr)++] = float_buf[--int_idx] + '0';
    }

    // cat_putchar('.');
    buf[(*buf_idx_ptr)++] = '.';

    /* 输出小数部分 */
    while(dec_idx < i)
    {
        // cat_putchar(buf[dec_idx++] + '0');
        buf[(*buf_idx_ptr)++] = float_buf[dec_idx++] + '0';
    }

    return ret;
}

static cat_int32_t _cat_scan_string(cat_uint8_t *str_dest, cat_uint32_t buf_len)
{
    cat_int32_t ret = CAT_EOK;
    cat_uint32_t i = 0;

    do
    {
        str_dest[i++] = cat_getchar();
    } while (
        ('\r' != str_dest[i-1]) &&
        ('\n' != str_dest[i-1]) &&
        ('\0' != str_dest[i-1]) &&
        (i < buf_len - 1)
    );

    str_dest[i] = '\0';

    return ret;
}

static cat_int32_t _cat_scan_int(cat_int32_t *dest)
{
    cat_int32_t ret = CAT_EOK;
    cat_uint8_t is_negative = 0;
    cat_uint8_t tmp_char = 0;
    cat_int32_t tmp_int = 0;

    tmp_char = cat_getchar();
    if('-' == tmp_char)
    {
        is_negative = 1;
        tmp_char = cat_getchar();
    }
    else if('+' == tmp_char)
    {
        tmp_char = cat_getchar();
    }


    while(
        ('0' <= tmp_char) &&
        ('9' >= tmp_char)
    )
    {
        tmp_int = (tmp_int * 10) + (tmp_char - '0');
        tmp_char = cat_getchar();
    }

    if(1 == is_negative)
    {
        tmp_int = -tmp_int;
    }

    *dest = tmp_int;

    return ret;
}

/************* func **************/
#include "cat_device.h"
#include "uart/cat_drv_uart.h"
#include "cat_error.h"

static cat_device_t *_stdio_dev = NULL;

cat_uint8_t cat_stdio_set_device(const cat_uint8_t *name)
{
    cat_uint8_t ret = CAT_EOK;

    _stdio_dev = cat_device_get(name);
    CAT_ASSERT(NULL != _stdio_dev);

    if(NULL != _stdio_dev)
    {
        ret = cat_device_init(_stdio_dev);
    }
    if(CAT_EOK == ret)
    {
        ret = cat_device_open(_stdio_dev, CAT_DEVICE_MODE_RDWR);
    }

    return ret;
}

cat_uint8_t cat_stdio_is_device_is_set(void)
{
    cat_uint8_t ret = 0;

    if(NULL != _stdio_dev)
    {
        ret = 1;
    }

    return ret;
}

cat_uint8_t cat_getchar(void)
{
    cat_uint8_t ret = 0;
    
#if 0
    cat_bsp_uart_receive_byte((cat_uint8_t *)&ret);
#else
    cat_device_read(_stdio_dev, 0, &ret, 1);
#endif

    return ret;
}

cat_uint8_t cat_putchar(cat_uint8_t ch)
{
#if 0
    cat_bsp_uart_transmit_byte((cat_uint8_t *)&ch);
#else
    cat_device_write(_stdio_dev, 0, &ch, 1);
#endif

    return ch;
}

cat_int32_t cat_scanf(const cat_uint8_t *format, ...)
{
    cat_int32_t ret = CAT_EOK;
    va_list ap;
    cat_uint8_t *p = NULL;

    if(NULL == format)
    {
        ret = CAT_ERROR;
        return ret;
    }

    /* 获取参数 */
    va_start(ap, format);

    /* 挨个处理 */
    p = (cat_uint8_t *)format;
    while('\0' != *p)
    {
        if('%' != *p)
        {
            //cat_putchar(*p);
            p++;
            continue;
        }
        else
        {
            p++;
        }

        switch (*p)
        {
        case 'd':
            p++;
            //_cat_print_int(va_arg(ap, cat_int32_t));
            _cat_scan_int(va_arg(ap, cat_int32_t *));
            break;

        case 's':
            p++;
            //_cat_print_string(va_arg(ap, cat_uint8_t *));
            _cat_scan_string(va_arg(ap, cat_uint8_t *), MAX_STRNG_LEN);
            break;
        
        default:
            /* 可以考虑单独定义一个输出纯字符串的函数，可以打印错误信息 */
            _cat_print_error(CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT);
            ret = CAT_ERROR;
            break;
        }

        if(CAT_ERROR == ret)
        {
            break;
        }
    }

    va_end(ap);

    // cat_putchar('\r');
    // cat_putchar('\n');

    return ret;
}

cat_int32_t cat_printf(const cat_uint8_t *format, ...)
{
    cat_int32_t ret = CAT_EOK;
    va_list ap;
    cat_uint8_t *p = NULL;          /**< 用来遍历format字符串 */

    cat_uint8_t wid_buf[5] = {0};   /**< 用于保存宽度的字符串 */
    cat_int32_t width = 0;          /**< 输出宽度(目前仅用于整型输出) */

    cat_uint8_t keep_buf[5] = {0};  /**< 用于保存保留小数位数的字符串 */
    cat_int32_t keep = 0;           /**< 保留位数 */
        

    cat_uint8_t hex_str[11] = {0};  /**< 用于保存转为十六进制的字符串*/

    if(NULL == format)
    {
        ret = CAT_ERROR;
        return ret;
    }

    /* 获取参数 */
    va_start(ap, format);

    /* 挨个处理 */
    p = (cat_uint8_t *)format;
    while('\0' != *p)
    {
        if('%' != *p)
        {
            /* 发现'%'就开始处理格式化输出内容 */
            cat_putchar(*p);
            p++;
            continue;
        }
        else
        {
            p++;
        }

        /* 获得宽度数据(如果有) */
        /* 清除上一次的宽度数据 */
        width = 0;
        while(
            ((*p) >= '0') &&
            ((*p) <= '9')
        )
        {
            wid_buf[width++] = *p;
            p++;
        }
        if(width > 0)
        {
            wid_buf[width] = '\0';
            cat_atoi(&width, wid_buf);

            /* 之前这里忘记清零了 */
            wid_buf[0] = '\0';
        }

        /* 获得小数保留位数, 0 和 1 均保留一位小数 */
        keep = 0; /* 必须在外面复位, 否则如果在一次非零 keep 之后
                   * 未声明保留位数就会整成上一次的, 缺省保留一位小数 */
        if('.' == *p)
        {
            p++;
            
            while(
                ((*p) >= '0') &&
                ((*p) <= '9')
            )
            {
                keep_buf[keep++] = *p;
                p++;
            }
            if(keep > 0)
            {
                cat_int32_t keep_int;

                keep_buf[keep] = '\0';
                cat_atoi(&keep_int, keep_buf);

                keep = (cat_uint8_t)keep_int;
                
                keep_buf[0] = '\0';
            }
        }

        switch (*p)
        {
        case 'd':
        {
            p++;
            _cat_print_int(va_arg(ap, cat_int32_t), width);
            width = 0;
            break;
        }
        case 's':
        {
            p++;
            _cat_print_string(va_arg(ap, cat_uint8_t *), width);
            break;
        }
        case 'x':
        {
            p++;
            cat_itoh(hex_str, va_arg(ap, cat_uint32_t));
            if('\0' != hex_str[2])
            {
                _cat_print_string(hex_str, width);
            }
            else
            {
                _cat_print_string(hex_str, width);
                cat_putchar('0');
            }
            break;
        }
        case 'f':
        {
            p++;
            _cat_print_float(va_arg(ap, cat_float_t), keep, width);
            break;
        }
            

        default:
            /* 可以考虑单独定义一个输出纯字符串的函数，可以打印错误信息 */
            _cat_print_error(CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT);
            ret = CAT_ERROR;
            break;
        }

        if(CAT_ERROR == ret)
        {
            break;
        }
    }

    va_end(ap);

    return ret;
}

/**
 * @brief 和printf差不多，只不过将数据放进buf中
 * 
 * @param  buf              要输出到的缓冲区
 * @param  format           格式化字符串
 * @param  with_end         是否带结束符
 * @param  ...              输出的参数列表
 * @return cat_int32_t          成功失败
 */
cat_int32_t cat_sprintf(cat_uint8_t *buf, cat_bool_t with_end, const cat_uint8_t *format, ...)
{
    cat_int32_t ret = CAT_EOK;
    va_list ap;
    cat_uint8_t *p = NULL;          /**< 用来遍历format字符串 */
    cat_uint8_t wid_buf[5] = {0};   /**< 用于保存宽度的字符串 */
    cat_int32_t width = 0;          /**< 输出宽度(目前仅用于整型输出) */

    cat_uint8_t keep_buf[5] = {0};  /**< 用于保存保留小数位数的字符串 */
    cat_int32_t keep = 0;           /**< 保留位数 */

    cat_uint8_t hex_str[11] = {0};  /**< 用于保存转为十六进制的字符串 */

    cat_uint32_t buf_idx = 0;       /**< 访问缓冲区 */

    if(NULL == format)
    {
        ret = CAT_ERROR;
        return ret;
    }

    /* 获取参数 */
    va_start(ap, format);

    /* 挨个处理 */
    p = (cat_uint8_t *)format;
    while('\0' != *p)
    {
        if('%' != *p)
        {
            /* 发现'%'就开始处理格式化输出内容 */
            //cat_putchar(*p);
            buf[buf_idx++] = *p;
            p++;
            continue;
        }
        else
        {
            p++;
        }

        /* 获得宽度数据(如果有) */
        if(
            ((*p) >= '0') &&
            ((*p) <= '9')
        )
        {
            wid_buf[width++] = *p;
            p++;
        }
        if(width > 0)
        {
            wid_buf[width] = '\0';
            cat_atoi(&width, wid_buf);
        }

        /* 获得小数保留位数, 0 和 1 均保留一位小数 */
        keep = 0; /* 必须在外面复位, 否则如果在一次非零 keep 之后
                   * 未声明保留位数就会整成上一次的, 缺省保留一位小数 */
        if('.' == *p)
        {
            p++;
            
            while(
                ((*p) >= '0') &&
                ((*p) <= '9')
            )
            {
                keep_buf[keep++] = *p;
                p++;
            }
            if(keep > 0)
            {
                cat_int32_t keep_int;

                keep_buf[keep] = '\0';
                cat_atoi(&keep_int, keep_buf);

                keep = (cat_uint8_t)keep_int;
                
                keep_buf[0] = '\0';
            }
        }


        switch (*p)
        {
        case 'd':
        {
            p++;
            _cat_sprint_int(buf, &buf_idx, va_arg(ap, cat_int32_t), width);
            width = 0;
            break;
        }
        case 's':
        {
            p++;
            _cat_sprint_string(buf, &buf_idx, va_arg(ap, cat_uint8_t *));
            break;
        }
        case 'x':
        {
            p++;
            cat_itoh(hex_str, va_arg(ap, cat_uint32_t));
            if('\0' != hex_str[2])
            {
                _cat_sprint_string(buf, &buf_idx, hex_str);
            }
            else
            {
                _cat_sprint_string(buf, &buf_idx, hex_str);
                //cat_putchar('0');
                buf[buf_idx++] = '0';
            }
            break;
        }
        case 'f':
        {
            p++;
            _cat_sprint_float(buf, &buf_idx, va_arg(ap, cat_float_t), keep, width);
            break;
        }

        default:
            /* 可以考虑单独定义一个输出纯字符串的函数，可以打印错误信息 */
            _cat_print_error(CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT);
            ret = CAT_ERROR;
            break;
        }

        if(CAT_ERROR == ret)
        {
            break;
        }
    }

    va_end(ap);

    if(CAT_TRUE == with_end)
    {
        buf[buf_idx] = '\0';
    }

    return ret;
}

