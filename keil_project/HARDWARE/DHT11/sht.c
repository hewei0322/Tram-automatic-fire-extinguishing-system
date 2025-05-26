#include "sht.h"
#include "delay.h"

enum {TEMP,HUMI};

u8 TEMP_BUF[7];				 	//用于记录温度
u8 HUMI_BUF[5];				 	//用于记录湿度
 
int real_tempure=0;		//实际温度
char real_humi=0;

void SHT11_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启DHT11_PORT的外设时钟*/
	RCC_APB2PeriphClockCmd(SHT11_DATA_GPIO_RCC, ENABLE); 

	/*选择要控制的DHT11_PORT引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SHT11_SCK_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化DHT11_PORT*/
  	GPIO_Init(SHT11_SCK_PORT, &GPIO_InitStructure);		  

	/* 拉高GPIOB10	*/
	GPIO_SetBits(SHT11_SCK_PORT, SHT11_SCK_PIN);	 
}


static void SHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*选择要控制的DHT11_PORT引脚*/	
	  GPIO_InitStructure.GPIO_Pin = SHT11_DATA_PIN;

	   /*设置引脚模式为浮空输入模式*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 

	  /*调用库函数，初始化DHT11_PORT*/
	  GPIO_Init(SHT11_DATA_PORT, &GPIO_InitStructure);	 
}


static void SHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	 	/*选择要控制的DHT11_PORT引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = SHT11_DATA_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*调用库函数，初始化DHT11_PORT*/
  	GPIO_Init(SHT11_DATA_PORT, &GPIO_InitStructure);	 	 
}

//写字节程序
char SHTXX_write_byte(unsigned char value)   
{ 
	unsigned char i,error=0; 
	for (i=0x80;i>0;i/=2)             //shift bit for masking 高位为1，循环右移
	{ 
		if (i&value) DATA=1;          //和要发送的数相与，结果为发送的位
    	else DATA=0;                        
    	SCK=1;                          
    	delay_us(3);        //延时3us 
    	SCK=0;
	}
	DATA=1;                           //释放数据线
	SCK=1;                            
	error=DATA;                       //检查应答信号，确认通讯正常
	delay_us(3);        //延时3us 
	SCK=0;        
	DATA=1;
	return error;                     //error=1 通讯错误
}

//读字节程序
char SHTXX_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
{ 
	unsigned char i,val=0;
    SHT11_Mode_Out_PP();
	DATA=1;                           //释放数据线
    SHT11_Mode_IPU();
	for(i=0x80;i>0;i>>=1)             //高位为1，循环右移
	{ 
		SCK=1;                         
    	if(SHT11_DATA_IN()) val=(val|i);        //读一位数据线的值 
    	SCK=0;       
	}
    SHT11_Mode_Out_PP();
	DATA=!ack;                        //如果是校验，读取完后结束通讯；
	SCK=1;                            
	delay_us(3);        //延时3us 
	SCK=0;   
	delay_us(3);        //延时3us   
	DATA=1;                           //释放数据线
	return val;
}

//启动传输
void SHTXX_transstart(void)
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{ 
   	DATA=1; SCK=0;                   //准备
   	delay_us(1);
  	SCK=1;
   	delay_us(1);
   	DATA=0;
   	delay_us(1);
  	SCK=0; 
  	delay_us(3);
   	SCK=1;
   	delay_us(1);
  	DATA=1;     
  	delay_us(1);
   	SCK=0;     
}


