#include "drv_hmc.h"

#define HMC5883L_GET_RANGE 0

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

static cat_iic_bus_t *_HMC5883L_iic_bus_ptr = CAT_NULL;
#define HMC5883L_IIC_BUS _HMC5883L_iic_bus_ptr

#define HMC_WRITE_IIC(reg, val) \
    cat_iic_write_reg(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR, reg, val)

#define HMC_READ_IIC(reg)       \
    cat_iic_read_reg(HMC5883L_IIC_BUS, HMC5883L_DEV_ADDR, reg)

static inline void _get_raw_data(cat_int32_t *x, cat_int32_t *z, cat_int32_t *y)
{
    cat_int32_t xr=0, yr=0, zr=0;
    cat_uint8_t tmp = 0;

    tmp = HMC_READ_IIC(HMC5883L_REG_DX_MSB);
    xr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DX_LSB);
    xr = (xr << 8) | tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DZ_MSB);
    zr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DZ_LSB);
    zr = (zr << 8) | tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DY_MSB);
    yr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DY_LSB);
    yr = (yr << 8) | tmp;

    if(xr > 0x7fff) xr -= 0xffff;
    if(yr > 0x7fff) yr -= 0xffff;
    if(zr > 0x7fff) zr -= 0xffff;

    *x = xr;
    *z = zr;
    *y = yr;
}

#if (HMC5883L_GET_RANGE == 1)
static float midx, midy;
static float kx,   ky;
static void _get_magnet_field(void)
{
    cat_uint8_t i = 0;
    float xmax, xmin, ymax, ymin, zmax, zmin;
    float x, y, z;

    hmc_get_all_data(&xmax, &zmax, &ymax);
    xmin = xmax;
    ymin = ymax;

    for(; i<100; i++)
    {
        hmc_get_all_data(&x, &z, &y);

        xmax = xmax < x ? x : xmax;
        xmin = xmin > x ? x : xmin;

        ymax = ymax < y ? y : ymax;
        ymin = ymin > y ? y : ymin;

        cat_delay_ms(10);
    }

    /* 平均值 */
    midx = (xmax + xmin) / 2;
    midy = (ymax + ymin) / 2;

    /* 系数 */
    kx = 2 / (xmax - xmin);
    ky = 2 / (ymax - ymin);
}
#endif

void hmc_init(cat_iic_bus_t *bus)
{
    cat_uint8_t identify_code[3] = {0};

    CAT_ASSERT(CAT_NULL != bus);
    _HMC5883L_iic_bus_ptr = bus;

    /* 初始化总线 */
    cat_iic_init(HMC5883L_IIC_BUS);

    cat_iic_wave(HMC5883L_IIC_BUS);

    CAT_KPRINTF("[hmc5883l] start init\r\n");

    /* 读取 identify 寄存器 */
    identify_code[0] = HMC_READ_IIC(HMC5883L_REG_VERIFY_A);
    CAT_KPRINTF("[hmc5883l] identify A=%x\r\n", identify_code[0]);
    identify_code[1] = HMC_READ_IIC(HMC5883L_REG_VERIFY_B);
    CAT_KPRINTF("[hmc5883l] identify B=%x\r\n", identify_code[1]);
    identify_code[2] = HMC_READ_IIC(HMC5883L_REG_VERIFY_C);
    CAT_KPRINTF("[hmc5883l] identify C=%x\r\n", identify_code[2]);

    if(
        (identify_code[0] == 0x48) &&
        (identify_code[1] == 0x34) &&
        (identify_code[2] == 0x33)
    )
    {
        CAT_KPRINTF("[hmc5883l] identify pass start setup the sensor\r\n");

        /* 设置数据输出速率 1.5 Hz，测量模式为正常测量模式 */
        HMC_WRITE_IIC(HMC5883L_REG_CONFIG_A, 0x70);

        /* 设置增益为 1090 高斯 */
        HMC_WRITE_IIC(HMC5883L_REG_CONFIG_B, 0x20);

        /* 设置为连续测量模式 */
        HMC_WRITE_IIC(HMC5883L_REG_MODE, 0x00);
        CAT_KPRINTF("[hmc5883l] init done\r\n");
    }
    else
    {
        CAT_PRINT_ERROR("[hmc5883l] identify fail, please check the sensor\r\n");
    }

#if (HMC5883L_GET_RANGE == 1)
    CAT_KPRINTF("[hmc5883l] start mesure field\r\n");
    _get_magnet_field();
    CAT_KPRINTF("[hmc5883l] mesure done, midx=%.2f, midy=%.2f, kx=%.2f, ky=%.2f\r\n", midx, midy, kx, ky);
#endif
}

