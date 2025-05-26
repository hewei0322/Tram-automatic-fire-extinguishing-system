#include "flame.h"

void Flame_Init(void)//IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTC时钟

	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;       //PA4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //设置成上拉输入，平时为高，按下时低电平拉低
 	GPIO_Init(GPIOA, &GPIO_InitStructure);           //初始化GPIOA4
}