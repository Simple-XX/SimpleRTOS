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

#include "cat_log.h"


/* var decl */
cat_task_t *cat_idle_task;                      /**< 空闲任务全局指针 */
static cat_task_t idle_task;                    /**< 空闲任务变量 */

cat_u8 idle_task_env[CATOS_IDLE_STACK_SIZE];  /**< 空闲任务堆栈 */

cat_u32 idle_cnt;                                      /**< 空闲任务时钟节拍计数*/
cat_u32 idle_max_cnt;                                  /**< 最大节拍输(现在是一秒内的)*/

/* funcs decl */
void cat_idle_entry(void *arg);


void cat_idle_task_create(void)
{
    cat_task_create(
        "idle_task",
        &idle_task,
        cat_idle_entry,
        CAT_NULL,
        CATOS_TASK_PRIO_MIN - 1,
        idle_task_env,
        CATOS_IDLE_STACK_SIZE
    );
    cat_idle_task = &idle_task;

#if (CATOS_STDIO_ENABLE == 1)
    if(cat_stdio_is_device_is_set())
    {
        CLOG_INFO("[cat_idle] idle task created");
    }
#endif
}

void cat_idle_entry(void *arg)
{
    (void)arg;
	
    for(;;)
    {

    }
}


