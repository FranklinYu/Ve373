#include <p32xxxx.h>

/*********************************
 * Input:                        *
 *     L1  L2  L3   L4           *
 *     CN8 CN9 CN18 CN17         *
 *     RG6 RG7 RF5  RF4          *
 * RB11 ~ RB14 is used as output *
 *********************************/

// FLAGS and return values
unsigned char key_F, key_E; // in test, key_F is not used
int number; // return number

void key_init()
{
  asm("di");
	TRISFSET = 0x30;
	TRISGSET = 0xC0;
	CNCONbits.ON = 1;
	CNENSET = 0x60300;
	CNPUESET = 0x60300;
	PORTF;
	PORTG;
	IPC6bits.CNIP = 7; // stop preemption of TIMER incrementing
	IPC6bits.CNIS = 3;
	IFS1bits.CNIF = 0;
	IEC1bits.CNIE = 1;
	TRISCCLR = 0x1E;
	PORTCCLR = 0x1E;
	TRISDCLR = 1; // pressing indicator
	PORTDCLR = 1; // turn off the light at first
	asm("ei");
}

#pragma interrupt CN_int ipl7 vector 26
void CN_int()
{
	int read = readPort();
	static unsigned int typed; // typed number
	static unsigned int password;
	static unsigned char mode; // setting alarm / time / date / password, or unlocking (0)
	static unsigned char i_type; // typed digit counter

	unsigned short row, column; // 4 means CN CN11 or RB14;
	if (read > 7) // hit on control button
		key_E = 1;
	if (key_E && read) // to skip nonsense hits
	{
		switch (read)
		{
		case 8:
			column = 4;
			break;
		case 4:
			column = 3;
			break;
		case 2:
			column = 2;
			break;
		case 1:
			column = 1;
			break;
		default:
			column = 0; // error (multi-key)
		}

		int i;
		for (i = 0; i < 4; i++)
		{
			PORTCSET = 0x1E;
			PORTCCLR = 1 << i + 1;
			int new = readPort();
			if (new == read)
				row = i + 1;
		} // multi-key: the lowest key
		PORTCCLR = 0x1E;

		number = get_num (row, column);
	}
	PORTDINV = 1;
	IFS1bits.CNIF = 0;
}

int readPort ()
{
	int readF = PORTF;
	int readG = PORTG;
	int read = readG >> 6 & 3;
	read |= readF >> 3 & 4 | readF >> 1 & 8; // link [4:1]
	read = ~read & 0xF;
	return read;
}

int get_num(int row, int column)
{
	switch ((row << 4) + column)
	{
		case 0x11:
			return 1;
		case 0x12:
			return 2;
		case 0x13:
			return 3;
		case 0x21:
			return 4;
		case 0x22:
			return 5;
		case 0x23:
			return 6;
		case 0x31:
			return 7;
		case 0x32:
			return 8;
		case 0x33:
			return 9;
		case 0x42:
			return 0;
		default:
			return -1; // exception
	}
}

int main ()
{
	INTCONbits.MVEC = 1; // Enable multiple vector interrupt
	key_init();
	while (1);
	return 0;
}
