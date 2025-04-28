/**
 * @file mutex_test.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试互斥量功能
 * 1. 该测试验证使用和不使用互斥量对同一临界资源(test_critical_resource)
 * 进行访问, 不使用互斥量时会出现并发导致的数据不一致:
 * 2. 原理:-
 * 3. 使用方法和现象:待任务启动运行结束出现
 * [12035] [t2] done
 * [12036] [t1] done
 * [12037] [t3] done
 * 后使用scr命令查看 test_critical_resource 的值
 *  3.1->若 USE_MUTEX == 0 则未使用互斥量, 结果为 1479522
 *  3.2->若 USE_MUTEX == 1 则使用了互斥量, 结果为 3000000
 *
 * !注意经测试INC_TIMES=10^5时不使用互斥量也不会出现数据不一致问题, 注意修改
 * 测试数据时考虑此因素
 * @version 0.1
 * @date 2025-03-06
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-06 <td>内容
 * </table>
 */
#include "catos.h"
#include "../cat_func_test.h"

#define INC_TIMES 1000000
#define MUTEX_TEST_TASK_STACK_SIZE (1024)

cat_task_t mutex_test_task1;
cat_task_t mutex_test_task2;
cat_task_t mutex_test_task3;

cat_u8 mutex_test_task1_env[MUTEX_TEST_TASK_STACK_SIZE];
cat_u8 mutex_test_task2_env[MUTEX_TEST_TASK_STACK_SIZE];
cat_u8 mutex_test_task3_env[MUTEX_TEST_TASK_STACK_SIZE];

cat_mutex_t test_mutex;

cat_u32 test_critical_resource = 0;

#define USE_MUTEX 1

void mutex_t1_entry(void *arg)
{
    (void)arg;

#if (USE_MUTEX == 0)
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        test_critical_resource++;
    }
    cat_kprintf("[t1] done\r\n");
#else
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        cat_mutex_get(&test_mutex, 100000);
        test_critical_resource++;
        cat_mutex_post(&test_mutex);
    }
    cat_kprintf("[t1] done\r\n");
#endif

    cat_task_delete(cat_task_self());
}

void mutex_t2_entry(void *arg)
{
    (void)arg;

#if (USE_MUTEX == 0)
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        test_critical_resource++;
    }
    cat_kprintf("[t2] done\r\n");
#else
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        cat_mutex_get(&test_mutex, 100000);
        test_critical_resource++;
        cat_mutex_post(&test_mutex);
    }
    cat_kprintf("[t2] done\r\n");
#endif

    cat_task_delete(cat_task_self());
}

void mutex_t3_entry(void *arg)
{
    (void)arg;

#if (USE_MUTEX == 0)
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        test_critical_resource++;
    }
    cat_kprintf("[t3] done\r\n");
#else
    cat_u32 i, j = INC_TIMES;

    for(i=0; i<j; i++)
    {
        cat_mutex_get(&test_mutex, 100000);
        test_critical_resource++;
        cat_mutex_post(&test_mutex);
    }
    cat_kprintf("[t3] done\r\n");
#endif

    cat_task_delete(cat_task_self());
}

void mutex_test(void)
{
    CAT_TEST_INFO(mutex_test, test mutex);
    cat_mutex_init(&test_mutex);

    cat_task_create(
        "mutex_t1",
        &mutex_test_task1,
        mutex_t1_entry,
        CAT_NULL,
        1,
        mutex_test_task1_env,
        MUTEX_TEST_TASK_STACK_SIZE);

    cat_task_create(
        "mutex_t2",
        &mutex_test_task2,
        mutex_t2_entry,
        CAT_NULL,
        1,
        mutex_test_task2_env,
        MUTEX_TEST_TASK_STACK_SIZE);

    cat_task_create(
        "mutex_t3",
        &mutex_test_task3,
        mutex_t3_entry,
        CAT_NULL,
        1,
        mutex_test_task3_env,
        MUTEX_TEST_TASK_STACK_SIZE);
}

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_IPC_MUT == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

void *do_test_mut(void *arg)
{
    (void)arg;

    mutex_test();

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_mut, test mutex, do_test_mut);

void *do_show_critical_res(void *arg)
{
    (void)arg;

    cat_kprintf("%d\r\n", test_critical_resource);

    return CAT_NULL;
}
CAT_DECLARE_CMD(scr, print critical resource, do_show_critical_res);
#endif
