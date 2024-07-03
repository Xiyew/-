#include "stm32f10x.h"

void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   //解除看门狗写保护

    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(2499);                           //16000ms

    IWDG_ReloadCounter();                           //喂狗
    IWDG_Enable();
}
