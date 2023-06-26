/**
 * @file cat_lib.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-23
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-23 1.0    amoigus             内容
 */

#include "cat_bitmap.h"

/**************位图 start****************/

cat_uint32_t cat_bitmap_get_wide(void)
{
    return 32;
}

void cat_bitmap_init(cat_bitmap *bitmap)
{
    bitmap->bitmap = 0;
}

void cat_bitmap_set(cat_bitmap *bitmap, cat_uint32_t pos)
{
    bitmap->bitmap |= (1 << pos);
}

void cat_bitmap_clr(cat_bitmap *bitmap, cat_uint32_t pos)
{
    bitmap->bitmap &= ~(1 << pos);
}

cat_uint32_t cat_bitmap_get_first_set(cat_bitmap *bitmap)
{
    static const cat_uint8_t quick_find_table[] = 
    {
        0xffu,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x00to0x0F*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x10to0x1F*/
        5u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x20to0x2F*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x30to0x3F*/
        6u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x40to0x4F*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x50to0x5F*/
        5u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x60to0x6F*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x70to0x7F*/
        7u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x80to0x8F*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0x90to0x9F*/
        5u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0xA0to0xAF*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0xB0to0xBF*/
        6u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0xC0to0xCF*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0xD0to0xDF*/
        5u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u,/*0xE0to0xEF*/
        4u,0u,1u,0u,2u,0u,1u,0u,3u,0u,1u,0u,2u,0u,1u,0u /*0xF0to0xFF*/
    };

    if(bitmap->bitmap & 0xff)
    {
        return quick_find_table[bitmap->bitmap & 0xff];
    }
    else if(bitmap->bitmap & 0xff00)
    {
        return quick_find_table[(bitmap->bitmap >> 8) & 0xff] + 8;
    }
    else if(bitmap->bitmap & 0xff0000)
    {
        return quick_find_table[(bitmap->bitmap >> 16) & 0xff] + 16;
    }
    else if(bitmap->bitmap & 0xff000000)
    {
        return quick_find_table[(bitmap->bitmap >> 24) & 0xff] + 24;
    }
    else
    {
        return cat_bitmap_get_wide();
    }
}

/**************位图 end****************/
