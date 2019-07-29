/*****************************************************************************
* 文件名：ds1302.c
* 说  明：操作DS1302的基本函数
*****************************************************************************/
#include "main.h"
#include "ds1302.h"
#include "hb.h"
#include "delay.h"
#define BCD2DEC(X)	(((X&0x70)>>4)*10 + (X&0x0F))	//用于将BCD码转成十进制
#define DEC2BCD(X)	((X/10)<<4 | (X%10))			//用于将十进制转成BCD码
//DS1302的引脚定义
sbit DS_RST = P1^0;
sbit DS_SCLK = P1^1;
sbit DS_IO = P1^2;

extern SYSTEM_TIME str_time;
extern uchar cur_pos;
extern uchar screen_num;
/******************************************************
* 函  数：初始化DS1302的引脚
* 参  数：空
******************************************************/
void ds1302_init(void)
{
	DS_RST = 0;
	DS_SCLK = 0;
	DS_IO = 0;
}

/******************************************************
* 函  数：写单字节到DS1302
* 参  数：dat要写的数据
******************************************************/
void write_byte(uchar dat)
{
	uchar i = 0;
	
	for(i=0; i<8; i++)
	{
		DS_SCLK = 0;
		DS_IO = (dat&1);
		_nop_(),_nop_();
		DS_SCLK = 1;		//上升沿发出数据
		_nop_(),_nop_();
		dat >>= 1;
	}
}

/******************************************************
* 函  数：从DS1302读单字节
* 参  数：空
* 返回值：返回读取的字节（10进制）
******************************************************/
uchar read_byte(void)
{
	uchar i = 0;
	uchar dat = 0;
	uchar tmp = 0;	
//	i = (i++)%10;

	for(i=0; i<8; i++)
	{ 		
		DS_SCLK = 1;   	
		_nop_(),_nop_();			
		DS_SCLK = 0;		//下降沿读出数据
		_nop_(),_nop_();
		tmp = DS_IO;
		dat >>= 1;			//先读的是低位，移位为低位
		dat |= (tmp<<7);
	}

	dat = BCD2DEC(dat);			//BCD转换
	return(dat);
}

/******************************************************
* 函  数：从DS1302的指定位置读数据
* 参  数：addr要读取数据的控制字（地址/命令）
* 返回值：返回读取的数字（10进制）
******************************************************/
uchar read_ds1302(uchar addr)
{
	uchar tmp;

	DS_RST = 0;
	DS_SCLK = 0;
	DS_RST = 1;
	write_byte(addr);
	tmp = read_byte();
	DS_RST = 0;
	DS_SCLK = 0;
	DS_IO = 0;

	return(tmp);
}

/******************************************************
* 函  数：写数据到DS1302的指定位置
* 参  数：addr要写入数据的控制字（地址/命令）
*         dat要写的数据
******************************************************/
void write_ds1302(uchar addr, uchar dat)
{
	uchar tmp = 0;
	tmp = DEC2BCD(dat);

	DS_RST = 0;
	DS_SCLK = 0;
	DS_RST = 1;
	write_byte(addr);
	write_byte(tmp);
	DS_RST = 0;
	DS_SCLK = 0;
}

/******************************************************
* 函  数：读取年、月、日、时、分、秒、星期
* 参  数：无
* 返回值：str_time读取到的时间
******************************************************/
SYSTEM_TIME read_time(void)
{
	SYSTEM_TIME str_time;
	str_time.year = read_ds1302(DS1302_YEAR|0x01);//或0x01表示地址+1读指令
	str_time.month = read_ds1302(DS1302_MONTH|0x01);
	str_time.date = read_ds1302(DS1302_DATE|0x01);
	str_time.hour = read_ds1302(DS1302_HOUR|0x01);
	str_time.min = read_ds1302(DS1302_MIN|0x01);
	str_time.sec = read_ds1302(DS1302_SEC|0x01);
	return(str_time);
}

