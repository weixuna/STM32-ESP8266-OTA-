#include "timer.h"

/*
函数功能：定时器x初始化
参数：TIM_TypeDef* TIMx,u16 Arr-->自动重装载值,u16 Psc-->预分频系数
Psc=7200-->10KHZ-->100us-->0.1ms,Psc=72-->1000KHZ-->1us-->0.001ms
返回值：无
*/
void TIMx_Init(TIM_TypeDef* TIMx,u16 Psc,u16 Arr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	if(TIMx==TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//使能定时器1时钟
		NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_IRQn;//定时器1更新中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级为1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级为0
		NVIC_Init(&NVIC_InitStruct);//初始化NVIC
	}
	else if(TIMx==TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能定时器2时钟
		NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;//定时器2中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级为1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//响应优先级为1
		NVIC_Init(&NVIC_InitStruct);//初始化NVIC
	}
	else if(TIMx==TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能定时器3时钟
		NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;//定时器3更新中断线
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级为1
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//响应优先级为2
		NVIC_Init(&NVIC_InitStruct);//初始化NVIC
	}
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//设置时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Period=Arr;//设置自动重装载值(最大值)    
	TIM_TimeBaseInitStruct.TIM_Prescaler=Psc;//设置预分频系数 
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStruct);//初始化定时器x参数
    
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);//使能定时器x溢出更新
	TIM_ClearITPendingBit(TIMx,TIM_IT_Update);
	//TIM_Cmd(TIMx,ENABLE);//开启定时器x
}


/*
函数功能：定时器1更新中断服务程序
参数：无
返回值：无
*/
void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)//检查TIM1更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);//清除TIM1更新中断标志 
        TIM_Cmd(TIM1,DISABLE);//关闭定时器1
        TIM_SetCounter(TIM1,0);//定时器1计数值清零
        usart1.flag=RX_BUFF_NO_EMPTY;//数据接收完成
	}
}


/*
函数功能：定时器2中断服务程序
参数：无
返回值：无
*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除TIM2更新中断标志 
		
	}
}

/*
函数功能：定时器3中断服务程序
参数：无
返回值：无
*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除TIM3更新中断标志 
		TIM_Cmd(TIM3,DISABLE);//关闭定时器3
        TIM_SetCounter(TIM3,0);//定时器3计数值清零
        usart3.flag=RX_BUFF_NO_EMPTY;//数据接收完成
	}
}




