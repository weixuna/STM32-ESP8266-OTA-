#include "usart.h"


void Usarts_Init(void)
{
    TIMx_Init(TIM1,72-1,10*1000);//��ʱ10ms
    USARTx_Init(USART1,115200);//��ʼ������1
    TIMx_Init(TIM3,72-1,10*1000);//��ʱ10ms
    USARTx_Init(USART3,115200);//��ʼ������3
    usart1.flag=RX_BUFF_EMPTY;
    usart3.flag=RX_BUFF_EMPTY;
}

/*
�������ܣ�USARTx��ʼ��
������USART_TypeDef* USARTx,u32 Baud(������)
����ֵ����
Ӳ�����ӣ�
PA9--USART1_TX--�����������
PA10--USART1_RX--��������

PA2--USART2_TX--�����������
PA3--USART2_RX--��������

PB10--USART3_TX--�����������
PB11--USART3_RX--��������

*/
void USARTx_Init(USART_TypeDef* USARTx,u32 Baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
    
	if(USARTx==USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1��GPIOAʱ��
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//����PA9Ϊ�����������
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//����PA10Ϊ��������ģʽ
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//�����ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�Ϊ2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//��Ӧ���ȼ�(�����ȼ�)Ϊ2
		NVIC_Init(&NVIC_InitStruct);//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	}
	else if(USARTx==USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//����PA2Ϊ�����������
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//����PA3Ϊ��������ģʽ
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//�����ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�Ϊ2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//��Ӧ���ȼ�(�����ȼ�)Ϊ1
		NVIC_Init(&NVIC_InitStruct);//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	}
	else if(USARTx==USART3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��GPIOBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
	
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//����PB10Ϊ�����������
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//����PB11Ϊ��������ģʽ
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;//�����ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�Ϊ2
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//��Ӧ���ȼ�(�����ȼ�)Ϊ1
		NVIC_Init(&NVIC_InitStruct);//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	}
	USART_InitStruct.USART_BaudRate=Baud;//���ò�����
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//���÷��ͺͽ���ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;//����żУ��λ
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//1��ֹͣλ
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//8λ�ֳ�
	USART_Init(USARTx, &USART_InitStruct);//��ʼ��USARTx
	
	USART_ITConfig(USARTx,USART_IT_RXNE, ENABLE);//ʹ��USARTx���ջ������ǿ��ж�
	USART_Cmd(USARTx,ENABLE);//����USARTx
}


/*
�������ܣ�����x�����ַ�
������USART_TypeDef* USARTx,u8 ch
����ֵ����
*/
void USARTx_Send_Char(USART_TypeDef* USARTx,u8 ch)
{
	USART_SendData(USARTx,ch);//��������
	while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//�ж�һ������֡�Ƿ������
}

/*
�������ܣ�����x�����ַ���
������USART_TypeDef* USARTx,u8 *str
����ֵ����
*/
void USARTx_Send_String(USART_TypeDef* USARTx,char *str)
{
	while(*str!='\0')//�ж��ַ����Ƿ������
	{
		USART_SendData(USARTx,*str);//��������
		while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//�ж�һ������֡�Ƿ������
		str++;//ָ��ƫ��
	}
}

/*
�������ܣ�����x����ָ����������
������USART_TypeDef* USARTx,u8 *data_buff,u16 data_len
����ֵ����
*/
void USARTx_Send_Buff(USART_TypeDef* USARTx,u8 *data_buff,u16 data_len)
{
    for(u16 i=0;i<data_len;i++)
    {
        USART_SendData(USARTx,*(data_buff+i));//��������
        while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));//�ж�һ������֡�Ƿ������
    }
}
/*
�������ܣ�printf()����ض���
*/
int fputc(int c, FILE *stream)
{
	USART_SendData(USART1,c);//��������
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));//�ж�һ������֡�Ƿ������
	return c;
}

usart_t usart1={.flag=RX_BUFF_EMPTY,.cnt=0};
//����1�жϷ������
void USART1_IRQHandler(void)
{
	u8 tmp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//�Ƿ�������ջ������ǿ��ж�
	{
		tmp=(u8)USART_ReceiveData(USART1);//��������,Ӳ���Զ������־λ
		if((usart1.cnt<USART_RX_MAXLEN)&&usart1.flag==RX_BUFF_EMPTY)//�ж��Ƿ�Խ��
		{
			
			TIM_SetCounter(TIM1,0);//��ʱ��1����ֵ����
			if(usart1.cnt==0)//���յ�һ���ֽ�
			{
				TIM_Cmd(TIM1,ENABLE);//������ʱ��1
                memset((u8*)usart1.rx_buff,0,USART_RX_MAXLEN+1);
			}
            usart1.rx_buff[usart1.cnt]=tmp;//����BUFF
			usart1.cnt++;//δ�������,��������
		}
		else usart1.flag=RX_BUFF_NO_EMPTY;//��������,��Ҫ��������
	}
}




usart_t usart3={.flag=RX_BUFF_EMPTY,.cnt=0};
//����3�жϷ������
void USART3_IRQHandler(void)
{
	u8 tmp;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//�Ƿ�������ջ������ǿ��ж�
	{
		tmp=(u8)USART_ReceiveData(USART3);//��������,Ӳ���Զ������־λ
       
		if(usart3.cnt<USART_RX_MAXLEN)//�ж��Ƿ�Խ��
		{
			TIM_SetCounter(TIM3,0);//��ʱ��3����ֵ����
			if(usart3.cnt==0)//���յ�һ���ֽ�
			{
				TIM_Cmd(TIM3,ENABLE);//������ʱ��3
                //memset((u8*)usart3.rx_buff,0,USART_RX_MAXLEN+1);
			}
            usart3.rx_buff[usart3.cnt++]=tmp;//����BUFF
		}
		else usart3.flag=RX_BUFF_NO_EMPTY;//��������,��Ҫ��������
	}
}




