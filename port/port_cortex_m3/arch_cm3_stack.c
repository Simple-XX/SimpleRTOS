/**
 * @file arch_cm3_stack.c
 * @author 文佳源 (648137125@qq.com)
 * @brief cortex-m3的cpu相关可移植接口实现
 * @version 0.1
 * @date 2023-06-04
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-04 <td>新建文件，从board目录将cpu相关内容放到这里
 * </table>
 */

#include "catos.h"

cat_u32 cat_context_from_task_sp_ptr;
cat_u32 cat_context_to_task_sp_ptr;

/* 栈初始化 */
/**< 异常触发时自动保存的寄存器 */
struct _exception_stack_frame
{
  cat_u32 r0;
  cat_u32 r1;
  cat_u32 r2;
  cat_u32 r3;
  cat_u32 r12;
  cat_u32 lr;  /**< r14 */
  cat_u32 pc;  /**< r15 */
  cat_u32 psr;
};

struct _stack_frame
{
  /**< 需要自行保存的寄存器 */
  cat_u32 r4;
  cat_u32 r5;
  cat_u32 r6;
  cat_u32 r7;
  cat_u32 r8;
  cat_u32 r9;
  cat_u32 r10;
  cat_u32 r11;

  struct _exception_stack_frame exeption_stack_frame;
};

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return cat_u8*     初始化后的栈顶地址
 */
cat_u8 *cat_hw_stack_init(void (*task_entry)(void *), void *arg, cat_u8 *stack_addr, void  (*exit_func)(void *))
{
  struct _stack_frame *stack_frame;
  cat_u8             *stack;
  cat_u32            i;

  /* 先加上4字节再8字节向下取整对齐(相当于四舍五入) */
  stack = stack_addr + sizeof(cat_u32);
  stack = (cat_u8 *)CAT_ALIGN_DOWN((cat_u32)stack, 8);

  /* 栈向上生长? */
  stack -= sizeof(struct _stack_frame);
  stack_frame = (struct _stack_frame *)stack;

  /* 初始化栈帧中所有寄存器 */
  for(i = 0; i < (sizeof(struct _stack_frame) / sizeof(cat_u32)); i++)
  {
      ((cat_u32 *)stack_frame)[i] = 0xdeadbeef;
  }

  stack_frame->exeption_stack_frame.r0  = (cat_u32)arg;
  stack_frame->exeption_stack_frame.r1  = 0x11111111;
  stack_frame->exeption_stack_frame.r2  = 0x22222222;
  stack_frame->exeption_stack_frame.r3  = 0x33333333;
  stack_frame->exeption_stack_frame.r12 = 0xcccccccc;
  stack_frame->exeption_stack_frame.lr  = (cat_u32)exit_func;
  stack_frame->exeption_stack_frame.pc  = (cat_u32)task_entry;
  stack_frame->exeption_stack_frame.psr = (cat_u32)(1 << 24);

  /* 返回当前栈指针 */
  return stack;
}

#if (CATOS_STDIO_ENABLE == 1)
#define SCB_CFSR        (*(volatile const unsigned *)0xE000ED28) /* Configurable Fault Status Register */
#define SCB_HFSR        (*(volatile const unsigned *)0xE000ED2C) /* HardFault Status Register */
#define SCB_MMAR        (*(volatile const unsigned *)0xE000ED34) /* MemManage Fault Address register */
#define SCB_BFAR        (*(volatile const unsigned *)0xE000ED38) /* Bus Fault Address Register */
#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                               /* Reset value, write to SCB_AIRCR can reset cpu */

#define SCB_CFSR_MFSR   (*(volatile const unsigned char*)0xE000ED28)  /* Memory-management Fault Status Register */
#define SCB_CFSR_BFSR   (*(volatile const unsigned char*)0xE000ED29)  /* Bus Fault Status Register */
#define SCB_CFSR_UFSR   (*(volatile const unsigned short*)0xE000ED2A) /* Usage Fault Status Register */

static void usage_fault_track(void)
{
    cat_printf("usage fault:\r\n");
    cat_printf("SCB_CFSR_UFSR:%2x ", SCB_CFSR_UFSR);

    if(SCB_CFSR_UFSR & (1<<0))
    {
        /* [0]:UNDEFINSTR */
        cat_printf("UNDEFINSTR ");
    }

    if(SCB_CFSR_UFSR & (1<<1))
    {
        /* [1]:INVSTATE */
        cat_printf("INVSTATE ");
    }

    if(SCB_CFSR_UFSR & (1<<2))
    {
        /* [2]:INVPC */
        cat_printf("INVPC ");
    }

    if(SCB_CFSR_UFSR & (1<<3))
    {
        /* [3]:NOCP */
        cat_printf("NOCP ");
    }

    if(SCB_CFSR_UFSR & (1<<8))
    {
        /* [8]:UNALIGNED */
        cat_printf("UNALIGNED ");
    }

    if(SCB_CFSR_UFSR & (1<<9))
    {
        /* [9]:DIVBYZERO */
        cat_printf("DIVBYZERO ");
    }

    cat_printf("\r\n");
}

