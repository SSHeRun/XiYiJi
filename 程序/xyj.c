/***************************ͷ�ļ�***********************/ 
#include <reg52.h> 
#include <intrins.h> 
/************************�������ͺ궨��******************/ 
#define uchar unsigned char 
#define uint unsigned int 
/***********************ʹ�ܡ���ֹ�궨��*****************/ 
#define Enable(); EA=1; 
#define Disable();EA=0; 
/**********************������ƺ궨��********************/ 
#define ZHENGZHUAN 0 
#define STOP 1 
#define FANZHUAN 2 
#define ZhengZhuan(); PIN_Motor_A = 0; PIN_Motor_B = 1; 
#define Stop(); PIN_Motor_A = 1; PIN_Motor_B = 1; 
#define FanZhuan(); PIN_Motor_A = 1; PIN_Motor_B = 0; 
/*******************��ͣ���������ƺ궨��*****************/ 
#define ZanTing(); ET0 = 0; ET1 = 0; Stop(); 
#define JiXu(); ET0 = 1; ET1 = 1; 
#define flag_Ok  (flag_XiDi||flag_TuoShui) 
/*************************�ܽŶ���***********************/ 
#define lcd P0     //Һ�����ݿ�

sbit PIN_JinShui = P2^2; //��ˮ
sbit PIN_PaiShui = P2^3; //��ˮ
sbit PIN_Motor_A = P2^4; //�����A 
sbit PIN_Motor_B = P2^5; //�����B 
sbit K_SEL_ChengXu = P1^5; //ѡ������ 
sbit qiangruo = P1^6; //ˮ��ѡ�� 
sbit shuiwei1 = P3^6; // ˮλ���
sbit p32 = P3^2; //   ����/��ͣ
sbit rs=P1^1;	   //Һ����������
sbit en=P1^2;
sbit SPK = P2^0; //��������
/*************************���ݶ���***********************/ 
uchar flag_SEL_ChengXu; //Ĭ��Ϊ��׼���� 
 
bit flag_Run; //���б�־��1Ϊ���� 
bit flag_XiDi; //��ϴ�ӱ�־ 
bit flag_TuoShui; //��ˮ��־
bit flag_PiaoXi;  //Ưϴ��־  
bit flag_SEl_QiangRuo; //Ĭ��Ϊǿϴ��1Ϊ��ϴ 

uchar _50ms; //ÿ50ms��һ�εı��� 
uint s,s1; //�� 
uchar k;
int counter1=0,counter2=0,counter3=0;  //���ת�ٿ���
uint T_S; //��ʱ��ʱ��  
int fen,miao;//ʣ��ʱ��
uint t1,t2,t3;	   //ϴ�ӡ�Ưϴ��˦��ʱ��
uchar a=15;  //ռ�ձ�
uchar mol=0;  //�ֶ�ģʽѡ��
uint count;
/**************************���鶨��********************/
uchar code table0[]="State:          ";

uchar table7[]="00:00";


/*************************��ʱ����*********************/ 
void Delay_10ms(uint T1) 
{ 
	uint t1,t2; 
	for(t1=0;t1<T1;t1++) 
	for(t2=0;t2<1250;t2++);//10ms 
} 


void delay(uint count)		 //1ms��ʱ
{
   uint x,y;
   for(x=0;x<count;x++)
   for(y=0;y<120;y++);
}
/*************************Һ����������*************************/
void w_cmd(uchar com)		  //lcd1602д����
{
  rs=0;
  lcd=com;
  en=1;
  delay(5);
  en=0;
}

void w_data(uchar dat)		  //lcd1602������
{
  rs=1;
  lcd=dat;
  en=1;
  delay(5);
  en=0;
}

void w_str(uchar *s)       //lcd1602д�ַ���
{
    while(*s)  w_data(*s++);
}

void lcd_int()		     //lcd1602��ʼ��
{
  en=0;
  w_cmd(0x38);
  w_cmd(0x0c);
  w_cmd(0x06);
  w_cmd(0x01);
  w_cmd(0x80+0x00);
  w_str(table0);
  //w_cmd(0xc0+0x00);
 // w_str(table1);
}


