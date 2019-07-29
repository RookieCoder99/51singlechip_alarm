/*****************************************************************************
* 文件名：main.c
*****************************************************************************/
#include <stdio.h>
#include "delay.h"
#include "main.h"
#include "ds1302.h"
#include "ds18B20.h"
#include "hb.h"
#include "iic.h"
#include "yuyin.h"
#include "alarm.h"
#include "main2233.h"
#include "switch_screen.h"
/****************闹钟功能部分变量*******************************/
//周几是否有闹钟标志
uchar  alarm_show_flag[2][7]=
{
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff}
};
bit screen_num_flag=1;
bit leap_month_flag=0;
bit light=0;//液晶亮不亮
uchar down_temper=0, up_temper=30;	//温度上、下限；
uchar wifi_light_flag;
uchar  wifi_light=1;
uchar night_light=0;
uchar festival_num;
bit start_music=1;
uchar broadcast_morning_flag_flag;
uchar broadcast_now_time_flag_flag;
uchar broadcast_morning_flag=1;//早上是否七点播报
uchar broadcast_now_time_flag=1;//整点是否播报
//闹钟选中位置选项0-none;1-sun;2-mon;..
uchar  alarm_pos=0;//
uchar  alarm_set=0;//当前为第几个闹钟,0,代表时钟界面
uchar  music_num=1;
uchar  volume=1;
uchar  pbuff[2] = {0,0};		//温度整数、小数部分
uchar sleep_flag;
uchar broadcast_morning_flag_tmp,broadcast_now_time_flag_tmp,wifi_light_tmp;
uchar  screen_num=0;  //界面号:0-normal;1-only time;2-only date.
/***********************************
	光标的位置
	0-none;1-year;2-month;3-date;4-hour;5-min
*************************************/
uchar  cur_pos=0;
SYSTEM_TIME str_time={17,7,23,12,1,0};//系统时间
SYSTEM_TIME str_time_lunar;//农历时间
SYSTEM_TIME str_time_alarm[2]={{0,0,0,1,0,0},{0,0,0,3,0,0}};
SYSTEM_TIME str_time_alarm_flag[2];
SYSTEM_TIME str_time_sleep[2]={{0,0,0,22,30,0},{0,0,0,7,30,0}};

/******************************************************
* 主函数：完成系统功能
******************************************************/

