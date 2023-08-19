#include "mpu6050.h"
#include "math.h"

#include "drv_soft_iic.h"
#include "cat_delay.h"
#include "cat_stdio.h"

#define MPU6050_BUS (&soft_iic_2)

#define MPU_WRITE_REG(reg_addr, data) \
    cat_iic_write_reg(MPU6050_BUS, SlaveAddress, reg_addr, data)

#define MPU_READ_REG(reg_addr) \
    cat_iic_read_reg(MPU6050_BUS, SlaveAddress, reg_addr)

#if 0
    #define SDA_Port  GPIO_PORTB_BASE
    #define SDA_Pin   GPIO_PIN_7
    #define SDA_Clock SYSCTL_PERIPH_GPIOB  

    #define SCL_Port  GPIO_PORTB_BASE
    #define SCL_Pin   GPIO_PIN_6
    #define SCL_Clock SYSCTL_PERIPH_GPIOB

    #define SDA_In()  GPIODirModeSet(SDA_Port,SDA_Pin,GPIO_DIR_MODE_IN);
    #define SDA_Out()  GPIODirModeSet(SDA_Port,SDA_Pin,GPIO_DIR_MODE_OUT);

    #define MPU6050_SDA_HIGH()  GPIOPinWrite(SDA_Port,SDA_Pin,SDA_Pin)
    #define MPU6050_SDA_LOW()  GPIOPinWrite(SDA_Port,SDA_Pin,0);
    #define MPU6050_SCL_HIGH()  GPIOPinWrite(SCL_Port,SCL_Pin,SCL_Pin)
    #define MPU6050_SCL_LOW()  GPIOPinWrite(SCL_Port,SCL_Pin,0);

    #define mpu6050_simiic_delay()  DELAY_US(2)

    #define GET_MPU6050_SDA     ((GPIOPinRead(SDA_Port,SDA_Pin)!=0)?(1):(0))

    #define no_ack   0
    #define ack      1
#else
#define SDA_In()   SDA_IN(MPU6050_BUS)
#define SDA_Out()  SDA_OUT(MPU6050_BUS);

#define MPU6050_SDA_HIGH()  SDA_H(MPU6050_BUS)
#define MPU6050_SDA_LOW()   SDA_L(MPU6050_BUS)
#define MPU6050_SCL_HIGH()  SCL_H(MPU6050_BUS)
#define MPU6050_SCL_LOW()   SCL_L(MPU6050_BUS)

#define DELAY_MS(ms)            cat_delay_ms(ms)
#define DELAY_US(us)            cat_delay_us(us)
#define mpu6050_simiic_delay()  cat_delay_us(2)


#define GET_MPU6050_SDA     GET_SDA(MPU6050_BUS)

#define no_ack   0
#define ack      1
#endif

cat_int16_t mpu6050_gyro_x=0,mpu6050_gyro_y=0,mpu6050_gyro_z=0;
cat_int16_t mpu6050_acc_x=0,mpu6050_acc_y=0,mpu6050_acc_z=0;

//=========================姿态解算==============================
_Matrix Mat = {0};
_Attitude att = {0};
imu660_offset set = {0};
float Atmpe_Y,Atmpe_X;
imu660_data imu = {0} ;
#define kp      0.5f                        //proportional gain governs rate of convergence to accelerometer/magnetometer
#define ki      0.0001f                     //integral gain governs rate of convergenceof gyroscope biases
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;       //quaternion elements representing theestimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;      //scaled integral error
cat_int16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;                           // 三轴陀螺仪滤波数据      gyro (陀螺仪)
cat_int16_t acc_x = 0, acc_y = 0, acc_z = 0;                              // 三轴加速度计滤波数据     acc (accelerometer 加速度计)

/*
 * 函数名：get_iir_factor
 * 描述  ：求取IIR滤波器的滤波因子
 * 输入  ：out_factor滤波因子首地址，Time任务执行周期，Cut_Off滤波截止频率
 * 返回  ：
 */
