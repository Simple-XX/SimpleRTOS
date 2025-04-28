/**
 * @file tests_config.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 测试配置
 * @version 0.1
 * @date 2025-04-05
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-04-05 <td>内容
 * </table>
 */
#ifndef TESTS_CONFIG_H
#define TESTS_CONFIG_H

/* 最好每次只选一个,不然放不下(如果存储够大可以考虑,但目前每次只能跑一个) */

#define TESTS_IPC_BASIC     0 /* 测试基础ipc */
#define TESTS_IPC_MQ        0 /* 测试消息队列 */
#define TESTS_IPC_PV        0 /* 测试优先级反转 */
#define TESTS_IPC_MUT       0 /* 测试互斥量 */
#define TESTS_IPC_SEM       0 /* 测试信号量 */
#define TESTS_LIB_DLIST     0 /* 测试差分队列(链表) */
#define TESTS_LIB_LIST      0 /* 测试双链表 */
#define TESTS_MEM           0 /* 测试动态内存 */

#define TESTS_TICK_OVERFLOW 1 /* 测试时钟32位溢出 */


#endif
