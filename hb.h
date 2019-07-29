#include<reg51.h>
#include<stdio.h>
#include<absacc.h>
#include<intrins.h>
#include<string.H>
#include "main.h"
#ifndef _HB_H
#define _HB_H

//��ʼ��ָ��
/****************�ַ���ʾ���������ڵ����ַ�����ʾ*****************/
//����Ӧ�ַ�λ����ʾ�ַ����� �����ʽ������ ��λ��(0-7) ��λ��(0-F) 
#define CHAR				0xF0	//��ʾ16*16������꺺��
#define ASC2_8_8		    0xF1	//��ʾ8*8�����׼ASC2�ַ�
#define ASC2_8_16		    0xF9	//��ʾ8*16�����Ǳ�׼ASC2�ַ�
#define NUM_3_5			    0xE1	//��ʾ3*5�������ּ���д��ĸ
#define ASC2_5_7		    0xE0	//��ʾ5*7�����׼ASC2�ַ�
#define NUM_6_12			0xE2	//��ʾ6*12˫����ϸ����
#define NUM_9_16			0xE3	//��ʾ9*16������ϸ����

//�����⴦��ʾ�ַ�����   �����ʽ������ �е�λ��(0-63) �е�λ��(0-127) 
#define CHAR_lat			0xEA	//��ʾ16*16������꺺��
#define ASC2_8_8_lat	    0xC1	//��ʾ8*8�����׼ASC2�ַ�
#define ASC2_8_16_lat	    0xC9	//��ʾ8*16�����Ǳ�׼ASC2�ַ�
#define NOM_3_5_lat		    0xC4	//��ʾ3*5�������ּ���д��ĸ
#define ASC2_5_7_lat	    0xC0	//��ʾ5*7�����׼ASC2�ַ�
#define NOM_6_12_lat		0xC2	//��ʾ6*12˫����ϸ����
#define NOM_9_16_lat		0xC3	//��ʾ9*16������ϸ����

//�ڹ�괦����ʾ�ַ����� �����ʽ������ ���ݡ�ʹ��ǰ�ȶ�����λ��
#define CHAR_cur			0xEC	//��ʾ16*16������꺺��
#define ASC2_8_8_cur	    0xD1	//��ʾ8*8�����׼ASC2�ַ�
#define ASC2_8_16_cur	    0xD9	//��ʾ8*16�����Ǳ�׼ASC2�ַ�
#define NUM_3_5_cur		    0xD4	//��ʾ3*5�������ּ���д��ĸ
#define ASC2_5_7_cur	    0xD0	//��ʾ5*7�����׼ASC2�ַ�
#define NUM_6_12_cur		0xD2	//��ʾ6*12˫����ϸ����
#define NUM_9_16_cur		0xD3	//��ʾ9*16������ϸ����

/**************�ַ�����ʾ���������ڶ���ַ�����ʾ*****************/
//�����ʽ������ ��λ��   ��λ��   ���� ��������0x00
//�����ʽ������ �е�λ�� �е�λ�� ���� ��������0x00
//�����ʽ������ ���� ��������0x00���ǵö�����λ��
#define STR_CHAR		0xE9	//�����ʾ16*16�����8*16�����Ǳ�׼ASC2�ַ���,�ַ�λ��
#define STR_CHAR_lat	0xEB	//�����ʾ16*16�����8*16�����Ǳ�׼ASC2�ַ�������괦
#define STR_ASC2_8_8	0xDF	//��ʾ8*8�����׼ASC2�ַ���
#define STR_NUM_3_5		0xDE	//��ʾ3*5�������ּ���д��ĸ�ַ���
#define STR_ASC2_5_7	0xDB	//��ʾ5*7����ASC2�ַ���
#define STR_NUM_6_12	0xDC	//��ʾ6*12����˫����ϸ���ִ�
#define STR_NUM_9_16	0xDD	//��ʾ9*16����������ϸ���ִ�

#define STR_CHAR_cur	0xED	//�����ʾ16*16�����8*16�����Ǳ�׼ASC2�ַ�������괦
#define STR_ASC2_8_8_cur	0xDA	//��ʾ8*8�����׼ASC2�ַ���
#define STR_NUM_3_5_cur		0xD6	//��ʾ3*5�������ּ���д��ĸ�ַ���
#define STR_ASC2_5_7_cur	0xD5	//��ʾ5*7����ASC2�ַ���
#define STR_NUM_6_12_cur	0xD7	//��ʾ6*12����˫����ϸ���ִ�
#define STR_NUM_9_16_cur	0xD8	//��ʾ9*16����������ϸ���ִ�
#define STRCHAR_12_12     0xB9   //�����ʾ12*12�����8*12�����Ǳ�׼ASC2�ַ�������괦

/****************ͼ����ʾ����������ͼ�ε���ʾ*****************/
//����Ӧ�ַ�λ����ʾ�ַ����� �����ʽ������ ��λ��(0-7) ��λ��(0-F) ����(2�ֽڻ�1�ֽ�)
#define BIT_LAT			0xF2		//��ʾλ����
#define CHAR_LAT		0xF3		//��ʾ�ֽڵ���
#define LINE		    0xC5		//����
#define BMP			   	0xFD		//��ͼƬ			  

/****************��Ļ�����������ͼ�εĿ���*****************/
#define SCREEN_CLA			0xF4		//ȫ��Ļ���
#define SCREEN_UP			0xF5		//ȫ��Ļ���ƶ�
#define SCREEN_DOWN		    0xF6		//ȫ��Ļ���ƶ�
#define SCREEN_LEFT		    0xF7		//ȫ��Ļ���ƶ�
#define SCREEN_RIGHT		0xF8		//ȫ��Ļ���ƶ�
#define INVERSE				0xFA		//����
#define CUR_SET				0xFB		//�����ʾ����
#define RATE_SET			0xFC		//�ƶ��ٶȵ���
#define SCREEN_ON			0xFE		//��Ļ���ؼ���ʾģʽ
#define CUR_LOC_SET			0xE4		//��궨λ����
#define LIGHT_SET			0xE5		//���⿪��
#define CONTRAST_SET		0xE6		//�Աȶȵ���

/****************ģ�������������ģ��Ŀ���*****************/
#define POWER_DOWN			0xFF		//ʡ��ģʽ
#define RESET				0xEF		//��λ����
#define BAND_SET			0xE8		//UART����������

/*********************��״ͼ�β�������************************/
#define BLOCK_MOVE			0xC6		//����ָ��������λ
#define BLOCK_OPRT			0xC7		//����ָ���������
#define BLOCK_RAY			0xC8		//����ָ��������˸

#define DELAY_TIME  10    //��ʱ����

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