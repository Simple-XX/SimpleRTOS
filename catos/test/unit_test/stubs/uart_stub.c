/**
 * @file uart_stub.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 串口设备桩函数
 * @version 0.1
 * @date 2025-02-25
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-02-25 <td>内容
 * </table>
 */

#include "catos_config.h"
#include "catos_types.h"

#include "cat_device.h"

cat_u32 uart_read(cat_device_t *dev, cat_i32 pos, void *buffer, cat_u32 size)
{
    (void)pos;

    printf("no read implemented\n");

    return 0;
}
cat_u32 uart_write(cat_device_t *dev, cat_i32 pos, const void *buffer, cat_u32 size)
{
    (void)pos;
    
    cat_u32 i=0;
    while(i<size)
    {
        putchar(((char*)buffer)[i++]);
    }

    return 0;
}
cat_u8 uart_ctrl(cat_device_t *dev, int cmd, void *args)
{
    (void)dev;
    (void)cmd;
    (void)args;
    return CAT_EOK;
}

cat_device_t uart1_dev = {
    .type = CAT_DEVICE_TYPE_CHAR,
    .init = CAT_NULL,
    .open = CAT_NULL,
    .close = CAT_NULL,
    .read = uart_read,
    .write = uart_write,
    .ctrl = uart_ctrl,

    .pri_data = CAT_NULL};

/* 挂载所有uart设备 */
cat_u8 cat_debug_uart_register(void)
{
    cat_u8 err = CAT_EOK;

    err = cat_device_register(
        &uart1_dev,
        "debug_uart",
        CAT_DEVICE_MODE_RDWR);

    if (CAT_EOK != err)
    {
        while (1)
            ;
    }

    return err;
}
