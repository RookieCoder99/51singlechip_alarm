#include <stdio.h>
#include "delay.h"
#include "main.h"
#include "ds1302.h"
#include "ds18B20.h"
#include "hb.h"
#include "iic.h"
#include "alarm.h"
extern bit screen_num_flag;
extern SYSTEM_TIME str_time;
extern uchar screen_num;
extern uchar festival_num;
extern uchar pbuff[2];
extern SYSTEM_TIME str_time_lunar;
void show_time_screen()
{
	switch(screen_num)
	{
		case 0:
			str_time = read_time();		//读取时间	
			hb_show_time(str_time);
			read_temper(pbuff);		//读取当前温度值
			temper_convert();	// 启动温度转换，需要750ms
			hb_show_temp(pbuff[0], pbuff[1]);		//显示当前温度
			cal_festival();
			cal_jieqi( str_time);
			judge_leap_month(str_time.year,str_time.month,str_time.date);
			sun_to_lunar(0,str_time.year,str_time.month,str_time.date,1);
			break;
		case 1:
			str_time = read_time();		//读取时间	
			hb_show_big_time_only(str_time);
			break;
		case 2:
			if(screen_num_flag==1)
			{
				screen_num_flag=0;
				str_time = read_time();		//读取时间	
				hb_show_big_date_only(str_time);
				show_lunar(str_time.year,str_time.month,str_time.date);
				delay_ms(100);
				show_lunar_festival();		
				delay_ms(100);
				show_festival();	
				delay_ms(100);
				show_jieqi(str_time);
			}
			break;
	}
}

void switch_screen()
{
			if(ADD==0)
			{
				delay_ms(40);
				if(ADD==0)
				{
					screen_num+=1;
					if(screen_num==3)
					{
						screen_num=0;
					}
					clean_all();
					
					switch(screen_num)
					{
						case 0:
							clean_all();
							hb_show_time(str_time);
							read_temper(pbuff);		//读取当前温度值
							temper_convert();	// 启动温度转换，需要750ms
							hb_show_temp(pbuff[0], pbuff[1]);		//显示当前温度
							break;
						case 1:
							clean_all();
							hb_show_big_time_only(str_time);
							break;
						case 2:
							clean_all();
							hb_show_big_date_only(str_time);
							//read_temper(pbuff);	//读取当前温度值
							//temper_convert();	//启动温度转换，需要750ms
							//hb_show_big_temp(pbuff[0], pbuff[1]);
							show_lunar(str_time.year,str_time.month,str_time.date);
							delay_ms(100);
							show_lunar_festival();		
							delay_ms(100);
							show_festival();	
							delay_ms(100);
							show_jieqi(str_time);
							break;
					}
				}
			}
			if(SUB==0)
			{
				delay_ms(40);
				if(SUB==0)
				{
					if(screen_num==0)
					{
						screen_num=3;
					}
					screen_num-=1;
					clean_all();
					switch(screen_num)
					{
						case 0:
							clean_all();
							hb_show_time(str_time);
							read_temper(pbuff);		//读取当前温度值
							temper_convert();	// 启动温度转换，需要750ms
							hb_show_temp(pbuff[0], pbuff[1]);		//显示当前温度
							break;
						case 1:
							clean_all();
							hb_show_big_time_only(str_time);
							break;
						case 2:
							clean_all();
							hb_show_big_date_only(str_time);
							//read_temper(pbuff);	//读取当前温度值
							//temper_convert();	//启动温度转换，需要750ms
							//hb_show_big_temp(pbuff[0], pbuff[1]);
							show_lunar(str_time.year,str_time.month,str_time.date);
							delay_ms(100);
							show_lunar_festival();		
							delay_ms(100);
							show_festival();	
							delay_ms(100);
							show_jieqi(str_time);
							break;
					}
				}
			}
		write_IIC(SCREEN_NUM_ADDR, screen_num);
}
