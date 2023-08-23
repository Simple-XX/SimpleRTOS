/**
 * @file delay.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-23
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-23 <td>内容
 * </table>
 */
#ifndef DELAY_H
#define DELAY_H

#include "cat_delay.h"

#define delay_us(us) cat_delay_us(us)
#define delay_ms(ms) cat_delay_ms(ms)

#endif
