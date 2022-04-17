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

// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,全灭
u8 smg_num[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00};

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
int mode=3;
int c=0;
int i;
//for mode0
u8 smg_wei2=2;//数码管位选
u8 num2=0;//数值

//for mode1
u8 smg_wei1=2;//数码管位选
u8 num1=0;//数值

u16 led_t=0;//led统计时间
u16 dht11_t=0;//dht11采样时间
u8 temperature;//温度值  	    
u8 humidity;//湿度值

//for mode2
u8 smg_wei=0;//数码管位选
u8 num=0;//数码管数值

u8 time=0;//时间值
u8 LEDT=0x03;    //初始化映射LED0 LED1接口  (点灯)
u8 LEDF=0X00;    //初始化映射LED接口  (关灯)

//设定温湿度报警上限
int te=30;//温度上限(temperature)
int hu=80;//湿度上限(humidity)

int main(void)
{
	int k=0;
	vu8 key=0;
  HAL_Init();                    	//初始化HAL库    
  Stm32_Clock_Init(96,4,2,4);     //设置时钟,96Mhz
  delay_init(96);                 //初始化延时函数
	uart_init(115200);				//初始化串口115200
	LED_Init();                     //初始化LED 
	LED_SMG_Init();	                //数码管初始化
	TIM3_Init(20-1,9600-1);         //数码管2ms定时显示	
	usmart_dev.init(96);         //初始化USMART	
	KEY_Init();	    
	BEEP_Init();                    //初始化蜂鸣器端口
	
	//RTC状态串口通讯显示
	printf("RTC TEST\r\n"); 
	while(RTC_Init())		//RTC初始化
	{ 
		printf("RTC ERROR!\r\n");
		delay_ms(800);
		printf("RTC Trying...\r\n");	
	}
  RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次  
	printf("RTC OK\r\n"); 
	
	//DHT11状态串口通讯显示
	printf("DHT11 TEST\r\n");
	while(DHT11_Init())	//DHT11初始化	
	{
		printf("DHT11 Error\r\n");
		delay_ms(200);
		printf("DHT11 Trying...\r\n");
	}
	printf("DHT11 OK\r\n"); 
	
	
	while(1)
	{		
		
		//按键判断
			key=KEY_Scan(0);	
			switch(key)
			{
				case KEY2_PRES://改变所调时间位数
				if(mode==2)
				{	
						k++;
						switch(k)
						{
						case 1:
								LED5=!LED5;//亮起时为调节"时"
						break;
						case 2:
								LED5=!LED5;
								LED6=!LED6;//亮起时为调节"分"
						break;
						case 3:
								LED6=!LED6;
								LED7=!LED7;//亮起时为调节"秒"
						break;
						case 4:
								k=0;
								LED7=!LED7;//全部熄灭锁定调节
						break;
						}
				}
				if(mode==0)
				{	
						k++;
						switch(k)
						{
						case 1:
								LED6=!LED6;//亮起时为调节“温度”
						break;
						case 2:
								LED6=!LED6;
								LED7=!LED7;//亮起时为调节“湿度”
						break;
						case 3:
								k=0;
								LED7=!LED7;//全部熄灭锁定调节
						break;
						}
				}
				break;
				case KEY1_PRES:
					
				//时间调整	
				if(mode==2)	
				{	
					if(k==1) //"时"减少模块
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
					if(k==2) //"分"减少模块
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
					if(k==3) //"秒"减少模块
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
				
				//温度报警值调整				
				if(mode==0)	
				{	
					if(k==1) //"温度"减少模块
					{
						te--;
					}
					if(k==2) //"湿度"减少模块
					{
						hu--;
					}
				}
				break;
				
				case WKUP_PRES:
					
				//时间调整		
				if(mode==2)
				{	
					if(k==1) //"时"增加模块
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
					if(k==2) //"分"增加模块
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
					if(k==3) //"秒"增加模块
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
				
				//温度报警值调整				
				if(mode==0)	
				{	
					if(k==1) //"温度"增加模块
					{
						te++;
					}
					if(k==2) //"湿度"增加模块
					{
						hu++;
					}
				}
				break;
				
				//模式切换
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

	//学号显示
	if(mode==3)//溢出中断
	{
		smg_wei=0;
		LED_Write_Data(smg_num[2],smg_wei);//数码管显示
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
			
		//温湿度警报（流水灯）与串口通讯
		if((temperature>=te||humidity>=hu)&&mode==1)
		{		
				printf("警告!温湿度过高!(温度高于%d,湿度高于%d)\r\n",te,hu);
				for(i=0;i<7;i++)//正向循环
				{ 
				HAL_GPIO_WritePin(GPIOC,LEDT,GPIO_PIN_RESET);//点亮指定LED(置0)		
				HAL_GPIO_WritePin(GPIOC,LEDF,GPIO_PIN_SET);//熄灭指定LED	(置1)				
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP引脚拉低，响， 等同BEEP=0;
				delay_ms(100);//延时100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP引脚拉高，不响，等同BEEP=1;
				delay_ms(100);//延时100 ms
				LEDT<<=1;//左移指向下一个地址
				LEDF<<=1;//左移指向下一个地址
				LEDF++;
				}
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP引脚拉低，响， 等同BEEP=0;
				delay_ms(100);//延时100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP引脚拉高，不响，等同BEEP=1;
				delay_ms(100);//延时100 ms
				LEDT=0x60;//映射到LED5 LED6
				LEDF=0x80;//映射到LED7
				for(i=0;i<6;i++)//逆向循环
				{ 
				HAL_GPIO_WritePin(GPIOC,LEDT,GPIO_PIN_RESET);//点亮指定LED
				HAL_GPIO_WritePin(GPIOC,LEDF,GPIO_PIN_SET);//熄灭指定LED
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP引脚拉低，响， 等同BEEP=0;
				delay_ms(100);//延时100 ms
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   //BEEP引脚拉高，不响，等同BEEP=1;
				delay_ms(100);//延时100 ms
				LEDT>>=1;//右移指向下一个地址
				LEDF>>=1;//右移指向下一个地址
				}
				LEDT=0x03;
				LEDF=0X00;//初始化所有LED映射准备下一循环			
		}
		else
		{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
				LED1=1;  //重置警报灯与蜂鸣器
		}
	} 		
}

	//RTC时间显示与串口通讯
u16 print=0;//输出计时
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //回调函数，定时器中断服务函数调用
{	
	
    if(htim==(&TIM3_Handler)&&mode==2)
    {
		 print++;
		 if(print==500)
		 {
				printf("当前的时间是：%d点%d分%d秒\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
				print=0;
		 }
		 HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
		 HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
     switch(smg_wei)
		{
		   case 0:  num = smg_num[RTC_TimeStruct.Hours/10]; break;//时
		   case 1:  num = smg_num[RTC_TimeStruct.Hours%10]; break; 
		   case 2:  num = 0x02; break; 	   
		   case 3:  num = smg_num[RTC_TimeStruct.Minutes/10]; break; //分
		   case 4:  num = smg_num[RTC_TimeStruct.Minutes%10]; break; 
			 case 5:  num = 0x02; break;
		   case 6:  num = smg_num[RTC_TimeStruct.Seconds/10]; break; //秒
		   case 7:  num = smg_num[RTC_TimeStruct.Seconds%10]; break;   
	   }
	   LED_Write_Data(num,smg_wei);//写数据到数码管
	   LED_Refresh();//更新显示
	   smg_wei++;
	   if(smg_wei==8) smg_wei=0;
    }
		
		//温湿度串口通讯显示
		if(htim==(&TIM3_Handler)&&mode==1)
    {
			print++;
		 if(print==500)
			{
				printf("当前温度为：%d 湿度为%d(当前预警值：%d %d)\r\n",temperature,humidity,te,hu);
				print=0;
			}
      dht11_t++;
			if(dht11_t==500)//DHT11 1S采样
			{
				dht11_t=0;
				DHT11_Read_Data(&temperature,&humidity);//读取温湿度值		
			}
			switch(smg_wei1)
			{
			   case 2:  num1 = smg_num[temperature/10]; break;//温度值
			   case 3:  num1 = smg_num[temperature%10];break; 
			   case 6:  num1 = smg_num[humidity/10]; break;//湿度值
			   case 7:  num1 = smg_num[humidity%10];break;      
			}
	    LED_Write_Data(num1,smg_wei1);//写数据到数码管
	    LED_Refresh();//更新显示
	    smg_wei1++;
			
			//数码管位数跳变
			if(smg_wei1==4)  smg_wei1=6;
			if(smg_wei1==8)  smg_wei1=2;	
			led_t++;
			if(led_t==250)//LED0每500ms闪烁
			{
				if(temperature<30&&humidity<80)
				{
					led_t=0;
					LED0=!LED0;
				}
					led_t=0;
			 }
		 }
		
		 //温湿度报警值显示
		 if(mode==0)
		 {
		 switch(smg_wei2)
			{
			   case 2:  num2 = smg_num[te/10]; break;//温度值
			   case 3:  num2 = smg_num[te%10];break; 
			   case 6:  num2 = smg_num[hu/10]; break;//湿度值
			   case 7:  num2 = smg_num[hu%10];break;      
			}
	    LED_Write_Data(num2,smg_wei2);//写数据到数码管
	    LED_Refresh();//更新显示
	    smg_wei2++;
			
			//数码管位数跳变
			if(smg_wei2==4)  smg_wei2=6;
			if(smg_wei2==8)  smg_wei2=2;
		}
}

