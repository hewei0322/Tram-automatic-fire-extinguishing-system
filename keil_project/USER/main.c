#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "led.h"
#include "oled.h"
#include "time2.h"
#include "ds18b20.h"
#include "usart2.h"
#include "adc.h"
#include "beep.h"
#include "flame.h"
#include "switch.h"  //����ˮ��

u8 key=0,set=0;           //������ֵ  ϵͳ����״̬
u8 number=0;              //
u8 function_flag=0;           

float temp=0;    //�¶�ֵ

u8 temp_fan=70;   //�¶Ƚ���ֵ
u8 temp_flame_warm=90;  //�¶Ȼ��ֱ���ֵ

u16 yanwu;            //����
u16 yanwu_warm=80;    //������ֵ����λ��ppm


u16 detect_count=0;                    //�ɼ�������
u16 control_count=0;                    //���Ƽ�����

u8 flame_flag=0;   //�����־λ


int main(void)
{
    unsigned char p[50]=" ";
    u8 i=0;
    u16 temp_adc;
    delay_init();                                  //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    LED_Init();                                    
    KEY_Init();                                    
    OLED_Init();                                   
    //TIM2_Init(4999,7199);                        
    Adc_Init();                                    
    usart2_init(9600);                             
    BEEP_Init();                                   
    switch_Init();
    while(DS18B20_Init())                     //�¶ȴ�������ʼ��
    {
        OLED_ShowString(20, 6, "Wait!!!!!", 8); 
        delay_ms1(100);
    }
    OLED_Clear();
    
    //OLED��ʾ���綯�����ϵͳ
    OLED_ShowCHinese12(20,0,0);
    OLED_ShowCHinese12(32,0,1);
    OLED_ShowCHinese12(44,0,2);
    OLED_ShowCHinese12(56,0,3);
    OLED_ShowCHinese12(68,0,4);
    OLED_ShowCHinese12(80,0,5);
    OLED_ShowCHinese12(92,0,6);

    while(1)
    {
        detect_count++;
        key=KEY_Scan(0);
        if(key==4)
        {
            set++;
            if(set>1)
                set=0;
            OLED_Clear();
            
            
            //��ʾ�̶�����
            if(set==0)  //���ģʽ
            {
                //OLED��ʾ���綯�����ϵͳ
                OLED_ShowCHinese12(20,0,0);
                OLED_ShowCHinese12(32,0,1);
                OLED_ShowCHinese12(44,0,2);
                OLED_ShowCHinese12(56,0,3);
                OLED_ShowCHinese12(68,0,4);
                OLED_ShowCHinese12(80,0,5);
                OLED_ShowCHinese12(92,0,6);
            }
            else if(set==1)  //���ù���ѡ��ģʽ
            {
                //OLED��ʾ���¶���ֵ����
                OLED_ShowCHinese12(20,0,7);
                OLED_ShowCHinese12(32,0,8);
                OLED_ShowCHinese12(44,0,15);
                OLED_ShowCHinese12(56,0,16);
                OLED_ShowCHinese12(68,0,17);
                OLED_ShowCHinese12(80,0,18);
            }
        }
        if(set==0)  //���ģʽ
        {
            if(detect_count>=100)   //ÿ100�βɼ�һ������
            {
                detect_count=0;
                //1.��ʪ�ȼ��
                temp = DS18B20_Get_Temp();
                OLED_ShowCHinese12(0,2,7);
                OLED_ShowCHinese12(12,2,8);
                sprintf((char*)p,":%4.1f",temp);
                OLED_ShowString(24,2,(uint8_t *)p,16);
                OLED_ShowCHinese12(72,2,25);

                //2.������
                temp_adc=Get_Adc(ADC_Channel_4);
                yanwu=temp_adc/10;    //�������Ũ��
                OLED_ShowCHinese12(0,4,13);
                OLED_ShowCHinese12(12,4,14);
                sprintf((char*)p,":%3dppm",yanwu);
                OLED_ShowString(24,4,(uint8_t *)p,16);
                //3.������
                flame_flag=FLAME;
                OLED_ShowCHinese12(0,6,11);
                OLED_ShowCHinese12(12,6,12);
                OLED_ShowString(24,6,":",16);
                if(flame_flag)
                    OLED_ShowCHinese12(32,6,26);
                else
                    OLED_ShowCHinese12(32,6,27);
                
                //4.���·��ȿ���
                if(temp>temp_fan)
                    switch_fan=1;
                else
                    switch_fan=0;
                
                //5.�¶��ٸߣ��������   ���л��汨����� �����������
                if(temp>temp_flame_warm || flame_flag || yanwu>yanwu_warm)
                {
                    BEEP=1;
                    switch_water=1;
                }
                else
                {
                    BEEP=0;
                    switch_water=0;
                }
                
            
            }
        
        }
        else if(set==1)
        {
            
            if(key==3)
                function_flag=!function_flag;
            else if(key==1)
            {
                if(function_flag && temp_fan<=300)
                    temp_fan++;
                else if(function_flag==0&&temp_flame_warm<300)
                    temp_flame_warm++;
                    
            }
            else if(key==2)
            {
                if(function_flag&& temp_fan>0)
                    temp_fan--;
                else if(function_flag==0&&temp_flame_warm>0)
                    temp_flame_warm--;
                    
            }
            sprintf((char*)p,"F:%2dC  W:%2dC  ",temp_fan,temp_flame_warm);
            OLED_ShowString(0,4,(uint8_t *)p,16);
            if(function_flag)
                OLED_ShowString(40,4,"<",16);
            else
                OLED_ShowString(96,4,"<",16);
        }
    }
            
}




