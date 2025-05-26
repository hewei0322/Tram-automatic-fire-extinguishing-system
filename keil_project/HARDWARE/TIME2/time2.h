#ifndef __TIME2_H
#define __TIME2_H	 
#include "sys.h"

extern u8 finish_count;  //计数完毕标志位
extern float WATER;    //水位

extern u16 feed_food_w;            //投喂重量
extern u16 food_weight;        //食物重量

extern uint8_t feed_time_hour[3];
extern uint8_t feed_time_min[3];
extern uint8_t feed_food_begin;

extern float WATER;            //水位
extern float water_w;        //到7cm停止加水
extern uint8_t feed_water_begin;            //1:开启加水  2：停止加水

extern u8 step_control;

void TIM2_Init(u16 arr,u16 psc);//初始化

		 				    
#endif

