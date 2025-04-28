/**
 * @file cat_task.h
 * @author mio (648137125@qq.com)
 * @brief 任务控制的用户接口
 * @version 0.1
 * @date 2022-07-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CAT_TASK_H
#define CAT_TASK_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_lib.h"
#include "cat_ipc.h"

/** 宏定义 */

#define CATOS_TASK_STATE_OFFS     (0)
#define CATOS_TASK_IPC_OFFS       (16)
#define CATOS_TASK_STATE_MASK     (0xffff << CATOS_TASK_STATE_OFFS)    /* 低16位表示任务状态 */
#define CATOS_TASK_IPC_MASK       (0xffff << CATOS_TASK_IPC_OFFS)      /* 高16位用作ipc相关的状态 */

/* 任务状态 */
#define CATOS_TASK_STATE_RDY         0       /**< 就绪(判断直接判断是否相等即可) */
#define CATOS_TASK_STATE_DELETED    (1 << 1) /**< 删除 */
#define CATOS_TASK_STATE_DELAY    (1 << 2) /**< 延时 */
#define CATOS_TASK_STATE_SUSPEND    (1 << 3) /**< 挂起 */

/* 调度策略 */
#define SCHED_STRATEGY_STATIC_PRIO  0               /**< 固定优先级调度 */

/** 数据结构定义 */
/* 类型使用前向声明，见 catos_types.h */
struct _cat_task_t
{
    void               *sp;                             /**< 栈顶(堆栈指针)*/
    const char         *task_name;                      /**< 任务名称*/
    cat_u8              sched_strategy;                 /**< 调度策略 */


    void              (*entry)(void *);                 /**< 入口函数 */
    void               *arg;                            /**< 入口函数的参数 */
    void               *stack_start_addr;               /**< 堆栈起始地址*/
    cat_u32             stack_size;                     /**< 堆栈大小*/

    cat_node_t          ready_node;                     /**< 用于就绪相关处理的链表节点 */
    cat_dnode_t         time_node;                      /**< 用于时间相关处理的差分链表节点 */

    cat_u32             state;                          /**< 当前状态(高十六位为ipc状态, 低十六位为任务自身状态) */

    cat_u8              prio;                           /**< 任务优先级*/
    cat_u32             slice;                          /**< 时间片(剩余时间)*/
    cat_u32             suspend_cnt;                    /**< 被挂起的次数*/

    cat_u32             sched_times;                    /**< 调度次数*/

    cat_node_t          ipc_wait_node;                  /**< 用于等待ipc的链表节点 */
    cat_ipc_t          *ipc_wait;                      /**< 等待的ipc(TODO:支持同时等待多个?) */
    void               *ipc_msg;                       /**< 等待的ipc携带的消息 */
    cat_err             error;                          /**< 出现的错误 */

    cat_node_t          manage_node;                    /**< 用于管理的链表节点 */
};

cat_bool catos_is_scheduling(void);
cat_task_t *cat_task_get_current(void);
void cat_task_set_current(cat_task_t *task);
cat_task_t *cat_task_self(void);

void cat_task_create(
    const char *task_name,
    cat_task_t *task, 
    void      (*entry)(void *), 
    void       *arg, 
    cat_u8      prio, 
    void       *stack_start_addr,
    cat_u32     stack_size
);

cat_task_t *cat_task_get_highest_ready(void);
void cat_task_delay_deal(void);
void cat_task_sched(void);

void cat_task_sched_unlock_sched(void);
void cat_task_sched_unlock(void);
void cat_task_sched_lock(void);

void cat_task_rdy(cat_task_t *task);
void cat_task_unrdy(cat_task_t *task);

void cat_task_yield(void);

void cat_task_set_delay_ticks(cat_task_t *task, cat_ubase ticks);
void cat_task_set_delay_ms(cat_task_t *task, cat_ubase ticks);
void cat_task_delay_ticks(cat_ubase ticks);
void cat_task_delay_until(cat_ubase tick);
void cat_task_delay_ms(cat_u32 ms);
void cat_task_delay_wakeup(cat_task_t *task);

void cat_task_suspend(cat_task_t *task);
void cat_task_suspend_wakeup(cat_task_t *task);

void cat_task_delete(cat_task_t *task);

cat_err cat_task_change_priority(
    cat_task_t *task,
    cat_u8 new_prio,
    cat_u8 *old_prio
);
cat_err cat_task_change_priority_without_sched(
    cat_task_t *task,
    cat_u8 new_prio,
    cat_u8 *old_prio
);

void cat_task_set_error(cat_task_t *task, cat_err error);
cat_err cat_task_get_error(void);


#endif
