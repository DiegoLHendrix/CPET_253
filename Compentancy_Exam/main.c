//Diego Lopez Lab section 5
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\Init_Ports.h"
#include "..\inc\Init_Timers.h"

void TimerInit(void){
    //SMCLK, clk divider of 4, stop mode
    TA1CTL = 0x0280;   //0000 0010 1000 0000
    TA1CCTL2 |= 0x00E0;//outmode 7(Reset/Set) for CCR3
    TA1CCR0 = 59999;    //60,000 - 1(starts at 0), counts for PWM period
    TA1CCR2 = 41999;    //70% Duty Cycle - 1
}
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();  // makes bus clock 48 MHz

	//Call functions from Init_Ports.c to initialize ports
	Port1_Init();
	Port7_Init();
	TimerInit();

	while(1) {
	    //P1.1 = S1

	if ((P1IN & BIT1)) { //determine if S1 is pushed - remember it is active low
	    //sets timer to UP mode while button is pushed
	    TA1CTL = 0x0290;   //0000 0010 1001 0000

	    while(!(P1IN & BIT1)); //wait here for pin to be released.

	    }
	//Stops timer while button is not pushed
	TA1CTL = 0x0280;   //0000 0010 1000 0000

	} //end while

	return; //code can never get here
}
