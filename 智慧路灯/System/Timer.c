#include "stm32f10x.h"                  // Device header

/*内部时钟使能*/
void Timer_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_InternalClockConfig(TIM2);    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseinitStructure;
    TIM_TimeBaseinitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseinitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseinitStructure.TIM_Period = 50000 - 1;    //ARR
    TIM_TimeBaseinitStructure.TIM_Prescaler = 7200 - 1; //PSC定时时间 = (ARR+1)/(PSC+1)/72M
    TIM_TimeBaseinitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseinitStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update); //防止一上电就立刻进入中断
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitTypeStructure;
    NVIC_InitTypeStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitTypeStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitTypeStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitTypeStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitTypeStructure);

    TIM_Cmd(TIM2, ENABLE);
}


