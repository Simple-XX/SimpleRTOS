/**
 * @file cat_device.h
 * @brief 设备驱动框架
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2022-07-30
 * Change Logs:
 * Date           Author        Notes
 * 2022-07-30     mio     first verion
 * 
 */
#ifndef CAT_DEVICE_H
#define CAT_DEVICE_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_list.h"

/*
 * 这里是用在返回值不是错误代码的函数中的
 * todo : 需要将cat_error独立为一个模块，提供errno功能 */
#define CAT_DEVICE_ERROR() while(1)


/* MACRO START */
/* 设备状态 */
#define CAT_DEVICE_STATE_UNINIT           0x000      /**< 未初始化 */
#define CAT_DEVICE_STATE_INITED           0x001      /**< 设备已初始化 */
#define CAT_DEVICE_STATE_OPENED           0x008      /**< 设备已打开 */
#define CAT_DEVICE_STATE_ACTIVATED        0x010      /**< 正在运行(初始化完成即进入该状态) */
#define CAT_DEVICE_STATE_SUSPENDED        0x020      /**< 挂起 */
/* 设备运行状态和允许的读写模式(设置设备时传入的参数) */
// #define CAT_DEVICE_MODE_UNINIT           0x000      /**< 未初始化 */
#define CAT_DEVICE_MODE_RDONLY           0x001      /**< 只读 */
#define CAT_DEVICE_MODE_WRONLY           0x002      /**< 只写 */
#define CAT_DEVICE_MODE_RDWR             0x003      /**< 可读可写 */
//#define CAT_DEVICE_MODE_REMOVABLE        0x004      /**< 可移除 */
//#define CAT_DEVICE_MODE_STANDALONE       0x008      /**<  */
// #define CAT_DEVICE_MODE_ACTIVATED        0x010      /**< 正在运行(初始化完成即进入该状态) */
// #define CAT_DEVICE_MODE_SUSPENDED        0x020      /**< 挂起 */
//#define CAT_DEVICE_MODE_STREAM           0x040      /**< 终端中使用的输出模式 */
#define CAT_DEVICE_MODE_INT_RX           0x100      /**< 中断接收 */
#define CAT_DEVICE_MODE_DMA_RX           0x200      /**< DMA接收 */
#define CAT_DEVICE_MODE_INT_TX           0x400      /**< 中断发送 */
#define CAT_DEVICE_MODE_DMA_TX           0x800      /**< DMA发送 */

/* 设备打开后要以何种方式使用(打开设备传入的参数) */
// #define CAT_DEVICE_OPEN_MODE_CLOSE            0x000      /**< 关闭 */
#define CAT_DEVICE_OPEN_MODE_RDONLY           0x001      /**< 只读 */
#define CAT_DEVICE_OPEN_MODE_WRONLY           0x002      /**< 只写 */
#define CAT_DEVICE_OPEN_MODE_RDWR             0x003      /**< 可读可写 */
// #define CAT_DEVICE_OPEN_MODE_OPEN             0x008      /**< 设备已打开 */
#define CAT_DEVICE_OPEN_MODE_MASK             0xf0f      /**< 掩码(打开模式可用的位) */
/* MACRO END */

/* DATA STRUCTS START */

enum _cat_device_type_t
{
    CAT_DEVICE_TYPE_CHAR = 0,                         /**< 字符(以字节作为io单位) */
    CAT_DEVICE_TYPE_BLK,                              /**< 块(可多字节io) */
    CAT_DEVICE_TYPE_RTC,                              /**< 实时时钟 */
    CAT_DEVICE_TYPE_GRAPHIC,                          /**< 图形和显示 */
    CAT_DEVICE_TYPE_IIC,                              /**< I2C 总线 */

    CAT_DEVICE_TYPE_UNDEF                             /**< 未定义设备 */
};

struct _cat_device_t
{
    cat_uint8_t                   *device_name;           /**< 设备名称 */

    struct _cat_node_t        link_node;              /**< 链表节点 */

    cat_device_type_t         type;                   /**< 设备类型 */
    cat_uint16_t                  state;                  /**< 设备状态 */
    cat_uint16_t                  aval_mode;              /**< 允许的运行模式 */
    cat_uint16_t                  open_mode;              /**< 本次被打开时使用的运行模式 */

