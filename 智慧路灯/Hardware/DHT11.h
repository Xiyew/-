#ifndef _DHT11_H
#define __DHT11_H

#define DHT11_Out_Pin		GPIO_Pin_7
#define DHT11_Out_RCC		RCC_APB2Periph_GPIOA
#define DHT11		        GPIOA

#define HIGH  1
#define LOW   0

typedef struct
{ 
    uint8_t humiH; 
    uint8_t humiL; 
    float   humi;
    uint8_t tempH;
    uint8_t tempL;  
    float   temp;
    uint8_t check;
}DHT11_TypeDef;

extern DHT11_TypeDef DHT11_Data;

void DHT11_Init(void);
uint8_t Read_DHT11(void);

#endif
