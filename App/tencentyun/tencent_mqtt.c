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




//����MQTT������
u8 MQTT_Connect(char *client_id,char *user_name,char *password,u16 keep_alive)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID.cstring =client_id;//�ͻ���ID
	data.keepAliveInterval = keep_alive;//����ʱ��,��λΪ��
	data.cleansession = 1;//����Ự
	data.username.cstring = user_name;//�û���
	data.password.cstring = password;//����
	
	u16 send_len=MQTTSerialize_connect(mqtt_buf,MQTT_MAX_BUFF,&data);//���л��������ݱ�
	transport_sendPacketBuffer(mqtt_buf,send_len);//������������������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if (MQTTPacket_read(mqtt_buf, MQTT_MAX_BUFF, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buf, MQTT_MAX_BUFF) != 1 || connack_rc != 0)//�����л�CONNACK����
		{
			return 1;//���ձ���������ȷ,������ʧ��
		}
	}
	else return 1;//���ձ������ʹ���
	
	return 0;
}

//�Ͽ�����
void MQTT_Disconnect(void)
{
	u16 send_len=MQTTSerialize_disconnect(mqtt_buf, MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//����������ͶϿ��������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	send_len=MQTTSerialize_disconnect(mqtt_buf, MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//����������ͶϿ��������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
}
//��������
u8 MQTT_Subscribe_Topic(char *sub_topic,int req_qos,int msgid)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sub_topic;
	u16 send_len = MQTTSerialize_subscribe(mqtt_buf, MQTT_MAX_BUFF, 0, msgid, 1, &topicString, &req_qos);//���л��������ݱ�
	transport_sendPacketBuffer(mqtt_buf,send_len);//����������Ͷ������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) == SUBACK)//�ж��Ƿ���SUBACK����
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		u8 rc=MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, mqtt_buf, MQTT_MAX_BUFF);//�����л�SUBACK����
		if(!rc || submsgid!=msgid || granted_qos==0x80)	return 1;////���ձ���������ȷ,������ʧ��
	}
	else return 1;//���ձ������ʹ���
	
	return 0;
}
//ȡ����������
u8 MQTT_Unsubscribe_Topic(char *unsub_topic,int msgid)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = unsub_topic;
	u16 send_len = MQTTSerialize_unsubscribe(mqtt_buf,MQTT_MAX_BUFF, 0, msgid,1, &topicString);//���л�ȡ���������ݱ�
	transport_sendPacketBuffer(mqtt_buf,send_len);//�����������ȡ���������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) == UNSUBACK)//�ж��Ƿ���UNSUBACK����
	{
		unsigned short unsubmsgid;
		u8 rc=MQTTDeserialize_unsuback(&unsubmsgid,mqtt_buf,MQTT_MAX_BUFF);//�����л�UNSUBACK����
		if(!rc || unsubmsgid!=msgid )	return 1;////���ձ���������ȷ,��ȡ������ʧ��
	}
	else return 1;//���ձ������ʹ���
	return 0;
}

//��������
u8 MQTT_Publish(char *pub_topic, unsigned char* payload)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = pub_topic;
	u16 send_len = MQTTSerialize_publish(mqtt_buf,MQTT_MAX_BUFF,0,0,0,0,topicString, payload, strlen((const char*)payload));//���л��������ݱ�
	transport_sendPacketBuffer(mqtt_buf,send_len);//����������ͷ������ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	return 0;
}
//����������
u8 MQTT_Heart_Send(void)
{
	u16 send_len=MQTTSerialize_pingreq(mqtt_buf,MQTT_MAX_BUFF);
	transport_sendPacketBuffer(mqtt_buf,send_len);//�����������PING���ݱ�
	memset(mqtt_buf,0,MQTT_MAX_BUFF);
	if(MQTTPacket_read(mqtt_buf,MQTT_MAX_BUFF, transport_getdata) != PINGRESP)//�ж��Ƿ���PINGRESP����
	{
		return 1;//���ձ������ʹ���
	}

	return 0;
}