void get_iir_factor(float *out_factor,float Time, float Cut_Off)
{
    *out_factor = Time /( Time + 1/(2.0f * PI * Cut_Off) );
}
/* 获取IIR低通滤波 */
void IIR_imu(void)
{
	cat_int8_t i;
    for(i=0;i<=100;i++)
    {
        mpu6050_get_gyro();
        set.gyro.x+=mpu6050_gyro_x;
        set.gyro.y+=mpu6050_gyro_y;
        set.gyro.z+=mpu6050_gyro_z;
    }
    set.gyro.x/= 100;
    set.gyro.y/= 100;
    set.gyro.z/= 100;
    set.offset_flag = 1;
    //printf("%d\n",set.offset_flag);
    get_iir_factor(&imu.att_acc_factor,imu_Read_Time,15);
    get_iir_factor(&imu.att_gyro_factor,imu_Read_Time,10);
}
/**
  * @brief   IIR低通滤波器
  * @param   *acc_in 输入三轴数据指针变量
  * @param   *acc_out 输出三轴数据指针变量
  * @param   lpf_factor 滤波因数
  * @retval  x
  */
float iir_lpf(float in,float out,float lpf_factor)
{
    out = out + lpf_factor * (in - out);
    return out;
}
//============================================================================四元数==============================================

//内部使用，用户无需调用
static void mpu6050_simiic_start(void)
{
#if 0
    SDA_Out();
	MPU6050_SDA_HIGH();
	MPU6050_SCL_HIGH();
	DELAY_US(4);
	MPU6050_SDA_LOW();
	DELAY_US(4);
	MPU6050_SCL_LOW();
#else
    cat_iic_start(MPU6050_BUS);
#endif
}

//内部使用，用户无需调用
static void mpu6050_simiic_stop(void)
{
#if 0
    SDA_Out();
    MPU6050_SCL_LOW();
    MPU6050_SDA_LOW();
    DELAY_US(4);
	MPU6050_SCL_HIGH();
	MPU6050_SDA_HIGH();
	DELAY_US(4);
#else
    cat_iic_stop(MPU6050_BUS);
#endif
}

//主应答(包含ack:SDA=1和no_ack:SDA=0)
//内部使用，用户无需调用
static void mpu6050_simiic_sendack(unsigned char ack_dat)
{
	if(ack_dat)
    {
#if 0
        MPU6050_SCL_LOW();
        SDA_Out();
        MPU6050_SDA_LOW();
        DELAY_US(2);
        MPU6050_SCL_HIGH();
        DELAY_US(2);
        MPU6050_SCL_LOW();
#else
        cat_iic_send_ack(MPU6050_BUS);
#endif
    } 
    else    	
    {
#if 0
        MPU6050_SCL_LOW();
        SDA_Out();
        MPU6050_SDA_HIGH();
        DELAY_US(2);
        MPU6050_SCL_HIGH();
        DELAY_US(2);
        MPU6050_SCL_LOW();
#else
        cat_iic_send_nack(MPU6050_BUS);
#endif
    }
}


static cat_bool_t mpu6050_sccb_waitack(void)
{
#if 0
    cat_uint8_t waittime = 0;

    SDA_In();

    MPU6050_SDA_HIGH();
    DELAY_US(1);
    MPU6050_SCL_HIGH();
    DELAY_US(1);

    while(GET_MPU6050_SDA)           //应答为高电平，异常，通信失败
    {
        waittime++;
        if(waittime > 250)
        {
            mpu6050_simiic_stop();
            return 1;
        }
    }
    MPU6050_SCL_LOW();
    return 0;
#else
    cat_bool_t ret;

    cat_iic_wait_ack(MPU6050_BUS);

    return ret;
#endif
}

//字节发送程序
//发送c(可以是数据也可是地址)，送完后接收从应答
//不考虑从应答位
//内部使用，用户无需调用
static void mpu6050_send_ch(cat_uint8_t c)
{
#if 0
	cat_uint8_t i = 0;
    SDA_Out();
    MPU6050_SCL_LOW();//拉低时钟开始数据传输

    for(i=0;i<8;i++)
    {
        if((( c >> 7) & 0x01) == 0x01) 
        {
            MPU6050_SDA_HIGH();//SDA 输出数据
        }
        else			
        {
            MPU6050_SDA_LOW();
        }
        c <<= 1;
        DELAY_US(2);
        MPU6050_SCL_HIGH();                //SCL 拉高，采集信号
        DELAY_US(2);
        MPU6050_SCL_LOW();                //SCL 时钟线拉低
        DELAY_US(2);
    }
	//mpu6050_sccb_waitack();
#else
    cat_iic_send_byte(MPU6050_BUS, c);
#endif
}


