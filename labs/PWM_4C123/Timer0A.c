// Timer0A.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

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
#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "Tach.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function


// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init( ){long sr;
  sr = StartCritical(); 
	
	SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F

  GPIO_PORTF_DIR_R &= ~0x01;       // make PF1 input
  GPIO_PORTF_AFSEL_R |= 0x01;      // enable alt funct on PF1
  GPIO_PORTF_DEN_R |= 0x01;        // configure PF1 as T0CCP0
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xF0FFFFFF)+0x07000000;
  TIMER0_CTL_R &= ~0x00000001;     // disable timer0A during setup
  TIMER0_CFG_R = 0x00000004;       // configure for 16-bit capture mode
  TIMER0_TAMR_R = 0x00000007;      // configure for rising edge event
  TIMER0_CTL_R &= ~0x0000000C;     // rising edge
  TIMER0_TAILR_R = 0x0000FFFF;     // start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit 
  TIMER0_IMR_R |= 0x00000004;      // enable capture match interrupt
  TIMER0_ICR_R = 0x00000004;       // clear timer0A capture match flag
  TIMER0_CTL_R |= 0x00000001;      // timer0A 24-b, +edge, interrupts
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; //Timer0A=priority 2
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
  EndCritical(sr);
}

void Timer0A_Handler22(void){
  //TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
 // (*PeriodicTask)();                // execute user task
	//Task();
	
	
}
