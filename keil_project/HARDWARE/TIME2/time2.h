#ifndef __TIME2_H
#define __TIME2_H	 
#include "sys.h"

extern u8 finish_count;  //������ϱ�־λ
extern float WATER;    //ˮλ

extern u16 feed_food_w;            //Ͷι����
extern u16 food_weight;        //ʳ������

extern uint8_t feed_time_hour[3];
extern uint8_t feed_time_min[3];
extern uint8_t feed_food_begin;

extern float WATER;            //ˮλ
extern float water_w;        //��7cmֹͣ��ˮ
extern uint8_t feed_water_begin;            //1:������ˮ  2��ֹͣ��ˮ

extern u8 step_control;

void TIM2_Init(u16 arr,u16 psc);//��ʼ��

		 				    
#endif

