/**
 * @file cat_ipc.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 任务间通信头
 * @version 0.1
 * @date 2025-02-07
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-02-07 <td>内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-05 <td>调整代码实现
 * </table>
 */
#ifndef CAT_IPC_H
#define CAT_IPC_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_lib.h"

#define IPC_TIMEOUT_FOREVER (-1)

/* 0-基础ipc */

/**< ipc变量类型(枚举) */
typedef enum
{
    CAT_IPC_TYPE_UNKOWN = 0,
    CAT_IPC_TYPE_SEM,   /**< 信号量 */
    CAT_IPC_TYPE_MUTEX, /**< 互斥量 */
    CAT_IPC_TYPE_MSGQ,  /**< 消息队列 */
} cat_ipc_type_t;

/**< ipc结构体 */
typedef struct
{
    cat_ipc_type_t      type;            /**< ipc变量类型 */
    cat_list_t          recv_wait_list; /**< 阻塞等待接收该ipc的任务队列 */
    cat_list_t          send_wait_list; /**< 阻塞等待发送ipc的任务队列 */
} cat_ipc_t;

/* 测试用导出声明 */
#define TEST_EXPORT_BASIC_IPC_INTERFACE 1
#if (TEST_EXPORT_BASIC_IPC_INTERFACE == 1)
void        test_export_cat_ipc_init(cat_ipc_t *ipc, cat_ipc_type_t type);
void        test_export_cat_ipc_wait( cat_ipc_t *ipc, cat_u8 wait_type, cat_task_t *task, cat_ipc_type_t type, cat_i32 timeout_ms);
cat_task_t *test_export_cat_ipc_wakeup_first(cat_ipc_t *ipc, cat_u8 wait_type, void *msg, cat_err error);
void        test_export_cat_ipc_wakeup(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);
void        test_export_cat_ipc_remove_wait_task(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);
cat_u32     test_export_cat_ipc_remove_all_wait_task(cat_ipc_t *ipc, cat_u8 wait_type, void *msg, cat_err error);
cat_u32     test_export_cat_ipc_wait_task_count(cat_ipc_t *ipc, cat_u8 wait_type);
#endif

/* 1-信号量 */

/**< 信号量结构体 */
typedef struct
{
    cat_ipc_t ipc; /**< ipc控制块 */
    cat_u32   val; /**< 信号量当前值 */
    cat_u32   max; /**< 信号量最大值 */
} cat_sem_t;

/**< 信号量信息结构体 */
typedef struct
{
    cat_u32 waiting_task_num; /**< 等待该信号量的任务数量 */
    cat_u32 val;              /**< 信号量当前值 */
    cat_u32 max;              /**< 信号量最大值 */
} cat_sem_info_t;

void cat_sem_init(cat_sem_t *sem, cat_u32 val, cat_u32 max);
cat_err cat_sem_get(cat_sem_t *sem, cat_i32 timeout_ms);
cat_err cat_sem_get_unsuspend(cat_sem_t *sem);
void cat_sem_post(cat_sem_t *sem);
cat_err cat_sem_delete(cat_sem_t *sem);

void cat_sem_get_info(cat_sem_t *sem, cat_sem_info_t *info);

/* 2-互斥量 */

/**< 互斥量结构体 */
typedef struct
{
    cat_ipc_t          ipc;           /**< 事件控制块*/
    cat_u32            locked_cnt;    /**< 被锁定次数*/
    cat_task_t        *owner;         /**< 拥有者*/
    cat_u32            original_prio; /**< 拥有者继承优先级之前的原始优先级*/
} cat_mutex_t;

/**< 互斥量信息结构体 */
typedef struct
{
    cat_u32            waiting_task_num;    /**<  */
    cat_u32            owner_prio;
    cat_u32            inherit_prio;
    cat_task_t        *owner;
    cat_u32            locked_cnt;
} cat_mutex_info_t;

void cat_mutex_init(cat_mutex_t *mutex);
cat_err cat_mutex_get(cat_mutex_t *mutex, cat_i32 timeout_ms);
cat_err cat_mutex_get_unsuspend(cat_mutex_t *mutex);
cat_err cat_mutex_post(cat_mutex_t *mutex);
cat_u32 cat_mutex_delete(cat_mutex_t *mutex);

void cat_mutex_get_info(cat_mutex_t *mutex, cat_mutex_info_t *info);

/* 3-消息队列 */

/**
 * 说明: 
 * 1. 消息队列中msg_space中除消息内容外还包含消息控制块cat_mq_msg_t, 用于串起所有消息(相当于一种内存管理)
 */
/**
 * 消息队列结构:
 * oldest<------------------->latest
 * 
 *  mq_tail                  mq_head
 *    |                         |
 *  MSG3 <-next- MSG2 <-next- MSG1
 */
/**< 消息队列结构体 */
typedef struct
{
    cat_ipc_t          ipc;            /**< 事件控制块*/
    
    void              *msg_space;      /**< 消息队列总存储的空间起始地址 */
    cat_u32            msg_space_size; /**< 总存储空间的大小 */
    cat_u32            msg_size;       /**< 单个消息大小 */
    cat_u32            msg_max;        /**< 容纳的消息最大数量 */

    cat_u32            msg_avaliable;  /**< 当前可取出的消息数量(最大为msg_max) */

    void              *mq_head;        /**< 消息队列头部(可读取的第一个) */
    void              *mq_tail;        /**< 消息队列尾部(新加入的消息放该消息后) */
    void              *mq_free;        /**< 下一个空消息块 */

    cat_list_t         send_wait_list; /**< 因消息队列满而等待的发送任务列表 */
} cat_mq_t;

/**< 消息队列的消息结构体 */
typedef struct _cat_mq_msg_t
{
    struct _cat_mq_msg_t *next; /* 下一个消息块(如果在消息队列中为下一个可用消息, 如果在空消息链表(mq_free)中则是下一个空消息块) */
} cat_mq_msg_t;

/**< 互斥量信息结构体 */
typedef struct
{
    cat_u32 msg_size;
    cat_u32 msg_max;
    cat_u32 msg_avaliable;

    cat_u32 send_waiting_num;    /**< 等待该消息队列的任务数量 */
    cat_u32 recv_waiting_num; /**< 因消息队列满而等待的发送任务数量 */
} cat_mq_info_t;

cat_err cat_mq_init(cat_mq_t *mq, void *msg_space, cat_u32 msg_space_size, cat_u32 msg_size);
cat_err cat_mq_recv(cat_mq_t *mq, void *buffer, cat_u32 buffer_size , cat_i32 timeout_ms);
cat_err cat_mq_send(cat_mq_t *mq, void *buffer, cat_u32 buffer_size , cat_i32 timeout_ms);
cat_u32 cat_mq_delete(cat_mq_t *mq);

void cat_mq_get_info(cat_mq_t *mq, cat_mq_info_t *info);

#endif
