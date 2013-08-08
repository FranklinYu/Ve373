/**********************************************************
 *      Project header                                    *
 **********************************************************/
#include <p32xxxx.h>

#define LCD_IDLE                0x33
#define LCD_2_LINE_4_BITS       0x28
#define LCD_2_LINE_8_BITS       0x38
#define LCD_DSP_CSR             0x0c
#define LCD_CLR_DSP             0x01
#define LCD_CSR_INC             0x06
#define LCD_SFT_MOV             0x14
#define LCD_DSP_FLASH  	        0x0f

#define RS      PORTDbits.RD3
#define E       PORTDbits.RD2
#define Data    PORTE
#define PA      PORTBbits.RB1 // Anode port of the LCD

typedef unsigned char uchar;

unsigned char key_F, key_E; // Flags for keyboard

uchar table[][3] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
uchar year[4],day[2],month[2],date;
uchar Second[2], Minute[2], Hour[2];
uchar small_mon, leap_year;
uchar CurrentTime[6], CurrentDate[11];
uchar CurrentTime_Disp[9];
unsigned int readD;
int setAlarm;
uchar AlarmTime[6], SnapTime[6];

void MCU_init();
void LCD_init();
void LCD_putchar(uchar c);
void LCD_puts(const uchar *s);
void LCD_goto(uchar addr);
void GenMsec();
void GenUsec();
void DelayUsec(uchar num);
void DelayMsec(uchar num);
void CNConfig(); // to initialize the keyboard
