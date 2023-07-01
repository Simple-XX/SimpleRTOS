/**
 * @file cpu_port.c
 * @author 文佳源 (648137125@qq.com)
 * @brief cortex-m33 cpu移植相关
 * @version 0.1
 * @date 2023-06-06
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-06 <td>创建
 * </table>
 */
#include "bsp_api.h"

#include "catos_types.h"

#include "cat_task.h"
#include "cat_stdio.h"

#include "cpu_port_def.h"

cat_uint32_t cat_context_from_task_sp_ptr;
cat_uint32_t cat_context_to_task_sp_ptr;

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

#if CATOS_6M5_USE_FPUSTACK
 //#error "fpu used"
    /* FPU register */
    cat_uint32_t S0;
    cat_uint32_t S1;
    cat_uint32_t S2;
    cat_uint32_t S3;
    cat_uint32_t S4;
    cat_uint32_t S5;
    cat_uint32_t S6;
    cat_uint32_t S7;
    cat_uint32_t S8;
    cat_uint32_t S9;
    cat_uint32_t S10;
    cat_uint32_t S11;
    cat_uint32_t S12;
    cat_uint32_t S13;
    cat_uint32_t S14;
    cat_uint32_t S15;
    cat_uint32_t FPSCR;
    cat_uint32_t NO_NAME;
#endif
};

struct _stack_frame
{
    /**< 需要自行保存的寄存器 */

    //cat_uint32_t tz; /* 暂时用不到 */
    //cat_uint32_t lr;
    cat_uint32_t psplim;
    //cat_uint32_t control;

    cat_uint32_t r4;
    cat_uint32_t r5;
    cat_uint32_t r6;
    cat_uint32_t r7;
    cat_uint32_t r8;
    cat_uint32_t r9;
    cat_uint32_t r10;
    cat_uint32_t r11;

    cat_uint32_t exc_return;

#if CATOS_6M5_USE_FPUSTACK
    /* FPU register s16 ~ s31 */
    cat_uint32_t s16;
    cat_uint32_t s17;
    cat_uint32_t s18;
    cat_uint32_t s19;
    cat_uint32_t s20;
    cat_uint32_t s21;
    cat_uint32_t s22;
    cat_uint32_t s23;
    cat_uint32_t s24;
    cat_uint32_t s25;
    cat_uint32_t s26;
    cat_uint32_t s27;
    cat_uint32_t s28;
    cat_uint32_t s29;
    cat_uint32_t s30;
    cat_uint32_t s31;
#endif

    struct _exception_stack_frame exeption_stack_frame;
};

struct _exception_info
{
    cat_uint32_t exc_return;
    struct _stack_frame stack_frame;
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
    cat_uint32_t         *stack;
    cat_uint32_t         i;
    cat_uint32_t         *p;

    /* 先加上4字节再8字节向下取整对齐(相当于四舍五入) */
    stack = stack_addr + sizeof(cat_uint32_t);
    stack = (cat_uint8_t *)CAT_ALIGN_DOWN((cat_uint32_t)stack, 8);

#if 0
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

  stack_frame->lr     = 0xffffffbc;
  stack_frame->psplim = 0x00;
  stack_frame->control = 0x00;
#else /* #if 0 */
#if 0
cat_uint32_t size_stack_frame= sizeof(struct _stack_frame);
    cat_uint32_t size_exception_frame = sizeof(struct _exception_stack_frame);
    stack_frame = (struct _stack_frame *)(stack - sizeof(struct _stack_frame));
#endif
    /* task context saved & restore by hardware: */
    *(--stack) = (cat_uint32_t)0x01000000L; /* xPSR: EPSR.T = 1, thumb mode   */
    *(--stack) = (cat_uint32_t)task_entry;       /* Entry Point */
    //*(--stack) = ((cat_uint32_t)task_entry & port_task_entry_mask);       /* Entry Point  */
    *(--stack) = (cat_uint32_t)exit_func; /* R14 (LR)              */
    *(--stack) = (cat_uint32_t)0x12121212L; /* R12                            */
    *(--stack) = (cat_uint32_t)0x03030303L; /* R3                             */
    *(--stack) = (cat_uint32_t)0x02020202L; /* R2                             */
    *(--stack) = (cat_uint32_t)0x01010101L; /* R1                             */
    *(--stack) = (cat_uint32_t)arg;         /* R0 : argument                  */

