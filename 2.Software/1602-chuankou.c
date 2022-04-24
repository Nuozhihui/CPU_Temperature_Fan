/*********************************************************************************
* 【编写时间】： 2014年3月5日
* 【作    者】： 清翔电子:03
* 【版    本】： 1.0
* 【网    站】： http://www.qxmcu.com/ 
* 【淘宝店铺】： http://qxmcu.taobao.com/ 
* 【实验平台】： QX-MCS51 单片机开发板
* 【外部晶振】： 11.0592mhz	
* 【主控芯片】： STC89C52
* 【编译环境】： Keil μVisio3	
* 【程序功能】： 			   			            			    
* 【使用说明】： 液晶1脚接1602黑色排母的1脚  如果是无背光的液晶只有14个脚 排母右边空2个位   
                 请把液晶对比度电位器顺时针调制10点钟方向，请勿多调超出极限位置会损坏电位器！
*  说明：免费开源，不提供源代码分析.
**********************************************************************************/
 
/************************************************************************
* 描述：（用串口调试助手软件观察）                                      
*   主机发送0123456789abcdef给单片机，单片机接收到数据后发送QX-MCU789abcdef给主机。  	
*   LCD1602上显示	   0123456789abcdef 
*   LCD1602显示接收数据的ASCII码。  波特率9600    发送和接收都要选择为文本格式      		        
*   注：当单片机收到数据只有凑够16个时才会一次在液晶上显示出来		    
************************************************************************/

#include <reg52.h>
#include <intrins.h>
	
#define uchar  unsigned char 
#define uint   unsigned int

sbit LCD_RS = P3^5;
sbit LCE_RW = P3^6;             
sbit LCD_EN = P3^4;

unsigned char pwm_motor_val = 254;//左右电机占空比值 取值范围0-170，0最快
unsigned char pwm_t;//周期

sbit _4IN = P1^0;//
sbit BIN = P1^1;//
sbit key_s2 = P3^2;
sbit key_s3 = P3^3;

#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};





uchar data  RXDdata[ ] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                          0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20 };
uchar temp,buf,m,count;

bit   playflag=0;

uchar code  cdis1[ ] = {" SERILA TRANFER "};
uchar code  cdis2[ ] = {"                "};

void delay(unsigned int z)//毫秒级延时
{
	unsigned int x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}
/**********************************************************

 延时子程序

**********************************************************/
void delay1(uint ms) 

{
   uchar k;
   while(ms--)
   {
     for(k = 0; k < 120; k++);
   }
}

/*******************************************************************/
/*                                                                 */
/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。                             */
/*                                                                 */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{                          
 //  while(lcd_busy());
    LCD_RS = 0;
  //  LCD_RW = 0;
    LCD_EN = 0;
    _nop_();
    _nop_(); 
    P0 = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;  
}

/*******************************************************************/
/*                                                                 */
/*写显示数据到LCD                                                  */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。                               */
/*                                                                 */
/*******************************************************************/
void lcd_wdat(uchar dat)
{                          
//   while(lcd_busy());
    LCD_RS = 1;
  //  LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
}

/*******************************************************************/
/*                                                                 */
/*  LCD初始化设定                                                  */
/*                                                                 */
/*******************************************************************/
void lcd_init()
{ 
     LCE_RW = 0; 
    delay1(15);   
    lcd_wcmd(0x01);      //清除LCD的显示内容            
    lcd_wcmd(0x38);      //16*2显示，5*7点阵，8位数据
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);

    lcd_wcmd(0x0c);      //开显示，显示光标，光标闪烁
    delay1(5);

    lcd_wcmd(0x01);      //清除LCD的显示内容
    delay1(5);
}

/*******************************************************************/
/*                                                                 */
/*  设定显示位置                                                   */
/*                                                                 */
/*******************************************************************/

void lcd_pos(uchar pos)
{                          
  lcd_wcmd(pos | 0x80);  //数据指针=80+地址变量
}


