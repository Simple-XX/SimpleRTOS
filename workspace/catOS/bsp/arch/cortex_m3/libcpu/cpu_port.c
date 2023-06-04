/**
 * @file cpu_port.c
 * @author 文佳源 (648137125@qq.com)
 * @brief cortex-m3的cpu相关可移植接口实现
 * @version 0.1
 * @date 2023-06-04
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-06-04 <td>新建文件，从board目录将cpu相关内容放到这里
 * </table>
 */

#include "catos_types.h"

/* 栈初始化 */
/**< 异常触发时自动保存的寄存器 */
struct _exception_stack_frame
{
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;  /**< r14 */
  uint32_t pc;  /**< r15 */
  uint32_t psr;
};

struct _stack_frame
{
  /**< 需要自行保存的寄存器 */
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;

  struct _exception_stack_frame exeption_stack_frame;
};

/**
 * @brief 栈初始化
 * 
 * @param task_entry    任务入口函数地址
 * @param parameter     参数
 * @param stack_addr    栈起始地址
 * @param exit          任务退出函数地址
 * @return uint8_t*     初始化后的栈顶地址
 */
uint8_t *cat_hw_stack_init(void *task_entry, void *arg, uint8_t *stack_addr, void *exit_func)
{
  struct _stack_frame *stack_frame;
  uint8_t             *stack;
  uint32_t            i;

  /* 先加上4字节再8字节向下取整对齐(相当于四舍五入) */
  stack = stack_addr += sizeof(uint32_t);
  stack = (uint8_t *)CAT_ALIGN_DOWN((uint32_t)stack, 8);

  /* 栈向上生长 */
  stack -= sizeof(struct _stack_frame);
  stack_frame = (struct _stack_frame *)stack;

  /* 初始化栈帧中所有寄存器 */
  for(i = 0; i < (sizeof(struct _stack_frame) / sizeof(uint32_t)); i++)
  {
      ((uint32_t *)stack_frame)[i] = 0xdeadbeef;
  }

  stack_frame->exeption_stack_frame.r0  = (uint32_t)arg;
  stack_frame->exeption_stack_frame.r1  = 0;
  stack_frame->exeption_stack_frame.r2  = 0;
  stack_frame->exeption_stack_frame.r3  = 0;
  stack_frame->exeption_stack_frame.r12 = 0;
  stack_frame->exeption_stack_frame.lr  = (uint32_t)exit_func;
  stack_frame->exeption_stack_frame.pc  = (uint32_t)task_entry;
  stack_frame->exeption_stack_frame.psr = (uint32_t)(1 << 24);

  /* 返回当前栈指针 */
  return stack;
}