/*
 * Virker. Koden tjekker om et LED output er HIGH, eller LOW og tænder en anden LED hvis den er.
 * Der er bygget videre på en UART koden i usart_transmit, men den bruges ikke. 
*/

#define F_CPU 1000000UL   // 1 MHz (16000000UL hvis dit board kører 16 MHz)

#include <avr/io.h>
#include <util/delay.h>
// #include <stdint.h>



void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
void USART_SendString(const char *s);

int main(void){

	int led_toggle = 0;

	DDRB	|=  (1 << PB3) | (1 << PB5) | (1 << PB6);	// Sæt PB3, -5 og -6 som output
	PORTB	|=  (1 << PB3) | (1<<PB5) | (1>>PB6); // Sluk LED D3. -5 og -6 : sæt output high

	DDRC	&= ~(1 << PC2);	// PC2 som input
	PORTC	|=  (1 << PC2);	// pull-up on

	uint8_t led3_state = 0;
	
	
	// For 1 MHz og 9600 baud i normal async mode:
	// UBRR = F_CPU/(16*BAUD) - 1 = 1000000/(16*9600) - 1 ≈ 6
	
	// baud 2400 ved 1 MHz, normal mode = 25
	USART_Init(25);

	while(1){
	if (!(PINC & (1 << PC2)) )   // aktiv low: knap trykket
	{
		_delay_ms(20);          // debounce
	
		if (!(PINC & (1 << PC2)))   // tjek igen
		{
			PORTB ^= (1 << PB3);    // skift LED
	
			if (led3_state) {
				led3_state = 0;
			} else{
				led3_state = 1;
			}
	
			while (!(PINC & (1 << PC2))) {
				// vent på at knappen slippes
				
			}
	
			_delay_ms(20);          // debounce ved slip
		}
//	} else if ((PINC & (1 << PC2))) {
		
	} // END else if


	if( led3_state ) {
		PORTB &= ~(1 << PB6);   // LOW
	}else {
		PORTB |= (1 << PB6);   // HIGH
	}

/*
	if( (PINB & (1 << PINB3))){
		PORTB = (1 << PB5); // Sluk LED D4 : sæt output high
		_delay_ms(500);
	}
*/
/*		if (PINC & (1 << PC2) && led_toggle == 0) {
			// input er high og LED er slukket.
			PORTB		&= ~(1 << PB3);   // output low
			led_toggle	= 1;
		} else if(PINC & (1 << PC2) && led_toggle == 1) {
			// input er high og LED er tændt.
			PORTB		|=  (1 << PB3);   // output high
			led_toggle	= 0;
		}
*//*		USART_SendString("Hello\r\n");
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