/********************�жϳ�ʼ������********************/ 
void ExInt_Init(void) 
{ 
	IT0 = 1; 
	EX0 = 1; 
	IT1 = 1; 
	EX1 = 1; 
} 
/*******************��ʱ��0��ʼ������******************/ 
void Timer0_Init(void) //����ûET0��Ϊ�Ժ������ͣ�õ� 
{ 
	TMOD = TMOD | 0X01; //��ʱ��ʽ1 
	TH0 = 0X3C; //50ms 
	TL0 = 0XB0; 
	TR0 = 1; 
	_50ms = 0; 
	s = 0; 
} 
/*******************��ʱ��1��ʼ������******************/ 
void Timer1_Init(void) // 
{ 
	TMOD=TMOD|0x10;
	
	TH1 = ( 65535 - 500 ) / 256;   //1ms 
	TL1 = ( 65535 - 500 ) % 256; 
	//ET1=1;	
	//EA=1;
	TR1=1;	

	
}


/*******�ⲿ�ж�0����Ϊ���л�����ͣ***********/ 
void int0(void) interrupt 0 
{ 
	 if(!p32) 
	 { 
	   Delay_10ms(1); //��ʱ10ms���ң�ȥ���� 
	   if(!p32) 
	   { 
		 	if(!flag_Run)
			{ 
		     	flag_Run = 1; //�����б�־
				
			} 
		    
		    else if(flag_Ok) 
		    { 
			      static bit flag_ZanTing; 
			      flag_ZanTing = ~flag_ZanTing; 
			      if(flag_ZanTing) 
			      {
				  	ZanTing();
				   }
					else 
					{JiXu();} 
		    } 
	    } 
	  }
 } 

/*******************��ʱ��0�жϳ���********************/ 
void Timer0(void) interrupt 1 
{ 
	 TR0 = 0; //ֹͣ���� 
	 TH0 = 0X3C; //��װ��ʱ��ֵ 
	 TL0 = 0XB0; 
	 TR0 = 1; 
	 _50ms++; 
	 if(_50ms == 20) //1s�� 
	 { 
		   _50ms = 0; 
		   
		  s++;
		  if(s == T_S) //��ʱ�� 
		  {
			 s = 0; 			 
   			 flag_XiDi = 0; //��ϴ�ӱ�־ 
 			 flag_TuoShui= 0; //����ˮ��־
			 flag_PiaoXi=0; //��Ưϴ��־
			 Stop();
			 ET1=0;
		  } 
		  else
		  { 
			  	ET1=1; //������Ƴ��� 
				
			   
			   
			   if(miao==0)
			   {
			   		miao=60;
					fen--;
					if(fen==0)
						fen=0;
			   }
			   miao--;
			   
		  }
		
	 } 
} 

/*******************��ʱ��1�жϳ���********************/ 
void Timer1 ( void ) interrupt 3  //��ʱ��1�жϺ��� 0.5ms 
{ 
	TH1 = ( 65535 - 500) / 256;  
	TL1 = ( 65535 - 500 ) % 256;
	if(flag_XiDi||flag_PiaoXi)		 
	{
			count++;
			if(count<16000)
			{
				counter1++;	  //d������Ƽ���
				if(counter1<=a)
				{
					ZhengZhuan();		 				
				}
				else if(counter1>a)
				{
					Stop();
				}
				if(counter1==100) counter1=0;
			}
			else if(count<20000)
			{
				 Stop();
			}
			else if(count<36000)
			{
				counter3++;	  //d������Ƽ���
				if(counter3<=a)
				{
					FanZhuan();		 				
				}
				else if(counter3>a)
				{
					Stop();
				}
				if(counter3==100) counter3=0;
			}
			else
			{
				count=0;
			}
	
	}
	 

	if(flag_TuoShui)
	{
		counter2++;	  //d������Ƽ���
		if(counter2<=40)
		{
			ZhengZhuan();		 				
		}
		else if(counter2>40)
		{
			Stop();
		}
		if(counter2==100) counter2=0;
	}
}


