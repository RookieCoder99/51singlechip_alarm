#include <stdio.h>
#include "main.h"
#include "delay.h"
#include "ds1302.h"
#include "ds18B20.h"
#include "hb.h"
#include "yuyin.h"
#include "iic.h"
extern uchar alarm_set; //第几个闹钟
extern uchar alarm_pos;
extern SYSTEM_TIME str_time;
extern SYSTEM_TIME str_time_alarm_flag[2];
extern SYSTEM_TIME str_time_alarm[2];
extern uchar  alarm_show_flag[2][7];

/**************闹钟功能实现主体********************/

void alarm_write_iic(SYSTEM_TIME str_time_alarm_[2],uchar alarm_show_flag_[2][7])
{
		write_IIC(ALARM_ADDR, str_time_alarm_[0].hour);
		delay_ms(2);
		write_IIC(ALARM_ADDR+1, str_time_alarm_[0].min);
		delay_ms(2);
		write_IIC(ALARM_ADDR+2, str_time_alarm_[0].sec);
		delay_ms(2);
		write_IIC(ALARM_ADDR+3, alarm_show_flag_[0][0]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+4, alarm_show_flag_[0][1]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+5, alarm_show_flag_[0][2]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+6, alarm_show_flag_[0][3]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+7, alarm_show_flag_[0][4]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+8, alarm_show_flag_[0][5]);
		delay_ms(2);
		write_IIC(ALARM_ADDR+9, alarm_show_flag_[0][6]);
		delay_ms(2);
	
		write_IIC(ALARM_ADDR2, str_time_alarm_[1].hour);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+1, str_time_alarm_[1].min);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+2, str_time_alarm_[1].sec);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+3, alarm_show_flag_[1][0]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+4, alarm_show_flag_[1][1]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+5, alarm_show_flag_[1][2]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+6, alarm_show_flag_[1][3]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+7, alarm_show_flag_[1][4]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+8, alarm_show_flag_[1][5]);
		delay_ms(2);
		write_IIC(ALARM_ADDR2+9, alarm_show_flag_[1][6]);
		delay_ms(2);
		
}
void alarm_read_iic()
{
		str_time_alarm[0].hour = read_IIC(ALARM_ADDR);
		delay_ms(2);
		str_time_alarm[0].min = read_IIC(ALARM_ADDR+1);
		delay_ms(2);
		alarm_show_flag[0][0]=read_IIC(ALARM_ADDR+3);
		delay_ms(2);
		alarm_show_flag[0][1]=read_IIC(ALARM_ADDR+4);
		delay_ms(2);
		alarm_show_flag[0][2]=read_IIC(ALARM_ADDR+5);
		delay_ms(2);
		alarm_show_flag[0][3]=read_IIC(ALARM_ADDR+6);
		delay_ms(2);
		alarm_show_flag[0][4]=read_IIC(ALARM_ADDR+7);
		delay_ms(2);
		alarm_show_flag[0][5]=read_IIC(ALARM_ADDR+8);
		delay_ms(2);
		alarm_show_flag[0][6]=read_IIC(ALARM_ADDR+9);
		delay_ms(2);
	
		str_time_alarm[1].hour = read_IIC(ALARM_ADDR2);
		delay_ms(2);
		str_time_alarm[1].min = read_IIC(ALARM_ADDR2+1);
		delay_ms(2);
		alarm_show_flag[1][0]=read_IIC(ALARM_ADDR2+3);
		delay_ms(2);
		alarm_show_flag[1][1]=read_IIC(ALARM_ADDR2+4);
		delay_ms(2);
		alarm_show_flag[1][2]=read_IIC(ALARM_ADDR2+5);
		delay_ms(2);
		alarm_show_flag[1][3]=read_IIC(ALARM_ADDR2+6);
		delay_ms(2);
		alarm_show_flag[1][4]=read_IIC(ALARM_ADDR2+7);
		delay_ms(2);
		alarm_show_flag[1][5]=read_IIC(ALARM_ADDR2+8);
		delay_ms(2);
		alarm_show_flag[1][6]=read_IIC(ALARM_ADDR2+9);
		delay_ms(2);
}