//字节接收程序
//接收器件传来的数据，此程序应配合|主应答函数|使用
//内部使用，用户无需调用
static cat_uint8_t mpu6050_read_ch(cat_uint8_t ack_x)
{
#if 0
    cat_uint8_t i;
    cat_uint8_t c = 0;
    SDA_In();   
    
    //MPU6050_SCL_LOW();         //置时钟线为低，准备接收数据位
    //DELAY_US(2);
    //MPU6050_SCL_HIGH();         //置时钟线为高，使数据线上数据有效     

    for(i=0;i<8;i++)
    {
        MPU6050_SCL_LOW();         //置时钟线为低，准备接收数据位
        DELAY_US(2);
        MPU6050_SCL_HIGH();         //置时钟线为高，使数据线上数据有效
        c <<= 1;

        if(GET_MPU6050_SDA) 
        {
            c++;   //读数据位，将接收的数据存c
        }
        DELAY_US(2);
    }
    //MPU6050_SCL_LOW();         //置时钟线为低，准备接收数据位
    //DELAY_US(2);
	mpu6050_simiic_sendack(ack_x);
    return c;
#else
    cat_uint8_t c = cat_iic_read_byte(MPU6050_BUS);
    return c;
#endif
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC写数据到设备寄存器函数
//  @param      dev_add			设备地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat				写入的数据
//  @return     void						
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static void mpu6050_simiic_write_reg(cat_uint8_t dev_add, cat_uint8_t reg, cat_uint8_t dat)
{
    //writeByteI2C(&soft_i2c,(dev_add<<1),reg,dat);
    
#if 0
	mpu6050_simiic_start();
    mpu6050_send_ch( (dev_add<<1) | 0x00);   //发送器件地址加写位
    mpu6050_sccb_waitack();
	mpu6050_send_ch( reg );   				 //发送从机寄存器地址
    mpu6050_sccb_waitack();
	mpu6050_send_ch( dat );   				 //发送需要写入的数据
    mpu6050_sccb_waitack();
	mpu6050_simiic_stop();
#else
    cat_uint8_t addr_shift = ((dev_add << 1) | 0x00);
    cat_iic_write_reg(MPU6050_BUS, addr_shift, reg, dat);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC从设备寄存器读取数据
//  @param      dev_add			设备地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      type			选择通信方式是IIC  还是 SCCB
//  @return     uint8			返回寄存器的数据			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
cat_uint8_t mpu6050_simiic_read_reg(cat_uint8_t dev_add, cat_uint8_t reg)
{
	cat_uint8_t data;

#if 0
    //readByteI2C(&soft_i2c,dev_add,reg,&data);
	mpu6050_simiic_start();
    mpu6050_send_ch( (dev_add<<1) | 0x00);  //发送器件地址加写位
    mpu6050_sccb_waitack();
	mpu6050_send_ch( reg );   				//发送从机寄存器地址
	mpu6050_sccb_waitack();
    mpu6050_simiic_stop();

	mpu6050_simiic_start();
	mpu6050_send_ch( (dev_add<<1) | 0x01);  //发送器件地址加读位
    mpu6050_sccb_waitack();
	data = mpu6050_read_ch(no_ack);   				//读取数据
	mpu6050_simiic_stop();
#else
    cat_uint8_t dev_shift = ((dev_add << 1) | 0x00);
    data = cat_iic_read_reg(MPU6050_BUS, dev_shift, reg);
#endif
	return data;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC读取多字节数据
//  @param      dev_add			设备地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat_add			数据保存的地址指针
//  @param      num				读取字节数量
//  @param      type			选择通信方式是IIC  还是 SCCB
//  @return     uint8			返回寄存器的数据			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_simiic_read_regs(cat_uint8_t dev_add, cat_uint8_t reg, cat_uint8_t *dat_add, cat_uint8_t num)
{
	mpu6050_simiic_start();
    mpu6050_send_ch( (dev_add<<1) | 0x00);  //发送器件地址加写位
    mpu6050_sccb_waitack();
	mpu6050_send_ch( reg );   				//发送从机寄存器地址
    mpu6050_sccb_waitack();
    //mpu6050_simiic_stop();
	
	mpu6050_simiic_start();
	mpu6050_send_ch( (dev_add<<1) | 0x01);  //发送器件地址加读位
    mpu6050_sccb_waitack();
    while(--num)
    {
        *dat_add = mpu6050_read_ch(ack); //读取数据
        dat_add++;
    }
    *dat_add = mpu6050_read_ch(no_ack); //读取数据
	mpu6050_simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MPU6050自检函数
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static cat_bool_t mpu6050_self1_check(void)
{
    CAT_SYS_PRINTF("ID:%x\n",mpu6050_simiic_read_reg(MPU6050_DEV_ADDR, WHO_AM_I));
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00);	//解除休眠状态
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07);   //125HZ采样率
    if(0x07 != mpu6050_simiic_read_reg(MPU6050_DEV_ADDR, SMPLRT_DIV))
    {
		CAT_SYS_PRINTF("mpu6050 init error. %d\r\n",mpu6050_simiic_read_reg(MPU6050_DEV_ADDR, SMPLRT_DIV));
		return CAT_FALSE;
        //卡在这里原因有以下几点
        //1 MPU6050坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
		//4 可能没有调用模拟IIC的初始化函数
    }
	return CAT_TRUE;
}

cat_bool_t mpu6050_init(void)
{
    DELAY_MS(800);

#if 0
    SysCtlPeripheralEnable(SDA_Clock);
    //2ma，上拉
    GPIOPadConfigSet(SDA_Port,SDA_Pin,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    //输出
    GPIODirModeSet(SDA_Port,SDA_Pin,GPIO_DIR_MODE_OUT);

    SysCtlPeripheralEnable(SCL_Clock);
    //2ma，开漏
    GPIOPadConfigSet(SCL_Port,SCL_Pin,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);
    //输出
    GPIODirModeSet(SCL_Port,SCL_Pin,GPIO_DIR_MODE_OUT);
#else
    cat_iic_init(MPU6050_BUS);
#endif

    if(mpu6050_self1_check() == CAT_FALSE)
	{
        CAT_SYS_PRINTF("[mpu6050] ERROR: self check faild!!\r\n");
		return CAT_FALSE;
	}
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00);	//解除休眠状态
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07);   //125HZ采样率
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, MPU6050_CONFIG, 0x04);       //
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, GYRO_CONFIG, 0x18);  //2000
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, ACCEL_CONFIG, 0x10); //8g
	mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, User_Control, 0x00);
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, INT_PIN_CFG, 0x02);
    mpu6050_gyro_x=0;
    mpu6050_gyro_y=0;
    mpu6050_gyro_z=0;

    /*if(MPU6050_DMP_Init())
    {
        printf("FALSE\n");
    }*/

    //开启滤波参数
    IIR_imu();
    return CAT_TRUE;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取MPU6050加速度计数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_accdata(void)
{
    cat_uint8_t dat[6];

    mpu6050_simiic_read_regs(MPU6050_DEV_ADDR, ACCEL_XOUT_H, dat, 6);

    mpu6050_acc_x = (cat_int16_t)(((cat_uint16_t)dat[0]<<8 | dat[1]));
    mpu6050_acc_y = (cat_int16_t)(((cat_uint16_t)dat[2]<<8 | dat[3]));
    mpu6050_acc_z = (cat_int16_t)(((cat_uint16_t)dat[4]<<8 | dat[5]));
    //printf("%d,%d,%d\n",mpu6050_acc_x,mpu6050_acc_y,mpu6050_acc_z);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取MPU6050陀螺仪数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_gyro(void)
{

    //所有
    cat_uint8_t dat[6];
    mpu6050_simiic_read_regs(MPU6050_DEV_ADDR, GYRO_XOUT_H, dat, 6);
    mpu6050_gyro_x = (cat_int16_t)(((cat_uint16_t)dat[0]<<8 | dat[1]));
    mpu6050_gyro_y = (cat_int16_t)(((cat_uint16_t)dat[2]<<8 | dat[3]));
    mpu6050_gyro_z = (cat_int16_t)(((cat_uint16_t)dat[4]<<8 | dat[5]));

    /*if(mpu6050_gyro_x < 200 && mpu6050_gyro_x > -200)
        mpu6050_gyro_x = 0;
    if(mpu6050_gyro_y < 200 && mpu6050_gyro_y > -200)
        mpu6050_gyro_y = 0;
    if(mpu6050_gyro_z < 200 && mpu6050_gyro_z > -200)
        mpu6050_gyro_z = 0;*/
    //printf("%d,%d,%d\n",mpu6050_gyro_x,mpu6050_gyro_y,mpu6050_gyro_z);
}

// Fast inverse square-root
float invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/*
 * 函数名：mahony_update
 * 描述  ：姿态解算
 * 输入  ：陀螺仪三轴数据（单位：弧度/秒），加速度三轴数据（单位：g）
 * 返回  ：
 */
//Gyroscope units are radians/second, accelerometer  units are irrelevant as the vector is normalised.
void mahony_update(float gx, float gy, float gz, float ax, float ay, float az)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    if(ax*ay*az==0)
        return;
    gx=gx*(PI / 180.0f);
    gy=gy*(PI / 180.0f);
    gz=gz*(PI / 180.0f);
    //[ax,ay,az]是机体坐标系下加速度计测得的重力向量(竖直向下)
    norm = invSqrt(ax*ax + ay*ay + az*az);
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

    //VectorA = MatrixC * VectorB
    //VectorA ：参考重力向量转到在机体下的值
    //MatrixC ：地理坐标系转机体坐标系的旋转矩阵
    //VectorB ：参考重力向量（0,0,1）
    //[vx,vy,vz]是地理坐标系重力分向量[0,0,1]经过DCM旋转矩阵(C(n->b))计算得到的机体坐标系中的重力向量(竖直向下)

    vx = Mat.DCM_T[0][2];
    vy = Mat.DCM_T[1][2];
    vz = Mat.DCM_T[2][2];

    //机体坐标系下向量叉乘得到误差向量，误差e就是测量得到的vˉ和预测得到的 v^之间的相对旋转。这里的vˉ就是[ax,ay,az]’,v^就是[vx,vy,vz]’
    //利用这个误差来修正DCM方向余弦矩阵(修正DCM矩阵中的四元素)，这个矩阵的作用就是将b系和n正确的转化直到重合。
    //实际上这种修正方法只把b系和n系的XOY平面重合起来，对于z轴旋转的偏航，加速度计无可奈何，
    //但是，由于加速度计无法感知z轴上的旋转运动，所以还需要用地磁计来进一步补偿。
    //两个向量的叉积得到的结果是两个向量的模与他们之间夹角正弦的乘积a×v=|a||v|sinθ,
    //加速度计测量得到的重力向量和预测得到的机体重力向量已经经过单位化，因而他们的模是1，
    //也就是说它们向量的叉积结果仅与sinθ有关，当角度很小时，叉积结果可以近似于角度成正比。

    ex = ay*vz - az*vy;
    ey = az*vx - ax*vz;
    ez = ax*vy - ay*vx;

    //对误差向量进行积分
    exInt = exInt + ex*ki;
    eyInt = eyInt + ey*ki;
    ezInt = ezInt + ez*ki;

    //姿态误差补偿到角速度上，修正角速度积分漂移，通过调节Kp、Ki两个参数，可以控制加速度计修正陀螺仪积分姿态的速度。
    gx = gx + kp*ex + exInt;
    gy = gy + kp*ey + eyInt;
    gz = gz + kp*ez + ezInt;

    //一阶龙格库塔法更新四元数
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)* MahonyPERIOD * 0.0005f;
    q1 = q1 + ( q0*gx + q2*gz - q3*gy)* MahonyPERIOD * 0.0005f;
    q2 = q2 + ( q0*gy - q1*gz + q3*gx)* MahonyPERIOD * 0.0005f;
    q3 = q3 + ( q0*gz + q1*gy - q2*gx)* MahonyPERIOD * 0.0005f;

    //把上述运算后的四元数进行归一化处理。得到了物体经过旋转后的新的四元数。
    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 = q0 * norm;
    q1 = q1 * norm;
    q2 = q2 * norm;
    q3 = q3 * norm;
    //printf("%f,%f,%f,%f\n",q0,q1,q2,q3);

    static float pitch_old=0, roll_old=0,yaw_old=0;
    float temp = 0;
    //四元素转欧拉角
    imu.pitch =   atan2f(2.0f*(q0*q1 + q2*q3),q0*q0 - q1*q1 - q2*q2 + q3*q3) * (180.0f / PI);
    imu.roll  =   -asinf(2.0f*(q0*q2 - q1*q3)) * (180.0f / PI);
    //z轴角速度积分的偏航角
    imu.yaw += imu.deg_s.z  * MahonyPERIOD * 0.001f;//存疑？也可能是imu_Read_Time。具体自己换着看下效果

    temp = imu.pitch-pitch_old;
    if(temp<2 && temp > -2)
    {
        imu.pitch = pitch_old;
    }
    else
    {
        pitch_old = imu.pitch;
    }
    //temp = imu.roll-roll_old;
    /*if(temp<2 && temp > -2)
    {
        imu.roll = roll_old;
    }
    else
    {
        roll_old = imu.roll;
    }*/
    temp = imu.yaw-yaw_old;
    if(temp<0.1f && temp > -0.1f)
    {
        imu.yaw = yaw_old;
    }
    else
    {
        yaw_old = imu.yaw;
    }
