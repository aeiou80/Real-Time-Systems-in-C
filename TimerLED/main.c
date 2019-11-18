#include "msp.h"

// A program that uses switches and timers to toggle selected LEDS
// Button 1 toggles between the LEDs, Button 2 pauses/resumes the system
// Behaviour: The currently selected LED switches its state every second
//			  via the timer. Red LED toggles on and off. RGB LED changes
//			  to the next color.
// author: Cameron Davis

int whichLED = 0; // Red = 0, RGB = 1

void PORT1_IRQHandler(void) {
	// button 1
	if ((P1IFG & (uint8_t)(1<<1)) != 0) {
		P1IFG &= ~(uint8_t)(1<<1); // clear IFG
		if (whichLED) { whichLED = 0; }
		else { whichLED = 1; }// toggle LED
	}
	// button 2
	else {
		P1IFG &= ~(uint8_t)(1<<4);
		TA0CTL ^= (uint16_t)(1<<1); // toggle timer interrupt enable
		}
}

void TA0_N_IRQHandler(void) {
	TA0CTL &= (uint16_t)(~(1<<0)); //clear IFG
	
	if (whichLED==0) {
		P1OUT ^= (uint8_t)(1<<0); // toggle red LED
	} else {
		switch (P2OUT & (uint8_t)0x07) { // roll through RGB states
			case 0x00 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x01;
				break;
			case 0x01 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x02;
				break;
			case 0x02 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x03;
				break;	
			case 0x03 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x04;
				break;
			case 0x04 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x05;
				break;
			case 0x05 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x06;
				break;
			case 0x06 :
				P2OUT &= ~(uint8_t)0x07;
				P2OUT |= (uint8_t)0x07;
			case 0x07 :
				P2OUT &= ~(uint8_t)0x07;
				break;
		}
	}
	
}

int main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	
	
	// Configure buttons 1 and 2 (P1.1 and P1.4) as inputs, pull-up internal resistors
	P1SEL0 &= (uint8_t)(~((1 << 4) | (1 << 1) | (1 << 0))); // GPIO Function
	P1SEL1 &= (uint8_t)(~((1 << 4) | (1 << 1) | (1 << 0)));
	P1DIR &= (uint8_t)(~((1 << 4) | (1 << 1))); // configure P1.1 and P1.4 direction as input (0)
	P1REN |= (uint8_t)((1 << 4) | (1 << 1)); // enable resistor in P1.1 and P1.4
	P1OUT |= (uint8_t)((1<<4) | (1<<1)); // ++configure as pull up resistor in P1.1 and P1.4

	// Configure LEDs (P1.0, P2.0, P2.1, P2.2) as outputs
	P2SEL0 &= (uint8_t)(~((1 << 2) | (1 << 1) | (1 << 0))); // GPIO Function
	P2SEL1 &= (uint8_t)(~((1 << 2) | (1 << 1) | (1 << 0)));
	P2DIR |= (uint8_t)((1 << 2) | (1 << 1) | (1 << 0)); // configure RGB LED as output
	P1DIR |= (uint8_t)(1<<0); // configure red LED as output
	
	// Configure LED states as OFF
	P2OUT &= ~(uint8_t)((1 << 2) | (1 << 1) | (1 << 0)); // turn off RGB LED
	P1OUT &= ~(uint8_t)(1 << 0); // turn off RED LED
	
	TA0CTL &= (uint16_t)(~((1<<5) | (1<<4))); //stop timer
	TA0CTL &= (uint16_t)(~(1<<0)); //clear interrupt flag
	TA0CCR0 = (uint16_t)(32767); //32,768 - 1
	TA0CTL |= (uint16_t)((1<<1)); //interrupt enable
	TA0CTL |= (uint16_t)((1<<4)); //up mode (count to CCR0)
	TA0CTL |= (uint16_t)((1<<8)); // Timer_A clock source select (set bit 8)
	TA0CTL &= (uint16_t)(~(1<<9)); // Timer_A clock source select (clear bit 9)
	
	// Configure interrupts **(DO THIS LAST)**
	P1IES |= (uint8_t)0x12; // P1.1 and P1.4 configured as falling edge event (active low switches)
	P1IFG &= (uint8_t)~0x12; // clear interrupt flags (don't want to prematurely interrupt)
	P1IE |= (uint8_t)0x12; // arm (enable) interrupt on P1.1 and P1.4

	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn); // clear pending interrupt in NVIC so no premature interrupt
	NVIC_EnableIRQ(PORT1_IRQn); // enable interrupt in NVIC
	
	NVIC_SetPriority(TA0_N_IRQn, 2);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);

	__ASM("CPSIE I"); // Globally enable interrupts at hardware level (in CPU)
	
	
	
	while (1) {
		// infinite loop
	}
	
	}