//连接复位
void SHTXX_Init(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{ 
    SHT11_Mode_Out_PP();
	unsigned char i; 
	DATA=1; SCK=0;                    //准备
	for(i=0;i<9;i++)                  //DATA保持高，SCK时钟触发9次，发送启动传输，通迅即复位
	{ 
		SCK=1;
    	SCK=0;
	}
	SHTXX_transstart();                   //启动传输
}

 /*
//软复位程序
char SHTXX_softreset(void)
// resets the sensor by a softreset 
{ 
	unsigned char error=0; 
	SHTXX_Init();              //启动连接复位
	error+=SHTXX_write_byte(RESET);       //发送复位命令
	return error;                     //error=1 通讯错误
}
  */

           
//温湿度测量
char SHTXX_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
// 进行温度或者湿度转换，由参数mode决定转换内容；
{ 
//	enum {TEMP,HUMI};		 //已经在头文件中定义
	unsigned error=0;
	unsigned int i;

    SHT11_Mode_Out_PP();
	SHTXX_transstart();                   //启动传输
	switch(mode)                     //选择发送命令
    {	
		case TEMP : error+=SHTXX_write_byte(MEASURE_TEMP); break;		   //测量温度
    	case HUMI : error+=SHTXX_write_byte(MEASURE_HUMI); break;		   //测量湿度
    	default     : break; 
	}
	for (i=0;i<65535;i++) 
    {
        if(DATA==0) break; //等待测量结束
        delay_us(1);
    }
	if(DATA) error+=1;                // 如果长时间数据线没有拉低，说明测量错误 
	*(p_value) =SHTXX_read_byte(ACK);    //读第一个字节，高字节 (MSB)
	*(p_value+1)=SHTXX_read_byte(ACK);    //读第二个字节，低字节 (LSB)
	*p_checksum =SHTXX_read_byte(noACK); //read CRC校验码
	return error;					 // error=1 通讯错误
}

//温湿度值标度变换及温度补偿
void SHTXX_calc(float *p_humidity ,float *p_temperature)
{ 

	float rh=*p_humidity;             // rh:      12位 湿度 
	float t=*p_temperature;           // t:       14位 温度
	float rh_lin;                     // rh_lin: 湿度 linear值
	float rh_true;                    // rh_true: 湿度 ture值
	float t_C;                        // t_C   : 温度 ℃

	t_C=t*0.01 - 40;                  //补偿温度
	rh_lin=-0.0000028*rh*rh + 0.0405*rh - 4;     //相对湿度非线性补偿
	rh_true=rh/33;   
	if(rh_true>26)
		rh_true=rh/33+1;
	if(rh_true>33)
		rh_true=rh/33+1.5;
	if(rh_true>40)
		rh_true=rh/33+3;	
	if(rh_true>50)
		rh_true=rh/33+3.7;	
	if(rh_true>70)
		rh_true=rh/33+3;	
	if(rh_true>90)
		rh_true=rh/33+1.6;	

    //rh_true=(t_C-25)*(0.01+0.00008*rh)+rh_lin;   //相对湿度对于温度依赖性补偿	，仿真的时候去掉补偿

		
	if(rh_true>100)rh_true=100;       //湿度最大修正
	if(rh_true<0.1)rh_true=0.1;       //湿度最小修正

	*p_temperature=t_C;               //返回温度结果
	*p_humidity=rh_true;              //返回湿度结果


}


void Covert_HT()
{
	value humi_val,temp_val;		//定义两个共同体，一个用于湿度，一个用于温度
	u8 checksum;			//CRC	 
	u8 error=0;
	error=0;					   //初始化error=0，即没有错误
	error+=SHTXX_measure((unsigned char*)&temp_val.i,&checksum,TEMP); //温度测量
	error+=SHTXX_measure((unsigned char*)&humi_val.i,&checksum,HUMI); //湿度测量
	if(error!=0) SHTXX_Init();                 ////如果发生错误，系统复位
	else
	{ 
		humi_val.f=(float)humi_val.i;                   //转换为浮点数
 		temp_val.f=(float)temp_val.i;                   //转换为浮点数
 		SHTXX_calc(&humi_val.f,&temp_val.f);            //修正相对湿度及温度
		
		real_tempure =temp_val.f+1;	   //修正一下偏
		real_humi    = humi_val.f-1;

		if(real_tempure>=0)
		{
  		TEMP_BUF[0]=real_tempure/100+'0';     		//温度百位
		
		if (TEMP_BUF[0]==0x30) TEMP_BUF[0]=0x20;
		TEMP_BUF[1]=real_tempure%100/10+'0';     	//温度十位	
		if (TEMP_BUF[1]==0x30 && TEMP_BUF[0]==0x30) TEMP_BUF[1]=0x20;
		TEMP_BUF[2]=real_tempure%10+'0';			//温度个位 
		
		}
		else  //负温度
		{
			TEMP_BUF[0]	='-';
			TEMP_BUF[1]=(real_tempure*(-1))%100/10+'0';     	//温度十位	
			if (TEMP_BUF[1]==0x30 && TEMP_BUF[0]==0x30) TEMP_BUF[1]=0x20;
			TEMP_BUF[2]=(real_tempure*(-1))%10+'0';			//温度个位
			
		}		
		TEMP_BUF[3]=0xdf;
		TEMP_BUF[4]=0x43;
		HUMI_BUF[0]=real_humi/100+'0';     		//湿度百位
  		if (HUMI_BUF[0]==0x30) HUMI_BUF[0]=0x20;
		HUMI_BUF[1]=real_humi%100/10+'0';     	//湿度十位	
		if (HUMI_BUF[1]==0x30 && HUMI_BUF[0]==0x30) HUMI_BUF[1]=0X20;  
		HUMI_BUF[2]=real_humi%10+'0';			//湿度个位
		HUMI_BUF[3]= '%';

	}
}


