#include "msp.h"

// A program that configures two timers to toggle LEDs 
// so that they are out of phase by 0.1 seconds
// TimerA0 triggers the Red LED every 1 second
// TimerA1 switches the RGB to the next color every 0.5 seconds
// author: Cameron Davis

void TA0_N_IRQHandler(void) {
	TA0CTL &= (uint16_t)(~(1<<0)); // clear IFG
	P1OUT ^= (uint8_t)(1<<0); // toggle red LED
}

void TA1_N_IRQHandler(void) {
	// first interrupt is after 0.1s then permanently set to every 0.5s
	static int x = 0;
	if (!x) { TA1CCR0 = (uint16_t)(8191); x = 1; }
	
	TA1CTL &= (uint16_t)(~(1<<0)); // clear IFG
	switch (P2OUT & (uint8_t)0x07) { // roll through RGB states
			case 0x00 :
				P2OUT &= (uint16_t)0x01;
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
				P2OUT &= ~(uint8_t)0x06;
				break;
		}
}

int main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	
	// Configure LEDs (P1.0, P2.0, P2.1, P2.2) as outputs
	P2SEL0 &= (uint8_t)(~((1 << 2) | (1 << 1) | (1 << 0))); // GPIO Function
	P2SEL1 &= (uint8_t)(~((1 << 2) | (1 << 1) | (1 << 0)));
	P2DIR |= (uint8_t)((1 << 2) | (1 << 1) | (1 << 0)); // configure RGB LED as output
	P1DIR |= (uint8_t)(1<<0); // configure red LED as output
	
	// Initialize LED states as OFF
	P2OUT &= ~(uint8_t)((1 << 2) | (1 << 1) | (1 << 0)); // turn off RGB LED
	P1OUT &= ~(uint8_t)(1 << 0); // turn off RED LED
	
	// Configure TimerA0
	TA0CTL &= (uint16_t)(~((1<<5) | (1<<4))); // stop timer
	TA0CTL &= (uint16_t)(~(1<<0)); // clear interrupt flag
	TA0CCR0 = (uint16_t)(32767);
	TA0CTL |= (uint16_t)((1<<1)); // interrupt enable
	// Clock source select
	TA0CTL |= (uint16_t)((1<<8)); 
	TA0CTL &= (uint16_t)(~(1<<9)); 
	
	// Configure TimerA1
	TA1CTL &= (uint16_t)(~((1<<5) | (1<<4))); // stop timer
	TA1CTL &= (uint16_t)(~(1<<0)); // clear interrupt flag
	TA1CCR0 = (uint16_t)(3276);
	TA1CTL |= (uint16_t)((1<<1)); // interrupt enable
	// Clock source select
	TA1CTL |= (uint16_t)((1<<8));
	TA1CTL &= (uint16_t)(~(1<<9));
	
	// Red LED initially turned ON
	P1OUT |= (uint8_t)(1<<0);
	
	// start timers
	TA0CTL |= (uint16_t)((1<<4)); // up mode
	TA1CTL |= (uint16_t)((1<<5) | (1<<4)); // up/down mode
	
	//TimerA0 NVIC
	NVIC_SetPriority(TA0_N_IRQn, 2);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
	//TimerA1 NVIC
	NVIC_SetPriority(TA1_N_IRQn, 2);
	NVIC_ClearPendingIRQ(TA1_N_IRQn);
	NVIC_EnableIRQ(TA1_N_IRQn);

	__ASM("CPSIE I"); // Globally enable interrupts
	
	while (1) {
	}
}