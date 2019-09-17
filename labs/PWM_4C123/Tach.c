#include <stdint.h>
#include "Timer0A.h"
#include "PWM.h"

#include "..//inc//tm4c123gh6pm.h"
#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define TIMER_TAMR_TACMR        0x00000004  // GPTM TimerA Capture Mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_IMR_CAEIM         0x00000004  // GPTM CaptureA Event Interrupt
                                            // Mask
#define TIMER_ICR_CAECINT       0x00000004  // GPTM CaptureA Event Interrupt
                                            // Clear
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low

uint32_t Period;
uint32_t First;        
int32_t Done;

uint32_t Target_Speed;
uint32_t Speed=0;      



int32_t Error = 0;           // speed error in 0.1 rps
int32_t Duty = 30000;           // duty cycle 40 to 39960
uint32_t delay = 1; // delay counter for PID control


void Tach_Init(void){
//on PB6
//	  First = 0;                       // first will be wrong
//  Done = 0;                        // set on subsequent
//	SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
//  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F

//  GPIO_PORTF_DIR_R &= ~0x01;       // make PF1 input
//  GPIO_PORTF_AFSEL_R |= 0x01;      // enable alt funct on PF1
//  GPIO_PORTF_DEN_R |= 0x01;        // configure PF1 as T0CCP0
//  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xF0FFFFFF)+0x07000000;
//  TIMER0_CTL_R &= ~0x00000001;     // disable timer0A during setup
//  TIMER0_CFG_R = 0x00000004;       // configure for 16-bit capture mode
//  TIMER0_TAMR_R = 0x00000007;      // configure for rising edge event
//  TIMER0_CTL_R &= ~0x0000000C;     // rising edge
//  TIMER0_TAILR_R = 0x0000FFFF;     // start value
//  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit 
//  TIMER0_IMR_R |= 0x00000004;      // enable capture match interrupt
//  TIMER0_ICR_R = 0x00000004;       // clear timer0A capture match flag
//  TIMER0_CTL_R |= 0x00000001;      // timer0A 24-b, +edge, interrupts
//  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; //Timer0A=priority 2
//  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
//	
	
	
		
	  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F

  int delay2 = SYSCTL_RCGC2_R;           // allow time for clock to start

  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F

  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0

  // only PF0 needs to be unlocked, other bits can't be locked

  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF

  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0

  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out

  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0

  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4

  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
	
	
	
  SYSCTL_RCGCTIMER_R |= 0x01;// activate timer0    
  SYSCTL_RCGCGPIO_R |= 0x22;       // activate port B and port F
                                   // allow time to finish activating
  First = 0;                       // first will be wrong
  Done = 0;                        // set on subsequent
  GPIO_PORTF_DIR_R &= ~0x01;       // make PB6 in
  GPIO_PORTF_AFSEL_R |= 0x01;      // enable alt funct on PB6/T0CCP0
  GPIO_PORTF_DEN_R |= 0x01;        // enable digital I/O on PB6
                                   // configure PB6 as T0CCP0
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFFFF0)+0x00000007;
  GPIO_PORTF_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
  GPIO_PORTF_DIR_R |= 0x04;        // make PF2 out (PF2 built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;     // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;        // enable digital I/O on PF2
                                   // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for 24-bit capture mode
  TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER0_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0xC);
  TIMER0_TAILR_R = TIMER_TAILR_TAILRL_M;// start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER0_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT19;     // enable interrupt 19 in NVIC
	
	
	
	
	

}

void Timer0A_Handler(void){
//	TIMER0_ICR_R = 0x00000004;       // acknowledge timer0A
//TIMER0_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer0A capture match
//	
//  //Period = (First - TIMER0_TAR_R);
//	//if(Period > 1000000)
//		//Period = 1000000;//overflow
// Period = (First - TIMER0_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
//  First = TIMER0_TAR_R;            // setup for next
//  Done = 1;                        // set semaphore
//	
	
	  PF2 = PF2^0x04;  // toggle PF2
  PF2 = PF2^0x04;  // toggle PF2
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer0A capture match
  Period = (First - TIMER0_TAR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution
  First = TIMER0_TAR_R;            // setup for next
  Done = 1;
  PF2 = PF2^0x04;  // toggle PF2
	
	
	
	
	
	
	
	
	Speed = 80000000/Period; // 0.1 rps
	//Speed=10;
//	if(delay == 0){
//		Error = Target_Speed-Speed;   
//		Duty+= (100*Error)/64;  
//		if(Duty < 20000) 
//			Duty=20000;        // setting constrains
//		if(Duty>35000) 
//				Duty=35000;      // 2000 to 35000
//		PWM0B_Duty(Duty);            // output
//		delay = 1;
//	}
//	else {
//		delay -= 1;
//	}
	
	
	if(Speed<Target_Speed){
	Duty= Duty+100;
		if(Duty>=35000)
			Duty=35000;
	PWM0B_Duty(Duty);
	
	}
	else{
		Duty=Duty-100;
		if(Duty<=2000)
			Duty=2000;
		PWM0B_Duty(Duty);
		
	}
}




void Tach_Set_Speed(uint32_t Target){

Target_Speed=Target;
}

uint32_t Tach_GetPeriod(void){

return Period;
}

uint32_t Tach_GetFreq(void){

return 1;
}

uint32_t Tach_GetSpeed(void){


return Speed;
}