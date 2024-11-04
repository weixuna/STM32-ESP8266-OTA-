#include "sys.h"

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


void RCC_Configuration(void)  

{  

    RCC_DeInit();//������ RCC�Ĵ�������Ϊȱʡֵ  

  

    RCC_HSICmd(ENABLE);//ʹ��HSI    

    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);//�ȴ�HSIʹ�ܳɹ�  

  

    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  

    FLASH_SetLatency(FLASH_Latency_2);  

     

    RCC_HCLKConfig(RCC_SYSCLK_Div1);     
    RCC_PCLK1Config(RCC_HCLK_Div2);  
    RCC_PCLK2Config(RCC_HCLK_Div1);  


//    //���� PLL ʱ��Դ����Ƶϵ��  

    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);//ʹ�ܻ���ʧ�� PLL�����������ȡ��ENABLE����DISABLE   

    RCC_PLLCmd(ENABLE);//���PLL������ϵͳʱ����ô�����ܱ�ʧ��  

//    //�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�  

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  


    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//ѡ����Ҫ��ϵͳʱ��   

    //�ȴ�PLL�ɹ�������ϵͳʱ�ӵ�ʱ��Դ  

    //  0x00��HSI ��Ϊϵͳʱ��   

    //  0x04��HSE��Ϊϵͳʱ��   

    //  0x08��PLL��Ϊϵͳʱ��    

    while(RCC_GetSYSCLKSource() != 0x08);//���뱻ѡ���ϵͳʱ�Ӷ�Ӧ����
    
    
}

