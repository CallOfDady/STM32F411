#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK NANO STM32F4������
//ϵͳʱ�ӳ�ʼ��	
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 

//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��
//Fusb:USB,SDIO��ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,HSE��. 
//plln:��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:50~432.
//pllm:��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
//pllq:USB/SDIO����PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.

//�ⲿ����Ϊ8M��ʱ��,�Ƽ�ֵ:plln=96,pllm=4,pllp=2,pllq=4.
//�õ�:Fvco=8*(96/4)=192Mhz
//     Fsys=192/2=96Mhz
//     Fusb=192/4=48Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
	 __HAL_RCC_PWR_CLK_ENABLE(); //ʹ��PWRʱ��
	
	//������������������õ�ѹ�������ѹ�����Ա�������δ�����Ƶ�ʹ���
    //ʱʹ�����빦��ʵ��ƽ�⣬�˹���ֻ��STM32F42xx��STM32F43xx�����У�
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//���õ�ѹ�������ѹ����1
    
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE; //ʱ��ԴΪHSE
	RCC_OscInitStructure.HSEState = RCC_HSE_ON;//��HSE
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;//��PLL
	RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;//PLLʱ��Դѡ��HSE
	RCC_OscInitStructure.PLL.PLLM = pllm;//��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
	RCC_OscInitStructure.PLL.PLLN = plln;//��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:50~432. 
	RCC_OscInitStructure.PLL.PLLP = pllp; //ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
	RCC_OscInitStructure.PLL.PLLQ = pllq; //USB/SDIO����PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.
	ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//��ʼ��
	
    if(ret!=HAL_OK) while(1);
    
	RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//����ϵͳʱ��ʱ��ԴΪPLL
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;//AHB��Ƶϵ��Ϊ1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;//APB1��Ƶϵ��Ϊ2
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;//APB2��Ƶϵ��Ϊ2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_3);//ͬʱ����FLASH��ʱ����Ϊ3WS��Ҳ����4��CPU���ڡ�
		
    if(ret!=HAL_OK) while(1);
	
	//��ʼ��HAL Systickʱ��
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
}

#ifdef  USE_FULL_ASSERT
//��������ʾ�����ʱ��˺����������������ļ���������
//file��ָ��Դ�ļ�
//line��ָ�����ļ��е�����
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
