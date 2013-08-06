#include <p32xxxx.h>
#include <plib.h>
#include"lab6.h"

uchar table[][3]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
uchar year[4],day[2],month[2],date;
uchar Second[2], Minute[2], Hour[2];
uchar small_mon,leap_year;
uchar CurrentTime[9], CurrentDate[11];
int wakeup =0;
int setalarm = 0;

/*******************DATE&TIME_INIT*********/
void Date_init() //日期初始化
{
  date = 6;
	day[0] = 0;
	day[1] = 7;
	month[0] = 0;
	month[1] = 7;
	year[0] = 2;
	year[1] = 0;
	year[2] = 1;
	year[3] = 3;
}

void Time_init() //时间初始化
{
	Second[1] = '0';
	Second[0] = '3';
	Minute[1] = '0';
	Minute[0] = '3';
	Hour[1] = '2';
	Hour[0] = '1';
}
/*********************END******************/

void state()	//leap year? small month?
{
	switch(month[1])
	{
		case 1: if(month[0] == 1) //若是11月,则是小月
		small_mon = 1;
		else //若是1月，则不是小月
		small_mon = 0;
		break;
		case 4:
		case 6:
		case 9: small_mon = 1;
		break;
		default:small_mon = 0;
		break;
	}
	if((year[0] * 1000 + year[1] * 100 + year[2] * 10 + year[3]) % 100 == 0)
	{
		if((year[0] * 1000 + year[1] * 100 + year[2] * 10 + year[3]) % 400 == 0)
		leap_year = 1;
	}
	else if((year[0] * 1000 + year[1] * 100 + year[2] * 10 + year[3]) % 4 == 0)
		leap_year = 1;
	else
	leap_year = 0;
}

void Dater() //日期函数
{
	state();        
	day[1]++;
	date=(date+1)%7;

	switch(day[1])
	{
		case 10:
		{
			if(day[0] == 2 && month[0] == 0 && month[1] == 2) //闰年时，把2月30号转变为3月1号
			{
				day[0] = 0;
				day[1] = 1;
				month[0]++;
			}
//日期改到这里
			else  //日期各位满10进1
			{
				day[1] = 0;
				day[0]++;
			}				
		}
		break;
		case 1:
		{
			if(day[0] == 3 && small_mon) //小月31号时把日期置为1号，月份+1
			{
				day[0] = 0;
				day[1] = 1;
				month[1]++;
			}
		}
		break;
		case 2:
		{
			if(day[0] == 3)
			{
				day[0] = 0;
				day[1] = 1;
				month[1]++;
			}
		}
		break;
		case 9:
		{
			if(day[0] == 2 && !leap_year && month[1] == 2 && month[0] == 0)  //平年时，把2月29号变为3月1号
			{
				day[0] = 0;
				day[1] = 1;
				month[1]++;
			}
		}
		break;
		default:break;
	}
	switch(month[1])
	{
		case 10:
		{
			month[1] = 0;
			month[0]++;
		}
		break;
		case 3:
		{
			if(month[0] == 1)
			{
				month[0] = 0;
				month[1] = 1;
				year[3]++;
			}
		}
		break;
	}
	if(year[3] > 9)
	{
		year[3] = 0;
		year[2]++;
		if(year[2] > 9)
		{
			year[2] = 0;
			year[1]++;
			if(year[1] > 9)
			{
				year[1] = 0;
				year[0]++;
				if(year[0] > 9)
				year[0] = 0;
			}
		}	
	}
}

void Timer_Config()
{
	T4CON = 0x0070;	//32bits
	IPC4bits.T4IP = 5;//Set priority level = 5
	IPC4bits.T4IS = 3;//Set subpriority level = 3
	IFS0bits.T4IF = 0;//Clear Timer3 interrupt flag
	IEC0bits.T4IE = 1;
	TMR4 = 0;
	PR4 = 15624;	//1s
	T4CONSET = 0x8000;
}

void Time_Display()
{
	LCD_goto(0x40);
	LCD_puts(CurrentTime);
}

void Date_Display()
{
	LCD_goto(0x0);
	LCD_puts(table[date]);
	sprintf(CurrentDate, "%d%d%d%d/%d%d/%d%d", year[0],year[1],year[2],year[3],month[0],month[1],day[0],day[1]);
	//CurrentDate[10]='/0';
	LCD_goto(0x06);
	LCD_puts(CurrentDate);
}

#pragma interrupt TMR4_ISR ipl5 vector 16 //timer5
void TMR4_ISR ()
{
	Second[1]++;
	if (Second[1] > '9')
	{
		Second[1] = '0';
		Second[0]++;
		if (Second[0] > '5')
		{
			Second[0]= '0';
			Minute[1]++;
			if (Minute[1] > '9')
			{
				Minute[1] = '0';
				Minute[0]++;
				if (Minute[0] > '5')
				{
					Minute[0] = '0';
					Hour[1]++;
					if (Hour[1] == '4' && Hour[0] == '2')
					{
						Hour[0] = Hour[1] = '0';
						//Dater();
					}
					else if (Hour[1] > '9')
					{
						Hour[1] = '0';
						Hour[0]++;
					}
				} 
			}
		}
	}

	//sprintf(CurrentTime, "%02d:%02d:%02d", Hour, Minute, Second);
	//CurrentTime[8] = '\0';
	CurrentTime[0]=Hour[0];
	CurrentTime[1]=Hour[1];
	CurrentTime[2]=':';
	CurrentTime[3]=Minute[0];
	CurrentTime[4]=Minute[1];
	CurrentTime[5]=':';
	CurrentTime[6]=Second[0];
	CurrentTime[7]=Second[1];
	CurrentTime[8] = '\0';
	Time_Display();
	Date_Display();
	IFS0bits.T4IF = 0;
}

