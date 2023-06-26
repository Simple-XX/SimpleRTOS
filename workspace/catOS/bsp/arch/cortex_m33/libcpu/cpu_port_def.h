/**
 * @file cpu_port_def.h
 * @author 文佳源 (648137125@qq.com)
 * @brief cpu移植相关宏定义等
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
#ifndef CPU_PORT_DEF_H
#define CPU_PORT_DEF_H

#include "bsp_api.h"

/* 最低位清零 */
#define port_task_entry_mask    ((cat_uint32_t) 0xfffffffeUL)

/* 使用fpu的更长的堆栈 */
#define CATOS_6M5_USE_FPUSTACK (0)

// /* basepri相关 */
//  #if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_8M_MAIN__) // CM4 or CM33
//   #define RM_CATOS_PORT_HAS_BASEPRI    (1)
//  #else
//   #define RM_CATOS_PORT_HAS_BASEPRI    (0)
//  #endif

/* 堆栈初始化lr相关 */
#if BSP_TZ_NONSECURE_BUILD

/**
 * Initial EXC_RETURN value.
 *
 *     FF         FF         FF         BC
 * 1111 1111  1111 1111  1111 1111  1011 1100
 *
 * Bit[6] - 0 --> The exception was taken from the Non-Secure state.
 * Bit[5] - 1 --> Do not skip stacking of additional state context.
 * Bit[4] - 1 --> The PE did not allocate space on the stack for FP context.
 * Bit[3] - 1 --> Return to the Thread mode.
 * Bit[2] - 1 --> Restore registers from the process stack.
 * Bit[1] - 0 --> Reserved, 0.
 * Bit[0] - 0 --> The exception was taken to the Non-Secure state.
 */
 #define portINITIAL_EXC_RETURN            (0xffffffbc)
#else
 #define portINITIAL_EXC_RETURN            (0xfffffffd)
#endif

/* PSP相关 */
/* Determine which stack monitor to use. */
#ifdef __ARM_ARCH_8M_MAIN__            // CM33
//#error "__ARM_ARCH_8M_MAIN__"
 #define RM_CATOS_PORT_PSPLIM_PRESENT               (1)
 #define RM_CATOS_PORT_SPMON_PRESENT                (0)
#else
 #define RM_CATOS_PORT_PSPLIM_PRESENT               (0)
 #define RM_CATOS_PORT_SPMON_PRESENT                (1)
#endif

/* Determine the memory size added to PSP in PendSV_Handler. */
#define RM_CATOS_PORT_REGS_ON_PSP                   (8)

#if BSP_TZ_NONSECURE_BUILD
 #define RM_CATOS_PORT_CONTROL_ON_PSP               (1)
 #define RM_CATOS_PORT_EXTRA_LR_ON_PSP              (1)
 #define RM_CATOS_PORT_SECURE_ID_ON_PSP             (1)
#else
 #define RM_CATOS_PORT_CONTROL_ON_PSP               (0)
 #define RM_CATOS_PORT_EXTRA_LR_ON_PSP              (0)
 #define RM_CATOS_PORT_SECURE_ID_ON_PSP             (0)
#endif

#define RM_CATOS_PORT_PSPLIM_ON_PSP                 (RM_CATOS_PORT_PSPLIM_PRESENT)

#define RM_CATOS_PORT_PENDSV_PSP_WORDS              (RM_CATOS_PORT_REGS_ON_PSP + RM_CATOS_PORT_CONTROL_ON_PSP + \
                                                     RM_CATOS_PORT_EXTRA_LR_ON_PSP +                            \
                                                     RM_CATOS_PORT_SECURE_ID_ON_PSP +                           \
                                                     RM_CATOS_PORT_PSPLIM_ON_PSP)

/* Round up to nearest multiple of 8 bytes (2 words). */
#define RM_CATOS_PORT_PENDSV_PSPLIM_OFFSET_BYTES    ((RM_CATOS_PORT_PENDSV_PSP_WORDS + \
                                                         (RM_CATOS_PORT_PENDSV_PSP_WORDS & 1)) * 4)

#endif