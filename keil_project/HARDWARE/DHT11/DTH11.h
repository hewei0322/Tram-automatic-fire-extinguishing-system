#ifndef _DTH11_H_
#define _DTH11_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"

#define DHT_GPIO_PORT		GPIOB
#define DHT_GPIO_PIN		GPIO_Pin_0
#define DHT_RCC_PORT		RCC_APB2Periph_GPIOB

uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[]);
#endif

