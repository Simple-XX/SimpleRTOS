#ifndef __MPU6050__H__
#define __MPU6050__H__


#include "catos_config.h"
#include "catos_types.h"

#include "cat_pin.h"

//! I2C3   PB2->SCL  PB3->SDA


// 定义MPU6050内部地址
#define	MPU6050_DEV_ADDR	    0x68	//IIC写入时的地址字节数据，+1为读取

#define	SMPLRT_DIV				0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	MPU6050_CONFIG          0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG				0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	        0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define INT_PIN_CFG	  			0x37    //设置6050辅助I2C为直通模式寄存器
#define	ACCEL_XOUT_H	        0x3B
#define	ACCEL_XOUT_L	        0x3C
#define	ACCEL_YOUT_H	        0x3D
#define	ACCEL_YOUT_L	        0x3E
#define	ACCEL_ZOUT_H	        0x3F
#define	ACCEL_ZOUT_L	        0x40
#define	GYRO_XOUT_H				0x43
#define	GYRO_XOUT_L				0x44	
#define	GYRO_YOUT_H				0x45
#define	GYRO_YOUT_L				0x46
#define	GYRO_ZOUT_H				0x47
#define	GYRO_ZOUT_L				0x48
#define User_Control 			0x6A    //关闭6050对辅助I2C设备的控制
#define	PWR_MGMT_1				0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I				0x75	//IIC地址寄存器(默认数值0x68，只读)

extern cat_int16_t mpu6050_gyro_x,mpu6050_gyro_y,mpu6050_gyro_z;
extern cat_int16_t mpu6050_acc_x,mpu6050_acc_y,mpu6050_acc_z;

#define PI 3.1415926f
//=============================================姿态解算================
typedef struct
{
    signed short x;
    signed short y;
    signed short z;
}S16_XYZ;

typedef struct
{
    float x;
    float y;
    float z;
}SI_F_XYZ;

typedef struct
{
    SI_F_XYZ deg_s;      //度每秒
    SI_F_XYZ acc_g;      //加速度

    float att_acc_factor;
    float att_gyro_factor;
    float roll;
    float pitch;
    float yaw;
}imu660_data;

typedef struct
{
    SI_F_XYZ gyro;      //角速度
    cat_int8_t offset_flag;
}imu660_offset;


typedef struct
{
    float ax;
    float ay;
    float az;

    float gx;
    float gy;
    float gz;

    float pit;
    float rol;
    float yaw;
}_Attitude;


typedef struct
{
    float DCM[3][3];        //机体坐标系 -> 地理坐标系
    float DCM_T[3][3];      //地理坐标系 -> 机体坐标系
}_Matrix;
void  IIR_imu                     (void);                                         //获得IIR低通滤波参数
void  IMU                         (void);                                         //解算

extern _Matrix Mat;
extern _Attitude att;

extern imu660_offset set;
extern imu660_data imu;    //陀螺仪数据存储
//=============================================姿态解算================
cat_bool_t  mpu6050_init(void);               //初始化MPU6050
void   mpu6050_get_accdata(void);
void   mpu6050_get_gyro(void);
void IMU(void);

#define imu_Read_Time   1.0f//原始数据采集时间间隔 秒为单位 0.01秒 10ms
#define MahonyPERIOD    1000.0f//姿态解算周期（ms）


//====DMP
/*
uint8_t mpu6050_DMP_Write(uint8_t dev_add, uint8_t reg, uint8_t len, uint8_t *dat);
uint8_t mpu6050_DMP_Read(uint8_t dev_add, uint8_t reg,uint8_t num, uint8_t *dat_add );
*/

#endif  //!__MPU6050__H__
