#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/Init_Ports.h"
#include "../inc/Init_Timers.h"

#define LA 13635
#define LAS 12870
#define LB 12148
#define LC 11466
#define LCS 10822
#define LD 10215
#define LDS 9641
#define LE 9100
#define LF 8589
#define LFS 8107
#define LG 7652
#define LGS 7223
#define MA 6817
#define MAS 6435
#define MB 6073
#define MC 5732
#define MCS 5411
#define MD 5107
#define MDS 4820
#define ME 4550
#define MF 4294
#define MFS 4053
#define MG 3826
#define MGS 3611
#define HA 3408

#define quarter 396 * 2
#define half quarter * 2

void playNote(uint16_t note, uint16_t length){
    TA0CTL |= 0x0010;
    TA0CCR0 = note;
    TA0CCR1 = note / 2;
    Clock_Delay1ms(length);
    TA0CTL &= ~0x0010;
    Clock_Delay1ms(50);
}

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Port2_Init();
	TimerA0_Init();

	while(1)
	{
	    playNote(LF, quarter);
	    playNote(LF, quarter);
	    playNote(LF, quarter);
	    playNote(LC, quarter);

	    playNote(LD, quarter);
	    playNote(LD, quarter);
	    playNote(LC, half);
	    playNote(MA, quarter);

	    playNote(MA, quarter);
	    playNote(LG, quarter);
	    playNote(LG, quarter);
	    playNote(LF, half);

	    Clock_Delay1ms(quarter);

	    playNote(LC, quarter);
	    playNote(LF, quarter);
	    playNote(LF, quarter);
	    playNote(LF, quarter);

	    playNote(LC, quarter);
	    playNote(LD, quarter);
	    playNote(LD, quarter);
	    playNote(LC, half);

	    playNote(MA, quarter);
	    playNote(MA, quarter);
	    playNote(LG, quarter);
	    playNote(LG, quarter);
	    playNote(LF, half + quarter);

	    Clock_Delay1ms(2000);
	}

}
