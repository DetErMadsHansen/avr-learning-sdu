/*
 * Eksempel 8, kapitel 11, s.386.
 * Virker, tilpasset så det passer til pin-in og -outs på udviklingsboardet.
 * Using Timer0 generate a square wave on pin PORTB.5, while at the same time transferring data from PORTC to PORTD.
 * Tilpasset porte på test-board.
*/


#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

int main () {
//	DDRB	|= 0x20; //DDRB.5 = output

	
	TCNT0 = -32; //timer value
	TCCR0 = 0x01; //Normal mode, int clk, no prescaler

	TIMSK = (1<<TOIE0); //enable Timer0 overflow interrupt
	sei (); //enable interrupts

//	DDRC = 0x00; //make PORTC input
//	DDRB = 0xFF; //make PORTD output - Obs ændret til portB
//	PORTB	|= 0x01;  // LED off (active-low)

	DDRB	= 0xFF;   // output
	PORTB	= 0xFF;  // LED off (active-low)
	
	DDRC	= 0x00;   // input
	PORTC	= 0xFF;  // pull-ups på alle input-pins

	while (1) { //wait here
		PORTB = PINC; // Obs portD ændret til portB
	} // END while 
} // END main

ISR (TIMER0_OVF_vect) { //ISR for Timer0 overflow
	TCNT0 = -32;
	PORTB ^= 0x80; //toggle PORTB.5 // LED nr. 8 = 8 bit (eg. d7 punktum) : 128 : 0b10000000 : 0x80 // Ændret fra 0x20 til 0x80
} // END ISR
