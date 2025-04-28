/**
 * @file cat_task.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 任务相关
 * @version 0.1
 * @date 2023-06-30
 *
 * Copyright (c) 2023
 *
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-30 <td>创建
 * <tr><td>v1.1 <td>文佳源 <td>2025-03-05 <td>1.cat_task_delay_wakeup函数中新增delay清零操作;2.cat_task_delay_deal中增加对任务等待ipc的处理
 * <tr><td>v1.2 <td>文佳源 <td>2025-03-08 <td>增加修改任务优先级接口
 * </table>
 */
#include "cat_task.h"

#include "cat_lib.h"
#include "cat_log.h"
#include "cat_intr.h"
#include "cat_assert.h"

#include "port.h"

/* 公有变量 */
cat_list_t cat_task_manage_list; /**< 用于管理的任务链表 */

/* 私有变量 */
static cat_bool cat_task_is_scheduling = CAT_FALSE; /**< 表示已开始调度（正在调度） */

static cat_task_t *cat_task_current; /**< 当前任务指针 */
static cat_bitmap cat_task_prio_bitmap;      /**< 就绪位图 */

static cat_u8 sched_lock_cnt;                                /**< 调度锁 0:未加锁；else：加锁(可多次加锁)*/
static cat_list_t task_rdy_tbl[CATOS_TASK_PRIO_MIN]; /**< 就绪表 */
// static cat_list_t cat_task_delayed_list;             /**< 延时链表 */
static cat_dlist_t delay_list; /* 延时链表 */

/* 内核内部声明 */
void cat_ipc_remove_wait_task(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);
void cat_ipc_wakeup(cat_ipc_t *ipc, cat_task_t *task, void *msg, cat_err error);

static void _default_task_exit(void *arg)
{
    (void)arg;

#if 0
    cat_irq_disable();
    cat_task_t *self = cat_task_self();

    cat_task_delete(self);
    cat_irq_enable();
#else
    CLOG_ERROR("task %s return, which should call cat_task_delete(cat_task_self())", cat_task_get_current()->task_name);
#endif

    /* 正常不会到达这里 */
    CAT_ASSERT(CAT_FALSE);
}

static void cat_task_init(
    const char *task_name,
    cat_task_t *task,
    void (*entry)(void *),
    void *arg,
    cat_u8 prio,
    void *stack_start_addr,
    cat_u32 stack_size,
    cat_u8 sched_strategy)
{
    task->task_name = task_name;
    task->sched_strategy = sched_strategy;

    cat_memset(stack_start_addr, 0xff, stack_size);

    task->sp = (void *)cat_hw_stack_init(
        entry,
        (void *)arg,
        (cat_u8 *)((cat_ubase)stack_start_addr + stack_size - sizeof(cat_ubase)),
        _default_task_exit);

    task->entry = entry;
    task->arg = arg;

    /* 初始化栈 */
    task->stack_start_addr = stack_start_addr;
    task->stack_size = stack_size;

    cat_list_node_init(&(task->ready_node));
    cat_dlist_node_init(&(task->time_node));

    task->state = CATOS_TASK_STATE_RDY;

    task->prio = prio;
    task->slice = CATOS_MAX_SLICE;
    task->suspend_cnt = 0;

    task->sched_times = 0;

    cat_list_node_init(&(task->ipc_wait_node));
    task->ipc_wait = CAT_NULL;
    task->ipc_msg = CAT_NULL;

    task->error = CAT_EOK;

    /* 将任务放进管理链表 */
    cat_list_node_init(&(task->manage_node));
    cat_list_add_last(&cat_task_manage_list, &(task->manage_node));
}

