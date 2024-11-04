#include "key.h"
#include "delay.h"
/*
�������ܣ�������ʼ��
��������
����ֵ����
*/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=KEY_S3_PIN|KEY_S4_PIN|KEY_S5_PIN;;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//����Ϊ��������ģʽ
    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin=KEY_S2_PIN;;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//����Ϊ��������ģʽ
    
}

/*
�������ܣ���ȡ��ֵ
������key_mode_t key_mode,����ģʽ/����ģʽ
����ֵ�����ؼ�ֵ
*/
key_val_t Get_Key_Value(key_mode_t key_mode)
{
	static u8 Flag=1;
	if(key_mode) Flag=1;//����ģʽ
	if((KEY_S2||!KEY_S2||!KEY_S4||!KEY_S5)&&Flag)
	{
		Flag=0;
		Delay_Ms(10);//����
		if(KEY_S2) return KEY_S2_PRESS;
		if(!KEY_S3) return KEY_S3_PRESS;
        if(!KEY_S4) return KEY_S4_PRESS;
        if(!KEY_S5) return KEY_S5_PRESS;
	}
	else if(!KEY_S2&&KEY_S3&&KEY_S4&&KEY_S5&&!Flag)	Flag=1;//����ģʽ
	return NO_KEY_PRESS;
}

