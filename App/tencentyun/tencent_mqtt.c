#include "tencent_mqtt.h"
#include "MQTTPacket.h"
#include "transport.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "ESP8266.h"

u8 mqtt_buf[MQTT_MAX_BUFF];
u16 rx_cnt=0;
u8 mqtt_connect_status=MQTT_DISCONNECT;




//连接MQTT服务器
u8 MQTT_Connect(char *client_id,char *user_name,char *password,u16 keep_alive)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID.cstring =client_id;//客户端ID
	data.keepAliveInterval = keep_alive;//保活时间,单位为秒
	data.cleansession = 1;//清除会话
	data.username.cstring = user_name;//用户名
	data.password.cstring = password;//密码
	
	u16 send_len=MQTTSerialize_connect(mqtt_buf,MQTT_MAX_BUFF,&data);//序列化连接数据报
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送连接数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if (MQTTPacket_read(mqtt_buf, MQTT_MAX_BUFF, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buf, MQTT_MAX_BUFF) != 1 || connack_rc != 0)//反序列化CONNACK报文
		{
			return 1;//接收报文类型正确,但连接失败
		}
	}
	else return 1;//接收报文类型错误
	
	return 0;
}

//断开连接
void MQTT_Disconnect(void)
{
	u16 send_len=MQTTSerialize_disconnect(mqtt_buf, MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送断开连接数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	send_len=MQTTSerialize_disconnect(mqtt_buf, MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送断开连接数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
}
//订阅主题
u8 MQTT_Subscribe_Topic(char *sub_topic,int req_qos,int msgid)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sub_topic;
	u16 send_len = MQTTSerialize_subscribe(mqtt_buf, MQTT_MAX_BUFF, 0, msgid, 1, &topicString, &req_qos);//序列化订阅数据报
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送订阅数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) == SUBACK)//判断是否是SUBACK报文
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		u8 rc=MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, mqtt_buf, MQTT_MAX_BUFF);//反序列化SUBACK报文
		if(!rc || submsgid!=msgid || granted_qos==0x80)	return 1;////接收报文类型正确,但订阅失败
	}
	else return 1;//接收报文类型错误
	
	return 0;
}
//取消订阅主题
u8 MQTT_Unsubscribe_Topic(char *unsub_topic,int msgid)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = unsub_topic;
	u16 send_len = MQTTSerialize_unsubscribe(mqtt_buf,MQTT_MAX_BUFF, 0, msgid,1, &topicString);//序列化取消订阅数据报
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送取消订阅数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) == UNSUBACK)//判断是否是UNSUBACK报文
	{
		unsigned short unsubmsgid;
		u8 rc=MQTTDeserialize_unsuback(&unsubmsgid,mqtt_buf,MQTT_MAX_BUFF);//反序列化UNSUBACK报文
		if(!rc || unsubmsgid!=msgid )	return 1;////接收报文类型正确,但取消订阅失败
	}
	else return 1;//接收报文类型错误
	return 0;
}

//发布主题
u8 MQTT_Publish(char *pub_topic, unsigned char* payload)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = pub_topic;
	u16 send_len = MQTTSerialize_publish(mqtt_buf,MQTT_MAX_BUFF,0,0,0,0,topicString, payload, strlen((const char*)payload));//序列化发布数据报
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送发布数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	return 0;
}
//发送心跳包
u8 MQTT_Heart_Send(void)
{
	u16 send_len=MQTTSerialize_pingreq(mqtt_buf,MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//向服务器发送PING数据报
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) != PINGRESP)//判断是否是PINGRESP报文
	{
		return 1;//接收报文类型错误
	}

	return 0;
}


