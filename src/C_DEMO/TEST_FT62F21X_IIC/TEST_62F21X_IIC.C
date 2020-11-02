//********************************************************* 
/*  �ļ�����Test_62F21X_IIC.c
*	���ܣ�  Test_62F21X_IIC������ʾ
*   IC:    FT62F21X SOP8
*   ����  16M/4T                    
*   ˵����  ����ʾ����λ62F21X_IIC����ʾ����.
*		   		�ó����ȡ(24C02)0x12��ַ��ֵ,ȡ�������0x13��ַ 
*                  							FT62F21X SOP8 
*                 								----------------
*  DemoPortOut ------------|1(PA4)    		 	(PA3)8 |-------------NC 
*  NC------------|2(TKCAP)   	 					(PA0)7 |-------------NC
*  NC------------|3(VDD)								(PA1)6 |-------------NC 
*  NC------------|4(VSS)   	 						(PA2)5	|-------------DemoPortIn
*			     								 ----------------
*/
//===========================================================
//===========================================================
#include	"SYSCFG.h";
#include 	"FT62F21X.h";

#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

#define  IIC_SCL		RA4 
#define  IIC_SDA		RA2

#define SDA_OUT  TRISA2 =0
#define SDA_IN	 TRISA2 =1

//===========================================================
//
//	ϵͳʱ��
//===========================================================
#define OSC_16M  0X70
#define OSC_8M   0X60
#define OSC_4M   0X50
#define OSC_2M   0X40
#define OSC_1M   0X30
#define OSC_500K 0X20
#define OSC_250K 0X10
#define OSC_32K  0X00
#define OSC_32K  0X00

//===========================================================
//��������
//===========================================================
unchar IICReadData;

/*-------------------------------------------------
 * ������: nterrupt ISR 
 *	���ܣ�  �жϷ�����
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void interrupt ISR(void)
{
    
}

/*-------------------------------------------------
 * �������ƣ�DelayUs
 * ���ܣ�   ����ʱ���� --16M-4T--��ſ�1%����.
 * ���������Time��ʱʱ�䳤�� ��ʱʱ��Time*2Us
 * ���ز������� 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
} 

/*------------------------------------------------- 
 * �������ƣ�DelayMs
 * ���ܣ�   ����ʱ����
 * ���������Time��ʱʱ�䳤�� ��ʱʱ��Time ms
 * ���ز������� 
 -------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(98); //��1%
		}
	}
}

/*------------------------------------------------- 
 * �������ƣ�DelayS
 * ���ܣ�   ����ʱ����
 * ���������Time ��ʱʱ�䳤�� ��ʱʱ��Time S
 * ���ز������� 
 -------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 
		}
	}
}

/*-------------------------------------------------
 * ��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = OSC_16M;	//	bit7	Timer2 ѡ��LIRCΪʱ��Դʱ LIRC��Ƶ��ѡ��  0:32KHz	1:256KHz
										//bit[6:4]	ϵͳƵ��ѡ��	
										//bit[2]		�����ڲ�ʱ��״̬	1:ready	0:not ready
										//bit[1]		�����ڲ�ʱ��״̬	1:ready	0:not ready

	INTCON = 0;  				//�ݽ�ֹ�����ж�
	OPTION = 0;
    TRISA	= 0;					//1:����	0:���		
    PSRCA 	= 0;					//00:	4mA		01/10:	8mA		11:	28mA	bit[3:2]����PA5Դ����	bit[1:0]����PA4Դ����
    PSINKA	= 0;					//bit[1:0]	����PA5��PA4		0:�������С	1:��������
    PORTA 	= 0;					//1:PAx����ߵ�ƽ	0:PAx����͵�ƽ
 	WPUA 	= 0;					//1:	ʹ��PA������	0:�ر�PA������   
}

/*-------------------------------------------------
 *  ��������IIC_Start
 *	���ܣ�  ����IIC��ʼ�ź�
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void IIC_Start(void)
{
	SDA_OUT;
 	IIC_SDA=1;	  	  
	IIC_SCL=1;
	DelayUs(10);
  	IIC_SDA=0;        //START:when CLK is high,DATA change form high to low 
	DelayUs(10);
	IIC_SCL=0;        //ǯסI2C���ߣ�׼�����ͻ�������� 
    DelayUs(10);          
}

/*-------------------------------------------------
 *  ��������IIC_Stop
 *	���ܣ�  ����IICֹͣ�ź�
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
 void IIC_Stop(void)
{
	SDA_OUT;          //SDA�����
	IIC_SCL=0;
	IIC_SDA=0;        //STOP:when CLK is high DATA change form low to high
 	DelayUs(10);
	IIC_SCL=1; 
    DelayUs(10);
	IIC_SDA=1;        //����I2C���߽����ź�
	DelayUs(10);							   	
}

/*-------------------------------------------------
 *  ��������IIC_Wait_Ack
 *	���ܣ�  �ȴ�Ӧ���źŵ���
 *  ���룺  ��
 *  �����  ����ֵ��1������Ӧ��ʧ��
 *               0������Ӧ��ɹ�
 --------------------------------------------------*/
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;       
	IIC_SDA=1;
 	SDA_IN;               //SDA����Ϊ���� 
	DelayUs(5);	   
	IIC_SCL=1;
	DelayUs(5);	 
	while(IIC_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250) //�ȴ���ʱ
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;            //ʱ�����0 	   
	return 0;  
} 
/*-------------------------------------------------
 * ��������IIC_Ack
 *	���ܣ�  ����ACKӦ��
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA�����
	IIC_SDA=0;
	DelayUs(5);	
	IIC_SCL=1;
	DelayUs(5);	
	IIC_SCL=0;
}

/*-------------------------------------------------
 *  ��������IIC_NAck
 *	���ܣ�  ������ACKӦ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA�����
	IIC_SDA=1;
	DelayUs(5);	
	IIC_SCL=1;
	DelayUs(5);	
	IIC_SCL=0;
}	

/*-------------------------------------------------
 *  ��������IIC_Send_Byte
 *	���ܣ�  IIC����һ���ֽ�
 *  ���룺  д��Ҫ���͵�һ���ֽ�����txd
 *  �����  ��
 --------------------------------------------------*/		  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT;	          //SDA�����   
    IIC_SCL=0;            //����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		if(txd&0x80)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		DelayUs(5);				  
		IIC_SCL=1;
		DelayUs(5);	
		IIC_SCL=0;	
		DelayUs(5);
    }	 
} 	 

