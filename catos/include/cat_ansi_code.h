/**
 * @file cat_ansi_code.h
 * @author 文佳源 (648137125@qq.com)
 * @brief ANSI 终端控制码
 * @version 0.1
 * @date 2025-03-31
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-31 <td>内容
 * </table>
 */
#ifndef CAT_ANSI_CODE_H
#define CAT_ANSI_CODE_H

#define CAT_ANSI_ENABLE 1
#if (CAT_ANSI_ENABLE == 1)

/* 文字样式代码*/
#define CA_ESC "\033"     /* esc符号 */
#define CA_CSI CA_ESC "[" /* 控制序列引导符 */

/* 文本样式 */
#define CA_STYLE_RESET          CA_CSI "0m"   /* 重置所有样式 */
#define CA_STYLE_BOLD           CA_CSI "1m"   /* 加粗 */
#define CA_STYLE_FAINT          CA_CSI "2m"   /* 淡化 */
#define CA_STYLE_ITALIC         CA_CSI "3m"   /* 斜体 */
#define CA_STYLE_UNDERLINE      CA_CSI "4m"   /* 下划线 */
#define CA_STYLE_BLINK          CA_CSI "5m"   /* 闪烁 */
#define CA_STYLE_INVERSE        CA_CSI "7m"   /* 反显（交换前景色和背景色） */
#define CA_STYLE_STRIKETHROUGH  CA_CSI "9m"   /* 删除线 */

/* 前景色（文字颜色）*/
#define CA_FG_BLACK       CA_CSI "30m"  /* 黑色 */
#define CA_FG_RED         CA_CSI "31m"  /* 红色 */
#define CA_FG_GREEN       CA_CSI "32m"  /* 绿色 */
#define CA_FG_YELLOW      CA_CSI "33m"  /* 黄色 */
#define CA_FG_BLUE        CA_CSI "34m"  /* 蓝色 */
#define CA_FG_MAGENTA     CA_CSI "35m"  /* 洋红色 */
#define CA_FG_CYAN        CA_CSI "36m"  /* 青色 */
#define CA_FG_WHITE       CA_CSI "37m"  /* 白色 */

/* 背景颜色 */
#define CA_BG_BLACK       CA_CSI "40m"  /* 黑色背景 */
#define CA_BG_RED         CA_CSI "41m"  /* 红色背景 */
#define CA_BG_GREEN       CA_CSI "42m"  /* 绿色背景 */
#define CA_BG_YELLOW      CA_CSI "43m"  /* 黄色背景 */
#define CA_BG_BLUE        CA_CSI "44m"  /* 蓝色背景 */
#define CA_BG_MAGENTA     CA_CSI "45m"  /* 洋红色背景 */
#define CA_BG_CYAN        CA_CSI "46m"  /* 青色背景 */
#define CA_BG_WHITE       CA_CSI "47m"  /* 白色背景 */

/* 光标控制 */
#define CA_CURSOR_UP(n)    CA_CSI #n "A"         /* 光标向上移动 n 行 */
#define CA_CURSOR_DOWN(n)  CA_CSI #n "B"         /* 光标向下移动 n 行 */
#define CA_CURSOR_FORWARD(n) CA_CSI #n "C"       /* 光标向右移动 n 列 */
#define CA_CURSOR_BACK(n)  CA_CSI #n "D"         /* 光标向左移动 n 列 */
#define CA_CURSOR_POS(x, y) CA_CSI #y ";" #x "H" /* 将光标移动到第 y 行第 x 列 */
#define CA_CLEAR_SCREEN    CA_CSI "2J"           /* 清屏 */
#define CA_CLEAR_LINE      CA_CSI "2K"           /* 清除当前行 */

#endif /* #if (CAT_ANSI_ENABLE == 1) */
#endif /* #ifndef CAT_ANSI_CODE_H */