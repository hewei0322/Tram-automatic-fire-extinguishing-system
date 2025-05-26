#include "usart2.h"
#include "stdio.h"
#include "delay.h"
#include "led.h"

#define HEADER_1 0x0A  // ��ͷ��һ���ֽ�
#define HEADER_2 0x0B  // ��ͷ�ڶ����ֽ�

u8 Res1[2] = {0};     // �洢���յ�������8λ����:��һ����ʾ����ڶ�����ʾ����
u8 header_flag = 0;   // ��ͷ����־
u8 data_count = 0;    // ���ݽ��ռ���
u8 rec_finish =0;

void usart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

}
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        u8 received_data = USART_ReceiveData(USART2);  // ��ȡ���յ�������

        if (header_flag == 0) {
            // ��ⱨͷ��һ���ֽ�
            if (received_data == HEADER_1) {
                header_flag = 1;
            }
        } else if (header_flag == 1) {
            // ��ⱨͷ�ڶ����ֽ�
            if (received_data == HEADER_2) {
                header_flag = 2;  // ��ͷ������
                data_count = 0;   // �������ݽ��ռ���
            } else {
                header_flag = 0;  // ������ǵڶ����ֽڣ����ñ�ͷ���
            }
        } else if (header_flag == 2) {
            // ��������8λ����
            if (data_count < 2) {
                Res1[data_count] = received_data;  // �洢����
                data_count++;
            }

            // ����Ѿ��������������ݣ�����״̬
            if (data_count >= 2) {
                header_flag = 0;  // ���ñ�ͷ���
                data_count = 0;   // �������ݽ��ռ���
                rec_finish =1;    //�����ݽ������
            }
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // ����жϱ�־
    }
}

/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendU16AsString(USART_TypeDef* USARTx, uint16_t data) {
    int i=0;
    char buffer[6];  // 16λ���ݵ����ֵ��65535�������ַ���������'\0'
    snprintf(buffer, sizeof(buffer), "%u", data);  // ��16λ����ת��Ϊ�ַ���
    for ( i = 0; buffer[i] != '\0'; i++) {
        Usart_SendByte(USARTx, buffer[i]);  // ��������ַ�
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

