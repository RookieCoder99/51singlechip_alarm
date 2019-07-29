/*******************************************************
* 文件名：iic.h
********************************************************/

#ifndef _IIC_H
#define _IIC_H
#include "main.h"
#define DAT_ADDR	0x20	//IIC EEPROM中保存的温度上、下限的地址
#define ALARM_ADDR	0x02
#define ALARM_ADDR2	0x12
#define VOLUME_ADDR	0x22
#define MUSIC_IIC_ADDR 0x23
#define SCREEN_NUM_ADDR 0x24
#define MORNING_ADDR 0x25
#define NOW_TIME_ADDR 0x26
#define START_MUSIC_ADDR 0x27
#define WIFI_LIGHT_ADDR 0x28
#define SLEEP0_ADDR 0x29
#define SLEEP1_ADDR 0x31
#define SLEEP_FLAG_ADDR 0x33
#define MORNING_FLAG_ADDR 0x34
#define NOW_TIME_FLAG_ADDR 0x35
#define WIFI_LIGHT_FLAG_ADDR 0x36
void init_IIC(void);
void write_IIC(uchar add,uchar dat);
uchar read_IIC(uchar add);
#endif