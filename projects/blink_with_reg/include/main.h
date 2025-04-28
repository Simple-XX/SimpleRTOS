/**
 * @file main.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 寄存器led闪烁头文件
 * @version 0.1
 * @date 2025-03-21
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-21 <td>内容
 * </table>
 */
#ifndef MAIN_H
#define MAIN_H

// 寄存器地址定义
#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */

#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)

#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)

#define RCC_APB2ENR_OFFSET    0x18
#define GPIOC_CRH_OFFSET      0x04
#define GPIOC_ODR_OFFSET      0x0C

#define RCC_CR                (*((volatile cat_u32*)(RCC_BASE + )))
#define RCC_CFGR              (*((volatile cat_u32*)(RCC_BASE + )))
#define RCC_CR                (*((volatile cat_u32*)(RCC_BASE + )))
#define RCC_CR                (*((volatile cat_u32*)(RCC_BASE + )))

#define RCC_APB2ENR           (*((volatile cat_u32*)(RCC_BASE + RCC_APB2ENR_OFFSET)))
#define GPIOC_CRH             (*((volatile cat_u32*)(GPIOC_BASE + GPIOC_CRH_OFFSET)))
#define GPIOC_ODR             (*((volatile cat_u32*)(GPIOC_BASE + GPIOC_ODR_OFFSET)))

#endif