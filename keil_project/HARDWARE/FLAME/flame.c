#include "flame.h"

void Flame_Init(void)//IO��ʼ��
{
    GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTCʱ��

	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;       //PA4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //���ó��������룬ƽʱΪ�ߣ�����ʱ�͵�ƽ����
 	GPIO_Init(GPIOA, &GPIO_InitStructure);           //��ʼ��GPIOA4
}