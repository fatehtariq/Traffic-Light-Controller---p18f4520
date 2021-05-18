#include <string.h>
#include <timers.h>
#include <p18f4520.h>

#pragma config OSC = HS
#pragma config LVP = OFF
#pragma config WDT = OFF
#pragma config DEBUG = OFF

#define MENU_MESSAGE 0
#define RED_ON 1
#define YELLOW_ON 2
#define GREEN_ON 3
#define RESET_TRUE 4
#define PATTERN_STOP 5

char MR[20] = "Red Led Is ON.\r";
char MY[20] = "Yellow Led Is ON\r";
char MG[20] = "Green Led Is ON\r\r";
char MReset[20] = "Pattern Reset\r\r";
char PStop[20] = "Pattern Stopped\r\r";
char menu[150] = "Traffic Light Controller:\r\rPress 'S' For To Start traffic light pattern\rPress 'R' to Reset pattern\rPress 'O' to stop\r";

#define LRed PORTDbits.RD0
#define LYellow PORTDbits.RD1
#define LGreen PORTDbits.RD2

char ch;
int x = 0;
int z = 0;
int message = 0;


void TX_ISR()
{
	if (message == MENU_MESSAGE) // Write out Menu Message 
	{
		if (x < strlen(menu))
		{
			TXREG = menu[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
	else if (message == RED_ON) // Write out Red LED Message 
	{
		if (x < strlen(MR))
		{
			TXREG = MR[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
	else if (message == YELLOW_ON) // Write out Yellow LED Message 
	{
		if (x < strlen(MY))
		{
			TXREG = MY[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
	else if (message == GREEN_ON) // Write out Green LED Message 
	{
		if (x < strlen(MG))
		{
			TXREG = MG[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
	else if (message == RESET_TRUE) // Write out Reset pattern Message 
	{
		if (x < strlen(MReset))
		{
			TXREG = MReset[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
	else if (message == PATTERN_STOP) // Write out Pattern Stop Message 
	{
		if (x < strlen(PStop))
		{
			TXREG = PStop[x];
			x++;
		}
		else
		{
			PIE1bits.TXIE = 0;
		}
	}
}

void RC_ISR()
{
	ch = RCREG;
}

#pragma interrupt polls
void polls(void)
{
	if (PIR1bits.TXIF == 1)
	{
		TX_ISR();
	}
	if (PIR1bits.RCIF == 1)
	{
		RC_ISR();
	}
}

#pragma code high_vector = 0x008 // High Interrupt Location
void interrup_handler(void)
{
	_asm GOTO polls
	_endasm
}
#pragma code

void delay(int secs)
{
	switch (secs)
	{
	case 1:
	{
		T0CON = 0x05;  // 1 second delay repeated three times for 3 second delay
		TMR0H = 0x67;
		TMR0L = 0x69;
		T0CONbits.TMR0ON = 1;
		while (INTCONbits.TMR0IF == 0);
		T0CONbits.TMR0ON = 0;
		INTCONbits.TMR0IF = 0;
		break;
	}
	case 2:
	{
		T0CON = 0x05; // 4 second delay
		TMR0H = 0x67;
		TMR0L = 0x60;
		T0CONbits.TMR0ON = 1;
		while (INTCONbits.TMR0IF == 0);
		T0CONbits.TMR0ON = 0;
		INTCONbits.TMR0IF = 0;
		break;
	}
	case 3:
	{
		T0CON = 0x06; // 5 second delay
		TMR0H = 0x1B;
		TMR0L = 0x1B;
		T0CONbits.TMR0ON = 1;
		while (INTCONbits.TMR0IF == 0);
		T0CONbits.TMR0ON = 0;
		INTCONbits.TMR0IF = 0;
		break;
	}
	}
}

void main()
{
	TRISD = 0x00;
	TRISCbits.TRISC6 = 0;
	TRISCbits.TRISC7 = 1;

	message = MENU_MESSAGE;

	TXSTA = 0x20;
	SPBRG = 15; // Set Baud Rate
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

	RCSTAbits.CREN = 1;
	RCSTAbits.SPEN = 1;
	TXSTAbits.TXEN = 1;

	PIE1bits.RCIE = 1;
	PIE1bits.TXIE = 1;

	while (1)
	{
		if (ch == 'S' || ch == 's')
		{
			while (1)
			{
				// Display Red LED Message 
				message = RED_ON;
				x = 0;
				while (x < strlen(MR)) PIE1bits.TXIE = 1;
				PIE1bits.TXIE = 0;
				// Turn Red LED on
				LRed = 1;
				for (z = 0; z < 2; z++)
				{
				delay(1);
				}
				// Turn off once Red Delay Over
				LRed = 0;
				if (ch == 'R' || ch == 'r') // Checks to see if User has pressed to reset pattern
				{
					ch = 'S'; 
					message = RESET_TRUE;
					x = 0;
					while (x < strlen(MReset)) 
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0; 
					continue;
				}
				if (ch == 'o' || ch == 'o') // Checks to see if User has pressed to stopped program
				{ 
					message = PATTERN_STOP;
					x = 0;
					while (x < strlen(PStop)) 
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0; 
					break;
				}

				// Display Yellow LED Message 
				message = YELLOW_ON;
				x = 0;
				while (x < strlen(MY)) PIE1bits.TXIE = 1;
				PIE1bits.TXIE = 0;
				// Turn Yellow LED on
				LYellow = 1;
				for (z = 0; z < 3; z++)
				{
				delay(2);
				}
				// Turn off once Yellow Delay Over
				LYellow = 0;
				if (ch == 'R' || ch == 'r') // Checks to see if User has pressed to reset pattern
				{
					ch = 'S';
					message = RESET_TRUE;
					x = 0;
					while (x < strlen(MReset))
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0;
					continue;
				}
				if (ch == 'o' || ch == 'o') // Checks to see if User has pressed to stopped program
				{
					message = PATTERN_STOP;
					x = 0;
					while (x < strlen(PStop))
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0;
					break;
				}

				// Display GREEN LED Message 
				message = GREEN_ON;
				x = 0;
				while (x < strlen(MG)) PIE1bits.TXIE = 1;
				PIE1bits.TXIE = 0;
				// Turn Green LED on
				LGreen = 1;
				delay(3);
				// Turn off once Green Delay Over
				LGreen = 0;
				if (ch == 'R' || ch == 'r') // Checks to see if User has pressed to reset pattern
				{
					ch = 'S';
					message = RESET_TRUE;
					x = 0;
					while (x < strlen(MReset))
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0;
					continue;
				}
				if (ch == 'o' || ch == 'o') // Checks to see if User has pressed to stopped program
				{
					message = PATTERN_STOP;
					x = 0;
					while (x < strlen(PStop))
					PIE1bits.TXIE = 1;
					PIE1bits.TXIE = 0;
					break;
				}
			}
		}
		else if (ch == 'R' || ch == 'r')
		{
			// Display Message and Reset Pattern back to start
			message = RESET_TRUE;
			x = 0;
			while (x < strlen(MReset)) PIE1bits.TXIE = 1;
			PIE1bits.TXIE = 0;
			ch = 'a';
		}
	}

	if (ch == 'O' || ch == 'o')
	{
		// Display Off Message
		message = PATTERN_STOP;
		x = 0;
		while (x < strlen(PStop)) PIE1bits.TXIE = 1;
		PIE1bits.TXIE = 0;
		// Turn off all LEDs
		LRed = 0;
		LYellow = 0;
		LGreen = 0;
	}
}
