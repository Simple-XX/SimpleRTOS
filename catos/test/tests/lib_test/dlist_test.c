/**
 * @file dlist_test.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试差分链表
 * @version 0.1
 * @date 2025-03-27
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-27 <td>内容
 * </table>
 */
#include "catos.h"
#include "../cat_func_test.h"

#define ITEM_NUM 5

typedef struct _item_t
{
    cat_u32 value;
    cat_dnode_t dn;
} item_t;

cat_dlist_t dlist;
item_t ditems[ITEM_NUM];

cat_u32 value[ITEM_NUM] = {24, 65, 12, 4, 24};

void print_dlist(cat_dlist_t *dl)
{
    cat_dnode_t *p = dl->head.next;

    cat_u32 real_value = 0;

    while(&(dl->head) != p)
    {
        real_value += p->value;
        cat_printf("%d(%d)->", real_value, p->value);
        
        p = p->next;
    }
    cat_printf("END\r\n");
}


void test_dlist_func(void)
{
    cat_kprintf("test init\r\n");
    cat_dlist_init(&dlist);

    cat_u32 i;
    for(i=0; i<ITEM_NUM; i++)
    {
        cat_dlist_node_init(&(ditems[i].dn));
        ditems[i].value = value[i];
        ditems[i].dn.value = value[i];
    }

    CAT_TEST_INFO(cat_dlist_add, test add);
    for(i=0; i<ITEM_NUM; i++)
    {
        cat_printf("add node %d\r\n", ditems[i].dn.value);
        cat_dlist_add(&dlist, &(ditems[i].dn));
        print_dlist(&dlist);
    }

    CAT_TEST_INFO(cat_dlist_pop, test pop);
    cat_dnode_t *tmp = cat_dlist_first(&dlist);
    while(CAT_NULL != tmp)
    {
        cat_dlist_remove(tmp);
        cat_printf("pop %d\r\n", tmp->value);
        print_dlist(&dlist);

        tmp = cat_dlist_first(&dlist);
    }
}

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_LIB_DLIST == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_test_dlist(void *arg)
{
    (void)arg;

    test_dlist_func();

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_dlist, test dlist, do_test_dlist);
#endif