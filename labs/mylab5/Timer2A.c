#include <stdint.h>
#include "Buttons.h"
#include "../inc/tm4c123gh6pm.h"


double time;
// ***************** TIMER2_Init ****************
// Activate TIMER3 interrupts to run user task periodically
// Inputs:  none
// Outputs: none
void Timer2_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER2_TAILR_R = 79999;  // 4) reload value for 10 hz
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 6
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 21 in NVIC
	TIMER2_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer2A_Handler(void){
	
	
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	//Buttons_10ms_Handler();
	time +=1;
	
	
	
}
