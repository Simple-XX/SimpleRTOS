/**
 * @file drv_delay.c
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
#include "cat_task.h"
#include "drv_delay.h"
#include "cat_intr.h"

#include "stm32f1xx.h"

static void cat_delay_systick(cat_uint32_t tick)
{
    cat_uint32_t tick_start, tick_cnt, tick_now;

    tick_start = CATOS_GET_SYSTICK();

    while(tick_cnt < tick)
    {
        tick_now = CATOS_GET_SYSTICK();
        tick_cnt = tick_now - tick_start;
    }

}

void cat_delay_us(cat_uint32_t us)
{
    cat_uint32_t tim_tick;
    cat_uint32_t told, tnow, tcnt = 0;
    cat_uint32_t reload = SysTick->LOAD;

    if(0 != us)
    {
        tim_tick = us * reload / (1000000 / CATOS_SYSTICK_FRQ);
        told = SysTick->VAL;

        while(1)
        {
            tnow = SysTick->VAL;
            
            if(tnow != told)
            {
                if(tnow < told)
                {
                    tcnt += told - tnow;
                }
                else
                {
                    tcnt += reload - tnow + told;
                }

                told = tnow;

                if(tcnt >= tim_tick)
                {
                    break;
                }
            }
        }
    }
}

void cat_delay_ms(cat_uint32_t ms)
{
    if(NULL != cat_sp_cur_task)
    {
        /* 说明已经开始调度 */
        if(ms >= CATOS_SYSTICK_MS)
        {
            /* 要等待的时间大于一个系统时钟的时间 */
            cat_sp_task_delay(ms/CATOS_SYSTICK_MS);
            ms %= CATOS_SYSTICK_MS;
        }
    }

    cat_delay_us(ms * 1000);
}
