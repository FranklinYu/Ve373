/************************************************
 * lab5.c                                       *
 * SYSCLK = 4 MHz is assumed.                   *
 ************************************************/

#include <p32xxxx.h>
#include "LCD.h"
#include <plib.h>

double voltage = 0;
uchar startStr1[4];

void redisplay (double data)
{
    sprintf(startStr1,"%-4.2lf",data);
    RS=0;
    LCD_putchar(0x1); // clear display
    LCD_goto(0x0);
    RS=1;
    LCD_puts(startStr1);
}

int main ()
{
	OSCSetPBDIV (OSC_PB_DIV_1);
	MCU_init();
	LCD_init();
	AD1PCFG = 0xFFFB;// all PORTB = Digital; RB2 = analog
	AD1CON1 = 0x40;// Timer3 period match ends sampling, integer 16-bit
	AD1CHS = 0x00020000;// Connect RB2/AN2 as CH0 input
	// here RB2/AN2 is the input
	AD1CSSL = 0;
	AD1CON3 = 0x0000;// Sample time is TMR3, TAD = internal TPB * 2
	AD1CON2 = 0x0000;// Interrupt at the end of each conversion
	// set TMR3 to time out every 2 ms
	T3CONSET = 0; // 16-bit mode, prescaler 1:1, internal clock
	TMR3 = 0x0; // Clear TMR3
	PR3 = 8000; // period = 2ms, frequency = 500 Hz
	            // if SYSCLK = 80 MHz, then prescale = 1:32, PR3 = 5000
	T3CONSET = 0x8000; // Start TMR3
	AD1CON1bits.ADON = 1;//turn ON the ADC
	AD1CON1SET = 0x0004;// start auto sampling every 2 mSecs
	while (1)
	{
		int i = 0;
		for (; i < 50; i++)
		{
			while (!IFS1 & 0x0002){};// conversion done?
			voltage += ADC1BUF0/1024*3.3;
			IFS1CLR = 0x0002;// clear ADIF
		}
		redisplay(voltage/50); // display the average
	}// repeat
}

/*************end of lab5.c**************/
