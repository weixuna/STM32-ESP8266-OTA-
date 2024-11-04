#ifndef _OTA_H_
#define _OTA_H_

#include "sys.h"

#define BOOTLOADER_START_ADDR   0x8000000  //BootLoader��ʼ��ַ
#define BOOTLOADER_SIZE         0x3C00  //BootLoader��С
#define FIRMWARE_VERSION_STORE_ADDR  (BOOTLOADER_START_ADDR+BOOTLOADER_SIZE)   //�洢�̼���Ϣ��FLASH��ַ
#define FIRMWARE_VERSION_SIZE   16 //�洢�̼���Ϣ���ֽ���
enum
{
	HTTP_REQUEST_TIMEOUT,
	GET_OTA_DATA_PACKET_FAIL,
	SERVER_CONNECT_FAIL,
	FIRMWARE_DOWNLOAD_SUCC,
	OTA_PACKET_SIZE_OVERFLOW
};


typedef struct ota_info
{
	u32 file_size;//�ļ���С
	char md5sum[100];//�������·���md5
	char url[512];//����URL
	char host[256];//OTA����������
	char http_request[512];//HTTP������
    char current_version[FIRMWARE_VERSION_SIZE];//��ǰ�汾��Ϣ
	char ota_version[FIRMWARE_VERSION_SIZE];//OTA�汾��Ϣ
}ota_info_t;


uint8_t Get_OTA_Info(char *rev_buff,ota_info_t *ota_info);
void MQTT_OTA_Init(void); //��ʼ��MQTT
void Get_OTA_Firmware(ota_info_t *ota_info); //�ϱ��̼��汾�Ի�ȡOTA�̼�
void Start_OTA_Update(ota_info_t *ota_info,char *check_md5); //��ʼOTA����
void Update_FirmwareToApp(ota_info_t *ota_info,char *check_md5); //���صĹ̼����µ�APP��
void Read_CurrentFirmwareVersion(char *current_version);  //��ȡ��ǰ�汾
void Write_CurrentFirmwareVersion(char *current_version);   //д�뵱ǰ�汾��Ϣ
#endif
