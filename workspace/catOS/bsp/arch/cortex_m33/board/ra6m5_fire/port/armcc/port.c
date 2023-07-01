/**
 * @file port.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-22
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-22 1.0    amoigus             内容
 */

#include "cat_task.h"
#include "cat_intr.h"
#include "port.h"

#include "../drivers/uart/cat_drv_uart.h"

#include "hal_data.h"

#define NVIC_INT_CTRL   0xE000ED04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SHPR3      0xE000ED20 /* System Handler Priority Register */
#define NVIC_PENDSV_PRI 0x00FF0000


#if defined(__GNUC__)
int __wrap_atexit(void __attribute__((unused)) (*function)(void)) {
    return -1;
}
#endif /* #if defined(__GNUC__) */

/* PRIVATE FUNCS DECL START */
static void SysTick_Init(cat_uint32_t IT_frequency);
static void cat_set_systick_period(cat_uint32_t ms);
/* PRIVATE FUNCS DECL END */

/**
 * @brief 硬件初始化
 */
void cat_hw_init(void)
{
    /* 设置系统时钟中断频率为100Hz(每秒100次) */
    cat_set_systick_period(CATOS_SYSTICK_MS);
}

/* ra6m5的时钟中断处理函数 */
void SysTick_Handler(void)
{
    /* 调用自己的处理函数 */
    cat_intr_systemtick_handler();
}

/**
 * @brief 开始调度
 * 
 */
void catos_start_sched(void)
{
    cat_uint32_t tmp_reg = 0;
    struct _cat_task_t *first_task = NULL;

    //cat_sp_task_before_start_first();
    /* 获取最高优先级任务 */
    first_task = cat_sp_task_highest_ready();

    /* 因为是第一个任务，不用像调度时判断是否和上一个任务一样，直接赋值给当前任务就行 */
    cat_sp_cur_task = first_task;

    /* 允许调度(打开调度锁，并且不在该处进行调度) */
    cat_sp_task_sched_enable_without_sched();

    /* 开启时钟中断 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    /* 设置pendsv中断优先级 */
    tmp_reg = MEM32(NVIC_SHPR3);
    tmp_reg |= NVIC_PENDSV_PRI;
    MEM32(NVIC_SHPR3) = tmp_reg;
    // NVIC_SetPriority(PendSV_IRQn, UINT8_MAX);

    /* 切换到第一个任务 */
    cat_hw_context_switch_to_first((cat_uint32_t)&(first_task->sp));
}


/* PRIVATE FUNCS DEF START */
/**
 * @brief 初始化时钟中断
 * 
 * @param  ms               每个tick的时间(ms)
 */
static void cat_set_systick_period(cat_uint32_t ms)
{
    cat_uint32_t err = 0;
    cat_uint32_t IT_Period = 0;

    IT_Period = ms * SystemCoreClock / 1000;

    //err = SysTick_Config(IT_Period);

    /* 如果设定的周期太离谱就停在这 */
    if ((IT_Period - 1UL) > SysTick_LOAD_RELOAD_Msk)
    {
        err = 1;
    }
    assert(0 == err);

    SysTick->LOAD  = (uint32_t)(IT_Period - 1UL);                         /* set reload register */
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | /* 设定为内核时钟FCLK */
                    SysTick_CTRL_TICKINT_Msk    | /* 设定为systick计数器倒数到0时触发中断 */
                    ~SysTick_CTRL_ENABLE_Msk;    /* 关闭定时器中断，若创建任务则在catos_start_sched()中开启该中断 */

}

/* PRIVATE FUNCS DEF END */