/******************��ˮ��ˮ**********************/
void JinShui()
{
	PIN_JinShui = 0; //��ˮ���� 
    PIN_PaiShui = 1;
}
void PaiShui()
{
	PIN_JinShui = 1; //paiˮ���� 
    PIN_PaiShui = 0;
}
/**********************ϴ�ӳ���***********************/ 
void XiDi(void) 
{ 

	 	 	
		JinShui();	   //��ˮ
		while(shuiwei1)
		{	 w_cmd(0x80+6);	
	        w_str(" JinShui"); 
		}									
		 PIN_JinShui = 1; //�ؽ�ˮ����
		 	 w_cmd(0x80+6);	
	      w_str("  XiDi    "); 
		 // Timer0_Init(); 
		 ET0=1;
		  T_S =t1;
	     fen=t1/60;miao=t1%60; 
	     
		 flag_XiDi=1;
		  while(flag_XiDi)
		 {	 
			 table7[0]=fen/10+0x30;
			 table7[1]=fen%10+0x30;
			 table7[3]=miao/10+0x30;
			 table7[4]=miao%10+0x30;
			 w_cmd(0xc0+7);	
			 w_str(table7);
			 
		}
		 
		ET0=0;
		w_cmd(0xc0+7);	
		 w_str("00:00");
		 
			PaiShui();		//��ˮ
			while(!shuiwei1)
			{
				w_cmd(0x80+7);	
        	    w_str(" PaiShui");
			}
		
		 PIN_PaiShui = 1; //����ˮ����	 

	
} 
/*****************Ưϴ***************************/
void PiaoXi()
{
	
		 	  
	    JinShui();	   //��ˮ
		while(shuiwei1)
		{	 w_cmd(0x80+6);	
	        w_str(" JinShui"); 
		}		
		PIN_JinShui = 1; //�ؽ�ˮ����
			w_cmd(0x80+7);	
        	w_str(" PiaoXi");	
		 ET0=1;
		 T_S =t2;
		 fen=t2/60;miao=t2%60; 	     
		 flag_PiaoXi=1;
		 
		  while(flag_PiaoXi)
		 {	 
			 table7[0]=fen/10+0x30;		//��ʾʣ��ʱ��
			 table7[1]=fen%10+0x30;
			 table7[3]=miao/10+0x30;
			 table7[4]=miao%10+0x30;
			 w_cmd(0xc0+7);	
			 w_str(table7);			 
		 }
		 ET0=0;
		 w_cmd(0xc0+7);	
		 w_str("00:00");
		 	PaiShui();		//��ˮ
			while(!shuiwei1)
			{
				w_cmd(0x80+7);	
        	    w_str(" PaiShui");
			}

		 PIN_PaiShui = 1; //����ˮ����		
	
}
/*****************˦��****************************/
void TuoShui()
{
	w_cmd(0x80+7);	
	w_str("TuoShui");
	PIN_JinShui = 1; //paiˮ���� 
    PIN_PaiShui = 0;
	flag_TuoShui=1;
    ET0=1;
	T_S =t3;
	fen=t3/60;miao=t3%60; 	     
	
	while(flag_TuoShui)
	{	 
		table7[0]=fen/10+0x30;		//��ʾʣ��ʱ��
		table7[1]=fen%10+0x30;
		table7[3]=miao/10+0x30;
	    table7[4]=miao%10+0x30;
		w_cmd(0xc0+7);	
		 w_str(table7);			 
	}
	//PIN_PaiShui = 1;
} 
/**********************��������************************/ 
void BaoJing(void) 
{  
	uint i, j;
	for (i = 0; i < 200; i++)
	{
         SPK = 0; for (j = 0; j < 100; j++);
         SPK = 1; for (j = 0; j < 100; j++);
    }	 
}
/**********************��������ѡ��*******************/
void keyscan()
{
	
	if(K_SEL_ChengXu==0)		 //ģʽѡ��  
	{
		delay(10);		
		if(K_SEL_ChengXu==0)
		{
			while(!K_SEL_ChengXu);
		
			mol++;
			if(mol>1) mol=0;
		}
	}
	if(qiangruo==0)		 //ģʽѡ��  
	{
		delay(10);		
		if(qiangruo==0)
		{
			while(!qiangruo);
			flag_SEl_QiangRuo=~flag_SEl_QiangRuo;
			if(flag_SEl_QiangRuo) a=10;
			if(!flag_SEl_QiangRuo) a=20;			
		}
	} 	
}
/************************������************************/ 
void main() 
{ 
	
	uchar i; 
	ExInt_Init(); //���жϳ�ʼ�� 
	lcd_int();
	Timer0_Init();
	Timer1_Init(); 
//	ET0 = 1; 
	Enable(); //�����ж� 
	while(1) 
	 {
		  
		
					 w_cmd(0x80+0x07);
	  			  	w_str("xyj");
				 
		 }
		   
	 
}
