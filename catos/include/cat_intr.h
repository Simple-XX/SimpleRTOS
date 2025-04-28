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

extern cat_ubase catos_systicks;

static inline cat_ubase catos_get_systick(void)
{
    return catos_systicks;
}

static inline cat_ubase catos_ms_to_tick(cat_u32 ms)
{
    return (cat_ubase)(ms / CATOS_SYSTICK_MS);
}

void cat_systick_init(void);

void cat_irq_enable(void);
void cat_irq_disable(void);

void cat_intr_enter(void);
void cat_intr_leave(void);

void cat_intr_systemtick_handler(void);
void cat_intr_default_handler(cat_u32 ipsr_val);

#endif