//    if(imu.yaw>180) imu.yaw-=360;
//    if(imu.yaw<-180) imu.yaw+=360;
}
/*
 * 函数名：rotation_matrix
 * 描述  ：旋转矩阵：机体坐标系 -> 地理坐标系
 * 输入  ：
 * 返回  ：
 */
void rotation_matrix(void)
{
    Mat.DCM[0][0] = 1.0f - 2.0f * q2*q2 - 2.0f * q3*q3;
    Mat.DCM[0][1] = 2.0f * (q1*q2 -q0*q3);
    Mat.DCM[0][2] = 2.0f * (q1*q3 +q0*q2);

    Mat.DCM[1][0] = 2.0f * (q1*q2 +q0*q3);
    Mat.DCM[1][1] = 1.0f - 2.0f * q1*q1 - 2.0f * q3*q3;
    Mat.DCM[1][2] = 2.0f * (q2*q3 -q0*q1);

    Mat.DCM[2][0] = 2.0f * (q1*q3 -q0*q2);
    Mat.DCM[2][1] = 2.0f * (q2*q3 +q0*q1);
    Mat.DCM[2][2] = 1.0f - 2.0f * q1*q1 - 2.0f * q2*q2;
}
/*
 * 函数名：rotation_matrix_T
 * 描述  ：旋转矩阵的转置矩阵：地理坐标系 -> 机体坐标系
 * 输入  ：
 * 返回  ：
 */
