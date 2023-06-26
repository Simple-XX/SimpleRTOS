/**
 * @file mem_rw.c
 * @author your name (you@domain.com)
 * @brief 用于直观读写寄存器或内存
 * @version 0.1
 * @date 2022-06-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "cat_shell.h"
#include "cat_stdio.h"
#include "cat_string.h"
#include "cat_error.h"

#include "mem_rw.h"

/* private func decls */


/* public func defs*/
cat_uint8_t get_reg(cat_uint32_t *addr, cat_uint32_t *val)
{
    CAT_ASSERT(val);

    cat_uint8_t ret = 0;

    *val = *addr;
    
    return ret;
}

cat_uint8_t set_reg(cat_uint32_t *addr, cat_uint32_t  val)
{
    cat_uint8_t ret = 0;

    *addr = val;

    return ret;
}

/* private func defs */
void *do_getreg(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t reg_addr;
    cat_uint32_t reg_val_dec;
    cat_uint8_t reg_val_hex[11];

    cat_int32_t ret = 0;

    
    if(inst->buffer.arg_num == 0)
    {
        CAT_SYS_PRINTF("[get_reg] usage:getreg [HEX_ADDR]\r\n");
    }
    else
    {
        /* 从buffer获取十六进制地址字符串并转换为整型 */
        ret = cat_htoi(&reg_addr, inst->buffer.args[0]);

        /* 调用get_reg函数获取十进制值 */
        ret = get_reg((cat_uint32_t *)reg_addr, &reg_val_dec);
        if(ret != 0)
        {
            CAT_SYS_PRINTF("[get_reg] fail to read !!\r\n");
        }
        else
        {
            /* 将十进制值转换为十六进制字符串以便打印查看 */
            cat_itoh(reg_val_hex, reg_val_dec);
        }
    }

    CAT_SYS_PRINTF("*(%s)=%s\r\n" ,inst->buffer.args[0], reg_val_hex);

    return NULL;
}
CAT_DECLARE_CMD(getreg, get_4byte_reg, do_getreg);

void *do_setreg(void *arg)
{
    CAT_ASSERT(arg);
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;

    cat_uint32_t reg_addr;
    cat_uint32_t val_want_set;

    cat_int32_t ret = 0;

    
    if(inst->buffer.arg_num == 0)
    {
        CAT_SYS_PRINTF("[set_reg] usage:setreg [HEX_ADDR] [HEX_VAL]\r\n");
    }
    else
    {
        /* 从buffer获取十六进制地址字符串和十六进制待写入值h的字符串并转换为整型 */
        ret = cat_htoi(&reg_addr, inst->buffer.args[0]);
        ret = cat_htoi(&val_want_set, inst->buffer.args[1]);

        /* 调用set_reg函数写入 */
        ret = set_reg((cat_uint32_t *)reg_addr, val_want_set);
        if(ret != 0)
        {
            CAT_SYS_PRINTF("[set_reg] fail to write !!\r\n");
        }
    }

    CAT_SYS_PRINTF("[set_reg] written\r\n");

    return NULL;
}
CAT_DECLARE_CMD(setreg, set_4byte_reg, do_setreg);



