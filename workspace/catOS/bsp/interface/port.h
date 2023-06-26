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

#define MEM32(addr)     *(volatile cat_uint32_t *)(addr)
#define MEM8(addr)      *(volatile cat_uint8_t  *)(addr)

/******* board port START ********/
/**
 * @brief 硬件初始化
 */
void cat_hw_init(void);

/**
 * @brief 开始调度
 * 
 */
void catos_start_sched(void);
/******* board port END ********/

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
 */
void cat_hw_context_switch(cat_uint32_t from_task_sp_addr, cat_uint32_t to_task_sp_addr);

/**
 * @brief 切换到第一个任务的上下文
 * 
 * @param  first_task_sp_addr  要切换的任务tcb中堆栈指针变量的 *地址*
 */
void cat_hw_context_switch_to_first(cat_uint32_t first_task_sp_addr);

/**
 * @brief 关中断进临界区
 * 
 * @return cat_uint32_t 
 */
cat_uint32_t cat_hw_irq_disable(void);

/**
 * @brief 开中断出临界区
 * 
 * @param status 
 */
void cat_hw_irq_enable(cat_uint32_t status);

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return cat_uint8_t*     初始化后的栈顶地址
 */
cat_uint8_t *cat_hw_stack_init(void *task_entry, void *parameter, cat_uint8_t *stack_addr, void *exit);
/******* cpu_port END ********/

#endif
