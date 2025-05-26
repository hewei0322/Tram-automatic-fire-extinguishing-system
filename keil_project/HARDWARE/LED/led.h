#ifndef	__LED_H
#define	__LED_H

#include "stm32f10x.h"                  // Device header

#define LED0 PCout(13)
#define LED1 PBout(2)

void LED_Init(void);
void LED_Toggle(void);
void LED_On(void);
void LED_Off(void);
void LED_Twinkle(void);
#endif
