#include<reg51.h>
#include<stdio.h>
#include<absacc.h>
#include<intrins.h>
#include<string.H>
#include "main.h"
#ifndef _HB_H
#define _HB_H

//初始化指令
/****************字符显示命名，用于单个字符的显示*****************/
//在相应字符位处显示字符命令 命令格式：命令 行位置(0-7) 列位置(0-F) 
#define CHAR				0xF0	//显示16*16点阵国标汉字
#define ASC2_8_8		    0xF1	//显示8*8点阵标准ASC2字符
#define ASC2_8_16		    0xF9	//显示8*16点阵半角标准ASC2字符
#define NUM_3_5			    0xE1	//显示3*5点阵数字及大写字母
#define ASC2_5_7		    0xE0	//显示5*7点阵标准ASC2字符
#define NUM_6_12			0xE2	//显示6*12双倍粗细数字
#define NUM_9_16			0xE3	//显示9*16三倍粗细数字

//在任意处显示字符命令   命令格式：命令 行点位置(0-63) 列点位置(0-127) 
#define CHAR_lat			0xEA	//显示16*16点阵国标汉字
#define ASC2_8_8_lat	    0xC1	//显示8*8点阵标准ASC2字符
#define ASC2_8_16_lat	    0xC9	//显示8*16点阵半角标准ASC2字符
#define NOM_3_5_lat		    0xC4	//显示3*5点阵数字及大写字母
#define ASC2_5_7_lat	    0xC0	//显示5*7点阵标准ASC2字符
#define NOM_6_12_lat		0xC2	//显示6*12双倍粗细数字
#define NOM_9_16_lat		0xC3	//显示9*16三倍粗细数字

//在光标处处显示字符命令 命令格式：命令 内容。使用前先定义光标位置
#define CHAR_cur			0xEC	//显示16*16点阵国标汉字
#define ASC2_8_8_cur	    0xD1	//显示8*8点阵标准ASC2字符
#define ASC2_8_16_cur	    0xD9	//显示8*16点阵半角标准ASC2字符
#define NUM_3_5_cur		    0xD4	//显示3*5点阵数字及大写字母
#define ASC2_5_7_cur	    0xD0	//显示5*7点阵标准ASC2字符
#define NUM_6_12_cur		0xD2	//显示6*12双倍粗细数字
#define NUM_9_16_cur		0xD3	//显示9*16三倍粗细数字

/**************字符串显示命名，用于多个字符的显示*****************/
//命令格式：命令 行位置   列位置   内容 结束符号0x00
//命令格式：命令 行点位置 列点位置 内容 结束符号0x00
//命令格式：命令 内容 结束符号0x00。记得定义光标位置
#define STR_CHAR		0xE9	//混合显示16*16点阵和8*16点阵半角标准ASC2字符串,字符位处
#define STR_CHAR_lat	0xEB	//混合显示16*16点阵和8*16点阵半角标准ASC2字符串，光标处
#define STR_ASC2_8_8	0xDF	//显示8*8点阵标准ASC2字符串
#define STR_NUM_3_5		0xDE	//显示3*5点阵数字及大写字母字符串
#define STR_ASC2_5_7	0xDB	//显示5*7点阵ASC2字符串
#define STR_NUM_6_12	0xDC	//显示6*12点阵双倍粗细数字串
#define STR_NUM_9_16	0xDD	//显示9*16点阵三倍粗细数字串

#define STR_CHAR_cur	0xED	//混合显示16*16点阵和8*16点阵半角标准ASC2字符串，光标处
#define STR_ASC2_8_8_cur	0xDA	//显示8*8点阵标准ASC2字符串
#define STR_NUM_3_5_cur		0xD6	//显示3*5点阵数字及大写字母字符串
#define STR_ASC2_5_7_cur	0xD5	//显示5*7点阵ASC2字符串
#define STR_NUM_6_12_cur	0xD7	//显示6*12点阵双倍粗细数字串
#define STR_NUM_9_16_cur	0xD8	//显示9*16点阵三倍粗细数字串
#define STRCHAR_12_12     0xB9   //混合显示12*12点阵和8*12点阵半角标准ASC2字符串，光标处

/****************图形显示命名，用于图形的显示*****************/
//在相应字符位处显示字符命令 命令格式：命令 行位置(0-7) 列位置(0-F) 内容(2字节或1字节)
#define BIT_LAT			0xF2		//显示位点阵
#define CHAR_LAT		0xF3		//显示字节点阵
#define LINE		    0xC5		//画线
#define BMP			   	0xFD		//画图片			  

/****************屏幕控制命令，用于图形的控制*****************/
#define SCREEN_CLA			0xF4		//全屏幕清除
#define SCREEN_UP			0xF5		//全屏幕上移动
#define SCREEN_DOWN		    0xF6		//全屏幕下移动
#define SCREEN_LEFT		    0xF7		//全屏幕左移动
#define SCREEN_RIGHT		0xF8		//全屏幕右移动
#define INVERSE				0xFA		//反白
#define CUR_SET				0xFB		//光标显示设置
#define RATE_SET			0xFC		//移动速度调整
#define SCREEN_ON			0xFE		//屏幕开关及显示模式
#define CUR_LOC_SET			0xE4		//光标定位设置
#define LIGHT_SET			0xE5		//背光开关
#define CONTRAST_SET		0xE6		//对比度调节