void cat_task_scheduler_init(void)
{
    /* 初始化管理链表 */
    cat_list_init(&cat_task_manage_list);

    /* 初始化全局任务指针 */
    cat_task_current = CAT_NULL;

    /* 调度锁初始为不禁止调度 */
    sched_lock_cnt = 0;

    /* 初始化位图 */
    cat_bitmap_init(&cat_task_prio_bitmap);

    /* 初始化等待链表 */
    // cat_list_init(&cat_task_delayed_list);
    cat_dlist_init(&delay_list);

    /* 初始化就绪表 */
    int i;
    for (i = 0; i < CATOS_TASK_PRIO_MIN; i++)
    {
        cat_list_init(&(task_rdy_tbl[i]));
    }

#if (CATOS_STDIO_ENABLE == 1)
    if (cat_stdio_is_device_is_set())
    {
        CLOG_INFO("[cat_sp_task] static priority scheduler init");
    }
#endif
}

void cat_hw_init_systick(cat_u32 ms);
/* 开始调度 */
void catos_start_schedule(void)
{
    cat_task_t *first_task = CAT_NULL;

    /* 获取最高优先级任务 */
    first_task = cat_task_get_highest_ready();

    /* 因为是第一个任务，不用像调度时判断是否和上一个任务一样，直接赋值给当前任务就行 */
    cat_task_set_current(first_task);

    /* 防止赋值cat_task_is_scheduling后进入systick中断进入调度*/
    _cat_hw_irq_disable();
    /* 表示已经开始调度 */
    cat_task_is_scheduling = CAT_TRUE;

    /* 初始化和开启时钟中断 */
    cat_hw_start_systick();

    cat_task_sched_unlock();

    /* 切换到第一个任务，里面会开中断，pendsv中断优先级比systick高，在执行第一个任务前不会执行到systick中的schedule */
    cat_hw_context_switch_to_first((cat_u32)&(first_task->sp));
}

/**
 * @brief 系统是否已经开始调度
 * 
 * @return cat_bool  CAT_TRUE : 系统已开始调度
 *                   CAT_FALSE: 系统未开始调度
 */
cat_bool catos_is_scheduling(void)
{
    return cat_task_is_scheduling;
}

cat_task_t *cat_task_get_current(void)
{
    return cat_task_current;
}

void cat_task_set_current(cat_task_t *task)
{
    cat_task_current = task;
}

/**
 * @brief 获取本任务(当前任务)指针
 *
 * @return cat_task_t* 本任务指针
 */
cat_task_t *cat_task_self(void)
{
    return cat_task_current;
}

void cat_task_create(
    const char *task_name,
    cat_task_t *task,
    void (*entry)(void *),
    void *arg,
    cat_u8 prio,
    void *stack_start_addr,
    cat_u32 stack_size)
{
    /* 初始化任务结构 */
    cat_task_init(
        task_name,
        task,
        entry,
        arg,
        prio,
        stack_start_addr,
        stack_size,
        SCHED_STRATEGY_STATIC_PRIO);

    /* 放入就绪表 */
    cat_task_rdy(task);
}

/**
 * @brief 获取最高优先级任务
 *
 * @return cat_task_t* CAT_NULL:无就绪任务; !CAT_NULL:就绪任务指针
 */
cat_task_t *cat_task_get_highest_ready(void)
{
    cat_task_t *ret;
    /* 获取最低非零位(有任务就绪的最高优先级) */
    cat_u32 highest_prio = cat_bitmap_get_first_set(&cat_task_prio_bitmap);

    /* 获取链表的第一个节点 */
    cat_node_t *node = cat_list_first(&(task_rdy_tbl[highest_prio]));

    /* 获取任务结构指针 */
    ret = CAT_GET_CONTAINER(node, cat_task_t, ready_node);

    CAT_ASSERT(ret);

    return ret;
}

/**
 * @brief 处理等待队列
 *
 */
