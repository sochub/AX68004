//********************************************************* 
/*	文件名：TEST_FT62F21x_EEPROM.c
*	功能：	 FT62F21x-EEPROM功能演示
*	IC:		 FT62F211 SOP8
*	晶振：   16M/4T                    
*	说明：   该程序读取0x12地址的值,取反后存入0x13地址
*
*                  FT62F211 SOP8 
*                 ----------------
*  NC-----------|1(PA4)      (PA3)8|------------NC    
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
//#include 	"FT62F21X.h";
//*********************************************************
#define OSC_16M		0X70
#define OSC_8M		0X60
#define OSC_4M		0X50
#define OSC_2M		0X40
#define OSC_1M		0X30
#define OSC_500K	0X20
#define OSC_250K	0X10
#define OSC_32K		0X00

#define WDT_256K	0X80
#define WDT_32K		0X00
//**********************************************************
//***********************宏定义*****************************
#define  unchar     unsigned char 
#define  unint      	unsigned int
#define  unlong     unsigned long  
 
unchar EEReadData;
 /*-------------------------------------------------
 *	函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	OSCCON = WDT_32K|OSC_16M|0X00;	
  //OSCCON = 0B01110000;		//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//暂禁止所有中断

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PA输入输出 0-输出 1-输入  
	WPUA = 0B00000000;     		//PA端口上拉控制 1-开上拉 0-关上拉

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE�
													//Bit7(PAPU)=0 由WPUA决定是否上拉
	MSCON = 0B00000000;		                             
}
/*-------------------------------------------------
 *	函数名：EEPROMread
 *	功能：  读EEPROM数据
 *	输入：  需要读取数据的地址 EEAddr
 *	输出：  对应地址读出的数据 ReEEPROMread
 --------------------------------------------------*/
unchar EEPROMread(unchar EEAddr)
{
	unchar ReEEPROMread;
    EEADR = EEAddr;
    RD=1;
    NOP();
    NOP();
    NOP();
    NOP();
    ReEEPROMread =EEDAT;
    return ReEEPROMread;
}
/*-------------------------------------------------
 *	函数名：EEPROMwrite
 *	功能：  写数据到EEPROM
 *	输入：  需要读取数据的地址 EEAddr
				需要写入的数据		  Data
 *	输出：  无
 --------------------------------------------------*/
void EEPROMwrite(unchar EEAddr,unchar Data)
{
	
	while(GIE)					//等待GIE为0
		{GIE = 0;	}						//写数据必须关闭中断
	EEADR = EEAddr; 	 			//EEPROM的地址
	EEDAT = Data;		 			//EEPROM的写数据  EEDATA = Data;
	EEIF = 0;
	EECON1 |= 0x34;				//置位WREN1,WREN2,WREN3三个变量.
	WR = 1;							//置位WR启动编�
    NOP();
    NOP();
    NOP();
    NOP();
	while(WR);      				//等待EE写入完成
	GIE = 1;
}
/*-------------------------------------------------
 *	函数名: main 
 *	功能：  主函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();								//系统初始化
 	EEPROMwrite(0x55,0xaa);	
	EEPROMwrite(0x55,0xaa);					//在未使用到的随意一个地址写两次0xAA
 
	EEReadData = EEPROMread(0x12); 		//读取0x12地址EEPROM值 
	EEPROMwrite(0x13,~EEReadData); 		//取反写入地址0x13
    
	while(1) 
	{
 
		NOP();
	 
	}
}
