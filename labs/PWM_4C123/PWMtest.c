// PWMtest.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2015
   Program 8.4, Section 8.3

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
#include "PLL.h"
#include "PWM.h"
#include "Tach.h"
#include "SysTick.h"
#include "SSD2119.h"
#include "motor.h"
#include "Timer0A.h"
#include "switch.h"
void WaitForInterrupt(void);  // low power mode
uint32_t Target;
int main(void){
	Target =50;
	DisableInterrupts();
  PLL_Init(Bus80MHz);               // bus clock at 80 MHz
   LCD_Init();// inits the LCD
  PWM0B_Init(40000, 30000);         // initialize PWM0, 1000 Hz, 25% duty//pB7
	//this inistializes the PWM on PB7 
 Board_Init();
  Tach_Init();								  	  // initialize the Tach measureing input capture//PB6

	
	
	
	//uint32_t Board_Input(void)
	// Output: 0x01 if only Switch 1 is pressed
//         0x10 if only Switch 2 is pressed
//         0x00 if both switches are pressed
//         0x11 if no switches are pressed

	EnableInterrupts();
  LCD_SetCursor(0,0);
	LCD_PrintString("Current Speed: \n" );
	LCD_PrintString("Target Speed: \n");
	//while(1){}
	while(1) {
			LCD_ColorFill(0);
		for(int i=0; i< 320 ; i++){
			uint32_t input= Board_Input(); 
			
			if(input==0x01){
				Target+=10;
				Target= Target%110;
				Target+=10;
//				while((Board_Input()==input )||(Board_Input()==0x00)){
//				
//				
//				}
				for(int j=0; j<1000000;j++){
							int y=0;
							y++;
							}
				
			}
			Tach_Set_Speed(Target);
					
					LCD_SetCursor(0,0);
			uint32_t speed = Tach_GetSpeed();
			LCD_PrintString("Current Speed: ");
			LCD_PrintInteger(speed);
			LCD_PrintString("\n");
		LCD_PrintString("Target Speed: ");
			LCD_PrintInteger(Target);
			LCD_PrintString("\n");
					
			double yval1= (double)speed*240/100;
			double yval2= (double)Target*240/100;		
						LCD_DrawPixel(i,yval1, 0x1234 );//for speed
						LCD_DrawPixel(i,yval2, 0x4321 );//for speed
			
						for(int j=0; j<10000;j++){
							int y=0;
							y++;
							}
				}
		
		
		
	}
}