    /* task context saved & restore by software: */
    /* EXC_RETURN = 0xFFFFFFBCL
       Task begin state: Thread mode + non-floating-point state + PSP + Non-secure */

#if __FPU_USED && !BSP_TZ_NONSECURE_BUILD
 //#error "__FPU_USED && !BSP_TZ_NONSECURE_BUILD"
    *(--stack) = (cat_uint32_t)portINITIAL_EXC_RETURN; /* exe_return值 */
#endif

    *(--stack) = (cat_uint32_t)0x11111111L; /* R11 */
    *(--stack) = (cat_uint32_t)0x10101010L; /* R10 */
    *(--stack) = (cat_uint32_t)0x09090909L; /* R9  */
    *(--stack) = (cat_uint32_t)0x08080808L; /* R8  */
    *(--stack) = (cat_uint32_t)0x07070707L; /* R7  */
    *(--stack) = (cat_uint32_t)0x06060606L; /* R6  */
    *(--stack) = (cat_uint32_t)0x05050505L; /* R5  */
    *(--stack) = (cat_uint32_t)0x04040404L; /* R4  */

    //*(--stack) = (cat_uint32_t)0x00;        /* control */
#if RM_CATOS_PORT_PSPLIM_PRESENT
    //*(--stack) = (cat_uint32_t)stack_addr + RM_CATOS_PORT_PENDSV_PSPLIM_OFFSET_BYTES;        /* psplim */
    *(--stack) = (cat_uint32_t)0x00;
#endif
    //*(--stack) = (cat_uint32_t)0xffffffbc;  /* lr */

    stack_frame = (struct _stack_frame *)stack;
#endif /* #if 0 */

  /* 返回当前栈指针 */
  return stack;
}

static void hard_fault_track(void);
#define CATOS_PRINT_EXC_STK(_exc_frame_ptr) \
    do{ \
        CAT_SYS_PRINTF("    psr: %8x\r\n", (_exc_frame_ptr)->psr); \
        CAT_SYS_PRINTF("     pc: %8x\r\n", (_exc_frame_ptr)->pc);  \
        CAT_SYS_PRINTF("     lr: %8x\r\n", (_exc_frame_ptr)->lr);  \
        CAT_SYS_PRINTF("    r12: %8x\r\n", (_exc_frame_ptr)->r12); \
        CAT_SYS_PRINTF("    r03: %8x\r\n", (_exc_frame_ptr)->r3);  \
        CAT_SYS_PRINTF("    r02: %8x\r\n", (_exc_frame_ptr)->r2);  \
        CAT_SYS_PRINTF("    r01: %8x\r\n", (_exc_frame_ptr)->r1);  \
        CAT_SYS_PRINTF("    r00: %8x\r\n", (_exc_frame_ptr)->r0);  \
    }while(0)

extern void *do_ps(void *arg);

/**
 * fault exception handling
 */
// void catos_hard_fault_deal(struct _exception_info *exception_info)
void catos_hard_fault_deal(struct _exception_info *exception_info, struct _exception_stack_frame *msp)
{
    struct _stack_frame *frame = NULL;

    CAT_SYS_PRINTF("\r\n****** HARD FAULT EXCEPTION ********\r\n");

    CAT_SYS_PRINTF("exc_return = %8x\r\n", exception_info->exc_return);

    /* 中断是否在handler(中断)中 */
    if (exception_info->exc_return & (1 << 2))
    {
        CAT_SYS_PRINTF("hard fault on task: %s\r\n", cat_sp_cur_task->task_name);
    }
    else
    {
        CAT_SYS_PRINTF("hard fault on handler\r\n");
    }

    /* 是否使用浮点单元 */
    if ( (exception_info->exc_return & 0x10) == 0)
    {
        CAT_SYS_PRINTF("FPU active!\r\n");
    }

    /* 打印堆栈 */
    frame = &(exception_info->stack_frame);
    
    CAT_SYS_PRINTF("\r\nbefore hardfault exception frame info:\r\n");
    CATOS_PRINT_EXC_STK(&(exception_info->stack_frame.exeption_stack_frame));

    CAT_SYS_PRINTF("\r\nmsp exception frame info:\r\n");
    CATOS_PRINT_EXC_STK(msp);

    CAT_SYS_PRINTF("\r\n psplim: %8x\r\n", frame->psplim);
    CAT_SYS_PRINTF("     r4: %8x\r\n", frame->r4); 
    CAT_SYS_PRINTF("     r5: %8x\r\n", frame->r5); 
    CAT_SYS_PRINTF("     r6: %8x\r\n", frame->r6);
    CAT_SYS_PRINTF("     r7: %8x\r\n", frame->r7); 
    CAT_SYS_PRINTF("     r8: %8x\r\n", frame->r8); 
    CAT_SYS_PRINTF("     r9: %8x\r\n", frame->r9); 
    CAT_SYS_PRINTF("    r10: %8x\r\n", frame->r10); 
    CAT_SYS_PRINTF("    r11: %8x\r\n", frame->r11); 

    do_ps(NULL);

    /* 诊断原因 */
    hard_fault_track();
    
    while (1);
}

