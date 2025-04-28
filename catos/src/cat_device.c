/**
 * @file cat_device.c
 * @brief 设备驱动框架
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2022-07-30
 * Change Logs:
 * Date           Author        Notes
 * 2022-07-30     mio     first verion
 * 
 */

#include "cat_device.h"
#include "cat_lib.h"
#include "cat_intr.h"
#include "cat_assert.h"

#include "port.h"

#if (CATOS_DEVICE_MODEL_ENABLE == 1)

/* PUBLIC VAR START */
cat_list_t cat_device_list;
/* PUBLIC VAR END */
/* STATIC VAT START */
static cat_u32 id_table[8];
/* STATIC VAT END */
/* PRIVATE FUNC DECL START */
/**
 * @brief 分配设备号
 * 
 * @return cat_u8 分配得到的设备号
 */
static cat_u8 _cat_device_alloc_id(void);

/**
 * @brief 释放设备号
 * 
 * @return cat_u8 0->成功
 */
static cat_u8 _cat_device_free_id(cat_u8 id);
#endif
/* PRIVATE FUNC DECL END */
/* PRIVATE FUNC DEF START */
cat_u8 _cat_device_alloc_id(void)
{
    cat_u8 offset = 0;           /* 32位偏移，用来遍历32位的id_table */
    cat_u8 bit = 0;              /* 位偏移 */
    
    while(0xffffffff == id_table[offset])
    {
        if(++offset == 8)
        {
            break;
        }
    };

    if(8 == offset)
    {
        /* 说明设备号已满，此处做相应的错误处理 */
        CAT_DEVICE_ERROR();
    }
    else
    {
        /* 找到该table项的最低零位 */
        while((id_table[offset] & (1 << bit)) != 0)
        {
            if(++bit == 32)
            {
                break;
            }
        };

        if(32 == bit)
        {
            CAT_DEVICE_ERROR();
        }
        else
        {
            /* 将相应位置一，表示该id已被分配 */
            id_table[offset] |= (1 << bit);
        }
    }
    
    return ((offset * 32) + bit);
}

cat_u8 _cat_device_free_id(cat_u8 id)
{
    cat_u8 ret = CAT_EOK;

    cat_u8 offset = id / 32;           /* 32位偏移，用来遍历32位的id_table */
    cat_u8 bit = id % 32;              /* 位偏移 */

    /* 此处==之前的表达式一定要用括号
     * 因为位运算符 & 优先级低于 逻辑运算符 ==
     * 若不加括号则相当于：
     * id_table[offset] & ((1 << bit) == 0) <exp1>
     * 由于表达式(1 << bit) == 0必定为假，则<exp1>等效于：
     * id_table[offset] & False <exp2>
     * <exp2>必定为假，故此处逻辑判断分支不会执行，重复free只会不断清零相同位
     */
    if((id_table[offset] & (1 << bit)) == 0)
    {
        ret = CAT_ERROR;
    }
    else
    {
        /* 将相应的id位清零 */
        id_table[offset] &= ~(1 << bit);
    }


    return ret;
}
/* PRIVATE FUNC DEF END */
/* PUBLIC FUNCS DEF START */
/**
 * @brief 设备驱动框架模块初始化(主要是链表)
 * 
 */
void cat_device_module_init(void)
{
    cat_list_init(&cat_device_list);
}
/**
 * @brief 根据名称获取设备结构体指针
 * 
 * @param name                    : 设备名称
 * @return cat_device_t * : 设备指针(句柄)
 */
cat_device_t *cat_device_get(const char *name)
{
    cat_device_t *ret = CAT_NULL;
    cat_node_t *tmp = CAT_NULL;

    CAT_LIST_FOREACH_NO_REMOVE(&cat_device_list, tmp)
    {
        ret = CAT_GET_CONTAINER(tmp, cat_device_t, link_node);
        
        if(0 == cat_strcmp(name, ret->device_name))
        {
            /* 找到就退出 */
            break;
        }
        else
        {
            /* 没找到就为空，作为返回值 */
            ret = CAT_NULL;
        }
    }

    return ret;
}

