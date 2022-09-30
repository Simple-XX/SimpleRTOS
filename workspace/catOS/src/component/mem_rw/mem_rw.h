/**
 * @file mem_rw.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MEM_RW_H
#define MEM_RW_H

#include "catos_config.h"
#include "catos_types.h"

uint8_t get_reg(uint32_t *addr, uint32_t *val);
uint8_t set_reg(uint32_t *addr, uint32_t  val);

#endif
