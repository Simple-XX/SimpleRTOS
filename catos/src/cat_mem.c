/**
 * @file cat_mem.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 内存管理
 * @version 0.1
 * @date 2025-03-21
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-8 <td>初始版本
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-21 <td>修正了 pmem 命令的输出对齐格式
 * </table>
 */
#include "catos_config.h"
#include "catos_types.h"

#include "cat_log.h"

#include "cat_assert.h"

#define USE_HEAP_SECTION 0 /* 使能该项以使用空闲堆空间(还没做呢QAQ) */

#define MCB_MAGIC_MASK ((cat_u32)0xffff0000)
#define MCB_MAGIC ((cat_u32)(0xAAAA << 16))

#define MCB_STATUS_MASK ((cat_u32)0x0000ffff)
#define MCB_USED (1 << 0)
#define MCB_LOCKED (1 << 1)

typedef struct _mcb
{
    cat_u32 flags; /**< 内存块标志,包含魔数和状态字 */
    cat_u32 size;  /**< 内存块除去控制块后实际可用大小(字节) */
                   /**< 链表节点 */
    struct _mcb *prev;
    struct _mcb *next;
} _mem_ctl_blk_t; /* 内存控制块结构体 */

static cat_err _cat_mem_init(cat_ubase start_addr, cat_ubase bytes);

_mem_ctl_blk_t mem_head; /* 内存块头部(mem_head->next才是第一个有效内存块) */
static cat_u8 _mem_space[CATOS_MEM_HEAP_SIZE]; /**< 动态内存空间 */

void cat_mem_init(void)
{
    cat_err ret = _cat_mem_init((cat_ubase)_mem_space, CATOS_MEM_HEAP_SIZE);

    if(CAT_EOK != ret)
    {
        CLOG_ERROR("[mem] mem init fail");
    }
    else
    {
        CLOG_INFO("[mem] mem init success");
    }
    
}

/**
 * @brief 初始化动态内存
 *
 * @param  start_addr       动态内存起始地址
 * @param  bytes            总大小
 * @return cat_err
 */
static cat_err _cat_mem_init(cat_ubase start_addr, cat_ubase bytes)
{
    cat_err ret = CAT_ERROR;

    if (CAT_NULL != (void *)start_addr)
    {
        /* 后期考虑是否要先对齐 */
        _mem_ctl_blk_t *mcb = (_mem_ctl_blk_t *)start_addr;
        mcb->flags = MCB_MAGIC;
        mcb->size = bytes - sizeof(_mem_ctl_blk_t);

        /* 将内存块挂到链表上 */
        mem_head.next = mcb;
        mem_head.prev = mcb;

        mcb->next = &mem_head;
        mcb->prev = &mem_head;

        ret = CAT_EOK;
    }

    return ret;
}

/**
 * @brief 分配空间
 *   
 * @param  bytes          申请的空间大小
 * @return cat_err        用户存储分配到的空间的变量地址
 *         如果申请成功则该变量的值会不为空，否则为空
 */
void *cat_malloc(cat_ubase bytes)
{
    void *ret = CAT_NULL;

    _mem_ctl_blk_t *cur_mcb = mem_head.next;
    _mem_ctl_blk_t *new_mcb = CAT_NULL;

    /* 4字节对齐 */
    bytes += 3;
    bytes &= ~0x3;

    while (cur_mcb != &mem_head)
    {
        if (
            ((cur_mcb->flags & MCB_MAGIC_MASK) == MCB_MAGIC) &&
            ((cur_mcb->flags & MCB_USED) == 0) &&
            (cur_mcb->size >= bytes))
        {
            /* 设置分配的空间 */
            ret = (void *)((cat_ubase)cur_mcb + sizeof(_mem_ctl_blk_t));

            /* 更新当前块为正在使用 */
            cur_mcb->flags |= MCB_USED;

            /* 如果剩下的大小还能存下一个控制块+一个4字节数据.就作为一个单独空闲块 */
            if (cur_mcb->size - bytes >= sizeof(_mem_ctl_blk_t) + 4)
            {
                /* 设置剩下空间划分出来的空闲节点 */
                new_mcb = (_mem_ctl_blk_t *)((cat_ubase)cur_mcb + sizeof(_mem_ctl_blk_t) + bytes);
                new_mcb->flags = MCB_MAGIC;
                new_mcb->size = cur_mcb->size - bytes - sizeof(_mem_ctl_blk_t);

                /* 更新当前块的大小 */
                cur_mcb->size = bytes;

                /* 挂进链表 */
                new_mcb->next = cur_mcb->next;
                cur_mcb->next->prev = new_mcb;

                cur_mcb->next = new_mcb;
                new_mcb->prev = cur_mcb;
            }

            break;
        }
        else
        {
            cur_mcb = cur_mcb->next;
        }
    }

    return ret;
}

cat_err cat_free(void *ptr)
{
    if(CAT_NULL == ptr)
    {
        return CAT_ENPTR;
    }

    _mem_ctl_blk_t *mcb = (_mem_ctl_blk_t *)((cat_ubase)ptr - sizeof(_mem_ctl_blk_t));
    _mem_ctl_blk_t *neighbor = CAT_NULL;

    if (
        ((mcb->flags & MCB_MAGIC_MASK) == MCB_MAGIC) &&
        ((mcb->flags & MCB_USED) == 1) /* 不空闲才能释放 */
    )
    {
        /* 清除使用中标志 */
        mcb->flags &= ~MCB_USED;

        neighbor = mcb->next;
        if (
            ((neighbor->flags & MCB_MAGIC_MASK) == MCB_MAGIC) &&
            ((neighbor->flags & MCB_USED) == 0))
        {
            /* 后一块是空闲块 */
            /* 合并 */
            mcb->next = neighbor->next;
            neighbor->next->prev = mcb;

            mcb->size += sizeof(_mem_ctl_blk_t) + neighbor->size;
        }

        neighbor = mcb->prev;
        if (
            ((neighbor->flags & MCB_MAGIC_MASK) == MCB_MAGIC) &&
            ((neighbor->flags & MCB_USED) == 0))
        {
            /* 前一块是空闲块 */
            /* 合并 */
            neighbor->next = mcb->next;
            mcb->next->prev = neighbor;

            neighbor->size += sizeof(_mem_ctl_blk_t) + mcb->size;
        }

        return CAT_EOK;
    }
    else
    {
        /* 说明出错了 */
        CLOG_ERROR("[mem] BLOCK FLUSHED OR NOT IN USE");
    }

    return CAT_ERROR;
}

#if (CATOS_CAT_SHELL_ENABLE == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
#include "port.h"

void cat_mem_print_info(void)
{
    _mem_ctl_blk_t *mcb = mem_head.next;
    int i = 0;

    cat_kprintf("id    addr_range                  size   status------\r\n");
    while (mcb != &mem_head)
    {
        cat_kprintf("blk%4d:[0x%8x, 0x%8x] %5d   %s\r\n",
               i,
               ((cat_ubase)mcb + sizeof(_mem_ctl_blk_t)),
               ((cat_ubase)mcb + sizeof(_mem_ctl_blk_t) + mcb->size),
               mcb->size,
               ((mcb->flags & MCB_USED) == 0) ? "FREE" : "USED");

        i++;
        mcb = mcb->next;
    }
    cat_kprintf("\r\n");
}
void *do_pmem(void *arg)
{
    (void)arg;
	
    cat_mem_print_info();
		
	return CAT_NULL;
}
CAT_DECLARE_CMD(pmem, print mem info, do_pmem);
#endif
