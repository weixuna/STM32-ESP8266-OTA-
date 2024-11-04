#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "iap.h"
#include "ota.h"


int main(void)
{
    ota_info_t ota_info;
    uint8_t key_val;
    char check_md5[100];
    Delay_Init();//延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
    KEY_Init();//按键初始化
    if(Get_Key_Value(KEY_SINGLE_MODE)==KEY_S2_PRESS)//运行BootLoader程序
    {
        Usarts_Init();//初始化串口
        printf("Run BootLoader...\r\n");
        MQTT_OTA_Init();//登录MQTT服务器
        Read_CurrentFirmwareVersion(ota_info.current_version);//读取当前APP版本
        if(strlen(ota_info.current_version)==0)//从未写入APP版本信息
        {
            printf("The APP version was not written,Writting...\r\n");
            memcpy(ota_info.current_version,"1.0",4);
            Write_CurrentFirmwareVersion(ota_info.current_version);//写入APP版本
        }
		printf("\r\n****************************\r\nCurrent Firmware Version:%s\r\nS3-->Get OTA Firmware\r\nS4-->Download OTA Firmware\r\nS5-->Update APP\r\n****************************\r\n\r\n",ota_info.current_version);
    }
    else //运行APP程序
    {
        iap_load_app(FLASH_APP_ADDR);//运行APP
    }
    while(1)
    {
       if(usart3.flag==RX_BUFF_NO_EMPTY)
       {
           for(uint16_t i=0;i<=usart3.cnt;i++)
           {
                if(usart3.rx_buff[i]=='\0')	usart3.rx_buff[i]='0';
           }
           if(strstr((char*)usart3.rx_buff,"report_version_rsp")==NULL)
           {
               if(Get_OTA_Info((char*)usart3.rx_buff,&ota_info))
               {
                    printf("\r\n****************************\r\n  OTA Firmware Infomation  \r\nfile_size:%d\r\nmd5sum:%s\r\nurl:%s\r\nhost:%s\r\nversion:%s\r\n****************************\r\n\r\n",
                    ota_info.file_size,ota_info.md5sum,ota_info.url,ota_info.host,ota_info.ota_version);
               }
               else 
               {
                    printf("None update firmware\r\n");
               }
           }
           else printf("Upload firmware version success\r\n");
           
            
            usart3.flag=RX_BUFF_EMPTY;
            usart3.cnt=0;
            memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//清空接收缓冲区
       }
       key_val=Get_Key_Value(KEY_SINGLE_MODE);
       if(key_val==KEY_S3_PRESS)
       {
           Get_OTA_Firmware(&ota_info);//向服务器发起更新请求
       }
       else if(key_val==KEY_S4_PRESS)
       {
            Start_OTA_Update(&ota_info,check_md5);//下载OTA固件
       }
       else if(key_val==KEY_S5_PRESS)
       {
            Update_FirmwareToApp(&ota_info,check_md5);//更新APP程序
       }
    }
}

