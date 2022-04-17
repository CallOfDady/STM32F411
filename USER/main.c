#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "rtc.h"
#include "smg.h"
#include "timer.h"
#include "usmart.h"
#include "key.h"
#include "dht11.h"
#include "beep.h"

//mode3 default num
//mode2 time
//mode1 temp
//mode0 set temp line

// ������������
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,ȫ��
u8 smg_num[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00};

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
int mode=3;
int c=0;
int i;
//for mode0
u8 smg_wei2=2;//�����λѡ
u8 num2=0;//��ֵ

//for mode1
u8 smg_wei1=2;//�����λѡ
u8 num1=0;//��ֵ

u16 led_t=0;//ledͳ��ʱ��
u16 dht11_t=0;//dht11����ʱ��
u8 temperature;//�¶�ֵ  	    
u8 humidity;//ʪ��ֵ

//for mode2
u8 smg_wei=0;//�����λѡ
u8 num=0;//�������ֵ

u8 time=0;//ʱ��ֵ
u8 LEDT=0x03;    //��ʼ��ӳ��LED0 LED1�ӿ�  (���)
u8 LEDF=0X00;    //��ʼ��ӳ��LED�ӿ�  (�ص�)

//�趨��ʪ�ȱ�������
int te=30;//�¶�����(temperature)
int hu=80;//ʪ������(humidity)

