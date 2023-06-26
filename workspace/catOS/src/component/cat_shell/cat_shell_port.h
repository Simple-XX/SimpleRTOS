/**
 * @file cat_shell_port.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-10
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-10 1.0    amoigus             内容
 */

#ifndef CAT_SHELL_PORT_H
#define CAT_SHELL_PORT_H

#include "catos_config.h"
#include "catos_types.h"

/**
 * @brief 创建shell任务
 * 
 * note :20220726踩坑：cat_shell_task_create(void) 声明未加 ;
 * 导致报错：
 * ...
 * catOS/src/component/cat_shell/cat_shell.h: In function 'cat_shell_task_create':
 * catOS/src/component/cat_shell/cat_shell.h:50:19: error: storage class specified for parameter 'shell_io_callback_t'
 * 50 | typedef cat_int16_t (*shell_io_callback_t)(cat_uint8_t *, cat_uint16_t );
 * ...
 */
void cat_shell_task_create(void);



#endif
