/**
 * @file cat_intr.c
 * @author mio (648137125@qq.com)
 * @brief 中断管理
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "cat_intr.h"

#include "cat_log.h"
#include "cat_task.h"
#include "cat_assert.h"

#include "port.h"

/** 系统时钟 START */
/* vars */
cat_ubase catos_systicks;    /**< 系统时钟数 */

static volatile cat_ubase cat_irq_register_backup = 0;/**< 关中断时中断屏蔽寄存器值 */
static volatile cat_u8 cat_irq_disable_nest = 0;/**< 关中断嵌套层数(以支持递归关中断 TODO:手册要案例解释) */

static volatile cat_u8 cat_intr_nest = 0;/**< 中断嵌套层数 */

cat_bool catos_is_scheduling(void); /* from cat_task.c */

/* funcs */
/**
 * @brief 初始化系统tick数
 * 
 */
void cat_systick_init(void)
{
    catos_systicks = 0;
}
/** 系统时钟 END */
/* 中断相关接口 START */

/**
 * @brief 开中断(可嵌套)
 * 
 */
void cat_irq_enable(void)
{
    /* TODO:此处若nest==0,则应有一个WARN_LOG */
    if(cat_irq_disable_nest > 0)
    {
        // CLOG_TRACE("<--irq:%d", cat_irq_disable_nest);
        cat_irq_disable_nest--;

        if(0 == cat_irq_disable_nest)
        {
            _cat_hw_irq_enable(cat_irq_register_backup);
        }
    }
}

/**
 * @brief 关中断(可嵌套)
 * 
 */
void cat_irq_disable(void)
{
    if(0 == cat_irq_disable_nest)
    {
        cat_irq_register_backup = _cat_hw_irq_disable();
    }

    // CLOG_TRACE("-->irq:%d", cat_irq_disable_nest);
    CAT_ASSERT(cat_irq_disable_nest < 255);

    cat_irq_disable_nest++;
}

/**
 * @brief 进入中断服务函数
 * 
 */
void cat_intr_enter(void)
{
    cat_irq_disable();
    cat_intr_nest++;
    cat_irq_enable();
}
/**
 * @brief 离开中断服务函数
 * 
 */
void cat_intr_leave(void)
{
    cat_irq_disable();
    cat_intr_nest--;
    cat_irq_enable();
}
/* 中断相关接口 END */
/* 中断处理函数 START */
/**
 * @brief 系统时钟中断处理函数
 *        处理tick等
 */
void cat_intr_systemtick_handler(void)
{
    if (CAT_TRUE == catos_is_scheduling())
    {
        /* 检查栈溢出 */
        cat_task_t *cur = cat_task_get_current();
        if(
            ((cat_ubase)cur->sp < (cat_ubase)cur->stack_start_addr) ||
            ((cat_ubase)cur->sp > ((cat_ubase)cur->stack_start_addr + cur->stack_size))
        )
        {
            CLOG_ERROR("task %s stack overflow", cur->task_name);
        }

        cat_irq_disable();
        /* 处理等待的任务 */
        cat_task_delay_deal();
        /* 系统tick数 */
        catos_systicks++;
        cat_irq_enable();
        
        /* 进行一次调度 */
        cat_task_sched();
    }
}

/**
 * @brief 默认中断服务函数
 * 
 */
void cat_intr_default_handler(cat_u32 ipsr_val)
{
#if 0
    cat_u32 irq_num = vector & 0x1f;
    cat_u32  exti_pr_reg = *((cat_u32 *)(0x40010414));
    cat_kprintf("cat_intr_default_handler triggered, vec=%d, irq=%d, exti_pr=%d\r\n", vector, irq_num, exti_pr_reg);
#endif
    cat_intr_enter();
    /* 减去不可编程的向量数得到向量号 */
    cat_u32 vector = ipsr_val - 16;

    CLOG_INFO("vector_id = %d", vector);
    (void)vector;

    cat_intr_leave();
}
/* 中断处理函数 END */
