#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

#define FLASH_SIZE (64*1024)//64K
#define SECTOR_SIZE 1024 //扇区大小
#define MAX_SECTOR_NUMBER  ((FLASH_SIZE/SECTOR_SIZE)-1) //最大扇区号

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   
//保留0X08000000~0X08003C00的空间为Bootloader使用	  
#define FLASH_START_ADDR    0x08000000  //FLASH起始地址
#define FLASH_APP_ADDR		0x08004000  	//应用程序起始地址(存放在FLASH)
#define FLASH_OTA_ADDR		0x0800A000  	//存放固件,用于固件下载

#define APP_FLASH_SIZE (24*1024)  //24K
#define OTA_FLASH_SIZE (24*1024)  //24K
void iap_load_app(u32 appxaddr);			//跳转到APP程序执行
void iap_erase_all_bkp_sector(void);
void iap_write_flash(uint32_t flash_addr,uint8_t *write_buff,uint16_t write_size);
#endif







































