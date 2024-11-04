#include "usart.h"


void Usarts_Init(void)
{
    TIMx_Init(TIM1,72-1,10*1000);//定时10ms
    USARTx_Init(USART1,115200);//初始化串口1
    TIMx_Init(TIM3,72-1,10*1000);//定时10ms
    USARTx_Init(USART3,115200);//初始化串口3
    usart1.flag=RX_BUFF_EMPTY;
    usart3.flag=RX_BUFF_EMPTY;
}

/*
函数功能：USARTx初始化
参数：USART_TypeDef* USARTx,u32 Baud(波特率)
返回值：无
硬件连接：
PA9--USART1_TX--复用推挽输出
PA10--USART1_RX--浮空输入

PA2--USART2_TX--复用推挽输出
PA3--USART2_RX--浮空输入

PB10--USART3_TX--复用推挽输出
PB11--USART3_RX--浮空输入

*/
void USARTx_Init(USART_TypeDef* USARTx,u32 Baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
    
	if(USARTx==USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);//使能USART1和GPIOA时钟
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//配置PA9为复用推挽输出
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//配置PA10为浮空输入模式
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//设置中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级为2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级(子优先级)为2
		NVIC_Init(&NVIC_InitStruct);//根据指定的参数初始化VIC寄存器
	}
	else if(USARTx==USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//配置PA2为复用推挽输出
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//配置PA3为浮空输入模式
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//设置中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级为2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//响应优先级(子优先级)为1
		NVIC_Init(&NVIC_InitStruct);//根据指定的参数初始化VIC寄存器
	}
	else if(USARTx==USART3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//配置PB10为复用推挽输出
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//配置PB11为浮空输入模式
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;//设置中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级为2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//响应优先级(子优先级)为1
		NVIC_Init(&NVIC_InitStruct);//根据指定的参数初始化VIC寄存器
	}
	USART_InitStruct.USART_BaudRate=Baud;//设置波特率
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//启用发送和接收模式
	USART_InitStruct.USART_Parity=USART_Parity_No;//无奇偶校验位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//1个停止位
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//8位字长
	USART_Init(USARTx, &USART_InitStruct);//初始化USARTx
	
	USART_ITConfig(USARTx,USART_IT_RXNE, ENABLE);//使能USARTx接收缓冲区非空中断
	USART_Cmd(USARTx,ENABLE);//开启USARTx
}


/*
函数功能：串口x发送字符
参数：USART_TypeDef* USARTx,u8 ch
返回值：无
*/
void USARTx_Send_Char(USART_TypeDef* USARTx,u8 ch)
{
	USART_SendData(USARTx,ch);//发送数据
	while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//判断一个数据帧是否发送完成
}

/*
函数功能：串口x发送字符串
参数：USART_TypeDef* USARTx,u8 *str
返回值：无
*/
void USARTx_Send_String(USART_TypeDef* USARTx,char *str)
{
	while(*str!='\0')//判断字符串是否发送完成
	{
		USART_SendData(USARTx,*str);//发送数据
		while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//判断一个数据帧是否发送完成
		str++;//指针偏移
	}
}

/*
函数功能：串口x发送指定长度数据
参数：USART_TypeDef* USARTx,u8 *data_buff,u16 data_len
返回值：无
*/
void USARTx_Send_Buff(USART_TypeDef* USARTx,u8 *data_buff,u16 data_len)
{
    for(u16 i=0;i<data_len;i++)
    {
        USART_SendData(USARTx,*(data_buff+i));//发送数据
        while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//判断一个数据帧是否发送完成
    }
}
/*
函数功能：printf()输出重定向
*/
int fputc(int c, FILE *stream)
{
	USART_SendData(USART1,c);//发送数据
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));//判断一个数据帧是否发送完成
	return c;
}

usart_t usart1={.flag=RX_BUFF_EMPTY,.cnt=0};
//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u8 tmp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//是否产生接收缓冲区非空中断
	{
		tmp=(u8)USART_ReceiveData(USART1);//接收数据,硬件自动清除标志位
		if((usart1.cnt<USART_RX_MAXLEN)&&usart1.flag==RX_BUFF_EMPTY)//判断是否越界
		{
			
			TIM_SetCounter(TIM1,0);//定时器1计数值清零
			if(usart1.cnt==0)//接收第一个字节
			{
				TIM_Cmd(TIM1,ENABLE);//开启定时器1
                memset((u8*)usart1.rx_buff,0,USART_RX_MAXLEN+1);
			}
            usart1.rx_buff[usart1.cnt]=tmp;//存入BUFF
			usart1.cnt++;//未接收完成,继续接收
		}
		else usart1.flag=RX_BUFF_NO_EMPTY;//缓冲区满,需要处理数据
	}
}




usart_t usart3={.flag=RX_BUFF_EMPTY,.cnt=0};
//串口3中断服务程序
void USART3_IRQHandler(void)
{
	u8 tmp;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//是否产生接收缓冲区非空中断
	{
		tmp=(u8)USART_ReceiveData(USART3);//接收数据,硬件自动清除标志位
       
		if(usart3.cnt<USART_RX_MAXLEN)//判断是否越界
		{
			TIM_SetCounter(TIM3,0);//定时器3计数值清零
			if(usart3.cnt==0)//接收第一个字节
			{
				TIM_Cmd(TIM3,ENABLE);//开启定时器3
                //memset((u8*)usart3.rx_buff,0,USART_RX_MAXLEN+1);
			}
            usart3.rx_buff[usart3.cnt++]=tmp;//存入BUFF
		}
		else usart3.flag=RX_BUFF_NO_EMPTY;//缓冲区满,需要处理数据
	}
}




