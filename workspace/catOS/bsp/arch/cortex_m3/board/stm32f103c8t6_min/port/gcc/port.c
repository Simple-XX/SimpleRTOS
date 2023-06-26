/**
 * @file port.c
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-03-22
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-03-22 1.0    amoigus             内容
 */

#include "cat_task.h"
#include "cat_intr.h"
#include "port.h"

#include "../../drivers/uart/cat_drv_uart.h"

//#define STM32F103xE
#include "stm32f1xx.h"

#define NVIC_INT_CTRL   0xE000ED04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SYSPRI2    0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF


#if defined(__GNUC__)
int __wrap_atexit(void __attribute__((unused)) (*function)(void)) {
    return -1;
}
#endif /* #if defined(__GNUC__) */

/* PRIVATE FUNCS DECL START */
static void SystemClock_Config(void);
static void cat_set_systick_period(cat_uint32_t ms);
/* PRIVATE FUNCS DECL END */

/**
 * @brief 硬件初始化
 */
void cat_hw_init(void)
{
    /* 系统时钟初始化成72 MHz */
    SystemClock_Config();

    /* 设置系统时钟中断频率为100Hz(每秒100次) */
    cat_set_systick_period(CATOS_SYSTICK_MS);

    // /* 初始化串口 */
    // cat_bsp_uart_init();
}

/* stm32的时钟中断处理函数 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    /* 调用自己的处理函数 */
    cat_intr_systemtick_handler();
}

/* 开始调度 */
void catos_start_sched(void)
{

    cat_sp_task_before_start_first();

    /* 开启定时器中断 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    __set_PSP(0);

    MEM8(NVIC_SYSPRI2)      = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL)    = NVIC_PENDSVSET;
}

/* 栈初始化 */
/**< 异常触发时自动保存的寄存器 */
struct _exception_stack_frame
{
  cat_uint32_t r0;
  cat_uint32_t r1;
  cat_uint32_t r2;
  cat_uint32_t r3;
  cat_uint32_t r12;
  cat_uint32_t lr;  /**< r14 */
  cat_uint32_t pc;  /**< r15 */
  cat_uint32_t psr;
};

struct _stack_frame
{
  /**< 需要自行保存的寄存器 */
  cat_uint32_t r4;
  cat_uint32_t r5;
  cat_uint32_t r6;
  cat_uint32_t r7;
  cat_uint32_t r8;
  cat_uint32_t r9;
  cat_uint32_t r10;
  cat_uint32_t r11;

  struct _exception_stack_frame exeption_stack_frame;
};

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return cat_uint8_t*     初始化后的栈顶地址
 */
cat_uint8_t *cat_hw_stack_init(void *task_entry, void *arg, cat_uint8_t *stack_addr, void *exit_func)
{
  struct _stack_frame *stack_frame;
  cat_uint8_t             *stack;
  cat_uint32_t            i;

  /* 先加上4字节再8字节向下取整对齐(相当于四舍五入) */
  stack = stack_addr += sizeof(cat_uint32_t);
  stack = (cat_uint8_t *)CAT_ALIGN_DOWN((cat_uint32_t)stack, 8);

  /* 栈向上生长 */
  stack -= sizeof(struct _stack_frame);
  stack_frame = (struct _stack_frame *)stack;

  /* 初始化栈帧中所有寄存器 */
  for(i = 0; i < (sizeof(struct _stack_frame) / sizeof(cat_uint32_t)); i++)
  {
      ((cat_uint32_t *)stack_frame)[i] = 0xdeadbeef;
  }

  stack_frame->exeption_stack_frame.r0  = (cat_uint32_t)arg;
  stack_frame->exeption_stack_frame.r1  = 0;
  stack_frame->exeption_stack_frame.r2  = 0;
  stack_frame->exeption_stack_frame.r3  = 0;
  stack_frame->exeption_stack_frame.r12 = 0;
  stack_frame->exeption_stack_frame.lr  = (cat_uint32_t)exit_func;
  stack_frame->exeption_stack_frame.pc  = (cat_uint32_t)task_entry;
  stack_frame->exeption_stack_frame.psr = (cat_uint32_t)(1 << 24);

  /* 返回当前栈指针 */
  return stack;
}


/* PRIVATE FUNCS DEF START */
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/**
 * @brief 初始化系统时钟中断(分频器)
 * 
 * @param ms 周期(ms)
 */
static void cat_set_systick_period(cat_uint32_t ms)
{
    SysTick->LOAD = ms * SystemCoreClock / 1000;                    /* 重载计数器值 */
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    SysTick->VAL = 0;

    /* 设定systick控制寄存器 */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |/* 设定为内核时钟FCLK */
                    SysTick_CTRL_TICKINT_Msk |/* 设定为systick计数器倒数到0时触发中断 */
                    ~SysTick_CTRL_ENABLE_Msk;/* 关闭定时器中断，若创建任务则在catos_start_sched()中开启该中断 */
}
/* PRIVATE FUNCS DEF END */


