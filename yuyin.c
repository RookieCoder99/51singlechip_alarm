/******************
*******************/
#include <reg51.H>
#include <intrins.h>
#include "main.h"
#include "delay.h"
#include "hb.h"
#include "ds1302.h"
#include "iic.h"
#include "ds18B20.h"
#include "switch_screen.h"
uchar MUSIC_YY_ADDR=0x01;
uchar wakeup_late=0;
uchar music_play_time=0;
uchar i_alarm=0;
extern uchar volume;
extern SYSTEM_TIME str_time_alarm_flag[2];
extern bit leap_month_flag;
extern uchar festival_num;
extern uchar music_num;//�ڼ�������������,Ĭ��Ϊ��һ��
extern uchar alarm_set;
extern SYSTEM_TIME str_time_alarm[2];
extern uchar pbuff[2];
extern SYSTEM_TIME str_time;
extern SYSTEM_TIME str_time_lunar;
extern uchar alarm_pos;//
extern uchar alarm_set;
extern uchar cur_pos;
extern uchar screen_num;
extern uchar up_temper;
extern uchar down_temper;
void Delay1us(uchar z)//ʵ����10΢��
{
	uchar v;
	for(v=z;v>0;v--)
	{
		_nop_();_nop_();
		_nop_();_nop_();
		_nop_();_nop_();
	}
}
void SendData (uchar MUSIC_YY_ADDR)//?????
{
    uchar i;

    EA = 0;/*???????,????????  */

    YYSDA = 1;/*????  */
    Delay1us ( 100 );
    YYSDA = 0;/*?????*/
    Delay1us ( 220 );/*?????????2ms,??????310ms  */

    for ( i = 0; i < 8; i++ ) /*??8???  */
    {
        YYSDA = 1;
        if ( MUSIC_YY_ADDR & 0x01 ) /*3:1?????1,??????????  */
        {
            Delay1us ( 50 );
            YYSDA = 0;
            Delay1us ( 20);
        }
        else              /*1:3?????0 ,??????????  */
        {
            Delay1us ( 20 );
            YYSDA = 0;
            Delay1us ( 50 );
        }
        MUSIC_YY_ADDR >>= 1;
    }
    YYSDA = 1;
    EA = 1;//????
}

