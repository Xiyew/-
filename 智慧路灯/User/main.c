#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "MyRTC.h"
#include "Timer.h"
#include "IWDG.h"

#include "onenet.h"
#include "esp8266.h"

#include "usart.h"
#include "OLED.h"
#include "AD.h"
#include "PWM.h"
#include "DHT11.h"

#include "string.h"

/*github推送测试*/
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	Delay_Init();
	MyRtc_Init();	
	Timer_init();

	OLED_Init();	
	DHT11_Init();		
	AD_Init();			
	PWM_Init();			

	Usart1_Init(115200);	//esp8266波特率
	Usart3_Init(115200);	//调试信息发送
	OLED_ShowString(1, 1, "Hardware OK");
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	unsigned char *dataPtr = NULL;

	Hardware_Init();		//硬件初始化
	Net_time();				//获取网络时间

	IWDG_Init();
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	ESP8266_Init();			//准备连接onenet

	/*连接MQTT*/
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	{
		DelayXms(500);
	}
	OLED_ShowString(3, 1, "Connected MQTT");
	
	/*连接OneNet*/
	while(OneNet_DevLink())			
		DelayXms(500);

	OLED_ShowString(4, 15, "O");
	OneNET_Subscribe();
	OLED_ShowString(4, 16, "K");

	TIM_SelectOCxM(TIM3,TIM_Channel_3,TIM_ForcedAction_InActive);
	TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);

	OLED_ShowNum(1, 16, Led_INFO.Led_Status, 1);
	OLED_ShowString(4, 1, "Time:XX:XX:XX");
	
	while(1)
	{
		MyRTC_ReadTime();

		OLED_ShowNum(4, 6, 	RTC_Time.hour, 2);
		OLED_ShowNum(4, 9,  RTC_Time.min,  2);
		OLED_ShowNum(4, 12, RTC_Time.sec,  2);


		/* 18:30 - 5:30 */
		if(Led_INFO.Led_Status == 1)
		{
			if(RTC_Time.hour >= 18 || RTC_Time.hour < 5 || ((RTC_Time.hour == 5) && (RTC_Time.min <= 30)))
			{
				Lumi_Control();
			}
			else
			{
				PWM_SetCompare3(0);
				AD_Data.Sensor_Data.MH = 0;
			}
		}


		/*接受数据*/
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
		DelayXms(10);	
	}

}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		
	{
		IWDG_ReloadCounter();
		/*发送数据*/
		AD_Getarrange();
		Read_DHT11();					//获取温湿度数据
		
		OneNet_SendData();				//发送数据

		ESP8266_Clear();	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);												
	}
}






