#include <reg52.h>	                      
#include <stdio.h>							  
#include <absacc.h>							 
#include <intrins.h>

//定义IC卡芯片的接口
sbit SCL_IC_CARD=P1^3;	   //24c01卡的3脚
sbit SDA_IC_CARD=P1^4;	   //24c01卡的6脚

//定义数组
unsigned char data  wr_buffer[3]={0xf0,0x99,0x77};
unsigned char data  rd_buffer[3];
void delay(int n);

/*定义IC卡子程序及变量*/

//定义IC卡的变量 
bdata char com_data;                          //将该变量定义在可按位寻址的内部数据存储器中
sbit mos_bit=com_data^7;
sbit low_bit=com_data^0;

bdata char rd_data;                           //将该变量定义在可按位寻址的内部数据存储器中
sbit rd_mbit=rd_data^7;
sbit rd_lbit=rd_data^0;

/*IC_card 子程序*/
unsigned char rd_24c01(unsigned char y,char a);  //y为器件地址，a为数据
void wr_24c01(unsigned char y,char a,char b);    //y为器件地址，a为数据，b

void start(void);                                //启动读写时序子程序
void stop(void);						                     //停止操作子程序
void ack(void);						                       //应答函数子程序
void wr_byte(char a);                            //I2C总线发送数据子程序
void rd_byte(void);

main()
{
  unsigned char i,IC_AD;
	IC_AD=0xa0;
 
	for(i=0;i<=2;i++)
	{
		  wr_24c01(IC_AD,i,wr_buffer[i]);
		  delay(250);
	}

  while(1)
  {	
	   for(i=0;i<=2;i++)
	   {
	     rd_buffer[i]=rd_24c01(IC_AD,i);//读回已经写入的数据
		   delay(250);
	   } 
  }
}

void delay(int n)
{
   int i;
	 for(i=1;i<=n;)
	  {
			i++;
		}
}

/*IC_card 子程序*/
void wr_24c01(unsigned char y,char a, char b)  //写IC卡函数：y是器件地址，a是字节地址，b为数据
{
	 _nop_();
	 start();
	 wr_byte(y);          //发送器件地址
	 ack();						    //接受应答信号
	 wr_byte(a);					//发送要访问的字节地址
	 ack();						    //接受应答信号
	 wr_byte(b);					//发送要写入的数据
	 ack();						    //接受应答信号
	 stop();
	 _nop_();
}

unsigned char rd_24c01(unsigned char y,char a)  //读IC卡函数：y是器件地址，a是字节地址
{
   data unsigned char x;
	 x=y|0x01;              //读命令中从器件地址，并置R/W位"1"
	
	 start();					      //发送启动命令
	 wr_byte(y);			      //发送器件地址
	 ack();						      //接受应答信号
	 wr_byte(a);					  //发送要访问的字节地址
	 ack();						      //接受应答信号
	
	 start();					      //重发启动命令
	 wr_byte(x);			      //主器件重发从器件地址并置R/W位"1"
	 ack();						      //接受应答信号
   rd_byte();
	 stop();		            //产生停止信号
	 return(rd_data);			  //返回读取的数据
}

/*IC_card 底层程序*/
void start()           //启动读写时序
{
	 SCL_IC_CARD=0;       //每次读写IC卡之前，确定CLK信号为低；
  
	/*模拟start信号*/
   SDA_IC_CARD=1;
	 SCL_IC_CARD=1;
	 SDA_IC_CARD=0;
	 SCL_IC_CARD=0;
}

void stop()						 //停止操作
{
	 SDA_IC_CARD=0;
	 SCL_IC_CARD=1;
	 SDA_IC_CARD=1;
	 SCL_IC_CARD=0;
}

void ack()						 //应答函数
{
   SCL_IC_CARD=1;
	 SCL_IC_CARD=0;
}

void wr_byte(char a)    //I2C总线发送字节
{
   data unsigned char i;
	 com_data=a;
	 for(i=0;i<8;i++)
	   {
	    SDA_IC_CARD=mos_bit;
	    SCL_IC_CARD=1;
	    SCL_IC_CARD=0;   
	    com_data=com_data<<1;
	   }
}

void rd_byte(void)
{  
	 data unsigned char i;
	 SDA_IC_CARD=1;       //IO引脚置为1，设置为输入状态
	 for(i=0;i<8;i++)			//循环8次读取从器件发来的1字节数据
    {
		rd_data=rd_data<<1;
		SCL_IC_CARD=1;
		rd_lbit=SDA_IC_CARD;
		SCL_IC_CARD=0;  			
		}
}