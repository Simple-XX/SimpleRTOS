/**
 * @file cat_lib.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-23
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-23 1.0    amoigus             位图
 */

#ifndef CAT_LIB_H
#define CAT_LIB_H

#include "catos_types.h"
#include "catos_config.h"

/* 位图数据结构 */
struct _cat_bitmap
{
    cat_uint32_t bitmap;
};
typedef struct _cat_bitmap cat_bitmap;

/**
 * @brief 获取位图位数
 */
cat_uint32_t cat_bitmap_get_wide(void);

/**
 * @brief 初始化位图
 * 
 * @param bitmap 位图指针
 */
void cat_bitmap_init(cat_bitmap *bitmap);

/**
 * @brief 置位
 * 
 * @param bitmap    位图指针
 * @param pos       位置
 */
void cat_bitmap_set(cat_bitmap *bitmap, cat_uint32_t pos);

/**
 * @brief 复位
 * 
 * @param bitmap    位图指针
 * @param pos       位置
 */
void cat_bitmap_clr(cat_bitmap *bitmap, cat_uint32_t pos);

/**
 * @brief 获取最低非零位
 * 
 * @param bitmap        位图指针
 * @return cat_uint32_t     最低非零位
 */
cat_uint32_t cat_bitmap_get_first_set(cat_bitmap *bitmap);

#endif
