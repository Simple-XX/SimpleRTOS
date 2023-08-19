
#include "catos.h"

#include "bmp.h"
#include "mpu6050.h"
#include "usr_hmc.h"


#define TASK1_STACK_SIZE    (1024)
struct _cat_task_t task1;
cat_stack_type_t task1_env[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE    (1024)
struct _cat_task_t task2;
cat_stack_type_t task2_env[TASK2_STACK_SIZE];

#define TASK3_STACK_SIZE    (1024)
struct _cat_task_t task3;
cat_stack_type_t task3_env[TASK3_STACK_SIZE];

#define TASK4_STACK_SIZE    (1024)
struct _cat_task_t task4;
cat_stack_type_t task4_env[TASK4_STACK_SIZE];


#define BOARD_LED_PIN PIN('B', 5)

void board_led_init(void)
{
    cat_pin_init(BOARD_LED_PIN, CAT_PIN_MODE_OUTPUT);
}

void board_led_on(void)
{
    cat_pin_write(BOARD_LED_PIN, CAT_PIN_LOW);
}

void board_led_off(void)
{
    cat_pin_write(BOARD_LED_PIN, CAT_PIN_HIGH);
}

void task1_entry(void *arg)
{

    for(;;)
    {
        board_led_on();
        cat_sp_task_delay(100);
        board_led_off();
        cat_sp_task_delay(100);
    }
}


#define OLED

// #define OLED_SHOW_MPU6050
#ifdef OLED_SHOW_MPU6050
cat_uint8_t acc_x_str[15] = {0}, acc_y_str[15] = {0}, acc_z_str[15] = {0};
cat_uint8_t gyr_x_str[15] = {0}, gyr_y_str[15] = {0}, gyr_z_str[15] = {0};
cat_bool_t new_mpu_data_written = CAT_FALSE;
#endif

#define OLED_SHOW_HMC5883L
#ifdef OLED_SHOW_HMC5883L
cat_float_t hmc_x=0, hmc_z=0, hmc_y=0;
cat_uint8_t hmc_x_str[15] = {0}, hmc_z_str[15] = {0}, hmc_y_str[15] = {0};
cat_bool_t new_hmc_data_written = CAT_FALSE;
#endif

void task2_entry(void *arg)
{
#ifdef OLED
    cat_uint8_t t;

    cat_iic_oled_init();
    cat_iic_oled_clear(); 

    t=' ';
#endif
    for(;;)
    {
        cat_sp_task_delay(100);
#ifdef OLED
 #ifdef OLED_SHOW_MPU6050
        //CAT_DEBUG_PRINTF("[task2] %d\r\n", catos_systicks);
        //cat_iic_oled_clear();
        cat_iic_oled_show_string(0,  0, (cat_uint8_t *)"accx=", 8);
        cat_iic_oled_show_string(0,  3, (cat_uint8_t *)"accy=", 8);
        cat_iic_oled_show_string(0,  6, (cat_uint8_t *)"accz=", 8);
        cat_iic_oled_show_string(64, 0, (cat_uint8_t *)"gyrx=", 8);
        cat_iic_oled_show_string(64, 3, (cat_uint8_t *)"gyry=", 8);
        cat_iic_oled_show_string(64, 6, (cat_uint8_t *)"gyrz=", 8);

        if(CAT_TRUE == new_mpu_data_written)
        {
            cat_iic_oled_show_string(0, 1,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(0, 4,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(0, 7,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(0, 1,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(0, 4,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(0, 7,  (cat_uint8_t *)"        ", 8);

            cat_iic_oled_show_string(0, 1, acc_x_str, 8);
            cat_iic_oled_show_string(0, 4, acc_y_str, 8);
            cat_iic_oled_show_string(0, 7, acc_z_str, 8);
            cat_iic_oled_show_string(64, 1, gyr_x_str, 8);
            cat_iic_oled_show_string(64, 4, gyr_y_str, 8);
            cat_iic_oled_show_string(64, 7, gyr_z_str, 8);

            new_mpu_data_written = CAT_FALSE;
        }
 #endif /* OLED_SHOW_MPU6050 */

 #ifdef OLED_SHOW_HMC5883L
        cat_iic_oled_show_string(0,  0, (cat_uint8_t *)"hmc_x=", 8);
        cat_iic_oled_show_string(0,  3, (cat_uint8_t *)"hmc_z=", 8);
        cat_iic_oled_show_string(0,  6, (cat_uint8_t *)"hmc_y=", 8);

        if(CAT_TRUE == new_hmc_data_written)
        {
            cat_iic_oled_show_string(64, 0,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(64, 3,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(64, 6,  (cat_uint8_t *)"        ", 8);

#if 1
            SPRINTF(hmc_x_str, "%.2f", hmc_x);
            SPRINTF(hmc_z_str, "%.2f", hmc_z);
            SPRINTF(hmc_y_str, "%.2f", hmc_y);

            cat_iic_oled_show_string(64, 0, hmc_x_str, 8);
            cat_iic_oled_show_string(64, 3, hmc_z_str, 8);
            cat_iic_oled_show_string(64, 6, hmc_y_str, 8);

#else
            //cat_iic_oled_show_number(64, 0, hmc_x_str, 6, 8);
            //cat_iic_oled_show_number(64, 3, hmc_z_str, 6, 8);
            //cat_iic_oled_show_number(64, 6, hmc_y_str, 6, 8);
#endif

            new_hmc_data_written = CAT_FALSE;
        }
 #endif /* OLED_SHOW_HMC5883L */

#endif /* OLED */

    //CAT_SYS_PRINTF("hmc_x_str=%s, hmc_z_str=%s, hmc_y_str=%s\r\n", hmc_x_str, hmc_z_str, hmc_y_str);
    }
}

#if 0
void task3_entry(void *arg)
{
    cat_int32_t acc_x, acc_y, acc_z;
    cat_int32_t gyr_x, gyr_y, gyr_z;

    cat_mpu6050_init();

    for(;;)
    {
        acc_x = cat_mpu6050_get_data(ACCEL_XOUT_H);
        acc_y = cat_mpu6050_get_data(ACCEL_YOUT_H);
        acc_z = cat_mpu6050_get_data(ACCEL_ZOUT_H);
        gyr_x = cat_mpu6050_get_data(GYRO_XOUT_H );
        gyr_y = cat_mpu6050_get_data(GYRO_YOUT_H );
        gyr_z = cat_mpu6050_get_data(GYRO_ZOUT_H );

        if(CAT_FALSE == new_mpu_data_written)
        {
            SPRINTF(acc_x_str, "%d", acc_x);
            SPRINTF(acc_y_str, "%d", acc_y);
            SPRINTF(acc_z_str, "%d", acc_z);
            SPRINTF(gyr_x_str, "%d", gyr_x);
            SPRINTF(gyr_y_str, "%d", gyr_y);
            SPRINTF(gyr_z_str, "%d", gyr_z);

            new_mpu_data_written = CAT_TRUE;
        }
        
        cat_sp_task_delay(10);
    }
}
#else
void task3_entry(void *arg)
{
    cat_int32_t pitch, roll, yaw;
    mpu6050_init();

    while(1)
    {
#if 1
        IMU();
        pitch = (cat_int32_t)imu.pitch;
        roll  = (cat_int32_t)imu.roll;
        yaw   = (cat_int32_t)imu.yaw;
        //CAT_SYS_PRINTF("%d, %d, %d\r\n", pitch, roll, yaw);
#endif
        cat_sp_task_delay(100);
    }

}
#endif

void task4_entry(void *arg)
{
    while(1)
    {
        hmc_get_all_data(&hmc_x, &hmc_z, &hmc_y);
        new_hmc_data_written = CAT_TRUE;

        CAT_SYS_PRINTF("hmc_x=%.2f, hmc_z=%.2f, hmc_y=%.2f\r\n", hmc_x, hmc_z, hmc_y);

        cat_sp_task_delay(100);
    }
}

extern cat_iic_bus_t soft_iic_2;
int main(void)
{
    /* 初始化操作系统 */
    catos_init();

    /* 利用pin驱动初始化板载led */
    board_led_init();

    /* 初始化软件 iic2 */
    cat_iic_init(&soft_iic_2);

    /* 初始化 HMC5883L */
    hmc_init();

#if 0
    /* 测试浮点输出 */
    cat_float_t a = 23.456;
    CAT_SYS_PRINTF("float var.0 a=%.0f\r\n", a);
    CAT_SYS_PRINTF("float var.1 a=%.1f\r\n", a);
    CAT_SYS_PRINTF("float var.2 a=%.2f\r\n", a);
    CAT_SYS_PRINTF("float var.3 a=%.3f\r\n", a);
    CAT_SYS_PRINTF("float var.4 a=%.4f\r\n", a);
    CAT_SYS_PRINTF("float var.5 a=%.5f\r\n", a);
    CAT_SYS_PRINTF("float var.10 a=%.10f\r\n", a);
#endif

    /* 测试创建任务运行 */
    cat_sp_task_create(
      (const cat_uint8_t *)"task1",
      &task1,
      task1_entry,
      NULL,
      0,
      task1_env,
      TASK1_STACK_SIZE
    );

    cat_sp_task_create(
      (const cat_uint8_t *)"task2",
      &task2,
      task2_entry,
      NULL,
      0,
      task2_env,
      sizeof(task2_env)
    );

    cat_sp_task_create(
      (const cat_uint8_t *)"task3",
      &task3,
      task3_entry,
      NULL,
      0,
      task3_env,
      sizeof(task3_env)
    );

    cat_sp_task_create(
      (const cat_uint8_t *)"task4",
      &task4,
      task4_entry,
      NULL,
      0,
      task4_env,
      sizeof(task4_env)
    );

    /* 开始调度 */
    catos_start_sched();

    return 0;
}
