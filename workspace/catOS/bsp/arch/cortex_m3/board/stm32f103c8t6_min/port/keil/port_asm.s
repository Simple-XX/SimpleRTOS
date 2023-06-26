;    thumb
;	preserve8
;	area    |.text|, code, readonly
;    .cpu cortex-m3
;    .syntax unified
;CODE16

    IMPORT  cat_sp_cur_task
    IMPORT  cat_sp_next_task

NVIC_INT_CTRL   EQU 0xE000ED04
NVIC_PENDSVSET  EQU 0x10000000
NVIC_SYSPRI2    EQU 0xE000ED22
NVIC_PENDSV_PRI EQU 0x000000FF

    AREA    |.text|, CODE, READONLY
;
; cat_uint32_t cat_hw_irq_disable(void)
; 关中断方式进入临界区
; primask-->r0
;
    EXPORT cat_hw_irq_disable
cat_hw_irq_disable  PROC
    mrs r0, primask             ;/* ret = primask */
    cpsid I                     ;/* disable irq */
    bx lr                       ;/* return ret */
    ENDP
;/**
; * void cat_hw_irq_enable(cat_uint32_t status)
; * 开中断方式出临界区
; * r0-->status
; */
    EXPORT cat_hw_irq_enable
cat_hw_irq_enable   PROC
    msr primask, r0             ;/* primask = status */
    bx lr     
    ENDP                  
    
;/**
; * void cat_hw_context_switch(void)
; * 触发pendsv中断进行任务切换(pendsv的优先级在开始第一个任务时已经设置)
; */
    
    EXPORT cat_hw_context_switch
cat_hw_context_switch   PROC
    ldr r0, =NVIC_INT_CTRL      
    ldr r1, =NVIC_PENDSVSET
    str r1, [r0]                ;/* *(NVIC_INT_CTRL) = NVIC_PENDSVSET */
    bx  lr
    ENDP

    EXPORT  PendSV_Handler
PendSV_Handler  PROC
    mrs r0, psp                     ;@保存当前任务堆栈到r0
                                    ;@用户级堆栈psp， 特权级堆栈msp

    cbz r0, PendSV_Handler_nosave   ;@若 psp==0 ,跳转到第一个任务

    stmfd r0!, {r4-r11}             ;@保存 r4-r11（减前/递减满堆栈）
    
    ldr r1, =cat_sp_cur_task        ;@记录最后的指针到任务栈curstk->stack
    ldr r1, [r1]
    str r0, [r1]
		
PendSV_Handler_nosave            
    ldr r0, =cat_sp_cur_task       ;@curtask = rdytask
    ldr r1, =cat_sp_next_task         
    ldr r2, [r1]
    str r2, [r0]

    ldr r0, [r2]                    ;@恢复新任务的寄存器
    ldmfd r0!, {r4-r11}

    msr psp, r0                     ;@从r0恢复下一任务的堆栈指针
    orr lr, lr, #0x04               ;@设置退出异常后使用psp堆栈
    bx lr                           ;@退出异常，自动从堆栈恢复部分寄存器
	ENDP
;		end
    END

