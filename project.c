/***********************************************************
 *      Project code
 **********************************************************/

#include <p32xxxx.h>

int main ()
{
    initialTimer ();
    initialPWM ();
    initialSpeaker ()
    startAll ();
    while (1);
    return 0;
}


/***********************************************************
 *      button.c
 **********************************************************/
#include <p32xxxx.h>

// CN8 ~ CN11 corresponds to RG6 ~ RG9 (J10:55~58)
// RB11 ~ RB14 is used as output

// FLAGS
unsigned char key_F, key_E;

void key_init()
{
    asm("di");
	TRISGSET = 0x3C0;
	CNCONbits.ON = 1;
	CNENSET = 0xF00;
	CNPUESET = 0xF00;
	PORTG;
	IPC6bits.CNIP = 7; // stop preemption of TIMER incrementing
	IPC6bits.CNIS = 3;
	IFS1bits.CNIF = 0;
	IEC1bits.CNIE = 0;
	asm("ei");
	TRISBCLR = 0x7800;
	PORTBCLR = 0x7800;
	TRISDSET = 4; // pressing indicator
	PORTDCLR = 4; // turn off the light at first
}

#pragma interrupt CN_int ipl6 vector 26
void CN_int()
{
	static unsigned int typed; // typed number
	static unsigned int password;
	static unsigned char mode; // setting alarm / time / date / password, or unlocking (0)
	static unsigned char i_type; // typed digit counter

	unsigned short row, column; // 4 means CN CN11 or RB14;
	int readG = (PORTG & 0x3C0) >> 6;
	if (readG > 7) // hit on control button
		key_E = 1;
	if (key_E) // to skip nonsense hits
	{
		switch (readG)
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
			PORTBSET = 0x7800;
			PORTBCLR = 1 << i + 11;
			if (!PORTG & readG << 6)
				row = i + 1;
		} // multi-key: the lowest key
		PORTBCLR = 0x7800;

		switch ((row << 4) + column)
		{
		case 0x14:
			typed = 0;
			mode = 1;
			i_type = 0;
			break;
		case 0x24:
			typed = 0;
			mode = 2;
			i_type = 0;
			break;
		case 0x34:
			typed = 0;
			mode = 3;
			i_type = 0;
			break;
		case 0x44:
			typed = 0;
			mode = 4;
			i_type = 0;
			break;
		default: // pure number input
			int tpnumber = get_num(row, column);
			if (tpnumber >= 0) // skip the two blank button
			{
				typed = typed * 10 + tpnumber;
				i_type++;

				switch ((i_type << 4) + mode)
				{
				case 0x61: // alarm setting finish
				case 0x62: // time setting finish
					unsigned short t_s, t_m, t_h;
					t_s = typed % 100;
					t_m = typed / 100 % 100;
					t_h = typed / 10000;
					if (t_s < 60 && t_m < 60 && t_h < 24)
					{
						Second[0] = t_s % 10;
						Second[1] = t_s / 10;
						Minute[0] = t_m % 10;
						Minute[1] = t_m / 10;
						Hour[0] = t_h % 10;
						Hour[1] = t_h / 10;
					}
					key_E = 0;
					break;
				case 0x83: // date setting finish
					unsigned short t_y, t_mon, t_d;
					t_d = typed % 100;
					t_mon = typed / 100 % 100;
					t_y = typed / 10000;
					// leap year???
				case 0x64: // password setting finish
					password = typed;
					key_E = 0;
					break;
				case 0x60: // password hitting finish
					key_F = typed == password;
					key_E = 0;
				}
			}
		}
	}
	// other instructions
	PORTDINV = 4; // button pressing indicator
	IFS1bits.CNIF = 0;
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
