/**
 * @file cat_sp_task.h
 * @author mio (648137125@qq.com)
 * @brief 固定优先级任务
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_SP_TASK_H
#define CAT_SP_TASK_H

#include "../basic_task/cat_basic_task.h"

#include "catos_types.h"
#include "catos_config.h"

extern struct _cat_task_t *cat_sp_cur_task;                /**< 当前任务的指针 */
extern struct _cat_task_t *cat_sp_next_task;                  /**< 要切换到的任务的指针 */

/**
 * @brief 初始化固定优先级任务调度
 * 
 */
void cat_sp_task_scheduler_init(void);

/**
 * @brief 创建固定优先级任务
 * 
 * @param task_name             任务名
 * @param task                  任务结构体
 * @param entry                 入口函数(无限循环)
 * @param arg                   参数
 * @param prio                  优先级
 * @param stack_start_addr      堆栈起始地址
 * @param stack_size            堆栈大小
 */
void cat_sp_task_create(
    const uint8_t *task_name,
    struct _cat_task_t *task, 
    void (*entry)(void *), 
    void *arg, 
    uint8_t prio, 
    void *stack_start_addr,
    uint32_t stack_size
);

/**
 * @brief 在开始第一个任务之前要调用该函数处理
 * 
 */
void cat_sp_task_before_start_first(void);

/**
 * @brief 获取最高优先级任务
 * 
 * @return struct _cat_task_t* NULL:无就绪任务; !NULL:就绪任务指针
 */
struct _cat_task_t *cat_sp_task_highest_ready(void);

/**
 * @brief 处理等待队列
 * 
 */
void cat_sp_task_delay_deal(void);

/**
 * @brief 进行一次调度
 * 
 */
void cat_sp_task_sched(void);

/**
 * @brief 允许调度
 * 
 */
void cat_sp_task_sched_enable(void);

/**
 * @brief 允许调度并且该操作不会包含一次调度
 * 
 */
void cat_sp_task_sched_enable_without_sched(void);

/**
 * @brief 禁止调度
 * 
 */
void cat_sp_task_sched_disable(void);

/**
 * @brief 将任务放入就绪表
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_rdy(struct _cat_task_t *task);

/**
 * @brief 将任务从就绪表移除
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_unrdy(struct _cat_task_t *task);

/**
 * @brief 当前任务等待
 * 
 * @param time 需要等待的tick数
 */
void cat_sp_task_delay(uint32_t time);

/**
 * @brief 将等待的任务从延时队列取出并挂入就绪表
 * 
 * @param task 等待的任务
 */
void cat_sp_task_delay_wakeup(struct _cat_task_t *task);

/**
 * @brief 挂起任务
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_suspend(struct _cat_task_t *task);

/**
 * @brief 唤醒任务
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_suspend_wakeup(struct _cat_task_t *task);

#endif


