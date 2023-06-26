/**
 * @file cat_list.h
 * @brief 循环链表
 * @author mio (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-17 1.0    mio                类型和函数声明
 * 2022-07-12        mio                重新组织并增加了注释
 */

#ifndef LIST_H
#define LIST_H
/*< **/
#include "catos_types.h"
#include "catos_config.h"

#define FIRST_NODE head_node.next_node  /*< 获取链表第一个结点**/
#define LAST_NODE  head_node.pre_node   /*< 获取链表最后一个结点**/

/**
 * ptr          成员指针
 * container    成员的拥有者数据结构类型
 * name         成员在其拥有者中的变量名
 */
/*< 获取指针的拥有者的结构体首地址**/
#define CAT_GET_CONTAINER(ptr, container, member) \
    ((container *)((cat_uint32_t)ptr - (cat_uint32_t)&((container *)0)->member))

/*< 获取链表节点的拥有者的结构体首地址**/
#define CAT_LIST_ENTRY(node, parent, member) \
    CAT_GET_CONTAINER(node, parent, member)

/**
 * @brief 遍历链表
 * 
 * list_ptr     :链表指针
 * tmp_ptr      :临时指针，循环之前定义，用于访问遍历的链表节点
 * 
 * note:注意这里宏中的 list_ptr 一定要加括号，因为该地址变量可能为
 * 链表变量取地址得到 &list ,若不加括号，则由于取地址运算符 & 的
 * 优先级低于取成员运算符 -> ,会导致如下表达式<exp1>：
 * &list->head_node <exp1>
 * 相当于表达式<exp2>:
 * &(list->head_node) <exp2>
 * 而 list 为变量，使用 -> 运算符会报错
 */
#define CAT_LIST_FOREACH(list_ptr, tmp_ptr) \
    cat_node_t *next_node; \
    tmp_ptr = (list_ptr)->head_node.next_node; \
    next_node = tmp_ptr->next_node; \
    for( \
        tmp_ptr = (list_ptr)->head_node.next_node; \
        tmp_ptr != &((list_ptr)->head_node); \
        tmp_ptr = next_node, next_node = tmp_ptr->next_node \
    )


/*< 链表结点结构体**/
struct _cat_node_t
{
    struct _cat_node_t *pre_node;
    struct _cat_node_t *next_node;
};

/*< 链表结构**/
struct _cat_list_t
{
    struct _cat_node_t head_node;
    cat_uint32_t node_cnt;
};


/**
 * @brief 初始化链表
 * 
 * @param list 要初始化的链表结构指针
 */
void                cat_list_init(struct _cat_list_t *list);

/**
 * @brief 初始化结点
 * 
 * @param node 要初始化的结点指针
 */
void                cat_list_node_init(struct _cat_node_t *node);

/**
 * @brief 获取链表结点数量
 * 
 * @param list          链表指针
 * @return cat_uint32_t     结点数量
 */
cat_uint32_t            cat_list_count(struct _cat_list_t *list);

/**
 * @brief 获取链表第一个结点的指针
 * 
 * @param list                  链表指针
 * @return struct _cat_node_t*  第一个结点指针
 */
struct _cat_node_t *cat_list_first(struct _cat_list_t *list);

/**
 * @brief 获取链表最后一个结点的指针
 * 
 * @param list                  链表指针
 * @return struct _cat_node_t*  最后一个结点指针
 */
struct _cat_node_t *cat_list_last(struct _cat_list_t *list);

/**
 * @brief 返回指定结点的上一个结点
 * 
 * @param list                  链表指针
 * @param node                  指定结点
 * @return struct _cat_node_t*  上一个结点的指针
 */
struct _cat_node_t *cat_list_pre(struct _cat_list_t *list, struct _cat_node_t *node);

/**
 * @brief 返回指定结点的下一个结点
 * 
 * @param list                  链表指针
 * @param node                  结点指针
 * @return struct _cat_node_t*  下一个结点的指针
 */
struct _cat_node_t *cat_list_next(struct _cat_list_t *list, struct _cat_node_t *node);

/**
 * @brief 删除指定结点
 * 
 * @param list  链表指针
 * @param node  要删除的指定结点
 */
void                cat_list_remove_node(struct _cat_list_t *list, struct _cat_node_t *node);


/**
 * @brief 移除链表中所有结点
 * 
 * @param list  链表指针
 */
void                cat_list_remove_all(struct _cat_list_t *list);

/**
 * @brief 将结点插入链表第一个结点之前
 * 
 * @param list  链表指针
 * @param node  要插入的结点
 */
void                cat_list_add_first(struct _cat_list_t *list, struct _cat_node_t *node);

/**
 * @brief 将结点插入链表最后一个结点之后
 * 
 * @param list  链表指针
 * @param node  要删除的结点
 */
void                cat_list_add_last(struct _cat_list_t *list, struct _cat_node_t *node);

/**
 * @brief 移除链表第一个结点，并返回其指针
 * 
 * @param list                  链表指针
 * @return struct _cat_node_t*  从链表移除的结点
 */
struct _cat_node_t *cat_list_remove_first(struct _cat_list_t *list);

/**
 * @brief 将结点插入指定结点之后
 * 
 * @param list              链表指针
 * @param node_after        指定结点
 * @param node_to_insert    要插入的结点
 */
void                cat_list_insert_after(struct _cat_list_t *list, struct _cat_node_t *node_after, struct _cat_node_t *node_to_insert);


#endif

