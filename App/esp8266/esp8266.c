#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

u8 ESP_Connect_Wifi(char *ssid,char *pwd)
{
	char buff[40];
	snprintf(buff,sizeof(buff),"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);//字符串拼接
	if(ESP_SendCmd(buff,"OK",1800,5)) return 1;//失败返回1
	else return 0;
}
u8 ESP_Connect_Server(char *server_ip,u16 port)
{
	char buff[100];
	snprintf(buff,sizeof(buff),"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",server_ip,port);//字符串拼接
	if(ESP_SendCmd(buff,"OK",100,10)) return 1;//失败返回1
	else return 0;
}
u8 ESP_Exit_Unvarnished(void)
{
	if(ESP_SendCmd("+++","CLOSE",1800,1)) return 1;//失败返回1
	else return 0;
}
u8 ESP_UnvarnishedMode_Init(char *ssid,char *pwd,char *server_ip,u16 port)
{
	ESP_Exit_Unvarnished();
	//1.测试WIFI模块
	if(ESP_SendCmd("AT\r\n","OK",10,10)) return 1;
	//2.配置WIFI工作模式
	if(ESP_Set_WorkMode(Station)) return 2;
	//3.重启WIFI模块
	ESP_Reset();
	//4.关闭回显
	if(ESP_SendCmd("ATE0\r\n","OK",10,10))	return 3;
	//5.连接wifi
	if(ESP_Connect_Wifi(ssid,pwd)) return 4;
	//6.设为透传模式
	if(ESP_SendCmd("AT+CIPMODE=1\r\n","OK",10,10)) return 5;
	//7.连接服务器
	if(ESP_Connect_Server(server_ip,port)) return 6;
	//8.开启透传
	if(ESP_SendCmd("AT+CIPSEND\r\n",">",10,10)) return 7;
	
	return	0;//WIFI模块初始化成功返回0
}
/*
函数功能：向ESP8266发送AT指令
参	 数：char *Cmd,AT指令
返回值：成功返回0,失败返回1
*/
u8 ESP_SendCmd(char *cmd,const char *check_str,u16 time_out,u16 repeat_cnt)
{
	u8 i,j;
	for(i=0;i<repeat_cnt;i++)//失败则重复发送指令
	{
		usart3.flag=RX_BUFF_EMPTY;//接收标志位清零
		usart3.cnt=0;
		memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//清空接收缓冲区
		USARTx_Send_String(USART3,cmd);
		for(j=0;j<repeat_cnt;j++)//重复检测接收数据是否正确
		{
			if(usart3.flag==RX_BUFF_NO_EMPTY)
			{
				usart3.rx_buff[usart3.cnt]='\0';//转为字符串
				if(strstr((char*)usart3.rx_buff,check_str))//判断AT指令是否发送成功
				{
					usart3.flag=0;//接收标志位清零
					usart3.cnt=0;
					memset(usart3.rx_buff,0,sizeof(usart3.rx_buff));//清空接收缓冲区
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
函数功能：WIFI模块工作模式设置
参数：u8 mode
Station 		1
SoftAP	 		2
SoftAP_Station 	3
返回值：成功返回0，失败返回1
*/
u8 ESP_Set_WorkMode(u8 mode)
{
	char buff[20];
	snprintf(buff,sizeof(buff),"AT+CWMODE=%d\r\n",mode);//字符串拼接
	if(ESP_SendCmd(buff,"OK",10,10)) return 1;//失败返回1
	else return 0;
}

/*
函数功能：重启WIFI模块
参数：无
返回值：无
*/
void ESP_Reset(void)
{
	USARTx_Send_String(USART3,"AT+RST\r\n");
	Delay_Ms(1000);//重启WIFI模块延时3S，避免后续操作忙
	Delay_Ms(1000);
	Delay_Ms(1000);
}

