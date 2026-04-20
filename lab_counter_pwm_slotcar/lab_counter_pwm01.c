/* 
 * lap_counter_pwm01.c fil tilpasset til slotcar in- og outputs. 
 * 
 * Udviddet med PWM funktionalitet, som er sat til en fast værdi. 
 * Tæller omgange ved at registrere sensor input fra målstregen.
 * Timer til LED - Lys i nogle sekund når input registreres. (Via intern interrupt timer)
 * PWM sættes til nul, når målstregen er registreret 5 gange. Derefter sættes PWM til nul.
 *
 * LED active low.
 * PD2 : sensor input. (Ekstern interrupt)
 * PB5 : LED indikator til målstregsregistrering.
 * PD7 : PWM motor
 * 
 * Working.
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t n_overflow			= 0; // Bruges til at tælle overflow til målstregsindikator LED'en. 
volatile uint8_t led_timer_active	= 0; // Bruges til at aktiverer timer når målstregen registreres.

volatile uint8_t lab_counter		= 0;
volatile uint8_t stop_counter		= 0;


// Timer0 overflow interrupt til LED signal.
ISR(TIMER0_OVF_vect) {
	if(led_timer_active) {
		n_overflow++;

		// Tæl 100 overflows, giver cirka 1,5 sekund. 
		if(n_overflow  >= 100) {
			PORTB |= (1 << PB5); // Sluk LED Indikator, output high

			led_timer_active	= 0;
			n_overflow 			= 0;
			
			if(lab_counter >= 5) {
				stop_counter = 1; // Stop bilen efter 3 sekunder og 5 omgange.
			}
		}
	} // END if : led_timer_active
} // END ISR timer


// Eksternt interrupt på INT0 = PD2
// Kaldes når sensoren registrerer målstregen.
ISR(INT0_vect) {
	PORTB 				&= ~(1 << PB5);	// LED tændt (active low)
	n_overflow 			= 0;			// start tælling forfra
	led_timer_active	= 1;			// aktiver timer-logikken
	lab_counter++;						// Tæl en omgang.

} // END ISR målstregs sensor


int main(void) {

	// Output pins og porte
	DDRB	|= (1 << PB5);	// LED Indikator, sæt port som output.
	PORTB	|= (1 << PB5);	// Sluk LED Indikator, output high
	DDRD	|= (1 << PD7);	// PWM til motor.

	// Input pins
	DDRD	&= ~(1 << PD2);	// Sensor/schmitt-trigger input, PD2 på bil (ben 31).
	PORTD	|=  (1 << PD2);	// pull-up on


//!	TCCR1A	= (1<<COM1A1) | (0<<COM1A0) | (1<<WGM11) | (0<<WGM10); // PWM test
//!	TCCR1B	= (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10); // PWM test
	TCCR2	= 0b01101011;  // Fra JJ's filer. 0x69 =Fast PWM mode, no pre_scaling, start timer


	// Timer0: normal mode, prescaler 1024
	TCCR0 |= (1<<CS02) | (1<<CS00); // set prescaler to 1024
	TIMSK |= (1<<TOIE0);            // enable overflow interrupt

	// INT0 på falling edge
	// Godt valg når man bruger pull-up og signalet går til GND ved aktivering.
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	
	/* Enable INT0 */
	GICR |= (1 << INT0);

	sei(); // global interrupts

	// Superloop
	while(1) {
		if( !stop_counter ) {
			OCR2 = 110; // Set PWM : værdi mellem 0 - 255
		} else {
			OCR2 = 0; // stop PWM
		}
	} // END while, superloop
} // END main
