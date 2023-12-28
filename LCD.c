//*********************************************************************
// DRALCD.c - The main application file for project DRALCD
//*********************************************************************
#include "LCD.h"

/*
 * The following main function is an example on how to use the LCD functions.
 * This main function initilises the LCD, clears the screen, and displays the
 * message "WELCOME TO IGEE!"
 */

	/*int main_example (void)
	{
	  WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer
	  LCD_Init();									  // initialize LCD controller
	  LCD_Comd(1);          			  // clear screen, move cursor to home

	  // Write "WELCOME TO IGEE!" on LCD
	  LCD_Data('W'); LCD_Data('E'); LCD_Data('L'); LCD_Data('C'); LCD_Data('O');
	  LCD_Data('M'); LCD_Data('E'); LCD_Data(' '); LCD_Data('T'); LCD_Data('O');
	  LCD_Data(' '); LCD_Data('I'); LCD_Data('G'); LCD_Data('E'); LCD_Data('E'); ;LCD_Data('!');
	}*/

/*
 * initialize eUSCI_B0 then initialize LCD controller
 */
void  LCD_Init(void)
{
	P1SEL0 |= BIT5|BIT6|BIT7;     // set SPI 3-pin P1.5, P1.6 & P1.7 as second function
	P6DIR |= BIT7;								// set P6.7 as output pin

	UCB0CTLW0 |= UCSWRST;         					// set UCSWRST = 1 to reset eUSCI_B0 module
	UCB0CTLW0 |= UCMST|UCSYNC|UCCKPL|UCMSB; // 3-pin master mode, 8-bit data length
	// clock polarity high, MSB first
	UCB0CTLW0 |= UCSSEL__ACLK;              // clock source = ACLK
	UCB0BRW = 0x01;                         // fBitClock = fBRCLK/(UCBRx+1)= fBRCLK/2
	UCB0CTLW0 &= ~UCSWRST;                  // reset UCSWRST to enable USCIB0 to begin normal work

	delay_ms(20);               // LCD controller reset sequence
	LCD_cd_Write(0x30, 0);			// send reset code 1 two times to SPLC780
	delay_ms(5);
	LCD_cd_Write(0x30, 0);
	delay_ms(1);
	LCD_cd_Write(0x30, 0);			// send reset code 2 to SPLC780
	delay_ms(1);

	LCD_cd_Write(0x20, 0);  		// use 4-bit data mode
	delay_ms(1);
	LCD_Comd(0x28);          		// set 4-bit data, 2-line, 5x7 font
	LCD_Comd(0x06);          		// move cursor right
	LCD_Comd(0x0C);          		// turn on display, cursor off - no blinking
	LCD_Comd(0x01);          		// clear screen, move cursor to home
}

/*
 * Send commands to the LCD controller
 */
void  LCD_Comd(unsigned char  cmd)
{
	LCD_cd_Write(cmd & 0xF0,  0);   // upper nibble first
	LCD_cd_Write(cmd << 4,  0);     // then lower nibble
	if (cmd < 4)
		delay_ms(2);         				  // command 1 and 2 needs up to 1.64ms
	else
		delay_ms(1);         				  // all others 40 µs
}

/*
 * Send data to the LCD controller
 */
void  LCD_Data(char  data)
{
	LCD_cd_Write(data & 0xF0,  RS);  // upper nibble first
	LCD_cd_Write(data << 4,  RS);    // then lower nibble
	delay_us(40);
}

void SPIB_Write(unsigned char  data)
{
	P6OUT = 0;       						 // clear STCP (CS_LCD) in 74VHCT595 to Low (P6.7)
	UCB0TXBUF = data;            // write serial data into 74VHCT595
	while (UCB0STATW & UCBUSY);  // wait for 74VHCT595 serial data shift done
	P6OUT |= BIT7;         			 // set CS_LCD (STCP) to High to simulate a positive-going-edge
}

void  LCD_cd_Write(char  data, unsigned char  control)
{
	data &= 0xF0;       				  					// clear lower nibble for data
	control &= 0x0F;    				 						// clear upper nibble for control
	SPIB_Write (data | control | BL);       // RS = 0, R/W = 0
	SPIB_Write (data | control | EN | BL);  // pulse E
	delay_ms(0);
	SPIB_Write (data | BL);
	SPIB_Write (BL);
}

/*
 * delay n milliseconds
 * (3 MHz CPU clock, default case of MSP432)
 */
void  delay_ms(int  time)
{
	int i,  j;
	for(i = 0 ; i < time; i++)
		for(j = 0; j < 596; j++){}  // do nothing for 1 ms
}

/*
 * delay n microseconds
 * (3 MHz CPU clock, default case of MSP432)
 */
void  delay_us(int  time)
{
	int i;
	for(i = 0 ; i < time;  i++){}  // do nothing for 1 µs
}