/******************************************************
* 函  数：设置DS1302的年、月、日、时、分、秒、星期
* 参  数：str_time要设定的时间
******************************************************/
void set_time(SYSTEM_TIME str_time)
{	
	write_ds1302(0x8E, 0x00);//0x8e写保护位，为0是允许写
	write_ds1302(DS1302_YEAR, str_time.year);  	
	write_ds1302(DS1302_MONTH, str_time.month);	
	write_ds1302(DS1302_DATE, str_time.date);
	write_ds1302(DS1302_HOUR, str_time.hour);
	write_ds1302(DS1302_MIN, str_time.min);	  
	write_ds1302(DS1302_SEC, str_time.sec);	  
	write_ds1302(0x8E, 0x80);  	
}


void add_sub_year(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		if(str_time.year==99)
			str_time.year=0;
		else
			str_time.year=str_time.year+1;
	}
	else
	{
		if(str_time.year==0)
			str_time.year=99;
		else
			str_time.year=str_time.year-1;
	}
		str_time.sec=0;
		set_time(str_time);
}
void add_sub_month(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		if(str_time.month==12)
			str_time.month=0;
		str_time.month+=1;
	}
	else
	{
		if(str_time.month==1)
			str_time.month=13;
		str_time.month-=1;
	}
	str_time.sec=0;
	set_time(str_time);
}
void add_sub_date(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		switch(str_time.month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				if(str_time.date==31)
				{
					str_time.date=0;
				}
				str_time.date+=1;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if(str_time.date==30)
				{
					str_time.date=0;
				}
				str_time.date+=1;
				break;
			case 2:
				if(str_time.year%4==0)
				{
					if(str_time.date==29)
					{
						str_time.date=0;
					}
					str_time.date+=1;
					break;
				}
				else
				{
					if(str_time.date==28)
					{
						str_time.date=0;
					}
					str_time.date+=1;
					break;
				}
		}
	}
	else
	{
			switch(str_time.month)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					if(str_time.date==1)
					{
						str_time.date=32;
					}
					str_time.date-=1;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					if(str_time.date==1)
					{
						str_time.date=31;
					}
					str_time.date-=1;
					break;
				case 2:
					if(str_time.year%4==0)
					{
						if(str_time.date==1)
						{
							str_time.date=30;
						}
						str_time.date-=1;
						break;
					}
					else
					{
						if(str_time.date==1)
						{
							str_time.date=29;
						}
						str_time.date-=1;
						break;
					}
			}
	}
	str_time.sec=0;
	set_time(str_time);
}

void add_sub_hour(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		str_time.hour+=1;
		if(str_time.hour==24)
		{
			str_time.hour=0;
		}

	}
	else
	{
		if(str_time.hour==0)
		{
			str_time.hour=24;
		}
		str_time.hour-=1;
	}
	str_time.sec=0;
	set_time(str_time);
}
void add_sub_min(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		str_time.min+=1;
		if(str_time.min==60)
		{
			str_time.min=0;
		}
	}
	else
	{
		if(str_time.min==0)
		{
			str_time.min=60;
		}
		str_time.min-=1;
	}
	str_time.sec=0;
	set_time(str_time);
}







SYSTEM_TIME add_sub_hour_alarm(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		str_time.hour+=1;
		if(str_time.hour==24)
		{
			str_time.hour=0;
		}

	}
	else
	{
		if(str_time.hour==0)
		{
			str_time.hour=24;
		}
		str_time.hour-=1;
	}
	str_time.sec=0;
	return str_time;
}
SYSTEM_TIME add_sub_min_alarm(SYSTEM_TIME str_time,uchar i)
{
	if(i==1)
	{
		str_time.min+=1;
		if(str_time.min==60)
		{
			str_time.min=0;
		}
	}
	else
	{
		if(str_time.min==0)
		{
			str_time.min=60;
		}
		str_time.min-=1;
	}
	str_time.sec=0;
	return str_time;
}

