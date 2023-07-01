/**
 * @file cat_support.h
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

#ifndef CAT_STRCMP_H
#define CAT_STRCMP_H

#include "catos_types.h"

/**
 * @brief 比较字符串是否相同
 * @param str1 
 * @param str2 
 * @return cat_uint32_t 0:equal, -1:not equal, -2:NULL args
 */
cat_int32_t cat_strcmp(const cat_uint8_t *str1, const cat_uint8_t *str2);

/**
 * @brief 字符串拷贝
 * @param dest 
 * @param src 
 * @return cat_int32_t 0:成功，else:失败
 */
cat_int32_t cat_strcpy(cat_uint8_t *dest, const cat_uint8_t *src, cat_uint32_t dest_len);

/**
 * @brief 字符串长度
 * 
 * @param  src              My Param doc
 * @return cat_uint32_t     长度
 */
cat_uint32_t cat_strlen(const cat_uint8_t *src);

/**
 * @brief 字符串转有符号32
 * @param dest 
 * @param src 
 * @return cat_int32_t 0:成功，else:失败
 */
cat_int32_t cat_atoi(cat_int32_t *dest, const cat_uint8_t *src);

/**
 * @brief 有符号32转字符串
 * @param dest 
 * @param src 
 * @return cat_int32_t 0:成功，else:失败
 */
cat_int32_t cat_itoa(cat_uint8_t *dest, cat_int32_t src);


/**
 * @brief 十六进制转有符号十进制
 * 
 * @param dest 
 * @param src 
 * @return cat_int32_t 
 */
cat_int32_t cat_htoi(cat_uint32_t *dest, const cat_uint8_t *src);

/**
 * @brief 十进制转十六进制字符串
 * 
 * @param dest 
 * @param src 
 * @return cat_int32_t 
 */
cat_int32_t cat_itoh(cat_uint8_t *dest, cat_uint32_t src);

/**
 * @brief 格式化内存块
 * 
 * @param start_addr    内存块起始地址
 * @param fill_with     填充的字符
 * @param size          内存块大小
 * @return void* 
 */
void *cat_memset(void *start_addr, cat_uint8_t fill_with, cat_uint32_t size);



#endif

