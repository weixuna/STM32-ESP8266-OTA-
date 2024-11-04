#ifndef _OTA_H_
#define _OTA_H_

#include "sys.h"

#define BOOTLOADER_START_ADDR   0x8000000  //BootLoader起始地址
#define BOOTLOADER_SIZE         0x3C00  //BootLoader大小
#define FIRMWARE_VERSION_STORE_ADDR  (BOOTLOADER_START_ADDR+BOOTLOADER_SIZE)   //存储固件信息的FLASH地址
#define FIRMWARE_VERSION_SIZE   16 //存储固件信息的字节数
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
	u32 file_size;//文件大小
	char md5sum[100];//服务器下发的md5
	char url[512];//下载URL
	char host[256];//OTA服务器域名
	char http_request[512];//HTTP请求报文
    char current_version[FIRMWARE_VERSION_SIZE];//当前版本信息
	char ota_version[FIRMWARE_VERSION_SIZE];//OTA版本信息
}ota_info_t;


uint8_t Get_OTA_Info(char *rev_buff,ota_info_t *ota_info);
void MQTT_OTA_Init(void); //初始化MQTT
void Get_OTA_Firmware(ota_info_t *ota_info); //上报固件版本以获取OTA固件
void Start_OTA_Update(ota_info_t *ota_info,char *check_md5); //开始OTA更新
void Update_FirmwareToApp(ota_info_t *ota_info,char *check_md5); //下载的固件更新到APP区
void Read_CurrentFirmwareVersion(char *current_version);  //读取当前版本
void Write_CurrentFirmwareVersion(char *current_version);   //写入当前版本信息
#endif
