#include "cat_stdio.h"
#include "cat_device.h"
#include "cat_list.h"

#if (CATOS_ENABLE_DEVICE_MODEL == 1)

#ifdef TEST_CAT_DEVICE_ALLOC_FREE_ID
extern cat_uint32_t id_table[8];
cat_uint8_t _cat_device_alloc_id(void);
cat_uint8_t _cat_device_free_id(cat_uint8_t id);
#endif

/* STATIC FUNCS DECL START */
static void print_device(cat_device_t *dev);
static void list_device(void);
/* STATIC FUNCS DECL END */
/* STATIC FUNCS DEF START */
static void print_device(cat_device_t *dev)
{
    CAT_KPRINTF("name=%s, type=%d, state=%d, aval_mode=%d, open_mode=%d, ref_count=%d, device_id=%d\r\n",
            dev->device_name,
            dev->type,
            dev->state,
            dev->aval_mode,
            dev->open_mode,
            dev->ref_count,
            dev->device_id
        );
}
static void list_device(void)
{
    cat_device_t *dev = NULL;
    cat_node_t *tmp = NULL;
    CAT_KPRINTF("\n*%d devices registerd*\r\n", cat_device_list.node_cnt);
    CAT_LIST_FOREACH(&cat_device_list, tmp)
    {
        dev = CAT_LIST_ENTRY(tmp, cat_device_t, link_node);
        
        print_device(dev);
    }
}
/* STATIC FUNCS DEF END */
/* TEST ID_ALLOC FREE */
#ifdef TEST
void test_device_id_alloc_free(void)
{
    int id = 0;
    int i = 0;

    CAT_KPRINTF("*** alloc 256 times ***\r\n");
    for(i = 0; i < 256; i++)
    {
        id = _cat_device_alloc_id();
        CAT_KPRINTF("i=%d, id=%d\r\n", i, id);
    }

#if 0
    CAT_KPRINTF("*** the 257nd time ***\r\n");
    id = _cat_device_alloc_id();
    CAT_KPRINTF("i=%d, id=%d\r\n", i, id);
#endif
    int ret = 0;
    CAT_KPRINTF("*** free 3 128 243 0 256 ***\r\n");
    ret += _cat_device_free_id(3);
    ret += _cat_device_free_id(128);
    ret += _cat_device_free_id(243);
    ret += _cat_device_free_id(0);
    ret += _cat_device_free_id(255);
    CAT_KPRINTF("ret = %d\r\n", ret);

    CAT_KPRINTF("*** alloc 5 times ***\r\n");
    for(i = 0; i < 5; i++)
    {
        id = _cat_device_alloc_id();
        CAT_KPRINTF("i=%d, id=%d\r\n", i, id);
    }

#if 1
    CAT_KPRINTF("*** free duplicated ***\r\n");
    ret = 0;
    ret += _cat_device_free_id(3);
    ret += _cat_device_free_id(3);
    CAT_KPRINTF("ret = %d\r\n", ret);
#endif
}
#endif

/* TEST_LIST */
typedef struct _test_node_t
{
    cat_node_t link_node;
    int val;
}test_node_t;

cat_list_t list;
test_node_t node_a = {
    .val = 243,
};
test_node_t node_b = {
    .val = 335
};
test_node_t node_c = {
    .val = 437
};
void test_list(void)
{
    

    cat_list_init(&list);

    cat_list_add_last(&list, &(node_a.link_node));
    cat_list_add_last(&list, &(node_b.link_node));
    cat_list_add_last(&list, &(node_c.link_node));

    test_node_t *tmp = NULL;
    cat_node_t  *cur_node = NULL;

    CAT_LIST_FOREACH(&list, cur_node)
    {
        tmp = CAT_LIST_ENTRY(cur_node, test_node_t, link_node);
        CAT_KPRINTF("%d\r\n", tmp->val);
    }
}

/* TEST_REGISTER */
cat_uint8_t test_init   (cat_device_t *dev)
{
    CAT_KPRINTF("[%s]:dev=%s\r\n", __func__, dev->device_name);
    return CAT_EOK;
}
cat_uint8_t test_open   (cat_device_t *dev, cat_uint16_t open_mode)
{
    CAT_KPRINTF("[%s]:dev=%s\r\n", __func__, dev->device_name);

    /* 这里仅赋值操作，实际设备驱动中应该根据open_mode对硬件进行设置 */
    dev->open_mode = (open_mode & CAT_DEVICE_OPEN_MODE_MASK);

    return CAT_EOK;
}
cat_uint8_t test_close  (cat_device_t *dev)
{
    CAT_KPRINTF("[%s]:dev=%s\r\n", __func__, dev->device_name);
    return CAT_EOK;
}
cat_uint8_t test_read   (cat_device_t *dev, cat_int32_t pos, void *buffer, cat_uint32_t size)
{
    CAT_KPRINTF("[%s]:dev=%s\r\n", __func__, dev->device_name);
    return CAT_EOK;
}
cat_uint8_t test_write  (cat_device_t *dev, cat_int32_t pos, const void *buffer, cat_uint32_t size)
{
    CAT_KPRINTF("[%s]:dev=%s\r\n", __func__, dev->device_name);
    return CAT_EOK;
}
cat_uint8_t test_ctrl(cat_device_t *dev, int cmd, void *args)
{
    CAT_KPRINTF("[%s]:dev=%s, cmd=%d, arg(int)=%d\r\n", __func__, dev->device_name, cmd, *((int*)args));
    return CAT_EOK;
}