int main(void)
{
	int k=0;
	vu8 key=0;
  HAL_Init();                    	//��ʼ��HAL��    
  Stm32_Clock_Init(96,4,2,4);     //����ʱ��,96Mhz
  delay_init(96);                 //��ʼ����ʱ����
	uart_init(115200);				//��ʼ������115200
	LED_Init();                     //��ʼ��LED 
	LED_SMG_Init();	                //����ܳ�ʼ��
	TIM3_Init(20-1,9600-1);         //�����2ms��ʱ��ʾ	
	usmart_dev.init(96);         //��ʼ��USMART	
	KEY_Init();	    
	BEEP_Init();                    //��ʼ���������˿�
	
	//RTC״̬����ͨѶ��ʾ
	printf("RTC TEST\r\n"); 
	while(RTC_Init())		//RTC��ʼ��
	{ 
		printf("RTC ERROR!\r\n");
		delay_ms(800);
		printf("RTC Trying...\r\n");	
	}
  RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //����WAKE UP�ж�,1�����ж�һ��  
	printf("RTC OK\r\n"); 
	
	//DHT11״̬����ͨѶ��ʾ
	printf("DHT11 TEST\r\n");
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		printf("DHT11 Error\r\n");
		delay_ms(200);
		printf("DHT11 Trying...\r\n");
	}
	printf("DHT11 OK\r\n"); 
	
	
	while(1)
	{		
		
		//�����ж�
			key=KEY_Scan(0);	
			switch(key)
			{
				case KEY2_PRES://�ı�����ʱ��λ��
				if(mode==2)
				{	
						k++;
						switch(k)
						{
						case 1:
								LED5=!LED5;//����ʱΪ����"ʱ"
						break;
						case 2:
								LED5=!LED5;
								LED6=!LED6;//����ʱΪ����"��"
						break;
						case 3:
								LED6=!LED6;
								LED7=!LED7;//����ʱΪ����"��"
						break;
						case 4:
								k=0;
								LED7=!LED7;//ȫ��Ϩ����������
						break;
						}
				}
				if(mode==0)
				{	
						k++;
						switch(k)
						{
						case 1:
								LED6=!LED6;//����ʱΪ���ڡ��¶ȡ�
						break;
						case 2:
								LED6=!LED6;
								LED7=!LED7;//����ʱΪ���ڡ�ʪ�ȡ�
						break;
						case 3:
								k=0;
								LED7=!LED7;//ȫ��Ϩ����������
						break;
						}
				}
				break;
				case KEY1_PRES:
					
				//ʱ�����	
				if(mode==2)	
				{	
					if(k==1) //"ʱ"����ģ��
					{
						if(RTC_TimeStruct.Hours==0)
							{
								RTC_Set_Time(23,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
							}
						else
						{
							RTC_TimeStruct.Hours--;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
					if(k==2) //"��"����ģ��
					{
						if(RTC_TimeStruct.Minutes==0)
							{
								RTC_Set_Time(RTC_TimeStruct.Hours,59,RTC_TimeStruct.Seconds,0);
							}
						else
						{
							RTC_TimeStruct.Minutes--;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
					if(k==3) //"��"����ģ��
					{
						if(RTC_TimeStruct.Seconds==0)
							{
								RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,59,0);
							}
						else
						{
							RTC_TimeStruct.Seconds--;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
				}
				
				//�¶ȱ���ֵ����				
				if(mode==0)	
				{	
					if(k==1) //"�¶�"����ģ��
					{
						te--;
					}
					if(k==2) //"ʪ��"����ģ��
					{
						hu--;
					}
				}
				break;
				
				case WKUP_PRES:
					
				//ʱ�����		
				if(mode==2)
				{	
					if(k==1) //"ʱ"����ģ��
					{
						if(RTC_TimeStruct.Hours==23)
							{
								RTC_Set_Time(0,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
							}
						else
						{
							RTC_TimeStruct.Hours++;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
					if(k==2) //"��"����ģ��
					{
						if(RTC_TimeStruct.Minutes==59)
							{
								RTC_Set_Time(RTC_TimeStruct.Hours,0,RTC_TimeStruct.Seconds,0);
							}
						else
						{
							RTC_TimeStruct.Minutes++;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
					if(k==3) //"��"����ģ��
					{
						if(RTC_TimeStruct.Seconds==59)
							{
								RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,0,0);
							}
						else
						{
							RTC_TimeStruct.Seconds++;
							RTC_Set_Time(RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds,0);
						}
					}
				}
				
				//�¶ȱ���ֵ����				
				if(mode==0)	
				{	
					if(k==1) //"�¶�"����ģ��
					{
						te++;
					}
					if(k==2) //"ʪ��"����ģ��
					{
						hu++;
					}
				}
				break;
				
				//ģʽ�л�
				case KEY0_PRES:
					c++;
				if(c==4)
				{
					c=0;
				}
				switch(c)
				{
					case 0 : //switch to num
							k=0;
							LED0=1;
							LED1=1;
							LED2=1;
							LED5=1;
							LED6=1;
							LED7=1;
							mode=3;
							break;
					
					case 1 : //switch to time
							k=0;
							LED0=1;
							LED2=1;
							mode=2;
							break;
					
					case 2 : //switch to temp
							k=0;
							LED0=1;
							LED1=1;
							LED5=1;
							LED6=1;
							LED7=1;
							mode=1;
							break;
					
					case 3 : //switch to temp  line set
							k=0;
							LED0=1;
							LED1=1;
							LED2=1;
							LED5=1;
							LED6=1;
							LED7=1;
							mode=0;
							break;
					
				 }
				break;
			}

	//ѧ����ʾ
	if(mode==3)//����ж�
	{
		smg_wei=0;
		LED_Write_Data(smg_num[2],smg_wei);//�������ʾ
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[1],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[0],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[2],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[4],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[3],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[1],smg_wei);
	    LED_Refresh();
	    smg_wei++;
		LED_Write_Data(smg_num[7],smg_wei);
	    LED_Refresh();
	    smg_wei++;
			if(smg_wei==8) smg_wei=0;
	}
			
		//��ʪ�Ⱦ�������ˮ�ƣ��봮��ͨѶ
		if((temperature>=te||humidity>=hu)&&mode==1)
		{		
				printf("����!��ʪ�ȹ���!(�¶ȸ���%d,ʪ�ȸ���%d)\r\n",te,hu);
				for(i=0;i<7;i++)//����ѭ��
				{ 
				HAL_GPIO_WritePin(GPIOC,LEDT,GPIO_PIN_RESET);//����ָ��LED(��0)		
				HAL_GPIO_WritePin(GPIOC,LEDF,GPIO_PIN_SET);//Ϩ��ָ��LED	(��1)				
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP�������ͣ��죬 ��ͬBEEP=0;
				delay_ms(100);//��ʱ100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP�������ߣ����죬��ͬBEEP=1;
				delay_ms(100);//��ʱ100 ms
				LEDT<<=1;//����ָ����һ����ַ
				LEDF<<=1;//����ָ����һ����ַ
				LEDF++;
				}
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP�������ͣ��죬 ��ͬBEEP=0;
				delay_ms(100);//��ʱ100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP�������ߣ����죬��ͬBEEP=1;
				delay_ms(100);//��ʱ100 ms
				LEDT=0x60;//ӳ�䵽LED5 LED6
				LEDF=0x80;//ӳ�䵽LED7
				for(i=0;i<6;i++)//����ѭ��
				{ 
				HAL_GPIO_WritePin(GPIOC,LEDT,GPIO_PIN_RESET);//����ָ��LED
				HAL_GPIO_WritePin(GPIOC,LEDF,GPIO_PIN_SET);//Ϩ��ָ��LED
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP�������ͣ��죬 ��ͬBEEP=0;
				delay_ms(100);//��ʱ100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP�������ߣ����죬��ͬBEEP=1;
				delay_ms(100);//��ʱ100 ms
				LEDT>>=1;//����ָ����һ����ַ
				LEDF>>=1;//����ָ����һ����ַ
				}
				LEDT=0x03;
				LEDF=0X00;//��ʼ������LEDӳ��׼����һѭ��			
		}
		else
		{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
				LED1=1;  //���þ������������
		}
	} 		
}

	//RTCʱ����ʾ�봮��ͨѶ
u16 print=0;//�����ʱ
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //�ص���������ʱ���жϷ���������
{	
	
    if(htim==(&TIM3_Handler)&&mode==2)
    {
		 print++;
		 if(print==500)
		 {
				printf("��ǰ��ʱ���ǣ�%d��%d��%d��\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
				print=0;
		 }
		 HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
		 HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
     switch(smg_wei)
		{
		   case 0:  num = smg_num[RTC_TimeStruct.Hours/10]; break;//ʱ
		   case 1:  num = smg_num[RTC_TimeStruct.Hours%10]; break; 
		   case 2:  num = 0x02; break; 	   
		   case 3:  num = smg_num[RTC_TimeStruct.Minutes/10]; break; //��
		   case 4:  num = smg_num[RTC_TimeStruct.Minutes%10]; break; 
			 case 5:  num = 0x02; break;
		   case 6:  num = smg_num[RTC_TimeStruct.Seconds/10]; break; //��
		   case 7:  num = smg_num[RTC_TimeStruct.Seconds%10]; break;   
	   }
	   LED_Write_Data(num,smg_wei);//д���ݵ������
	   LED_Refresh();//������ʾ
	   smg_wei++;
	   if(smg_wei==8) smg_wei=0;
    }
		
		//��ʪ�ȴ���ͨѶ��ʾ
		if(htim==(&TIM3_Handler)&&mode==1)
    {
			print++;
		 if(print==500)
			{
				printf("��ǰ�¶�Ϊ��%d ʪ��Ϊ%d(��ǰԤ��ֵ��%d %d)\r\n",temperature,humidity,te,hu);
				print=0;
			}
      dht11_t++;
			if(dht11_t==500)//DHT11 1S����
			{
				dht11_t=0;
				DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��ֵ		
			}
			switch(smg_wei1)
			{
			   case 2:  num1 = smg_num[temperature/10]; break;//�¶�ֵ
			   case 3:  num1 = smg_num[temperature%10];break; 
			   case 6:  num1 = smg_num[humidity/10]; break;//ʪ��ֵ
			   case 7:  num1 = smg_num[humidity%10];break;      
			}
	    LED_Write_Data(num1,smg_wei1);//д���ݵ������
	    LED_Refresh();//������ʾ
	    smg_wei1++;
			
			//�����λ������
			if(smg_wei1==4)  smg_wei1=6;
			if(smg_wei1==8)  smg_wei1=2;	
			led_t++;
			if(led_t==250)//LED0ÿ500ms��˸
			{
				if(temperature<30&&humidity<80)
				{
					led_t=0;
					LED0=!LED0;
				}
					led_t=0;
			 }
		 }
		
		 //��ʪ�ȱ���ֵ��ʾ
		 if(mode==0)
		 {
		 switch(smg_wei2)
			{
			   case 2:  num2 = smg_num[te/10]; break;//�¶�ֵ
			   case 3:  num2 = smg_num[te%10];break; 
			   case 6:  num2 = smg_num[hu/10]; break;//ʪ��ֵ
			   case 7:  num2 = smg_num[hu%10];break;      
			}
	    LED_Write_Data(num2,smg_wei2);//д���ݵ������
	    LED_Refresh();//������ʾ
	    smg_wei2++;
			
			//�����λ������
			if(smg_wei2==4)  smg_wei2=6;
			if(smg_wei2==8)  smg_wei2=2;
		}
}

