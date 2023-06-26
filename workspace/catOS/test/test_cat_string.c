/**
 * @file test_cat_string.c
 * @brief 测试cat_string
 * @author mio (648137125@qq.com)
 * @version 1.0
 * @date 2022-08-05
 * Change Logs:
 * Date           Author        Notes
 * 2022-08-05     mio     first verion
 * 
 */
#include "catos_config.h"
#include "catos_types.h"

#include "cat_string.h"

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"
#include "cat_stdio.h"

void do_test_htoitoh(void)
{
    cat_uint8_t test_htoitoh_hex[] = "0x12345678";
    cat_uint32_t test_htoitoh_dec = 0;
    cat_uint8_t  buff[11];

    CAT_KPRINTF("test_cat_string:%s\r\n", __func__);

    cat_htoi(&test_htoitoh_dec, (const cat_uint8_t *)test_htoitoh_hex);
    cat_itoh(buff, test_htoitoh_dec);

    CAT_KPRINTF("%s --> %d --> %s\r\n", test_htoitoh_hex, test_htoitoh_dec, buff);

}

void *do_test_cat_string(void *arg)
{
    (void)arg;

    do_test_htoitoh();

    return NULL;
}
CAT_DECLARE_CMD(test_string, test_string, do_test_cat_string);

#endif /* #if (CATOS_ENABLE_CAT_SHELL == 1) */