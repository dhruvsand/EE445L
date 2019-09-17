// ADCTestmain.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.
// Daniel Valvano
// May 3, 2015

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
// top of X-ohm potentiometer connected to +3.3V through X/10-ohm ohm resistor


/*

answers the the PREP
1)
List three ways you could use to initiate the ADC conversion process. 
What is the way to know when the conversion process has been completed?  

One way to start ADC conversion is a Software start. This can be done for example when the software writes an 8 (SS3) to the ADC0_PSSI_R to initiate a conversion on sequencer 3
Another way is a Timer triggered start which we implement. It starts when the timer gives out a pulse and this initates the conversion. 
Another way is Analog comparators to see if we get a bigger value that what we set as threshold, triggers the conversion process.
Another way is continous sampling.
Another is PWM but I am not sure how it works.
All information was got from table 14.4 and each mode is selected by setting bits in ADC0_EMUX_R.
Another is taking input from some GPIO port

We will know the conversion is done by looking at the ADC0_RIS_R. 

2)
Why is it is important to connect bypass capacitors across the power pins for the analog IC components? 

In a high-frequency context, the capacitor is a low-impedance path to ground that protects the IC from high-frequency noise on the power line



*/





#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"
#include "SSD2119.h"
#include "UART.h"
#include "calib.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

//debug code
//
// This program periodically samples ADC0 channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.

//extern uint32_t ADCvalue;
extern int size;

//uint32_t ADCvalue2=0;
int main(void){
  PLL_Init(Bus80MHz);                      // 80 MHz system clock
	
	    // Initialize LCD
  LCD_Init();
      UART_Init();              // initialize UART device
	
	
	
	
  SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
  ADC0_InitTimer0ATriggerSeq3(0, 80000); // ADC channel 0, 100 Hz sampling //PE3
  GPIO_PORTF_DIR_R |= 0x04;                // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;             // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;                // enable digital I/O on PF2
                                           // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
  GPIO_PORTF_DATA_R &= ~0x04;              // turn off LED
  EnableInterrupts();
	
	
	UART_OutString("Data: \n");
	int tempData=0;
	while(tempData==0){
	 
		tempData= fifo_full();
	
	}
	for(int i = 0; i < size; i += 1) {
		//int mydata = fifo_pop();
		UART_OutUDec(fifo_Get()[i]);
		//UART_OutUDec(mydata);
		UART_OutString("\n");
	}
	
	
	
	
  while(1){
   // WaitForInterrupt();
 //   GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED
		//over here do the display functions
		
		
LCD_ColorFill(0xFFFF);	
				
				
				
				for(int i=0; i< 320 ; i++){
					int ADCvalue= fifo_pop();
					if(ADCvalue==-1){
						i=i-1;
						continue;
					}
					LCD_SetCursor(0,0);
					LCD_PrintInteger(ADCvalue);
					int temp = Temperature_Convert(ADCvalue);
					LCD_PrintString("\n");
					LCD_PrintInteger(temp/100);
					LCD_PrintString(".");
					LCD_PrintInteger(temp%100);
						unsigned short yval= 220* ADCvalue/ 4095;
						LCD_DrawPixel(i,yval, 0x1234 );
						for(int j=0; j<10000;j++){
							int y=0;
							y++;
							}
				}
					
	
  }
}

