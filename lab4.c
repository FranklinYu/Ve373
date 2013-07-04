/*********************************************************
 * Lab4.c
 *********************************************************
 #include <p32xxxx.h>
#include <plib.h>
/* Global variables */
unsigned int readD;
unsigned int acc = 500; //acc determines the acceleration rate

#pragma interrupt T2_ISR ipl6 vector 8
#pragma interrupt CN_ISR ipl7 vector 26
void T2_ISR (void)
{
  IFS0CLR = 0x00000100;
}
void CN_ISR (void) {
	static int pwm = 0; //variable duty cycle
	IEC1bits.CNIE = 0;
	if(readD==1)
		pwm += acc;
	if (pwm<0) 
		{pwm = 0;acc = -acc;}
	else if (pwm>3000) 
		{pwm = 3000;acc = -acc;}
	OC1RS = pwm; //update duty cycle register

	int n = 0;
	while (n < 1000) {
		n++;
	}

	IFS1bits.CNIF = 0;
	IEC1bits.CNIE = 1;
}

void CNConfig(void){
	asm("di");
	CNCON = 0x8000;
	CNENbits.CNEN15 = 1; //CN15/RD6/s1 as input
	CNPUEbits.CNPUE15 = 1;
	readD = PORTDbits.RD6;
	IPC6SET = 0x00140000; //Set priority level = 5
	IPC6SET = 0x00030000; //Set subpriority level = 3
	IFS1CLR = 0x0001; //Clear interrupt flag
	IEC1SET = 0x0001;
	asm("ei");
}

void initIntGlobal() {
	INTCONbits.MVEC = 1; // Enable multiple vector interrupt
	asm("ei"); // Enable all interrupts
}

void initPWM() {
	OC1CON = 0x0000; //stop OC1 module
	OC1RS = 0; //initialize duty cycle register
	OC1R = 0; //initialize OC1R register for the first time
	OC1CON = 0x0006; //OC1 16-bit, Timer 2, in PWM mode w/o FP
	T2CON = 0x0000; //Prescaler 1:1
	PR2 = 40000; //period=0.01s
	IFS0CLR = 0x00000100; //clear Timer 2 interrupt
	IEC0SET = 0x00000100; //enable Timer 2 interrupt
	IPC2SET = 0x0000000F; //Timer 2 interrupt priority 3, subpriority 3
	T2CONSET = 0x8000; //start Timer 2
	OC1CONSET = 0x8000; //enable OC1 module for PWM generation
						//RD0 as the output
}

/* main function */
int main() {
	OSCSetPBDIV (OSC_PB_DIV_1); 
	initIntGlobal();
	CNConfig();
	initPWM();
	while(1);
}
