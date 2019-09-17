// PeriodicTimer0AInts.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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



#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "PLL.h"
#include "Timer0A.h"
#include "Timer1A.h"
#include "DAC.h"
#include "Switch.h"
#include "Music.h"
#include "SysTick.h"
#include "Buttons.h"
#include "Timer2A.h"
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
}


const Song song = {90, mary_lamb};
bool Play_mode = false;
void Pause(void);



//debug code
int main(void){ 
	DisableInterrupts();
  PLL_Init(Bus80MHz);								// bus clock at 80 MHz
	DAC_Init(0);// initalize dac to 1.5 volts 
	Buttons_Init();
	PortF_Init();
	Timer1_Init();
	Timer2_Init();
	SysTick_Init();
	Song_PlayInit(song);
	int bpm= 80000000/song.tempo;
  Timer0A_Init(&Song_PlayHandler, bpm);		// initialize timer0A (16 Hz)
 EnableInterrupts();
	Pause();


  while(1){
		//do nothing as we let the timers1 keep interacting
		//DAC_Output(4096);
		//SysTick_Wait10ms(10);
		//DAC_Output(0);
		//SysTick_Wait10ms(10);
		int b=0;
	   GPIO_PORTF_DATA_R ^= 0x02;  
		for(int i=0; i<100000;i++){
		b++;
		}
		
		
		
  }
}
