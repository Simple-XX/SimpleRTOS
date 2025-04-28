/**
 * @file mem_test.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试动态内存管理功能
 * 1. 该测试用例主要测试内存分配和释放
 * 2. 原理:-
 * 3. 使用方法和现象:系统启动后利用shell命令test_mem进行测
 * 试, 观察分配和释放现象，以及内存碎片的出现
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

#define TEST_TITLE(info) \
    do{ \
        cat_kprintf("========= TEST %s =========\r\n", #info); \
    }while(0)

void test_entry(void *arg)
{
    (void)arg;

    cat_kprintf("test task run!\r\n");
    cat_task_delete(cat_task_self());
}

void test_mem(void)
{
    cat_u8 *mem0, *mem1, *mem2, *mem3, *mem4;

    cat_mem_print_info();

    /* 测试分配 */
    CAT_TEST_INFO(malloc, test malloc);
    cat_kprintf("malloc 512 bytes\r\n");
    mem0 = cat_malloc(512);
    cat_kprintf("malloc 64 bytes\r\n");
    mem1 = cat_malloc(64);
    cat_kprintf("malloc 1024 bytes\r\n");
    mem2 = cat_malloc(1024);
    cat_kprintf("malloc 128 bytes\r\n");
    mem3 = cat_malloc(128);
    cat_mem_print_info();

    /* 展示内存碎片的出现 */
    CAT_TEST_INFO(fragment, show how fra appear);
    cat_kprintf("->free mem1\r\n");
    cat_free(mem1);
    cat_mem_print_info();

    cat_kprintf("now free space should > 304\r\n\r\n");

    /* 尝试分配一个小于当前空闲总容量的内存块 */
    cat_kprintf("try to malloc 256 bytes\r\n");
    mem4 = cat_malloc(256);
    if(CAT_NULL == mem4)
    {
        /* 此时空闲总空间大于304字节, 但由于被分成两部分, 无法被分配, 称为内存碎片 */
        cat_kprintf("fail to malloc mem4:256bytes\r\n\r\n");
    }
    
    /* 测试释放 */
    CAT_TEST_INFO(free, test free);
    cat_mem_print_info();
    cat_kprintf("->free mem2\r\n");
    cat_free(mem2);
    cat_mem_print_info();

    cat_kprintf("<--free all mem\r\n");
    cat_free(mem0);
    cat_free(mem3);
    cat_mem_print_info();

    CAT_TEST_INFO(task stack, test stak);
    cat_task_t *dynamic_task = (cat_task_t *)cat_malloc(sizeof(cat_task_t));
    cat_kprintf("malloc task struct\r\n");
    if(CAT_NULL == dynamic_task)
    {
        cat_kprintf("[ERROR] fail to malloc tcb\r\n");
        return;
    }
    cat_u8 *stack = (cat_u8 *)cat_malloc(sizeof(cat_u8)*1024);
    cat_kprintf("malloc stack space\r\n");
    if(CAT_NULL == dynamic_task)
    if(CAT_NULL == stack)
    {
        cat_kprintf("[ERROR] fail to malloc stack\r\n");
        return;
    }   

    cat_kprintf("create task\r\n");
    cat_task_create
    (
        "dyn_task",
        dynamic_task,
        test_entry,
        CAT_NULL,
        1,
        stack,
        1024
    );

}

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_MEM == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

void *do_test_mem(void *arg)
{
    (void)arg;
	
    test_mem();
		
	return CAT_NULL;
}
CAT_DECLARE_CMD(test_mem, test mem, do_test_mem);
#endif