/**
 * @brief 根据设备号获取设备结构体指针
 * 
 * @param id                      : 设备号
 * @return cat_device_t * : 设备指针(句柄)
 */
cat_device_t *cat_device_get_by_id(cat_u8 id)
{
    (void)id;

    return CAT_NULL;
}

/**
 * @brief 注册设备(只会初始化链表节点和参数中的成员，其余由用户自行初始化和赋值)
 * 
 * @param dev                      : 设备指针
 * @param name                     : 设备名
 * @param aval_mode                : 允许的运行模式
 * @return cat_u8                 : 0->成功
 */
cat_u8 cat_device_register(cat_device_t *dev, const char *name, cat_u16 aval_mode)
{
    cat_u8 ret = CAT_EOK;
    
    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        if(CAT_NULL != cat_device_get(name))
        {
            ret = CAT_ERROR;
        }
        else
        {
            /* 初始化结构体成员 */
            dev->device_name = name;
            cat_list_node_init(&dev->link_node);
            // dev->type = CAT_DEVICE_TYPE_UNDEF;
            dev->aval_mode = aval_mode;
            // dev->open_mode = 0;
            // dev->ref_count = 0;
            dev->device_id = _cat_device_alloc_id();

            // dev->rx_callback = CAT_NULL;
            // dev->tx_callback = CAT_NULL;
            // dev->init        = CAT_NULL;
            // dev->open        = CAT_NULL;
            // dev->close       = CAT_NULL;
            // dev->read        = CAT_NULL;
            // dev->write       = CAT_NULL;
            // dev->ctrl        = CAT_NULL;

            // dev->pri_data    = CAT_NULL;    /* 私有数据由相应的设备自行设置 */

            /* 将设备挂到设备链表中 */
            cat_irq_disable();
            cat_list_add_last(&cat_device_list, &dev->link_node);
            cat_irq_enable();

        }/* if(CAT_NULL != cat_device_get(name)) */
    }/* if(CAT_NULL == dev) */

    return ret;
}

/**
 * @brief 移除设备(从注册移除)
 * 
 * @param dev                      : 设备指针
 * @return cat_u8                 : 0->成功
 */
cat_u8 cat_device_unregister(cat_device_t *dev)
{
    cat_u8 ret = CAT_EOK;

    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        ret = _cat_device_free_id(dev->device_id);

        cat_irq_disable();

        /* 从设备链表节点移除 */
        cat_list_remove_node(&(dev->link_node));
        cat_irq_enable();
    }

    return ret;
}

/**
 * @brief 设置接收中断回调函数
 * 
 * @param dev 
 * @param rx_cbk 
 * @return cat_u8 
 */
cat_u8 cat_device_set_rx_cbk(cat_device_t *dev, cat_u8 (*rx_cbk)(cat_device_t *dev, cat_u32 size))
{
    cat_u8 ret = CAT_EOK;

    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        dev->rx_callback = rx_cbk;
    }

    return ret;
}

/**
 * @brief 设置发送中断回调函数
 * 
 * @param dev 
 * @param tx_cbk 
 * @return cat_u8 
 */
cat_u8 cat_device_set_tx_cbk(cat_device_t *dev, cat_u8 (*tx_cbk)(cat_device_t *dev, void *buffer))
{
    cat_u8 ret = CAT_EOK;

    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        dev->tx_callback = tx_cbk;
    }

    return ret;
}

