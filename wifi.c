#include <reg51.h>
#include <stdio.h>
#include <main.h>
#include "delay.h"
#include "string.h"
#include "ds1302.h"
sbit WIFI_LED=P2^5;

uchar idata Rxbuff[50],Rxnum;//串口收到的
uchar	Recwifi_data[5];
char *app_message=0;
char clinetid;
uchar getflag;   //getflag判断有没有发消息过来，1-发了消息
uchar service_time;  //自动校正时间按钮
extern SYSTEM_TIME str_time;
extern SYSTEM_TIME str_time_alarm[2];
extern uchar alarm_show_flag[2][7];
extern music_num;

void app_adjust(void);
void InitUART(void)
{
	SCON=0x50;
	TMOD=0x20;
	TH1=TL1=0xfd;
	TR1=1;
	EA=ES=1;
}
void Clear_Buffer(void)
{
	Rxnum=0;
}
 
void Clear_Rxbuff(void)
{
	while(Rxnum<50)
	{
		Rxbuff[Rxnum]=0;
		Rxnum++;
	}
	Rxnum=0;
}

void delayms1(unsigned int x)
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
	delayms1(500);
	app_message=strstr((const char*)Rxbuff,(const char*)"OK");
	while(app_message==NULL)
	{
		Send_Str("AT\r\n");
		delayms1(500);
		app_message=strstr((const char*)Rxbuff,(const char*)"OK");
	}
	
	Send_Str("AT\r\n");
	delayms1(500);
	Send_Str("AT\r\n");
	delayms1(500);
	Send_Str("ATE0\r\n");
	delayms1(500);
	
  Send_Str("AT+CWSAP=\"ESP8266_xjh\",\"12345678\",5,3\r\n");	  //5 3;11 0
	delayms1(500);
	Send_Str("AT+CIPMUX=1\r\n");
	delayms1(500);	
	
	Clear_Buffer();	//指针返回头
	Send_Str("AT+CIPSERVER=1,8888\r\n");
	delayms1(500);	
	app_message=strstr((const char*)Rxbuff,(const char*)"OK");
	
	while(app_message==NULL)
	{		Clear_Buffer();	 
			delayms1(500);
			
			app_message=strstr((const char*)Rxbuff,(const char*)"OK");
	}
	Clear_Buffer();	
	WIFI_LED=0;
}


uchar string_to_uchar(uchar in[5])
{
	uint res=0;
	uchar i=0;
	uint x;
	while(in[i]!='\0')
	{
		x=(uint)(in[i]-0x30);
		res=(10*res+x);
		i++;
	}
	WIFI_LED=0;
	return (uchar)res;
}



void Send_DATA(uchar *buffer)
{
		uchar i;
   	Send_Str("AT+CIPSEND=0,3\r\n");//AT+CIPSEND=<id>,<length> 
		delayms1(300);
   	Send_Str(buffer);//????
   	delayms1(100);
		app_message=strstr((const char*)Rxbuff,(const char*)"SEND OK");//??OK
		while(app_message==NULL)
		{
			delayms1(100);
			app_message=strstr((const char*)Rxbuff,(const char*)"SEND OK");//??OK
		}
	 	app_message=strstr((const char*)Rxbuff,(const char*)"+IPD");//?????	 ,???????????
	  if(app_message)//??????
	 {
			 clinetid=app_message[5];
			 app_message=strstr((const char*)Rxbuff,(const char*)":");//?????
			 for(i=0;i<5;i++)
			 Recwifi_data[i]=app_message[1+i];		  //??????????? app_message?????????
			 Clear_Buffer();	
    }
		Clear_Buffer();	 
}



void fasong(uchar in)
{
	unsigned char   Tx_Buf[10];
	uchar tmp;
		
	Tx_Buf[0]='S';	//约定的标志位
	tmp=(uint)(in)/10;
  Tx_Buf[1]=tmp+0x30;
	tmp=(uint)(in)%10;
	Tx_Buf[2]=tmp+0x30;
  Send_DATA(Tx_Buf);
}


void app_adjust(void)
{
	uchar i=0,t=0;
  getflag=0;
	app_message=NULL;
	if(service_time==0)
	{
		app_message=strstr((const char*)Rxbuff,(const char*)"CONNECT");
	}
	else 
	{
		app_message='1';
	}
	delayms1(500);
	while(app_message)
	{
		app_message=NULL;
	  Clear_Buffer();	
		app_message=strstr((const char*)Rxbuff,(const char*)"+IPD");
		if(*app_message=='+')
		{
			getflag=1;
			WIFI_LED=1;
			break;
		}
	}
	app_message=strstr((const char*)Rxbuff,(const char*)"CLOSE");
	
	if(app_message)
	{
	  Clear_Buffer();	
		getflag=0;
	}
	if(getflag)   
	{
		delayms1(1000);
		clinetid=app_message[5];
		app_message=strstr((const char*)Rxbuff,(const char*)":");
		if(app_message[1]=='T')
		{
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[2+i];
			str_time.year=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[4+i];
			str_time.month=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[6+i];
			str_time.date=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[8+i];
			str_time.hour=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[10+i];
			str_time.min=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[12+i];
			str_time.sec=string_to_uchar(Recwifi_data);
			
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[14+i];
			str_time.day=string_to_uchar(Recwifi_data);
			
			set_time(str_time);
		}

		if(app_message[1]=='A')  //闹钟1  
		{
				for(i=0;i<2;i++)
					Recwifi_data[i]=app_message[2+i];
				str_time_alarm[0].hour=string_to_uchar(Recwifi_data);
				
				for(i=0;i<2;i++)
					Recwifi_data[i]=app_message[4+i];
				str_time_alarm[0].min=string_to_uchar(Recwifi_data);
					
				for(i=6;i<13;++i)
				{
					Recwifi_data[i-6]=app_message[i];
				}
					switch(string_to_uchar(Recwifi_data))
					{
						case 0:
							alarm_show_flag[0][i-6]=0x00;
							break;
						case 1:
							alarm_show_flag[0][i-6]=0xff;
							break;
					}
		}
		
		if(app_message[1]=='B')  //闹钟2
		{
				for(i=0;i<2;i++)
					Recwifi_data[i]=app_message[2+i];
				str_time_alarm[1].hour=string_to_uchar(Recwifi_data);
				
				for(i=0;i<2;i++)
					Recwifi_data[i]=app_message[4+i];
				str_time_alarm[1].min=string_to_uchar(Recwifi_data);
					
				for(i=6;i<13;++i)
				{
					Recwifi_data[i-6]=app_message[i+7];
				}
				switch(string_to_uchar(Recwifi_data))
				{
					case 0:
						alarm_show_flag[1][i-6]=0x00;
						break;
					case 1:
						alarm_show_flag[1][i-6]=0xff;
						break;
					}
		}		
		if(app_message[1]=='M')  //音乐
		{
			for(i=0;i<2;i++)
				Recwifi_data[i]=app_message[2+i];
			music_num=string_to_uchar(Recwifi_data);
		}
		service_time=1;
		Clear_Buffer();
		Clear_Rxbuff();
	}
}





void wifi_init()
{
	delayms1(1000);
 	InitUART(); 
  ESP8266_SERVER();
	service_time=0;
	delayms1(500);
}

void UARTInterrupt(void) interrupt 4//接收
{	
	if(RI)
	{
		Rxbuff[Rxnum++]=SBUF;
		RI = 0;
		if(Rxnum>=50)
			Rxnum=0;
   }  
}