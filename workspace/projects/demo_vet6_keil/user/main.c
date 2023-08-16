
#include "catos.h"

#include "bmp.h"

#define TASK1_STACK_SIZE    (1024)
struct _cat_task_t task1;
cat_stack_type_t task1_env[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE    (1024)
struct _cat_task_t task2;
cat_stack_type_t task2_env[TASK2_STACK_SIZE];

#define TASK3_STACK_SIZE    (1024)
struct _cat_task_t task3;
cat_stack_type_t task3_env[TASK3_STACK_SIZE];


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

cat_uint8_t acc_x_str[15] = {0}, acc_y_str[15] = {0}, acc_z_str[15] = {0};
cat_uint8_t gyr_x_str[15] = {0}, gyr_y_str[15] = {0}, gyr_z_str[15] = {0};
cat_bool_t new_data_written = CAT_FALSE;
#define OLED
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
        //CAT_DEBUG_PRINTF("[task2] %d\r\n", catos_systicks);
        //cat_iic_oled_clear();
        cat_iic_oled_show_string(0,  0, (cat_uint8_t *)"accx=", 8);
        cat_iic_oled_show_string(0,  3, (cat_uint8_t *)"accy=", 8);
        cat_iic_oled_show_string(0,  6, (cat_uint8_t *)"accz=", 8);
        cat_iic_oled_show_string(64, 0, (cat_uint8_t *)"gyrx=", 8);
        cat_iic_oled_show_string(64, 3, (cat_uint8_t *)"gyry=", 8);
        cat_iic_oled_show_string(64, 6, (cat_uint8_t *)"gyrz=", 8);

        if(CAT_TRUE == new_data_written)
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

            new_data_written = CAT_FALSE;
        }
        

        //cat_iic_oled_show_string(0, 0, (cat_uint8_t *)"QAQ", 16);
        //cat_iic_oled_show_string(24, 0, (cat_uint8_t *)"QAQ", 8);
        //cat_iic_oled_show_string(42, 0, (cat_uint8_t *)"GOGOGO!", 16);
        //cat_iic_oled_show_string(0, 2, (cat_uint8_t *)"2023/8/10", 16);
		//cat_iic_oled_show_char(48,6,t,16);//显示ASCII字符	   
		//t++;
		//if(t>'~')t=' ';
		//cat_iic_oled_show_number(103,6,t,3,16);//显示ASCII字符的码值 	

        //cat_delay_ms(8000);
#endif
    }
}

// #define USE_NM_PDU
#ifdef USE_NM_PDU

#define BYTE0(data) (*((char*)(&data)))
#define BYTE1(data) (*((char*)(&data) + 1))
#define BYTE2(data) (*((char*)(&data) + 2))
#define BYTE3(data) (*((char*)(&data) + 3))

typedef struct _nm_mpu_pdu
{
    cat_uint8_t HEAD;
    cat_uint8_t D_ADDR;
    cat_uint8_t ID;
    cat_uint8_t LEN;

    cat_uint8_t ACC_X_L;
    cat_uint8_t ACC_X_H;
    cat_uint8_t ACC_Y_L;
    cat_uint8_t ACC_Y_H;
    cat_uint8_t ACC_Z_L;
    cat_uint8_t ACC_Z_H;
    cat_uint8_t GYR_X_L;
    cat_uint8_t GYR_X_H;
    cat_uint8_t GYR_Y_L;
    cat_uint8_t GYR_Y_H;
    cat_uint8_t GYR_Z_L;
    cat_uint8_t GYR_Z_H;

    cat_uint8_t SHOCK_STA;

    cat_uint8_t SUM_CHECK;
    cat_uint8_t ADD_CHEDK;  
} __attribute__((packed)) nm_mpu_pdu;


static void uart_send_mpu_niming(
    cat_uint16_t acc_x, 
    cat_uint16_t acc_y, 
    cat_uint16_t acc_z, 
    cat_uint16_t gyr_x,  
    cat_uint16_t gyr_y, 
    cat_uint16_t gyr_z
)
{
#if 0
    cat_uint8_t send_buff[30];

    cat_uint8_t cnt=0;

    send_buff[cnt++]=0xaa;
    send_buff[cnt++]=0xff;
    send_buff[cnt++]=0xf1;
    send_buff[cnt++]=;
    send_buff[cnt++]=;
    send_buff[cnt++]=;
    send_buff[cnt++]=;
    send_buff[cnt++]=;
#endif

#if 1
    nm_mpu_pdu data;
    cat_uint8_t *data_ptr = (cat_uint8_t *)&data;
    cat_uint8_t i;

    data.HEAD   = 0xaa;
    data.D_ADDR = 0xff;
    data.ID     = 0xf1;
    data.LEN    = 13;

    data.ACC_X_L  = BYTE0(acc_x);
    data.ACC_Y_L  = BYTE0(acc_y);
    data.ACC_Z_L  = BYTE0(acc_z);
    data.ACC_X_H  = BYTE1(acc_x);
    data.ACC_Y_H  = BYTE1(acc_y);
    data.ACC_Z_H  = BYTE1(acc_z);

    data.GYR_X_L  = BYTE0(gyr_x);
    data.GYR_Y_L  = BYTE0(gyr_y);
    data.GYR_Z_L  = BYTE0(gyr_z);
    data.GYR_X_H  = BYTE1(gyr_x);
    data.GYR_Y_H  = BYTE1(gyr_y);
    data.GYR_Z_H  = BYTE1(gyr_z);

    data.SHOCK_STA = 0x00;

    for(i=0; i<sizeof(nm_mpu_pdu); i++)
    {
        data.SUM_CHECK += data_ptr[i];
        data.ADD_CHEDK += data.SUM_CHECK;
    }


#if 1
    for(i=0; i<data.LEN+6; i++)
    {
        CAT_SYS_PUTCHAR(data_ptr[i]);
        //CAT_SYS_PRINTF("%x ", data_ptr);
    }
    //CAT_SYS_PRINTF("\r\n"); 
#else
    data_ptr = (cat_uint8_t *)&(data.ACC_X);
    for(i=0; i<data.LEN; i++)
    {
       CAT_SYS_PUTCHAR(data_ptr[i]); 
    }
#endif
#endif
    
}
#endif /* USE_NM_PDU */

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

#ifdef USE_NM_PDU
        uart_send_mpu_niming(acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z);
#elif 0
        //CAT_KPRINTF("acc_x=%d, acc_y=%d, acc_z=%d, gyr_x=%d, gyr_y=%d, gyr_z=%d\r\n", acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z);
        CAT_KPRINTF("acc_x=%d, gyr_x=%d\r\n", acc_x, gyr_x);
#else
        if(CAT_FALSE == new_data_written)
        {
            SPRINTF(acc_x_str, "%d", acc_x);
            SPRINTF(acc_y_str, "%d", acc_y);
            SPRINTF(acc_z_str, "%d", acc_z);
            SPRINTF(gyr_x_str, "%d", gyr_x);
            SPRINTF(gyr_y_str, "%d", gyr_y);
            SPRINTF(gyr_z_str, "%d", gyr_z);

            new_data_written = CAT_TRUE;
        }
        
#endif
        cat_sp_task_delay(10);
    }
}

extern cat_iic_bus_t soft_iic_2;
int main(void)
{
    /* 初始化操作系统 */
    catos_init();

    /* 利用pin驱动初始化板载led */
    board_led_init();

    cat_iic_init(&soft_iic_2);

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

    /* 开始调度 */
    catos_start_sched();

    return 0;
}
