/**
 * @file drv_mpu6050.c
 * @author 文佳源 (648137125@qq.com)
 * @brief mpu6050驱动
 * @version 0.1
 * @date 2023-08-10
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-10 <td>创建
 * </table>
 */
#include "drv_mpu6050.h"
#include "drv_soft_iic.h"
#include "cat_delay.h"
#include "cat_stdio.h"
#include "cat_error.h"

#define MPU_USE_EMPL_LIB    (1)

#include "inv_mpu.h"//陀螺仪驱动库
#include "inv_mpu_dmp_motion_driver.h" //DMP姿态解读库

/* 6050内部地址 */
#define MPU_REG_SELF_TESTX		0X0D	//自检寄存器X
#define MPU_REG_SELF_TESTY		0X0E	//自检寄存器Y
#define MPU_REG_SELF_TESTZ		0X0F	//自检寄存器Z
#define MPU_REG_SELF_TESTA		0X10	//自检寄存器A
#define MPU_REG_SAMPLE_RATE		0X19	//采样频率分频器
#define MPU_REG_CFG				0X1A	//配置寄存器
#define MPU_REG_GYRO_CFG		0X1B	//陀螺仪配置寄存器
#define MPU_REG_ACCEL_CFG		0X1C	//加速度计配置寄存器
#define MPU_REG_MOTION_DET		0X1F	//运动检测阀值设置寄存器
#define MPU_REG_FIFO_EN			0X23	//FIFO使能寄存器
#define MPU_REG_I2CMST_CTRL		0X24	//IIC主机控制寄存器
#define MPU_REG_I2CSLV0_ADDR	0X25	//IIC从机0器件地址寄存器
#define MPU_REG_I2CSLV0			0X26	//IIC从机0数据地址寄存器
#define MPU_REG_I2CSLV0_CTRL	0X27	//IIC从机0控制寄存器
#define MPU_REG_I2CSLV1_ADDR	0X28	//IIC从机1器件地址寄存器
#define MPU_REG_I2CSLV1			0X29	//IIC从机1数据地址寄存器
#define MPU_REG_I2CSLV1_CTRL	0X2A	//IIC从机1控制寄存器
#define MPU_REG_I2CSLV2_ADDR	0X2B	//IIC从机2器件地址寄存器
#define MPU_REG_I2CSLV2			0X2C	//IIC从机2数据地址寄存器
#define MPU_REG_I2CSLV2_CTRL	0X2D	//IIC从机2控制寄存器
#define MPU_REG_I2CSLV3_ADDR	0X2E	//IIC从机3器件地址寄存器
#define MPU_REG_I2CSLV3			0X2F	//IIC从机3数据地址寄存器
#define MPU_REG_I2CSLV3_CTRL	0X30	//IIC从机3控制寄存器
#define MPU_REG_I2CSLV4_ADDR	0X31	//IIC从机4器件地址寄存器
#define MPU_REG_I2CSLV4			0X32	//IIC从机4数据地址寄存器
#define MPU_REG_I2CSLV4_DO		0X33	//IIC从机4写数据寄存器
#define MPU_REG_I2CSLV4_CTRL	0X34	//IIC从机4控制寄存器
#define MPU_REG_I2CSLV4_DI		0X35	//IIC从机4读数据寄存器

#define MPU_REG_I2CMST_STA		0X36	//IIC主机状态寄存器
#define MPU_REG_INTBP_CFG		0X37	//中断/旁路设置寄存器
#define MPU_REG_INT_EN			0X38	//中断使能寄存器
#define MPU_REG_INT_STA			0X3A	//中断状态寄存器

#define MPU_REG_ACCEL_XOUTH		0X3B	//加速度值,X轴高8位寄存器
#define MPU_REG_ACCEL_XOUTL		0X3C	//加速度值,X轴低8位寄存器
#define MPU_REG_ACCEL_YOUTH		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_REG_ACCEL_YOUTL		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_REG_ACCEL_ZOUTH		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_REG_ACCEL_ZOUTL		0X40	//加速度值,Z轴低8位寄存器

