/*******************************************************
* 文件名：main.h
********************************************************/
#include <reg51.h>
#include <intrins.h>

#ifndef _MAIN_H
#define _MAIN_H

#define uint	unsigned int
#define uchar	unsigned char
	

//自定义时间结构体
typedef struct   __system_time{
	uchar year;
	uchar month;
	uchar date;
	uchar hour;
	uchar min;
	uchar sec;
	//uchar day;	//星期
} SYSTEM_TIME;
/***************定义切换按键*************/
sbit SWITCH=P1^5;
sbit ADD=P1^6;
sbit SUB=P1^7;
sbit ALARM=P3^6;
sbit TEMP_CONVER=P1^4;
sbit HUMAN_SENSOR=P2^4;//人体感应
sbit CHOOSE_MUSIC=P2^2;//选音乐按钮
sbit VOLUME=P2^3;
sbit BUSY=P3^5;//液晶屏引脚定义
sbit REQ =P3^4;
sbit YYSDA =P2^0;//语音引脚定义
sbit YYBUSY=P2^1;
sbit SHAKE=P2^6;
sbit LIGHT_SENSOR=P2^5;

void get_temper(uchar *pBuff);
void get_temper_updown(uchar *pDown, uchar *pUp);
void get_system_time(uchar *pBuff);
void set_temper_updown(uchar down, uchar up);
void set_system_time(uchar *pBuff);
uchar normal_temper(uchar int_temper, uchar down_temper, uchar up_temper);

#endif