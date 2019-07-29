#include <stdio.h>
#include "string.h"
#include "main.h"
#include "delay.h"
#include "string.h"
#include "ds1302.h"
#include "alarm.h"
#include "hb.h"
#include "stdlib.h"
#include "iic.h"
sbit LED1=P2^5;
extern bit light;//液晶亮不亮
extern uchar wifi_light;
extern uchar sleep_flag;
extern uchar alarm_show_flag[2][7];
extern SYSTEM_TIME str_time_alarm_flag[2];
extern uchar wifi_light_flag;
extern uchar broadcast_morning_flag_flag;
extern uchar broadcast_now_time_flag_flag;
uchar data alarm_wifi_state[2];
uchar data Rxbuff[40],Rxnum;//串口收到的
uchar data wifi_data[8];
uchar  data str2[25];
uchar *data strx=&str2[0];
uchar  clinetid;
uchar  getflag;   //getflag判断有没有发消息过来，1-发了消息
extern bit start_music;//开机音乐
extern uchar down_temper,up_temper;
extern uchar broadcast_morning_flag;//早上是否七点播报
extern uchar broadcast_now_time_flag;//整点是否播报
extern SYSTEM_TIME str_time;
extern SYSTEM_TIME str_time_alarm[2];
extern uchar music_num;
extern uchar volume;
extern SYSTEM_TIME str_time_sleep[2];

void InitUART(void)
{
	SCON=0x50;
	TMOD=0x20;
	TH1=TL1=0xfd;
	TR1=1;
	ES=1;
}

void Clear_Buffer(void)
{
	Rxnum=0;
}

void Clear_Rxbuff(void)
{
	while(Rxnum<40)
	{
		Rxbuff[Rxnum]=0;
		Rxnum++;
	}
	Rxnum=0;
}

void delayms(unsigned int x)
{
	unsigned int i;
	while(x--)
		for(i=125;i>0;i--);
}

void UART_send_byte(char dat)
{
	SBUF = dat;
	while (TI == 0);
	TI = 0;
}

void Send_Str(unsigned char *buf)
{
	while (*buf != '\0')
	{
		UART_send_byte(*buf++);
	}
}
void ESP8266_SERVER(void)
{

	Send_Str("AT\r\n");
	delayms(500);
	strx=strstr((const char*)Rxbuff,(const char*)"OK");
	while(strx==NULL)
	{
		Send_Str("AT\r\n");
		delayms(500);
		strx=strstr((const char*)Rxbuff,(const char*)"OK");
	}
	
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("ATE0\r\n");
	delayms(500);
  Send_Str("AT+CWSAP=\"ESP8266_xjh\",\"12345678\",5,3\r\n");	  //5 3;11 0
	delayms(500);
	Send_Str("AT+CIPMUX=1\r\n");
	delayms(500);	
	
	Clear_Buffer();	//指针返回头
	Send_Str("AT+CIPSERVER=1,8888\r\n");
	delayms(500);	
	strx=strstr((const char*)Rxbuff,(const char*)"OK");
	
	while(strx==NULL)
	{		
		Clear_Buffer();	 
		delayms(500);
		strx=strstr((const char*)Rxbuff,(const char*)"OK");
	}
	Clear_Buffer();	
	LED1=0;
}

uchar string_to_uchar(uchar in[5])
{
	uchar res=0;
	uchar i=0;
	while(in[i]!='\0')
	{
		res=(10*res+in[i]-0x30);  
		i++;
	}
	return res;
}

