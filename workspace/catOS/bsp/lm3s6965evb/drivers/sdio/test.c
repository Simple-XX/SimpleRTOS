
// #include "cat_task.h"
#include "cat_stdio.h"
#include "sdio_sdcard.h"   
//ALIENTEK��ӢSTM32F103������ ʵ��35
//SD�� ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾

//ͨ�����ڴ�ӡSD�������Ϣ
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:CAT_SYS_PRINTF("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:CAT_SYS_PRINTF("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:CAT_SYS_PRINTF("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:CAT_SYS_PRINTF("Card Type:MMC Card\r\n");break;
	}	
  	CAT_SYS_PRINTF("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//������ID
 	CAT_SYS_PRINTF("Card RCA:%d\r\n",SDCardInfo.RCA);								//����Ե�ַ
	CAT_SYS_PRINTF("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//��ʾ����
 	CAT_SYS_PRINTF("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//��ʾ���С
}  

u8 buf[512];
int test_sdio_sdcard(void)
{		
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	
  
 	// Stm32_Clock_Init(9);		//ϵͳʱ������
	// uart_init(72,115200);		//���ڳ�ʼ��Ϊ115200
	// delay_init(72);	   	 		//��ʱ��ʼ�� 
 	// LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	// KEY_Init();					//��ʼ������
	// LCD_Init();			   		//��ʼ��LCD   
 	// my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
 	// POINT_COLOR=RED;			//��������Ϊ��ɫ 
	CAT_SYS_PRINTF("ELITE STM32F103 ^_^\r\n");	
	CAT_SYS_PRINTF("SD CARD TEST\r\n");	
	CAT_SYS_PRINTF("ATOM@ALIENTEK\r\n");
	CAT_SYS_PRINTF("2015/1/20\r\n"); 
	CAT_SYS_PRINTF("KEY0:Read Sector 0\r\n");	   
 	if(0 != SD_Init())//��ⲻ��SD��
	{
		CAT_SYS_PRINTF("SD Card Error!\r\n");			
		CAT_SYS_PRINTF("Please Check! \r\n");

		while(1);
	}
	show_sdcard_info();	//��ӡSD�������Ϣ
	//���SD���ɹ� 											    
	CAT_SYS_PRINTF("SD Card OK    \r\n");
	CAT_SYS_PRINTF("SD Card Size:     MB\r\n");
	CAT_SYS_PRINTF("sdcard_capa=%d\r\n", SDCardInfo.CardCapacity);

	if(SD_ReadDisk(buf,0,1)==0)	//��ȡ0����������
	{	
		CAT_SYS_PRINTF("USART1 Sending Data...\r\n");
		CAT_SYS_PRINTF("SECTOR 0 DATA:\r\n\r\n");
		for(sd_size=0;sd_size<512;sd_size++)CAT_SYS_PRINTF("%d ",buf[sd_size]);//��ӡ0��������    	   
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