cat_device_t test_uart0_dev = {
    .type        = CAT_DEVICE_TYPE_CHAR,
    .init        = test_init,
    .open        = test_open,
    .read        = test_read,
    .write       = test_write,
    .ctrl        = test_ctrl,
};
cat_device_t test_uart1_dev = {
    .type        = CAT_DEVICE_TYPE_CHAR,
    .init        = test_init,
    .open        = test_open,
    .read        = test_read,
    .write       = test_write,
    .ctrl        = test_ctrl,
};
cat_device_t test_iic0_dev = {
    .type        = CAT_DEVICE_TYPE_IIC,
    .init        = test_init,
    .open        = test_open,
    .read        = test_read,
    .write       = test_write,
    .ctrl        = test_ctrl,
};
cat_device_t test_screen0_dev = {
    .type        = CAT_DEVICE_TYPE_GRAPHIC,
    .init        = test_init,
    .open        = test_open,
    .read        = test_read,
    .write       = test_write,
    .ctrl        = test_ctrl,
};

void test_device_register(void)
{
    CAT_KPRINTF("\n*** 1 test device_register ***\r\n");
    int err = 0;
    err += cat_device_register(&test_uart0_dev,   (const cat_uint8_t *)"uart0",   CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    err += cat_device_register(&test_uart1_dev,   (const cat_uint8_t *)"uart1",   CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    err += cat_device_register(&test_iic0_dev,    (const cat_uint8_t *)"iic0",    CAT_DEVICE_MODE_RDWR);
    err += cat_device_register(&test_screen0_dev, (const cat_uint8_t *)"screen0", CAT_DEVICE_MODE_WRONLY | CAT_DEVICE_MODE_DMA_TX);
    CAT_KPRINTF("err = %d\r\n", err);

    list_device();
}

void test_device_open_close(void)
{
    CAT_KPRINTF("\n*** 2 test device_open_close ***\r\n");
    CAT_KPRINTF("-->open test_uart0_dev twice with 0x103\r\n");
    CAT_KPRINTF("-->open test_iic0_dev with 0x003\r\n");
    cat_device_open(&test_uart0_dev, CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    cat_device_open(&test_uart0_dev, CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    cat_device_open(&test_iic0_dev, CAT_DEVICE_MODE_RDWR);
    list_device();

    CAT_KPRINTF("<--close all\r\n");
    cat_device_close(&test_uart0_dev);
    cat_device_close(&test_uart0_dev);
    cat_device_close(&test_iic0_dev);
    list_device();
}

void test_device_read_write(void)
{
    CAT_KPRINTF("\n*** 3 test device_read_write ***\r\n");
    CAT_KPRINTF("-->open test_uart0_dev with 0x103\r\n");
    CAT_KPRINTF("-->open test_iic0_dev with 0x003\r\n");
    cat_device_open(&test_uart0_dev, CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    cat_device_open(&test_iic0_dev, CAT_DEVICE_MODE_RDWR);
    list_device();

    CAT_KPRINTF("-->read device\r\n");
    cat_device_read(&test_uart0_dev, 0, NULL, 0);
    cat_device_read(&test_iic0_dev, 0, NULL, 0);

    CAT_KPRINTF("-->write device\r\n");
    cat_device_write(&test_uart0_dev, 0, NULL, 0);
    cat_device_write(&test_iic0_dev, 0, NULL, 0);

    CAT_KPRINTF("<--close all\r\n");
    cat_device_close(&test_uart0_dev);
    cat_device_close(&test_iic0_dev);
    list_device();
}

void test_device_ctrl(void)
{
    CAT_KPRINTF("\n*** 4 test device_open_close ***\r\n");
    CAT_KPRINTF("-->open test_uart0_dev with 0x103\r\n");
    CAT_KPRINTF("-->open test_iic0_dev with 0x003\r\n");
    cat_device_open(&test_uart0_dev, CAT_DEVICE_MODE_RDWR   | CAT_DEVICE_MODE_INT_RX);
    cat_device_open(&test_iic0_dev, CAT_DEVICE_MODE_RDWR);
    list_device();

    int a = 5, b = 6;
    CAT_KPRINTF("-->ctrl test_uart0_dev with: cmd=0x2, arg=&(int a=5)\r\n");
    cat_device_ctrl(&test_uart0_dev, 0x2, (void*)&a);
    CAT_KPRINTF("-->ctrl test_uart0_dev with: cmd=0x4, arg=&(int b=6)\r\n");
    cat_device_ctrl(&test_iic0_dev, 0x4, (void*)&b);

    CAT_KPRINTF("<--close all\r\n");
    cat_device_close(&test_uart0_dev);
    cat_device_close(&test_iic0_dev);
    list_device();
}

void test_device_unregister(void)
{
    CAT_KPRINTF("\n*** test device_unregister ***\r\n");
    int err = 0;
    CAT_KPRINTF("-->unregister all\r\n");
    err += cat_device_unregister(&test_uart0_dev);
    err += cat_device_unregister(&test_uart1_dev);
    err += cat_device_unregister(&test_iic0_dev);
    err += cat_device_unregister(&test_screen0_dev);
    CAT_KPRINTF("err = %d\r\n", err);

    list_device();
}

int do_test_device(void)
{
    CAT_KPRINTF("************* TEST DEVICE MODEL ****************\r\n");
    cat_device_module_init();

#ifdef TEST
    test_device_id_alloc_free();
#endif

#if 0
    test_list();
#endif

#if 1
    test_device_register();
#endif


    test_device_open_close();

    test_device_read_write();

    test_device_ctrl();

#if 1
    test_device_unregister();
#endif

    return 0;
}

#endif /* #if (CATOS_ENABLE_DEVICE_MODEL == 1) */
