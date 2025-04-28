/**
 * @file cat_lib.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 部分基础功能
 * @version 0.1
 * @date 2021-06-09
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2021-06-09 <td>内容
 * </table>
 */

#ifndef CAT_STRCMP_H
#define CAT_STRCMP_H

#include "catos_config.h"
#include "catos_types.h"

/* 位图 START */
/* 位图数据结构 */
typedef struct
{
    cat_u32 bitmap;
} cat_bitmap;

/**
 * @brief 获取位图位数
 */
cat_u32 cat_bitmap_get_wide(void);

/**
 * @brief 初始化位图
 * 
 * @param bitmap 位图指针
 */
void cat_bitmap_init(cat_bitmap *bitmap);

/**
 * @brief 置位
 * 
 * @param bitmap    位图指针
 * @param pos       位置
 */
void cat_bitmap_set(cat_bitmap *bitmap, cat_u32 pos);

/**
 * @brief 复位
 * 
 * @param bitmap    位图指针
 * @param pos       位置
 */
void cat_bitmap_clr(cat_bitmap *bitmap, cat_u32 pos);

/**
 * @brief 获取最低非零位
 * 
 * @param bitmap        位图指针
 * @return cat_u32     最低非零位
 */
cat_u32 cat_bitmap_get_first_set(cat_bitmap *bitmap);
/* 位图 END */
/* 差分链表 START */
#define CAT_DLIST_INVAL 0xffffffff /* 差分链表节点初始值，非法 */
/**
 * @brief 差分链表节点
 */
typedef struct _cat_dnode_t
{
    cat_u32 value;
    struct _cat_dnode_t *prev;
    struct _cat_dnode_t *next;
} cat_dnode_t;

/**
 * @brief 差分链表
 */
typedef struct _cat_dlist_t
{
    cat_dnode_t head;
} cat_dlist_t;

void cat_dlist_init(cat_dlist_t *dlist);
void cat_dlist_node_init(cat_dnode_t *dnode);
void cat_dlist_add(cat_dlist_t *dlist, cat_dnode_t *new);
cat_dnode_t *cat_dlist_first(cat_dlist_t *dlist);
cat_dnode_t *cat_dlist_remove(cat_dnode_t *dnode);
/* 差分链表 END */
/* 链表 START */
/**
 * ptr          成员指针
 * container    成员的拥有者数据结构类型
 * name         成员在其拥有者中的变量名
 */
/*< 获取指针的拥有者的结构体首地址**/
#define CAT_GET_CONTAINER(ptr, container, member) \
    ((container *)((cat_ubase)ptr - (cat_ubase)&((container *)0)->member))

#define FIRST_NODE head_node.next_node  /*< 获取链表第一个有意义的结点**/
#define LAST_NODE  head_node.pre_node   /*< 获取链表最后一个有意义的结点**/

/**
 * @brief 遍历链表
 * 
 * list_ptr     :链表指针
 * iter_ptr     :临时指针，循环之前定义，用于访问遍历的链表节点
 * tmp_ptr      :用于暂存下一个节点指针, 防止因节点删除导致无法继续访问下一个
 * 
 * note:注意这里宏中的 list_ptr 一定要加括号，因为该地址变量可能为
 * 链表变量取地址得到 &list ,若不加括号，则由于取地址运算符 & 的
 * 优先级低于取成员运算符 -> ,会导致如下表达式<exp1>：
 * &list->head_node <exp1>
 * 相当于表达式<exp2>:
 * &(list->head_node) <exp2>
 * 而 list 为变量，使用 -> 运算符会报错
 */
#define CAT_LIST_FOREACH(list_ptr, iter_ptr, tmp_ptr) \
    iter_ptr = (list_ptr)->head_node.tmp_ptr; \
    tmp_ptr = iter_ptr->tmp_ptr; \
    for( \
        iter_ptr = (list_ptr)->head_node.tmp_ptr; \
        iter_ptr != &((list_ptr)->head_node); \
        iter_ptr = tmp_ptr, tmp_ptr = iter_ptr->tmp_ptr \
    )

/**
 * @brief 遍历链表(无删除操作)
 * 
 * list_ptr     :链表指针
 * iter_ptr     :临时指针，循环之前定义，用于访问遍历的链表节点
 */
#define CAT_LIST_FOREACH_NO_REMOVE(list_ptr, iter_ptr) \
    for( \
        iter_ptr = (list_ptr)->FIRST_NODE; \
        iter_ptr != &((list_ptr)->head_node); \
        iter_ptr = iter_ptr->next_node \
    )


/*< 链表结点结构体**/
typedef struct _cat_node_t
{
    struct _cat_node_t *pre_node;
    struct _cat_node_t *next_node;
} cat_node_t;

/*< 链表结构(TODO:去掉node_cnt否则remove_node可能出问题)->20250304已解决**/
typedef struct
{
    cat_node_t head_node; /* head_node只用于连接第一个节点和最后一个节点 */
} cat_list_t;

/**
 * @brief 初始化链表
 * 
 * @param list 要初始化的链表结构指针
 */
void cat_list_init(cat_list_t *list);

/**
 * @brief 初始化结点
 * 
 * @param node 要初始化的结点指针
 */
void cat_list_node_init(cat_node_t *node);

/**
 * @brief 获取链表结点数量
 * 
 * @param list          链表指针
 * @return cat_u32     结点数量
 */
cat_u32 cat_list_count(cat_list_t *list);

/**
 * @brief 检查节点是否在链表上
 * 
 * @param  list             链表指针
 * @param  node             节点指针
 * @return cat_bool       CAT_TRUE：节点在该链表上
 *                          CAT_FALSE:节点不在该链表上
 */
