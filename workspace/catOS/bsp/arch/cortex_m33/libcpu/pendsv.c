/**
 * @file pendsv.c
 * @author 文佳源 (648137125@qq.com)
 * @brief pendsv中断处理函数源文件
 * @version 0.1
 * @date 2023-06-11
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-11 <td>内容
 * </table>
 */
#include "bsp_api.h"

#include "catos_config.h"
#include "catos_types.h"

#include "cpu_port_def.h"

void PendSV_Handler(void)
{
    __asm volatile (
        /* 关闭全局中断并保存当前中断屏蔽寄存器中的值方便恢复 */
    "mrs r2, primask                            \n"
    "cpsid i                                    \n"

    /* 保存中断屏蔽寄存器状态 */
    "push {r2}                                  \n"

    /* 获取from任务的堆栈指针变量中的值 */
    /* 在进入pendsv之前 cat_context_from_task_sp_ptr = &(from_task->sp) */
    /** 故有：
     * r0 =  &(from_task->sp)
     * r1 = *(&(from_task->sp)) 等价于 r1 = from_task->sp
     */
    "ldr r0, =cat_context_from_task_sp_ptr      \n"
    "ldr r1, [r0]                               \n"
    /* 如果为零则说明是第一个任务 */
    "cbz r1, switch_to_thread                   \n"

/* 暂时可能用不到trustzone, 因此直接跳转 */
    "b contex_ns_store                          \n"
/* 暂时可能用不到 END */

"contex_ns_store:                               \n"

    /* 用户级堆栈是psp，特权级堆栈是msp */
    /* 任务用的是psp，将当前寄存器保存到堆栈中 */
    "mrs r1, psp                                \n"

#if __FPU_USED
    "tst lr, #0x10                              \n"
    "it eq                                      \n"
    "vstmdbeq r1!, {s16-s31}                    \n"

 #if BSP_TZ_NONSECURE_BUILD
  #error "BSP_TZ_NONSECURE_BUILD"
 #else
  //#error "!BSP_TZ_NONSECURE_BUILD"
        /* Stack R4-R11 on the process stack. Also stack LR since the FPU is supported. */
        "STMDB     R1!, {R4-R11, LR}             \n"
 #endif
#else
 #error "must use fpu"
#endif

#if BSP_TZ_NONSECURE_BUILD
 #error "should not use BSP_TZ_NONSECURE_BUILD"
#elif RM_CATOS_PORT_PSPLIM_PRESENT
//#error "RM_CATOS_PORT_PSPLIM_PRESENT"
        "mrs     r3, psplim                      \n" /* R3 = PSPLIM. */
        "stmdb   r1!, {r3}                       \n"
#endif

    /* 记录最后的指针到任务栈curstk->stack */
    /* 更新tcb的堆栈指针变量值 */
    /**
        from_task->sp = r1
     */
    "ldr r0, [r0]                               \n"
    "str r1, [r0]                               \n"

#if !__FPU_USED
 #error "must use fpu"
#endif

        /* 上下文保存结束 */
		
"switch_to_thread:                              \n"
    "ldr r1, =cat_context_to_task_sp_ptr        \n"
    "ldr r1, [r1]                               \n"
    "ldr r1, [r1]                               \n"

#if BSP_TZ_NONSECURE_BUILD
 #error "not support BSP_TZ_NONSECURE_BUILD"
#elif RM_CATOS_PORT_PSPLIM_PRESENT
 //#error "RM_CATOS_PORT_PSPLIM_PRESENT"
        "LDMIA   R1!, {R2}                       \n" /* R1 = PSPLIM */
        "MSR     PSPLIM, R2                      \n" /* Restore the PSPLIM register value for the task. */
#endif

#if BSP_TZ_NONSECURE_BUILD
 #error "not support BSP_TZ_NONSECURE_BUILD"
#endif

        "b contex_ns_load                           \n"
"contex_ns_load:                                    \n"

#if __FPU_USED
 #if BSP_TZ_NONSECURE_BUILD
  #error "not support BSP_TZ_NONSECURE_BUILD"
 #else
  //#error "!BSP_TZ_NONSECURE_BUILD"
        /* Restore R4-R11 and LR from the process stack. */
        "LDMIA   R1!, {R4-R11, LR}               \n"
 #endif

        /* Check to see if the thread being restored is using the FPU. If so, restore S16-S31. */
        "TST       LR, #0x10                     \n"
        "IT        EQ                            \n"
        "VLDMIAEQ  R1!, {S16-S31}                \n"
#else
 #error "must use fpu"
#endif

"pendsv_exit:                                   \n"
    "msr psp, r1                                \n"

#if !__FPU_USED
 #error "must use fpu"
#endif

    "pop {r2}                                   \n"

    /* 恢复屏蔽寄存器值 */
    "msr primask, r2                            \n"

#if __FPU_USED
    "bx lr                                      \n"
#else
 #error "must use fpu"
#endif
    );
}