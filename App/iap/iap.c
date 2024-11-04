#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "iap.h"


iapfun jump2app;   

void iap_erase_all_bkp_sector(void)
{
    uint16_t erase_sector_num;//Ҫ��д��������
    uint16_t erase_sector_start;//��������ʼ����

    erase_sector_num=OTA_FLASH_SIZE/SECTOR_SIZE;
    if(OTA_FLASH_SIZE%SECTOR_SIZE!=0) erase_sector_num++;
    erase_sector_start=(FLASH_OTA_ADDR-FLASH_START_ADDR)/SECTOR_SIZE;//������ʼ����
    if(erase_sector_start>MAX_SECTOR_NUMBER) return;   
    FLASH_Unlock();//����FLASH
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for(uint16_t i=erase_sector_start;i<(erase_sector_start+erase_sector_num);i++)
    {
        FLASH_ErasePage(FLASH_START_ADDR+i*SECTOR_SIZE);//��������
    }
    FLASH_Lock();//FLASH����
}

/*
�������ܣ�������д��flash
������
����ֵ����
*/


void iap_write_flash(uint32_t flash_addr,uint8_t *write_buff,uint16_t write_size)
{
    uint16_t erase_sector_num;//Ҫ��д��������
    uint16_t erase_sector_start;//��������ʼ����
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
    
    erase_sector_start=(flash_addr-FLASH_START_ADDR)/SECTOR_SIZE;//������ʼ����
    if(erase_sector_start>MAX_SECTOR_NUMBER) return;
    FLASH_Unlock();//����FLASH
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    for(uint16_t i=erase_sector_start;i<(erase_sector_start+erase_sector_num);i++)
    {
        FLASH_ErasePage(FLASH_START_ADDR+i*SECTOR_SIZE);//��������
    }
    data=(uint32_t*)write_buff;
    for(uint32_t i=0;i<write_size;i+=4)
    {
        FLASH_ProgramWord(flash_addr+i,*data);
        data++;
    }
    
    FLASH_Lock();//FLASH����
}



//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}		 














