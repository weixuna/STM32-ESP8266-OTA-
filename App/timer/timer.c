#include "timer.h"

/*
�������ܣ���ʱ��x��ʼ��
������TIM_TypeDef* TIMx,u16 Arr-->�Զ���װ��ֵ,u16 Psc-->Ԥ��Ƶϵ��
Psc=7200-->10KHZ-->100us-->0.1ms,Psc=72-->1000KHZ-->1us-->0.001ms
����ֵ����
*/
void TIMx_Init(TIM_TypeDef* TIMx,u16 Psc,u16 Arr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	if(TIMx==TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//ʹ�ܶ�ʱ��1ʱ��
		NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_IRQn;//��ʱ��1�����ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�Ϊ1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//��Ӧ���ȼ�Ϊ0
		NVIC_Init(&NVIC_InitStruct);//��ʼ��NVIC
	}
	else if(TIMx==TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//ʹ�ܶ�ʱ��2ʱ��
		NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;//��ʱ��2�ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�Ϊ1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//��Ӧ���ȼ�Ϊ1
		NVIC_Init(&NVIC_InitStruct);//��ʼ��NVIC
	}
	else if(TIMx==TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ�ܶ�ʱ��3ʱ��
		NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;//��ʱ��3�����ж���
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�Ϊ1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//��Ӧ���ȼ�Ϊ2
		NVIC_Init(&NVIC_InitStruct);//��ʼ��NVIC
	}
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//����ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInitStruct.TIM_Period=Arr;//�����Զ���װ��ֵ(���ֵ)    
	TIM_TimeBaseInitStruct.TIM_Prescaler=Psc;//����Ԥ��Ƶϵ�� 
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStruct);//��ʼ����ʱ��x����
    
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);//ʹ�ܶ�ʱ��x�������
	TIM_ClearITPendingBit(TIMx,TIM_IT_Update);
	//TIM_Cmd(TIMx,ENABLE);//������ʱ��x
}


/*
�������ܣ���ʱ��1�����жϷ������
��������
����ֵ����
*/
void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)//���TIM1�����жϷ������
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);//���TIM1�����жϱ�־ 
        TIM_Cmd(TIM1,DISABLE);//�رն�ʱ��1
        TIM_SetCounter(TIM1,0);//��ʱ��1����ֵ����
        usart1.flag=RX_BUFF_NO_EMPTY;//���ݽ������
	}
}


/*
�������ܣ���ʱ��2�жϷ������
��������
����ֵ����
*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//���TIM2�����жϱ�־ 
		
	}
}

/*
�������ܣ���ʱ��3�жϷ������
��������
����ֵ����
*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//���TIM3�����жϱ�־ 
		TIM_Cmd(TIM3,DISABLE);//�رն�ʱ��3
        TIM_SetCounter(TIM3,0);//��ʱ��3����ֵ����
        usart3.flag=RX_BUFF_NO_EMPTY;//���ݽ������
	}
}




