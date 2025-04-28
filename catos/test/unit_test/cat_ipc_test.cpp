
#include "gtest/gtest.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* 桩函数 */
#include "port_stub.h"

#include "catos.h"
// #include "cat_ipc.h"
// #include "cat_task.h"

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#include "stub.h"

#define TASK1_STACK_SIZE    (1024)
#define TASK2_STACK_SIZE    (1024)
cat_task_t task1, task2;
cat_u8 task1_env[TASK1_STACK_SIZE];
cat_u8 task2_env[TASK2_STACK_SIZE];
void task1_entry(void *arg)
{
    (void)arg;
    for(;;){}
}
void task2_entry(void *arg)
{
    (void)arg;
    for(;;){}
}

cat_ipc_t test_ipc;

void test_setup(void);

TEST(cat_ipc_init, init)
{
    test_setup();

    cat_ipc_init(&(test_ipc), CAT_IPC_TYPE_SEM);
    EXPECT_EQ(test_ipc.type, CAT_IPC_TYPE_SEM);

    cat_ipc_recv_wait(&test_ipc, &task1, CAT_IPC_TYPE_SEM, 0);
    EXPECT_EQ(task1.state, (cat_u32)(CAT_IPC_TYPE_SEM<<16));
    EXPECT_EQ(task1.ipc_wait, &test_ipc);
    EXPECT_EQ(task1.error, CAT_EOK);
    EXPECT_EQ(test_ipc.recv_wait_list.head_node.pre_node, &(task1.link_node));
    EXPECT_EQ(cat_ipc_wait_task_count(&test_ipc), (cat_u32)1);

    cat_ipc_recv_wait(&test_ipc, &task2, CAT_IPC_TYPE_SEM, 2*CATOS_SYSTICK_MS);
    EXPECT_EQ(cat_ipc_wait_task_count(&test_ipc), (cat_u32)2);
    EXPECT_EQ(task2.delay, (cat_u32)2);
}



void test_setup(void)
{
    catos_init();

    cat_task_create(
      "task1_task",
      &task1,
      task1_entry,
      CAT_NULL,
      0,
      task1_env,
      TASK1_STACK_SIZE
    );
    cat_task_create(
      "task2_task",
      &task2,
      task2_entry,
      CAT_NULL,
      0,
      task2_env,
      sizeof(task2_env)
    );
}