void choose_music(void)
{
	clean_all();
	SendData(0x0a);
	SendData(volume);
	SendData(0x00);
	SendData(0x0c);
	while(1)
	{
		show_choose_music(music_num);
		//music_num=0;//�Ƿ��Ѿ����ù���������,1��������Ϊ��������
		//��ʼ������,�ӵ�һ����ʼ
		SendData(0x0a); 		
		SendData(0x05);
		SendData(MUSIC_YY_ADDR+music_num-0x01);//���趨�����ֿ�ʼ��
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			if(CHOOSE_MUSIC==0)//ȷ�Ͻ���ǰ������Ϊ����
			{
				delay_ms(40);
				if(CHOOSE_MUSIC==0)
				{
					break;
				}
			}
			if(ADD==0)
			{
				delay_ms(90);
				if(ADD==0)
				{
					if(music_num==3)
					{
						music_num=0;
					}
					music_num+=1;
					break;
				}
			}
			if(SUB==0)//
			{
				delay_ms(90);
				if(SUB==0)
				{
					if(music_num==1)
					{
						music_num=4;
					}
					music_num-=1;
					break;
				}
			}
		}
		if(CHOOSE_MUSIC==0)//ȷ�Ͻ���ǰ������Ϊ����
		{
				delay_ms(40);
				if(CHOOSE_MUSIC==0)
				{
					//ֹͣ����
					SendData(0x0a);
					SendData(0x05);
					SendData(MUSIC_YY_ADDR+music_num-0x01);
					SendData(0x12);
					clean_all();
					
//					alarm_set=1;
//					show_alarm_all(alarm_set);
//					show_time_only(str_time_alarm[0]);
//					show_alarm();
//					start_blink_alarm();
					write_IIC(MUSIC_IIC_ADDR,music_num);
					break;
				}
		}
		if(MUSIC_YY_ADDR>0x03)
		{
			MUSIC_YY_ADDR=0x01;
		}
	}
}
void music_screen()
{
			while(!CHOOSE_MUSIC)
			{
				delay_ms(40);
				if(CHOOSE_MUSIC!=0) //�������Ϊ�Ǹ���ť���ǻ��󴥵��¶���
				{
					break;
				}
				while(1)
				{
					if(CHOOSE_MUSIC==0)
					{
						delay_ms(40);
						if(CHOOSE_MUSIC==0)
						{
							choose_music();
							break;
						}							
					}
				}
				break;
			}
}
void adjust_volume()
{
//��ʼ����Ϊ10
	SendData(0x0a);
	SendData(volume);
	SendData(0x00);
	SendData(0x0c);
	while(1)
	{
		show_volume(volume);
		
		if(ADD==0)
		{
			delay_ms(90);
			if(ADD==0)
			{
				if(volume==9)
				{
					volume-=1;
				}
				volume++;
				SendData(0x0a);
				SendData(volume);
				SendData(0x00);
				SendData(0x0c);
				//��
				SendData(0x0a);
				SendData(0x02);
				SendData(0x05);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY);
			}
		}
		if(SUB==0)
		{
			delay_ms(90);
			if(SUB==0)
			{
				if(volume==0)
				{
					volume+=1;
				}
				volume--;
				SendData(0x0a);
				SendData(volume);
				SendData(0x00);
				SendData(0x0c);
				//��
				SendData(0x0a);
				SendData(0x02);
				SendData(0x05);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY);
			}
		}
		if(VOLUME==0)
		{
			delay_ms(60);
			if(VOLUME==0)
			{
				clean_all();
				break;
			}
		}
	}
	//eeprom
	write_IIC(VOLUME_ADDR,volume);
}

void volume_control()
{
		while(!VOLUME && alarm_pos==0 && cur_pos==0)
		{
			delay_ms(30);	
			if(VOLUME!=0) //�������Ϊ�Ǹ���ť���ǻ��󴥵��¶���
			{
				break;
			}
			if(VOLUME==0)
			{
				delay_ms(50);
				if(VOLUME==0)
				{
					if(alarm_set==1 || alarm_set==2)
					{
						stop_blink_alarm();
					}
					clean_all();
					adjust_volume();
					break;
				}
			}
		}
}


