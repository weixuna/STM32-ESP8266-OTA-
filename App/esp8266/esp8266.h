#ifndef	_ESP8266_H_
#define	_ESP8266_H_

#include "stm32f10x.h"
#define ERR_REPEAT_SEND_CNT 10
//工作模式
#define Station 		1
#define SoftAP	 		2
#define SoftAP_Station 	3

//加密方式
#define OPEN 			0
#define WPA_PSK 		2
#define WPA2_PSK 		3
#define WPA_WPA2_PSK	4

//连接状态
#define ESP_DISCONNECT  0
#define ESP_CONNECT 	1

#define SERVER_IP "71248IVBSR.iotcloud.tencentdevices.com"
#define SERVER_PORT 1883

#define WIFI_SSID "test123"
#define WIFI_PWD  "12344321"


u8 ESP_SendCmd(char *Cmd,const char *check_str,u16 time_out,u16 repeat_num);
u8 ESP_Set_WorkMode(u8 mode);
void ESP_Reset(void);

u8 ESP_UnvarnishedMode_Init(char *ssid,char *pwd,char *server_ip,u16 port);
u8 ESP_Exit_Unvarnished(void);
u8 ESP_Connect_Wifi(char *ssid,char *pwd);
u8 ESP_Connect_Server(char *server_ip,u16 port);
#endif