static void bus_fault_track(void)
{
    cat_printf("bus fault:\r\n");
    cat_printf("SCB_CFSR_BFSR:%2x ", SCB_CFSR_BFSR);

    if(SCB_CFSR_BFSR & (1<<0))
    {
        /* [0]:IBUSERR */
        cat_printf("IBUSERR ");
    }

    if(SCB_CFSR_BFSR & (1<<1))
    {
        /* [1]:PRECISERR */
        cat_printf("PRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<2))
    {
        /* [2]:IMPRECISERR */
        cat_printf("IMPRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<3))
    {
        /* [3]:UNSTKERR */
        cat_printf("UNSTKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<4))
    {
        /* [4]:STKERR */
        cat_printf("STKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<7))
    {
        cat_printf("SCB->BFAR:%8x\r\n", SCB_BFAR);
    }
    else
    {
        cat_printf("\r\n");
    }
}

static void mem_manage_fault_track(void)
{
    cat_printf("mem manage fault:\r\n");
    cat_printf("SCB_CFSR_MFSR:%2x ", SCB_CFSR_MFSR);

    if(SCB_CFSR_MFSR & (1<<0))
    {
        /* [0]:IACCVIOL */
        cat_printf("IACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<1))
    {
        /* [1]:DACCVIOL */
        cat_printf("DACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<3))
    {
        /* [3]:MUNSTKERR */
        cat_printf("MUNSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<4))
    {
        /* [4]:MSTKERR */
        cat_printf("MSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<7))
    {
        /* [7]:MMARVALID */
        cat_printf("SCB->MMAR:%8x\r\n", SCB_MMAR);
    }
    else
    {
        cat_printf("\r\n");
    }
}

static void hard_fault_track(void)
{
    if(SCB_HFSR & (1UL<<1))
    {
        /* [1]:VECTBL, Indicates hard fault is caused by failed vector fetch. */
        cat_printf("failed vector fetch\r\n");
    }

    if(SCB_HFSR & (1UL<<30))
    {
        /* [30]:FORCED, Indicates hard fault is taken because of bus fault,
                        memory management fault, or usage fault. */
        if(SCB_CFSR_BFSR)
        {
            bus_fault_track();
        }

        if(SCB_CFSR_MFSR)
        {
            mem_manage_fault_track();
        }

        if(SCB_CFSR_UFSR)
        {
            usage_fault_track();
        }
    }

    if(SCB_HFSR & (1UL<<31))
    {
        /* [31]:DEBUGEVT, Indicates hard fault is triggered by debug event. */
        cat_printf("debug event\r\n");
    }
}

/**
 * fault exception handling
 */
void catos_hard_fault_deal(struct _stack_frame *stack)
{
    cat_kprintf("\r\n\r\n*** HardFault occurred ***\r\n");
    cat_u32 exc_ret = *((cat_u32*)stack); /* 这里其实是最后压栈的lr */
    cat_kprintf("exrt: 0x%8x\r\n", exc_ret);
    stack = (struct _stack_frame *)((cat_ubase)stack+4);
    cat_kprintf("r00 : 0x%8x\r\n", stack->exeption_stack_frame.r0);
    cat_kprintf("r01 : 0x%8x\r\n", stack->exeption_stack_frame.r1);
    cat_kprintf("r02 : 0x%8x\r\n", stack->exeption_stack_frame.r2);
    cat_kprintf("r03 : 0x%8x\r\n", stack->exeption_stack_frame.r3);
    cat_kprintf("r12 : 0x%8x\r\n", stack->exeption_stack_frame.r12);
    cat_kprintf(" lr : 0x%8x\r\n", stack->exeption_stack_frame.lr);
    cat_kprintf(" pc : 0x%8x\r\n", stack->exeption_stack_frame.pc);
    cat_kprintf("psr : 0x%8x\r\n", stack->exeption_stack_frame.psr);

    void *do_ps(void *arg);
    do_ps(CAT_NULL);

    cat_task_t *current_task = cat_task_get_current();

    cat_kprintf("cat_task_current=%x\r\n", (cat_ubase)current_task);

    if(CAT_NULL != current_task)
    {
        cat_kprintf("hard fault on thread: %s\r\n", current_task->task_name);

        cat_kprintf(
            "\ttask info:\r\n"
            "\t\t->sp         = 0x%8x\r\n"
            "\t\t->entry      = 0x%8x\r\n"
            "\t\t->stk_start  = 0x%8x\r\n"
            "\t\t->stack_size = 0x%8x\r\n"
            "\t\t->delay      = 0x%8x\r\n"
            "\t\t->state      = 0x%8x\r\n"
            "\t\t->prio       = 0x%8x\r\n"
            "\t\t->slice      = 0x%8x\r\n"
            "\t\t->ipc_wait   = 0x%8x\r\n"
            "\t\t->error      = 0x%8x\r\n"
            ,
            (cat_u32)current_task->sp,
            (cat_u32)current_task->entry,
            (cat_u32)current_task->stack_start_addr,
            (cat_u32)current_task->stack_size,
            (cat_u32)current_task->delay,
            (cat_u32)current_task->state,
            (cat_u32)current_task->prio,
            (cat_u32)current_task->slice,
            (cat_u32)current_task->ipc_wait,
            (cat_u32)current_task->error
        );
    }
    else
    {
        cat_kprintf("hard fault before first task start\r\n");
    }

    hard_fault_track();

    while (1);
}
#else /* #if (CATOS_STDIO_ENABLE == 1) */
void catos_hard_fault_deal(struct _stack_frame *stack)
{
    (void)stack;

    while (1);
}
#endif /* #if (CATOS_STDIO_ENABLE == 1) */
