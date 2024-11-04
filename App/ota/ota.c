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
�������ܣ���ʼ��OTA,����MQTT������
��������
����ֵ����
*/
void MQTT_OTA_Init(void)
{
	ESP_UnvarnishedMode_Init(WIFI_SSID,WIFI_PWD,SERVER_IP,SERVER_PORT);//����TCP������������͸��ģʽ
	if(MQTT_Connect(CLIENT_ID,USER_NAME,PASSWORD,KEEPALIVE_TIME))//��¼MQTT������
    {
        printf("MQTT Server Connect Failure\r\n");
    }
	else 
	{
		printf("MQTT Server Connect success\r\n");	
		if(MQTT_Subscribe_Topic(OTA_SUB_TOPIC,REQ_QOS_0,1))//����OTA������Ϣ
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
�������ܣ��ϱ��̼��汾�Ի�ȡOTA�̼�
��������
����ֵ����
*/
void Get_OTA_Firmware(ota_info_t *ota_info)
{
    char buff[100];
	usart3.flag=0;
	usart3.cnt=0;
	memset((char *)usart3.rx_buff,0,USART_RX_MAXLEN);//��ս��ջ�����
    snprintf(buff,sizeof(buff),OTA_VERSION_UPLOAD_TEMPLATE,ota_info->current_version);
	MQTT_Publish(OTA_PUB_TOPIC,(uint8_t*)buff);//�ϱ��̼��汾
}

/*
�������ܣ��ӽ���buff�л�ȡota��Ϣ
������char *rev_buff,ota_info_t *ota_info
����ֵ����
*/
uint8_t Get_OTA_Info(char *rev_buff,ota_info_t *ota_info)
{
	char *data_pt_start=NULL;
	char *data_pt_end=NULL;
	char buff[100];
    uint8_t get_state=0;
	if(strstr(rev_buff,"\"file_size\""))//��ȡ�̼���С
	{
		data_pt_start=strstr(rev_buff,"\"file_size\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(buff,data_pt_end-data_pt_start-11,"%s",data_pt_start+12);
		ota_info->file_size=atoi(buff);
        get_state=1;
	}
	if(strstr(rev_buff,"\"md5sum\""))//��ȡMD5ֵ
	{
		data_pt_start=strstr(rev_buff,"\"md5sum\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(ota_info->md5sum,data_pt_end-data_pt_start-10,"%s",data_pt_start+10);
        get_state=1;
	}
	if(strstr(rev_buff,"\"url\""))//��ȡ����URL
	{
		data_pt_start=strstr(rev_buff,"\"url\"");
		data_pt_end=strstr(data_pt_start,",");
		snprintf(ota_info->url,data_pt_end-data_pt_start-7,"%s",data_pt_start+7);
        get_state=1;
	}
	if(strstr(ota_info->url,"https://"))//��ȡOTA��������
	{
		data_pt_start=strstr(rev_buff,"https://");
		data_pt_end=strstr(data_pt_start+8,"/");
		snprintf(ota_info->host,data_pt_end-data_pt_start-7,"%s",data_pt_start+8);
		get_state=1;
	}

	if(strstr(rev_buff,"\"version\""))//��ȡ�̼��汾
	{
		data_pt_start=strstr(rev_buff,"\"version\"");
		data_pt_end=strstr(data_pt_start,"}");
		snprintf(ota_info->ota_version,data_pt_end-data_pt_start-11,"%s",data_pt_start+11);
	}
    return get_state;
}

/*
�������ܣ���ʼ����OTA�̼�
������ota_info_t *ota_info
����ֵ��u8
*/
static u8 DownLoad_OTA_Firmware(ota_info_t *ota_info)
{
	int i;
	char *data_pt_start=NULL;
	u8 time_cnt=0;
	u8 data_packet_num=0;
	if(ota_info->file_size<=APP_FLASH_SIZE)//�жϹ̼��Ƿ񳬹�APP������С
	{
		if(!ESP_UnvarnishedMode_Init(WIFI_SSID,WIFI_PWD,ota_info->host,80))//����OTA�̼����ط�����
		{
			//����ְ����ش���
			if(ota_info->file_size%1024==0) data_packet_num=(ota_info->file_size/1024)-1;
            else  data_packet_num=ota_info->file_size/1024;
			for(i=0;i<=data_packet_num;i++)//�ְ�����
			{
					usart3.flag=RX_BUFF_EMPTY;//���ձ�־λ����
					usart3.cnt=0;
					memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//��ս��ջ�����
					snprintf(ota_info->http_request,sizeof(ota_info->http_request),
                        "GET http://%s HTTP/1.1\r\nHost:%s\r\nRange:bytes=%d-%d\r\n\r\n",
                    ota_info->url+8,ota_info->host,i*1024,(i+1)*1024-1);//���л�HTTP������
					USARTx_Send_String(USART3,ota_info->http_request);//����HTTP���ع̼�����
					while(usart3.flag==RX_BUFF_EMPTY)//�ȴ������������
					{
						Delay_Ms(100);
						if(++time_cnt>=30) return HTTP_REQUEST_TIMEOUT;//HTTP����ʱ
					}
					time_cnt=0;
					if(strstr((char*)usart3.rx_buff,"\r\n\r\n"))//Ѱ�����ݰ�ͷ��
					{
						data_pt_start=strstr((char*)usart3.rx_buff,"\r\n\r\n");
                        iap_write_flash(FLASH_OTA_ADDR+i*1024,(u8 *)data_pt_start+4,1024);
						printf("�ܴ�С:%d Bytes,д���%d��,%d-%d Bytes��д��\r\n",ota_info->file_size,i+1,i*1024,(i+1)*1024-1);
					}
					else
                    {
                        printf("δ�ҵ�ͷ������\r\n");
                        return GET_OTA_DATA_PACKET_FAIL;//����OTA���ݰ�ʧ��
                    }
			}
            printf("�̼����سɹ�\r\n");
			return FIRMWARE_DOWNLOAD_SUCC;//�̼����سɹ�
		}
		else 
        {
            printf("����͸��ģʽʧ��\r\n");
            return SERVER_CONNECT_FAIL;//����������ʧ��
        }
	}
	else
	{
		printf("OTA�̼���С������Χ\r\n");
		return OTA_PACKET_SIZE_OVERFLOW;
	}
	
}

/*
�������ܣ���ʼOTA����
������ota_info_t *ota_info,char *check_md5
����ֵ����
*/
void Start_OTA_Update(ota_info_t *ota_info,char *check_md5)
{
	if(ota_info->file_size!=0)
	{
		printf("Firmware Size:%d Bytes,Downloading...\r\n",ota_info->file_size);
		if(DownLoad_OTA_Firmware(ota_info)==FIRMWARE_DOWNLOAD_SUCC)//��ʼ���ع̼�
		{
			Compute_string_md5((u8*)FLASH_OTA_ADDR,ota_info->file_size, check_md5);//����APP2 MD5ֵ
			printf("OTA Firmware MD5:%s\r\nFirmware Download Success\r\n",check_md5);
		}
		else printf("Firmware Download Failure\r\n");
        printf("Login MQTT Server...\r\n");
        MQTT_OTA_Init();//���µ�¼MQTT������
	}
    else printf("The updated firmware was not obtained\r\n");//δ��ȡ�����¹̼�
	
}

/*
�������ܣ������صĹ̼����µ�APP1��
������ota_info_t *ota_info,char *check_md5
����ֵ����
*/
void Update_FirmwareToApp(ota_info_t *ota_info,char *check_md5)
{
	if(strcmp(ota_info->md5sum,check_md5)==0)//�Ƚ�MD5ֵ
	{
		printf("OTA Firmware MD5 OK\r\nUpdate APP...\r\n");
        iap_write_flash(FLASH_APP_ADDR,(u8*)FLASH_OTA_ADDR,ota_info->file_size);
		Compute_string_md5((u8*)FLASH_APP_ADDR,ota_info->file_size, check_md5);//����APP MD5ֵ
		printf("APP MD5:%s\r\n",check_md5);
		if((strcmp(ota_info->md5sum,check_md5)==0)&&(((*(vu32*)(FLASH_APP_ADDR+4))&0xFF000000)==0x08000000))//�ж��Ƿ�Ϊ0X08XXXXXX.
		{
			printf("APP MD5 OK\r\n");
			ota_info->file_size=0;//����̼�
            iap_erase_all_bkp_sector();//�������ص�OTA�̼�
            memcpy(ota_info->current_version,ota_info->ota_version,sizeof(ota_info->ota_version));
            USART_ITConfig(USART3,USART_IT_RXNE, DISABLE);//�ر�USART3���ջ������ǿ��ж�
            Get_OTA_Firmware(ota_info);//�ϱ��̼��汾,֪ͨ������OTA�������
            Write_CurrentFirmwareVersion(ota_info->ota_version);//д��APP�汾
            printf("Jump Run APP...\r\n");
			iap_load_app(FLASH_APP_ADDR);//ִ��FLASH APP����
            
		}
		else printf("Update APP Failure\r\n");
	}
	else printf("OTA Firmware MD5 Error\r\n");
}

/*
�������ܣ���ȡ��ǰ�汾��Ϣ
������char *current_version
����ֵ����
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
�������ܣ�д�뵱ǰ�汾��Ϣ
������char *current_version
����ֵ����
*/
void Write_CurrentFirmwareVersion(char *current_version)
{
    iap_write_flash(FIRMWARE_VERSION_STORE_ADDR,(uint8_t*)current_version,FIRMWARE_VERSION_SIZE);
}

