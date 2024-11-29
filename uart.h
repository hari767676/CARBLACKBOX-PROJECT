#ifndef SCI_H
#define SCI_H

#define RX_PIN					TRISC7
#define TX_PIN					TRISC6

void init_uart(void);//configure 3 registers
void putch(unsigned char byte);//transmit 1 byte if data
int puts(const char *s);//trasmit the string
unsigned char getch(void);//receive 1 byte of data
unsigned char getche(void);//receive 1 byte of data and transmit 1 byte of data

#endif
