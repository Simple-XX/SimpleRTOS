/**
 * @file cat_ipc.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 任务间通信
 * @version 0.1
 * @date 2025-02-07
 *
 * Copyright (c) 2025
 *
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-02-07 <td>添加
 * <tr><td>v1.1 <td>文佳源 <td>2025-03-05 <td>1.调整代码实现；2.添加ipc测试接口
 * <tr><td>v1.2 <td>文佳源 <td>2025-03-06 <td>1.添加信号量；2.添加互斥量
 * <tr><td>v1.2 <td>文佳源 <td>2025-03-08 <td>1.添加消息队列;2.增加timeout小于0时的无限等待功能
 * <tr><td>v1.2 <td>文佳源 <td>2025-04-06 <td>1.修复优先级继承时可能会导致上下文切换错误的问题
 * </table>
 */
#include "catos_config.h"
#include "catos_types.h"

#include "cat_ipc.h"

#include "cat_intr.h"
#include "cat_log.h"
#include "cat_task.h"

#include "cat_assert.h"

/* 任务正在等待的ipc操作(接收/发送) */
#define IPC_TASK_WAIT_OFFS    (CATOS_TASK_IPC_OFFS)         /* 任务等待状态在任务状态中的偏移量 */
#define IPC_TASK_WAIT_MASK    (1<<IPC_TASK_WAIT_OFFS)       /* 任务正在等待的ipc操作的掩码 */
#define IPC_TASK_IS_WAIT_RECV (0<<IPC_TASK_WAIT_OFFS)       /* 任务在等待接收 */
#define IPC_TASK_IS_WAIT_SEND (1<<IPC_TASK_WAIT_OFFS)       /* 任务在等待发送 */

/* 任务正在等待的ipc类型（信号量、互斥量、消息队列） */
#define IPC_TYPE_OFFS         (CATOS_TASK_IPC_OFFS+1)       /* ipc类型在任务状态中的偏移量 */
#define IPC_TYPE_MASK         (0xfffe<<CATOS_TASK_IPC_MASK) /* 任务正在等待的ipc类型的掩码 */

typedef enum
{
    IPC_WAIT_TYPE_RECV = 0,
    IPC_WAIT_TYPE_SEND,
} ipc_wait_type_t;

static char *wait_type_str[] = 
{
    "recv",
    "send"
};

/* 基础ipc的非static函数只能用于内核中调用, 调用前在被使用的c文件中补充声明 */
void cat_ipc_remove_wait_task(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);
void cat_ipc_wakeup(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);

static void cat_ipc_init(cat_ipc_t *ipc, cat_ipc_type_t type);
static void cat_ipc_wait(cat_ipc_t *ipc, ipc_wait_type_t wait_type, cat_task_t *task, cat_ipc_type_t type, cat_i32 timeout_ms);
static cat_task_t *cat_ipc_wakeup_first(cat_ipc_t *ipc, ipc_wait_type_t wait_type, void *msg, cat_err error);
static cat_u32 cat_ipc_remove_all_wait_task(cat_ipc_t *ipc, ipc_wait_type_t wait_type, void *msg, cat_err error);
static cat_u32 cat_ipc_wait_task_count(cat_ipc_t *ipc, ipc_wait_type_t wait_type);

/* 基础IPC START */
/**
 * @brief 移除等待该ipc的特定任务(不处理剩下的delay)
 *
 * @param  ipc              ipc指针
 * @param  task             任务指针
 * @param  msg              消息
 * @param  error            为等待的任务给错误代码
 */
void cat_ipc_remove_wait_task(
    cat_ipc_t *ipc,
    cat_task_t *task,
    void *msg,
    cat_err error)
{
    CAT_ASSERT(task->ipc_wait == ipc);

    cat_irq_disable();

    /* 将任务从等待队列中取出（接收等待队列/发送等待队列） */
    cat_list_remove_node(&(task->ipc_wait_node));
    CLOG_TRACE("ipc remove %s from %s", task->task_name, wait_type_str[(task->state & IPC_TASK_WAIT_MASK)>>IPC_TASK_WAIT_OFFS]);
    
    /* 移除该任务等待的ipc */
    task->ipc_wait = CAT_NULL;
    /* 将ipc携带的消息赋值给任务 */
    task->ipc_msg = msg;
    task->error = error;
    /* 清除事件类型数据 */
    task->state &= ~CATOS_TASK_IPC_MASK;

    /** 这里没有对延时进行处理，因为此函数主要是在tick中
     *  并且在其中对延时处理之前被调用，所以延时相关处理在这里不必管
     */

    cat_irq_enable();
}

/**
 * @brief 初始化ipc
 *
 * @param  ipc            My Param doc
 * @param  type             My Param doc
 */