void cat_task_delay_deal(void)
{
    /* 处理延时队列 */
    cat_dnode_t *dnode = cat_dlist_first(&delay_list);

    if (CAT_NULL != dnode && dnode->value > 0)
    {
        /* 将任务delay的tick数减1 */
        dnode->value--;
    }

    /* 若剩余等待时间为零则唤醒 */
    while (CAT_NULL != dnode && 0 == dnode->value)
    {
        cat_task_t *task = CAT_GET_CONTAINER(dnode, cat_task_t, time_node);

        if (CAT_NULL != task->ipc_wait)
        {
            /* 如果是因为等待ipc超时,给个超时错误 */
            cat_ipc_remove_wait_task(task->ipc_wait, task, CAT_NULL, CAT_ETIMEOUT);
        }

        CLOG_TRACE("task %s is wakeup delay\r\n", task->task_name);
        cat_task_delay_wakeup(task);

        dnode = cat_dlist_first(&delay_list);
    }

    /* 处理时间片 */
    if (--cat_task_current->slice == 0)
    {
        /* TODO: 这里应该判断大于1? */
        if (cat_list_count(&(task_rdy_tbl[cat_task_current->prio])))
        {
            /* 将当前任务节点从任务链表首位去掉 */
            cat_list_remove_first(&(task_rdy_tbl[cat_task_current->prio]));
            /* 将当前任务节点加到任务链表末尾 */
            cat_list_add_last(&(task_rdy_tbl[cat_task_current->prio]), &(cat_task_current->ready_node));

            /* 重置时间片 */
            cat_task_current->slice = CATOS_MAX_SLICE;
        }
    }
}

/**
 * @brief 进行一次调度
 *
 */
void cat_task_sched(void)
{
    cat_task_t *from_task, *to_task;
    cat_irq_disable();

    /* 如果调度被上锁就直接返回，不调度 */
    if (sched_lock_cnt > 0)
    {
        cat_irq_enable();
        return;
    }

    to_task = cat_task_get_highest_ready();
    if (to_task != cat_task_current)
    {
        from_task = cat_task_current;
        cat_task_current = to_task;

        /* 增加调度次数信息 */
        to_task->sched_times++;
        CLOG_TRACE("%s->%s", from_task->task_name, to_task->task_name);

        /* 切换上下文 */
        cat_hw_context_switch(
            (cat_ubase) & (from_task->sp),
            (cat_ubase) & (to_task->sp)
        );
    }

    cat_irq_enable();
}

/**
 * @brief 允许调度
 *
 */
void cat_task_sched_unlock_sched(void)
{
    cat_irq_disable();
    if (sched_lock_cnt > 0)
    {
        if (--sched_lock_cnt == 0)
        {
            cat_task_sched();
        }
    }

    cat_irq_enable();
}

/**
 * @brief 禁止调度
 *
 */
void cat_task_sched_lock(void)
{
    cat_irq_disable();
    if (sched_lock_cnt < 255)
    {
        sched_lock_cnt++;
    }

    cat_irq_enable();
}

/**
 * @brief 允许调度并且该操作不会包含一次调度
 *
 */
void cat_task_sched_unlock(void)
{
    cat_irq_disable();
    if (sched_lock_cnt > 0)
    {
        --sched_lock_cnt;
    }
    cat_irq_enable();
}

/**
 * @brief 将任务放入就绪表
 *
 * @param task 任务结构体指针
 */
void cat_task_rdy(cat_task_t *task)
{
    CAT_ASSERT(task);
    cat_list_add_last(&(task_rdy_tbl[task->prio]), &(task->ready_node));
    cat_bitmap_set(&cat_task_prio_bitmap, task->prio);
}

/**
 * @brief 将任务从就绪表移除
 *
 * @param task 任务结构体指针
 */
void cat_task_unrdy(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_list_remove_node(&(task->ready_node));

    if (cat_list_count(&(task_rdy_tbl[task->prio])) == 0) /* 如果没有任务才清除就绪位 */
    {
        cat_bitmap_clr(&cat_task_prio_bitmap, task->prio);
    }
}