#define MPU_REG_TEMP_OUTH		0X41	//温度值高八位寄存器
#define MPU_REG_TEMP_OUTL		0X42	//温度值低8位寄存器

#define MPU_REG_GYRO_XOUTH		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_REG_GYRO_XOUTL		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_REG_GYRO_YOUTH		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_REG_GYRO_YOUTL		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_REG_GYRO_ZOUTH		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_REG_GYRO_ZOUTL		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_REG_I2CSLV0_DO		0X63	//IIC从机0数据寄存器
#define MPU_REG_I2CSLV1_DO		0X64	//IIC从机1数据寄存器
#define MPU_REG_I2CSLV2_DO		0X65	//IIC从机2数据寄存器
#define MPU_REG_I2CSLV3_DO		0X66	//IIC从机3数据寄存器

#define MPU_REG_I2CMST_DELAY	0X67	//IIC主机延时管理寄存器
#define MPU_REG_SIGPATH_RST		0X68	//信号通道复位寄存器
#define MPU_REG_MDETECT_CTRL	0X69	//运动检测控制寄存器
#define MPU_REG_USER_CTRL		0X6A	//用户控制寄存器
#define MPU_REG_PWR_MGMT1		0X6B	//电源管理寄存器1
#define MPU_REG_PWR_MGMT2		0X6C	//电源管理寄存器2 
#define MPU_REG_FIFO_CNTH		0X72	//FIFO计数寄存器高八位
#define MPU_REG_FIFO_CNTL		0X73	//FIFO计数寄存器低八位
#define MPU_REG_FIFO_RW			0X74	//FIFO读写寄存器
#define	MPU_REG_WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	        0xD0	//IIC写入时的地址字节数据，+1为读取

static cat_iic_bus_t *_mpu6050_iic_bus_ptr = CAT_NULL;
#define MPU6050_BUS (_mpu6050_iic_bus_ptr)

#define MPU_WRITE_REG(reg_addr, data) \
    cat_iic_write_reg(MPU6050_BUS, SlaveAddress, reg_addr, data)

#define MPU_READ_REG(reg_addr) \
    cat_iic_read_reg(MPU6050_BUS, SlaveAddress, reg_addr)

static void _cat_mpu_set_gyro_fsr(cat_uint8_t fsr);      /* 设置陀螺仪量程 */
static void _cat_mpu_set_accel_fsr(cat_uint8_t fsr);     /* 设置加速度计;量程 */
static void _cat_mpu_set_lpf(cat_uint16_t lpf);          /* 设置数字低通滤波器 */
static void _cat_mpu_set_sample_rate(cat_uint16_t hz);   /* 设置采样频率 */


