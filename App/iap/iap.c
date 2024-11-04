#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "iap.h"


iapfun jump2app;   

void iap_erase_all_bkp_sector(void)
{
    uint16_t erase_sector_num;//要擦写的扇区数
    uint16_t erase_sector_start;//擦除的起始扇区

    erase_sector_num=OTA_FLASH_SIZE/SECTOR_SIZE;
    if(OTA_FLASH_SIZE%SECTOR_SIZE!=0) erase_sector_num++;
    erase_sector_start=(FLASH_OTA_ADDR-FLASH_START_ADDR)/SECTOR_SIZE;//计算起始扇区
    if(erase_sector_start>MAX_SECTOR_NUMBER) return;   
    FLASH_Unlock();//解锁FLASH
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for(uint16_t i=erase_sector_start;i<(erase_sector_start+erase_sector_num);i++)
    {
        FLASH_ErasePage(FLASH_START_ADDR+i*SECTOR_SIZE);//擦除扇区
    }
    FLASH_Lock();//FLASH上锁
}

/*
函数功能：按扇区写入flash
参数：
返回值：无
*/


void iap_write_flash(uint32_t flash_addr,uint8_t *write_buff,uint16_t write_size)
{
    uint16_t erase_sector_num;//要擦写的扇区数
    uint16_t erase_sector_start;//擦除的起始扇区
    uint32_t  *data=NULL;
    
    if(write_size<SECTOR_SIZE)
    {
        erase_sector_num=1;
    }
    else
    {
        erase_sector_num=write_size/SECTOR_SIZE;
        if(write_size%SECTOR_SIZE!=0) erase_sector_num++;
    }
    
    erase_sector_start=(flash_addr-FLASH_START_ADDR)/SECTOR_SIZE;//计算起始扇区
    if(erase_sector_start>MAX_SECTOR_NUMBER) return;
    FLASH_Unlock();//解锁FLASH
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for(uint16_t i=erase_sector_start;i<(erase_sector_start+erase_sector_num);i++)
    {
        FLASH_ErasePage(FLASH_START_ADDR+i*SECTOR_SIZE);//擦除扇区
    }
    data=(uint32_t*)write_buff;
    for(uint32_t i=0;i<write_size;i+=4)
    {
        FLASH_ProgramWord(flash_addr+i,*data);
        data++;
    }
    
    FLASH_Lock();//FLASH上锁
}



//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}		 














