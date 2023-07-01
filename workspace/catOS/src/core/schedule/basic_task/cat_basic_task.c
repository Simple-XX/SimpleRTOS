/**
 * @file cat_basic_task.c
 * @author mio (648137125@qq.com)
 * @brief 基础任务定义，仅作为其他任务调度器的基础被使用，用户不使用
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// #include <string.h>
#include "cat_string.h"
#include "cat_basic_task.h"
#include "port.h"

extern struct _cat_list_t cat_task_manage_list; /* <- cat_task.c */

static void _default_task_exit(void);

static void _default_task_exit(void)
{
    while(1);
}

void cat_task_init(
    const cat_uint8_t *task_name,
    struct _cat_task_t *task, 
    void (*entry)(void *), 
    void *arg, 
    cat_uint8_t prio, 
    void *stack_start_addr,
    cat_uint32_t stack_size,
    cat_uint8_t sched_strategy
)
{
    task->task_name = (cat_uint8_t *)task_name;
    task->sched_strategy = sched_strategy;

    cat_memset(stack_start_addr, 0, stack_size);

    task->sp = (void *)cat_hw_stack_init(
        (void*)entry,
        (void*)arg,
        (cat_uint8_t *)((cat_uint32_t)stack_start_addr + stack_size - sizeof(cat_uint32_t)),
        (void *)_default_task_exit
    );

    task->entry = (void *)entry;
    task->arg   = arg;

    /* 初始化栈 */
    task->stack_start_addr = stack_start_addr;
    task->stack_size = stack_size;    

#if 0
    sp = stack_start_addr + (stack_size / sizeof(cat_uint32_t));

    //pensv自动保存的部分
    *(--sp) = (cat_uint32_t)(1 << 24);//spsr
    *(--sp) = (cat_uint32_t)entry;//pc
    *(--sp) = (cat_uint32_t)0x14;//lr(r14)
    *(--sp) = (cat_uint32_t)0x12;//r12
    *(--sp) = (cat_uint32_t)0x3;//r3
    *(--sp) = (cat_uint32_t)0x2;//r2
    *(--sp) = (cat_uint32_t)0x1;//r1
    *(--sp) = (cat_uint32_t)arg;//r0

    *(--sp) = (cat_uint32_t)0x11;
    *(--sp) = (cat_uint32_t)0x10;
    *(--sp) = (cat_uint32_t)0x9;
    *(--sp) = (cat_uint32_t)0x8;
    *(--sp) = (cat_uint32_t)0x7;
    *(--sp) = (cat_uint32_t)0x6;
    *(--sp) = (cat_uint32_t)0x5;
    *(--sp) = (cat_uint32_t)0x4;

    task->sp = sp;
#endif

    cat_list_node_init(&(task->link_node));

    task->delay = 0;

    task->state = CATOS_TASK_STATE_RDY;

    task->prio = prio;
    task->slice = CATOS_MAX_SLICE;
    task->suspend_cnt = 0;

    task->sched_times = 0;

    /* 将任务放进管理链表 */
    cat_list_node_init(&(task->manage_node));
    cat_list_add_last(&cat_task_manage_list, &(task->manage_node));

}