static void cat_ipc_init(cat_ipc_t *ipc, cat_ipc_type_t type)
{
    ipc->type = type;
    cat_list_init(&(ipc->recv_wait_list));
    cat_list_init(&(ipc->send_wait_list));
}

/**
 * @brief 任务等待ipc(不进行调度)
 *
 * @param  ipc              ipc指针
 * @param  wait_type        等待类型
 * @param  task             任务指针
 * @param  type             ipc类型
 * @param  timeout_ms       超时时间(ms)
 */
static void cat_ipc_wait(
    cat_ipc_t *ipc,
    ipc_wait_type_t wait_type,
    cat_task_t *task,
    cat_ipc_type_t type,
    cat_i32 timeout_ms)
{
    CAT_ASSERT(ipc);
    CAT_ASSERT(task);

    /* 能到这里说明对应ipc必定需要阻塞等待 */
    cat_irq_disable();

    /* 设置ipc类型 */
    task->state |= ((type << IPC_TYPE_OFFS) | (wait_type << IPC_TASK_WAIT_OFFS));
    /* 设置等待的ipc结构体 */
    task->ipc_wait = ipc;
    /* 初始化任务错误代码 */
    task->error = CAT_EOK;

    /* 将任务从就绪队列取下 */
    cat_task_unrdy(task);

    /* 将任务挂到ipc的对应等待队列中 */
    switch(wait_type)
    {
        case IPC_WAIT_TYPE_RECV:
        {
            /* 将任务挂到ipc的接收等待队列 */
            cat_list_add_last(&(ipc->recv_wait_list), &(task->ipc_wait_node));
            break;
        }
        case IPC_WAIT_TYPE_SEND:
        {
            /* 将任务挂到ipc的发送等待队列 */
            cat_list_add_last(&(ipc->send_wait_list), &(task->ipc_wait_node));
            break;
        }
        default:
        {
            CLOG_ERROR("ipc wait type unkown");
        }
    }

    if (timeout_ms > 0)
    {
        /* 设置超时时间,等待在时钟中断中处理 !注意这里会使用task->link_node, 所以上面ipc只能用单独的链表节点 */
        cat_task_set_delay_ticks(task, (timeout_ms / CATOS_SYSTICK_MS));
    }
    
    if(timeout_ms < 0)
    {
        /* 无限等待 */
        cat_task_suspend(task);
    }

    cat_irq_enable();
}

/**
 * @brief 唤醒第一个等待该ipc的任务
 *        不进行调度
 *
 * @param  ipc                 ipc指针
 * @param  wait_type           等待类型
 * @param  msg                 ipc携带的消息
 * @param  error               错误代码
 * @return cat_task_t*         被唤醒任务的指针
 *                             CAT_NULL: 无等待任务
 */
static cat_task_t *cat_ipc_wakeup_first(cat_ipc_t *ipc, ipc_wait_type_t wait_type, void *msg, cat_err error)
{
    cat_task_t *task = CAT_NULL;

    cat_irq_disable();

    cat_node_t *node = CAT_NULL;
    if(IPC_WAIT_TYPE_RECV == wait_type)
    {
        /* 将任务从ipc的接收等待队列移除 */
        node = cat_list_remove_first(&(ipc->recv_wait_list));
    }
    else
    {
        /* 将任务从ipc的发送等待队列移除 */
        node = cat_list_remove_first(&(ipc->send_wait_list));
    }

    if (CAT_NULL != node)
    {
        /* 获取任务指针 */
        task = CAT_GET_CONTAINER(node, cat_task_t, ipc_wait_node);

        /* 移除该任务等待的ipc */
        task->ipc_wait = CAT_NULL;
        /* 将ipc携带的消息赋值给任务 */
        task->ipc_msg = msg;
        task->error = error;
        /* 清除事件类型数据 */
        task->state &= ~CATOS_TASK_IPC_MASK;

        if (task->time_node.value > 0)
        {
            /* !若此时该任务delay>0, 只可能是因为等待ipc而导致的等待 */
            cat_task_delay_wakeup(task);
        }
        else
        {
            if(task->state & CATOS_TASK_STATE_SUSPEND)
            {
                /* 这里是因无限等待被挂起 */
                cat_task_suspend_wakeup(task);
            }
            else
            {
                /* 否则直接唤醒 */
                cat_task_rdy(task);
            }
        }
    }

    cat_irq_enable();

    return task;
}

/**
 * @brief 唤醒等待该ipc的特定任务
 *
 * 1.仅当该任务delay大于零时该函数可以唤醒，和cat_task中
 * 的cat_task_delay_wakeup不同，后者无论什么情况都可唤醒;
 * 2.不进行调度
 *
 * @param  ipc              ipc指针
 * @param  task             任务指针
 * @param  msg              ipc携带的消息
 * @param  error            错误代码
 */
