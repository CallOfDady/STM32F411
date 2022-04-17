#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK NANO STM32F4������
//KEY��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2019/3/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2019-2029
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//����ķ�ʽ��ͨ��λ��������ʽ��ȡIO
//#define KEY0   PCin(8)   	
//#define KEY1   PCin(9)
//#define KEY2   PDin(2)	 
//#define WK_UP  PAin(0)	


//����ķ�ʽ��ͨ��ֱ�Ӳ���HAL�⺯����ʽ��ȡIO
#define KEY0        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8)  //KEY0����PC8
#define KEY1        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9)  //KEY1����PC9
#define KEY2        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)  //KEY2����PD2
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP����PA0

#define KEY0_PRES	1	//KEY0  
#define KEY1_PRES	2	//KEY1 
#define KEY2_PRES	3	//KEY2
#define WKUP_PRES	4	//WK_UP 

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