//�ж���ô������01,0��ʱ������ʱ����
void broadcast(uchar temp,uchar flag)
{
	uchar shiwei;
	uchar gewei;
	uint int_temp=(uint)temp;
	shiwei=(uchar)int_temp/10;
	gewei=(uchar)int_temp%10;
	if(flag==1)//��ʮ
	{
		switch(shiwei)
		{
			case 0:break;
			//����10��Ӧ���ļ���
			case 1://ʮλ��1
				SendData(0x0a); 
				SendData(0x04);
				SendData(0x00);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY)
				{
					str_time = read_time();		//��ȡʱ��		
					show_time_screen();
					/*
					hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
					read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
					temper_convert();	// �����¶�ת������Ҫ750ms
					hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
					*/
				}
				break;
			default:
				SendData(0x0a);
				//�����ļ�����3��ͷ�ģ��ȴ�һ��3���ٴ�ʮλ��Ӧ������
				SendData(0x03);
				SendData(shiwei);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY)
				{
						str_time = read_time();		//��ȡʱ��		
						show_time_screen();
						/*
						hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
						read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
						temper_convert();	// �����¶�ת������Ҫ750ms
						hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
					*/
				}
				SendData(0x0a); 
				//����10��40��ʼ
				SendData(0x04);
				SendData(0x00);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY)
				{
					str_time = read_time();		//��ȡʱ��		
					show_time_screen();
					/*
					hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
					read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
					temper_convert();	// �����¶�ת������Ҫ750ms
					hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
					*/
				}
				//shi=1;
				break;
		}
		switch(gewei)
		{
			case 0:
				if(shiwei==0)
				{
					SendData(0x0a); 
					SendData(0x03);
					SendData(0x00);
					SendData(0x0b);
					while(!YYBUSY);
					while(YYBUSY)
					{
						str_time = read_time();		//��ȡʱ��
						show_time_screen();						
						/*
						hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
						read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
						temper_convert();	// �����¶�ת������Ҫ750ms
						hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
						*/
					}
				}
					break;
			default:
				SendData(0x0a); 
				SendData(0x03);
				SendData(gewei);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY)
				{
					str_time = read_time();		//��ȡʱ��		
					show_time_screen();
					/*
					hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
					read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
					temper_convert();	// �����¶�ת������Ҫ750ms
					hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
					*/
				}
				break;
		}
	}
	else//��1,0
	{
		SendData(0x0a); 
		SendData(0x03);
		SendData(shiwei);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��		
			show_time_screen();
			/*
			hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
			read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
			temper_convert();	// �����¶�ת������Ҫ750ms
			hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
			*/
		}
		SendData(0x0a); 
		SendData(0x03);
		SendData(gewei);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��		
			show_time_screen();
			/*
			hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
			read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
			temper_convert();	// �����¶�ת������Ҫ750ms
			hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
			*/
		}
	}
}



