/**
 * @file cat_idle.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-29
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-29 1.0    amoigus             内容
 */

#include "cat_idle.h"
#include "cat_task.h"


/* var decl */
struct _cat_task_t *cat_idle_task;                      /**< 空闲任务全局指针 */
static struct _cat_task_t idle_task;                    /**< 空闲任务变量 */

cat_stack_type_t idle_task_env[CATOS_IDLE_STACK_SIZE];  /**< 空闲任务堆栈 */

uint32_t idle_cnt;                                      /**< 空闲任务时钟节拍计数*/
uint32_t idle_max_cnt;                                  /**< 最大节拍输(现在是一秒内的)*/

/* funcs decl */
void cat_idle_entry(void *arg);


void cat_idle_task_create(void)
{
    cat_sp_task_create(
        (const uint8_t *)"idle_task",
        &idle_task,
        cat_idle_entry,
        NULL,
        CATOS_MAX_TASK_PRIO - 1,
        idle_task_env,
        CATOS_IDLE_STACK_SIZE
    );
    cat_idle_task = &idle_task;
}

void cat_idle_entry(void *arg)
{

    for(;;)
    {

    }
}


