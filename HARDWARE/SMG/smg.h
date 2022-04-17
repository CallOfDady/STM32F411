#ifndef _LED_SMG_H
#define _LED_SMG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK NANO STM32F4������
//�������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2019/4/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2019-2029
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

////74HC138������
#define LED_A0 PCout(10) //A0��ַ��
#define LED_A1 PCout(11) //A1��ַ��
#define LED_A2 PCout(12) //A2��ַ��

////74HC595������
#define LED_DS		PBout(3) //������
#define LED_LCLK	PBout(4) //����ʱ����
#define LED_SCK		PBout(5) //ʱ����

void LED_SMG_Init(void);
void LED_Refresh(void);
void LED_Write_Data(u8 duan,u8 wei);

#endif