int main()
{
	OSCSetPBDIV (OSC_PB_DIV_1);
	MCU_init();
	LCD_init();
	Time_init();
	Date_init();
	Timer_Config();
	Time_Display();
	if(setalarm==1)
	{
		RS = 0;
		LCD_goto(0x4F);
		LCD_puts("A");		//如果设定了闹钟，显示“A”
		RS = 1;
	}
	while(1);
}

/*************Display Part*****************/
void MCU_init() {
//configure PBDIV so PBCLK = SYSCLK
/* setup I/O ports to connect to the LCD module */
    
    TRISE=0x0000;//regard 0-15 pins of portc as the output to the LCD pins
    TRISD=0xFF00;
    INTCONbits.MVEC = 1; // Enable multiple vector interrupt
	asm ("ei"); // Enable all interrupts
	
	IFS0CLR = 0x00001000; // Clear timer interrupt flag
	IEC0SET = 0x00001000; // Enable Timer3 interrupt
}

void LCD_init() {
	DelayMsec(15);
	RS = 0;
	Data = LCD_IDLE;
	DelayMsec(5);
	Data = LCD_IDLE;
	DelayUsec(100);
	Data = LCD_IDLE;
	DelayMsec(5);
	Data = LCD_IDLE;
	DelayUsec(100);
	LCD_putchar(LCD_2_LINE_4_BITS);
	DelayUsec(40);	
	LCD_putchar(LCD_DSP_CSR);
	DelayUsec(40);
	LCD_putchar(LCD_CLR_DSP);
	DelayMsec(5);
	LCD_putchar(LCD_CSR_INC);
}

/* Send one byte c (instruction or data) to the LCD */
void LCD_putchar(uchar c) {
	E = 1;
	Data = c;
	E = 0;
	E = 1;
	Data <<= 4;
	E = 0;
}

void LCD_puts(const uchar *s) {
	RS=1;
	int i=0;
	while (*s!='\0')
	  {
	   i=i+1;
	   if(i<17)
	   {
	   LCD_putchar(*s);
	   DelayUsec(40);
	   s=s+1;
	   }
	   else
	   {
	   if(i==17)
	   {
	       LCD_goto(0x40);
	       RS=1;
	   }
	   if(i==33)
	   {
	       LCD_goto(0x00);
	       RS=1;
	   }
	   if (i<33)
	   {
	   //LCD_goto(0x40);
	   DelayUsec(40);
	   LCD_putchar(*s);
	   DelayUsec(40);
	   s=s+1;
	   }
	   else
	   {
	   //LCD_goto(0x00);
	   LCD_putchar(*s);
	   DelayUsec(40);
	   s=s+1;
	   }
	   }
  } 
//NOTE: must wait for at least 40 us after sending each character to 
//the LCD module.
}

/* go to a specific DDRAM address addr */
void LCD_goto(uchar addr) {
     RS=0;
     LCD_putchar(addr+0x80);
     DelayUsec(40);
     // ...send an address to the LCD
      //NOTE: must wait for at least 40 us after the address is sent
}

/* configure timer SFRs to generate num us delay*/
void GenUsec(){    
	T2CON = 0x0; // Stop any 16/32-bit Timer2 operation
	T3CON = 0x0; // Stop any 16-bit Timer3 operation
	T2CONSET = 0x0078; // Enable 32-bit mode, prescaler 256:1,
	// internal clock
	TMR2 = 0x0; // Clear contents of TMR32
	PR2 = 0x0002 ; // Load PR32
	T2CONSET = 0x8000; // Start Timer 32
    while (1)
	{
		//IEC0SET = 0x00001000; // Enable Timer3 interrupt
			if (IFS0bits.T3IF==1)
			{
				IFS0CLR=0x00001000;
                break;
			}		
	}
}

void DelayUsec(uchar num) {
      uchar i;
      for (i=0; i<num; i++) {
      GenUsec();
      }
}

/* configure timer SFRs to generate 1 ms delay*/
void GenMsec() { 
	T2CON = 0x0; // Stop any 16/32-bit Timer2 operation
	T3CON = 0x0; // Stop any 16-bit Timer3 operation
	T2CONSET = 0x0078; // Enable 32-bit mode, prescaler 256:1,
	// internal clock
	TMR2 = 0x0; // Clear contents of TMR32
	PR2 = 0x07A1 ; // Load PR32
	T2CONSET = 0x8000; // Start Timer 32
    while (1)
	{
		//IEC0SET = 0x00001000; // Enable Timer3 interrupt
			if (IFS0bits.T3IF==1)
			{
				IFS0CLR=0x00001000;
                break;
			}		
	}
}

/* Call GenMsec() num times to generate num ms delay*/
void DelayMsec(uchar num) {
      uchar i;
      for (i=0; i<num; i++) {
      GenMsec();
      }
}
/******************/