void rotation_matrix_T(void)
{
    Mat.DCM_T[0][0] = 1.0f - 2.0f * q2*q2 - 2.0f * q3*q3;
    Mat.DCM_T[0][1] = 2.0f * (q1*q2 +q0*q3);
    Mat.DCM_T[0][2] = 2.0f * (q1*q3 -q0*q2);

    Mat.DCM_T[1][0] = 2.0f * (q1*q2 -q0*q3);
    Mat.DCM_T[1][1] = 1.0f - 2.0f * q1*q1 - 2.0f * q3*q3;
    Mat.DCM_T[1][2] = 2.0f * (q2*q3 +q0*q1);

    Mat.DCM_T[2][0] = 2.0f * (q1*q3 +q0*q2);
    Mat.DCM_T[2][1] = 2.0f * (q2*q3 -q0*q1);
    Mat.DCM_T[2][2] = 1.0f - 2.0f * q1*q1 - 2.0f * q2*q2;
}
/*
 * 函数名：Matrix_ready
 * 描述  ：矩阵更新准备，为姿态解算使用
 * 输入  ：
 * 返回  ：
 */
void Matrix_ready(void)
{
    rotation_matrix();                      //旋转矩阵更新
    rotation_matrix_T();                    //旋转矩阵的逆矩阵更新
}
//
//
//
///*********************************************************************************************************
//*函  数：void IMUupdate(FLOAT_XYZ *Gyr_rad,FLOAT_XYZ *Acc_filt,FLOAT_ANGLE *Att_Angle)
//*功　能：获取姿态角
//*参  数：Gyr_rad 指向角速度的指针（注意单位必须是弧度） GX
//*        Acc_filt 指向加速度的指针 AX
//*        Att_Angle 指向姿态角的指针
//*返回值：无
//*备  注：求解四元数和欧拉角都在此函数中完成
//**********************************************************************************************************/	
//#define Kp_New      0.9f              //互补滤波当前数据的权重
//#define Kp_Old      0.1f              //互补滤波历史数据的权重  
//#define G					  9.80665f		      // m/s^2	 重力加速度
//#define RadtoDeg    57.324841f				//弧度到角度 (弧度 * 180/3.1415)
////#define DegtoRad    0.0174533f				//角度到弧度 (角度 * 3.1415/180)
//#define Acc_Gain  	0.0001220f				//加速度变成G (初始化加速度满量程-+4g LSBa = 2*4/65535.0)
//#define Gyro_Gain 	0.0609756f				//角速度变成度 (初始化陀螺仪满量程+-2000 LSBg = 2*2000/65535.0)
//#define Gyro_Gr	    0.0010641f			  //角速度变成弧度(3.1415/180 * LSBg)  
//float   DCMgb[3][3];                  //方向余弦阵（将 惯性坐标系 转化为 机体坐标系）
//
////kp=ki=0 就是完全相信陀螺仪
//#define Kp 1.50f                         // proportional gain governs rate of convergence to accelerometer/magnetometer
//                                         //比例增益控制加速度计，磁力计的收敛速率
//#define Ki 0.005f                        // integral gain governs rate of convergence of gyroscope biases  
//                                         //积分增益控制陀螺偏差的收敛速度
//#define halfT 0.005f                     // half the sample period 采样周期的一半
//
//void IMUupdate(float ax,float ay,float az,float gx,float gy,float gz,imu660_data *Att_Angle)
//{
//	cat_uint8_t i;
//	float matrix[9] = {1.f,  0.0f,  0.0f, 0.0f,  1.f,  0.0f, 0.0f,  0.0f,  1.f };//初始化矩阵
//    //float ax = Acc_filt->X,ay = Acc_filt->Y,az = Acc_filt->Z;
//    //float gx = Gyr_rad->X,gy = Gyr_rad->Y,gz = Gyr_rad->Z;
//    float vx, vy, vz;
//    float ex, ey, ez;
//        float norm;
//    static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;     // quaternion elements representing the estimated orientation
//    static float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error
//
//    float q0q0 = q0*q0;
//    float q0q1 = q0*q1;
//    float q0q2 = q0*q2;
//    float q0q3 = q0*q3;
//    float q1q1 = q1*q1;
//    float q1q2 = q1*q2;
//    float q1q3 = q1*q3;
//    float q2q2 = q2*q2;
//    float q2q3 = q2*q3;
//    float q3q3 = q3*q3;
//        
//        if(ax*ay*az==0)
//            return;
//        
//    //加速度计测量的重力向量(机体坐标系) 
//        norm = invSqrt(ax*ax + ay*ay + az*az); 
//    ax = ax * norm;
//    ay = ay * norm;
//    az = az * norm;
//    //	printf("ax=%0.2f ay=%0.2f az=%0.2f\r\n",ax,ay,az);
//    
//        //陀螺仪积分估计重力向量(机体坐标系) 
//    vx = 2*(q1q3 - q0q2);												
//    vy = 2*(q0q1 + q2q3);
//    vz = q0q0 - q1q1 - q2q2 + q3q3 ;
//    // printf("vx=%0.2f vy=%0.2f vz=%0.2f\r\n",vx,vy,vz); 
//        
//        //测量的重力向量与估算的重力向量差积求出向量间的误差 
//    ex = (ay*vz - az*vy); //+ (my*wz - mz*wy);                     
//    ey = (az*vx - ax*vz); //+ (mz*wx - mx*wz);
//    ez = (ax*vy - ay*vx); //+ (mx*wy - my*wx);
//
//    //用上面求出误差进行积分
//    exInt = exInt + ex * Ki;								 
//    eyInt = eyInt + ey * Ki;
//    ezInt = ezInt + ez * Ki;
//
//    //将误差PI后补偿到陀螺仪
//    gx = gx + Kp*ex + exInt;					   		  	
//    gy = gy + Kp*ey + eyInt;
//    gz = gz + Kp*ez + ezInt;//这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减
//
//    //四元素的微分方程
//    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
//    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
//    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
//    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;
//
//    //单位化四元数 
//    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
//    q0 = q0 * norm;
//    q1 = q1 * norm;
//    q2 = q2 * norm;  
//    q3 = q3 * norm;
//        
//        //矩阵R 将惯性坐标系(n)转换到机体坐标系(b) 
//        matrix[0] = q0q0 + q1q1 - q2q2 - q3q3;// 11(前列后行)
//        matrix[1] = 2.f * (q1q2 + q0q3);	    // 12
//        matrix[2] = 2.f * (q1q3 - q0q2);	    // 13
//        matrix[3] = 2.f * (q1q2 - q0q3);	    // 21
//        matrix[4] = q0q0 - q1q1 + q2q2 - q3q3;// 22
//        matrix[5] = 2.f * (q2q3 + q0q1);	    // 23
//        matrix[6] = 2.f * (q1q3 + q0q2);	    // 31
//        matrix[7] = 2.f * (q2q3 - q0q1);	    // 32
//        matrix[8] = q0q0 - q1q1 - q2q2 + q3q3;// 33
//        
//    //四元数转换成欧拉角(Z->Y->X) 
//    Att_Angle->yaw += gz *RadtoDeg*0.01f;     
//    //	Att_Angle->yaw = atan2(2.f * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3)* 57.3f; // yaw
//    Att_Angle->pitch = -asinf(2.f * (q1q3 - q0q2))* 57.3f;                                 // pitch(负号要注意) 
//    Att_Angle->roll = atan2f(2.f * q2q3 + 2.f * q0q1, q0q0 - q1q1 - q2q2 + q3q3)* 57.3f ; // roll
//    /*for(i=0;i<9;i++)
//    {
//        *(&(DCMgb[0][0])+i) = matrix[i];
//    }*/
//	
//	//失控保护 (调试时可注释掉)
////	Safety_Check(); 
//}

