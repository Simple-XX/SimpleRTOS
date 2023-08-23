/**
 * @file drv_iic_oled.c
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
#include "cat_task.h"
#include "drv_iic_oled.h"
#include "oledfont.h"

#include "cat_error.h"
#include "cat_delay.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

static cat_iic_bus_t *_oled_iic_bus_ptr = CAT_NULL;
#define OLED_BUS (_oled_iic_bus_ptr)

static void write_iic_oled_data(cat_uint8_t data)
{
    cat_iic_start(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, 0x78);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, 0x40);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, data);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_stop(OLED_BUS);
}

static void write_iic_oled_cmd(cat_uint8_t cmd)
{
    cat_iic_start(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, 0x78);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, 0x00);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_send_byte(OLED_BUS, cmd);
    //cat_iic_wait_ack(OLED_BUS);
    cat_iic_stop(OLED_BUS);
}

//m^n函数
static cat_uint32_t oled_pow(cat_uint8_t m,cat_uint8_t n)
{
	cat_uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

void cat_iic_oled_send_byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
        write_iic_oled_data(dat);
        // cat_iic_write_reg(OLED_BUS, 0x78, 0x40, dat);
    }
	else {
        write_iic_oled_cmd(dat);
	}
}

//开启OLED显示    
void cat_iic_oled_display_on(void)
{
    cat_iic_oled_send_byte(0X8D,OLED_CMD);  //SET DCDC命令
    cat_iic_oled_send_byte(0X14,OLED_CMD);  //DCDC ON
    cat_iic_oled_send_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void cat_iic_oled_display_off(void)
{
    cat_iic_oled_send_byte(0X8D,OLED_CMD);  //SET DCDC命令
    cat_iic_oled_send_byte(0X10,OLED_CMD);  //DCDC OFF
    cat_iic_oled_send_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

void cat_iic_oled_init(cat_iic_bus_t *bus)
{
    CAT_ASSERT(NULL != bus);
    _oled_iic_bus_ptr = bus;

    cat_iic_init(OLED_BUS);

    /* 等待 100 毫秒 */
    cat_delay_ms(100);

    cat_iic_wave(OLED_BUS);

    cat_iic_oled_send_byte(0xAE,OLED_CMD);//--display off
    
	cat_iic_oled_send_byte(0x00,OLED_CMD);//---set low column address
	cat_iic_oled_send_byte(0x10,OLED_CMD);//---set high column address
	cat_iic_oled_send_byte(0x40,OLED_CMD);//--set start line address  
	cat_iic_oled_send_byte(0xB0,OLED_CMD);//--set page address
	cat_iic_oled_send_byte(0x81,OLED_CMD); // contract control
	cat_iic_oled_send_byte(0xFF,OLED_CMD);//--128   
	cat_iic_oled_send_byte(0xA1,OLED_CMD);//set segment remap 
	cat_iic_oled_send_byte(0xA6,OLED_CMD);//--normal / reverse
	cat_iic_oled_send_byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	cat_iic_oled_send_byte(0x3F,OLED_CMD);//--1/32 duty
	cat_iic_oled_send_byte(0xC8,OLED_CMD);//Com scan direction
	cat_iic_oled_send_byte(0xD3,OLED_CMD);//-set display offset
	cat_iic_oled_send_byte(0x00,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xD5,OLED_CMD);//set osc division
	cat_iic_oled_send_byte(0x80,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xD8,OLED_CMD);//set area color mode off
	cat_iic_oled_send_byte(0x05,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	cat_iic_oled_send_byte(0xF1,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xDA,OLED_CMD);//set com pin configuartion
	cat_iic_oled_send_byte(0x12,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xDB,OLED_CMD);//set Vcomh
	cat_iic_oled_send_byte(0x30,OLED_CMD);//
	
	cat_iic_oled_send_byte(0x8D,OLED_CMD);//set charge pump enable
	cat_iic_oled_send_byte(0x14,OLED_CMD);//
	
	cat_iic_oled_send_byte(0xAF,OLED_CMD);//--turn on oled panel
}

void cat_iic_oled_clear(void)  
{  
	cat_uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		cat_iic_oled_send_byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		cat_iic_oled_send_byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		cat_iic_oled_send_byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)cat_iic_oled_send_byte(0,OLED_DATA); 
	} //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void cat_iic_oled_show_char(cat_uint8_t x,cat_uint8_t y,cat_uint8_t chr,cat_uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			cat_iic_oled_set_position(x,y);	
			for(i=0;i<8;i++)
			cat_iic_oled_send_byte(F8X16[c*16+i],OLED_DATA);
			cat_iic_oled_set_position(x,y+1);
			for(i=0;i<8;i++)
			cat_iic_oled_send_byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				cat_iic_oled_set_position(x,y);
				for(i=0;i<6;i++)
				cat_iic_oled_send_byte(F6x8[c][i],OLED_DATA);
				
			}
}

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void cat_iic_oled_show_number(cat_uint8_t x,cat_uint8_t y,cat_uint32_t num,cat_uint8_t len,cat_uint8_t size2)
{         	
	cat_uint8_t t,temp;
	cat_uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				cat_iic_oled_show_char(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	cat_iic_oled_show_char(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 

//显示一个字符号串
void cat_iic_oled_show_string(cat_uint8_t x,cat_uint8_t y,cat_uint8_t *chr,cat_uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		cat_iic_oled_show_char(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

void cat_iic_oled_set_position(unsigned char x, unsigned char y) 
{ 	cat_iic_oled_send_byte(0xb0+y,OLED_CMD);
	cat_iic_oled_send_byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	cat_iic_oled_send_byte((x&0x0f),OLED_CMD); 
}


/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void cat_iic_oled_draw_bmp(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		cat_iic_oled_set_position(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	cat_iic_oled_send_byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

void cat_iic_oled_delay50ms(unsigned int Del_50ms)
{
    cat_delay_ms(50);
}

void cat_iic_oled_delay1ms(unsigned int Del_1ms)
{
    while(Del_1ms--)
    {
        cat_delay_us(1000);
    }
}

/********************************************
// cat_iic_oled_fill_picture
********************************************/
void cat_iic_oled_fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		cat_iic_oled_send_byte(0xb0+m,0);		//page0-page1
		cat_iic_oled_send_byte(0x00,0);		//low column start address
		cat_iic_oled_send_byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				cat_iic_oled_send_byte(fill_Data,1);
			}
	}
}