void adjust_time()
{
	uchar set_blink_flag=0;
	while(!SWITCH)
	{
			delay_ms(20);
			if(SWITCH!=0) //加这个因为那个按钮总是会误触导致抖动
			{
				break;
			}
			while(1)
			{
				switch(screen_num)
				{
					case 0:
						if(cur_pos==0 && set_blink_flag==0)
						{
							show_set();
							set_start_blink();
							set_blink_flag=1;
						}
						if(ALARM==0)
						{
							delay_ms(40);
							if(ALARM==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_year();
										break;
									case 2:
										stop_blink_month();
										break;
									case 3:
										stop_blink_date();
										break;
									case 4:
										stop_blink_hour();
										break;
									case 5:
										stop_blink_min();
										break;
								}
								cur_pos=0;
								set_blink_flag=0;
								set_stop_blink();
								clean_set();
								break;
							}
						}
						if(SWITCH==0)
						{
							delay_ms(90);
							if(SWITCH==0)
							{
								cur_pos+=1;
								if(cur_pos==6)
								{
									cur_pos=0;
									set_blink_flag=0;
									stop_blink_min();
									set_stop_blink();
									clean_set();
									break;
								}
								switch (cur_pos)
								{
									case 1:
										set_stop_blink();
										start_blink_year();
										break;
									case 2:
										stop_blink_year();
										start_blink_month();
										break;
									case 3:
										stop_blink_month();
										start_blink_date();
										break;
									case 4:
										stop_blink_date();
										start_blink_hour();
										break;
									case 5:
										stop_blink_hour();
										start_blink_min();
										break;
								}
							}
						}
						if(ADD==0)
						{
							delay_ms(90);
							if(ADD==0)
							{
								switch (cur_pos)
								{
									case 1:
										stop_blink_year();
										add_sub_year(str_time,1);
										str_time = read_time();	
										hb_show_year(str_time);
										start_blink_year();
										break;
									case 2:
										stop_blink_month();
										add_sub_month(str_time,1);
										str_time = read_time();	
										str_time.date=1;//调月份的时候，把日设为1
										set_time(str_time);
										hb_show_month(str_time);
										hb_show_date(str_time);
										start_blink_month();
										break;
									case 3:
										stop_blink_date();
										add_sub_date(str_time,1);
										str_time = read_time();	
										hb_show_date(str_time);
										start_blink_date();
										break;
									case 4:
										stop_blink_hour();
										add_sub_hour(str_time,1);
										str_time = read_time();	
										hb_show_hour(str_time);
										start_blink_hour();
										break;
									case 5:
										stop_blink_min();
										add_sub_min(str_time,1);
										str_time = read_time();
										hb_show_min(str_time);
										start_blink_min();
										break;
								}
							}
						}
						if(SUB==0)
						{
							delay_ms(100);
							if(SUB==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_year();
										add_sub_year(str_time,0);
										str_time = read_time();	
										hb_show_year(str_time);
										start_blink_year();
										break;
									case 2:
										stop_blink_month();
										add_sub_month(str_time,0);
										str_time = read_time();	
										str_time.date=1;//调月份的时候，把日设为1
										set_time(str_time);
										hb_show_month(str_time);
										hb_show_date(str_time);
										start_blink_month();
										break;
									case 3:
										stop_blink_date();
										add_sub_date(str_time,0);
										str_time = read_time();	
										hb_show_date(str_time);
										start_blink_date();
										break;
									case 4:
										stop_blink_hour();
										add_sub_hour(str_time,0);
										str_time = read_time();	
										hb_show_hour(str_time);
										start_blink_hour();
										break;
									case 5:
										stop_blink_min();
										add_sub_min(str_time,0);
										str_time = read_time();	
										hb_show_min(str_time);
										start_blink_min();
										break;
								}
							}
						}
						break;
					case 1:
						if(ALARM==0)
						{
							delay_ms(90);
							if(ALARM==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_hour();
										break;
									case 2:
										stop_blink_min();
										break;
								}
								cur_pos=0;
								set_blink_flag=0;
								//stop_blink_big_set();
								//clean_big_set();
								break;
							}
						}
						if(SWITCH==0)
						{
							delay_ms(90);
							if(SWITCH==0)
							{
								cur_pos+=1;
								if(cur_pos==3)
								{
									cur_pos=0;
									set_blink_flag=0;
									stop_blink_big_min();
									//stop_blink_big_set();
									//clean_big_set();
									break;
								}
								switch (cur_pos)
								{
									case 1:
										//stop_blink_big_set();
										start_blink_big_hour();
										break;
									case 2:
										stop_blink_big_hour();
										start_blink_big_min();
										break;
								}
							}
						}
						if(ADD==0)
						{
							delay_ms(90);
							if(ADD==0)
							{
								switch (cur_pos)
								{
									case 1:
										stop_blink_big_hour();
										add_sub_hour(str_time,1);
										str_time = read_time();	
										hb_show_big_hour(str_time);
										//show_big_set();
										start_blink_big_hour();
										break;
									case 2:
										stop_blink_big_min();
										add_sub_min(str_time,1);
										str_time = read_time();
										hb_show_big_min(str_time);
										start_blink_big_min();
										break;
								}
							}
						}
						if(SUB==0)
						{
							delay_ms(100);
							if(SUB==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_big_hour();
										add_sub_hour(str_time,0);
										str_time = read_time();	
										hb_show_big_hour(str_time);
										//show_big_set();
										start_blink_big_hour();
										break;
									case 2:
										stop_blink_big_min();
										add_sub_min(str_time,0);
										str_time = read_time();	
										hb_show_big_min(str_time);
										start_blink_big_min();
										break;
								}
							}
						}
						break;
					case 2:
						if(ALARM==0)
						{
							delay_ms(40);
							if(ALARM==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_big_year();
										break;
									case 2:
										stop_blink_big_month();
										break;
									case 3:
										stop_blink_big_date();
										break;
								}
								cur_pos=0;
								set_blink_flag=0;
								//stop_blink_big_set();
								//clean_big_set();
								break;
							}
						}
						if(SWITCH==0)
						{
							delay_ms(90);
							if(SWITCH==0)
							{
								cur_pos+=1;
								//clean_lunar_year();
								if(cur_pos==4)
								{
									cur_pos=0;
									//set_blink_flag=0;
									stop_blink_big_date();
									//stop_blink_big_set();
									//clean_big_set();
									break;
								}
								switch (cur_pos)
								{
									case 1:
										//stop_blink_big_set();
										start_blink_big_year();
										break;
									case 2:
										stop_blink_big_year();
										start_blink_big_month();
										break;
									case 3:
										stop_blink_big_month();
										start_blink_big_date();
										break;
								}
							}
						}
						if(ADD==0)
						{
							delay_ms(100);
							if(ADD==0)
							{
								switch (cur_pos)
								{
									case 1:
										stop_blink_big_year();
										add_sub_year(str_time,1);
										str_time = read_time();	
										hb_show_big_year(str_time);
										start_blink_big_year();
										break;
									case 2:
										stop_blink_big_month();
										add_sub_month(str_time,1);
										str_time = read_time();	
										str_time.date=1;//调月份的时候，把日设为1
										set_time(str_time);
										hb_show_big_month(str_time);
										hb_show_big_date(str_time);
										start_blink_big_month();
										break;
									case 3:
										stop_blink_big_date();
										add_sub_date(str_time,1);
										str_time = read_time();	
										hb_show_big_date(str_time);
										start_blink_big_date();
										break;
								}
							}
						}
						if(SUB==0)
						{
							delay_ms(100);
							if(SUB==0)
							{
								switch(cur_pos)
								{
									case 1:
										stop_blink_big_year();
										add_sub_year(str_time,0);
										str_time = read_time();	
										hb_show_big_year(str_time);
										start_blink_big_year();
										break;
									case 2:
										stop_blink_big_month();
										add_sub_month(str_time,0);
										str_time = read_time();	
										str_time.date=1;//调月份的时候，把日设为1
										set_time(str_time);
										hb_show_big_month(str_time);
										hb_show_big_date(str_time);
										start_blink_big_month();
										break;
									case 3:
										stop_blink_big_date();
										add_sub_date(str_time,0);
										str_time = read_time();	
										hb_show_big_date(str_time);
										start_blink_big_date();
										break;
								}
							}
						}
						
						
						
						break;
				}
				if(cur_pos==0 && set_blink_flag==0)
				{
					if(screen_num==2)
					{
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
			break;
	}
}
