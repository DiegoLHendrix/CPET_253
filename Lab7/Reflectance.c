// Reflectance.c
// Provide functions to take measurements using the kit's built-in
// QTRX reflectance sensor array.  Pololu part number 3672. This works by outputting to the
// sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.
// Daniel and Jonathan Valvano
// July 11, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// reflectance even LED illuminate connected to P5.3
// reflectance odd LED illuminate connected to P9.2
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\inc\Clock.h"


// ------------Reflectance_Read------------
// function to Read the eight IRsensors
//
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 100 ms
// Make the sensor pins input
// wait 'time' us
// Read sensors
// Turn off the 8 IR LEDs
//
// Input: time to wait in usec
// Output: sensor readings
// Assumes: port pins are configured correctly
uint8_t Reflectance_Read(uint32_t time){
    uint8_t result;

    /**
     * Turn on sensor LEDs
     */
    P5OUT |= BIT3;
    P9OUT |= BIT2;

    /**
     * Charge capacitor by setting
     * Capacitor will take around 100ms to charge
     */
    P7DIR |= 0xFF;
    P7OUT = 0xFF;
    Clock_Delay1ms(100);

    /**
     * Stop charging capacitor by setting the pins as inputs
     * Delay for time parameter then read the inputs
     */
    P7DIR &= ~0xFF;
    Clock_Delay1us(time);
    result = P7IN;

    /**
     * Turn off sensor LEDs
     */
    P5OUT &= ~BIT3;
    P9OUT &= ~BIT2;

  return result;
}

// ------------Reflectance_Position------------
// function to Perform sensor integration
//
// multiply each bit of the input 'data' by 
// it's respective weight, add the values together and 
// divide by the total number of non-zero bits
//          bit position   |  weight
//=========================|==============
//		0	   |	-334
//		1	   |	-238
//		2	   |	-142
//		3	   |	 -48
//		4	   |	  48
//		5	   |	 142
//		6	   | 	 238
//		7	   |	 334
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line
int32_t Reflectance_Position(uint8_t data){
    /**
     * Store weight values in an array.
     * Each value corresponds to a bit position 0 through 7 weight -334 through 334 respectively.
     */
    int16_t weight[8] = {-334, -238, -142, -48, 48, 142, 238, 334};

    /**
     * Get sensor input received through the data parameter
     * Multiply bit by its respective weight
     * Add each of the values together
     * Find total number of non zero bits
     */
    int32_t position = 0;
    int count = 0;
    int i;

    for(i = 0; i < 8; i++) {
        data = (data >> i);
        position = ( (data & 1) * weight[i] ) + position;
        if (BIT0 & data) {
            count++;
        }//if
    }//for

    /** Divide by the total number of non-zero bits */
    position /= count;

 return position;
}