void broadcast_temp(uchar pbuff[2])
{
		//�����¶�
	uchar symbol=0;
	uint dec_temper;
	SendData(0x0a); 
	SendData(0x05);
	SendData(0x00);
	SendData(0x0b);
	while(!YYBUSY);
	while(YYBUSY)
	{
		str_time = read_time();		//��ȡʱ��		
		show_time_screen();
		/*
		hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
		read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
		temper_convert();	// �����¶�ת������Ҫ750ms
		hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
		*/
	}
	
	if((pbuff[0] &0x80) != 0)
	{
		symbol = 1;				//����Ϊ��
		pbuff[0] = ~pbuff[0];
		//dec_temper = (0x0F-dec_temper) +1;	//����ȡ����1
		pbuff[1]=pbuff[1]&0x0f;
		pbuff[1] = (0x0F-pbuff[1]) +1;
		if(pbuff[1] == 0x10)			//С�����ֽ�λ����������λ
		{
			pbuff[0] += 1;
			pbuff[1] = 0;
		}
	}
	
	if(symbol==1)//�¶�����
	{
			SendData(0x0a); 
			SendData(0x02);
			SendData(0x04);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��	
				show_time_screen();				
				/*
				hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
				read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
				temper_convert();	// �����¶�ת������Ҫ750ms
				hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
				*/
			}
	}
	
	broadcast(pbuff[0],1);
	
	//���ֵ�
	SendData(0x0a); 
	SendData(0x04);
	SendData(0x06);
	SendData(0x0b);
	while(!YYBUSY);
	while(YYBUSY)
	{
		str_time = read_time();		//��ȡʱ��	
		show_time_screen();		
		/*
		hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
		read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
		temper_convert();	// �����¶�ת������Ҫ750ms
		hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
		*/
	}
	
	//�¶�С��λ��ֻ��һλ
	
	dec_temper = (uint)pbuff[1];
	dec_temper = dec_temper * 625 + 500;	//�԰ٷ�λ������������
	dec_temper = dec_temper/1000;		//ֻ��ʾ1λС��
	pbuff[1]=(uchar)dec_temper;
	broadcast(pbuff[1],1);
	
	//�������϶�
	SendData(0x0a); 
	SendData(0x04);
	SendData(0x07);
	SendData(0x0b);
	while(!YYBUSY);
	while(YYBUSY)
	{
		str_time = read_time();		//��ȡʱ��		
		show_time_screen();
		/*
		hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
		read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
		temper_convert();	// �����¶�ת������Ҫ750ms
		hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
		*/
	}
	
	if(pbuff[0]>=up_temper)//����
	{
		SendData(0x0a); 
		SendData(0x04);
		SendData(0x04);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��	
			show_time_screen();			
			/*
			hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
			read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
			temper_convert();	// �����¶�ת������Ҫ750ms
			hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
			*/
		}
	}
	else if(pbuff[0]<down_temper)//��ů
	{
		SendData(0x0a);
		SendData(0x04);
		SendData(0x05);
		SendData(0x0b);
		
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��		
			show_time_screen();
			/*
			hb_show_time(str_time);		//��ʾ�ꡢ�¡��ա�ʱ���֡��롢����
			read_temper(pbuff);		//��ȡ��ǰ�¶�ֵ
			temper_convert();	// �����¶�ת������Ҫ750ms
			hb_show_temp(pbuff[0], pbuff[1]);		//��ʾ��ǰ�¶�
			*/
		}
	}
}
//��ʱ
void check_now_time(SYSTEM_TIME str_time)
{
		uint tmp_hour;
		uchar shiwei;
		uchar gewei;
		tmp_hour=(uint)str_time.hour;
		shiwei=(uchar)tmp_hour/10;
		gewei=(uchar)tmp_hour%10;
		SendData(0x0a);
		//��ʱֱ�ӷ��ͼ���ͺ�
		SendData(shiwei);
		SendData(gewei);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{		
			str_time = read_time();		//��ȡʱ��	
			show_time_screen();
		}
		
}
void broadcast_morning(SYSTEM_TIME str_time)
{
		//���Ϻ�,������
		SendData(0x0a); 
		SendData(0x04);
		SendData(0x08);
		SendData(0x0b);
		
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��		
			show_time_screen();
		}
		//2
		SendData(0x0a); 
		SendData(0x03);
		SendData(0x02);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();			
		}
		//0
		SendData(0x0a); 
		SendData(0x03);
		SendData(0x00);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();			
		}
		//������ flag=0��10����������
		broadcast(str_time.year,0);
		
	//��
		SendData(0x0a);
		SendData(0x04);
		SendData(0x01);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();			
		}
	//��
		broadcast(str_time.month,1);
		SendData(0x0a); 
		SendData(0x04);
		SendData(0x02);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();			
		}
		broadcast(str_time.date,1);
	//��
		SendData(0x0a);
		SendData(0x04);
		SendData(0x03);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��
			show_time_screen();			
		}
		
	//����
		SendData(0x0a); 
		SendData(0x04);
		SendData(0x09);
		SendData(0x0b);
		while(!YYBUSY);
		while(YYBUSY)
		{
			str_time = read_time();		//��ȡʱ��
				show_time_screen();			
		}
		//��
		
		if(calculate_day_num(str_time)!=0)
		{
			broadcast(calculate_day_num(str_time),1);
		}
		else
		{	
			SendData(0x0a);
			SendData(0x04);
			SendData(0x03);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();			
			}
		}
		
		//ũ��
			SendData(0x0a);
			SendData(0x02);
			SendData(0x09);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
			
			
			
		if(leap_month_flag==1)
		{
			SendData(0x0a);
			SendData(0x02);
			SendData(0x08);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
		}
		if(str_time_lunar.month==1)
		{
			SendData(0x0a);
			SendData(0x05);
			SendData(0x04);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
		}
		else if(str_time_lunar.month==11)
		{
			SendData(0x0a);
			SendData(0x05);
			SendData(0x05);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
		}
		else if(str_time_lunar.month==12)
		{
			SendData(0x0a);
			SendData(0x05);
			SendData(0x06);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
		}
		else if(str_time_lunar.month==10)
		{
			SendData(0x0a);
			SendData(0x04);
			SendData(0x00);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();		//��ȡʱ��
				show_time_screen();	
			}
		}
		else
		{
			SendData(0x0a);
			SendData(0x03);
			SendData(str_time_lunar.month);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
		}
		//��
			SendData(0x0a);
			SendData(0x04);
			SendData(0x02);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
			
			if(str_time_lunar.date<=10)
			{
				//��
				SendData(0x0a);
				SendData(0x02);
				SendData(0x07);
				SendData(0x0b);
				while(!YYBUSY);
				while(YYBUSY)
				{
					str_time = read_time();	//��ȡʱ��
					show_time_screen();	
				}				
				
				if(str_time_lunar.date==10)
				{
					SendData(0x0a);
					SendData(0x04);
					SendData(0x00);
					SendData(0x0b);
					while(!YYBUSY);
					while(YYBUSY)
					{
						str_time = read_time();	//��ȡʱ��
						show_time_screen();	
					}		
				}
				else{
					SendData(0x0a);
					SendData(0x03);
					SendData(str_time_lunar.date);
					SendData(0x0b);
					while(!YYBUSY);
					while(YYBUSY)
					{
						str_time = read_time();	//��ȡʱ��
						show_time_screen();	
					}	
				}
			}
		else
		{
			broadcast(str_time_lunar.date,1);
		}
		check_now_time(str_time);
		
		if(festival_num!=0)
		{
			SendData(0x0a);
			SendData(0x06);
			SendData(0x01);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
			switch(festival_num)
			{
				case 1:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 2:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 3:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 4:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 5:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 6:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 7:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 8:
					SendData(0x0a);
					SendData(0x06);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 9:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 10:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 11:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 12:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 13:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 14:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 15:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 16:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 17:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 18:
					SendData(0x0a);
					SendData(0x07);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 19:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 20:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 21:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 22:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 23:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 24:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 25:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 26:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 27:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 28:
					SendData(0x0a);
					SendData(0x08);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 29:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 30:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 31:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 32:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 33:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 34:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 35:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 36:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 37:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 38:
					SendData(0x0a);
					SendData(0x09);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 39:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 40:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 41:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 42:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 43:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 44:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 45:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 46:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 47:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 48:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x00);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 49:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 50:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 51:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 52:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 53:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 54:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 55:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 56:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 57:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 58:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x01);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 59:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 60:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 61:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x02);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 62:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x03);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 63:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x04);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 64:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x05);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 65:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x06);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 66:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x07);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 67:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x08);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 68:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x02);
					SendData(0x09);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 69:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x03);
					SendData(0x00);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
				case 70:
					SendData(0x0a);
					SendData(0x01);
					SendData(0x03);
					SendData(0x01);
					SendData(0x0b);while(!YYBUSY);
			while(YYBUSY)
			{
				str_time = read_time();	//��ȡʱ��
				show_time_screen();	
			}
					break;
			}

		}
		
		
}

