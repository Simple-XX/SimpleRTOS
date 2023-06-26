#    thumb
#	preserve8
#	area    |.text|, code, readonly
    .cpu cortex-m3
    .syntax unified
    .thumb
    .text

    .extern  cat_sp_cur_task

    .equ SCB_VTOR, 0xE000ED08
    .equ NVIC_INT_CTRL,   0xE000ED04
    .equ NVIC_PENDSVSET,  0x10000000
    .equ NVIC_SYSPRI2,    0xE000ED22
    .equ NVIC_PENDSV_PRI, 0x000000FF

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
    ldr r0, =NVIC_INT_CTRL      
    ldr r1, =NVIC_PENDSVSET
    str r1, [r0]                /* *(NVIC_INT_CTRL) = NVIC_PENDSVSET */
    bx  lr

    .global  PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    /* 关闭全局中断并保存当前中断屏蔽寄存器中的值方便恢复 */
    mrs r2, primask
    cpsid i

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

    /* 用户级堆栈是psp，特权级堆栈是msp */
    /* 任务用的是psp，将当前寄存器保存到堆栈中 */
    mrs r1, psp
    /* 保存 r4-r11（减前/递减满堆栈） */
    stmfd r1!, {r4-r11}
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

    ldmfd r1!, {r4-r11}
    msr psp, r1

pendsv_exit:
    /* 恢复屏蔽寄存器值 */
    msr primask, r2
    /* 设置退出pendsv中断后使用psp堆栈寄存器 */
    orr lr, lr, #0x04
    bx lr

    .global  cat_hw_context_switch_to_first
    .type cat_hw_context_switch_to_first, %function
cat_hw_context_switch_to_first:
    /* 加载参数到变量 cat_context_to_task_sp_ptr*/
    ldr r1, =cat_context_to_task_sp_ptr
    str r0, [r1]

    /* 将变量 cat_context_from_task_sp_ptr 设置为0*/
    ldr r1, =cat_context_from_task_sp_ptr
    mov r0, #0
    str r0, [r1]

    /* 触发pendsv中断，允许中断后会立即进入pendsv切换 */
    ldr r0, =NVIC_INT_CTRL      
    ldr r1, =NVIC_PENDSVSET
    str r1, [r0]                /* *(NVIC_INT_CTRL) = NVIC_PENDSVSET */

    /* 因为开始调度后不会返回主函数，故将msp设置为初值(向量表第一个成员) */
    ldr r0, =SCB_VTOR
    ldr r0, [r0]
    ldr r0, [r0]

    msr msp, r0

    /* 打开中断，然后会立即进入pendsv中断开始调度 */
    cpsie f
    cpsie i
    /* 不会到达这里 */

    .global  HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    /* 获取当前上下文 */
    mrs r0, msp
    tst lr, #0x04
    beq _get_sp_done
    mrs r0, psp

_get_sp_done:
    stmfd r0!, {r4-r11}
    stmfd r0!, {lr}

    tst lr, #0x04
    beq _update_msp
    msr psp, r0
    b _update_done

_update_msp:
    msr msp, r0

_update_done:
    push {lr}
    bl catos_hard_fault_deal

    /* 理论上回不来 */
    pop {lr}

    orr lr, lr, #0x04
    bx lr

    .end

