/**
 * @file cat_component.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 系统组件初始化
 * @version 0.1
 * @date 2025-03-23
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-23 <td>内容
 * </table>
 */
#include "meoo/meoo.h"

void cat_shell_task_create(void);

void cat_component_init(void)
{
    /* 创建shell任务 */
    cat_shell_task_create();

    meoo_init();
}