void Get_Clinet(void)
{
	uchar i=0,t=0;
	uchar data wifi_data_temp[8];
	uchar data sleep_data[10];
  getflag=0;
	  Clear_Buffer();
		strx=strstr((const char*)Rxbuff,(const char*)"+IPD");
		if(*strx=='+')
		{
			getflag=1;
			//LED1=1;
		}
	
	if(getflag)
	{

		delayms(500);
		clinetid=strx[5];
		strx=strstr((const char*)Rxbuff,(const char*)":");
		if(strx[1]=='T')
		{
			for(i=0;i<2;i++)
				wifi_data[i]=strx[2+i];
			str_time.year=string_to_uchar(wifi_data);

			for(i=0;i<2;i++)
				wifi_data[i]=strx[4+i];
			str_time.month=string_to_uchar(wifi_data);
			
			for(i=0;i<2;i++)
				wifi_data[i]=strx[6+i];
			str_time.date=string_to_uchar(wifi_data);
		
			for(i=0;i<2;i++)
				wifi_data[i]=strx[8+i];
			str_time.hour=string_to_uchar(wifi_data);
			
			for(i=0;i<2;i++)
				wifi_data[i]=strx[10+i];
			str_time.min=string_to_uchar(wifi_data);
			
			for(i=0;i<2;i++)
				wifi_data[i]=strx[12+i];
			str_time.sec=string_to_uchar(wifi_data);
			set_time(str_time);
			
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
	}
	
		
		
		if(strx[1]=='S'&&strx[2]=='O'&&strx[3]=='N')
		{
			sleep_flag=1;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			write_IIC(SLEEP_FLAG_ADDR,sleep_flag);
		}
		else if(strx[1]=='S'&&strx[2]=='O'&&strx[3]=='F')
		{
			sleep_flag=0;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			write_IIC(SLEEP_FLAG_ADDR,sleep_flag);
		}
		if(strx[1]=='S' && strx[2]=='S')
		{
			if(sleep_flag==1)
			{
				for(i=0;i<2;i++)
					wifi_data[i]=strx[3+i];
				str_time_sleep[0].hour=string_to_uchar(wifi_data);
				
				for(i=0;i<2;i++)
					wifi_data[i]=strx[5+i];
				str_time_sleep[0].min=string_to_uchar(wifi_data);				
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(SLEEP0_ADDR,str_time_sleep[0].hour);
				write_IIC(SLEEP0_ADDR+1,str_time_sleep[0].min);
			}
		}
		if(strx[1]=='S' && strx[2]=='E')
		{
			if(sleep_flag==1)
			{
				for(i=0;i<2;i++)
					wifi_data[i]=strx[3+i];
				str_time_sleep[1].hour=string_to_uchar(wifi_data);
				for(i=0;i<2;i++)
					wifi_data[i]=strx[5+i];
				str_time_sleep[1].min=string_to_uchar(wifi_data);
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(SLEEP1_ADDR,str_time_sleep[1].hour);
				write_IIC(SLEEP1_ADDR+1,str_time_sleep[1].min);
			}
		}
		if(strx[1]=='A' && strx[2]=='O' && strx[3]=='N')
		{
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			alarm_wifi_state[0]=1;
				for(i=0;i<7;i++)
				{
					alarm_show_flag[0][i]=0xff;
				}
		}
		else if(strx[1]=='A' && strx[2]=='O' && strx[3]=='F')
		{
			alarm_wifi_state[0]=0;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
				for(i=0;i<7;i++)
				{
					alarm_show_flag[0][i]=0x00;
				}
			str_time_alarm[0].hour=7;
			str_time_alarm[0].min=30;
		}		
		
		if(strx[1]=='B' && strx[2]=='O' && strx[3]=='N')
		{
			alarm_wifi_state[1]=1;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			for(i=0;i<7;i++)
			{
				alarm_show_flag[1][i]=0xff;
			}
		}
		else if(strx[1]=='B' && strx[2]=='O' && strx[3]=='F')
		{
			alarm_wifi_state[1]=0;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);

			for(i=0;i<7;i++)
			{
				alarm_show_flag[1][i]=0x00;
			}
			str_time_alarm[1].hour=7;
			str_time_alarm[1].min=30;
		}				
		//闹铃A
		if(strx[1]=='A'  &&strx[2]!='O')
		{
			if( alarm_wifi_state[0]==1)
			{
				for(i=0;i<2;i++)
					wifi_data[i]=strx[2+i];
				str_time_alarm[0].hour=string_to_uchar(wifi_data);
				
				for(i=0;i<2;i++)
					wifi_data[i]=strx[4+i];
				str_time_alarm[0].min=string_to_uchar(wifi_data);
			}
			str_time_alarm_flag[0]=str_time_alarm[0];
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
		}
		//闹铃B
		if(strx[1]=='B' && strx[2]!='O')
		{
			if(alarm_wifi_state[1]==1)
			{			
				for(i=0;i<2;i++)
					wifi_data[i]=strx[2+i];
				str_time_alarm[1].hour=string_to_uchar(wifi_data);
				
				for(i=0;i<2;i++)
					wifi_data[i]=strx[4+i];
				str_time_alarm[1].min=string_to_uchar(wifi_data);
			}
			str_time_alarm_flag[1]=str_time_alarm[1];
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
		}		
					//温度上下限调节
			if(strx[1]=='P')
			{
				if(strx[2]!='E')
				{
					for(i=0;i<2;i++)
						wifi_data_temp[i]=strx[2+i];
					up_temper=string_to_uchar(wifi_data_temp);
					for(i=0;i<2;i++)
						wifi_data_temp[i]=strx[4+i];
					down_temper=string_to_uchar(wifi_data_temp);
				}
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(DAT_ADDR,down_temper);
				write_IIC(DAT_ADDR+1,up_temper);	
			}
			
			if(strx[1]=='C') //校正液晶背光
			{
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				Screen_Con(LIGHT_SET);delay_ms(5);
			}
			
			if(strx[1]=='l')
			{
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				if(strx[2]=='0')//关闭
				{
					wifi_light=0;
					wifi_light_flag=0;
					if(light==1)
					{
						Screen_Con(LIGHT_SET);delay_ms(5);
						light=0;
					}
				}
				else if(strx[2]=='1')//智能
				{
					wifi_light=1;
					wifi_light_flag=1;
				}
				else if(strx[2]=='2')//开启
				{
					wifi_light=2;
					wifi_light_flag=2;
					if(light==0)
					{
						Screen_Con(LIGHT_SET);delay_ms(5);
						light=1;
					}
				}
				write_IIC(WIFI_LIGHT_ADDR, wifi_light);
				write_IIC(WIFI_LIGHT_FLAG_ADDR, wifi_light_flag);
			}
			
		//音乐选择
			if(strx[1]=='M')
			{
				if(strx[2]=='1')
				{
					music_num=1;
				}
				else if(strx[2]=='2')
				{
					music_num=2;
				}
				else if(strx[2]=='3')
				{
					music_num=3;
				}
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(MUSIC_IIC_ADDR, music_num);
			}
			
			//整点报时开关
			if(strx[1]=='F' && strx[2]=='O' && strx[3]=='N')
			{
				broadcast_now_time_flag=1;
				broadcast_now_time_flag_flag=1;
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(NOW_TIME_ADDR,broadcast_now_time_flag);
				write_IIC(NOW_TIME_FLAG_ADDR,broadcast_now_time_flag_flag);
			}
			else if(strx[1]=='F' && strx[2]=='O' && strx[3]=='F')
			{
				broadcast_now_time_flag=0;
				broadcast_now_time_flag_flag=0;
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(NOW_TIME_FLAG_ADDR,broadcast_now_time_flag_flag);
				write_IIC(NOW_TIME_ADDR,broadcast_now_time_flag);
			}
			//每日播报开关
			if(strx[1]=='G' && strx[2]=='O' && strx[3]=='N')
			{
				broadcast_morning_flag=1;
				broadcast_morning_flag_flag=1;
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(MORNING_ADDR,broadcast_morning_flag);
				write_IIC(MORNING_FLAG_ADDR,broadcast_morning_flag_flag);
			}
			else if(strx[1]=='G' && strx[2]=='O' && strx[3]=='F')
			{
				broadcast_morning_flag_flag=0;
				broadcast_morning_flag=0;
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(MORNING_ADDR,broadcast_morning_flag);
				write_IIC(MORNING_FLAG_ADDR,broadcast_morning_flag_flag);
			}
			
		if(strx[1]=='V')
		{
				if(strx[2]=='1')
				{
					volume=1;
				}
				else if(strx[2]=='2')
				{
					volume=2;
				}
				else if(strx[2]=='3')
				{
					volume=3;
				}
				else if(strx[2]=='4')
				{
					volume=4;
				}			
				else if(strx[2]=='5')
				{
					volume=5;
				}
				else if(strx[2]=='6')
				{
					volume=6;
				}
				else if(strx[2]=='7')
				{
					volume=7;
				}
				else if(strx[2]=='8')
				{
					volume=8;
				}
				else if(strx[2]=='9')
				{
					volume=9;
				}
				LED1=~LED1;delay_ms(80);
				LED1=~LED1;delay_ms(5);
				write_IIC(VOLUME_ADDR, volume);
		}
		
		//第一个闹铃星期几
		if(strx[1]=='D')
		{
			LED1=~LED1;delay_ms(80);
		LED1=~LED1;delay_ms(5);
//			Screen_Con(LIGHT_SET);delay_ms(80);
//			Screen_Con(LIGHT_SET);delay_ms(5);
			if(alarm_wifi_state[0]==1)
			{
				for(i=0;i<7;i++)
				{
					if(strx[2+i]=='1')
					{
						alarm_show_flag[0][i]=0xff;
					}
					else if(strx[2+i]=='0')
					{
						alarm_show_flag[0][i]=0x00;
					}
				}
			}
			if(alarm_wifi_state[0]==0)
			{
				for(i=0;i<7;i++)
				{
					alarm_show_flag[0][i]=0x00;
				}
			}
		}
		//第2个闹铃星期几
		if(strx[1]=='E')
		{
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			if(alarm_wifi_state[1]==1)
			{
				for(i=0;i<7;i++)
				{
					if(strx[2+i]=='1')
					{
						alarm_show_flag[1][i]=0xff;
					}
					else if(strx[2+i]=='0')
					{
						alarm_show_flag[1][i]=0x00;
					}
				}
			}
			if(alarm_wifi_state[1]==0)
			{
				for(i=0;i<7;i++)
				{
					alarm_show_flag[1][i]=0x00;
				}
			}
		}
		
		if(strx[1]=='K' && strx[2]=='O' && strx[3]=='N')
		{
			start_music=1;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			write_IIC(START_MUSIC_ADDR,start_music);
		}
		else if(strx[1]=='K' && strx[2]=='O' && strx[3]=='F')
		{
			start_music=0;
			LED1=~LED1;delay_ms(80);
			LED1=~LED1;delay_ms(5);
			write_IIC(START_MUSIC_ADDR,start_music);
		}
  	alarm_write_iic(str_time_alarm,alarm_show_flag);
		Clear_Buffer();	
		Clear_Rxbuff();
		strx=NULL;
		str2[0]=NULL;
		wifi_data[0]=NULL;
		sleep_data[0]=NULL;
		//free(strx);
	}
	return ;
}


void wifi_init()
{

	delayms(200);
  ESP8266_SERVER();
	delayms(100);
	strx=NULL;
	Screen_Con(LIGHT_SET);Delay_1ms(DELAY_TIME);
}
void UARTInterrupt(void) interrupt 4//接收
{	
	if(RI)
	{
		Rxbuff[Rxnum++]=SBUF;
		RI = 0;
		if(Rxnum>=40)
			Rxnum=0;
   }
}