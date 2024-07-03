#include "stm32f10x.h"

#include "time.h"
#include "MyRTC.h"
#include "Delay.h"

#include "usart.h"
#include "OLED.h"

#include "esp8266.h"
#include "cJSON.h"

#include "stdlib.h"
#include "string.h"

MyTime_TypeDef RTC_Time = {2024, 1, 2, 0, 0, 1};

void MyRtc_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);

    PWR_BackupAccessCmd(ENABLE);

    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)   //检测是否主电源与备用电源均断电
    {
        RCC_LSEConfig(RCC_LSE_ON);
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        
        RTC_WaitForSynchro();       //等待同步
        RTC_WaitForLastTask();      //等待上一次写入完成

        RTC_SetPrescaler(32768 - 1);
        RTC_WaitForLastTask();

        MyRTC_SetTime();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        RTC_WaitForSynchro(); 
        RTC_WaitForLastTask();
    }
}

void MyRTC_SetTime(void)
{
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_date.tm_year = RTC_Time.year - 1900;		//将数组的时间赋值给日期时间结构体
	time_date.tm_mon  = RTC_Time.mon  - 1;
	time_date.tm_mday = RTC_Time.day;
	time_date.tm_hour = RTC_Time.hour;
	time_date.tm_min  = RTC_Time.min;
	time_date.tm_sec  = RTC_Time.sec;
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//调用mktime函数，将日期时间转换为秒计数器格式
													//- 8 * 60 * 60为东八区的时区调整
	
	RTC_SetCounter(time_cnt);						//将秒计数器写入到RTC的CNT中
	RTC_WaitForLastTask();							//等待上一次操作完成
}

/**
  * 函    数：RTC读取时间
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，RTC硬件电路里时间值将刷新到全局数组
  */
void MyRTC_ReadTime(void)
{
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;		//读取RTC的CNT，获取当前的秒计数器
													//+ 8 * 60 * 60为东八区的时区调整
	
	time_date = *localtime(&time_cnt);				//使用localtime函数，将秒计数器转换为日期时间格式
	
	RTC_Time.year = time_date.tm_year + 1900;		//将日期时间结构体赋值给数组的时间
	RTC_Time.mon  = time_date.tm_mon  + 1;
	RTC_Time.day  = time_date.tm_mday;
	RTC_Time.hour = time_date.tm_hour;
	RTC_Time.min  = time_date.tm_min;
	RTC_Time.sec  = time_date.tm_sec;
}


void Net_time(void)
{
	unsigned char *Time_Buff = NULL;
	cJSON *raw_json, *time_json;
	time_t time_cnt;

    /*连接网络时间网站*/
	ESP8266_Clear();
	while(ESP8266_SendCmd("+++",""));

	while(ESP8266_SendCmd("AT\r\n", "OK"))				//8266测试
		DelayXms(500);

	while(ESP8266_SendCmd("AT+RST\r\n", "OK"))			//复位
		DelayXms(500);

	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))		//配置为STA模式
		DelayXms(500);

	while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"))		//设置为单连接，否则无法使用透传
		DelayXms(500);

	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))	//连接WiFi
		DelayXms(500);

	while(ESP8266_SendCmd(ESP8266_TIME_INFO, "CONNECT"))//连接网络时间API
		DelayXms(500);

	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"))	//开启透传
		DelayXms(500);

	ESP8266_Clear();
	
	if(!ESP8266_SendCmd("AT+CIPSEND\r\n", ">"))				//收到‘>’时可以发送数据
	{
		Time_Buff = ESP8266_GetTime(ESP8266_TIME_GET, "{");
	}
	raw_json = cJSON_Parse((char*)Time_Buff);	//将原始字符串信息转换为CJSON格式

	ESP8266_Clear();

	while(ESP8266_SendCmd("+++",""));							//退出透传模式
	time_json = cJSON_GetObjectItem(raw_json, "server_time");	//处理网站返回数据
	char *server_time = cJSON_Print(time_json);
	cJSON_Delete(raw_json);
         
    char timestamp[11];
    strncpy(timestamp, server_time, 10);
    timestamp[10] = '\0'; 
    time_cnt = atoi(timestamp);

	RTC_SetCounter(time_cnt);							//将秒计数器写入到RTC的CNT中
	RTC_WaitForLastTask();								//等待上一次操作完成
    free(server_time);

	while(ESP8266_SendCmd("+++",""));					//退出数据模式
}
