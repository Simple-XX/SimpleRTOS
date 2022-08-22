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

static void _default_task_exit(void);

static void _default_task_exit(void)
{
    while(1);
}

void cat_task_init(
    const uint8_t *task_name,
    struct _cat_task_t *task, 
    void (*entry)(void *), 
    void *arg, 
    uint8_t prio, 
    void *stack_start_addr,
    uint32_t stack_size,
    uint32_t sched_strategy
)
{
    task->task_name = (uint8_t *)task_name;
    task->sched_strategy = sched_strategy;

    cat_memset(stack_start_addr, 0, stack_size);

    task->sp = (void *)cat_hw_stack_init(
        (void*)entry,
        (void*)arg,
        (uint8_t *)(stack_start_addr + stack_size - sizeof(uint32_t)),
        (void *)_default_task_exit
    );

    task->entry = (void *)entry;
    task->arg   = arg;

    /* 初始化栈 */
    task->stack_start_addr = stack_start_addr;
    task->stack_size = stack_size;    

#if 0
    sp = stack_start_addr + (stack_size / sizeof(uint32_t));

    //pensv自动保存的部分
    *(--sp) = (uint32_t)(1 << 24);//spsr
    *(--sp) = (uint32_t)entry;//pc
    *(--sp) = (uint32_t)0x14;//lr(r14)
    *(--sp) = (uint32_t)0x12;//r12
    *(--sp) = (uint32_t)0x3;//r3
    *(--sp) = (uint32_t)0x2;//r2
    *(--sp) = (uint32_t)0x1;//r1
    *(--sp) = (uint32_t)arg;//r0

    *(--sp) = (uint32_t)0x11;
    *(--sp) = (uint32_t)0x10;
    *(--sp) = (uint32_t)0x9;
    *(--sp) = (uint32_t)0x8;
    *(--sp) = (uint32_t)0x7;
    *(--sp) = (uint32_t)0x6;
    *(--sp) = (uint32_t)0x5;
    *(--sp) = (uint32_t)0x4;

    task->sp = sp;
#endif

    cat_list_node_init(&(task->link_node));

    task->delay = 0;

    task->state = CATOS_TASK_STATE_RDY;

    task->prio = prio;
    task->slice = CATOS_MAX_SLICE;
    task->suspend_cnt = 0;

    task->sched_times = 0;

}