/*-------------------------------------------------
 *  ��������IIC_Read_Byte
 *	���ܣ�  IIC��һ���ֽ�
 *  ���룺  ��
 *  �����  �����洢����������ݲ�����receive
 --------------------------------------------------*/
 unsigned char IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_IN;               //SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        DelayUs(5);	
     	IIC_SCL=1;
        receive<<=1;
        if(IIC_SDA)receive++;   
		DelayUs(5);	
    }					 
    IIC_NAck();           //����nACK
  
    return receive;
}

/*-------------------------------------------------
 *  ��������IIC_READ
 *	���ܣ�  IIC�����ƶ�λ�õ�����
 *  ���룺  address
 *  �����  ����address�洢�����������iicdata
 --------------------------------------------------*/
 unsigned char IIC_READ(unsigned char address)
{
	unsigned char iicdata = 0;
	IIC_READ_Begin:
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		IIC_Send_Byte(address);				    //��Ҫ�������ݵ�ַ
		if(IIC_Wait_Ack())goto IIC_READ_Begin; 
		IIC_Start();
		IIC_Send_Byte(0xa1);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		iicdata=IIC_Read_Byte();
		IIC_Stop();		
		return iicdata;
}
 /*-------------------------------------------------
 *  ��������IIC_WRITE
 *	���ܣ�  IIC������dataд���ƶ���λ��address
 *  ���룺  address��data
 *  �����  ��
 --------------------------------------------------*/
void IIC_WRITE(unsigned char address,unsigned char data)
{
	IIC_WRITE_Begin:
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(address);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(data);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Stop();	
}
/*-------------------------------------------------
 *  ������: main 
 *	���ܣ�  ������
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void main()
{
 	POWER_INITIAL();				//ϵͳ��ʼ��
    
	IICReadData = IIC_READ(0x12); 		//��ȡ0x12��ַEEPROMֵ 
	IIC_WRITE(0x13,~IICReadData); 		//ȡ��д���ַ0x13
	while(1)
	{
    	NOP();
	}	      
}
//===========================================================