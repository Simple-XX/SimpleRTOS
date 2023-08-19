#include "usr_hmc.h"

#define HMC5883L_DEV_ADDR       0x3c
#define HMC5883L_REG_CONFIG_A   0x00
#define HMC5883L_REG_CONFIG_B   0x01
#define HMC5883L_REG_MODE       0x02

#define HMC5883L_REG_DX_MSB     0x03
#define HMC5883L_REG_DX_LSB     0x04
#define HMC5883L_REG_DZ_MSB     0x05
#define HMC5883L_REG_DZ_LSB     0x06
#define HMC5883L_REG_DY_MSB     0x07
#define HMC5883L_REG_DY_LSB     0x08

#define HMC5883L_REG_STATUS     0x09

#define HMC5883L_REG_VERIFY_A   0x0a
#define HMC5883L_REG_VERIFY_B   0x0b
#define HMC5883L_REG_VERIFY_C   0x0c

#define HMC5883L_IIC_BUS &soft_iic_3

#define HMC_WRITE_IIC(reg, val) \
    cat_iic_write_reg(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR, reg, val)

#define HMC_READ_IIC(reg)       \
    cat_iic_read_reg(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR, reg)

#define SOFT_IIC_3_CONFIG \
{ \
    .scl = PIN('A', 3), \
    .sda = PIN('A', 2), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"soft_iic3", \
}
cat_iic_bus_t soft_iic_3 = SOFT_IIC_3_CONFIG;


void hmc_init(void)
{
    cat_uint8_t identify_code = 0;

    /* 初始化总线 */
    cat_iic_init(HMC5883L_IIC_BUS);

    cat_iic_wave(HMC5883L_IIC_BUS);

    CAT_KPRINTF("[hmc5883l] start init\r\n");

    /* 读取 identify 寄存器 */
    // identify_code = HMC_READ_IIC(HMC5883L_REG_VERIFY_A);
    // CAT_KPRINTF("[hmc5883l] identify A=%x\r\n", identify_code);
    // identify_code = HMC_READ_IIC(HMC5883L_REG_VERIFY_B);
    // CAT_KPRINTF("[hmc5883l] identify B=%x\r\n", identify_code);
    // identify_code = HMC_READ_IIC(HMC5883L_REG_VERIFY_C);
    // CAT_KPRINTF("[hmc5883l] identify C=%x\r\n", identify_code);

    /* 设置数据输出速率 1.5 Hz，测量模式预留 */
    HMC_WRITE_IIC(HMC5883L_REG_CONFIG_A, 0x70);

    /* 设置增益为 1090 高斯 */
    HMC_WRITE_IIC(HMC5883L_REG_CONFIG_B, 0x20);

    /* 设置为连续测量模式 */
    HMC_WRITE_IIC(HMC5883L_REG_MODE, 0x00);
    CAT_KPRINTF("[hmc5883l] init done\r\n");
}

void hmc_get_all_data(cat_float_t *x, cat_float_t *z, cat_float_t *y)
{
    cat_int32_t xr=0, yr=0, zr=0;
    cat_uint8_t tmp = 0;

    cat_iic_start(HMC5883L_IIC_BUS);
    cat_iic_send_byte(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR);
    cat_iic_send_byte(HMC5883L_IIC_BUS, HMC5883L_REG_DX_MSB);
    cat_iic_start(HMC5883L_IIC_BUS);
    cat_iic_send_byte(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR + 1);

    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    xr = tmp;
    cat_iic_send_ack(HMC5883L_IIC_BUS);
    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    xr  = (xr << 8) | tmp;
    cat_iic_send_ack(HMC5883L_IIC_BUS);
    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    zr = tmp;
    cat_iic_send_ack(HMC5883L_IIC_BUS);
    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    zr = (zr << 8) | tmp;
    cat_iic_send_ack(HMC5883L_IIC_BUS);
    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    yr = tmp;
    cat_iic_send_ack(HMC5883L_IIC_BUS);
    tmp = cat_iic_read_byte(HMC5883L_IIC_BUS);
    yr = (yr << 8) | tmp;
    cat_iic_send_nack(HMC5883L_IIC_BUS);

    cat_iic_stop(HMC5883L_IIC_BUS);

#if 0
    if(xr > 0x7fff) xr -= 0xffff;
    if(zr > 0x7fff) zr -= 0xffff;
    if(yr > 0x7fff) yr -= 0xffff;

    *x = xr;
    *z = zr;
    *y = yr;
#else
    /* 要除以增益 */
    *x = (cat_float_t)xr / 1090;
    *y = (cat_float_t)yr / 1090;

    /* z不准, 直接搞成 0 */
    *z = 0;
#endif
}
