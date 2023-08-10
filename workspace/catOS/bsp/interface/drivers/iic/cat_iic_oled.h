/**
 * @file cat_iic_oled.h
 * @author 文佳源 (648137125@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-05
 * 
 * Copyright (c) 2023
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2023-08-05 <td>内容
 * </table>
 */
#ifndef CAT_IIC_OLED_H
#define CAT_IIC_OLED_H

#include "cat_iic.h"

//OLED控制用函数
void cat_iic_oled_send_byte(unsigned dat,unsigned cmd);  
void cat_iic_oled_display_on(void);
void cat_iic_oled_display_off(void);	   							   		    
void cat_iic_oled_init(void);
void cat_iic_oled_clear(void);


void cat_iic_oled_show_char(cat_uint8_t x,cat_uint8_t y,cat_uint8_t chr,cat_uint8_t Char_Size);
void cat_iic_oled_show_number(cat_uint8_t x,cat_uint8_t y,cat_uint32_t num,cat_uint8_t len,cat_uint8_t size);
void cat_iic_oled_show_string(cat_uint8_t x,cat_uint8_t y, cat_uint8_t *p,cat_uint8_t Char_Size);	 
void cat_iic_oled_set_position(unsigned char x, unsigned char y);
void cat_iic_oled_draw_bmp(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void cat_iic_oled_delay50ms(unsigned int Del_50ms);
void cat_iic_oled_delay1ms(unsigned int Del_1ms);
void cat_iic_oled_fill_picture(unsigned char fill_Data);



#endif
