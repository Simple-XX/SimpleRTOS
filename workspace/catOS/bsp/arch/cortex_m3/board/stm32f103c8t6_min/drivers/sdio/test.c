
// #include "cat_task.h"
#include "cat_stdio.h"
#include "sdio_sdcard.h"   
//ALIENTEK精英STM32F103开发板 实验35
//SD卡 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:CAT_SYS_PRINTF("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:CAT_SYS_PRINTF("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:CAT_SYS_PRINTF("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:CAT_SYS_PRINTF("Card Type:MMC Card\r\n");break;
	}	
  	CAT_SYS_PRINTF("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	CAT_SYS_PRINTF("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	CAT_SYS_PRINTF("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	CAT_SYS_PRINTF("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}  

u8 buf[512];
int test_sdio_sdcard(void)
{		
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	
  
 	// Stm32_Clock_Init(9);		//系统时钟设置
	// uart_init(72,115200);		//串口初始化为115200
	// delay_init(72);	   	 		//延时初始化 
 	// LED_Init();		  			//初始化与LED连接的硬件接口
	// KEY_Init();					//初始化按键
	// LCD_Init();			   		//初始化LCD   
 	// my_mem_init(SRAMIN);		//初始化内部内存池
 	// POINT_COLOR=RED;			//设置字体为红色 
	CAT_SYS_PRINTF("ELITE STM32F103 ^_^\r\n");	
	CAT_SYS_PRINTF("SD CARD TEST\r\n");	
	CAT_SYS_PRINTF("ATOM@ALIENTEK\r\n");
	CAT_SYS_PRINTF("2015/1/20\r\n"); 
	CAT_SYS_PRINTF("KEY0:Read Sector 0\r\n");	   
 	if(0 != SD_Init())//检测不到SD卡
	{
		CAT_SYS_PRINTF("SD Card Error!\r\n");			
		CAT_SYS_PRINTF("Please Check! \r\n");

		while(1);
	}
	show_sdcard_info();	//打印SD卡相关信息
	//检测SD卡成功 											    
	CAT_SYS_PRINTF("SD Card OK    \r\n");
	CAT_SYS_PRINTF("SD Card Size:     MB\r\n");
	CAT_SYS_PRINTF("sdcard_capa=%d\r\n", SDCardInfo.CardCapacity);

	if(SD_ReadDisk(buf,0,1)==0)	//读取0扇区的内容
	{	
		CAT_SYS_PRINTF("USART1 Sending Data...\r\n");
		CAT_SYS_PRINTF("SECTOR 0 DATA:\r\n\r\n");
		for(sd_size=0;sd_size<512;sd_size++)CAT_SYS_PRINTF("%d ",buf[sd_size]);//打印0扇区数据    	   
		CAT_SYS_PRINTF("\r\nDATA ENDED\r\n\r\n");
		CAT_SYS_PRINTF("USART1 Send Data Over!\r\n");
	}   

}

#if (CATOS_ENABLE_CAT_SHELL == 1)
#include "cat_shell.h"

void *do_test_sdio(void *arg)
{
    (void)arg;
	
	test_sdio_sdcard();
		
	return NULL;
}
CAT_DECLARE_CMD(test_sdio, test sdio, do_test_sdio);

#endif
