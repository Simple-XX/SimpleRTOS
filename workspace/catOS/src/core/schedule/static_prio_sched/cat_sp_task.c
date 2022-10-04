/**
 * @file cat_sp_task.c
 * @author mio (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "../../../../bsp/interface/port.h"
#include "cat_list.h"
#include "cat_bitmap.h"

#include "cat_sp_task.h"

void assert_task(struct _cat_task_t *task)
{
    if(task->stack_size != 4096)
    { 
        while(1); 
    } 
}

#if 0
    #define TMP_ASSERT_TASK(task) \
    do{ \
        assert_task(task); \
    }while(0)
#else
    #define TMP_ASSERT_TASK(task)
#endif

/** 变量声明 */
/* PUBLIC */
struct _cat_task_t *cat_sp_cur_task;                /**< 当前任务的指针 */
struct _cat_task_t *cat_sp_next_task;                  /**< 要切换到的任务的指针 */

cat_bitmap cat_task_prio_bitmap;                    /**< 就绪位图 */

/* PRIVATE */
static uint8_t sched_lock_cnt;                                  /**< 调度锁 0:未加锁；else：加锁(可多次加锁)*/
static struct _cat_list_t task_rdy_tbl[CATOS_MAX_TASK_PRIO];    /**< 就绪表 */
static struct _cat_list_t cat_task_delayed_list;                /**< 延时链表 */

/**
 * @brief 初始化固定优先级任务调度
 * 
 */
void cat_sp_task_scheduler_init(void)
{
    /* 调度锁初始为不禁止调度 */
	sched_lock_cnt = 0;

    /* 初始化位图 */
    cat_bitmap_init(&cat_task_prio_bitmap);

    /* 初始化等待链表 */
    cat_list_init(&cat_task_delayed_list);

    /* 初始化就绪表 */
    int i;
    for(i=0; i < CATOS_MAX_TASK_PRIO; i++)
    {
        cat_list_init(&(task_rdy_tbl[i]));
    }
}

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
)
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
        SCHED_STRATEGY_STATIC_PRIO
    );

    /* 放入就绪表 */
    cat_sp_task_rdy(task);
}

/**
 * @brief 在开始第一个任务之前要调用该函数处理
 * 
 */
void cat_sp_task_before_start_first(void)
{
    /* 获取最高优先级任务 */
    cat_sp_next_task = cat_sp_task_highest_ready();

    /* 允许调度(打开调度锁，并且不在该处进行调度) */
    cat_sp_task_sched_enable_without_sched();
}

/**
 * @brief 获取最高优先级任务
 * 
 * @return struct _cat_task_t* NULL:无就绪任务; !NULL:就绪任务指针
 */
struct _cat_task_t *cat_sp_task_highest_ready(void)
{
    struct _cat_task_t *ret;
    /* 获取最低非零位(有任务就绪的最高优先级) */
    uint32_t highest_prio = cat_bitmap_get_first_set(&cat_task_prio_bitmap);

    /* 获取链表的第一个节点 */
    struct _cat_node_t *node = cat_list_first(&(task_rdy_tbl[highest_prio]));

    /* 获取任务结构指针 */
    ret = CAT_GET_CONTAINER(node, struct _cat_task_t, link_node);

    TMP_ASSERT_TASK(ret);

    return ret;
}

/**
 * @brief 处理等待
 * 
 */
void cat_sp_task_delay_deal(void)
{
    struct _cat_node_t *node, *next_node;

    /* 处理延时队列 */
    /* 取得等待链表第一个节点 */
    node = cat_task_delayed_list.head_node.next_node;
    /* 遍历等待链表 */
    while(node != &(cat_task_delayed_list.head_node))
    {
        /* 保存下一个节点，否则在任务控制块中只有一个链表节点的情况下进行wakeup操作后
         * node的下一个节点会被改变为就绪链表的下一个节点 */
        next_node = node->next_node;
        
        /* 取得任务控制块指针 */
        struct _cat_task_t *task = CAT_GET_CONTAINER(node, struct _cat_task_t, link_node);
        
        TMP_ASSERT_TASK(task);

        if(--task->delay == 0)
        {
            cat_sp_task_delay_wakeup(task);/* 从延时表取出并挂到就绪表中 */
        }

        /* 在下一次循环前挪动当前指针到先前保存的next_node位置 */
        node = next_node;
    }

    /* 处理时间片 */
    if(--cat_sp_cur_task->slice == 0)
    {
        if(cat_list_count(&(task_rdy_tbl[cat_sp_cur_task->prio])))
        {
            /* 将当前任务节点从任务链表首位去掉 */
            cat_list_remove_first(&(task_rdy_tbl[cat_sp_cur_task->prio]));
            /* 将当前任务节点加到任务链表末尾 */
            cat_list_add_last(&(task_rdy_tbl[cat_sp_cur_task->prio]), &(cat_sp_cur_task->link_node));

            /* 重置时间片 */
            cat_sp_cur_task->slice = CATOS_MAX_SLICE;
        }
    }
}

/**
 * @brief 进行一次调度
 * 
 */
