#ifndef _TIMER_H_
#define	_TIMER_H_

#include "stm32f10x.h"
#include "usart.h"

void TIMx_Init(TIM_TypeDef* TIMx,u16 Psc,u16 Arr);
#endif
