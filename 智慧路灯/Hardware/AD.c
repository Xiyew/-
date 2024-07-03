#include "stm32f10x.h"
#include "AD.h"
#include "math.h"

ADdata_Typedef AD_Data;
LED_Typedef Led_INFO = {0};
uint8_t times = 0;
uint8_t Row = sizeof(AD_Data.Value) / sizeof(AD_Data.Value[0]);       //求行 
uint8_t Len = sizeof(AD_Data.Value[0]) / sizeof(AD_Data.Value[0][0]); //求列

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA 
                            | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       //模拟输入断开GPIO，ADC专属模式
                                                        //防止GPIO口输入输出对模拟电压造成干扰
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_NbrOfChannel = Len;
    ADC_Init(ADC1, &ADC_InitStructure);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Data.Vaule_temp;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = Len;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//使能DMA传输完成中断

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    /*ADC校准*/
    ADC_ResetCalibration(ADC1);                         //开始复位校准
    while(ADC_GetResetCalibrationStatus(ADC1) == SET);  //等待复位校准是否完成
    ADC_StartCalibration(ADC1);                         //开始校准
    while(ADC_GetCalibrationStatus(ADC1) == SET);       //等待校准是否完成

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);             //连续转换模式使用
}

/*
************************************************************
*	函数名称：	AD_Getarrange
*
*	函数功能：	AD值求平均值
*
*	入口参数：	sum单个传感器AD值总和 Row单个传感器AD值存储次数
*
*	返回参数：	AD_Data.ArrValue平均AD值 AD_Data.Voltage平均电压 
*
*	说明：		无
************************************************************
*/
void AD_Getarrange(void)
{
    uint16_t sum;
    if(AD_Data.Value[Row - 1][Len - 1] != 0)
    {
        for(uint8_t y = 0; y < Len; y++)
        {
            for (uint8_t x = 0; x < Row; x++)
            {
                sum += AD_Data.Value[x][y];
            }
            AD_Data.ArrValue[y] = sum / Row;
            AD_Data.Voltage[y]  = AD_Data.ArrValue[y] * 3.3 / 4095;

            AD_Data.Sensor_Data.MQ135   = (double)pow((3.4880 * 10 * AD_Data.ArrValue[0]) / (4095 - AD_Data.ArrValue[0]), (1.0 / 0.3203));
            AD_Data.Sensor_Data.light   = 4095 - AD_Data.ArrValue[1]; 
            AD_Data.Sensor_Data.LM2904  = AD_Data.ArrValue[2] * 120 / 4095;
            AD_Data.Sensor_Data.ACS712  = ((2.5 - AD_Data.ArrValue[3] * 3.3 / 4095) / 0.185) * 1000;
            AD_Data.Sensor_Data.volt    = (double)(AD_Data.ArrValue[4] * 3.3 * 5 / 4095);
			sum = 0;

        }
    }
}

/*
************************************************************
*	函数名称：	DMA1_Channel1_IRQHandler
*
*	函数功能：	DMA转运完成中断
*
*	入口参数：	times转运完成次数 AD_Data.Vaule_temp为DMA转运中转
*
*	返回参数：	AD_Data.Value各个传感器AD值的多次采样数据
*
*	说明：		存储多次AD采样数据
************************************************************
*/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
        times++;

        for (uint8_t y = 0; y < Len; y++)
        {
            AD_Data.Value[times][y] = AD_Data.Vaule_temp[y];
        }
        if(times == Row - 1)
        {
            times = 0;
        }
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}
