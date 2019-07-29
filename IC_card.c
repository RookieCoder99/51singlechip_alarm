#include <reg52.h>	                      
#include <stdio.h>							  
#include <absacc.h>							 
#include <intrins.h>

//����IC��оƬ�Ľӿ�
sbit SCL_IC_CARD=P1^3;	   //24c01����3��
sbit SDA_IC_CARD=P1^4;	   //24c01����6��

//��������
unsigned char data  wr_buffer[3]={0xf0,0x99,0x77};
unsigned char data  rd_buffer[3];
void delay(int n);

/*����IC���ӳ��򼰱���*/

//����IC���ı��� 
bdata char com_data;                          //���ñ��������ڿɰ�λѰַ���ڲ����ݴ洢����
sbit mos_bit=com_data^7;
sbit low_bit=com_data^0;

bdata char rd_data;                           //���ñ��������ڿɰ�λѰַ���ڲ����ݴ洢����
sbit rd_mbit=rd_data^7;
sbit rd_lbit=rd_data^0;

/*IC_card �ӳ���*/
unsigned char rd_24c01(unsigned char y,char a);  //yΪ������ַ��aΪ����
void wr_24c01(unsigned char y,char a,char b);    //yΪ������ַ��aΪ���ݣ�b

void start(void);                                //������дʱ���ӳ���
void stop(void);						                     //ֹͣ�����ӳ���
void ack(void);						                       //Ӧ�����ӳ���
void wr_byte(char a);                            //I2C���߷��������ӳ���
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
	     rd_buffer[i]=rd_24c01(IC_AD,i);//�����Ѿ�д�������
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

/*IC_card �ӳ���*/
void wr_24c01(unsigned char y,char a, char b)  //дIC��������y��������ַ��a���ֽڵ�ַ��bΪ����
{
	 _nop_();
	 start();
	 wr_byte(y);          //����������ַ
	 ack();						    //����Ӧ���ź�
	 wr_byte(a);					//����Ҫ���ʵ��ֽڵ�ַ
	 ack();						    //����Ӧ���ź�
	 wr_byte(b);					//����Ҫд�������
	 ack();						    //����Ӧ���ź�
	 stop();
	 _nop_();
}

unsigned char rd_24c01(unsigned char y,char a)  //��IC��������y��������ַ��a���ֽڵ�ַ
{
   data unsigned char x;
	 x=y|0x01;              //�������д�������ַ������R/Wλ"1"
	
	 start();					      //������������
	 wr_byte(y);			      //����������ַ
	 ack();						      //����Ӧ���ź�
	 wr_byte(a);					  //����Ҫ���ʵ��ֽڵ�ַ
	 ack();						      //����Ӧ���ź�
	
	 start();					      //�ط���������
	 wr_byte(x);			      //�������ط���������ַ����R/Wλ"1"
	 ack();						      //����Ӧ���ź�
   rd_byte();
	 stop();		            //����ֹͣ�ź�
	 return(rd_data);			  //���ض�ȡ������
}

/*IC_card �ײ����*/
void start()           //������дʱ��
{
	 SCL_IC_CARD=0;       //ÿ�ζ�дIC��֮ǰ��ȷ��CLK�ź�Ϊ�ͣ�
  
	/*ģ��start�ź�*/
   SDA_IC_CARD=1;
	 SCL_IC_CARD=1;
	 SDA_IC_CARD=0;
	 SCL_IC_CARD=0;
}

void stop()						 //ֹͣ����
{
	 SDA_IC_CARD=0;
	 SCL_IC_CARD=1;
	 SDA_IC_CARD=1;
	 SCL_IC_CARD=0;
}

void ack()						 //Ӧ����
{
   SCL_IC_CARD=1;
	 SCL_IC_CARD=0;
}

void wr_byte(char a)    //I2C���߷����ֽ�
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
	 SDA_IC_CARD=1;       //IO������Ϊ1������Ϊ����״̬
	 for(i=0;i<8;i++)			//ѭ��8�ζ�ȡ������������1�ֽ�����
    {
		rd_data=rd_data<<1;
		SCL_IC_CARD=1;
		rd_lbit=SDA_IC_CARD;
		SCL_IC_CARD=0;  			
		}
}