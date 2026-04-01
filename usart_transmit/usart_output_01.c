#define F_CPU 1000000UL   // 1 MHz (16000000UL hvis dit board kører 16 MHz)

#include <avr/io.h>


void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
void USART_SendString(const char *s);

int main(void){

	// For 1 MHz og 9600 baud i normal async mode:
	// UBRR = F_CPU/(16*BAUD) - 1 = 1000000/(16*9600) - 1 ≈ 6
	
	// baud 2400 ved 1 MHz, normal mode = 25
	USART_Init(25);

	while(1){
		USART_SendString("Hello\r\n");
		for (volatile unsigned long d = 0; d < 50000; d++) {
			;   // lille pause så terminalen kan følge med
		}
	} // END while
} // END main

/*
* USART Initialization
* Example from datasheet section 20.5
* Takes baud rate, values see table 20-9
* std 2400
*/
void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;

	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);

	/* Set frame format: 8data, 2stop bit */
	/* Eksempel kode fra datasheet. */
//	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);

	/* ChatGPT : De fleste terminalopsætninger bruger normalt 8N1: 8 databits, ingen parity, 1 stopbit. */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}


/*
 *  Transmit data
 *	See table 20.6.1 
*/
void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	
	/* Put data into buffer, sends the data */
	UDR = data;
}


/*
 * Send string
 * ChatGPT
*/
void USART_SendString(const char *s)
{
	while (*s) {
		USART_Transmit(*s++);
	}
}
