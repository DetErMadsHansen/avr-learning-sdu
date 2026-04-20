/* 
 * lap_counter.c fil tilpasset til slotcar in- og outputs. 
 * 
 * Udviddet med PWM funktionalitet. Sat til fast værdi. 
 * Tæller omgange ved at registrere sensor input fra målstregen.
 * Timer til LED - Lys i et sekund når input registreres. Intern interrupt timer
 *
 * LED active low.
 * PD2 : ekstern interrupt mappede til input knap s11.
 * PB7 : LED indikator til målstregsregistrering.
 * PD7 : PWM til motor
 *
 * Working.
*/

#define F_CPU 1000000UL
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

		// Tæl 12 overflows, giver cirka 3 sekunder. 
		if(n_overflow  >= 12) {
			PORTB |= (1 << PB7); // LED slukket, output high

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
	PORTB &= ~(1 << PB7);   // LED tændt (active low)
	n_overflow = 0;         // start tælling forfra
	led_timer_active = 1;   // aktiver timer-logikken
	lab_counter++;			// Tæl en omgang.

} // END ISR målstregs sensor


//void pwm_set_duty(uint8_t duty);


int main(void) {
	// Output pins og porte
	DDRB	|= (1 << PB7) | (1 << PB0);	// PB7 som output Hvad vil jeg bruge PB0 til. Var det en ekstra LED indikator, når bilen var tændt?
	PORTB	|= (1 << PB7) | (1 << PB0);	// LED slukket, output high

	DDRD	|= (1 << PD7); // PWM test port til motor.

	// Input pins
	DDRD	&= ~(1 << PD2);	// S11 (PD2) som input, også PD2 på bil (ben 31).
	PORTD	|=  (1 << PD2);	// pull-up on
	
	// gl. Input pins - Hvorfor har jeg skrevet C i port og DDR?
//	DDRC	&= ~(1 << PD2);	// S11 (PD2) som input også PD2 på bil (ben 31).
//	PORTC	|=  (1 << PD2);	// pull-up on




//!	TCCR1A	= (1<<COM1A1) | (0<<COM1A0) | (1<<WGM11) | (0<<WGM10); // PWM test
//!	TCCR1B	= (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10); // PWM test
	TCCR2	= 0b01101011;  // 0x69 =Fast PWM mode, no pre_scaling, start timer


	// Timer0: normal mode, prescaler 1024
	TCCR0 |= (1<<CS02) | (1<<CS00); // prescaler 1024
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
	//	pwm_set_duty(50); // PWM test
		if( !stop_counter ) {
			OCR2 = 70; // Set PWM 0 - 100% : 0 - 255 : Min. 70 ved 4,88V 0,5A (for at motor kører på test board)
		} else {
			OCR2 = 0; // stop PWM
		}
	} // END while, superloop
} // END main



/* PWM test
* pwm_set_duty(duty)
* 
* @param int duty value 0 to 255
* 60% = 153
* @return Sets the OCR1A 
*/
/*
void pwm_set_duty(uint8_t duty) {
	OCR2 = (uint8_t)round((duty * 255)/ 100));
}
*/
