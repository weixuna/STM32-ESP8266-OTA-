#include "ota.h"
#include <string.h>
#include <stdlib.h>
#include "usart.h"
#include "esp8266.h"
#include "iap.h"
#include "delay.h"
#include "tencent_mqtt.h"
#include "md5.h"


/*
函数功能：初始化OTA,连接MQTT服务器
参数：无
返回值：无
*/
void MQTT_OTA_Init(void)
{
	ESP_UnvarnishedMode_Init(WIFI_SSID,WIFI_PWD,SERVER_IP,SERVER_PORT);//连接TCP服务器并进入透传模式
	if(MQTT_Connect(CLIENT_ID,USER_NAME,PASSWORD,KEEPALIVE_TIME))//登录MQTT服务器
    {
        printf("MQTT Server Connect Failure\r\n");
    }
	else 
	{
		printf("MQTT Server Connect success\r\n");	
		if(MQTT_Subscribe_Topic(OTA_SUB_TOPIC,REQ_QOS_0,1))//订阅OTA下行消息
		{
			printf("OTA Topic Subscribe Failure\r\n");
		}
		else
		{
			printf("OTA Topic Subscribe success\r\n");
		}			
	}
}


/*
函数功能：上报固件版本以获取OTA固件
参数：无
返回值：无
*/
void Get_OTA_Firmware(ota_info_t *ota_info)
{
    char buff[100];
	usart3.flag=0;
	usart3.cnt=0;
	memset((char *)usart3.rx_buff,0,USART_RX_MAXLEN);//清空接收缓冲区
    snprintf(buff,sizeof(buff),OTA_VERSION_UPLOAD_TEMPLATE,ota_info->current_version);
	MQTT_Publish(OTA_PUB_TOPIC,(uint8_t*)buff);//上报固件版本
}

