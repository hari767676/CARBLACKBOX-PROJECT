#include <xc.h>
#include "uart.h"

void init_uart(void)
{
	/* Serial initialization */
	RX_PIN = 1;
	TX_PIN = 0;

	/* TXSTA:- Transmitor Status and control Register */
	/* 9bit TX enable or disable bit */ 
	TX9 = 0;
	/* UART Tarsmition enable bit */
	TXEN = 1;
	/* Synchronous or Asynchronous mode selection */
	/* Asynchronous */
	SYNC = 0;
	/* Send the Break character bit */
	SENDB = 0;
	/* Low or High baud rate selection bit */
	/* High Baud Rate */
	BRGH = 1;

	/* RCSTA :- Recepition Status and control Register */
	/* TX/RC7 and RX/RC6 act as serial port */ 
	SPEN = 1;
	/* 9bit RX enable or disable bit */
	RX9 = 0;
	/* Continous reception enable or disable */ 
	CREN = 1;

	/* BAUDCTL:- Baud rate control register */

	/* 16bit baud generate bit */ 
	BRG16 = 0;
	

	/* Baud Rate Setting Register */
	/* Set to 10 for 115200, 64 for 19200 and 129 for 9600 */
	SPBRG = 129;


	/* TX interrupt flag bit */
	TXIF = 0;

	/* RX interrupt enable bit */
	RCIF = 0;
}

void putch(unsigned char byte) 
{
	/* Output one byte */
	/* Set when register is empty */
	while(!TXIF)//check wether the register is empty or not
	{
		continue;
	}
	TXIF = 0;//reg is not empty(full))
	TXREG = byte;
} 

int puts(const char *s)//used to tramsmit a sting
{
	while(*s)		
	{
		putch(*s++);	
	}
	return 0;
}

unsigned char getch(void)//receive the 1 byte of data
{
	/* Retrieve one byte */
	/* Set when register is not empty */
	while(!RCIF)  
	{
		continue;
	}
	RCIF = 0;//reg is empty
	return RCREG;
}

unsigned char getche(void)  //receive and trasmit back the same data
{
	unsigned char c;

	putch(c = getch());//it will receieve 1 byte of data

	return (c);
}