/**
 * @brief 放弃cpu(没有相同或更高优先级任务处于就绪态时无效)
 *        任务仍然是就绪状态,只是放到同优先级队列末尾
 *
 */
void cat_task_yield(void)
{
    cat_irq_disable();

    /* 查看是否存在该任务同优先级任务 */
    cat_u32 same_prio_task_num = cat_list_count(&(task_rdy_tbl[cat_task_current->prio]));

    if (same_prio_task_num > 1)
    {
        /* 将当前任务节点从任务链表首位去掉 */
        cat_list_remove_first(&(task_rdy_tbl[cat_task_current->prio]));
        /* 将当前任务节点加到任务链表末尾 */
        cat_list_add_last(&(task_rdy_tbl[cat_task_current->prio]), &(cat_task_current->ready_node));
    }

    /* TODO:主动放弃cpu暂时就不重置时间片了,不知道会不会有问题 */

    /* 进行调度必须放在判断后,因为可能没有相同优先级任务,但存在更高优先级任务*/
    cat_task_sched();

    cat_irq_enable();
}

/**
 * @brief 设置任务等待的系统时钟数
 *
 * @param  task             任务指针
 * @param  ticks            要等待的时钟数
 */
void cat_task_set_delay_ticks(cat_task_t *task, cat_ubase ticks)
{
    CAT_ASSERT(task);
    
    if (0 == ticks)
    {
        return;
    }

    cat_irq_disable();

    /* 要等待的tick数 */
    // task->delay = ticks;
    task->time_node.value = ticks;
    /* 将任务从就绪表中取出 */
    cat_task_unrdy(task);
    /* 插入等待链表的末尾 */
    // cat_list_add_last(&cat_task_delayed_list, &(task->ready_node));
    cat_dlist_add(&delay_list, &(task->time_node));
    /* 置位等待状态 */
   
    task->state |= CATOS_TASK_STATE_DELAY;

    cat_irq_enable();

    /* 进行一次调度 */
    cat_task_sched();
}

/**
 * @brief 设置任务等待的毫秒数(注意!当等待的时间小于一个系统时钟时会有阻塞等待)
 *
 * @param  task             任务指针
 * @param  ms               要等待的毫秒数
 */
void cat_task_set_delay_ms(cat_task_t *task, cat_ubase ms)
{
    if (CAT_NULL != task)
    {
        if (ms >= CATOS_SYSTICK_MS)
        {
            /* 要等待的时间大于一个系统时钟的时间 */
            cat_task_set_delay_ticks(task, catos_ms_to_tick(ms));
            ms %= CATOS_SYSTICK_MS;
        }
        cat_delay_us(ms * 1000);
    }
}

/**
 * @brief 当前任务等待
 *
 * @param ticks 需要等待的tick数
 */
void cat_task_delay_ticks(cat_ubase ticks)
{
    cat_task_set_delay_ticks(cat_task_current, ticks);
}

/**
 * @brief 延迟到某个tick
 * 
 * 用法：
 * cat_ubase tick = catos_get_systick();
 * do something
 * tick = tick + catos_ms_to_tick(500);
 * cat_task_delay_until(tick);
 * 用于delay到cur延后500ms的位置
 * 
 * @param[in] tick     目标tick
 */
void cat_task_delay_until(cat_ubase tick)
{
    cat_task_delay_ticks(tick-catos_get_systick());
}

/**
 * @brief 当前任务毫秒级等待
 *
 * @param  ms 要等待的时间(毫秒)
 */
void cat_task_delay_ms(cat_u32 ms)
{
    cat_task_set_delay_ms(cat_task_current, ms);
}

/**
 * @brief 将等待的任务从延时队列取出并挂入就绪表
 *        不进行调度
 *
 * @param task 等待的任务
 */