void welcome_music(uchar volume_start)
{

			SendData(0x0a);
			SendData(volume_start);
			SendData(0x00);
			SendData(0x0c);
	
			SendData(0x0a);
			SendData(0x02);
			SendData(0x06);
			SendData(0x0b);
			while(!YYBUSY);
			while(YYBUSY)
			{
				welcome_init();
			}
}
void check_alarm_time(SYSTEM_TIME str_time,
												SYSTEM_TIME str_time_alarm[2],
												uchar alarm_show_flag[2][7])
{
	uint tmp_year,tmp_month,tmp_date;
	uint iWeekDay = 10;
	tmp_year=(uint)str_time.year;
	tmp_month=(uint)str_time.month;
	tmp_date=(uint)str_time.date;
	
	 if (1 == tmp_month || 2 == tmp_month)
    {
        tmp_month += 12;
        tmp_year--;
    }
    iWeekDay = (tmp_date + 1 + 2 * tmp_month + 3 * (tmp_month + 1) / 5 +
								tmp_year + tmp_year / 4 - tmp_year / 100 + tmp_year / 400) % 7;
		//���Ϻ����Ǽ������ڶ�Ӧ������ iWeekDay:0-sun;1-mon
		
		//��������
		SendData(0x0a);
		SendData(volume);
		SendData(0x00);
		SendData(0x0c);
		for(i_alarm=0;i_alarm<2;i_alarm++)
		{
					if(str_time.hour==str_time_alarm[i_alarm].hour &&
							str_time.min==str_time_alarm[i_alarm].min &&
							str_time.sec==str_time_alarm[i_alarm].sec &&
							alarm_show_flag[i_alarm][iWeekDay] || music_play_time!=0)
					{
						SendData(0x0a);
						SendData(0x05);
						SendData(MUSIC_YY_ADDR+music_num-0x01);//���趨�����ֿ�ʼ��
						SendData(0x0b);//������
						music_play_time+=1;
						while(!YYBUSY);
						while(YYBUSY)
						{
							str_time = read_time();		//��ȡʱ��		
							if(music_play_time!=0)
							{
								if(music_play_time==3)
								{
									music_play_time=0;
								}
							}
							show_time_screen();
							//i_alarm=0;//��֤�´μ�����ӹ�������
							if(SWITCH==0 || ADD==0 || SUB==0 || 
									ALARM==0 || CHOOSE_MUSIC ==0 || VOLUME==0)
							{
								delay_ms(90);
								if(SWITCH==0 || ADD==0 || SUB==0 || 
									ALARM==0 || CHOOSE_MUSIC ==0 || VOLUME==0 )
								{
									//ֹͣ����
									SendData(0x0a);
									SendData(0x05);
									SendData(MUSIC_YY_ADDR+music_num-0x01);
									SendData(0x12);
									music_play_time=0;
									if(wakeup_late!=0)
									{
										//str_time_alarm[i_alarm]=str_time_alarm_flag[i_alarm];
										if(str_time_alarm[i_alarm].min<wakeup_late*5)
										{
											str_time_alarm[i_alarm].min=str_time_alarm[i_alarm].min-wakeup_late*5+60;
											//str_time_alarm[i_alarm].hour=23;
											if(str_time_alarm[i_alarm].hour!=0)
											{
												str_time_alarm[i_alarm].hour-=1;						
											}
											else
											{
												str_time_alarm[i_alarm].hour=23;
											}
											//wakeup_late=0;
										}
										else
										{
											str_time_alarm[i_alarm].min=str_time_alarm[i_alarm].min-wakeup_late*5;
											//wakeup_late=0;
										}
										wakeup_late=0;
									}		
									break;
								}
							}
							if(SHAKE==0)
							{
								wakeup_late+=1;
								SendData(0x0a);
								SendData(0x05);
								SendData(MUSIC_YY_ADDR+music_num-0x01);
								SendData(0x12);
								str_time_alarm[i_alarm].min+=5;
								if(str_time_alarm[i_alarm].min>=60)
								{
									str_time_alarm[i_alarm].hour+=1;
									if(str_time_alarm[i_alarm].hour==24)
									{
										str_time_alarm[i_alarm].hour=0;
									}
									str_time_alarm[i_alarm].min-=60;
								}
								if(wakeup_late==3)
								{
									wakeup_late=0;
									if(str_time_alarm[i_alarm].min<15)
									{
										str_time_alarm[i_alarm].min=str_time_alarm[i_alarm].min+60-15;
										if(str_time_alarm[i_alarm].hour==0)
										{
											str_time_alarm[i_alarm].hour=23;
										}
										else
										{
											str_time_alarm[i_alarm].hour-=1;
										}
									}
									else
									{
										str_time_alarm[i_alarm].min=str_time_alarm[i_alarm].min-15;
									}
								}
								music_play_time=0;
								break;
							}
						
						}
						//break;��ǰû��
					}
		}
}