/*
 * Virker. Koden tæller input og gemmer det i en varibel. Variabel værdien udskrives til en 8-bit led række.
 * Der er bygget videre på en UART koden i io_if. 
*/

#define F_CPU 1000000UL   // 1 MHz (16000000UL hvis dit board kører 16 MHz)

#include <avr/io.h>
#include <util/delay.h>

void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
void USART_SendString(const char *s);

int main(void){

	int counter		= 0;

	DDRB	= 0xFF;			// Sæt port b som output.
	PORTB	|= ~counter;	// Sluk ouput på port b

	DDRC	&= ~(1 << PC2);	// PC2 som input
	PORTC	|=  (1 << PC2);	// pull-up on

//	uint8_t led3_state = 0;
	
	
	// For 1 MHz og 2400 baud i normal async mode:
	// UBRR = F_CPU/(16*BAUD) - 1 = 1000000/(16*2400) - 1 = 25
	
	// baud 2400 ved 1 MHz, normal mode = 25
	USART_Init(25);

	while(1){
	if (!(PINC & (1 << PC2)) )   // aktiv low: knap trykket
	{
		_delay_ms(20);          // debounce
	
		if (!(PINC & (1 << PC2)))   // tjek igen
		{
			counter++;			// Øg variabel værdien når tryk registreres.
			PORTB = ~counter;	// Output værdien til LED-rækken.
	
			while (!(PINC & (1 << PC2))) {
				// vent på at knappen slippes	
			}
			_delay_ms(20);		// debounce ved slip
		}

	} // END if

/*		USART_SendString("Hello\r\n");
		for (volatile unsigned long d = 0; d < 50000; d++) {
			;   // lille pause så terminalen kan følge med
		}
*/	} // END while
} // END main


/* .:: Function definitions ::. */

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
