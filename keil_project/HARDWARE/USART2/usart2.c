#include "usart2.h"
#include "stdio.h"
#include "delay.h"
#include "led.h"

#define HEADER_1 0x0A  // 报头第一个字节
#define HEADER_2 0x0B  // 报头第二个字节

u8 Res1[2] = {0};     // 存储接收到的两个8位数据:第一个表示命令，第二个表示数据
u8 header_flag = 0;   // 报头检测标志
u8 data_count = 0;    // 数据接收计数
u8 rec_finish =0;

void usart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口1 

}
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        u8 received_data = USART_ReceiveData(USART2);  // 读取接收到的数据

        if (header_flag == 0) {
            // 检测报头第一个字节
            if (received_data == HEADER_1) {
                header_flag = 1;
            }
        } else if (header_flag == 1) {
            // 检测报头第二个字节
            if (received_data == HEADER_2) {
                header_flag = 2;  // 报头检测完成
                data_count = 0;   // 重置数据接收计数
            } else {
                header_flag = 0;  // 如果不是第二个字节，重置报头检测
            }
        } else if (header_flag == 2) {
            // 接收两个8位数据
            if (data_count < 2) {
                Res1[data_count] = received_data;  // 存储数据
                data_count++;
            }

            // 如果已经接收完两个数据，重置状态
            if (data_count >= 2) {
                header_flag = 0;  // 重置报头检测
                data_count = 0;   // 重置数据接收计数
                rec_finish =1;    //新数据接收完成
            }
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // 清除中断标志
    }
}

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendU16AsString(USART_TypeDef* USARTx, uint16_t data) {
    int i=0;
    char buffer[6];  // 16位数据的最大值是65535，加上字符串结束符'\0'
    snprintf(buffer, sizeof(buffer), "%u", data);  // 将16位数据转换为字符串
    for ( i = 0; buffer[i] != '\0'; i++) {
        Usart_SendByte(USARTx, buffer[i]);  // 逐个发送字符
    }
}

u8 Get_Res1_1(void)
{
    return Res1[0];
}

u8 Get_Res1_2(void)
{
    return Res1[1];
}

