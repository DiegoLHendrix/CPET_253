/****************************************************************************************
         CPET253 Lab4 - Servos and Ultrasonic Sensing

 Jeanne Christman
 original version 1/18/2024

 This program uses an ultrasonic sensor to determine when there is an object in front of a
 forward moving robot. Once it is determined that the robot is approaching an object, it backs up,
 stops and then controls a servo motor to sweep the sensor 90 degrees right then 90 degrees left.
 A distance measurement is taken on each sweep. The robot then turns in the direction that is "more
 clear" and continues forward again.

 The servo motor is controlled by a PWM signal from TimerA3. The period of the PWM signal is 20ms
 and the pulse width ranges from 1 ms to 2 ms to control the sweep. The PWM signal is output on
 pin P8.2

 The ultrasonic sensor trigger is connected to pin P6.2 and the echo is connected to pin P6.3. The
 distance measurement is calculated using the width of the pulse returned from the sensor. TimerA2
 is used to determine the duration of the pulse from the sensor.

 Functions in this code:
     -Clock_Init48MHz() - function provided by TI to set system clock
     -Clock_Delay1ms(time) - built in function that delays time ms
     -Clock_Delay1us(time) - built in function that delays time us
     -Port2_Init();
     -Port3_Init();
     -Port5_Init();
     -Port6_Init();
     -Port8_Init();
     -TimerA0_Init();
     -TimerA2_Init();
     -TimerA3_Init();
     -Motor_Forward(volatile uint16_t rightDuty, volatile uint16_t leftDuty );
     -Motor_Backward(volatile uint16_t rightDuty, volatile uint16_t leftDuty );
     -Motor_Right(volatile uint16_t rightDuty, volatile uint16_t leftDuty );
     -Motor_Left(volatile uint16_t rightDuty, volatile uint16_t leftDuty );
     -Motor_Stop();

The state machine has 6 states; forward, right, left, backward, sweep right, sweep left
*******************************************************************************************/

#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/motor.h"
#include "../inc/Init_Ports.h"
#include "../inc/Init_Timers.h"

#define TRIGGER 0x04
#define ECHO 0x08

#define microsecondsToClockCycles(a) ( (a) * 1.5 )       //assume 12Mhz clock divided by 8
#define clockCyclesToMicroseconds(a) ( (a) / 1.5 )       // 1.5 clock cycles = 1us

void Servo(uint16_t angle);
uint32_t pulseIn (void);


void ServoInit(void)  //This function initializes the servo to be centered (0 degrees)
{
    Servo(4499);//call Servo() function to center servo, angle_count = counts * (4/12MHz)
    Clock_Delay1us(20000);//delay here to give servo time to move - can use built in timer function
    TA3CTL &= ~0x0030;//stop the timer
    return;
}
void Servo(uint16_t angle_count) // this function moves the servo.
//input: angle_count should be in terms of clock counts to create the 
//desired pulse width in the PWM signal
{
    //set period for 20ms
    TA3CCR0 = 59999;//counts
    TA3CCR2 = angle_count;//set high time for the input angle using angle_count
    //set timer for up mode
    TA3CTL |= 0x0010;//0001 0000
    TA3CTL &= ~0x0020;//~0010 0000
    return;
}
uint16_t distanceInCm(void) {  //this function measures and returns the distance to the nearest object
    uint16_t distance, timeBuff;

    P6OUT |= TRIGGER;//drive trigger pin high
    Clock_Delay1us(10);//wait 10 us - can use built-in timer function
    P6OUT &= ~TRIGGER;//drive trigger pin low
    timeBuff = pulseIn();
    distance = timeBuff * (0.034 / 2);//calculate distance using s=t * 0.034/2. t comes from pulseIn() function
    // if no echo (distance = 0), assume object is at farthest distance
    return distance;//return the distance
}
uint32_t pulseIn (void)  //this function returns the width of the return pulse
//from the ultrasonic sensor in terms of microseconds
{
    uint16_t width = 0;   //will be in clock counts
    uint16_t time = 0;    //the result of converting clock counts to microseconds
    uint16_t maxcount = 56999;  //max count for 38 ms (timeout)

    //set timer for continuous mode
    TA2CTL |= 0x0020;//0000 0000 0010 0000
    TA2CTL &= ~0x0010;//0000 0000 0001 0000
    TA2CCR0 = 59999;//reset the count register
    while(!(P6IN & ECHO)){//wait for the pulse to start (while Echo is low)
        if(TA2R > maxcount){//if count is greater than maxcount return 0
            return 0;
        }
    }
    TA2R = 0;//reset the count register
    while(P6IN & ECHO){//wait for the pulse to finish (while Echo is high)
        if(TA2R > maxcount){//if count is greater than maxcount return 0
            return 0;
        }
    }
    if(TA2R > maxcount){
            return 0;
        }
    width = TA2R;//read the count (width of the return pulse)
    TA2CTL &= ~0x0030;//stop the timer
    time = clockCyclesToMicroseconds(width);//convert the reading to microseconds.
    return time;//return the microsecond reading
}

