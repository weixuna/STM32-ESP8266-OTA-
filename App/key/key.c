#include "key.h"
#include "delay.h"
/*
函数功能：按键初始化
参数：无
返回值：无
*/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=KEY_S3_PIN|KEY_S4_PIN|KEY_S5_PIN;;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//配置为上拉输入模式
    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin=KEY_S2_PIN;;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//配置为下拉输入模式
    
}

/*
函数功能：获取键值
参数：key_mode_t key_mode,连续模式/单次模式
返回值：返回键值
*/
key_val_t Get_Key_Value(key_mode_t key_mode)
{
	static u8 Flag=1;
	if(key_mode) Flag=1;//连续模式
	if((KEY_S2||!KEY_S2||!KEY_S4||!KEY_S5)&&Flag)
	{
		Flag=0;
		Delay_Ms(10);//消抖
		if(KEY_S2) return KEY_S2_PRESS;
		if(!KEY_S3) return KEY_S3_PRESS;
        if(!KEY_S4) return KEY_S4_PRESS;
        if(!KEY_S5) return KEY_S5_PRESS;
	}
	else if(!KEY_S2&&KEY_S3&&KEY_S4&&KEY_S5&&!Flag)	Flag=1;//单次模式
	return NO_KEY_PRESS;
}

