void rm_freertos_port_restore_task_stackless (void)
{
    /* This is a naked/stackless function. Do not pass arguments to the inline assembly when the GCC compiler is
     * used. */

    __asm volatile (
        /* Restore the thread context and PSP.  */

        /* The first item in pxCurrentTCB is the task top of stack. */
        /* R0 = &pxCurrentTCB */
        /* R1 = pxCurrentTCB */
        /* R2 = pxCurrentTCB[0] = pxCurrentTCB->pxStack */
        "LDR     R0, =pxCurrentTCB               \n"
        "LDR     R1, [R0]                        \n"
        "LDR     R2, [R1]                        \n"

        "LDMIA   R2!, {R1}                       \n" /* R1 = PSPLIM */
        "MSR     PSPLIM, R1                      \n" /* Restore the PSPLIM register value for the task. */

        /* Restore R4-R11 and LR from the process stack. */
        "LDMIA   R2!, {R4-R11, LR}               \n"


        /* Check to see if the thread being restored is using the FPU. If so, restore S16-S31. */
        "TST       LR, #0x10                     \n"
        "IT        EQ                            \n"
        "VLDMIAEQ  R2!, {S16-S31}                \n"

        "MSR     PSP, R2                         \n"

        /** Return to the thread using the process stack and saved FPU setting. */
        "BX     LR                               \n"

        );
}

void PendSV_Handler (void)
{
    /* This function is assembly only to meet the requirements for naked/stackless functions. For GCC, only basic
     * assembly is allowed. */

    /* Before entry into this exception handler, the hardware stacks XPSR, PC, LR, R12, R3, R2, R1, and R0 onto the
     * stack (typically the process stack of the executing task). When the FPU is in use, the FPSCR and S0-S15
     * registers are also stored on the stack. All other registers are stored by software in this function. */

    /* Only R0-R3 and R12 can be used before stack preservation is complete. */

    /* R0-R3, R12, and LR are not guaranteed to retain their values after C functions are called. The main stack is used
     * when C functions are called. */

    __asm volatile (

        /* Store the address of pxCurrentTCB in R3. */
        /* R3 = &pxCurrentTCB */
        /* R1 = pxCurrentTCB */
        "LDR     R3, =pxCurrentTCB               \n"
        "LDR     R1, [R3]                        \n"

        /* Store PSP in R2 so it can be used with STM instructions.  */
        /* R2 = PSP */
        "MRS     R2,  PSP                        \n"

        /* Check to see if the current thread is using the FPU. If so, stack S16-S31. */
        "TST       LR, #0x10                     \n"
        "IT        EQ                            \n"
        "VSTMDBEQ  R2!, {S16-S31}                \n"

        /* Stack R4-R11 on the process stack. Also stack LR since the FPU is supported. */
        "STMDB     R2!, {R4-R11, LR}             \n"

        "MRS     R3, PSPLIM                      \n" /* R3 = PSPLIM. */
        "STMDB   R2!, {R3}                       \n"

        /* Save the new top of the stack into the first member of the TCB. */
        "STR     R2, [R1]                        \n"

        /* Restore the task. This function does not return. */
        "B rm_freertos_port_restore_task_stackless \n"
        );
}