void alarm_function()
{	
	while(!ALARM)
	{
		delay_ms(100);
		if(ALARM!=0) //加这个因为那个按钮总是会误触导致抖动
		{
			break;
		}
		while(1)
		{
			if(ALARM==0)
			{
				delay_ms(10);
				if(ALARM==0)
				{
					alarm_set+=1;
					if(alarm_set==3)
					{
						alarm_set=0;
						stop_blink_week(alarm_pos-1);
						alarm_pos=0;
						stop_blink_alarm();
						clean_all();
						break;
					}
					switch(alarm_set)
					{
						case 1:
							clean_all();
							show_alarm_all(alarm_set-1);
							show_time_only(str_time_alarm[0]);
							show_alarm();
							start_blink_alarm();
							break;
						case 2:
							stop_blink_week(alarm_pos-1);
							clean_all();
							show_alarm_all(alarm_set-1);
							alarm_pos=0;//从SUN开始选择
							show_time_only(str_time_alarm[1]);
							show_alarm();
							start_blink_alarm();
							break;
					}		
				}
			}
			//设置闹钟时间等
			if(SWITCH==0)
			{
				delay_ms(90);
				if(SWITCH==0)
				{
					alarm_pos+=1;
					show_alarm();
					if(alarm_pos==10)
					{
						alarm_pos=0;
						stop_blink_min();
						start_blink_alarm();
					}
					switch(alarm_pos)
					{
						case 1:
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][0]==0x00)
							{
								show_alarm_day(0);
							}
							start_blink_week(0);
							break;
						case 2:
							stop_blink_week(0);
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][1]==0x00)
							{
								show_alarm_day(1);
							}
							start_blink_week(1);
							break;
						case 3:
							stop_blink_week(1);
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][2]==0x00)
							{
								show_alarm_day(2);
							}
							start_blink_week(2);
							break;
						case 4:
							stop_blink_week(2);
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][3]==0x00)
							{
								show_alarm_day(3);
							}
							start_blink_week(3);
							break;
						case 5:
							stop_blink_week(3);
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][4]==0x00)
							{
								show_alarm_day(4);
							}
							start_blink_week(4);
							break;
						case 6:
							stop_blink_week(4);
							show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][5]==0x00)
							{
								show_alarm_day(5);
							}
							start_blink_week(5);
							break;
						case 7:
							stop_blink_week(5);
								show_alarm_all(alarm_set-1);
							if(alarm_show_flag[alarm_set-1][6]==0x00)
							{
								show_alarm_day(6);
							}
							start_blink_week(6);
							break;
						case 8:
							stop_blink_week(6);
							show_alarm_all(alarm_set-1);
							start_blink_hour();
							break;
						case 9:
							stop_blink_hour();
							start_blink_min();
							break;
					}
				}
			}
			//加和减调节时间和星期几
			if(ADD==0)
			{
				delay_ms(100);
				if(ADD==0)
				{
					switch(alarm_pos)
					{
						case 1:
							stop_blink_week(0);
							alarm_show_flag[alarm_set-1][0]=alarm_show_flag[alarm_set-1][0]^0xff;
							if(alarm_show_flag[alarm_set-1][0]==0xff)
							{
								show_alarm_day(0);
							}
							else
							{
								clean_alarm_day(0);
							}
							break;
						case 2:
							stop_blink_week(1);
							alarm_show_flag[alarm_set-1][1]=alarm_show_flag[alarm_set-1][1]^0xff;
							if(alarm_show_flag[alarm_set-1][1]==0xff)
							{
								show_alarm_day(1);
							}
							else
							{
								clean_alarm_day(1);
							}
							break;
						case 3:
							stop_blink_week(2);
							alarm_show_flag[alarm_set-1][2]=alarm_show_flag[alarm_set-1][2]^0xff;
							if(alarm_show_flag[alarm_set-1][2]==0xff)
							{
								show_alarm_day(2);
							}
							else
							{
								clean_alarm_day(2);
							}
							break;
						case 4:
							stop_blink_week(3);
						alarm_show_flag[alarm_set-1][3]=alarm_show_flag[alarm_set-1][3]^0xff;
							if(alarm_show_flag[alarm_set-1][3]==0xff)
							{
								show_alarm_day(3);
							}
							else
							{
								clean_alarm_day(3);
							}
							break;
						case 5:
							stop_blink_week(4);
						alarm_show_flag[alarm_set-1][4]=alarm_show_flag[alarm_set-1][4]^0xff;
							if(alarm_show_flag[alarm_set-1][4]==0xff)
							{
								show_alarm_day(4);
							}
							else
							{
								clean_alarm_day(4);
							}
							break;
						case 6:
							stop_blink_week(5);
							alarm_show_flag[alarm_set-1][5]=alarm_show_flag[alarm_set-1][5]^0xff;
							if(alarm_show_flag[alarm_set-1][5]==0xff)
							{
								show_alarm_day(5);
							}
							else
							{
								clean_alarm_day(5);
							}
							break;
						case 7:
							stop_blink_week(6);
							alarm_show_flag[alarm_set-1][6]=alarm_show_flag[alarm_set-1][6]^0xff;
							if(alarm_show_flag[alarm_set-1][6]==0xff)
							{
								show_alarm_day(6);
							}
							else
							{
								clean_alarm_day(6);
							}
							break;
						case 8:
							stop_blink_hour();
							str_time_alarm[alarm_set-1]=add_sub_hour_alarm(str_time_alarm[alarm_set-1],1);
							hb_show_hour(str_time_alarm[alarm_set-1]);
							start_blink_hour();
							break;
						case 9:
							stop_blink_min();
							str_time_alarm[alarm_set-1]=add_sub_min_alarm(str_time_alarm[alarm_set-1],1);
							hb_show_min(str_time_alarm[alarm_set-1]);
							start_blink_min();
							break;
					}
					alarm_write_iic(str_time_alarm,alarm_show_flag);
					str_time_alarm_flag[alarm_set-1]=str_time_alarm[alarm_set-1];
				}
			}
			if(SUB==0)
			{
				delay_ms(80);
				if(SUB==0)
				{
					switch(alarm_pos)
					{
						case 1:
							stop_blink_week(0);
							alarm_show_flag[alarm_set-1][0]=alarm_show_flag[alarm_set-1][0]^0xff;
							if(alarm_show_flag[alarm_set-1][0]==0xff)
							{
								show_alarm_day(0);
							}
							else
							{
								clean_alarm_day(0);
							}
							break;
						case 2:
							stop_blink_week(1);
							alarm_show_flag[alarm_set-1][1]=alarm_show_flag[alarm_set-1][1]^0xff;
							if(alarm_show_flag[alarm_set-1][1]==0xff)
							{
								show_alarm_day(1);
							}
							else
							{
								clean_alarm_day(1);
							}
							break;
						case 3:
							stop_blink_week(2);
							alarm_show_flag[alarm_set-1][2]=alarm_show_flag[alarm_set-1][2]^0xff;
							if(alarm_show_flag[alarm_set-1][2]==0xff)
							{
								show_alarm_day(2);
							}
							else
							{
								clean_alarm_day(2);
							}
							break;
						case 4:
							stop_blink_week(3);
						alarm_show_flag[alarm_set-1][3]=alarm_show_flag[alarm_set-1][3]^0xff;
							if(alarm_show_flag[alarm_set-1][3]==0xff)
							{
								show_alarm_day(3);
							}
							else
							{
								clean_alarm_day(3);
							}
							break;
						case 5:
							stop_blink_week(4);
						alarm_show_flag[alarm_set-1][4]=alarm_show_flag[alarm_set-1][4]^0xff;
							if(alarm_show_flag[alarm_set-1][4]==0xff)
							{
								show_alarm_day(4);
							}
							else
							{
								clean_alarm_day(4);
							}
							break;
						case 6:
							stop_blink_week(5);
							alarm_show_flag[alarm_set-1][5]=alarm_show_flag[alarm_set-1][5]^0xff;
							if(alarm_show_flag[alarm_set-1][5]==0xff)
							{
								show_alarm_day(5);
							}
							else
							{
								clean_alarm_day(5);
							}
							break;
						case 7:
							stop_blink_week(6);
							alarm_show_flag[alarm_set-1][6]=alarm_show_flag[alarm_set-1][6]^0xff;
							if(alarm_show_flag[alarm_set-1][6]==0xff)
							{
								show_alarm_day(6);
							}
							else
							{
								clean_alarm_day(6);
							}
							break;
						case 8:
							stop_blink_hour();
							str_time_alarm[alarm_set-1]=add_sub_hour_alarm(str_time_alarm[alarm_set-1],0);
							hb_show_hour(str_time_alarm[alarm_set-1]);
							start_blink_hour();
							break;
						case 9:
							stop_blink_min();
							str_time_alarm[alarm_set-1]=add_sub_min_alarm(str_time_alarm[alarm_set-1],0);
							hb_show_min(str_time_alarm[alarm_set-1]);
							start_blink_min();
							break;
					}
					alarm_write_iic(str_time_alarm,alarm_show_flag);
					str_time_alarm_flag[alarm_set-1]=str_time_alarm[alarm_set-1];
				}
			}
		}
		break;	
	}
}