void cat_ipc_wakeup(
    cat_ipc_t *ipc,
    cat_task_t *task,
    void *msg,
    cat_err error)
{
    CAT_ASSERT(task->ipc_wait == ipc);

    cat_irq_disable();

    /* 将任务从等待队列中取出（接收等待队列/发送等待队列） */
    cat_list_remove_node(&(task->ipc_wait_node));
    CLOG_TRACE("ipc wakeup %s from %s", task->task_name, wait_type_str[(task->state & IPC_TASK_WAIT_MASK)>>IPC_TASK_WAIT_OFFS]);

    /* 移除该任务等待的ipc */
    task->ipc_wait = CAT_NULL;
    /* 将ipc携带的消息赋值给任务 */
    task->ipc_msg = msg;
    task->error = error;
    /* 清除事件类型数据 */
    task->state &= ~CATOS_TASK_IPC_MASK;

    if (task->time_node.value > 0)
    {
        /* !若此时该任务delay>0, 只可能是因为等待ipc而导致的等待 */
        cat_task_delay_wakeup(task);
    }
    else
    {
        if(task->state & CATOS_TASK_STATE_SUSPEND)
        {
            /* 这里是因无限等待被挂起 */
            cat_task_suspend_wakeup(task);
        }
        else
        {
            /* 否则直接唤醒 */
            cat_task_rdy(task);
        }
    }

    cat_irq_enable();
}

static cat_u32 cat_ipc_remove_all_wait_task(cat_ipc_t *ipc, ipc_wait_type_t wait_type, void *msg, cat_err error)
{
    cat_irq_disable();

    /* 获取对应链表长度 */
    cat_u32 count = cat_ipc_wait_task_count(ipc, wait_type);
    
    cat_u32 i = 0;
    for (i = 0; i < count; i++)
    {
        /* 获取任务指针 */
        cat_node_t *node = CAT_NULL;
        if(IPC_WAIT_TYPE_RECV == wait_type)
        {
            /* 将任务从ipc的接收等待队列移除 */
            node = cat_list_remove_first(&(ipc->recv_wait_list));
        }
        else
        {
            /* 将任务从ipc的发送等待队列移除 */
            node = cat_list_remove_first(&(ipc->send_wait_list));
        }

        cat_task_t *task = CAT_GET_CONTAINER(node, cat_task_t, ipc_wait_node);

        /* 移除该任务等待的ipc */
        task->ipc_wait = CAT_NULL;
        /* 将ipc携带的消息赋值给任务 */
        task->ipc_msg = msg;
        task->error = error;
        /* 清除事件类型数据 */
        task->state &= ~CATOS_TASK_IPC_MASK;

        if (task->time_node.value > 0)
        {
            /* !若此时该任务delay>0, 只可能是因为等待ipc而导致的等待 */
            cat_task_delay_wakeup(task);
        }
        else
        {
            if(task->state & CATOS_TASK_STATE_SUSPEND)
            {
                /* 这里是因无限等待被挂起 */
                cat_task_suspend_wakeup(task);
            }
            else
            {
                /* 否则直接唤醒 */
                cat_task_rdy(task);
            }
        }
    }

    cat_irq_enable();

    return count;
}

static cat_u32 cat_ipc_wait_task_count(cat_ipc_t *ipc, ipc_wait_type_t wait_type)
{
    cat_irq_disable();

    cat_u32 count = 0;
    switch(wait_type)
    {
        case IPC_WAIT_TYPE_RECV:
        {
            count = cat_list_count(&(ipc->recv_wait_list));
            break;
        }
        case IPC_WAIT_TYPE_SEND:
        {
            /* 将任务从ipc的发送等待队列移除 */
            count = cat_list_count(&(ipc->send_wait_list));
            break;
        }
        default:
        {
            CLOG_ERROR("ipc wait type unkown");
        }
    }

    cat_irq_enable();

    return count;
}

