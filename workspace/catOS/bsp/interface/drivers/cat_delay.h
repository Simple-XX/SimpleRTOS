/**
 * @file cat_delay.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-05
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-05 <td>内容
 * </table>
 */
#ifndef CAT_DELAY_H
#define CAT_DELAY_H

#include "catos_config.h"
#include "catos_types.h"

void cat_delay_us(cat_uint32_t us);

void cat_delay_ms(cat_uint32_t ms);

// void cat_delay_systick(cat_uint32_t tick);

#endif
