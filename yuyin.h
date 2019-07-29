#include<reg51.h>
#include<stdio.h>
#include<absacc.h>
#include<intrins.h>
#include "main.h"
#ifndef _YUYIN_H
#define _YUYIN_H
void check_alarm_time(SYSTEM_TIME str_time,
												SYSTEM_TIME str_time_alarm[2],
												uchar alarm_show_flag[2][7]);
void check_now_time(SYSTEM_TIME str_time);
void choose_music(void);
void broadcast_temp(uchar pbuff[2]);
void SendData (uchar MUSIC_ADDR);
void broadcast_morning(SYSTEM_TIME str_time);
void welcome_music(uchar volume_start);
void volume_control();
void music_screen();
#endif