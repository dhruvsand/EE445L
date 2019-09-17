// SysTick.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  If you are using this module, you may need more
// precise timing, so it is assumed that you are using the PLL to set
// the system clock to 50 MHz.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1, Program 4.7
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 2.11, Section 2.6

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "dac.h" 
#include "Sound.h"
#include "piano.h"
//#include "SysTick.h"
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value


int p = 0;
/*struct State{
	;//double sin_value;
	uint32_t DAC_sin;          
};

typedef const struct State STyp;


STyp IYTab[32]={                             //MIGHT NEED TO CHANGE SINE CURVE TO REFLECT 2^5 bits          might need to get to 0x1F
	{1.7, 8},{2.0,10},{2.3,11},{2.6,13},{2.8,14},{3.0,14},{3.2,15},
	{3.3, 15},{3.2,15},{3.0,14},{2.8,14},{2.6,13},{2.3,11},
	{2.0,10},{1.7, 8},{1.3,6},{1.1,5},{.8,4},{.6,3},{.4,2},{.3,2},{.1,1},
	{.1, 1},{.3,2},{.2,1},{.3,2},{.4,2},{.6,3},{.8,4},{1.1,6},{1.3,6},
	{1.7, 8}};

const uint32_t IYTab[32]={                             //MIGHT NEED TO CHANGE SINE CURVE TO REFLECT 2^5 bits          might need to get to 0x1F
	8, 10,11,13,14,14,15,
	15,15,14,14,13,11,
	10,8,6,5,4,3,2,2,1,
	1,2,1,1,2,3,4,6,6,
	8};
	*/
	const uint32_t IYTab[32]={                             //MIGHT NEED TO CHANGE SINE CURVE TO REFLECT 2^5 bits          might need to get to 0x1F
	16, 20,22,24,26,28,29,
	31,29,28,26,24,22,
	20,16,12,10,7,5,3,2,1,
	1,1,2,3,4,5,7,10,12,
	16};
/*
	const uint32_t IYTab[64]={                             //MIGHT NEED TO CHANGE SINE CURVE TO REFLECT 2^5 bits          might need to get to 0x1F
	32,35,38,41,44,47,49,52,54,56,58,59,61,62,62,63,63,63,62,62,61,59,58,
	56,54,52,49,47,44,41,38,35,32,29,26,23,20,17,15,12,10,8,6,5,3,2,2,1,
	1,1,2,2,3,5,6,8,10,12,15,17,20,23,26,29};
	*/
uint32_t Sin(void){                  	// HAVE THE OTPUTTER OF THE SIN VALUE WE WANT   AKA THE IY value
	uint32_t out = IYTab[p];
	p = (p+1)&0x1F;
	return(out);                       // set this value equal to waht will point to the DAC
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(uint32_t period){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = period/32;  //                    maximum reload value = the frequency / number of bit possibilities
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF) | 0x40000000;
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;
	
}
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(uint32_t delay){
  uint32_t g = 0;
  while(g <= delay){
		g++;
    //SysTick_Wait(500000);  // wait 10ms (assumes 50 MHz clock) original    800000
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 20 nsec for 50 MHz clock)
	}
}
void SysTick_Handler(void){
	extern int keys_pressed;
	if(keys_pressed == 0){
		uint32_t DAC_SIN = Sin();
		DAC_SIN = DAC_SIN & 0x1F;
		DAC_Out(DAC_SIN);
	}else{
		DAC_Out(0);
	}
}
