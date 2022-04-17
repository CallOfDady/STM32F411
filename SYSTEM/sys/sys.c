#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32F4开发板
//系统时钟初始化	
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 

//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率
//Fusb:USB,SDIO的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:50~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO的主PLL分频系数(PLL之后的分频),取值范围:2~15.

//外部晶振为8M的时候,推荐值:plln=96,pllm=4,pllp=2,pllq=4.
//得到:Fvco=8*(96/4)=192Mhz
//     Fsys=192/2=96Mhz
//     Fusb=192/4=48Mhz
//返回值:0,成功;1,失败。
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
	 __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟
	
	//下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡，此功能只有STM32F42xx和STM32F43xx器件有，
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//设置调压器输出电压级别1
    
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE; //时钟源为HSE
	RCC_OscInitStructure.HSEState = RCC_HSE_ON;//打开HSE
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;//打开PLL
	RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
	RCC_OscInitStructure.PLL.PLLM = pllm;//主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
	RCC_OscInitStructure.PLL.PLLN = plln;//主PLL倍频系数(PLL倍频),取值范围:50~432. 
	RCC_OscInitStructure.PLL.PLLP = pllp; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
	RCC_OscInitStructure.PLL.PLLQ = pllq; //USB/SDIO的主PLL分频系数(PLL之后的分频),取值范围:2~15.
	ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//初始化
	
    if(ret!=HAL_OK) while(1);
    
	RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;//AHB分频系数为1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;//APB1分频系数为2
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;//APB2分频系数为2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_3);//同时设置FLASH延时周期为3WS，也就是4个CPU周期。
		
    if(ret!=HAL_OK) while(1);
	
	//初始化HAL Systick时钟
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
}

#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
