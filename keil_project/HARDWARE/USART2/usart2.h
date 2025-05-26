#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"  

extern u8 rec_finish;

void usart2_init(u32 bound);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendU16AsString(USART_TypeDef* USARTx, uint16_t data) ;
u8 Get_Res1_1(void);
u8 Get_Res1_2(void);

#endif


