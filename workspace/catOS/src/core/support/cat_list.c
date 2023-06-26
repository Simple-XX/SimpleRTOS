/**
 * @file struct _cat_list_t.c
 * @brief 双向链表
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-17 1.0    amoigus             list函数框架
 */

#include "cat_list.h"


void cat_list_init(struct _cat_list_t *list)
{
    list->FIRST_NODE = &(list->head_node);
    list->LAST_NODE  = &(list->head_node);
    list->node_cnt   = 0;
}

void cat_list_node_init(struct _cat_node_t *node)
{
    node->next_node = node;
    node->pre_node = node;
}

cat_uint32_t cat_list_count(struct _cat_list_t *list)
{
    return list->node_cnt;
}

struct _cat_node_t *cat_list_first(struct _cat_list_t *list)
{
    struct _cat_node_t *node = NULL;
    
    if(list->node_cnt != 0)
    {
        node = list->FIRST_NODE;
    }

    return node;
}

struct _cat_node_t *cat_list_last(struct _cat_list_t *list)
{
    struct _cat_node_t *node = NULL;

    if(list->node_cnt != 0)
    {
        node = list->LAST_NODE;
    }

    return node;
}

struct _cat_node_t *cat_list_pre(struct _cat_list_t *list, struct _cat_node_t *node)
{
    if(node->pre_node == node)
    {
        return NULL;
    }
    else
    {
        return node->pre_node;
    }
}

struct _cat_node_t *cat_list_next(struct _cat_list_t *list, struct _cat_node_t *node)
{
    if(node->next_node == node)
    {
        return NULL;
    }
    else
    {
        return node->next_node;
    }
}

void cat_list_remove_node(struct _cat_list_t *list, struct _cat_node_t *node)
{
    node->pre_node->next_node = node->next_node;
    node->next_node->pre_node = node->pre_node;
    list->node_cnt--;
}

void cat_list_remove_all(struct _cat_list_t *list)
{
    cat_uint32_t count;
    struct _cat_node_t *next_node;

    next_node = list->FIRST_NODE;
    for(count = list->node_cnt; count != 0; count--)
    {
        struct _cat_node_t *current_node = next_node;
        next_node = next_node->next_node;

        current_node->next_node = current_node;
        current_node->pre_node = current_node;
    }

    list->FIRST_NODE = &(list->head_node);
    list->LAST_NODE  = &(list->head_node);
    list->node_cnt = 0;
}

void cat_list_add_first(struct _cat_list_t *list, struct _cat_node_t *node)
{
    node->pre_node = list->FIRST_NODE->pre_node;
    node->next_node = list->FIRST_NODE;

    list->FIRST_NODE->pre_node = node;
    list->FIRST_NODE = node;
    list->node_cnt++;
}

void cat_list_add_last(struct _cat_list_t *list, struct _cat_node_t *node)
{
    node->next_node = &(list->head_node);
    node->pre_node = list->LAST_NODE;

    list->LAST_NODE->next_node = node;
    list->LAST_NODE = node;
    list->node_cnt++;
}

struct _cat_node_t *cat_list_remove_first(struct _cat_list_t *list)
{
    struct _cat_node_t *node = NULL;

    if(list->node_cnt != 0)
    {
        node = list->FIRST_NODE;

        node->next_node->pre_node = &(list->head_node);
        list->FIRST_NODE = node->next_node;
        list->node_cnt--;
    }
    return node;
}

void cat_list_insert_after(
    struct _cat_list_t *list, 
    struct _cat_node_t *node_after, 
    struct _cat_node_t *node_to_insert
)
{
    node_to_insert->pre_node = node_after;
    node_to_insert->next_node = node_after->next_node;

    node_after->next_node->pre_node = node_to_insert;
    node_after->next_node = node_to_insert;

    list->node_cnt++;
}