/*********************************************************

  串行中断服务函数

*********************************************************/
void  serial() interrupt 4 
{
   ES = 0;                //关闭串行中断
   RI = 0;                //清除串行接受标志位
   buf = SBUF;            //从串口缓冲区取得数据
     
//   switch(buf)
//   {
//      case 0x01:  senddata('1');wen=1;break;  //接受到1，发送字符'Q'给计算机         
//      case 0x02:  senddata('2');wen=2;break;  //接受到2，发送字符'X'给计算机       
//      case 0x03:  senddata('3');wen=3;break;  //接受到3，发送字符'-'给计算机       
//      case 0x04:  senddata('4');wen=4;break;  //接受到4，发送字符'M'给计算机       
//      case 0x05:  senddata('5');wen=5;break;  //接受到5，发送字符'C'给计算机           
//      case 0x06:  senddata('6');wen=6;break;  //接受到5，发送字符'U'给计算机
//      case 0x07:  senddata('7');wen=7;break;  //接受到1，发送字符'Q'给计算机         
//      case 0x08:  senddata('8');wen=8;break;  //接受到2，发送字符'X'给计算机       
//      case 0x09:  senddata('9');wen=9;break;  //接受到3，发送字符'-'给计算机       
//      case 0x00:  senddata('0');wen=0;break;  //接受到4，发送字符'M'给计算机        		 
//      default:    senddata(buf);wen=0;break;  //接受到其它数据，将其发送给计算机         
//   }
//	 if(wen_wei==0)
//	 {
//		 
//		 z_wen=wen*10;
//		 wen_wei=1;
//		 
//	 }else{
//		 //30  0   90 255
//		pwm_motor_val=-3.22*(wen+z_wen)+190.6;		
//		  wen_wei=0;
//	 }


   if(buf!=0x0D)
   {   
     if(buf!=0x0A)
     { 
       temp =buf;
       if(count<2)
       {         
         RXDdata[count]=temp;
         count++;
		 if(count==2)
		 	playflag=1;
       }
     } 
   }
   ES = 1;    //允许串口中断
}
int wen=0;
int wen_1=0;
int i=0;
/*********************************************************

  数据显示函数

*********************************************************/
void  play()
{
   if(playflag)
   { 
     lcd_pos(0x40);           //设置位置为第二行
     for(m=0;m<2;m++) 
     {
	 	lcd_wdat(cdis2[m]);      //清LCD1602第二行
		delay1(5);
	 }
	 lcd_pos(0x40);           //设置位置为第二行
     for(m=0;m<2;m++)
     {
			 wen=(int)RXDdata[m];
			 
			 if(m==0)
			 {
				 wen_1=wen*10;

			 }else if(m==1){
				 pwm_motor_val=(int)(255-2.4*(wen_1+wen));
			
			 }
    //   lcd_pos(0x40+m);       //设置显示位置为第二行
       lcd_wdat(wen);  //显示字符 
	   delay1(5);
     }

     playflag=0;              
     count=0x00;

     for(m=0;m<2;m++)      
     {
	 	RXDdata[m]=0x20;         //清显存单元    
		delay1(5);
	 }
   }               
}

/*********************************************************

  主函数

*********************************************************/
void main(void) 
{
    P0 = 0xff;
    P2 = 0xff;         
    
    SCON=0x50;           //设定串口工作方式
    PCON=0x00;           //波特率不倍增
			
    TMOD=0x20;           //定时器1工作于8位自动重载模式, 用于产生波特率
    EA=1;
    ES = 1;              //允许串口中断
    TL1=0xfd;
    TH1=0xfd;             //波特率9600
    TR1=1;
	
	//PWM
		TMOD |= 0x02;//8位自动重装模块
		TH0 = 220;
		TL0 = 220;//11.0592M晶振下占空比最大比值是256,输出100HZ
		TR0 = 1;//启动定时器0
		ET0 = 1;//允许定时器0中断
		EA	= 1;//总中断允许
	
	
    lcd_init();
    lcd_pos(0x00);        //设置显示位置为第一行
    for(m=0;m<16;m++) 
    {
		lcd_wdat(cdis1[m]);   //显示字符
		delay1(5);
	}
    lcd_pos(0x40);        //设置显示位置为第二行
    for(m=0;m<16;m++) 
    {
		lcd_wdat(cdis2[m]);   //显示字符            
 		delay1(5);
	}
		BIN=0;
    while(1)        
    {
      play();     
			
		if(key_s2 == 0)	 //S2按下LED变暗
		{
			delay(5);
			if(key_s2 == 0)
			{
				if(pwm_motor_val < 254)
				{
					pwm_motor_val++;
				}
			}
		}
		if(key_s3 == 0)	//S3按键LED变亮
		{
			delay(5);
			if(key_s3 == 0)
			{
				if(pwm_motor_val > 0)
				{
					pwm_motor_val--;
				}
			}
		}			
    }
}

//定时器0中断
void timer0() interrupt 1
{
	pwm_t++;
	if(pwm_t == 255)
		_4IN = 0;
	if(pwm_motor_val == pwm_t)
		_4IN = 1;					 
}
/*********************************************************/ 
