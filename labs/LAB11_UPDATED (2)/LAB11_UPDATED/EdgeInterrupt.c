// EdgeInterrupt.c
// Runs on LM4F120 or TM4C123
// Request an interrupt on the falling edge of PF4 (when the user
// button is pressed) and increment a counter in the interrupt.  Note
// that button bouncing is not addressed.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 5.6, Section 5.5

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

// user button connected to PF4 (increment counter on falling edge)

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
//#include "ISR.h"
#include "coeff.h"
#include "PLL.h"
//#include "fixed.h"
//#include "ST7735.h"
#include "ECG.h"
#include "SSD2119.h"
#include "Buttons.h"
#include "ACCEL.h"
#include "ADCT0ATrigger.h"
//#include "ADCT0ATrigger.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define PI 3.14159265
void DelayWait10ms(uint32_t n);
void PortF_Init(void);
void PortB_Init(void);
void Set_Timer(void);

int snooze_counter,snooze_flag=0,alarm_on_flag=0, alarm_flag=0;

// const will place these structures in ROM



#define PF2   (*((volatile uint32_t *)0x40025010))
#define PF3   (*((volatile uint32_t *)0x40025020))
#define PF4   (*((volatile uint32_t *)0x40025040))
#define PB0   (*((volatile uint32_t *)0x40005004))
#define PB1   (*((volatile uint32_t *)0x40005008))
#define PB2   (*((volatile uint32_t *)0x40005010))
#define PB3   (*((volatile uint32_t *)0x40005020))	
	
void Set_Alarm(void);
	int time=0,seconds=0, time_changed_flag=0; int milliseconds  =0;

int hour=0,minute=0, alarm_hour=0,alarm_minute=0;
void alarm_call(void);
int set_Display=0;


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
  //TIMER0_TAILR_R = 799999;         // start value for 1 Hz interrupts
	TIMER0_TAILR_R = 799;              //Start clock for 1000HZ interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts

  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  milliseconds++;
	if(milliseconds >= 1000){
		seconds +=1;
	}
	if(seconds>=60){
		seconds=0;
		time=(time+1)%720;
		time_changed_flag=1;
		
	}
	
	else{
		//TIMER0_TAILR_R = 799999;
		TIMER0_TAILR_R = 799;
	}
	
	
	
	
}

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	
}

//debug code
int main(void){
 /* //EdgeCounter_Init();           // initialize GPIO Port F interrupt
	int button_pressed;
	PLL_Init(Bus80MHz); 
	ST7735_InitR(INITR_REDTAB);
	ACCEL_Init();
	Buttons_Init();
	ECG_Init();
	EnableInterrupts();
	button_pressed = 0;
  while(1){
		if(button_pressed == 0){
			ACCEL_Get_Data();
		}else{
			ECG_Get_Data();
		}
  }
}*/
	int delay;
	PLL_Init(Bus80MHz);                      // 80 MHz system clock
  //SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
	//SYSCTL_RCGCGPIO_R |= 0x0000003F;
//	delay = SYSCTL_RCGC2_R;
	DisableInterrupts();
  //PLL_Init(Bus80MHz); 
//	GPIO_PORTF_DIR_R |= 0x06;                // make PF2 out (built-in LED)
//  GPIO_PORTF_AFSEL_R &= ~0x06;             // disable alt funct on PF2
//  GPIO_PORTF_DEN_R |= 0x06;                // enable digital I/O on PF2
//                                           // configure PF2 as GPIO
//  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
//  GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
//  GPIO_PORTF_DATA_R &= ~0x06;              // turn off LED
	//GPIO_PORTF_DATA_R |= 0x04;
	
	//GPIO_PORTF_DATA_R ^= 0x04;
	//Touch_Init();
	//ST7735_InitR(INITR_REDTAB);
	//GPIO_PORTF_DATA_R |= 0x04;
	//ACCEL_Init();//COmmented out by dhruv as not using
	//GPIO_PORTF_DATA_R ^= 0x02; 
	//Buttons_Init(); //Commented out by dhruv as it initializes both Port E and F Pur
	//GPIO_PORTF_DATA_R ^= 0x02; 
	//GPIO_PORTF_DATA_R ^= 0x04; 
	LCD_Init();
	
	
	ECG_Init();
	
	//GPIO_PORTF_DATA_R ^= 0x02; 
	EnableInterrupts();
  while(1){
		//GPIO_PORTF_DATA_R &= ~0x06;  
    ECG_Display_Data(2);
		//GPIO_PORTF_DATA_R ^= 0x04;              // turn off LED
		/*
		LCD_ColorFill(0xFFFF);	
				
				
				
				for(int i=0; i< 320 ; i++){
					int ADCvalue= fifo_pop();
					if(ADCvalue==-1){
						i=i-1;
						continue;
					}
					LCD_SetCursor(0,0);
					LCD_PrintInteger(ADCvalue);
					//int temp = Temperature_Convert(ADCvalue);
					LCD_PrintString("\n");
					LCD_PrintInteger(ADCvalue/100);
					LCD_PrintString(".");
					LCD_PrintInteger(ADCvalue%100);
						unsigned short yval= 220* ADCvalue/ 4095;
						LCD_DrawPixel(i,yval, 0x1234 );
						for(int j=0; j<10000;j++){
							int y=0;
							y++;
							}
				}
  }*/
}
	}