void main(void)
{

    uint16_t distance, right_wall, left_wall;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();  // makes bus clock 48 MHz
	//call all the port initialization functions
	Port1_Init();
	Port2_Init();
	Port3_Init();
	Port5_Init();
	Port6_Init();
	Port8_Init();
	//call all the timer initialization functions
	TimerA0_Init();
	TimerA2_Init();
	TimerA3_Init();
	//center the servo using the ServoInit() function
	ServoInit();
	//These are the states of the state machine
	enum motor_states {FORWARD, RIGHT, LEFT, BACKWARDS, SWEEP_LEFT, SWEEP_RIGHT}state, prevState;
	
	state = FORWARD;          //start in FORWARD state
	prevState = !FORWARD;   //used to know when the state has changed
	uint16_t stateTimer = 0;           //used to stay in a state
	bool isNewState;              //true when the state has switched

	while(1){
	    Servo(1500);//right
	    Clock_Delay1ms(1000);//one second delay
	    Servo(4500);//center
	    Clock_Delay1ms(1000);
	    Servo(7500);//left
	    Clock_Delay1ms(1000);
	    distance = distanceInCm();
	    Clock_Delay1ms(10);
	}

	/**/
	while(0) {
	    isNewState = (state != prevState);
	    prevState = state;

	    switch (state) {
	    case FORWARD:
	        //entry housekeeping
	        if(isNewState){
	            stateTimer = 0;
	            Motor_Forward(14999, 14999);
	        }

	        //state business
	        stateTimer++;
	        distance = distanceInCm();  //this needs to be moved to the states in which it is used

	        //exit housekeeping
	        if(stateTimer >= 100000 & distance < 15){
	            Motor_Stop();
	            state = BACKWARDS;
	        }
	        break;
	    case BACKWARDS:
	        //entry housekeeping
	        if(isNewState){
	            stateTimer = 0;
	            Motor_Backward(14999, 14999);
	        }

	        //state business
	        stateTimer++;

	        //exit housekeeping
	        if(stateTimer >= 100){
	            Motor_Stop();
	            state = SWEEP_LEFT;
	        }
	        break;
	    case SWEEP_RIGHT:
	        //entry housekeeping
	        if(isNewState){
	            stateTimer = 0;
	            Servo(1500);
	        }

	        //state business
	        right_wall = distanceInCm();

	        //exit housekeeping
	        if(Servo(1500)){
	            state = SWEEP_LEFT;
	        }
	        break;
	    case SWEEP_LEFT:
	        //entry housekeeping
	        if(isNewState){
	            stateTimer = 0;
	            Servo(7500);
	        }

	        //state business
	        left_wall = distanceInCm();

	        //exit housekeeping
	        if(Servo(7500) & left_wall < distance){
	            state = LEFT;
	        } else if(Servo(7500) & right_wall < distance){
                state = RIGHT;
            }
	        break;
	    case LEFT:
	        //entry housekeeping
	        if(isNewState){
	            stateTimer = 0;
	            Motor_Left(14999, 0);
	        }

	        //state business
	        stateTimer++;

	        //exit housekeeping
	        if(stateTimer >= 100){
	            Motor_Stop();
	            state = FORWARD;
	        }
	        break;
	    } //switch 
        Clock_Delay1ms(20);
	}  //while
	/**/
}
