/**
 * @file contex.s
 * @author 文佳源 (648137125@qq.com)
 * @brief arm-cortex-m33上下文切换汇编源文件
 * @version 0.1
 * @date 2023-06-06
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-06 <td>创建
 * <tr><td>v1.1 <td>文佳源 <td>2023-06-27 <td>简要整理
 * </table>
 */
    .cpu cortex-m33
    .syntax unified
    .thumb
    .text

    .extern  cat_sp_cur_task

    .equ SCB_ICSR,        0xE000ED04 @ 中断控制寄存器
    .equ SCB_VTOR,        0xE000ED08 @ 中断向量表偏移寄存器
    .equ ICSR_PENDSVSET,  0x10000000 @ pendsv触发值

    .equ SHPR3_PRI_14,    0xE000ED22 @ 系统异常handler优先级寄存器 3 (PendSV).
    .equ PRI_LVL_PENDSV,  0xFF       @ pendsv优先级 (最低).
    .equ SHPR3_PRI_15,    0xE000ED23 @ 系统异常handler优先级寄存器 3 (Systick).

/* 定义在上下文切换使用的临时变量 */
/* !!!用不了，不知道为啥 */
@ cat_context_from_task_sp_ptr:
@     .word 0x0
@ cat_context_to_task_sp_ptr:
@     .word 0x0

/**
 * cat_uint32_t cat_hw_irq_disable(void)
 * 关中断方式进入临界区
 * primask-->r0
 */
    .global cat_hw_irq_disable
    .type cat_hw_irq_disable, %function
cat_hw_irq_disable:
    mrs r0, primask             /* ret = primask */
    cpsid I                     /* disable irq */
    bx lr                       /* return ret */

/**
 * void cat_hw_irq_enable(cat_uint32_t status)
 * 开中断方式出临界区
 * r0-->status
 */
    .global cat_hw_irq_enable
    .type cat_hw_irq_enable, %function
cat_hw_irq_enable:
    msr primask, r0             /* primask = status */
    bx lr                       
    
/**
 * void cat_hw_context_switch(void)
 * 触发pendsv中断进行任务切换(pendsv的优先级在开始第一个任务时已经设置)
 */
    .global cat_hw_context_switch
    .type cat_hw_context_switch, %function
cat_hw_context_switch:
    /* 将两个任务的堆栈指针变量的 *地址* 加载到临时变量中 */
    /* cat_context_from_task_sp_ptr = &(cat_sp_cur_task->sp) */
    ldr r2, =cat_context_from_task_sp_ptr
    str r0, [r2]

    /* cat_context_to_task_sp_ptr = &(cat_sp_next_task->sp) */
    ldr r2, =cat_context_to_task_sp_ptr
    str r1, [r2]

    /* 触发pendsv中断进行切换 */
    ldr r0, =SCB_ICSR      
    ldr r1, =ICSR_PENDSVSET
    str r1, [r0]                /* *(SCB_ICSR) = ICSR_PENDSVSET */
    bx  lr

    .global  HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    /* 获取当前上下文 */
    mrs r0, msp
    /** 此时lr中存储的是EXC_RETURN值，其中[2]位即
     * 第三个bit指明从中断退出后使用的堆栈，若是1
     * 则说明使用psp，故需要将psp放进r0作为参数 
     */
    tst lr, #0x04
    beq _get_sp_done
    mrs r0, psp

_get_sp_done:
    stmfd r0!, {r4-r11, lr}

    mrs r4, psplim

    stmfd r0!, {r4}

    /* 诊断数据结构顶上有个exc_return，所以还得存一遍 */
    stmfd r0!, {lr}

    tst lr, #0x04
    beq _update_msp
    msr psp, r0
    b _update_done

_update_msp:
    msr msp, r0

_update_done:
    /* 获取msp作为第二个参数 */
    mrs r1, msp
    push {lr}
    /**
     * r0->进入hardfault前的堆栈
     * r1->msp
     */
    bl catos_hard_fault_deal

    /* 理论上回不来 */
    pop {lr}

    orr lr, lr, #0x04
    bx lr

    .end
