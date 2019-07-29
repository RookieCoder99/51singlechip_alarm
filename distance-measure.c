#include <reg52.h>	                      
#include <stdio.h>							  
#include <absacc.h>							 
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit Trig= P1^3;
void square();
void square(void)
{
	uint i;
	for (i = 8; i > 0; i--);
	{
		Trig=1;
		nop_();
		Trig=0;
		nop_();
	};
}
float dist()
{
uint echo_time;
uint timeH;
	uint timeL;
float dist; 
echo_time=0;
IT0=1;


TR0=1;
square();
	while(TH<30);
EA=1;
EX0=1;
timeH=TH0;
timeL=TL0;
echo_time=256*timeH+timeL;	
dist=(float)echo_time*(331.4+0.607*temp())/20000;
return dist;
while(1);
}
void receive1_int() interrupt 0      //int0	
{
  TR0=0;

	EA=0;
	EX0=EX1=0;			
}
void timer4_int() interrupt 1      
{
  TR0=0;
	TH0=0;
	TL0=0;
}