    cat_uint8_t                   ref_count;              /**< 设备被引用的次数 */
    cat_uint8_t                   device_id;              /**< 设备号 */

    /* 中断回调函数 */
    cat_uint8_t (*rx_callback)(cat_device_t*dev, cat_uint32_t size); /**< 中断接收通知 */
    cat_uint8_t (*tx_callback)(cat_device_t*dev, void *buffer);  /**< 中断发送完成 */

    /* 通用设备接口，也可包含device结构后自定义操作接口 */
    cat_uint8_t (*init)   (cat_device_t*dev);
    cat_uint8_t (*open)   (cat_device_t*dev, cat_uint16_t oflag);
    cat_uint8_t (*close)  (cat_device_t*dev);
    cat_uint32_t (*read)   (cat_device_t*dev, cat_int32_t pos, void *buffer, cat_uint32_t size);
    cat_uint32_t (*write)  (cat_device_t*dev, cat_int32_t pos, const void *buffer, cat_uint32_t size);
    cat_uint8_t (*ctrl)(cat_device_t*dev, int cmd, void *args);

    void                     *pri_data;            /**< 设备独有的数据 */
};
/* DATA STRUCTS END */
/* PUBLIC VAR DECL START */
extern struct _cat_list_t cat_device_list;
/* PUBLIC VAR DECL END */
/* FUNCS START */
/**
 * @brief 设备驱动框架模块初始化(主要是链表)
 * 
 */
void cat_device_module_init(void);
/**
 * @brief 根据名称获取设备结构体指针
 * 
 * @param name                    : 设备名称
 * @return cat_device_t* : 设备指针(句柄)
 */
cat_device_t *cat_device_get(const cat_uint8_t *name);

/**
 * @brief 根据设备号获取设备结构体指针
 * 
 * @param id                      : 设备号
 * @return cat_device_t* : 设备指针(句柄)
 */
cat_device_t *cat_device_get_by_id(cat_uint8_t id);

/**
 * @brief 注册设备(只会初始化链表节点和参数中的成员，其余由用户自行初始化和赋值)
 * 
 * @param dev                      : 设备指针
 * @param name                     : 设备名
 * @param aval_mode                : 允许的运行模式
 * @return cat_uint8_t                 : 0->成功
 */
cat_uint8_t cat_device_register(cat_device_t *dev, const cat_uint8_t *name, cat_uint16_t aval_mode);

/**
 * @brief 移除设备(从注册移除)
 * 
 * @param dev                      : 设备指针
 * @return cat_uint8_t                 : 0->成功
 */
cat_uint8_t cat_device_unregister(cat_device_t *dev);

/**
 * @brief 设置接收中断回调函数
 * 
 * @param dev 
 * @param rx_cbk 
 * @return cat_uint8_t 
 */
cat_uint8_t cat_device_set_rx_cbk(cat_device_t *dev, cat_uint8_t (*rx_cbk)(cat_device_t *dev, cat_uint32_t size));

/**
 * @brief 设置发送中断回调函数
 * 
 * @param dev 
 * @param tx_cbk 
 * @return cat_uint8_t 
 */
cat_uint8_t cat_device_set_tx_cbk(cat_device_t *dev, cat_uint8_t (*tx_cbk)(cat_device_t *dev, void *buffer));

/* 设备控制接口，通过调用相应设备自身的操作函数 */
cat_uint8_t cat_device_init(cat_device_t *dev);
cat_uint8_t cat_device_open(cat_device_t *dev, cat_uint16_t open_mode);
cat_uint8_t cat_device_close(cat_device_t *dev);
cat_uint32_t cat_device_read(cat_device_t *dev, cat_int32_t pos, void *buffer, cat_uint32_t size);
cat_uint32_t cat_device_write(cat_device_t *dev, cat_int32_t pos, const void *buffer, cat_uint32_t size);
cat_uint8_t cat_device_ctrl(cat_device_t *dev, cat_uint8_t cmd, void *arg);

/* FUNCS END */

#endif
