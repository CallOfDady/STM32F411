#ifndef __RTC_H
#define __RTC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK NANO STM32F4������
//RTC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/4/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2019-2029
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////	

extern RTC_HandleTypeDef RTC_Handler;  //RTC���
extern int te;
extern int hu;
void DHT(u8 a,u8 b);
u8 RTC_Init(void);              //RTC��ʼ��
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);	//RTC��������
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec); //��������ʱ��(����������,24Сʱ��)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);             //�����Ի��Ѷ�ʱ������
#endif