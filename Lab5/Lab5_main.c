/****************************************************************************************
         CPET253 Lab3 - PWM and Motor Drivers

 Jeanne Christman
 original version 6/1/2022

 This program uses a state machine to control the TI-RSLK robot to drive
 in a pattern of forward, right turn, backward, left turn, forward, right turn, ….. 

 To control the motors on the TI-RSLK robot, there are three outputs that need
 to be driven.
    :Pin    :Description            :Notes
    :=======:=======================:=========================
    : P5.5  : Right motor direction : 0=forwards, 1=backwards
    : P3.6  : Right motor sleep     : 0=sleep, 1=awake
    : P2.6  : Right motor PWM       : 0=stop, PWM signal = go
    : P5.4  : Left motor direction  : 0=forwards, 1=backwards
    : P3.7  : Left motor sleep      : 0=sleep, 1= awake
    : P2.7  : Left motor PWM        : 0=stop, PWM signal = go

 Functions in this code:
     -Clock_Init48MHz() - function provided by TI to set system clock
     -Clock_Delay1ms(time) - built in function that delays time ms
     -Port2_Init();
     -Port3_Init();
     -Port5_Init();
     -TimerA0_Init();
     -MotorForward(volatile uint16_t rightDuty, volatile uint16_t leftDuty ); 
     -MotorBackward(volatile uint16_t rightDuty, volatile uint16_t leftDuty ); 
     -MotorTurnRight(volatile uint16_t rightDuty, volatile uint16_t leftDuty ); 
     -MotorTurnLeft(volatile uint16_t rightDuty, volatile uint16_t leftDuty ); 

The state machine has 4 states; forward, right, left, backward
use FSM to make a pattern: Forward, right turn 90 degrees, backwards, left turn 90, forward...
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
#include "../inc/BumpInt.h"

#define RED 0x01 //0000 0001
#define GREEN 0x02 //0000 0010
#define BLUE 0x04 //0000 0100
#define PURPLE 0x05 //0000 0101

void LED_Color (uint8_t color) {
//This is a simple function to turn on the multi-colored LED on the MSP432 Launchpad
//board according to the argument passed into the function
//The LED is controlled by bits 0, 1 and 2 on PORT2
    P2OUT &= ~0x07; //first turn off all colors
    P2OUT |= color; //second turn on the input color
}

void main(void)
{
     extern bool wasInterrupt;
     extern direction;

       WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
       Clock_Init48MHz();  // makes bus clock 48 MHz
       //Call the appropriate functions from Init_Ports.c
       Port1_Init();
       Port2_Init();
       Port3_Init();
       Port4_Init();
       Port5_Init();

       //Call the appropriate functions from Init_Timers.c
       TimerA0_Init();

       DisableInterrupts();
       BumpInt_Init();
       EnableInterrupts();

       //These are the four states of the state machine
       enum motor_states {FORWARD, RIGHT, LEFT, BACKWARDS}state, prevState;

       state = FORWARD;//start state
       prevState = !FORWARD;//used to know when the state has changed
       uint16_t stateTimer;       //used to stay in a state
       bool isNewState;           //true when the state has switched

       P2OUT = 0x00;

       while(1)
       {
          isNewState = (state != prevState);
          prevState = state;  //save state for next time

          switch (state) {
          //each case below should have entry housekeeping, state business and exit housekeeping
          //remember to reset the stateTimer each time you enter a new state
          //you must assign a new state when stateTimer reaches the correct value
          case FORWARD:
              //entry housekeeping
              if(isNewState){
                  stateTimer = 0;
                  Motor_Forward(14999, 14999);
                  LED_Color(RED);
              }

              //state business
              stateTimer++;

              //exit housekeeping
              if(true == wasInterrupt){
                  P2OUT = 0x00;
                  Motor_Stop();
                  state = BACKWARDS;
                  wasInterrupt = false;
              }
              break;

          case BACKWARDS:
              //entry housekeeping
              if(isNewState){
                  stateTimer = 0;
                  Motor_Backward(14999, 14999);
                  LED_Color(BLUE);
              }

              //state business
              stateTimer++;

              //exit housekeeping
              if( (stateTimer >= 50) & (1 == direction) )
              {
                  Motor_Stop();
                  state = LEFT;
              }

              else if( (stateTimer >= 50) & (2 == direction))
              {
                  Motor_Stop();
                  state = RIGHT;
              }
              break;

          case RIGHT:
              //entry housekeeping
              if(isNewState){
                  stateTimer = 0;
                  Motor_Right(0, 14999);
                  LED_Color(GREEN);
              }

              //state business
              stateTimer++;

              //exit housekeeping
              if(stateTimer >= 100){
                  Motor_Stop();
                  P4IV &= 0x00;
                  state = FORWARD;
              }
              break;

          case LEFT:
              //entry housekeeping
              if(isNewState){
                  stateTimer = 0;
                  Motor_Left(14999, 0);
                  LED_Color(PURPLE);
              }

              //state business
              stateTimer++;

              //exit housekeeping
              if(stateTimer >= 100){
                  Motor_Stop();
                  P4IV &= 0x00;
                  state = FORWARD;
              }
              break;

          } //switch
          Clock_Delay1ms(10);  //10ms delay so that each increment of statetimer is 10ms
       } //while(1)
       /**/
} //main()
