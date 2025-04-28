/**
 * @file catos.h
 * @author mio (648137125@qq.com)
 * @brief catOS 用户接口
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CATOS_H
#define CATOS_H

#include "catos_types.h"
#include "catos_config.h"

#include "cat_device.h"
#include "cat_intr.h"
#include "cat_ipc.h"
#include "cat_lib.h"
#include "cat_log.h"
#include "cat_mem.h"
#include "cat_shell.h"
#include "cat_stdio.h"
#include "cat_task.h"

#include "cat_assert.h"

/* 系统相关 START */
/**
 * @brief 系统初始化
 */
void catos_init(void);

void catos_start_schedule(void);
/* 系统相关 END */

#endif