/* static func defs */
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
    CAT_SYS_PRINTF("usage fault:\r\n");
    CAT_SYS_PRINTF("SCB_CFSR_UFSR:%2x ", SCB_CFSR_UFSR);

    if(SCB_CFSR_UFSR & (1<<0))
    {
        /* [0]:UNDEFINSTR */
        CAT_SYS_PRINTF("UNDEFINSTR ");
    }

    if(SCB_CFSR_UFSR & (1<<1))
    {
        /* [1]:INVSTATE */
        CAT_SYS_PRINTF("INVSTATE ");
    }

    if(SCB_CFSR_UFSR & (1<<2))
    {
        /* [2]:INVPC */
        CAT_SYS_PRINTF("INVPC ");
    }

    if(SCB_CFSR_UFSR & (1<<3))
    {
        /* [3]:NOCP */
        CAT_SYS_PRINTF("NOCP ");
    }

    if(SCB_CFSR_UFSR & (1<<4))
    {
        /* [3]:NOCP */
        CAT_SYS_PRINTF("STKOF ");
    }

    if(SCB_CFSR_UFSR & (1<<8))
    {
        /* [8]:UNALIGNED */
        CAT_SYS_PRINTF("UNALIGNED ");
    }

    if(SCB_CFSR_UFSR & (1<<9))
    {
        /* [9]:DIVBYZERO */
        CAT_SYS_PRINTF("DIVBYZERO ");
    }

    CAT_SYS_PRINTF("\r\n");
}

static void bus_fault_track(void)
{
    CAT_SYS_PRINTF("bus fault:\r\n");
    CAT_SYS_PRINTF("SCB_CFSR_BFSR:%2x ", SCB_CFSR_BFSR);

    if(SCB_CFSR_BFSR & (1<<0))
    {
        /* [0]:IBUSERR */
        CAT_SYS_PRINTF("IBUSERR ");
    }

    if(SCB_CFSR_BFSR & (1<<1))
    {
        /* [1]:PRECISERR */
        CAT_SYS_PRINTF("PRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<2))
    {
        /* [2]:IMPRECISERR */
        CAT_SYS_PRINTF("IMPRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<3))
    {
        /* [3]:UNSTKERR */
        CAT_SYS_PRINTF("UNSTKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<4))
    {
        /* [4]:STKERR */
        CAT_SYS_PRINTF("STKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<7))
    {
        CAT_SYS_PRINTF("SCB->BFAR:%8x\r\n", SCB_BFAR);
    }
    else
    {
        CAT_SYS_PRINTF("\r\n");
    }
}

static void mem_manage_fault_track(void)
{
    CAT_SYS_PRINTF("mem manage fault:\r\n");
    CAT_SYS_PRINTF("SCB_CFSR_MFSR:%2x ", SCB_CFSR_MFSR);

    if(SCB_CFSR_MFSR & (1<<0))
    {
        /* [0]:IACCVIOL */
        CAT_SYS_PRINTF("IACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<1))
    {
        /* [1]:DACCVIOL */
        CAT_SYS_PRINTF("DACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<3))
    {
        /* [3]:MUNSTKERR */
        CAT_SYS_PRINTF("MUNSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<4))
    {
        /* [4]:MSTKERR */
        CAT_SYS_PRINTF("MSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<7))
    {
        /* [7]:MMARVALID */
        CAT_SYS_PRINTF("SCB->MMAR:%8x\r\n", SCB_MMAR);
    }
    else
    {
        CAT_SYS_PRINTF("\r\n");
    }
}

static void hard_fault_track(void)
{
    if(SCB_HFSR & (1UL<<1))
    {
        /* [1]:VECTBL, Indicates hard fault is caused by failed vector fetch. */
        CAT_SYS_PRINTF("failed vector fetch\r\n");
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
        CAT_SYS_PRINTF("debug event\r\n");
    }
}