void hmc_self_test(cat_iic_bus_t *bus)
{
    cat_uint8_t data;
    cat_uint8_t identify_code[3] = {0};

    CAT_ASSERT(CAT_NULL != bus);
    _HMC5883L_iic_bus_ptr = bus;

    cat_iic_init(HMC5883L_IIC_BUS);

    /* 读取 identify 寄存器 */
    identify_code[0] = HMC_READ_IIC(HMC5883L_REG_VERIFY_A);
    CAT_KPRINTF("[hmc5883l] identify A=%x\r\n", identify_code[0]);
    identify_code[1] = HMC_READ_IIC(HMC5883L_REG_VERIFY_B);
    CAT_KPRINTF("[hmc5883l] identify B=%x\r\n", identify_code[1]);
    identify_code[2] = HMC_READ_IIC(HMC5883L_REG_VERIFY_C);
    CAT_KPRINTF("[hmc5883l] identify C=%x\r\n", identify_code[2]);

    if(
        (identify_code[0] == 0x48) &&
        (identify_code[1] == 0x34) &&
        (identify_code[2] == 0x33)
    )
    {
        CAT_KPRINTF("[hmc5883l] identify pass start setup the sensor\r\n");
    }

    CAT_KPRINTF("[hmc5883l] start self test\r\n");

    /* 修改配置寄存器 A 最低两位 */
    HMC_WRITE_IIC(HMC5883L_REG_CONFIG_A, 0x71);
    data = HMC_READ_IIC(HMC5883L_REG_CONFIG_A);
    CAT_KPRINTF("[hmc5883l] config a=%x\r\n", data);

    /* config B 寄存器为默认值0x40 */
    HMC_WRITE_IIC(HMC5883L_REG_CONFIG_B, 0x40);
    data = HMC_READ_IIC(HMC5883L_REG_CONFIG_B);
    CAT_KPRINTF("[hmc5883l] config b=%x\r\n", data);

    /* 设置为单次测量模式 */
    HMC_WRITE_IIC(HMC5883L_REG_MODE, 0x01);
    data = HMC_READ_IIC(HMC5883L_REG_MODE);
    CAT_KPRINTF("[hmc5883l] mode=%x\r\n", data);

    

    cat_int32_t xr=0, yr=0, zr=0;
    cat_uint8_t tmp = 0;
    while(1)
    {
        tmp = HMC_READ_IIC(HMC5883L_REG_STATUS);
        CAT_SYS_PRINTF("before read status=%x\r\n", tmp);

        tmp = HMC_READ_IIC(HMC5883L_REG_DX_MSB);
        xr = tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_DX_LSB);
        xr = (xr << 8) | tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_DZ_MSB);
        zr = tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_DZ_LSB);
        zr = (zr << 8) | tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_DY_MSB);
        yr = tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_DY_LSB);
        yr = (yr << 8) | tmp;
        tmp = HMC_READ_IIC(HMC5883L_REG_STATUS);
        CAT_SYS_PRINTF("xr=%d, zr=%d, yr=%d\r\n", xr, zr, yr);

        //tmp = HMC_READ_IIC(HMC5883L_REG_STATUS);
        CAT_SYS_PRINTF("after read status=%x\r\n\r\n", tmp);

        cat_delay_ms(1000);
    }

}

void hmc_get_all_data(cat_float_t *x, cat_float_t *z, cat_float_t *y)
{
    cat_int32_t xr=0, yr=0, zr=0;

#if 0
    tmp = HMC_READ_IIC(HMC5883L_REG_DX_MSB);
    xr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DX_LSB);
    xr = (xr << 8) | tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DZ_MSB);
    zr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DZ_LSB);
    zr = (zr << 8) | tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DY_MSB);
    yr = tmp;
    tmp = HMC_READ_IIC(HMC5883L_REG_DY_LSB);
    yr = (yr << 8) | tmp;

    if(xr > 0x7fff) xr -= 0xffff;
    if(yr > 0x7fff) yr -= 0xffff;
#endif
    _get_raw_data(&xr, &zr, &yr);

    /* 要除以增益 */
    *x = (cat_float_t)xr / 1090;
    *z = (cat_float_t)zr / 1090;
    *y = (cat_float_t)yr / 1090;
}
