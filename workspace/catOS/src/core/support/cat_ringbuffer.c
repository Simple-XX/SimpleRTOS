/**
 * @file cat_ringbuffer.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 环形缓冲区源文件
 * @version 0.1
 * @date 2023-06-17
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-17 <td>新建文件
 * </table>
 */
#include <stdio.h>
#include "cat_error.h"

#include "cat_ringbuffer.h"

#define IS_POWER_OF_TWO(_num) \
    ((_num & (_num - 1)) == 0)

#define RINGBUFFER_INDEX_MOVE_FOWARD(_ringbuffer, _index_name) \
    do{ \
        (_ringbuffer)->_index_name = (((_ringbuffer)->_index_name + 1) & ((_ringbuffer)->ring_mask)); \
    }while(0)

void cat_ringbuffer_init(cat_ringbuffer_t *p_ringbuffer, cat_uint8_t *p_buffer_space, cat_uint32_t buffer_size)
{
    CAT_ASSERT(NULL != p_ringbuffer);
    CAT_ASSERT(NULL != p_buffer_space);
    //CAT_ASSERT(buffer_size >= MIN_RINGBUFFER_SIZE);
    /* 如果要使用ring_mask来代替取余操作实现循环，buffer大小必须为2的n次方 */
    CAT_ASSERT(1 == (IS_POWER_OF_TWO(buffer_size)));

    p_ringbuffer->p_buffer       = p_buffer_space;
    p_ringbuffer->ring_mask      = buffer_size - 1;
    p_ringbuffer->tail_index     = 0;
    p_ringbuffer->head_index     = 0;
}

void cat_ringbuffer_clear(cat_ringbuffer_t *p_ringbuffer)
{
    CAT_ASSERT(NULL != p_ringbuffer);
    p_ringbuffer->tail_index     = 0;
    p_ringbuffer->head_index     = 0;
}

cat_int32_t cat_ringbuffer_put(cat_ringbuffer_t *p_ringbuffer, cat_uint8_t data)
{
    cat_int32_t ret = CAT_ERROR;

    if(0 != cat_ringbuffer_is_full(p_ringbuffer))
    {
        /* 如果满了就覆盖最早的 */
        //CAT_SYS_PRINTF("[ringbuffer] buffer overflow\r\n");

        /* 尾部索引往前走 */
        //p_ringbuffer->tail_index = ((p_ringbuffer->tail_index + 1) & (p_ringbuffer->ring_mask));
        RINGBUFFER_INDEX_MOVE_FOWARD(p_ringbuffer, tail_index);
    }
    else
    {
        ret = CAT_EOK;
    }

    p_ringbuffer->p_buffer[p_ringbuffer->head_index] = data;
    //p_ringbuffer->head_index = ((p_ringbuffer->head_index + 1) & (p_ringbuffer->ring_mask));
    RINGBUFFER_INDEX_MOVE_FOWARD(p_ringbuffer, head_index);


    return ret;
}


cat_int32_t cat_ringbuffer_get(cat_ringbuffer_t *p_ringbuffer, cat_uint8_t *p_data)
{
    cat_int32_t ret = CAT_ERROR;

    if(0 == cat_ringbuffer_is_empty(p_ringbuffer))
    {
        /* 不为空才执行 */
        *p_data = p_ringbuffer->p_buffer[p_ringbuffer->tail_index];
        //p_ringbuffer->tail_index = ((p_ringbuffer->tail_index + 1) & (p_ringbuffer->ring_mask));
        RINGBUFFER_INDEX_MOVE_FOWARD(p_ringbuffer, tail_index);

        ret = CAT_EOK;
    }

    return ret;
}

cat_int32_t cat_ringbuffer_put_more(cat_ringbuffer_t *p_ringbuffer, const cat_uint8_t *p_data, cat_uint32_t size)
{
    cat_int32_t ret = 0;
    cat_uint32_t i = 0;

    for(i=0; i<size; i++)
    {
        ret += cat_ringbuffer_put(p_ringbuffer, p_data[i]);
    }

    if(0 != ret)
    {
        ret = CAT_ERROR;
    }

    return ret;
}

cat_uint32_t cat_ringbuffer_get_more(cat_ringbuffer_t *p_ringbuffer, cat_uint8_t *p_data, cat_uint32_t size)
{
    cat_int32_t err = CAT_ERROR;
    cat_uint8_t *p = NULL;
    cat_uint32_t cnt = 0;


    if(0 == cat_ringbuffer_is_empty(p_ringbuffer))
    {
        /* 不为空才继续 */
        p   = p_data;
        cnt = 0;

        do{
            err = cat_ringbuffer_get(p_ringbuffer, p);
            cnt++;
            p++;
        }while(
            (cnt < size) &&
            (CAT_EOK == err)
        );

    }

    return cnt;
}

/* 测试用函数 */
void cat_ringbuffer_print_all(cat_ringbuffer_t *p_ringbuffer)
{
    cat_uint32_t idx = p_ringbuffer->tail_index;

    printf("************\n****tail=%2d, head=%2d****\n", (p_ringbuffer->tail_index), (p_ringbuffer->head_index));
    while(idx != (p_ringbuffer->head_index))
    {
        printf("index=%2d, data=0x%x\n", idx, p_ringbuffer->p_buffer[idx]);
        idx = (idx + 1) & (p_ringbuffer->ring_mask);
    }
    printf("************\n");
}
