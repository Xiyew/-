#include "stm32f10x.h"

int16_t Encoder_Count;

void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_Pin_5
                                             |GPIO_Pin_6);

    EXTI_InitTypeDef EXTI_InitStrusture;
    EXTI_InitStrusture.EXTI_Line = EXTI_Line5 | EXTI_Line6;
    EXTI_InitStrusture.EXTI_LineCmd = ENABLE;
    EXTI_InitStrusture.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStrusture.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStrusture);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}

int16_t Encoder_Get(void)
{
    int16_t temp;
    temp = Encoder_Count;
    Encoder_Count = 0;
    return temp;
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line6) == SET)
    {
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
        {
            Encoder_Count += 10;
        }
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    else if(EXTI_GetITStatus(EXTI_Line5) == SET)
    {
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
        {
            Encoder_Count -= 10;
        }
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}
