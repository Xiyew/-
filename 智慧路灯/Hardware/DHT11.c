#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DHT11.h"

DHT11_TypeDef DHT11_Data;

void DHT11_DATA_OUT(int8_t Value)	
{
    if(Value == 1)
    {
        GPIO_SetBits(DHT11, DHT11_Out_Pin);
    }
    else
    {
        GPIO_ResetBits(DHT11, DHT11_Out_Pin);
    }
}   
					
void DHT11_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DHT11_Out_RCC, ENABLE); 
  	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(DHT11, &GPIO_InitStructure);		  

	GPIO_SetBits(DHT11, DHT11_Out_Pin);	 
}

static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(DHT11, &GPIO_InitStructure);	 
}

static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(DHT11, &GPIO_InitStructure);	 	 
}

static uint8_t Read_Byte(void)
{
	uint8_t Data = 0;

	for (uint8_t i = 0; i < 8; i++)    
	{	 
		while (GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_RESET);
		DelayUs(40);    	  
		if (GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_SET)
		{
			while(GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_SET);
			Data |= (uint8_t)(0x01 << (8 - i - 1));
		}
		else
		{			   
			Data &= (uint8_t) ~ (0x01<<(8 - i - 1)); 
		}
	}
	return Data;
}

uint8_t Read_DHT11(void)
{  
	DHT11_Mode_Out_PP();
	DHT11_DATA_OUT(LOW);
	DelayXms(18);
	DHT11_DATA_OUT(HIGH); 
	DelayUs(30);  
	DHT11_Mode_IPU();

	if(GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_RESET)     
	{
		while(GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_RESET);
		while(GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin) == Bit_SET);

        DHT11_Data.humiH = Read_Byte();
        DHT11_Data.humiL = Read_Byte();
        DHT11_Data.tempH = Read_Byte();
        DHT11_Data.tempL = Read_Byte();
        DHT11_Data.check = Read_Byte();

		DHT11_Mode_Out_PP();
		DHT11_DATA_OUT(HIGH);

        if(DHT11_Data.check == DHT11_Data.humiH + DHT11_Data.humiL
							 + DHT11_Data.tempH + DHT11_Data.tempL)
        {
			DHT11_Data.humi = (float)(DHT11_Data.humiH) + (float)(DHT11_Data.humiL)/10;
			DHT11_Data.temp = (float)(DHT11_Data.tempH) + (float)(DHT11_Data.tempL)/10;
            return SUCCESS;
        }
		else 
        {
            return ERROR;
        }
			
	}
	else
	{		
		return ERROR;
	}   
}