void main(void)
{
 	uchar check_sensor=0x00;//检测人体感应的标志
	uchar check_sensor2=0x01;
	ds1302_init();		//DS1302端口初始化
	set_time(str_time);
	InitUART();
	init_IIC();
	EA = 1;
	temper_convert();	//启动温度转换
	delay_ms(30);
	
	/*********以下从E2PROM中读取上次设置的数据**********/
	down_temper=read_IIC(DAT_ADDR);
	up_temper=read_IIC(DAT_ADDR+1);
	start_music=read_IIC(START_MUSIC_ADDR);
	alarm_read_iic();
	volume=read_IIC(VOLUME_ADDR);
	music_num=read_IIC(MUSIC_IIC_ADDR);
	screen_num=read_IIC(SCREEN_NUM_ADDR);
	broadcast_morning_flag=read_IIC(MORNING_ADDR);
	broadcast_now_time_flag=read_IIC(NOW_TIME_ADDR);
	wifi_light=read_IIC(WIFI_LIGHT_ADDR);
	broadcast_morning_flag_flag=read_IIC(MORNING_FLAG_ADDR);
	broadcast_now_time_flag_flag=read_IIC(NOW_TIME_FLAG_ADDR);
	wifi_light_flag=read_IIC(WIFI_LIGHT_FLAG_ADDR);
	str_time_sleep[0].hour=read_IIC(SLEEP0_ADDR);
	str_time_sleep[0].min=read_IIC(SLEEP0_ADDR+1);
	str_time_sleep[1].hour=read_IIC(SLEEP1_ADDR);
	str_time_sleep[1].min=read_IIC(SLEEP1_ADDR+1);
	sleep_flag=read_IIC(SLEEP_FLAG_ADDR);
	hb_init();
	//wifi_init();
	
	while(1)
	{
		cur_pos=0;
		draw_frame();
		/*************显示时间界面************/
		show_time_screen();
		
		/***************人体红外传感器************/
		if(wifi_light==1)
		{
			if(light==0)
			{
				if(HUMAN_SENSOR==0 && check_sensor==1)
				{
					check_sensor=0;
					Screen_Con(LIGHT_SET);
					light=~light;
					Delay_1ms(DELAY_TIME);
				}
				if(HUMAN_SENSOR==1 && check_sensor==0)
				{
					check_sensor=1;
					Screen_Con(LIGHT_SET);
					light=~light;
				}
			}
			else
			{
				if(HUMAN_SENSOR==0 && check_sensor2==1)
				{
					check_sensor=0;
					Screen_Con(LIGHT_SET);
					light=~light;
					Delay_1ms(DELAY_TIME);
				}
				if(HUMAN_SENSOR==1 && check_sensor2==0)
				{
					check_sensor=1;
					Screen_Con(LIGHT_SET);
					light=~light;
				}
				
			}
		}
		else if(wifi_light==2)
		{
			if(light==0)
			{
				Screen_Con(LIGHT_SET);
				light=~light;
				if(night_light==1)
			  {
				  delay_ms(1000);
					night_light=0;
			  }
			}
			
		}
		else if(wifi_light==0)
		{
			if(light==1)
			{
				Screen_Con(LIGHT_SET);
				light=~light;
			}
		}
		/************判断闹钟是否到时间响******************/
		check_alarm_time(str_time, str_time_alarm,alarm_show_flag);
		
		if(str_time.min==0 && str_time.sec==0)
		{
			if(str_time.hour==8 && broadcast_morning_flag==1)
			{
				broadcast_morning(str_time);
				broadcast_temp(pbuff);
			}
		 else if(broadcast_now_time_flag==1 )
			{
				check_now_time(str_time);
				broadcast_temp(pbuff);
			}
		}
	/****************切换时间显示界面***************/
		switch_screen();
	/****************闹钟音乐显示界面***************/
		music_screen();
	/**********切换光标位置以调节时间***********/
		adjust_time();
	/*************调节声音*************************/
		volume_control();
	/*******************************************************
	*闹钟功能实现主体
	*包括按键进入闹钟，闹钟调时间，选择闹钟音乐等功能
	********************************************************/
		alarm_function();
		/*************app控制单片机***************************/
		//Get_Clinet();
		
		if(sleep_flag==1)
		{
			if((str_time.hour>str_time_sleep[0].hour 
				||(str_time.hour==str_time_sleep[0].hour &&
						str_time.min>=str_time_sleep[0].min)) ||
			
				(str_time.hour<str_time_sleep[1].hour 
				||(str_time.hour==str_time_sleep[1].hour &&
						str_time.min<=str_time_sleep[1].min)))
			{
				wifi_light=0;
				broadcast_morning_flag=0;
				broadcast_now_time_flag=0;
				if(!ALARM)
				{
					delay_ms(350);
					if(!ALARM)
					{
							night_light=1;
							wifi_light=2;
					}
				}
			}
			else
			{
					wifi_light=wifi_light_flag;
  				broadcast_morning_flag=broadcast_morning_flag_flag;
					broadcast_now_time_flag=broadcast_now_time_flag_flag;
			}
		}
		else
		{
			wifi_light=wifi_light_flag;
  		broadcast_morning_flag=broadcast_morning_flag_flag;
			broadcast_now_time_flag=broadcast_now_time_flag_flag;
		}
		
		/***************摇晃可以切换背光**********************/
		
//			if(SHAKE==0)
//			{
//				delay_ms(5);
//				if(SHAKE==0)
//				{
//					wifi_light+=1;
//					if(wifi_light==1)
//					{
//						wifi_light_flag=1;
//					}
//					else if(wifi_light==2)
//					{
//						wifi_light_flag=2;
//					}
//					if(wifi_light==3)
//					{
//						wifi_light=0;
//						wifi_light_flag=0;
//					}
//				}
//			}
		
	}
}