/****************模块设置命令，用于模块的控制*****************/
#define POWER_DOWN			0xFF		//省电模式
#define RESET				0xEF		//复位命令
#define BAND_SET			0xE8		//UART波特率设置

/*********************块状图形操作命令************************/
#define BLOCK_MOVE			0xC6		//任意指定区域移位
#define BLOCK_OPRT			0xC7		//任意指定区域操作
#define BLOCK_RAY			0xC8		//任意指定区域闪烁

#define DELAY_TIME  10    //延时参数

void Screen_Con(uchar command);
void Display_Lin(uchar command,uchar x,uchar y,uchar typ1,uchar typ2,uchar len);
void Display_StrChar(uchar command,uchar x,uchar y,uchar *p);
void Delay_1ms(uint j);
uchar* Num_to_Str(float x);
void hb_init(void);
void hb_show_time(SYSTEM_TIME str_time);
void hb_show_temp(uchar int_tmp, uchar dec_tmp);
void show_set();
void hb_show_year(SYSTEM_TIME str_time);
void hb_show_month(SYSTEM_TIME str_time);
void hb_show_date(SYSTEM_TIME str_time);
void hb_show_week(SYSTEM_TIME str_time);
void hb_show_hour(SYSTEM_TIME str_time);
void hb_show_min(SYSTEM_TIME str_time);
void hb_show_sec(SYSTEM_TIME str_time);
void Display_Bmp(uchar command,uchar x,uchar y,uchar len,uchar high,uchar *p);
void start_blink_year();
void stop_blink_year();
void start_blink_month();
void stop_blink_month();
void start_blink_date();
void stop_blink_date();

void start_blink_min();
void start_blink_hour();
void stop_blink_hour();
void stop_blink_min();
void set_show_start_blink();
void set_show_stop_blink();

void stop_blink_week(uchar i);
void start_blink_week(uchar i);
void stop_blink_alarm();
void start_blink_alarm();
void show_alarm();
void show_alarm_all(uchar alarm_set);
void clean_alarm_all();
void clean_date();
void clean_alarm_day(uchar i);
void show_alarm_day(uchar i);
void clean_temp();
void show_time_only(SYSTEM_TIME str_time);
SYSTEM_TIME add_sub_hour_alarm(SYSTEM_TIME str_time,uchar i);
SYSTEM_TIME add_sub_min_alarm(SYSTEM_TIME str_time,uchar i);
uchar calculate_day_num(SYSTEM_TIME str_time);
void show_volume(uchar volume);
void clean_all();
void show_choose_music(uchar music_num);
void welcome_init();
void start_blink_big_hour();
void stop_blink_big_hour();
void start_blink_big_min();
void stop_blink_big_min();
void start_blink_big_year();
void stop_blink_big_year();
void start_blink_big_month();
void stop_blink_big_month();
void start_blink_big_date();
void stop_blink_big_date();
void start_blink_big_set();
void stop_blink_big_set();
void show_big_sec(uchar shi,uchar ge);
void hb_show_big_time_only(SYSTEM_TIME str_time);
void hb_show_big_date_only(SYSTEM_TIME str_time);
void hb_show_big_temp(uchar int_tmp, uchar dec_tmp);
void show_big_set();
void clean_big_set();
void start_blink_big_hour();
void stop_blink_big_hour();
void start_blink_big_min();
void stop_blink_big_min();
void start_blink_big_year();
void stop_blink_big_year();
void start_blink_big_month();
void stop_blink_big_month();
void start_blink_big_date();
void stop_blink_big_date();
void start_blink_big_set();
void stop_blink_big_set();
void hb_show_big_year(SYSTEM_TIME str_time);
void hb_show_big_month(SYSTEM_TIME str_time);
void hb_show_big_date(SYSTEM_TIME str_time);
void hb_show_big_week(SYSTEM_TIME str_time);
void hb_show_big_hour(SYSTEM_TIME str_time);
void hb_show_big_min(SYSTEM_TIME str_time);
void hb_show_big_sec(SYSTEM_TIME str_time);
void set_stop_blink();
void set_start_blink();
void cal_festival();
void cal_jieqi(SYSTEM_TIME str_time);
void judge_leap_month(uchar year,uchar month,uchar day);
void clean_set();
void draw_frame();
void show_lunar(uchar year,uchar month,uchar date);
void show_lunar_festival();
void show_jieqi(SYSTEM_TIME str_time);
uchar hex_to_bcd(uchar hex);
void show_festival();
uchar sun_to_lunar(bit c,uchar year1,uchar month1,uchar day1,uchar return_flag);
#endif