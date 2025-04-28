/**
 * @file cat_mem.h
 * @brief 
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2024-04-03
 * Change Logs:
 * Date           Author        Notes
 * 2024-04-03     mio     first verion
 * 
 */
#ifndef _CAT_MEM_H
#define _CAT_MEM_H
#include "catos_types.h"

void *cat_malloc(cat_ubase bytes);

cat_err cat_free(void *ptr);

void cat_mem_print_info(void);

#endif