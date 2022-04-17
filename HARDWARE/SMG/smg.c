#include "smg.h"
#include "delay.h"

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

//���������IO��ʼ��
void LED_SMG_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	
   __HAL_RCC_GPIOB_CLK_ENABLE();  //����GPIOBʱ��
   __HAL_RCC_GPIOC_CLK_ENABLE();  //����GPIOCʱ��

	//74HC595  DS-PB3 LCLK-PB4 SCK-PB5
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;//PB3~5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);   	

	//74HC138  A0-PC10 A1-PC11 A2-PC12
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;//PC10~12
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);   	

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET);
}
//74HC138����
//�����λѡ
//num:Ҫ��ʾ������ܱ�� 0-7(��8�������)
void LED_Wei(u8 num)
{
    LED_A0=num&0x01;
	LED_A1=(num&0x02)>>1;
	LED_A2=(num&0x04)>>2;
}
//74HC595����
//�������ʾ
//duan:��ʾ�Ķ���
//wei:Ҫ��ʾ������ܱ�� 0-7(��8�������)
void LED_Write_Data(u8 duan,u8 wei)
{
	u8 i;
	for( i=0;i<8;i++)//���Ͷ�
	{
		LED_DS=(duan>>i)&0x01;
		LED_SCK=0;
		delay_us(5);
		LED_SCK=1;
	}
    LED_Wei(wei);//��ѡ��λ
}
//74HC595����
//�����ˢ����ʾ
void LED_Refresh(void)
{
	LED_LCLK=1;
	delay_us(5);
	LED_LCLK=0;
}

