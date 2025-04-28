/**
 * @file port_stub.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试时的可移植接口桩实现
 * @version 0.1
 * @date 2025-02-25
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-02-25 <td>内容
 * </table>
 */

#include "catos_config.h"
#include "catos_types.h"

#include <stdio.h>

void cat_hw_init(void){}

void catos_start_sched(void){}

void cat_delay_us(cat_u32 us){}

void cat_hw_context_switch(cat_ubase from_task_sp_addr, cat_ubase to_task_sp_addr)
{
    (void)from_task_sp_addr;
    (void)to_task_sp_addr;
}

void cat_hw_context_switch_to_first(cat_ubase first_task_sp_addr)
{
    (void)first_task_sp_addr;
}

cat_ubase _cat_hw_irq_disable(void){return 0;}

void _cat_hw_irq_enable(cat_ubase status)
{
    (void)status;
}

cat_u8 *cat_hw_stack_init(void (*task_entry)(void *), void *arg, cat_u8 *stack_addr, void  (*exit_func)(void *))
{
    (void)task_entry;
    (void)arg;
    (void)stack_addr;
    (void)exit_func;

    return CAT_NULL;
}