/**
 * @file cat_intr.h
 * @author mio (648137125@qq.com)
 * @brief 中断管理
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_INTR_H
#define CAT_INTR_H

#include "catos_types.h"
#include "catos_config.h"

extern cat_uint32_t catos_systicks;

/**
 * @brief 获取时钟数
 * 
 */
#define CATOS_GET_SYSTICK() \
    (catos_systicks)

/**
 * @brief 初始化系统tick数
 * 
 */
void cat_systick_init(void);

/**
 * @brief 系统时钟中断处理函数
 *        处理tick等
 */
void cat_intr_systemtick_handler(void);

/**
 * @brief 默认中断服务函数
 * 
 */
void cat_intr_default_handler(cat_uint32_t ipsr_val);

#endif
