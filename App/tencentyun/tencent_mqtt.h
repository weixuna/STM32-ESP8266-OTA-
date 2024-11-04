#ifndef _TENCENT_MQTT_H_
#define	_TENCENT_MQTT_H_

#include "stm32f10x.h"

#define CLIENT_ID "ZD0FYH1ONPtest"
#define USER_NAME	"ZD0FYH1ONPtest;12010126;8LG0Z;1649303686"
#define PASSWORD	"a18ee5dd7f068d85924806896e3205fa314b09a2;hmacsha1"
#define KEEPALIVE_TIME 1200

#define OTA_PUB_TOPIC "$ota/report/ZD0FYH1ONP/test"
#define OTA_SUB_TOPIC "$ota/update/ZD0FYH1ONP/test"

#define OTA_VERSION_UPLOAD_TEMPLATE	"{\"type\": \"report_version\",\"report\":{\"version\": \"%s\"}}"

#define MQTT_MAX_BUFF 512

#define MQTT_DISCONNECT 0
#define MQTT_CONNECT		1

extern u8 MQTT_ConnectStatus;

enum req_qos
{
	REQ_QOS_0=0,
	REQ_QOS_1,
	REQ_QOS_2
};


u8 MQTT_Connect(char *client_id,char *user_name,char *password,u16 keep_alive);
void MQTT_Disconnect(void);
u8 MQTT_Subscribe_Topic(char *topic,int req_qos,int msgid);
u8 MQTT_Unsubscribe_Topic(char *topic,int msgid); 
u8 MQTT_Publish(char *pub_topic, unsigned char* payload);
u8 MQTT_Heart_Send(void);

#endif
