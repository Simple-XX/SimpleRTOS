/**
 * @file basic_ipc_test.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试基础——cat_ipc功能
 * 1. 该测试用例主要测试ipc的基础功能, 建立了三个任务
 * 2. 原理:-
 * 3. 使用方法和现象:任务启动后, 三个任务以 t1->t2->t3->t1的顺序轮流利用ipc方
 * 式同步执行顺序
 * @version 0.1
 * @date 2025-03-03
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-03 <td>内容
 * </table>
 */
#include "catos.h"
#include "../cat_func_test.h"

#define IPC_TEST_TASK_STACK_SIZE    (1024)

#define IPC_TEST_TASK_TIMES    3

typedef enum
{
    IPC_WAIT_TYPE_RECV = 0,
    IPC_WAIT_TYPE_SEND,
} ipc_wait_type_t;

cat_task_t ipc_test_task1;
cat_task_t ipc_test_task2;
cat_task_t ipc_test_task3;

cat_u8 ipc_test_task1_env[IPC_TEST_TASK_STACK_SIZE];
cat_u8 ipc_test_task2_env[IPC_TEST_TASK_STACK_SIZE];
cat_u8 ipc_test_task3_env[IPC_TEST_TASK_STACK_SIZE];

cat_ipc_t test_ipc1;
cat_ipc_t test_ipc2;
cat_ipc_t test_ipc3;

#define TEST

void ipc_t1_entry(void *arg)
{
    (void)arg;

    cat_u32 t1_notified_times = 0;

    cat_task_delay_ms(1000);
    cat_kprintf("[t1]-->notify ipc2\r\n");
    test_export_cat_ipc_wakeup_first(&test_ipc2, IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);

    cat_u8 i;
    for(i = 0; i<IPC_TEST_TASK_TIMES; i++)
    {
        cat_kprintf("[t1]-->ipc1 wait\r\n");

        /* 等待唤醒 */
        test_export_cat_ipc_wait(&test_ipc1, IPC_WAIT_TYPE_RECV, &ipc_test_task1, CAT_IPC_TYPE_SEM, 10000);
        if(cat_task_get_error() == CAT_ETIMEOUT)
        {
            cat_kprintf("[t1][ERROR] ipc1 timeout\r\n");
        }
        else
        {
            cat_kprintf("[t1]<--ipc1 notified (%d)\r\n", t1_notified_times++);
            cat_task_delay_ms(1000);
            cat_kprintf("[t1]-->notify ipc2\r\n");
            test_export_cat_ipc_wakeup_first(&test_ipc2, IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);
        }
    }

    cat_kprintf("ipc t1 end\r\n");
    cat_task_delete(cat_task_self());
}

void ipc_t2_entry(void *arg)
{
    (void)arg;

    cat_u32 t2_notified_times = 0;

    cat_u8 i;
    for(i = 0; i<IPC_TEST_TASK_TIMES; i++)
    {
        cat_kprintf("[t2]-->ipc2 wait\r\n");

        /* 等待唤醒 */
        test_export_cat_ipc_wait(&test_ipc2, IPC_WAIT_TYPE_RECV, &ipc_test_task2, CAT_IPC_TYPE_SEM, 10000);
        if(cat_task_get_error() == CAT_ETIMEOUT)
        {
            cat_kprintf("[t2][ERROR] ipc2 timeout\r\n");
        }
        else
        {
            cat_kprintf("[t2]<--ipc2 notified (%d)\r\n", t2_notified_times++);
            cat_task_delay_ms(1000);
            cat_kprintf("[t2]-->notify ipc3\r\n");
            test_export_cat_ipc_wakeup_first(&test_ipc3, IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);
        }
    }
    
    cat_kprintf("ipc t2 end\r\n");
    cat_task_delete(cat_task_self());
}

void ipc_t3_entry(void *arg)
{
    (void)arg;

    cat_u32 t3_notified_times = 0;

    cat_u8 i;
    for(i = 0; i<IPC_TEST_TASK_TIMES; i++)
    {
        cat_kprintf("[t3]-->ipc3 wait\r\n");

        /* 等待唤醒 */
        test_export_cat_ipc_wait(&test_ipc3, IPC_WAIT_TYPE_RECV, &ipc_test_task3, CAT_IPC_TYPE_SEM, 10000);
        if(cat_task_get_error() == CAT_ETIMEOUT)
        {
            cat_kprintf("[t3][ERROR] ipc3 timeout\r\n");
        }
        else
        {
            cat_kprintf("[t3]<--ipc3 notified (%d)\r\n", t3_notified_times++);
            cat_task_delay_ms(1000);
            cat_kprintf("[t3]-->notify ipc1\r\n");
            test_export_cat_ipc_wakeup_first(&test_ipc1, IPC_WAIT_TYPE_RECV, CAT_NULL, CAT_EOK);
        }
    }
    
    cat_kprintf("ipc t3 end\r\n");
    cat_task_delete(cat_task_self());
}

void ipc_test(void)
{
    CAT_TEST_INFO(ipc_test, test basic ipc);
    test_export_cat_ipc_init(&test_ipc1, CAT_IPC_TYPE_SEM);
    test_export_cat_ipc_init(&test_ipc2, CAT_IPC_TYPE_SEM);
    test_export_cat_ipc_init(&test_ipc3, CAT_IPC_TYPE_SEM);

    cat_task_create(
        "ipc_t1",
        &ipc_test_task1,
        ipc_t1_entry,
        CAT_NULL,
        1,
        ipc_test_task1_env,
        IPC_TEST_TASK_STACK_SIZE
      );

      cat_task_create(
        "ipc_t2",
        &ipc_test_task2,
        ipc_t2_entry,
        CAT_NULL,
        1,
        ipc_test_task2_env,
        IPC_TEST_TASK_STACK_SIZE
      );

      cat_task_create(
        "ipc_t3",
        &ipc_test_task3,
        ipc_t3_entry,
        CAT_NULL,
        1,
        ipc_test_task3_env,
        IPC_TEST_TASK_STACK_SIZE
      );
}

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_IPC_BASIC == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_test_ipc(void *arg)
{
    (void)arg;

    ipc_test();

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_ipc, test ipc, do_test_ipc);
#endif

