/**
 * @file port.h
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

#ifndef CAT_PORT_H
#define CAT_PORT_H

#include "catos_config.h"
#include "catos_types.h"


/**
 * @brief 等待微秒
 *        !!阻塞等待, TODO:如果时钟中断频率提高到微秒级别要修改
 *        !!bug:多任务一起用会出问题, 毕竟定时器只有一个
 * @param  us 等待的微秒数
 */
void cat_delay_us(cat_u32 us);

/* 初始化以及开启系统时钟中断 */
void cat_hw_start_systick(void);


/******* cpu_port START ********/
/**
 * @brief 上下文切换
 * 
 */
//void cat_hw_context_switch(void);
/**
 * @brief 上下文切换
 * 
 * @param  from_task_sp_addr 上一个任务tcb中堆栈指针变量的 *地址*
 * @param  to_task_sp_addr   下一个任务tcb中堆栈指针变量的 *地址*
 *   !注意：这里是task中sp的指针，即&(task->sp)
 */
void cat_hw_context_switch(cat_ubase from_task_sp_addr, cat_ubase to_task_sp_addr);

/**
 * @brief 切换到第一个任务的上下文
 * 
 * @param  first_task_sp_addr  要切换的任务tcb中堆栈指针变量的 *地址*
 *  !注意：这里是task中sp的指针，即&(task->sp)
 */
void cat_hw_context_switch_to_first(cat_ubase first_task_sp_addr);

/**
 * @brief 关中断(使用时请调用cat_irq_disable())
 * 
 * @return cat_u32 
 */
cat_ubase _cat_hw_irq_disable(void);

/**
 * @brief 开中断(使用时请调用cat_irq_enable())
 * 
 * @param status 
 */
void _cat_hw_irq_enable(cat_ubase status);

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return cat_u8*     初始化后的栈顶地址
 */
cat_u8 *cat_hw_stack_init(void (*task_entry)(void *), void *arg, cat_u8 *stack_addr, void  (*exit_func)(void *));
/******* cpu_port END ********/

#endif
