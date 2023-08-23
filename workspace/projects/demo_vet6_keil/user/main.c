
#include "catos.h"

/* 定义任务资源 START */
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
/* 定义任务资源 END   */

/* 定义引脚 */
#define BOARD_LED_PIN PIN('B', 5)

/* 定义要使用的 IIC 总线 START */
#define OLED_IIC_BUS_CONFIG \
{ \
    .scl = PIN('A', 5), \
    .sda = PIN('A', 7), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"oled_bus", \
}

#define MPU6050_IIC_BUS_CONFIG \
{ \
    .scl = PIN('B', 6), \
    .sda = PIN('B', 7), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"mpu_bus", \
}

#define HMC5883L_IIC_BUS_CONFIG \
{ \
    .scl = PIN('A', 3), \
    .sda = PIN('A', 2), \
    .is_init = CAT_FALSE, \
    .delay_us = 0, \
    .timeout_us = 100, \
    .name = (const cat_uint8_t *)"hmc_bus", \
}
cat_iic_bus_t oled_iic_bus = OLED_IIC_BUS_CONFIG;
cat_iic_bus_t mpu_iic_bus  = MPU6050_IIC_BUS_CONFIG;
cat_iic_bus_t hmc_iic_bus  = HMC5883L_IIC_BUS_CONFIG;
/* 定义要使用的 IIC 总线 END   */

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

/* 板载 led 闪烁 */
void task1_entry(void *arg)
{
    /* 利用pin驱动初始化板载led */
    board_led_init();

    for(;;)
    {
        board_led_on();
        cat_sp_task_delay(100);
        board_led_off();
        cat_sp_task_delay(100);
    }
}


#define OLED

#define OLED_SHOW_MPU6050
#ifdef OLED_SHOW_MPU6050
cat_uint8_t pitch_str[15] = {0}, roll_str[15] = {0}, yaw_str[15] = {0};
cat_bool_t new_mpu_data_written = CAT_FALSE;
#endif

// #define OLED_SHOW_HMC5883L
#ifdef OLED_SHOW_HMC5883L
cat_float_t hmc_x=0, hmc_z=0, hmc_y=0;
cat_uint8_t hmc_x_str[15] = {0}, hmc_z_str[15] = {0}, hmc_y_str[15] = {0};
cat_bool_t new_hmc_data_written = CAT_FALSE;
#endif

/* oled 显示 */
void task2_entry(void *arg)
{
#ifdef OLED
    cat_uint8_t t;

    cat_iic_oled_init(&oled_iic_bus);
    cat_iic_oled_clear(); 

#endif
    for(;;)
    {
        cat_sp_task_delay(10);
#ifdef OLED
 #ifdef OLED_SHOW_MPU6050
        cat_iic_oled_show_string(0,  0, (cat_uint8_t *)"pitch=", 8);
        cat_iic_oled_show_string(0,  2, (cat_uint8_t *)"roll =", 8);
        cat_iic_oled_show_string(0,  4, (cat_uint8_t *)"yaw  =", 8);

        if(CAT_TRUE == new_mpu_data_written)
        {
            cat_iic_oled_show_string(64, 0,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(64, 2,  (cat_uint8_t *)"        ", 8);
            cat_iic_oled_show_string(64, 4,  (cat_uint8_t *)"        ", 8);

            cat_iic_oled_show_string(64, 0, pitch_str, 8);
            cat_iic_oled_show_string(64, 2, roll_str , 8);
            cat_iic_oled_show_string(64, 4, yaw_str  , 8);

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

    }
}

/* mpu6050 数据获取 */
void task3_entry(void *arg)
{
    cat_float_t ax, ay, az;
    cat_float_t gx, gy, gz;
    cat_float_t pitch, roll, yaw;

    cat_mpu6050_init(&mpu_iic_bus);
    cat_delay_ms(1000);

    for(;;)
    {
        cat_mpu6050_get_angle(&pitch, &roll, &yaw);

        if(CAT_FALSE == new_mpu_data_written)
        {
            SPRINTF(pitch_str, "%.2f", pitch);
            SPRINTF(roll_str , "%.2f", roll);
            SPRINTF(yaw_str  , "%.2f", yaw);

            new_mpu_data_written = CAT_TRUE;
        }

        cat_sp_task_delay(10);
    }
}

/* hmc5883l 磁场数据获取 */
void task4_entry(void *arg)
{
    /* 初始化 HMC5883L */
    // hmc_init();

    // hmc_self_test(&hmc_iic_bus);

    while(1)
    {
#if 0
        hmc_get_all_data(&hmc_x, &hmc_z, &hmc_y);
        new_hmc_data_written = CAT_TRUE;

        CAT_SYS_PRINTF("hmc_x=%.2f, hmc_z=%.2f, hmc_y=%.2f\r\n", hmc_x, hmc_z, hmc_y);

        cat_sp_task_delay(100);   
#endif
        cat_sp_task_delay(100);
    }
}


int main(void)
{
    /* 初始化操作系统 */
    catos_init();

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
      1,
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

    // cat_sp_task_create(
    //   (const cat_uint8_t *)"task4",
    //   &task4,
    //   task4_entry,
    //   NULL,
    //   0,
    //   task4_env,
    //   sizeof(task4_env)
    // );

    /* 开始调度 */
    catos_start_sched();

    return 0;
}
