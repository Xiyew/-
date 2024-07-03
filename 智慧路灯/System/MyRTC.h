#ifndef __MYRTC_H
#define __MYRTC_H

#define ESP8266_TIME_INFO		"AT+CIPSTART=\"TCP\",\"api.pinduoduo.com\",80\r\n"
#define ESP8266_TIME_GET		"GET http://api.pinduoduo.com/api/server/_stm\r\n"

typedef struct
{
	uint16_t  year;		
	uint16_t  mon;	 	
	uint16_t  day;	 	
	uint16_t  hour;	 	
	uint16_t  min;	 
    uint16_t  sec;	                  
}MyTime_TypeDef;

extern MyTime_TypeDef RTC_Time;

void MyRtc_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
void Net_time(void);

#endif