void cat_sp_task_sched(void)
{
    struct _cat_task_t *temp_task;
    uint32_t status = cat_hw_irq_disable();

    /* 如果调度被上锁就直接返回，不调度 */
    if(sched_lock_cnt > 0)
    {
        cat_hw_irq_enable(status);
        return;
    }

    temp_task = cat_sp_task_highest_ready();
    if(temp_task != cat_sp_cur_task)
    {
        cat_sp_next_task = temp_task;

        /* 增加调度次数信息 */
        cat_sp_next_task->sched_times++;

        cat_hw_context_switch();
    }

    cat_hw_irq_enable(status);
}

/**
 * @brief 允许调度并且该操作包含一次调度
 * 
 */
void cat_sp_task_sched_enable(void)
{
    uint32_t status = cat_hw_irq_disable();
    if(sched_lock_cnt > 0)
    {
        if(--sched_lock_cnt == 0)
        {
            cat_sp_task_sched();
        }
    }

    cat_hw_irq_enable(status);
}

/**
 * @brief 允许调度并且该操作不会包含一次调度
 * 
 */
void cat_sp_task_sched_enable_without_sched(void)
{
    uint32_t status = cat_hw_irq_disable();
    if(sched_lock_cnt > 0)
    {
        --sched_lock_cnt;
    }
    cat_hw_irq_enable(status);
}

/**
 * @brief 禁止调度
 * 
 */
void cat_sp_task_sched_disable(void)
{
    uint32_t status = cat_hw_irq_disable();
    if(sched_lock_cnt < 255)
    {
        sched_lock_cnt++;
    }

    cat_hw_irq_enable(status);
}

/**
 * @brief 将任务放入就绪表
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_rdy(struct _cat_task_t *task)
{
    TMP_ASSERT_TASK(task);
    cat_list_add_last(&(task_rdy_tbl[task->prio]), &(task->link_node));
    cat_bitmap_set(&cat_task_prio_bitmap, task->prio);
}

/**
 * @brief 将任务从就绪表移除
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_unrdy(struct _cat_task_t *task)
{
    TMP_ASSERT_TASK(task);
    cat_list_remove_node(&(task_rdy_tbl[task->prio]), &(task->link_node));
    if(cat_list_count(&(task_rdy_tbl[task->prio])) == 0)/* 如果没有任务才清除就绪位 */
    {
        cat_bitmap_clr(&cat_task_prio_bitmap, task->prio);
    }
}

/**
 * @brief 当前任务等待
 * 
 * @param ticks 需要等待的tick数
 */
void cat_sp_task_delay(uint32_t ticks)
{
    uint32_t status = cat_hw_irq_disable(); 

    TMP_ASSERT_TASK(cat_sp_cur_task);

    /* 要等待的tick数 */
    cat_sp_cur_task->delay = ticks;
    /* 将任务从就虚表中取出 */
    cat_sp_task_unrdy(cat_sp_cur_task);
    /* 插入等待链表的末尾 */
    cat_list_add_last(&cat_task_delayed_list, &(cat_sp_cur_task->link_node));
    /* 置位等待状态 */
    cat_sp_cur_task->state |= CATOS_TASK_STATE_DELAYED;

    cat_hw_irq_enable(status);

    /* 进行一次调度 */
    cat_sp_task_sched();
}

/**
 * @brief 将等待的任务从延时队列取出并挂入就绪表
 * 
 * @param task 等待的任务
 */
void cat_sp_task_delay_wakeup(struct _cat_task_t *task)
{
    TMP_ASSERT_TASK(task);

    /* 从等待链表取出 */
    cat_list_remove_node(&cat_task_delayed_list, &(task->link_node));
    /* 复位等待状态位 */
    task->state &= ~CATOS_TASK_STATE_DELAYED;

    /* 将任务就绪 */
    cat_sp_task_rdy(task);
}

/**
 * @brief 挂起任务
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_suspend(struct _cat_task_t *task)
{
    TMP_ASSERT_TASK(task);

    uint32_t status = cat_hw_irq_disable();

    /* 只有不在延时状态时可以被挂起 */
    if(!(task->state & CATOS_TASK_STATE_DELAYED))
    {
        if(++task->suspend_cnt <= 1)/* 增加被阻塞次数 */
        {                           /* 若当前未被阻塞，则阻塞(是否可用后缀简化？) */
            task->state |= CATOS_TASK_STATE_SUSPEND;/* 置位阻塞状态位 */
            cat_sp_task_unrdy(task);/* 从就绪表取出 */
            
            /* 如果被阻塞的是当前任务，则需要执行下一个任务，即进行一次调度 */
            if(task == cat_sp_cur_task)
            {
                cat_sp_task_sched();
            }
        }
    }

    cat_hw_irq_enable(status);
}

/**
 * @brief 从挂起状态唤醒任务
 * 
 * @param task 任务结构体指针
 */
void cat_sp_task_suspend_wakeup(struct _cat_task_t *task)
{
    TMP_ASSERT_TASK(task);

    uint32_t status = cat_hw_irq_disable();

    /* 只有已经被挂起至少一次的任务才能被唤醒 */
    if(task->state & CATOS_TASK_STATE_SUSPEND)
    {
        if(--task->suspend_cnt == 0)/* 减少被阻塞次数 */
        {                           /* 若被阻塞次数为零，则退出阻塞状态 */
            task->state &= ~(CATOS_TASK_STATE_SUSPEND);
            cat_sp_task_rdy(task);/* 挂到就绪表 */
            cat_sp_task_sched();/* 进行一次调度 */
        }
    }

    cat_hw_irq_enable(status);
}
