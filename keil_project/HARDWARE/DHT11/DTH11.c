#include "DTH11.h"
 

/**
  * @brief  DHT11_GPIO初始化函数
  * @param  Mode：指定输入或输出模式
  * @return None
  */
// DHT_GPIO_Init 修改输入模式为IPU
void DHT_GPIO_Init(GPIOMode_TypeDef Mode) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DHT_RCC_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DHT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = Mode;
    GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStructure);
    
}

// 修改后的DHT_Start函数（含超时）
uint8_t DHT_Start(void) {
    uint32_t timeout = 10000;
    DHT_GPIO_Init(GPIO_Mode_Out_PP);
    GPIO_ResetBits(DHT_GPIO_PORT, DHT_GPIO_PIN);
    delay_ms(20);
    GPIO_SetBits(DHT_GPIO_PORT, DHT_GPIO_PIN);
    
    DHT_GPIO_Init(GPIO_Mode_IPU);
    delay_us(30);
    
    if (!GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN)) {
        while (!GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) && timeout--);
        if (!timeout) return 0;
        
        timeout = 10000;
        while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) && timeout--);
        return timeout ? 1 : 0;
    }
    return 0;
}
/**
  * @brief  接收DHT11发送来8位的数据
  * @param  None
  * @return 返回接收到的8位数据
  */
uint8_t DHT_Get_Byte_Data(void)
{
	uint8_t temp;
    int i = 0;
	for(i = 0; i < 8; i++)
	{
		temp <<= 1;
		while(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
		delay_us(28);
		GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN) ? (temp |= 0x01) : (temp &= ~0x01);
		while(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	}
	return temp;
}

/**
  * @brief  获取DHT11的温度湿度数据
  * @param  buffer[]：需要传入一个存储数据的数组
  * @return 返回数据校验是否正确 1：正确 0：失败
  */
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[])
{
	if(DHT_Start())
	{
		buffer[0] = DHT_Get_Byte_Data();
		buffer[1] = DHT_Get_Byte_Data();
		buffer[2] = DHT_Get_Byte_Data();
		buffer[3] = DHT_Get_Byte_Data();
		buffer[4] = DHT_Get_Byte_Data();
	}
	return (buffer[0]+buffer[1]+buffer[2]+buffer[3] == buffer[4]) ? 1 : 0;
}



