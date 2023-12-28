#include <stdint.h>
#include <stdbool.h>
#include <msp.h>

/*
 * The following main function is an example on how to read keys from keypad.
 */

/*int main(void)
{
	char keyCode, keyNum;

	InitKeypad();													// initialize GPIO pins
	P3DIR = BIT2|BIT3;							// set P3.2 & P3.3 as output pins (LED0-LED1)
	P2DIR = BIT5|BIT4;							// set P2.5 & P2.4 as output pins (LED2-LED3)

	while(1)
	{
		keyCode = ReadKeypad();							// try to read a key from keypad
		if (keyCode != 0) 									// if a key is pressed...
		{ 
			keyNum = keyCode; 								// reserve the key code
			keyCode = ReadKeypad();						// try to get the key again
			if (keyCode == keyNum) 						// is the same key?
			{
				P3OUT = (keyCode & 0x03) << 2; 	// display keycode on LEDs LED0-LED1
				P2OUT = 0;											// clean P2 output pins to 0
				P2OUT |= (keyCode & BIT2) << 3;	// display bit2 on keyCode in LED2 (P2.5 pin)
				P2OUT |= (keyCode & BIT3) << 1;	// display bit3 on keyCode in LED3 (P2.4 pin)
			}	
		}			
	}
}*/

/*
 * initialize the ports connected to the kaypad
 */
void  InitKeypad(void)
{
	WDTCTL = WDTPW | WDTHOLD;       // stop WDT
	P6DIR = BIT1;									  // configure P6.1 as output pin
	P4DIR = BIT0|BIT2|BIT4;				  // set P4.0, P4.2 & P4.4 as output pins
	P3DIR &= ~BIT6;									// set P3.6 as input pin
	P5DIR &= ~(BIT0|BIT2);					// set P5.0 & P5.2 as input pins
	P4DIR &= ~BIT6;									// set P4.6 as input pin
	P3REN = BIT6;
	P3OUT &= ~BIT6; 								// pull-down resistor input on P3.6
	P5REN = BIT0|BIT2;
	P5OUT &= ~(BIT0|BIT2);					// pull-down resistor input on P5.0 & P5.2
	P4REN = BIT6;
	P4OUT &= ~BIT6;									// pull-down resistor input on P4.6
}

/*
 * Read the keycode (char) that corresponds to the pressed key
 * by scanning the keypad's rows and columns
 */
char  ReadKeypad(void)
{
	P4OUT = 0;													// output Low to all P4 pins
	P6OUT = BIT1;												// output High to P6.1 (row0)
	if (GetKey(0)) {return GetKey(0);} 	// get the key on row0
	P6OUT &= ~BIT1;											// output LOW to P6.1 (row0)
	P4OUT = BIT0;												// output High to P4.0 (row1)	
	if (GetKey(1)) {return GetKey(1);} 	// get the key on row1
	P4OUT &= ~BIT0;											// output LOW to P4.0 (row1)
	P4OUT = BIT2;												// output High to P4.2 (row2)	
	if (GetKey(2)) {return GetKey(2);}	// get the key on row2
	P4OUT &= ~BIT2;											// output LOW to P4.2 (row2)
	P4OUT = BIT4;												// output High to P4.4 (row3)	
	if (GetKey(3)) {return GetKey(3);}	// get the key on row3
	P4OUT &= ~BIT4;											// output LOW to P4.4 (row3)
	return 0;
}

char GetKey(char row)
{
	if (P3IN & BIT6) return (row * 4 + 1);		// key in column 0 (P3.6)
	if (P5IN & BIT2) return (row * 4 + 2); 		// key in column 1 (P5.2)
	if (P5IN & BIT0) return (row * 4 + 3);		// key in column 2 (P5.0)
	if (P4IN & BIT6) return (row * 4 + 4); 		// key in column 3 (P4.6)

	return 0;
}