/* 设备控制接口，通过调用相应设备自身的操作函数 */
cat_u8 cat_device_init(cat_device_t *dev)
{
    cat_u8 ret = CAT_EOK;

    if(
        (CAT_NULL == dev) ||
        (CAT_NULL == dev->init)
    )
    {
        ret = CAT_ERROR;
    }
    else
    {
        /* 未初始化才进行，防止重复初始化 */
        if(0 == (dev->state & CAT_DEVICE_STATE_INITED))
        {
            ret = dev->init(dev);
            if(CAT_EOK != ret)
            {
                CAT_DEVICE_ERROR();
            }
            else
            {
                dev->state = CAT_DEVICE_STATE_INITED;
            }
        }
    }

    return ret;
}
cat_u8 cat_device_open(cat_device_t *dev, cat_u16 open_mode)
{
    cat_u8 ret = CAT_EOK;

    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        /* 如果没初始化则先初始化 */
        cat_device_init(dev);

        /* 打开设备 */
        if(CAT_NULL != dev->open)
        {
            /* 有初始化函数时参数被传递到底层进行相应处理 */
            ret = dev->open(dev, open_mode);
        }
        else
        {
            /* 若没有打开函数则说明不需要开启函数，直接对开启模式赋值 */
            dev->open_mode = (open_mode & CAT_DEVICE_OPEN_MODE_MASK);
        }

        if(CAT_EOK == ret)
        {
            dev->state |= CAT_DEVICE_STATE_OPENED;
            dev->ref_count++;

            /* 防止溢出 */
            CAT_ASSERT(0 != dev->ref_count);
        }

    }

    return ret;
}
cat_u8 cat_device_close(cat_device_t *dev)
{
    cat_u8 ret = CAT_EOK;

    if(
        (CAT_NULL == dev) ||
        (0 == dev->ref_count)
    )
    {
        ret = CAT_ERROR;
    }
    else
    {
        /* 引用数减一 */
        dev->ref_count--;

        /* 若没有引用则真正关闭 */
        if(0 == dev->ref_count)
        {
            if(CAT_NULL != dev->close)
            {
                ret = dev->close(dev);
            }

            if(CAT_EOK == ret)
            {
                dev->state &= ~CAT_DEVICE_STATE_OPENED;
                dev->open_mode = 0x0;
            }/* if(CAT_EOK == ret) */
        }/* if(0 == dev->ref_count) */
    }

    return ret;
}
cat_u32 cat_device_read(cat_device_t *dev, cat_i32 pos, void *buffer, cat_u32 size)
{
    cat_u32 ret = 0;

    if(
        (CAT_NULL == dev) ||
        (0 == dev->ref_count)
    )
    {
        CAT_DEVICE_ERROR();
    }
    else
    {
        if(CAT_NULL != dev->read)
        {
            ret = dev->read(dev, pos, buffer, size);
        }/* if(CAT_NULL != dev->read) */
    }

    return ret;
}
cat_u32 cat_device_write(cat_device_t *dev, cat_i32 pos, const void *buffer, cat_u32 size)
{
    cat_u32 ret = CAT_EOK;

    if(
        (CAT_NULL == dev) ||
        (0 == dev->ref_count)
    )
    {
        ret = CAT_ERROR;
    }
    else
    {
        if(CAT_NULL != dev->write)
        {
            ret = dev->write(dev, pos, buffer, size);
        }
    }

    return ret;
}
cat_u8 cat_device_ctrl(cat_device_t *dev, cat_u8 cmd, void *arg)
{
    cat_u8 ret = CAT_EOK;

    if(CAT_NULL == dev)
    {
        ret = CAT_ERROR;
    }
    else
    {
        if(CAT_NULL != dev->ctrl)
        {
            ret = dev->ctrl(dev, cmd, arg);
        }
    }

    return ret;
}
/* PUBLIC FUNCS DEF END */

#if (CATOS_CAT_SHELL_ENABLE == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_list_device(void *arg)
{
    (void)arg;

    cat_device_t *dev = CAT_NULL;
    cat_node_t *tmp = CAT_NULL;

    cat_kprintf("-->%d devices:\r\n", cat_list_count(&cat_device_list));

    CAT_LIST_FOREACH_NO_REMOVE(&cat_device_list, tmp)
    {
        dev = CAT_GET_CONTAINER(tmp, cat_device_t, link_node);
        cat_kprintf(
            "id=%2d, name=%s, type=%d, state=%d, aval_mode=%d, open_mode=%d, ref_count=%d\r\n",
            dev->device_id,
            dev->device_name,
            dev->type,
            dev->state,
            dev->aval_mode,
            dev->open_mode,
            dev->ref_count
        );

    }

    return CAT_NULL;
}
CAT_DECLARE_CMD(list_device, list devices, do_list_device);
#endif /* #if (CATOS_CAT_SHELL_ENABLE == 1) */