/* 测试用导出接口 */
#if (TEST_EXPORT_BASIC_IPC_INTERFACE == 1)
void test_export_cat_ipc_init(cat_ipc_t *ipc, cat_ipc_type_t type)
{
    cat_ipc_init(ipc, type);
}
void test_export_cat_ipc_wait(cat_ipc_t *ipc, cat_u8 wait_type, cat_task_t *task, cat_ipc_type_t type, cat_i32 timeout_ms)
{
    cat_ipc_wait(ipc, (ipc_wait_type_t)wait_type, task, type, timeout_ms);
}
cat_task_t *test_export_cat_ipc_wakeup_first(cat_ipc_t *ipc, cat_u8 wait_type, void *msg, cat_err error)
{
    return cat_ipc_wakeup_first(ipc, (ipc_wait_type_t)wait_type, msg, error);
}
void test_export_cat_ipc_wakeup(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error)
{
    cat_ipc_wakeup(ipc, task, msg, error);
}
void test_export_cat_ipc_remove_wait_task(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error)
{
    cat_ipc_remove_wait_task(ipc, task, msg, error);
}
cat_u32 test_export_cat_ipc_remove_all_wait_task(cat_ipc_t *ipc, cat_u8 wait_type, void *msg, cat_err error)
{
    return cat_ipc_remove_all_wait_task(ipc, (ipc_wait_type_t)wait_type, msg, error);
}
cat_u32 test_export_cat_ipc_wait_task_count(cat_ipc_t *ipc, cat_u8 wait_type)
{
    return cat_ipc_wait_task_count(ipc, (ipc_wait_type_t)wait_type);
}
#endif
/* 基础IPC END */
/* 信号量 START */
void cat_sem_init(cat_sem_t *sem, cat_u32 val, cat_u32 max)
{
    CAT_ASSERT(sem);

    /* 初始化ipc块 */
    cat_ipc_init(&(sem->ipc), CAT_IPC_TYPE_SEM);

    sem->max = max;

    /* 初值不能高于max */
    if (val > max)
    {
        sem->val = max;
    }
    else
    {
        sem->val = val;
    }
}

cat_err cat_sem_get(cat_sem_t *sem, cat_i32 timeout_ms)
{
    CAT_ASSERT(sem);

    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    if (sem->val > 0)
    {
        /* 信号量当前值减一 */
        (sem->val)--;

        cat_irq_enable();
        CLOG_TRACE("%s get sem", cat_task_self()->task_name);

        ret = CAT_EOK;
    }
    else
    {
        cat_irq_enable();
        /* 等待cat_sem_post唤醒 */
        CLOG_TRACE("%s waiting for sem", cat_task_self()->task_name);
        cat_ipc_wait(&(sem->ipc), IPC_WAIT_TYPE_RECV, cat_task_self(), CAT_IPC_TYPE_SEM, timeout_ms);

        /* cat_ipc_wait会挂起任务切换, 开中断后就切走了, 等待唤醒 */

        /* 切换回来后取出结果作为返回值 */
        ret = cat_task_self()->error;
    }

    return ret;
}

cat_err cat_sem_get_unsuspend(cat_sem_t *sem)
{
    CAT_ASSERT(sem);

    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    if (sem->val > 0)
    {
        /* 信号量当前值减一 */
        (sem->val)--;

        cat_irq_enable();
        CLOG_TRACE("%s get sem", cat_task_self()->task_name);

        ret = CAT_EOK;
    }
    else
    {
        /* 非阻塞不等待唤醒 */
        cat_irq_enable();
        CLOG_TRACE("%s get sem fail", cat_task_self()->task_name);

        /* 返回"资源已耗尽"错误代码 */
        ret = CAT_EOUTOF;
    }

    return ret;
}

