#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

u8 ESP_Connect_Wifi(char *ssid,char *pwd)
{
	char buff[40];
	snprintf(buff,sizeof(buff),"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);//�ַ���ƴ��
	if(ESP_SendCmd(buff,"OK",1800,5)) return 1;//ʧ�ܷ���1
	else return 0;
}
u8 ESP_Connect_Server(char *server_ip,u16 port)
{
	char buff[100];
	snprintf(buff,sizeof(buff),"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",server_ip,port);//�ַ���ƴ��
	if(ESP_SendCmd(buff,"OK",100,10)) return 1;//ʧ�ܷ���1
	else return 0;
}
u8 ESP_Exit_Unvarnished(void)
{
	if(ESP_SendCmd("+++","CLOSE",1800,1)) return 1;//ʧ�ܷ���1
	else return 0;
}
u8 ESP_UnvarnishedMode_Init(char *ssid,char *pwd,char *server_ip,u16 port)
{
	ESP_Exit_Unvarnished();
	//1.����WIFIģ��
	if(ESP_SendCmd("AT\r\n","OK",10,10)) return 1;
	//2.����WIFI����ģʽ
	if(ESP_Set_WorkMode(Station)) return 2;
	//3.����WIFIģ��
	ESP_Reset();
	//4.�رջ���
	if(ESP_SendCmd("ATE0\r\n","OK",10,10))	return 3;
	//5.����wifi
	if(ESP_Connect_Wifi(ssid,pwd)) return 4;
	//6.��Ϊ͸��ģʽ
	if(ESP_SendCmd("AT+CIPMODE=1\r\n","OK",10,10)) return 5;
	//7.���ӷ�����
	if(ESP_Connect_Server(server_ip,port)) return 6;
	//8.����͸��
	if(ESP_SendCmd("AT+CIPSEND\r\n",">",10,10)) return 7;
	
	return	0;//WIFIģ���ʼ���ɹ�����0
}
/*
�������ܣ���ESP8266����ATָ��
��	 ����char *Cmd,ATָ��
����ֵ���ɹ�����0,ʧ�ܷ���1
*/
u8 ESP_SendCmd(char *cmd,const char *check_str,u16 time_out,u16 repeat_cnt)
{
	u8 i,j;
	for(i=0;i<repeat_cnt;i++)//ʧ�����ظ�����ָ��
	{
		usart3.flag=RX_BUFF_EMPTY;//���ձ�־λ����
		usart3.cnt=0;
		memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//��ս��ջ�����
		USARTx_Send_String(USART3,cmd);
		for(j=0;j<repeat_cnt;j++)//�ظ������������Ƿ���ȷ
		{
			if(usart3.flag==RX_BUFF_NO_EMPTY)
			{
				usart3.rx_buff[usart3.cnt]='\0';//תΪ�ַ���
				if(strstr((char*)usart3.rx_buff,check_str))//�ж�ATָ���Ƿ��ͳɹ�
				{
					usart3.flag=0;//���ձ�־λ����
					usart3.cnt=0;
					memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//��ս��ջ�����
					return 0;
				}
				else break;
			}
           Delay_Ms(time_out);
		}
	}
	return 1;
}

/*
�������ܣ�WIFIģ�鹤��ģʽ����
������u8 mode
Station 		1
SoftAP	 		2
SoftAP_Station 	3
����ֵ���ɹ�����0��ʧ�ܷ���1
*/
u8 ESP_Set_WorkMode(u8 mode)
{
	char buff[20];
	snprintf(buff,sizeof(buff),"AT+CWMODE=%d\r\n",mode);//�ַ���ƴ��
	if(ESP_SendCmd(buff,"OK",10,10)) return 1;//ʧ�ܷ���1
	else return 0;
}

/*
�������ܣ�����WIFIģ��
��������
����ֵ����
*/
void ESP_Reset(void)
{
	USARTx_Send_String(USART3,"AT+RST\r\n");
	Delay_Ms(1000);//����WIFIģ����ʱ3S�������������æ
	Delay_Ms(1000);
	Delay_Ms(1000);
}

