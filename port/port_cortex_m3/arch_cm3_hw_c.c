/**
 * @file arch_xx_hw_c.c
 * @author 文佳源 (648137125@qq.com)
 * @brief catOS可移植硬件部分-c语言部分
 * @version 0.1
 * @date 2025-03-10
 *
 * Copyright (c) 2025
 *
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-10 <td>内容
 * </table>
 */
#include "catos_config.h"
#include "catos_types.h"

/* 硬件相关宏定义 */
/* ARCH_CM3_SHP ==  0xe000ed18 cortex-m3 的系统异常优先级寄存器 */
#define ARCH_CM3_SHP_3           (*((volatile cat_u32 *)0xe000ed20))  /* 第3个32四字节寄存器(主要是为了每次访问4字节地址) */

/* !!PENDSV中断一定要是系统中最低优先级 */
#define ARCH_CM3_PENDSV_SHP3_OFFS     16          /* pendsv 优先级配置寄存器 (8bit) 在SHP_3中的偏移量 */
#define ARCH_CM3_PENDSV_PREEMPT_PRIO  15          /* pendsv 抢占优先级 */
#define ARCH_CM3_PENDSV_SUB_PRIO      0           /* pendsv 子优先级 */
#define PENDSV_PRIO (((ARCH_CM3_PENDSV_PREEMPT_PRIO << 4) | ARCH_CM3_PENDSV_SUB_PRIO) << ARCH_CM3_PENDSV_SHP3_OFFS) /* 写入到寄存器的 pendsv 优先级 */

#define ARCH_CM3_SYSTICK_SHP3_OFFS    24          /* PENDSV 优先级配置寄存器 (8bit) 在SHP_3中的偏移量 */
#define ARCH_CM3_SYSTICK_PREEMPT_PRIO 15          /* systick 抢占优先级 */
#define ARCH_CM3_SYSTICK_SUB_PRIO     0           /* systick 子优先级 */
#define SYSTICK_PRIO (((ARCH_CM3_SYSTICK_PREEMPT_PRIO << 4) | ARCH_CM3_SYSTICK_SUB_PRIO) << ARCH_CM3_SYSTICK_SHP3_OFFS) /* 写入到寄存器的 systick 优先级 */

#define ARCH_CM3_SYSTICK_CTRL    (*((volatile cat_u32 *)0xe000e010)) /* systick控制寄存器 */
#define ARCH_CM3_SYSTICK_LOAD    (*((volatile cat_u32 *)0xe000e014)) /* systick重装载寄存器 */
#define ARCH_CM3_SYSTICK_VAL     (*((volatile cat_u32 *)0xe000e018)) /* systick重装载寄存器 */

#define ARCH_CM3_SYSTICK_CTRL_CLKSOURCE_BIT (1UL << 2UL) /* systick时钟源控制位 (0:外部时钟源STCLK, 1:内核时钟FCLK) */
#define ARCH_CM3_SYSTICK_CTRL_INT_BIT       (1UL << 1UL) /* systick中断使能位 (0:无操作, 1:倒数至0时产生systick异常请求) */
#define ARCH_CM3_SYSTICK_CTRL_ENABLE_BIT    (1UL << 0UL) /* systick定时器使能位 */

void cat_hw_start_systick(void)
{
    /* cortexm-m3中要设置pendsv,设置并开启systick */

    /* 1. pendsv 设置 */
    // NVIC_SetPriority(PendSV_IRQn, 15); /* 设置pendsv优先级 */
    ARCH_CM3_SHP_3        |= PENDSV_PRIO; /* 设置pensv优先级 */

    /* 2. systick 设置 */
    ARCH_CM3_SYSTICK_CTRL  = 0; /* 清空控制寄存器 (清空设置、关闭定时器一步到位！) */

    // NVIC_SetPriority(SysTick_IRQn, 15); 
    ARCH_CM3_SHP_3        |= SYSTICK_PRIO; /* 设置systick优先级 */
    ARCH_CM3_SYSTICK_LOAD  = CATOS_SYSTICK_MS * CATOS_CLOCK_FRQ / 1000; /* 重载计数器值 */
    ARCH_CM3_SYSTICK_VAL   = 0;

    /* 设定systick控制寄存器 */
    ARCH_CM3_SYSTICK_CTRL = ARCH_CM3_SYSTICK_CTRL_CLKSOURCE_BIT | /* 设定为内核时钟FCLK */
                            ARCH_CM3_SYSTICK_CTRL_INT_BIT       | /* 设定为systick计数器倒数到0时触发中断 */
                            ARCH_CM3_SYSTICK_CTRL_ENABLE_BIT;     /* 开启定时器中断-在start_schedule中调用此函数前已经关闭了中断 */
}

#define NOPS_PER_MS (100000 / 18)
#define NOPS_PER_US (NOPS_PER_MS/1000)
#define _RUN_NOP() __asm__ __volatile__ ("nop"::)

void cat_delay_us(cat_u32 us)
{
    /**
     * 测试10^5个nop要跑180个tick(即约1800ms)
     * 因此速率为(10^5)/18 nop/ms
     */
    cat_ubase run_times = us * NOPS_PER_US;

    while (run_times > 0)
    {
        _RUN_NOP();
        run_times--;
    }
}