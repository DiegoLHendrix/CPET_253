/**
 * @file      Init_Timers.h
 * @brief     Provide TimerA initialization functions 
 * @details   These functions will be used in labs 3-7 for CPET253
There is an initialization function for each of the instantiations of TimerA
 * @version   v1.1
 * @author    Prof. Christman
 * @date      28 June 2023
 *
*/

// ------------TimerA0_Init------------
void TimerA0_Init(void);
  // In the TA0CTL register:
  // 	Disable timer for initialization
  // 	Choose a clock source
  // 	Select a clock scaler if needed
  //    Enable overflow interrupt if needed
  // In the TA0EX0 register:
  //    Select a second clock scaler if needed
  // For EACH CCRn being used, in the TA0CCTLn register
  // 	set the PWM outmode if needed
  //    Enable the CCRn match interrupt if needed
  // For EACH CCRn being used, Load TA0CCRn with clock
  //	counts for desired delay

void TimerA1_Init(void);
  // In the TA1CTL register:
  // 	Disable timer for initialization
  // 	Choose a clock source
  // 	Select a clock scaler if needed
  //    Enable overflow interrupt if needed
  // In the TA1EX0 register:
  //    Select a second clock scaler if needed
  // For EACH CCRn being used, in the TA1CCTLn register
  // 	set the PWM outmode if needed
  //    Enable the CCRn match interrupt if needed
  // For EACH CCRn being used, Load TA1CCRn with clock
  //	counts for desired delay

void TimerA2_Init(void);
  // In the TA2CTL register:
  // 	Disable timer for initialization
  // 	Choose a clock source
  // 	Select a clock scaler if needed
  //    Enable overflow interrupt if needed
  // In the TA2EX0 register:
  //    Select a second clock scaler if needed
  // For EACH CCRn being used, in the TA2CCTLn register
  // 	set the PWM outmode if needed
  //    Enable the CCRn match interrupt if needed
  // For EACH CCRn being used, Load TA2CCRn with clock
  //	counts for desired delay

void TimerA3_Init(void);
  // In the TA3CTL register:
  // 	Disable timer for initialization
  // 	Choose a clock source
  // 	Select a clock scaler if needed
  //    Enable overflow interrupt if needed
  // In the TA3EX0 register:
  //    Select a second clock scaler if needed
  // For EACH CCRn being used, in the TA3CCTLn register
  // 	set the PWM outmode if needed
  //    Enable the CCRn match interrupt if needed
  // For EACH CCRn being used, Load TA3CCRn with clock
  //	counts for desired delay


