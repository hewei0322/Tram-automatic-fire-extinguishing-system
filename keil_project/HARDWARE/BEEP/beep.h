#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

//蜂鸣器端口定义
#define BEEP PAout(1)	// BEEP,蜂鸣器接口			   

void BEEP_Init(void);	//初始化
		 				    
#endif