void cat_sem_post(cat_sem_t *sem)
{
    CAT_ASSERT(sem);

    cat_irq_disable();

    cat_task_t *task = cat_ipc_wakeup_first(&(sem->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);

    if (CAT_NULL != task)
    {
        CLOG_TRACE("%s post sem wakeup %s", cat_task_self()->task_name, task->task_name);
        /* 唤醒的任务优先级更高时进行一次调度 */
        if (task->prio < cat_task_self()->prio)
        {
            cat_task_sched();
        }
    }
    else
    {
        sem->val++;

        CLOG_TRACE("%s post sem", cat_task_self()->task_name);
        if (sem->val > sem->max)
        {
            sem->val = sem->max;
            CLOG_WARNING("%s post sem overflow", cat_task_self()->task_name);
        }
    }

    cat_irq_enable();
}

cat_err cat_sem_delete(cat_sem_t *sem)
{
    CAT_ASSERT(sem);

    cat_irq_disable();

    cat_u32 count = cat_ipc_remove_all_wait_task(&(sem->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EUNEXIST);
    sem->val = 0;

    /* 如果被删除的信号量中有任务，则现在就绪队列中多了些任务，故进行一次调度 */
    if (count > 0)
    {
        cat_task_sched();
    }

    cat_irq_enable();
    CLOG_TRACE("%s deleted sem", cat_task_self()->task_name);

    return count;
}

void cat_sem_get_info(cat_sem_t *sem, cat_sem_info_t *info)
{
    CAT_ASSERT(sem);
    CAT_ASSERT(info);

    cat_irq_disable();

    info->waiting_task_num = cat_ipc_wait_task_count(&(sem->ipc), IPC_WAIT_TYPE_RECV);
    info->val = sem->val;
    info->max = sem->max;

    cat_irq_enable();
}
/* 信号量 END */
/* 互斥量 START */
void cat_mutex_init(cat_mutex_t *mutex)
{
    cat_ipc_init(&(mutex->ipc), CAT_IPC_TYPE_MUTEX);
    mutex->locked_cnt = 0;
    mutex->owner = CAT_NULL;
    mutex->original_prio = CATOS_TASK_PRIO_MIN; /* 比最大优先级还要大的无效值 */
}

/* TODO: 增加timeout_ms给0直接变为非阻塞, 去掉get_unsuspend接口 */
cat_err cat_mutex_get(cat_mutex_t *mutex, cat_i32 timeout_ms)
{
    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    cat_task_t *cur_task = cat_task_self();

    if (0 == mutex->locked_cnt)
    {
        /* 互斥量没被持有 */

        CAT_ASSERT(CAT_NULL == mutex->owner);

        mutex->owner = cur_task;
        mutex->original_prio = cur_task->prio;
        mutex->locked_cnt++;

        cat_irq_enable();
        CLOG_TRACE("%s get mutex", cur_task->task_name);

        ret = CAT_EOK;
    }
    else
    {
        /* 互斥量已被持有 */

        if (mutex->owner == cur_task)
        {
            /* 说明互斥量被同一任务递归申请 */
            mutex->locked_cnt++;

            cat_irq_enable();
            CLOG_TRACE("%s get mutex recursivley", cur_task->task_name);

            ret = CAT_EOK;
        }
        else
        {
            /* 申请获取互斥量的不是已持有互斥量的任务 */

            if (cur_task->prio < mutex->owner->prio)
            {
                /**
                 * 申请互斥量的任务优先级若比持有互斥量的任务更高,
                 * 则让持有互斥量的任务继承优先级, 以避免优先级反转
                 */
                cat_task_t *owner = mutex->owner;

                cat_task_change_priority_without_sched(owner, cur_task->prio, CAT_NULL);
                CLOG_TRACE("%s wait mutex, mutex owner: %s prio:%d->%d", cur_task->task_name, owner->task_name, mutex->original_prio, owner->prio);
#if 0
                if ((owner->state & CATOS_TASK_STATE_MASK) == CATOS_TASK_STATE_RDY)
                {
                    /** 如果持有互斥量的任务状态为就绪态, 则先取下再
                     * 更改优先级, 因为优先级修改后会挂到新优先级就绪
                     * 队列上 */
                    cat_task_unrdy(owner);
                    owner->prio = cur_task->prio;
                    cat_task_rdy(owner);
                }
                else
                {
                    /* 否则说明持有者任务不在就绪队列中,可以直接修改优先级 */
                    owner->prio = cur_task->prio;
                }
#endif
            }

            /* 把申请互斥量的任务挂到互斥量等待队列中 */
            cat_ipc_wait(&(mutex->ipc), IPC_WAIT_TYPE_RECV, cur_task, CAT_IPC_TYPE_MUTEX, timeout_ms);

            /* cat_ipc_wait会挂起任务切换, 开中断后就切走了, 等待唤醒 */
            cat_irq_enable();

            /* 切换回来后取出结果作为返回值 */
            ret = cat_task_self()->error;
        }
    }

    return ret;
}
cat_err cat_mutex_get_unsuspend(cat_mutex_t *mutex)
{
    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    cat_task_t *cur_task = cat_task_self();

    if (0 == mutex->locked_cnt)
    {
        /* 互斥量没被持有 */

        CAT_ASSERT(CAT_NULL == mutex->owner);

        mutex->owner = cur_task;
        mutex->original_prio = cur_task->prio;
        mutex->locked_cnt++;

        cat_irq_enable();
        CLOG_TRACE("%s get mutex", cur_task->task_name);

        ret = CAT_EOK;
    }
    else
    {
        /* 互斥量已被持有 */

        if (mutex->owner == cur_task)
        {
            /* 说明互斥量被同一任务递归申请 */
            mutex->locked_cnt++;

            cat_irq_enable();
            CLOG_ERROR("%s get mutex dup", cur_task->task_name);

            ret = CAT_EOK;
        }
        else
        {
            cat_irq_enable();
            CLOG_TRACE("%s get mutex fail", cur_task->task_name);

            ret = CAT_EOUTOF;
        }
    }

    return ret;
}

cat_err cat_mutex_post(cat_mutex_t *mutex)
{
    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    cat_task_t *cur_task = cat_task_self();

    if (0 == mutex->locked_cnt)
    {
        /* 没有任务持有该互斥量,该函数无实际作用 */
        cat_irq_enable();
        CLOG_WARNING("%s post mutex without effect", cur_task->task_name);

        ret = CAT_EOK;
    }
    else if (mutex->owner != cur_task)
    {
        /* 当释放互斥量的任务不是持有者时返回错误 */
        cat_irq_enable();
        CLOG_ERROR("%s get mutex, but taken by %s", cur_task->task_name, mutex->owner->task_name);

        ret = CAT_EINVAL;
    }
    else
    {
        mutex->locked_cnt--;

        if (mutex->locked_cnt > 0)
        {
            /* 说明持有者释放了一层互斥量, 但未全部释放 */
            cat_irq_enable();
            CLOG_TRACE("%s post mutex 1/%d", cur_task->task_name, mutex->locked_cnt+1);

            ret = CAT_EOK;
        }
        else
        {
            /* 持有者(也是当前任务)已完全释放互斥量, 即 locked_cnt == 0 */

            /* 检查持有者优先级 */
            if (mutex->owner->prio != mutex->original_prio)
            {
                cat_task_t *owner = mutex->owner;

                /* 如果有优先级继承, 则先把优先级置回原值 */
                CLOG_TRACE("low prio task %s post mutex, prio %d->%d", cur_task->task_name, owner->prio, mutex->original_prio);

                /**
                 * 2025-04-06
                 * 这里不应该调度，否则会在关中断的临界区中调用两次 cat_task_sched()，
                 * 导致上下文切换出错
                 * 
                 */
#if 0
                cat_task_change_priority(owner, mutex->original_prio, CAT_NULL);
#else
                cat_task_change_priority_without_sched(owner, mutex->original_prio, CAT_NULL);
#endif
            }

            /* 清空持有者 */
            mutex->owner = CAT_NULL;

            /* 检查是否有任务在等待互斥量 */
            if (cat_ipc_wait_task_count(&(mutex->ipc), IPC_WAIT_TYPE_RECV) > 0)
            {
                /* 若有任务等待则唤醒第一个等待的任务 */
                cat_task_t *waken_task = cat_ipc_wakeup_first(&(mutex->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);
                CLOG_TRACE("%s wakeup %s", cur_task->task_name, waken_task->task_name);

                /* 设置互斥量拥有者 */
                mutex->owner = waken_task;
                mutex->original_prio = waken_task->prio;
                mutex->locked_cnt++;

                if (waken_task->prio < cur_task->prio)
                {
                    cat_task_sched();
                }
            }

            cat_irq_enable();
            ret = CAT_EOK;
        }
    }

    return ret;
}

cat_u32 cat_mutex_delete(cat_mutex_t *mutex)
{
    cat_u32 count = 0;

    cat_irq_disable();

    if (mutex->locked_cnt > 0)
    {
        /* 如果互斥量被持有 */
        cat_task_t *owner = mutex->owner;

        /* 如果继承了优先级, 则变回原优先级 */
        if (mutex->original_prio != owner->prio)
        {
            /* 如果有优先级继承, 则先把优先级置回原值 */
            cat_task_change_priority_without_sched(owner, mutex->original_prio, CAT_NULL);
        }

        /* 移除所有等待该互斥量的任务并放入就绪队列 */
        count = cat_ipc_remove_all_wait_task(&(mutex->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EUNEXIST);

        if (count > 0)
        {
            cat_task_sched();
        }
    }

    cat_irq_enable();

    return count;
}

void cat_mutex_get_info(cat_mutex_t *mutex, cat_mutex_info_t *info)
{
    cat_irq_disable();

    info->waiting_task_num = cat_ipc_wait_task_count(&(mutex->ipc), IPC_WAIT_TYPE_RECV);
    info->owner_prio = mutex->original_prio;

    if (CAT_NULL != mutex->owner)
    {
        /* 有拥有者的情况才获取继承优先级 */
        info->inherit_prio = mutex->owner->prio;
    }
    else
    {
        /* 如果没有持有者则给个非法值 */
        info->inherit_prio = CATOS_TASK_PRIO_MIN;
    }

    info->owner = mutex->owner;
    info->locked_cnt = mutex->locked_cnt;

    cat_irq_enable();
}
/* 互斥量 END */
/* 消息队列 START */
/**
 * 需要关中断保护临界资源: 
 * 1. 消息队列指针
 */

/**
 * @brief
 *
 * @param  mq               消息队列指针
 * @param  msg_space        消息队列空间起始地址
 * @param  msg_space_size   消息队列空间的大小
 * @param  msg_size         每个消息的(最大)长度
 * @return cat_err        CAT_EOK: 成功
 *                          else:    失败
 */
cat_err cat_mq_init(cat_mq_t *mq, void *msg_space, cat_u32 msg_space_size, cat_u32 msg_size)
{
    CAT_ASSERT(mq);
    CAT_ASSERT(msg_space);

    cat_err ret = CAT_ERROR;

    cat_ipc_init(&(mq->ipc), CAT_IPC_TYPE_MSGQ);

    mq->msg_space = msg_space;
    mq->msg_space_size = msg_space_size;
    mq->msg_size = CAT_ALIGN(msg_size, sizeof(cat_ubase));

    /* 计算最大消息数量需要考虑消息控制块的空间 */
    mq->msg_max = msg_space_size / (mq->msg_size + sizeof(cat_mq_msg_t));

    mq->msg_avaliable = 0;

    mq->mq_head = CAT_NULL;
    mq->mq_tail = CAT_NULL;
    mq->mq_free = CAT_NULL;

    /* 初始化消息队列空间, 主要是给每个消息空间分配消息控制块并串到一起 */
    cat_u32 i;
    for (i = 0; i < mq->msg_max; i++)
    {
        /* 按(mq->msg_size + sizeof(cat_mq_msg_t))的大小遍历消息队列空间, 初始化控制块 */
        cat_mq_msg_t *msg = (cat_mq_msg_t *)((cat_u8 *)mq->msg_space + i * (mq->msg_size + sizeof(cat_mq_msg_t)));

        msg->next = (cat_mq_msg_t *)(mq->mq_free);
        mq->mq_free = msg;
    }

    cat_list_init(&(mq->send_wait_list));

    ret = CAT_EOK;

    return ret;
}

/**
 * @brief 取出最早消息块
 * 
 * @param  mq               消息队列指针
 * @return cat_mq_msg_t*    取出的消息块
 */
static inline cat_mq_msg_t *_mq_get_avail_msg_blk(cat_mq_t *mq)
{
    /* 获取最早插入队列的消息 */
    cat_mq_msg_t *msg = (cat_mq_msg_t *)(mq->mq_head);

    /* 移动消息队列头部指针至下一个可用消息 */
    mq->mq_head = msg->next;
    if (mq->mq_tail == msg)
    {
        /* 如果这次取出的消息是最后一个消息, 则取出后队列为空 */
        mq->mq_tail = CAT_NULL;
    }

    /* 减少可用消息数量 */
    mq->msg_avaliable--;

    return msg;
}

cat_err cat_mq_recv(cat_mq_t *mq, void *buffer, cat_u32 buffer_size, cat_i32 timeout_ms)
{
    CAT_ASSERT(mq);
    CAT_ASSERT(buffer);
    CAT_ASSERT(0 != buffer_size);
    CAT_ASSERT(mq->msg_size <= buffer_size);

    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    if(mq->msg_avaliable == 0)
    {
        /* 队列为空 */
        if (timeout_ms != 0)
        {
            /* TODO:要清空task->error? */

            /* 等待接收下一个消息 */
            cat_ipc_wait(&(mq->ipc), IPC_WAIT_TYPE_RECV, cat_task_self(), CAT_IPC_TYPE_MSGQ, timeout_ms);

            cat_irq_enable();

            /* 等待结束后回来取出错误代码结果作为返回值 */
            ret = cat_task_get_error();
        }
        else
        {
            cat_irq_enable();
            ret = CAT_EOUTOF;
        }
    }

    if (
        (mq->msg_avaliable > 0) &&
        (CAT_EOK == ret)            /* 说明任务等待未超时 */
    )
    {
        /* 队列中已有消息 */

        /* 从消息队列头部取下最早插入队列的消息 */
        cat_mq_msg_t *msg = _mq_get_avail_msg_blk(mq);

        cat_irq_enable();

        /* 内存拷贝耗时, 出临界区允许调度 */
        /* 将消息拷贝到给定缓冲区 */
        cat_memcpy(buffer, (void *)((cat_ubase)msg + sizeof(cat_mq_msg_t)), mq->msg_size);

        cat_irq_disable();

        /* 把空消息块放进空消息块队列中 */
        msg->next = (cat_mq_msg_t *)(mq->mq_free);
        mq->mq_free = msg;

        /* 唤醒一个等待着发送消息的任务(如果有) */
        cat_task_t *wait_sender = cat_ipc_wakeup_first(&(mq->ipc), IPC_WAIT_TYPE_SEND, CAT_NULL, CAT_EOK);
        if (CAT_NULL != wait_sender)
        {
            /* 有正在等待发送的任务 */
            if (wait_sender->prio < cat_task_self()->prio)
            {
                cat_task_sched();
            }
        }

        cat_irq_enable();
    }

    return ret;
}

/**
 * @brief 防入消息块
 * 
 * @param  mq               消息队列
 * @param  msg              消息块
 */
static inline void _mq_put_avail_msg_blk(cat_mq_t *mq, cat_mq_msg_t *msg)
{
    if(CAT_NULL != mq->mq_tail)
    {
        /* 如果消息队列中还有消息块, 还需修改原来的尾部块连接当前块 */
        ((cat_mq_msg_t *)(mq->mq_tail))->next = msg;
    }

    /* 移动尾部指针 */
    mq->mq_tail = msg;

    if(CAT_NULL == mq->mq_head)
    {
        /* 如果消息队列中没有消息块, 还需修改头指针 */
        mq->mq_head = msg;
    }

    mq->msg_avaliable++;
}

cat_err cat_mq_send(cat_mq_t *mq, void *buffer, cat_u32 buffer_size, cat_i32 timeout_ms)
{
    CAT_ASSERT(mq);
    CAT_ASSERT(buffer);
    CAT_ASSERT(0 != buffer_size);
    CAT_ASSERT(mq->msg_size <= buffer_size);

    cat_err ret = CAT_ERROR;

    cat_irq_disable();

    /* 获取空闲消息块 */
    cat_mq_msg_t *msg = (cat_mq_msg_t *)(mq->mq_free);
    if(CAT_NULL == msg)
    {
        if(timeout_ms != 0)
        {
            /* TODO:要清空task->error? */

            /* 等待发送消息, 即等待消息队列有空闲块 */
            cat_ipc_wait(&(mq->ipc), IPC_WAIT_TYPE_SEND, cat_task_self(), CAT_IPC_TYPE_MSGQ, timeout_ms);

            cat_irq_enable();

            /* 等待结束后回来取出错误代码结果作为返回值 */
            ret = cat_task_get_error();
        }
        else
        {
            cat_irq_enable();
            ret = CAT_EFULL;
        }
    }
    else
    {
        /* msg不为空, 可以接着下面的发送流程 */
        ret = CAT_EOK;
    }

    msg = (cat_mq_msg_t *)(mq->mq_free);
    if(
        (CAT_NULL != msg) &&
        (CAT_EOK == ret)      /* 说明任务等待未超时 */
    )
    {
        /* 移动空闲消息块指针 */
        mq->mq_free = msg->next;

        /* 此时mq访问结束, 离开中断临界区 */
        cat_irq_enable();

        /**
         * mio's Tips:此时msg->next指向下一个空闲消息块, 如果这里不将新消息块的
         * next置空, 在接收消息取出消息块(_mq_get_avail_msg_blk())时, 代码:
         * * 移动消息队列头部指针至下一个可用消息 *
         *    mq->mq_head = msg->next;
         * 会让消息队列头部指向空闲块链表, 导致之后接收消息全是从空闲块中取出
         */
        /* 新消息块会插入到队列尾部, 因此成为新的尾部, next要置0 */
        msg->next = CAT_NULL;

        /* 将数据从缓冲区拷贝到消息块 */
        cat_memcpy((void *)((cat_ubase)msg + sizeof(cat_mq_msg_t)), buffer, mq->msg_size);

        cat_irq_disable();

        /* 将消息挂到消息队列尾部 */
        _mq_put_avail_msg_blk(mq, msg);

        /* 最大消息数是由空间计算出, 而这里每当挂一个消息块才会增加msg_avaliable, 因此理论上不可能出现可用消息多于最大 */
        CAT_ASSERT(mq->msg_avaliable < mq->msg_max);

        /* 唤醒一个等待着接收消息的任务(如果有) */
        cat_task_t *wait_recver = cat_ipc_wakeup_first(&(mq->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);
        if (CAT_NULL != wait_recver)
        {
            /* 有正在等待发送的任务 */
            if (wait_recver->prio < cat_task_self()->prio)
            {
                cat_task_sched();
            }
        }

        cat_irq_enable();
    }

    return ret;
}
cat_u32 cat_mq_delete(cat_mq_t *mq)
{
    CAT_ASSERT(mq);

    cat_irq_disable();

    cat_u32 count = 0;

    /* 唤醒所有等待接收的任务 */
    count += cat_ipc_remove_all_wait_task(&(mq->ipc), IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EUNEXIST);

    /* 唤醒所有等待发送的任务 */
    count += cat_ipc_remove_all_wait_task(&(mq->ipc), IPC_WAIT_TYPE_SEND, CAT_NULL, CAT_EUNEXIST);

    /* 如果被删除的信号量中有任务，则现在就绪队列中多了些任务，故进行一次调度 */
    if (count > 0)
    {
        cat_task_sched();
    }

    cat_irq_enable();

    return count;
}

void cat_mq_get_info(cat_mq_t *mq, cat_mq_info_t *info)
{
    CAT_ASSERT(mq);
    CAT_ASSERT(info);

    cat_irq_disable();

    info->msg_size = mq->msg_size;
    info->msg_max = mq->msg_max;
    info->msg_avaliable = mq->msg_avaliable;
    
    info->recv_waiting_num = cat_ipc_wait_task_count(&(mq->ipc), IPC_WAIT_TYPE_RECV);
    info->send_waiting_num = cat_ipc_wait_task_count(&(mq->ipc), IPC_WAIT_TYPE_SEND);

    cat_irq_enable();
}
/* 消息队列 END */
/* 邮箱 START */

/* 邮箱 END */
/* 事件标志 START */

/* 事件标志 END */