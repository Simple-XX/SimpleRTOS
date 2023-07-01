/**
 * @file switch_to_first.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 切换到第一个任务
 * @version 0.1
 * @date 2023-06-12
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-12 <td>内容
 * </table>
 */
#include "bsp_api.h"

#include "catos_config.h"
#include "catos_types.h"

#include "cpu_port_def.h"

/**
 * @brief 切换到第一个任务的上下文
 * 
 */
void cat_hw_context_switch_to_first(void)
{
    __enable_irq();
    __ISB();

    /* 各个寄存器地址 */
    __asm volatile (
        ".equ SCB_ICSR,        0xE000ED04 \n" // 中断控制寄存器
        ".equ SCB_VTOR,        0xE000ED08 \n" // 中断向量表偏移寄存器
        ".equ ICSR_PENDSVSET,  0x10000000 \n" // pendsv触发值

        ".equ SHPR3_PRI_14,    0xE000ED22 \n" // 系统异常handler优先级寄存器 3 (PendSV).
        ".equ PRI_LVL_PENDSV,  0xFF        \n"// pendsv优先级 (最低).
        ".equ SHPR3_PRI_15,    0xE000ED23  \n"// 系统异常handler优先级寄存器 3 (Systick).

        "ldr r1, =cat_context_to_task_sp_ptr            \n"
        "str r0, [r1]                                   \n"

#if __FPU_USED
 //#error "__FPU_USED"
        /* 清除control寄存器的FPCA */
        "mrs     r2, control                            \n"  /* read */
        "bic     r2, r2, #0x04                          \n"  /* modify */
        "msr     control, r2                            \n"  /* write-back */
#else
 #error "must use fpu"
#endif

        /* 将变量 cat_context_from_task_sp_ptr 设置为0*/
        "ldr r1, =cat_context_from_task_sp_ptr          \n"
        "mov r0, #0                                     \n"
        "str r0, [r1]                                   \n"

        "mov r4, #0x1234 \n"

        /* 触发pendsv中断，允许中断后会立即进入pendsv切换 */
        "ldr r0, =SCB_ICSR                              \n"
        "ldr r1, =ICSR_PENDSVSET                        \n"
        "str r1, [r0]                                   \n" /* *(SCB_ICSR) = "ICSR_PENDSVSET */

        /* 不会到达这里 */
        "dsb                                            \n"
        "isb                                            \n"
        "svc 0                                          \n"
    );
}