void cat_mpu6050_init(cat_iic_bus_t *bus)
{
    cat_uint8_t ret = 0;

    CAT_ASSERT(CAT_NULL != bus);
    _mpu6050_iic_bus_ptr = bus;

    cat_iic_init(MPU6050_BUS); /* 初始化总线 */

    cat_delay_ms(100);
    cat_iic_wave(MPU6050_BUS);

    MPU_WRITE_REG(MPU_REG_PWR_MGMT1, 0x80);     /* 复位mpu */
    cat_delay_ms(100);
    MPU_WRITE_REG(MPU_REG_PWR_MGMT1, 0x00);     /* 唤醒, 解除休眠 */

    _cat_mpu_set_gyro_fsr(3);                        /* 设置陀螺仪满量程 +-2000dps */
    _cat_mpu_set_accel_fsr(0);                       /* 设置加速度传感器满量程 +-2g */
    _cat_mpu_set_sample_rate(50);                    /* 设置采样频率 50 Hz */

    MPU_WRITE_REG(MPU_REG_INT_EN, 0x00);        /* 关闭所有中断 */
    MPU_WRITE_REG(MPU_REG_USER_CTRL, 0x00);     /* iic 主模式(master)关闭 */
    MPU_WRITE_REG(MPU_REG_FIFO_EN, 0x00);       /* 关闭 FIFO */
    MPU_WRITE_REG(MPU_REG_INTBP_CFG, 0x80);     /* INT 引脚低电平有效 */

    ret = MPU_READ_REG(MPU_REG_WHO_AM_I);
    if(0x68 == ret)
    {
        MPU_WRITE_REG(MPU_REG_PWR_MGMT1, 0x01); /* 设置 CLK_SET, PLL X轴为参考 */
        MPU_WRITE_REG(MPU_REG_PWR_MGMT2, 0x00); /* 加速度计与陀螺仪都工作 */
        _cat_mpu_set_sample_rate(50);                /* 设置采样率 50 Hz */
        CAT_KPRINTF("[mpu6050] init success, reg WHO_AM_I=%x\r\n", ret);
    }
    else
    {
        CAT_PRINT_ERROR("[mpu6050] ERROR: WHO_AM_I not match n");
    }

#if(MPU_USE_EMPL_LIB == 1)
    ret = mpu_dmp_init();

    if(0 == ret)
    {
        CAT_KPRINTF("{mpu6050} dmp init success\r\n", ret);
    }
    else
    {
        CAT_FALTAL_ERROR("[mpu6050] ERROR: fail to init DMP\r\n");
    }
#endif
}

cat_int32_t cat_mpu6050_get_data(cat_uint8_t reg_addr)
{
    cat_uint32_t data;
    cat_uint8_t data_h, data_l;

    data_h = MPU_READ_REG(reg_addr);
    data_l = MPU_READ_REG(reg_addr+1);

    data = ((data_h << 8) + data_l);

    if((data & 0x00008000) == 0x00008000)
    {
        data |= 0xffff0000;
    }

    return (cat_int32_t)data;
}

cat_err_t cat_mpu6050_get_angle(cat_float_t *pitch_ptr, cat_float_t *roll_ptr, cat_float_t *yaw_ptr)
{   
    cat_err_t ret = CAT_ERROR;

#if(MPU_USE_EMPL_LIB == 1)
    cat_uint8_t dmp_ret = mpu_dmp_get_data(pitch_ptr, roll_ptr, yaw_ptr);

    if(0 == dmp_ret)
    {
        ret = CAT_EOK;
    }
#else

#endif

    return ret;
}

/************ PRIVATE FUNCS ***********/
static void _cat_mpu_set_gyro_fsr(cat_uint8_t fsr)
{
    MPU_WRITE_REG(MPU_REG_GYRO_CFG, fsr << 3);
}
static void _cat_mpu_set_accel_fsr(cat_uint8_t fsr)
{
    MPU_WRITE_REG(MPU_REG_ACCEL_CFG, fsr << 3);
}
static void _cat_mpu_set_lpf(cat_uint16_t lpf)
{
    cat_uint8_t data = 0;

    if(lpf >= 188)      data = 1;
    else if(lpf >= 98)  data = 2;
    else if(lpf >= 42)  data = 3;
    else if(lpf >= 20)  data = 4;
    else if(lpf >= 10)  data = 5;
    else                data = 6;

    MPU_WRITE_REG(MPU_REG_CFG, data);
}

/**
 * @brief 
 * 
 * @param  hz               4~1000 Hz
 */
static void _cat_mpu_set_sample_rate(cat_uint16_t hz)
{
    cat_uint8_t data;

    if(hz > 1000) hz = 1000;
    if(hz < 4)    hz = 4;

    data = 1000/hz - 1;

    MPU_WRITE_REG(MPU_REG_SAMPLE_RATE, data);

    _cat_mpu_set_lpf(hz / 2);
}