void cat_task_delay_wakeup(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_irq_disable();
    if(task->state & CATOS_TASK_STATE_DELAY)
    {
        // CLOG_WARNING("task %s is wakeup delay\r\n", task->task_name);
        // while(1);
        /* 从等待链表取出 */
        // cat_list_remove_node(&(task->ready_node));
        cat_dlist_remove(&(task->time_node));
        /* delay值清零 */
        // task->delay = 0;
        task->time_node.value = 0;
        /* 复位等待状态位 */
        task->state &= ~CATOS_TASK_STATE_DELAY;

        /* 将任务就绪 */
        cat_task_rdy(task);
    }
    else
    {
        CLOG_WARNING("task %s is wakeup but not only delayed, state=0x%x\r\n", task->task_name, task->state);
        // while(1);
    }
    cat_irq_enable();
}

/**
 * @brief 挂起任务
 *
 * @param task 任务结构体指针
 */
void cat_task_suspend(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_irq_disable();

    /* 只有不在延时状态时可以被挂起 */
    if (!(task->state & CATOS_TASK_STATE_DELAY))
    {
        if (++task->suspend_cnt <= 1)                /* 增加被阻塞次数 */
        {                                            /* 若当前未被阻塞，则阻塞(是否可用后缀简化？) */
            task->state |= CATOS_TASK_STATE_SUSPEND; /* 置位阻塞状态位 */
            cat_task_unrdy(task);                    /* 从就绪表取出 */

            /* 如果被阻塞的是当前任务，则需要执行下一个任务，即进行一次调度 */
            if (task == cat_task_current)
            {
                CLOG_TRACE("%s suspend self", task->task_name);
                cat_task_sched();
            }
            else
            {
                CLOG_TRACE("%s suspend %s", cat_task_current->task_name, task->task_name);
            }
        }
    }

    cat_irq_enable();
}

/**
 * @brief 唤醒任务(将挂起的任务放入就绪表)
 *        不进行调度
 *
 * @param task 任务结构体指针
 */
void cat_task_suspend_wakeup(cat_task_t *task)
{
    CAT_ASSERT(task);

    cat_irq_disable();

    /* 只有已经被挂起至少一次的任务才能被唤醒 */
    if (task->state & CATOS_TASK_STATE_SUSPEND)
    {
        if (--task->suspend_cnt == 0) /* 减少被阻塞次数 */
        {                             /* 若被阻塞次数为零，则退出阻塞状态 */
            task->state &= ~(CATOS_TASK_STATE_SUSPEND);
            cat_task_rdy(task); /* 挂到就绪表 */
            // cat_task_sched();/* 进行一次调度 */
        }
    }

    cat_irq_enable();
}

/**
 * @brief 删除任务
 *
 * @param  task             任务指针
 */
void cat_task_delete(cat_task_t *task)
{
    CAT_ASSERT(task);
    CAT_ASSERT(0 == (task->state & CATOS_TASK_STATE_DELETED));

    cat_irq_disable();

    if (CATOS_TASK_STATE_RDY == (task->state & CATOS_TASK_STATE_MASK))
    {
        cat_task_unrdy(task);
    }
    else if (0 != (task->state & CATOS_TASK_STATE_DELAY))
    {
        /* 处于delay状态, 从等待队列取出并放入就绪队列 */
        if (CAT_NULL != task->ipc_wait)
        {
            /* 说明有等待的ipc, 将任务从该ipc的等待队列中取下 */
            cat_ipc_wakeup(task->ipc_wait, task, CAT_NULL, CAT_EUNEXIST);
        }
        else
        {
            cat_task_delay_wakeup(task);
        }
        /* 两种情况下的处理都会将任务挂入就绪表, 因此需要取下 */
        cat_task_unrdy(task);
    }
    /** 如果既不处于就绪态也不处于延时态, 说明处于挂起状态, 此
     *  情况下任务的ready_node不处于任何队列中(包括就绪表), 不需要做任何操
     *  作
     */

    task->state = CATOS_TASK_STATE_DELETED;

    cat_task_t *cur_task = cat_task_get_current();
    if (cur_task == task)
    {
        cat_task_sched();
        CLOG_TRACE("%s exit, deleted by self", cur_task->task_name);
    }
    else
    {
        CLOG_TRACE("%s exit, deleted by %s", task->task_name, cur_task->task_name);
    }

    cat_irq_enable();
}

