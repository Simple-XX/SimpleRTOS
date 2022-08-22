#ifndef __OLED_H
#define __OLED_H 

//#include "sys.h"
#include "stdlib.h"	
#include "stm32f1xx.h"

#include "catos.h"
#include "catos_types.h"

#define OLED_DELAY_MS(x) cat_sp_task_delay(x)

//-----------------OLED�˿ڶ���---------------- 

#define OLED_SCL_PIN              GPIO_PIN_4                
#define OLED_SCL_PORT            GPIOA                     
#define OLED_SCL_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

#define OLED_SDA_PIN              GPIO_PIN_5                
#define OLED_SDA_PORT            GPIOA                     
#define OLED_SDA_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

#define OLED_RES_PIN              GPIO_PIN_6                
#define OLED_RES_PORT            GPIOB                     
#define OLED_RES_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

#define OLED_SCL_Clr() HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN, GPIO_PIN_RESET)//SCL
#define OLED_SCL_Set() HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN, GPIO_PIN_SET)

#define OLED_SDA_Clr() HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN, GPIO_PIN_RESET)//DIN
#define OLED_SDA_Set() HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN, GPIO_PIN_SET)

#define OLED_RES_Clr() HAL_GPIO_WritePin(OLED_RES_PORT,OLED_RES_PIN, GPIO_PIN_RESET)//RES
#define OLED_RES_Set() HAL_GPIO_WritePin(OLED_RES_PORT,OLED_RES_PIN, GPIO_PIN_SET)


#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

void OLED_ClearPoint(uint8_t x,uint8_t y);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(uint8_t dat);
void OLED_WR_Byte(uint8_t dat,uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowChar6x8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);
void OLED_Init(void);

#endif