cat_bool cat_list_is_in(cat_list_t *list, cat_node_t *node);

/**
 * @brief 获取链表第一个结点的指针
 * 
 * @param list                  链表指针
 * @return cat_node_t*  第一个结点指针
 */
cat_node_t *cat_list_first(cat_list_t *list);

/**
 * @brief 获取链表最后一个结点的指针
 * 
 * @param list                  链表指针
 * @return cat_node_t*  最后一个结点指针
 */
cat_node_t *cat_list_last(cat_list_t *list);

/**
 * @brief 删除指定结点
 * 
 * @param list  链表指针
 * @param node  要删除的指定结点
 */
void cat_list_remove_node(cat_node_t *node);

/**
 * @brief 移除链表中所有结点
 * 
 * @param list  链表指针
 */
void cat_list_remove_all(cat_list_t *list);

/**
 * @brief 将结点插入链表第一个结点之前
 * 
 * @param list  链表指针
 * @param node  要插入的结点
 */
void cat_list_add_first(cat_list_t *list, cat_node_t *node);

/**
 * @brief 将结点插入链表最后一个结点之后
 * 
 * @param list  链表指针
 * @param node  要删除的结点
 */
void cat_list_add_last(cat_list_t *list, cat_node_t *node);

/**
 * @brief 移除链表第一个结点，并返回其指针
 * 
 * @param list                  链表指针
 * @return cat_node_t*  从链表移除的结点
 */
cat_node_t *cat_list_remove_first(cat_list_t *list);

/**
 * @brief 将结点插入指定结点之后
 * 
 * @param list              链表指针
 * @param node_after        指定结点
 * @param node_to_insert    要插入的结点
 */
void cat_list_insert_after(cat_node_t *node_before, cat_node_t *node_to_insert);
/* 链表 END */
/* 字符串相关 START */
/**
 * @brief 比较字符串是否相同
 * @param str1 
 * @param str2 
 * @return cat_u32 0:equal, -1:not equal, -2:CAT_NULL args
 */
cat_i32 cat_strcmp(const char *str1, const char *str2);

/**
 * @brief 字符串拷贝
 * @param dest     目标buffer
 * @param src      源字符串
 * @param dest_len 目标buffer长度
 * @return cat_i32 0:成功，else:失败
 */
cat_i32 cat_strcpy(char *dest, const char *src, cat_u32 dest_len);

/**
 * @brief 字符串长度
 * 
 * @param  src         源字符串
 * @return cat_u32     长度
 */
cat_u32 cat_strlen(const char *src);

/**
 * @brief 字符串转有符号32
 * @param dest 目标buffer
 * @param src  源字符串
 * @return cat_i32 0:成功，else:失败
 */
cat_i32 cat_atoi(cat_i32 *dest, const char *src);

cat_i32 cat_itoa(char *dst, cat_i32 num);

cat_i32 cat_htoi(cat_u32 *dst, const char *src);
cat_i32 cat_itoh(char *dst, cat_u32 src);

void *cat_memset(void *start_addr, cat_u8 fill_with, cat_u32 size);

void *cat_memcpy(void *dst, const void *src, cat_ubase bytes);
/* 字符串相关 END */
/* 环形缓冲区 START */
typedef struct
{
    cat_u8      *p_buffer;     /* 放置数据的缓冲区地址 */
    cat_u32      ring_mask;    /* 用于循环头尾索引的掩码，用于代替取余操作 */
    cat_u32      tail_index;   /* 尾部索引 */
    cat_u32      head_index;   /* 头部索引 */
} cat_ringbuffer_t;

/**
 * @brief 缓冲区初始化
 *        !缓冲区大小必须为2^N, 且实际可用大小为2^N-1
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 * @param  p_buffer_space   缓冲区内存空间首地址
 * @param  buffer_size      缓冲区大小(字节)
 */
void cat_ringbuffer_init(cat_ringbuffer_t *p_ringbuffer, cat_u8 *p_buffer_space, cat_u32 buffer_size);

/**
 * @brief 清空缓冲区里所有数据
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 */
void cat_ringbuffer_clear(cat_ringbuffer_t *p_ringbuffer);

/**
 * @brief 从缓冲区取元素
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 * @param  data             要存入的数据值
 * @return cat_i32 
 */
cat_i32 cat_ringbuffer_put(cat_ringbuffer_t *p_ringbuffer, cat_u8 data);

/**
 * @brief 
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 * @param  p_data           取出的数据存放的数据地址
 * @return cat_i32      成功失败
 */
cat_i32 cat_ringbuffer_get(cat_ringbuffer_t *p_ringbuffer, cat_u8 *p_data);

/**
 * @brief 
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 * @param  p_data           要存入的数据的起始地址
 * @param  size             要存入的数据的长度
 * @return cat_i32      成功失败
 */
cat_i32 cat_ringbuffer_put_more(cat_ringbuffer_t *p_ringbuffer, const cat_u8 *p_data, cat_u32 size);

/**
 * @brief 
 * 
 * @param  p_ringbuffer     环形缓冲区结构体指针
 * @param  p_data           取出数据要存放的起始地址
 * @param  size             要取出数据的长度
 * @return cat_u32     从环形缓冲区实际取出的数据个数(目前每个数据单位为字节)
 */
cat_u32 cat_ringbuffer_get_more(cat_ringbuffer_t *p_ringbuffer, cat_u8 *p_data, cat_u32 size);

void cat_ringbuffer_print_all(cat_ringbuffer_t *p_ringbuffer);
/* 环形缓冲区 END */
#endif

