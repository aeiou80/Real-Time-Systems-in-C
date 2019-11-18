#include "msp.h"

// A program that infinitely polls for user input via active-low switches
// and toggles the corresponding LEDs
// author: Cameron Davis

int main() {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	
	
	P1->SEL0 &= (uint8_t) (~((1 << 4) | (1 << 1) | (1 << 0)));
	P1->SEL1 &= (uint8_t) (~((1 << 4) | (1 << 1) | (1 << 0)));
	
	P2->SEL0 &= (uint8_t) (~((1 << 0) | (1 << 1) | (1 << 2)));
	P2->SEL1 &= (uint8_t) (~((1 << 0) | (1 << 1) | (1 << 2)));
	
	P1->OUT |= (uint8_t)((1 << 4) | (1 << 1));
	P1->REN |= (uint8_t)((1 << 4) | (1 << 1));
	P1->DIR &= (uint8_t)(~((1 << 4) | (1 << 1)));
	
	P1->DIR |= (uint8_t)((1 << 0));
	P2->DIR |= (uint8_t)((1 << 0) | (1 << 1) | (1 << 2));
	
	P1->OUT &= (uint8_t) (~(1 << 0));
	P2->OUT &= (uint8_t) (~((1 << 0) | (1 << 1) | (1 << 2)));

	while(1)
	{
		static uint8_t toggle=0x0;
		int i = 100;
	while((P1->IN &(uint8_t) 0x2)||(P1->IN &(uint8_t) 0x10)) {}//do nothing
		while(i > 0){i--;} //delay loop
		if((P1->IN &(uint8_t) 0x2)||(P1->IN &(uint8_t) 0x10))
			continue;
		else {
			//Button 1
			if (!(P1->IN & 0x02)) {
				toggle ^= 0x1;
				
			}else if (!(P1->IN & 0x10)) {
				if (toggle == 0) {
					P1->OUT ^= (uint8_t) 0x1;
				} else if(toggle==0x01){
					switch(P2->OUT & (uint8_t) 0x7) {
						case 0x00:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x01;
							break;
						case 0x01:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x02;
							break;
						case 0x02:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x03;
							break;
						case 0x03:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x04;
							break;
						case 0x04:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x05;
							break;
						case 0x05:
							P2->OUT &= ~(0x7);
							P2->OUT |= 0x06;
							break;
						case 0x06:
							P2->OUT &= ~(0x7);
							P2->OUT &= ~(0x07);
							break;
						case default:
							P2->OUT &= ~(0x7);
							break;
					}
			}
	}
}
}
	return 0;
	}