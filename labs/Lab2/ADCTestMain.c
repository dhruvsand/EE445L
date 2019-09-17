// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdlib.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t ADCvalue;
int32_t Required_Debug_array1[1000];
int32_t Required_Debug_array2[1000];
int array1_pointer;
int array2_pointer;
int complete_flag;

void Timer1_Init(void);
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
	ADC0_SAC_R=0x06;
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
		if(array1_pointer <= 999){
		Required_Debug_array1[array1_pointer] = TIMER1_TAR_R;//This one is obviously the timer
			array1_pointer++;
		Required_Debug_array2[array2_pointer]  = ADCvalue;//This one is obviously the ADC
			array2_pointer++;
	}else{
		complete_flag = 1;
	}
	
  PF2 ^= 0x04;                   // profile
	
	
	
	
}
void Jitter_Calculation(void){//int *, 
	int i; 
	int timer_max;
	int timer_min;
	int ADC_min;
	int ADC_max;
	int time_difference;
	int ADC_difference;
	int temp,temp2;
	
	timer_max =  abs(Required_Debug_array1[1] - Required_Debug_array1[0]);;
	ADC_max =  abs(Required_Debug_array2[1] - Required_Debug_array2[0]);;
	time_difference = 0;
	ADC_min =  abs(Required_Debug_array2[1] - Required_Debug_array2[0]);;
	timer_min =  abs(Required_Debug_array1[1] - Required_Debug_array1[0]);;
	
	
	for(i =2; i< 999; i++){
		temp=abs(Required_Debug_array1[i] - Required_Debug_array1[i-1]);
		
		if(temp < timer_min){
			timer_min  = temp;
		}
		if(temp > timer_max){
			timer_max  = temp;
		}
		//This is the part for the ADC
		temp2=abs(Required_Debug_array2[i] - Required_Debug_array2[i-1]);
		if(temp2 < ADC_min){
			ADC_min  = temp2;
		}
		if(temp2 > ADC_max){
			ADC_max  = temp2;
		}
		
	}
	time_difference = timer_max - timer_min;
	ADC_difference = ADC_max - ADC_min;

	i++;
	
}

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	Timer1_Init();												//Timer 1 for the clock values
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
  while(1){
  //  PF1 ^= 0x02;  // toggles when running in main
		    GPIO_PORTF_DATA_R ^= 0x02;  
		
			if(complete_flag == 1){
			Jitter_Calculation();
			break;
		}
	
	}
	//)x0936 to 0x93C
//	int arrx[]={0x0936,0x0937,0x0938,0x0939,0x093A,0x093B,0x093C};
//	int arry[7];
//	int z=0;
//	for(int i=0;i<7;i++){
//		int count=0;
//		for(int x=0;x<1000;x++){
//		if(Required_Debug_array2[x]==arrx[i])
//			count++;
//		
//		}
//		arry[i]=count;
//		
//		z++;
//		
//		
//		
//	}
	
	
	
	
	
	
			int x1,y1,x2,y2;
		x1=0;
		y1=0;
		x2=127;
		y2=128;
			ST7735_InitR(INITR_REDTAB);
	    ST7735_FillScreen(ST7735_BLACK); 
	
	
	

		while(1){
			
			
		ST7735_DrawCue(x1,y1,x2,y2, ST7735_BLUE);
			x1=(x1+1)%128;
			y1=(y1+2)%160;
		}
		
		
			
		
			
  }



// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
//  PeriodicTask = task;          // user function
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
//  TIMER1_TAILR_R = period-1;    // 4) reload value
	TIMER1_TAILR_R = 0xFFFFFFFF;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
//  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
//  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}




