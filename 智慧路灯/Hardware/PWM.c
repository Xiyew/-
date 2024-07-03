#include "stm32f10x.h"                  // Device header
#include "AD.h"

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_InternalClockConfig(TIM3);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 900 - 1;         //ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;      //PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;      //CCR

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

void PWM_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(TIM3, Compare);
}

void Lumi_Control(void)
{

    if(AD_Data.ArrValue[1] > 3200)
    {
        switch ((AD_Data.ArrValue[1] - 3200) / 100)
        {
            case 0: /*PWM_SetCompare3(100); AD_Data.Sensor_Data.MH = 10;  break;*/
            case 1: PWM_SetCompare3(200); AD_Data.Sensor_Data.MH = 20;  break;
            case 2: /*PWM_SetCompare3(300); AD_Data.Sensor_Data.MH = 30;  break;*/
            case 3: PWM_SetCompare3(400); AD_Data.Sensor_Data.MH = 40;  break;
            case 4: /*PWM_SetCompare3(500); AD_Data.Sensor_Data.MH = 50;  break;*/
            case 5: PWM_SetCompare3(600); AD_Data.Sensor_Data.MH = 60;  break;
            case 6: PWM_SetCompare3(700); AD_Data.Sensor_Data.MH = 70;  break;
            case 7: PWM_SetCompare3(800); AD_Data.Sensor_Data.MH = 85;  break;
            case 8: PWM_SetCompare3(900); AD_Data.Sensor_Data.MH = 100; break;
            
            default: break;
        }
    }
    else if(AD_Data.ArrValue[1] < 3000)
    {
        PWM_SetCompare3(0);
        AD_Data.Sensor_Data.MH = 0;
    }
    
}
