
#include "catos.h"

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
        // CAT_DEBUG_PRINTF("[task2] %d\r\n", catos_systicks);
    }
}


int main(void)
{
    board_led_init();
    // EXTI_Key_Config();

#if 1
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

    catos_start_sched();
#else
    /* 测试不创建任务下运行 */
    uint32_t i = 0;

    while(i++ < 0xffff);
    board_led_on();

    while(i-- > 0xd);
    board_led_off();
#endif

    return 0;
}
