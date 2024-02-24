#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "Encoder.h"

int main(void)
{
	int16_t i;
	OLED_Init();
	PWM_Init();
	Encoder_Init();
	while(1)
	{
		i += Encoder_Get();
		if(i >= 0 && i <= 100)
		{
			PWM_SetCompare1(i);
			OLED_ShowNum(1, 1, i, 3);
			//Delay_ms(10);
		}
		else if(i < 0)
		{
			i = 0;
		}
		else if(i > 100)
		{
			i = 100;
		}
	}
}


