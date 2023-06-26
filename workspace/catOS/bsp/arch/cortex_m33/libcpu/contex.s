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
 * </table>
 */
    .cpu cortex-m33
    .syntax unified
    .thumb
    .text

    .extern  cat_sp_cur_task


    @ .equ SCB_VTOR,          0xE000ED08 /* 中断向量表偏移寄存器 */
    @ .equ NVIC_INT_CTRL,     0xE000ED04 /* 中断控制寄存器 */
    @ .equ NVIC_PENDSVSET,    0x10000000 /* pendsv触发值 */
    @ .equ NVIC_SYSPRI2,      0xE000ED20 /* 优先级寄存器 */
    @ .equ NVIC_PENDSV_PRI,   0xFFFF0000 /* pendsv优先级 */

    .equ SCB_ICSR,        0xE000ED04 @ 中断控制寄存器
    .equ SCB_VTOR,        0xE000ED08 @ 中断向量表偏移寄存器
    .equ ICSR_PENDSVSET,  0x10000000 @ pendsv触发值

    .equ SHPR3_PRI_14,    0xE000ED22 @ 系统异常handler优先级寄存器 3 (PendSV).
    .equ PRI_LVL_PENDSV,  0xFF       @ pendsv优先级 (最低).
    .equ SHPR3_PRI_15,    0xE000ED23 @ 系统异常handler优先级寄存器 3 (Systick).
    @.equ PRI_LVL_SYSTICK, 0xFF       @ SYstick priority level (lowest).

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

    //.global  PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    /* 关闭全局中断并保存当前中断屏蔽寄存器中的值方便恢复 */
    mrs r2, primask
    cpsid i

    /* 保存中断屏蔽寄存器状态 */
    push {r2}

    /* 获取from任务的堆栈指针变量中的值 */
    /* 在进入pendsv之前 cat_context_from_task_sp_ptr = &(from_task->sp) */
    /** 故有：
     * r0 =  &(from_task->sp)
     * r1 = *(&(from_task->sp)) 等价于 r1 = from_task->sp
     */
    ldr r0, =cat_context_from_task_sp_ptr
    ldr r1, [r0]
    /* 如果为零则说明是第一个任务 */
    cbz r1, switch_to_thread

/* 暂时可能用不到trustzone, 因此直接跳转 */
    b contex_ns_store
/* 暂时可能用不到 END */

contex_ns_store:

    /* 用户级堆栈是psp，特权级堆栈是msp */
    /* 任务用的是psp，将当前寄存器保存到堆栈中 */
    mrs r1, psp

    tst lr, #0x10
    it eq
    vstmdbeq r1!, {s16-s31}

    /* 保存 r4-r11（减前/递减满堆栈） */
    stmfd r1!, {r4-r11, lr}

    /* 保存m33新增寄存器 */
    mrs r4, psplim
    stmfd r1!, {r4}

    /* 记录最后的指针到任务栈curstk->stack */
    /* 更新tcb的堆栈指针变量值 */
    /**
        from_task->sp = r1
     */
    ldr r0, [r0]
    str r1, [r0]
		
switch_to_thread:             
    ldr r1, =cat_context_to_task_sp_ptr
    ldr r1, [r1]
    ldr r1, [r1]

    ldmfd r1!, {r4}
    msr psplim, r4

    b contex_ns_load

contex_ns_load:

    ldmfd r1!, {r4-r11, lr}

    tst lr, #0x10
    it eq
    vldmiaeq r1!, {s16-s31}

pendsv_exit:
    msr psp, r1

    pop {r2}

    /* 恢复屏蔽寄存器值 */
    msr primask, r2
    /* 设置退出pendsv中断后使用psp堆栈寄存器 */
    //orr lr, lr, #0x04
    bx lr

    //.global  cat_hw_context_switch_to_first
    .type cat_hw_context_switch_to_first, %function
cat_hw_context_switch_to_first:
    /* 加载参数到变量 cat_context_to_task_sp_ptr*/
    ldr r1, =cat_context_to_task_sp_ptr
    str r0, [r1]

    /* must use fpu */
    /* 清除control寄存器的FPCA */
    mrs     r2, control         /* read */
    bic     r2, #0x04           /* modify */
    msr     control, r2         /* write-back */


    /* 将变量 cat_context_from_task_sp_ptr 设置为0*/
    ldr r1, =cat_context_from_task_sp_ptr
    mov r0, #0
    str r0, [r1]

    svc 2
    b .

    /* 触发pendsv中断，允许中断后会立即进入pendsv切换 */
    ldr r0, =SCB_ICSR      
    ldr r1, =ICSR_PENDSVSET
    str r1, [r0]                /* *(SCB_ICSR) = ICSR_PENDSVSET */

    /* 打开中断，然后会立即进入pendsv中断开始调度 */
    cpsie f
    cpsie i

    nop
    nop

    svc 0
    /* 不会到达这里 */

    //.global catos_restore_first_task
    .type catos_restore_first_task, %function
catos_restore_first_task:

    ldr r1, =cat_context_to_task_sp_ptr
    ldr r1, [r1]
    ldr r1, [r1]
    LDMIA   R1!, {R2}
    MSR     PSPLIM, R2

    LDMIA   R1!, {R4-R11, LR}
    TST       LR, #0x10
    IT        EQ
    VLDMIAEQ  R1!, {S16-S31}

    msr psp, r1

    pop {r2}
    msr primask, r2

    bx lr

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

    @ mov r3, lr
    mrs r4, psplim
    @ mrs r5, control

    @ stmfd r0!, {r3-r5}
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
