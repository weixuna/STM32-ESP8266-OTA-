#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

#define FLASH_SIZE (64*1024)//64K
#define SECTOR_SIZE 1024 //������С
#define MAX_SECTOR_NUMBER  ((FLASH_SIZE/SECTOR_SIZE)-1) //���������

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   
//����0X08000000~0X08003C00�Ŀռ�ΪBootloaderʹ��	  
#define FLASH_START_ADDR    0x08000000  //FLASH��ʼ��ַ
#define FLASH_APP_ADDR		0x08004000  	//Ӧ�ó�����ʼ��ַ(�����FLASH)
#define FLASH_OTA_ADDR		0x0800A000  	//��Ź̼�,���ڹ̼�����

#define APP_FLASH_SIZE (24*1024)  //24K
#define OTA_FLASH_SIZE (24*1024)  //24K
void iap_load_app(u32 appxaddr);			//��ת��APP����ִ��
void iap_erase_all_bkp_sector(void);
void iap_write_flash(uint32_t flash_addr,uint8_t *write_buff,uint16_t write_size);
#endif







