/**
 * @brief 修改任务优先级并根据情况进行一次调度
 *        
 * !会进行一次调度, 如果允许切换走, 那么需要保证调度未上锁
 * 
 * @param  task             任务指针
 * @param  new_prio         新优先级
 * @param[out]  old_prio    旧优先级
 * @return cat_err          CAT_EOK: 成功
 *                          else:    失败
 */
cat_err cat_task_change_priority(
    cat_task_t *task,
    cat_u8 new_prio,
    cat_u8 *old_prio
)
{
    CAT_ASSERT(task);

    cat_irq_disable();

    if(CAT_NULL != old_prio)
    {
        *old_prio = task->prio;
    }

    if ((task->state & CATOS_TASK_STATE_MASK) == CATOS_TASK_STATE_RDY)
    {
        /* 先取下再更改优先级, 因为优先级修改后会挂到新优先级的就绪队列上 */
        cat_task_unrdy(task);
        task->prio = new_prio;
        cat_task_rdy(task);

        /* 如果改的是自己的优先级, 则进行一次调度(可能有比自己优先级更高的任务处于就绪态) */
        if(task == cat_task_self())
        {
            cat_task_sched();
        }
    }
    else
    {
        /* 否则说明任务不在就绪队列中,可以直接修改优先级 */
        task->prio = new_prio;
    }
    cat_irq_enable();

    return CAT_EOK;
}

/**
 * @brief 修改任务优先级(不进行调度）
 * 
 * @param  task             任务指针
 * @param  new_prio         新优先级
 * @param  old_prio[out]    旧优先级
 * @return cat_err          CAT_EOK: 成功
 *                          else:    失败
 */
cat_err cat_task_change_priority_without_sched(
    cat_task_t *task,
    cat_u8 new_prio,
    cat_u8 *old_prio
)
{
    CAT_ASSERT(task);

    cat_irq_disable();

    if(CAT_NULL != old_prio)
    {
        *old_prio = task->prio;
    }

    if ((task->state & CATOS_TASK_STATE_MASK) == CATOS_TASK_STATE_RDY)
    {
        /* 先取下再更改优先级, 因为优先级修改后会挂到新优先级的就绪队列上 */
        cat_task_unrdy(task);
        task->prio = new_prio;
        cat_task_rdy(task);
    }
    else
    {
        /* 否则说明任务不在就绪队列中,可以直接修改优先级 */
        task->prio = new_prio;
    }
    cat_irq_enable();

    return CAT_EOK;
}

/**
 * @brief 设置任务错误代码
 *
 * @param  task             任务指针
 * @param  error            错误代码
 */
void cat_task_set_error(cat_task_t *task, cat_err error)
{
    CAT_ASSERT(task);

    task->error = error;
}

/**
 * @brief 获取本任务错误代码
 *
 * @return cat_err        错误代码
 */
cat_err cat_task_get_error(void)
{
    CAT_ASSERT(cat_task_current);

    return cat_task_current->error;
}


/* 打印任务信息 */