/***********************************************************
函数名称：void IMU(void)
函数功能：获得姿态结算后的值
入口参数：无
出口参数：无
备 注：直接读取imu.pitch  imu.roll  imu.yaw
***********************************************************/
void IMU(void)
{
    if(set.offset_flag)
    {
        /*获取X、Y的角速度和加速度*/
        mpu6050_get_accdata();
        mpu6050_get_gyro();
        /*滤波算法*/
        mpu6050_gyro_x-=set.gyro.x;
        mpu6050_gyro_y-=set.gyro.y;
        mpu6050_gyro_z-=set.gyro.z;

        acc_x = iir_lpf(mpu6050_acc_x,acc_x,imu.att_acc_factor);
        acc_y = iir_lpf(mpu6050_acc_y,acc_y,imu.att_acc_factor);
        acc_z = iir_lpf(mpu6050_acc_z,acc_z,imu.att_acc_factor);
        //printf("%.2f,%.2f,%.2f\n",acc_x,acc_y,acc_z);
        gyro_x =iir_lpf(mpu6050_gyro_x,gyro_x,imu.att_gyro_factor);
        gyro_y =iir_lpf(mpu6050_gyro_y,gyro_y,imu.att_gyro_factor);
        gyro_z =iir_lpf(mpu6050_gyro_z,gyro_z,imu.att_gyro_factor);
        //printf("%d,%d,%d\n",gyro_x,gyro_y,gyro_z);

        //=================重力补偿版
        /*acc_x = (float)mpu6050_acc_x * Acc_Gain * G;
        acc_y = (float)mpu6050_acc_y * Acc_Gain * G;
        acc_z = (float)mpu6050_acc_z * Acc_Gain * G;
        gyro_x = (float)mpu6050_gyro_x * Gyro_Gain;
        gyro_y = (float)mpu6050_gyro_y * Gyro_Gain;
        gyro_z = (float)mpu6050_gyro_z * Gyro_Gain;
        //-----------------IIR滤波
        acc_x = acc_x*Kp_New + acc_x_old *Kp_Old;
        acc_y = acc_y*Kp_New + acc_y_old *Kp_Old;
        acc_z = acc_z*Kp_New + acc_z_old *Kp_Old;
        acc_x_old = acc_x;
        acc_y_old = acc_y;
        acc_z_old = acc_z;
        IMUupdate(acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,&imu);*/
        //===============================

        /*数据存储*/
        imu.acc_g.x = (float)acc_x/4096; //加速度计量程为:±8g/4096, ±16g/2048, ±4g/8192, ±2g/16384 
        imu.acc_g.y = (float)acc_y/4096;
        imu.acc_g.z = (float)acc_z/4096;
        imu.deg_s.x = (float)mpu6050_gyro_x/16.4f;//陀螺仪量程为:±2000dps/16.4, ±1000dps/32.8, ±500 dps /65.6
        imu.deg_s.y = (float)mpu6050_gyro_y/16.4f;//±250 dps/131.2, ±125 dps/262.4
        imu.deg_s.z = (float)mpu6050_gyro_z/16.4f;

        //卡尔曼姿态解算
        //imu.roll = -Kalman_Filter_x(imu.acc_g.x,imu.deg_s.x);
        //imu.pitch = -Kalman_Filter_y(imu.acc_g.y,imu.deg_s.y);
        //imu.yaw = -Kalman_Filter_x(imu.acc_g.z,imu.deg_s.z);

        /*姿态解算*/
        mahony_update(imu.deg_s.x,imu.deg_s.y,imu.deg_s.z,imu.acc_g.x,imu.acc_g.y,imu.acc_g.z);
        Matrix_ready();
    }
}
