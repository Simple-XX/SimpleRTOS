/**
 * @file msg_queue_test.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试消息队列功能
 * 1. 本测试用例主要测试消息队列功能, 建立了三个任务
 * 2. 原理:-
 * 3. 使用方法和现象: 
 * @version 0.1
 * @date 2025-03-08
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-08 <td>内容
 * </table>
 */
#include "catos.h"
#include "../cat_func_test.h"

#define MQ_TEST_TASK_STACK_SIZE (1024)
#define MQ_SIZE 1024

#define MQ_TEST_T2_TIMES    3 /* 任务2接收消息次数 */
#define MQ_TEST_T3_TIMES    4 /* 任务3接收消息次数 */


typedef struct _msg_content_t
{
    cat_u32 low_data;
    cat_u32 high_data;
} msg_content_t;

cat_task_t mq_test_task1;
cat_task_t mq_test_task2;
cat_task_t mq_test_task3;

cat_u8 mq_test_task1_env[MQ_TEST_TASK_STACK_SIZE];
cat_u8 mq_test_task2_env[MQ_TEST_TASK_STACK_SIZE];
cat_u8 mq_test_task3_env[MQ_TEST_TASK_STACK_SIZE];

cat_mq_t test_mq;
cat_u8   test_space[MQ_SIZE];

#define TEST

void mq_t1_entry(void *arg)
{
    (void)arg;

    cat_u32 msg_send_cnt = 0;
    msg_content_t content;

    cat_u8 i;
    for (i=0; i<(MQ_TEST_T2_TIMES + MQ_TEST_T3_TIMES); i++)
    {
        cat_task_delay_ms(1000);
        cat_kprintf("[t1]-->msg send(%d)\r\n", msg_send_cnt++);
        
        content.high_data = msg_send_cnt;
        content.low_data  = msg_send_cnt+2;

        cat_mq_send(&test_mq, &content, sizeof(msg_content_t), 10000);
    }

    cat_kprintf("mq t1 end\r\n");
    cat_task_delete(cat_task_self());
}

void mq_t2_entry(void *arg)
{
    (void)arg;

    msg_content_t recv_content;

    cat_u8 i;
    for (i=0; i<MQ_TEST_T2_TIMES; i++)
    {
        cat_kprintf("[t2]-->t2 wait\r\n");

        /* 等待唤醒 */
        cat_mq_recv(&test_mq, &recv_content, sizeof(msg_content_t), 10000);
        if (cat_task_get_error() == CAT_ETIMEOUT)
        {
            cat_kprintf("[t2][ERROR] t2 timeout\r\n");
        }
        else
        {
            cat_kprintf("[t2]<--t2 recv content:low=%d, high=%d\r\n", recv_content.low_data, recv_content.high_data);
        }
    }
    
    cat_kprintf("mq t2 end\r\n");
    cat_task_delete(cat_task_self());
}

void mq_t3_entry(void *arg)
{
    (void)arg;

    msg_content_t recv_content;

    cat_u8 i;
    for (i=0; i<MQ_TEST_T3_TIMES; i++)
    {
        cat_kprintf("[t3]-->t3 wait\r\n");

        /* 等待唤醒 */
        cat_mq_recv(&test_mq, &recv_content, sizeof(msg_content_t), 10000);
        if (cat_task_get_error() == CAT_ETIMEOUT)
        {
            cat_kprintf("[t3][ERROR] t3 timeout\r\n");
        }
        else
        {
            cat_kprintf("[t3]<--t3 recv content:low=%d, high=%d\r\n", recv_content.low_data, recv_content.high_data);
        }
    }
    
    cat_kprintf("mq t3 end\r\n");
    cat_task_delete(cat_task_self());
}

void mq_test(void)
{
    CAT_TEST_INFO(mq_test, test mq);
    cat_err err = cat_mq_init(&test_mq, test_space, MQ_SIZE, sizeof(msg_content_t));
    CAT_ASSERT(CAT_EOK == err);

    cat_task_create(
        "mq_t1",
        &mq_test_task1,
        mq_t1_entry,
        CAT_NULL,
        1,
        mq_test_task1_env,
        MQ_TEST_TASK_STACK_SIZE);

    cat_task_create(
        "mq_t2",
        &mq_test_task2,
        mq_t2_entry,
        CAT_NULL,
        1,
        mq_test_task2_env,
        MQ_TEST_TASK_STACK_SIZE);

    cat_task_create(
        "mq_t3",
        &mq_test_task3,
        mq_t3_entry,
        CAT_NULL,
        1,
        mq_test_task3_env,
        MQ_TEST_TASK_STACK_SIZE);
}

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_IPC_MQ == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_test_mq(void *arg)
{
    (void)arg;

    mq_test();

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_mq, test message_queue, do_test_mq);
#endif
