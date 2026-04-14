/* 
 * Tæller omgange ved at registrere sensor input fra målstregen.
 * Timer til LED - Lys i et sekund når input registreres. Intern interrupt timer
 *
 * LED active low.
 * PD2 ekstern interrupt mappede til input knap s11.
 *
 * Not Working. Blinker med LED udelukkende gennem interrupt.
*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t n_overflow			= 0;
volatile uint8_t led_timer_active	= 0;

volatile uint8_t lab_counter		= 0;

// Timer0 overflow interrupt til LED signal.
ISR(TIMER0_OVF_vect) {
	if(led_timer_active) {
		n_overflow++;

		// Tæl 12 overflows, giver cirka 3 sekunder. 
		if(n_overflow  >= 12) {
			PORTB |= (1 << PB7); // LED slukket, output high

			led_timer_active	= 0;
			n_overflow 			= 0;
		}
	} // END if : led_timer_active
} // END ISR timer


// Eksternt interrupt på INT0 = PD2
// Kaldes når sensoren registrerer målstregen.
ISR(INT0_vect) {
	PORTB &= ~(1 << PB7);   // LED tændt (active low)
	n_overflow = 0;         // start tælling forfra
	led_timer_active = 1;   // aktiver timer-logikken
	lab_counter++;			// Tæl en omgang.
} // END ISR målstregs sensor


int main(void) {
	// Output pins og porte
	DDRB	|=  (1 << PB7);	// PB7 som output
	PORTB	|= (1 << PB7);	// LED slukket, output high
	

	// Input pins
	DDRC	&= ~(1 << PD2);	// S11 (PD2) som input
	PORTC	|=  (1 << PD2);	// pull-up on


	// Timer0: normal mode, prescaler 1024
	TCCR0 |= (1<<CS02) | (1<<CS00); // prescaler 1024
	TIMSK |= (1<<TOIE0);            // enable overflow interrupt

	// INT0 på faldende flanke
	// Godt valg når man bruger pull-up og signalet går til GND ved aktivering.
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	
	/* Enable INT0 */
	GICR |= (1 << INT0);

	sei(); // global interrupts

	// Superloop
	while(1) {
		// Not (!) pga. intern pull-up
//		if (!(PINC & (1 << PC0))) {
//			PORTB	&= ~(1 << PB7);	// LED tændt (active-low) - output low
//		}
	} // END while, superloop
} // END main
