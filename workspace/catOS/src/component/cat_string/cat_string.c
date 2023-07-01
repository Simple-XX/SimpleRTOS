/**
 * @file cat_support.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-09
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-09 1.0    amoigus             内容
 */

#include "cat_string.h"
#include "cat_stdio.h"

#include "cat_error.h"

/****************** macros *******************/
#define CAT_SUPPORT_INT_LONG 10


/*************************************/

/****************** funcs *******************/
cat_int32_t cat_strcmp(const cat_uint8_t *str1, const cat_uint8_t *str2)
{
    cat_int32_t ret = CAT_EOK;

    if((str1 == NULL) || (str2 == NULL))
    {
        CAT_SYS_PRINTF("[cat_strcmp] at least one arg is NULL!\r\n");
    }
    else
    {
        while((*str1 == *str2) && (*str1 != '\0'))
        {
            str1++;
            str2++;
        }
        
        if((*str1 == '\0') && (*str2 == '\0'))
        {
            ret = CAT_EOK;
        }
        else
        {
            ret = CAT_ERROR;
        }
    }

    return ret;
}

cat_int32_t cat_strcpy(cat_uint8_t *dest, const cat_uint8_t *src, cat_uint32_t dest_len)
{
    //cat_uint8_t *p = src;
    cat_int32_t ret = CAT_EOK;
    CAT_ASSERT(NULL != dest);
    CAT_ASSERT(NULL != src);
    cat_uint32_t i = 0;
    while(
        (src[i] != '\0') &&
        (i < dest_len)
    )
    {
        dest[i] = src[i];
        i++;
    }

    return ret;
}

cat_uint32_t cat_strlen(const cat_uint8_t *src)
{
    cat_uint32_t i = 0;

    while('\0' != src[i])
    {
        i++;
    }

    return i;
}

cat_int32_t cat_atoi(cat_int32_t *dest, const cat_uint8_t *src)
{
    CAT_ASSERT(dest);
    CAT_ASSERT(src);

    cat_int32_t ret = CAT_EOK;
    cat_int32_t temp = 0;
    cat_uint8_t sign = src[0];

    if(('-' == *src) || ('+' == *src))
    {
        src++;
    }
    while('\0' != *src)
    {
        if((*src < '0') || (*src > '9'))
        {
            ret = CAT_ERROR;
            break;
        }
        temp = temp * 10 + (*src - '0');
        src++;
    }
    if('-' == sign)
    {
        temp = -temp;
    }
		
		*dest = temp;

    return ret;
}

cat_int32_t cat_itoa(cat_uint8_t *dest, cat_int32_t src)
{
	(void)dest;
	(void)src;
	cat_int32_t ret = CAT_EOK;

#if 0
    cat_uint32_t tmp = 0;
    /* cat_uint32_t max val = 4294967295 */
    cat_uint8_t buf[CAT_SUPPORT_INT_LONG] = {0};
    cat_uint8_t i = 0;

    if(num < 0)
    {
        *(dest++) = '-';
        num = -num;
    }

    /* 计算每一位的数字大小 */
    do
    {
        buf[i++] = num % 10;
        num = num / 10;

    }while((0 != num) && (i < CAT_SUPPORT_INT_LONG));

    /* 记录长度 */
    ret = i;

    while(i > 0)
    {
        *(dest++) = buf[--i] + '0';
    }
    *(dest) = '\0';
#endif //#if 0

	return ret;
}

/**
 * @brief 十六进制转无符号十进制
 * 
 * @param dest 
 * @param src 
 * @return cat_int32_t 
 */
cat_int32_t cat_htoi(cat_uint32_t *dest, const cat_uint8_t *src)
{
    CAT_ASSERT(dest);
    CAT_ASSERT(src);

    cat_int32_t ret = CAT_EOK;
    cat_uint32_t temp = 0;

    //0x12
    if(
        (src[0] != '0') ||
        ((src[1] != 'x') && (src[1] != 'X'))
    )
    {
        ret = CAT_ERROR;
    }
    else
    {
        src += 2; //跳过0x
        while('\0' != *src)
        {
            if((*src >= '0') && (*src <= '9'))
            {
                temp = temp * 16 + (*src - '0');
                src++;
            }
            else if((*src >= 'A') && (*src <= 'F'))
            {
                temp = temp * 16 + (*src - 'A' + 10);
                src++;
            }
            else if((*src >= 'a') && (*src <= 'f'))
            {
                temp = temp * 16 + (*src - 'a' + 10);
                src++;
            }
            else
            {
                ret = CAT_ERROR;
                break;
            }
        } /* while */
    } /* else */

    if(CAT_EOK == ret)
    {
        *dest = temp;
    }

    return ret;
}