/*
函数功能：从接收buff中获取ota信息
参数：char *rev_buff,ota_info_t *ota_info
返回值：无
*/
uint8_t Get_OTA_Info(char *rev_buff,ota_info_t *ota_info)
{
	char *data_pt_start=NULL;
	char *data_pt_end=NULL;
	char buff[100];
    uint8_t get_state=0;
	if(strstr(rev_buff,"\"file_size\""))//获取固件大小
	{
		data_pt_start=strstr(rev_buff,"\"file_size\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(buff,data_pt_end-data_pt_start-11,"%s",data_pt_start+12);
		ota_info->file_size=atoi(buff);
        get_state=1;
	}
	if(strstr(rev_buff,"\"md5sum\""))//获取MD5值
	{
		data_pt_start=strstr(rev_buff,"\"md5sum\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(ota_info->md5sum,data_pt_end-data_pt_start-10,"%s",data_pt_start+10);
        get_state=1;
	}
	if(strstr(rev_buff,"\"url\""))//获取下载URL
	{
		data_pt_start=strstr(rev_buff,"\"url\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(ota_info->url,data_pt_end-data_pt_start-7,"%s",data_pt_start+7);
        get_state=1;
	}
	if(strstr(ota_info->url,"https://"))//获取OTA主机域名
	{
		data_pt_start=strstr(rev_buff,"https://");
		data_pt_end=strstr(data_pt_start+8,"/");
		snprintf(ota_info->host,data_pt_end-data_pt_start-7,"%s",data_pt_start+8);
		get_state=1;
	}

	if(strstr(rev_buff,"\"version\""))//获取固件版本
	{
		data_pt_start=strstr(rev_buff,"\"version\"");
		data_pt_end=strstr(data_pt_start,"}");
		snprintf(ota_info->ota_version,data_pt_end-data_pt_start-11,"%s",data_pt_start+11);
	}
    return get_state;
}

/*
函数功能：开始下载OTA固件
参数：ota_info_t *ota_info
返回值：u8
*/
static u8 DownLoad_OTA_Firmware(ota_info_t *ota_info)
{
	int i;
	char *data_pt_start=NULL;
	u8 time_cnt=0;
	u8 data_packet_num=0;
	if(ota_info->file_size<=APP_FLASH_SIZE)//判断固件是否超过APP分区大小
	{
		if(!ESP_UnvarnishedMode_Init(WIFI_SSID,WIFI_PWD,ota_info->host,80))//连接OTA固件下载服务器
		{
			//计算分包下载次数
			if(ota_info->file_size%1024==0) data_packet_num=(ota_info->file_size/1024)-1;
            else  data_packet_num=ota_info->file_size/1024;
			for(i=0;i<=data_packet_num;i++)//分包下载
			{
					usart3.flag=RX_BUFF_EMPTY;//接收标志位清零
					usart3.cnt=0;
					memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//清空接收缓冲区
					snprintf(ota_info->http_request,sizeof(ota_info->http_request),
                        "GET http://%s HTTP/1.1\r\nHost:%s\r\nRange:bytes=%d-%d\r\n\r\n",
                    ota_info->url+8,ota_info->host,i*1024,(i+1)*1024-1);//序列化HTTP请求报文
					USARTx_Send_String(USART3,ota_info->http_request);//发起HTTP下载固件请求
					while(usart3.flag==RX_BUFF_EMPTY)//等待接收数据完成
					{
						Delay_Ms(100);
						if(++time_cnt>=30) return HTTP_REQUEST_TIMEOUT;//HTTP请求超时
					}
					time_cnt=0;
					if(strstr((char*)usart3.rx_buff,"\r\n\r\n"))//寻找数据包头部
					{
						data_pt_start=strstr((char*)usart3.rx_buff,"\r\n\r\n");
                        iap_write_flash(FLASH_OTA_ADDR+i*1024,(u8 *)data_pt_start+4,1024);
						printf("总大小:%d Bytes,写入第%d包,%d-%d Bytes已写入\r\n",ota_info->file_size,i+1,i*1024,(i+1)*1024-1);
					}
					else
                    {
                        printf("未找到头部数据\r\n");
                        return GET_OTA_DATA_PACKET_FAIL;//解析OTA数据包失败
                    }
			}
            printf("固件下载成功\r\n");
			return FIRMWARE_DOWNLOAD_SUCC;//固件下载成功
		}
		else 
        {
            printf("进入透传模式失败\r\n");
            return SERVER_CONNECT_FAIL;//服务器连接失败
        }
	}
	else
	{
		printf("OTA固件大小超出范围\r\n");
		return OTA_PACKET_SIZE_OVERFLOW;
	}
	
}

/*
函数功能：开始OTA更新
参数：ota_info_t *ota_info,char *check_md5
返回值：无
*/
void Start_OTA_Update(ota_info_t *ota_info,char *check_md5)
{
	if(ota_info->file_size!=0)
	{
		printf("Firmware Size:%d Bytes,Downloading...\r\n",ota_info->file_size);
		if(DownLoad_OTA_Firmware(ota_info)==FIRMWARE_DOWNLOAD_SUCC)//开始下载固件
		{
			Compute_string_md5((u8*)FLASH_OTA_ADDR,ota_info->file_size, check_md5);//计算APP2 MD5值
			printf("OTA Firmware MD5:%s\r\nFirmware Download Success\r\n",check_md5);
		}
		else printf("Firmware Download Failure\r\n");
        printf("Login MQTT Server...\r\n");
        MQTT_OTA_Init();//重新登录MQTT服务器
	}
    else printf("The updated firmware was not obtained\r\n");//未获取到更新固件
	
}

/*
函数功能：将下载的固件更新到APP1中
参数：ota_info_t *ota_info,char *check_md5
返回值：无
*/
void Update_FirmwareToApp(ota_info_t *ota_info,char *check_md5)
{
	if(strcmp(ota_info->md5sum,check_md5)==0)//比较MD5值
	{
		printf("OTA Firmware MD5 OK\r\nUpdate APP...\r\n");
        iap_write_flash(FLASH_APP_ADDR,(u8*)FLASH_OTA_ADDR,ota_info->file_size);
		Compute_string_md5((u8*)FLASH_APP_ADDR,ota_info->file_size, check_md5);//计算APP MD5值
		printf("APP MD5:%s\r\n",check_md5);
		if((strcmp(ota_info->md5sum,check_md5)==0)&&(((*(vu32*)(FLASH_APP_ADDR+4))&0xFF000000)==0x08000000))//判断是否为0X08XXXXXX.
		{
			printf("APP MD5 OK\r\n");
			ota_info->file_size=0;//清除固件
            iap_erase_all_bkp_sector();//擦除下载的OTA固件
            memcpy(ota_info->current_version,ota_info->ota_version,sizeof(ota_info->ota_version));
            USART_ITConfig(USART3,USART_IT_RXNE, DISABLE);//关闭USART3接收缓冲区非空中断
            Get_OTA_Firmware(ota_info);//上报固件版本,通知服务器OTA升级完成
            Write_CurrentFirmwareVersion(ota_info->ota_version);//写入APP版本
            printf("Jump Run APP...\r\n");
			iap_load_app(FLASH_APP_ADDR);//执行FLASH APP代码
            
		}
		else printf("Update APP Failure\r\n");
	}
	else printf("OTA Firmware MD5 Error\r\n");
}

/*
函数功能：读取当前版本信息
参数：char *current_version
返回值：无
*/
void Read_CurrentFirmwareVersion(char *current_version)
{
    uint8_t *version_pt=(uint8_t*)FIRMWARE_VERSION_STORE_ADDR;
    for(uint8_t i=0;i<FIRMWARE_VERSION_SIZE;i++)
    {
        if(*(version_pt+i)==0xFF) continue;
        *(current_version+i)=*(version_pt+i);
    }
}

/*
函数功能：写入当前版本信息
参数：char *current_version
返回值：无
*/
void Write_CurrentFirmwareVersion(char *current_version)
{
    iap_write_flash(FIRMWARE_VERSION_STORE_ADDR,(uint8_t*)current_version,FIRMWARE_VERSION_SIZE);
}

