/**
 * @file cat_basic_task.h
 * @author mio (648137125@qq.com)
 * @brief 基础任务定义，仅作为其他任务调度器的基础被使用，用户不使用
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_BASIC_TASK_H
#define CAT_BASIC_TASK_H

#include "../../support/cat_list.h"

#include "catos_types.h"
#include "catos_config.h"


/** 宏定义 */
/* 任务状态 */
#define CATOS_TASK_STATE_RDY        0               //目前不具备判断功能
#define CATOS_TASK_STATE_DESTROYED  (1 << 1)        //删除
#define CATOS_TASK_STATE_DELAYED    (1 << 2)        //延时
#define CATOS_TASK_STATE_SUSPEND    (1 << 3)        //挂起
#define CATOS_TASK_EVENT_MASK       (0xff << 16)    //高16位用作事件相关的状态

/* 调度策略 */
#define SCHED_STRATEGY_STATIC_PRIO  0               /**< 固定优先级调度 */

/** 数据结构定义 */
/* 任务控制块 */
struct _cat_task_t
{
    void               *sp;                             /**< 栈顶(堆栈指针)*/
    uint8_t            *task_name;                      /**< 任务名称*/
    uint8_t             sched_strategy;                 /**< 调度策略 */


    void               *entry;                          /**< 入口函数 */
    void               *arg;                            /**< 入口函数的参数 */
    void               *stack_start_addr;               /**< 堆栈起始地址*/
    uint32_t            stack_size;                     /**< 堆栈大小*/

    struct _cat_node_t  link_node;                      /**< 任务表中的链表节点，也用于delay链表*/
    uint32_t            delay;                          /**< 延时剩余tick数*/

    uint32_t            state;                          /**< 当前状态*/

    uint8_t             prio;                           /**< priority of task*/
    uint32_t            slice;                          /**< 时间片(剩余时间)*/
    uint32_t            suspend_cnt;                    /**< 被挂起的次数*/

    uint32_t            sched_times;                    /**< 调度次数*/

    

};


/** 函数声明 */

/**
 * @brief                       初始化任务，仅根据参数初始化数据结构，并不进行策略特定的操作(如挂就绪队列)
 * 
 * 
 * @param task                  任务控制块指针
 * @param entry                 入口函数指针
 * @param arg                   入口函数的参数
 * @param prio                  任务优先级
 * @param stack_start_addr      堆栈起始地址
 * @param stack_size            堆栈大小
 */
void cat_task_init(
    const uint8_t *task_name,
    struct _cat_task_t *task, 
    void (*entry)(void *), 
    void *arg, 
    uint8_t prio, 
    void *stack_start_addr,
    uint32_t stack_size,
    uint32_t sched_strategy
);

#endif