typedef struct
{
    void *sp;              /**< 栈顶(堆栈指针)*/
    const char *task_name;     /**< 任务名称*/
    cat_u8 sched_strategy; /**< 调度策略 */

    // void               *entry;                          /**< 入口函数 */
    // void               *arg;                            /**< 入口函数的参数 */
    void *stack_start_addr; /**< 堆栈起始地址*/
    cat_u32 stack_size;     /**< 堆栈大小*/

    // cat_node_t  ready_node;                      /**< 任务表中的链表节点，也用于delay链表*/
    cat_u32 delay; /**< 延时剩余tick数*/

    cat_u32 state; /**< 当前状态*/

    cat_u8 prio;         /**< priority of task*/
    cat_u32 slice;       /**< 时间片(剩余时间)*/
    cat_u32 suspend_cnt; /**< 被挂起的次数*/

    cat_u32 sched_times; /**< 调度次数*/

    // cat_node_t *manage_node;                    /**< 用于管理的链表节点 */
} cat_task_info_t;

static const cat_u8 strategy_name_map[][8] =
    {
        "s_prio", /* 固定优先级调度 */
};

static const cat_u8 state_name_map[][8] =
    {
        "ready",
        "deleted",
        "delay",
        "suspend",
};

static inline cat_u8 *get_state_name(cat_u8 state)
{
    cat_u8 *ret = CAT_NULL;

    switch (state & CATOS_TASK_STATE_MASK)
    {
    case CATOS_TASK_STATE_RDY:
    {
        ret = (cat_u8 *)state_name_map[0];
        break;
    }
    case CATOS_TASK_STATE_DELETED:
    {
        ret = (cat_u8 *)state_name_map[1];
        break;
    }
    case CATOS_TASK_STATE_DELAY:
    {
        ret = (cat_u8 *)state_name_map[2];
        break;
    }
    case CATOS_TASK_STATE_SUSPEND:
    {
        ret = (cat_u8 *)state_name_map[3];
        break;
    }
    default:
    {
        CLOG_ERROR("[cat_task] invalid state 0x%x!", state);
        break;
    }
    }

    return ret;
}

void *do_ps(void *arg)
{
    (void)arg;

    cat_task_t *task = CAT_NULL;
    cat_task_info_t info = {0};
    cat_node_t *tmp = CAT_NULL;
    cat_ubase *p = CAT_NULL;

    cat_kprintf("-----------------------------------------------------------------------------------------\r\n");
    cat_kprintf("| task_name    | stragegy | prio | state    | stk_sz | stk_top    | stk_use | stk_max |\r\n");
    cat_kprintf("-----------------------------------------------------------------------------------------\r\n");

    CAT_LIST_FOREACH_NO_REMOVE(&cat_task_manage_list, tmp)
    {
        /* 获取任务结构体指针 */
        task = CAT_GET_CONTAINER(tmp, cat_task_t, manage_node);

        /* 在临界区复制需要的值 */
        cat_irq_disable();
        info.task_name = task->task_name;
        info.sched_strategy = task->sched_strategy;
        info.prio = task->prio;
        info.state = task->state;
        info.stack_start_addr = task->stack_start_addr;
        info.stack_size = task->stack_size;
        info.sp = task->sp;
        cat_irq_enable();

        for (p = (cat_ubase *)info.stack_start_addr + sizeof(cat_ubase); (cat_ubase)p <= ((cat_ubase)info.stack_start_addr + info.stack_size - sizeof(cat_ubase)); p++)
        {
            if (0xffffffff != *p)
            {
                break;
            }
        }

        cat_kprintf(
            "| %12s | %8s | %4d | %8s | %6d | %8x | %6d | %7d |\r\n",
            info.task_name,
            strategy_name_map[info.sched_strategy],
            info.prio,
            get_state_name(info.state),
            info.stack_size,
            info.sp,
            (100 - (((cat_ubase)info.sp - (cat_ubase)info.stack_start_addr) * 100 / info.stack_size)),
            (100 - (((cat_ubase)p - (cat_ubase)info.stack_start_addr) * 100 / info.stack_size)));
        // cat_kprintf("------------------------------------------------------------------------------\r\n");
    }

    return CAT_NULL;
}
#if (CATOS_CAT_SHELL_ENABLE == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"
CAT_DECLARE_CMD(ps, print task, do_ps);
#endif