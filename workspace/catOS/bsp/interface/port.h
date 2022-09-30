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

#define MEM32(addr)     *(volatile uint32_t *)(addr)
#define MEM8(addr)      *(volatile uint8_t  *)(addr)


/**
 * @brief 硬件初始化
 */
void cat_hw_init(void);

/**
 * @brief 开始调度
 * 
 */
void catos_start_sched(void);

/**
 * @brief 上下文切换
 * 
 */
void cat_hw_context_switch(void);

/**
 * @brief 关中断进临界区
 * 
 * @return uint32_t 
 */
uint32_t cat_hw_irq_disable(void);

/**
 * @brief 开中断出临界区
 * 
 * @param status 
 */
void cat_hw_irq_enable(uint32_t status);

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return uint8_t*     初始化后的栈顶地址
 */
uint8_t *cat_hw_stack_init(void *task_entry, void *parameter, uint8_t *stack_addr, void *exit);

#endif