/**
 * @brief 十进制转十六进制字符串
 * 
 * @param dest 
 * @param src 
 * @return cat_int32_t 
 * 
 * note: 先从低位到高位计算出字符串，再反转除 "0x" 的字符串
 */
cat_int32_t cat_itoh(cat_uint8_t *dest, cat_uint32_t src)
{
    CAT_ASSERT(dest);

    cat_int32_t ret = CAT_EOK;
    cat_uint8_t temp = 0;
    cat_uint8_t *start;

    start = dest + 2;   /* 16进制的起始位置 */

    *(dest++) = '0';
    *(dest++) = 'x';

    while(src != 0)
    {
        temp = src % 16;
        src /= 16;
        if(temp < 10)
        {
            *dest = temp + '0';
        }
        else
        {
            *dest = temp - 10 + 'A';
        }
        
        dest++;
    }

    *(dest--) = '\0';   /* 这之后要反转字符串，所以避开 "\0" */

    while(start < dest)
    {
        temp   = *start;
        *start = *dest;
        *dest  = temp;

        start++;
        dest--;
    }

    return ret;
}

/**
 * @brief 格式化内存块
 * 
 * @param start_addr    内存块起始地址
 * @param fill_with     填充的字符
 * @param size          内存块大小
 * @return void* 
 */
void *cat_memset(void *start_addr, cat_uint8_t fill_with, cat_uint32_t size)
{
/* 最大块长度(与架构位数有关) */
#define LONG_BLK_SZ     (sizeof(cat_uint32_t))
/* 未对齐(目标地址低于最大块长度的位有非零位) */
#define UNALIGNED(_addr)    ((cat_uint32_t)_addr & (LONG_BLK_SZ - 1))
/* 内存块太小 */
#define TOO_SMALL(_size)    ((_size) < LONG_BLK_SZ)

    /**< 对齐的地址 */
    cat_uint32_t *aligned_addr;     
    /**< 没对齐或者碎片地址 */
    cat_uint8_t *piece_addr = (cat_uint8_t *)start_addr;   
    /**< 4字节填充缓冲 */    
    cat_uint32_t word_fill_buff;    

    if(
        !(TOO_SMALL(size)) &&
        !UNALIGNED(start_addr)
    )
    {
        aligned_addr = start_addr;

        if(LONG_BLK_SZ == 4)
        {
            /* word_fill_buff = |fill_with|fill_with|fill_with|fill_with| */
            word_fill_buff = (cat_uint32_t)(fill_with << 8) | fill_with;
            word_fill_buff |= (word_fill_buff << 16);
        }
        else
        {
            /* 目前只支持32位 */
            CAT_ASSERT(LONG_BLK_SZ == 4);
        }

        /* 16字节填充 */
        while(size >= (LONG_BLK_SZ << 2))
        {
            *aligned_addr++ = word_fill_buff;
            *aligned_addr++ = word_fill_buff;
            *aligned_addr++ = word_fill_buff;
            *aligned_addr++ = word_fill_buff;
            size -= (LONG_BLK_SZ << 2);
        }
        /* 4字节填充 */
        while(size >= LONG_BLK_SZ)
        {
            *aligned_addr++ = word_fill_buff;
            size -= LONG_BLK_SZ;
        }

        /* 获取剩余的内存起始地址 */
        piece_addr = (cat_uint8_t *)aligned_addr;
    }

    while(size--)
    {
        *piece_addr++ = (cat_uint8_t)fill_with;
    }

    return start_addr;

/* 取消宏定义 */
#undef LONG_BLK_SZ
#undef UNALIGNED
#undef TOO_SMALL
}
