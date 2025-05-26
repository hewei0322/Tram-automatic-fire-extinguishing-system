#ifndef _DHT_H_
#define _DHT_H_

#include "sys.h"

//SHT11程序


#define noACK 0				//继续传输数据，用于判断是否结束通讯
#define ACK   1             //结束数据传输；
							//地址  命令  读/写
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

//SHT11接口
#define SHT11_SCK_PORT  			GPIOB
#define SHT11_SCK_PIN				GPIO_Pin_0
#define SHT11_SCK_GPIO_RCC   RCC_APB2Periph_GPIOB

#define SHT11_DATA_PORT  			GPIOB
#define SHT11_DATA_PIN				GPIO_Pin_1
#define SHT11_DATA_GPIO_RCC   RCC_APB2Periph_GPIOB

#define SCK PBout(0)
#define DATA PBout(1)

#define  SHT11_DATA_IN()	   GPIO_ReadInputDataBit(SHT11_DATA_PORT,SHT11_DATA_PIN)

typedef union		   //定义共同类型
{ 
  unsigned int i;	   //i表示测量得到的温湿度数据（int 形式保存的数据） 
  float f;			   //f表示测量得到的温湿度数据（float 形式保存的数据）
} value;

extern int real_tempure;		//实际温度
extern char real_humi;


void SHT11_GPIO_Config(void);
static void SHT11_Mode_IPU(void);
static void SHT11_Mode_Out_PP(void);
char SHTXX_write_byte(unsigned char value)  ;
char SHTXX_read_byte(unsigned char ack);
void SHTXX_transstart(void);
void SHTXX_Init(void);
char SHTXX_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void SHTXX_calc(float *p_humidity ,float *p_temperature);
void Covert_HT(void);



#endif

