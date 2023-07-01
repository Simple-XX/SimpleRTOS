#include "hal_data.h"
#include "catos.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
#define TASK1_STACK_SIZE    (1024)
#define TASK2_STACK_SIZE    (1024)

struct _cat_task_t task1;
struct _cat_task_t task2;


cat_stack_type_t task1_env[TASK1_STACK_SIZE];
cat_stack_type_t task2_env[TASK2_STACK_SIZE];

uint32_t sched_task1_times = 0;
uint32_t sched_task2_times = 0;

#define BOARD_LED_PIN 0

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
        sched_task1_times++;
        board_led_on();
        cat_sp_task_delay(100);
        board_led_off();
        cat_sp_task_delay(100);
    }
}

void task2_entry(void *arg)
{
    for(;;)
    {
        cat_sp_task_delay(100);
        //CAT_DEBUG_PRINTF("[task2] %d\r\n", catos_systicks);
    }
}

//#define TEST_STACK_OVERFLOW
#ifdef TEST_STACK_OVERFLOW
void test_stkof(void)
{
    char a[4096] = {0};
    unsigned int i = 0;

    a[3456] = CAT_PIN_LOW;
    cat_pin_write(BOARD_LED_PIN, a[3456]);
}
#endif

void hal_entry(void)
{
    /* TODO: add your own code here */

    /* 初始化os */
    catos_init();

#ifdef TEST_STACK_OVERFLOW
    test_stkof();
#endif
    /* 利用pin驱动初始化板载led */
    board_led_init();

    /* 测试创建任务运行 */
    cat_sp_task_create(
      (const uint8_t *)"task1_task",
      &task1,
      task1_entry,
      NULL,
      0,
      task1_env,
      TASK1_STACK_SIZE
    );

    cat_sp_task_create(
      (const uint8_t *)"task2_task",
      &task2,
      task2_entry,
      NULL,
      0,
      task2_env,
      sizeof(task2_env)
    );


    /* 开始调度 */
    catos_start_sched();

    /* 不会到达这里 */

    while(1);

#if 0
    while(1)
    {
        cat_pin_write(0, CAT_PIN_LOW);
        R_SCI_UART_Write(&g_uart4_ctrl, (const uint8_t *)"low\r\n", 5);
        while(false == uart_send_complete_flag);

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

        cat_pin_write(0, CAT_PIN_HIGH);
        R_SCI_UART_Write(&g_uart4_ctrl, (const uint8_t *)"high\r\n", 6);
        while(false == uart_send_complete_flag);

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
    }
#endif

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
