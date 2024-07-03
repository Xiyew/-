#ifndef __AD_H
#define __AD_H

typedef struct
{
    uint16_t        MH;             //LED亮度
    uint16_t        light;          //光敏
    uint16_t        LM2904;         //噪声
    double          MQ135;          //空气质量
    double          volt;           //电压
    uint16_t        ACS712;         //电流
}Sensor_Typedef;

/*
    Vaule_temp = {空气质量，光敏，分贝，电压，电流}
*/
typedef struct          //行为采样次数,列为传感器数量
{
    uint16_t        Vaule_temp[5];  //实时的AD采样数据
    uint16_t        Value[10][5];   //10行5列,用于存储十次AD采样值
    uint16_t        ArrValue[5];    //10次AD采样值的平均值
    double          Voltage[5];     //AD转换电压
    Sensor_Typedef  Sensor_Data;
}ADdata_Typedef;

typedef struct
{
	_Bool Led_Status;
}LED_Typedef;

extern ADdata_Typedef AD_Data;
extern LED_Typedef Led_INFO;

void AD_Init(void);
void AD_Getarrange(void);

#endif
