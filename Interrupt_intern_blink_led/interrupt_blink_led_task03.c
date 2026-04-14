/* 
 * Løsning til Task 3, lektion er ukendt. (Fra itsl)
 * Working. Blinker med LED udelukkende gennem interrupt.
*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t n_overflow = 0;

ISR(TIMER0_OVF_vect)
{	
	n_overflow++;

	if(n_overflow  >= 2) // toggle every 2 × 262ms ? 0.5 s ? blink ? 1 Hz
	{
		PORTB ^= 0x01; // Toggle PB0
		n_overflow  = 0;
	}
}

int main(void)
{
	DDRB |= 0x01;   // PB0 as output
	PORTB |= 0x01;  // LED off (active-low)

	// Timer0: normal mode, prescaler 1024
	TCCR0 |= (1<<CS02) | (1<<CS00); // prescaler 1024
	TIMSK |= (1<<TOIE0);            // enable overflow interrupt

	sei(); // global interrupts

	while(1)
	{
		// nothing else here, LED toggles in ISR
	}
}
