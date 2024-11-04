#include "transport.h"
#include "usart.h"
#include <string.h>
#include "delay.h"


int transport_sendPacketBuffer( unsigned char* buf, int buflen)
{
	usart3.flag=0;
	usart3.cnt=0;
	memset((char *)usart3.rx_buff,0,sizeof(usart3.rx_buff));//清空接收缓冲区
    USARTx_Send_Buff(USART3,buf,buflen);//发送数据
	return buflen;
}


int transport_getdata(unsigned char* buf, int count)
{
	u16 rx_cnt=0;
	static u16 read_count=0;
	while(!usart3.flag && rx_cnt<=200)//等待接收响应数据报
	{
		Delay_Ms(10);
		rx_cnt++;
	}
	if(rx_cnt>100) return 1;//超时,未接收到数据
	rx_cnt=0;
	memcpy(buf,usart3.rx_buff+read_count,count);
	read_count+=count;//USART3_BUFF数据偏移
	if(read_count>=usart3.cnt)//已经读完USART3_BUFF数据
	{
		usart3.flag=0;
		usart3.cnt=0;
		read_count=0;
	}	
	return count;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	
	return 0;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{

	return 0;
}

int transport_close(int sock)
{


	return 0;
}
