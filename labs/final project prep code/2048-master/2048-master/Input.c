#include "ADC.h"
#include "../inc/tm4c123gh6pm.h"
#include "SSD2119.h"

unsigned int ADCMail;    // 12-bit ADC
int prevX = 92;
int prevY = 200;
int arrowReady;

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;				// disable SysTick during setup
	NVIC_ST_RELOAD_R = 2000000;		// initialize reload register for 40Hz
	NVIC_ST_CURRENT_R = 0;			// clear counter
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	NVIC_ST_CTRL_R = 0x07;			// enable with core clock and interrupts	
}

void Button1_Init(void){       
  SYSCTL_RCGC2_R |= 0x00000004; 	// (a) activate clock for port C
  GPIO_PORTC_DIR_R &= ~0x20;    	// (c) make PC5 input
  GPIO_PORTC_AFSEL_R &= ~0x20;  	//     disable alt funct on PC5
  GPIO_PORTC_DEN_R |= 0x20;     	//     enable digital I/O on PC5
  GPIO_PORTC_PCTL_R &= ~0x00F00000; // 	   configure PC5 as GPIO
  GPIO_PORTC_AMSEL_R &= ~0x20;  	//     disable analog functionality on PC5
  GPIO_PORTC_IS_R &= ~0x020;     	// (d) PC5 is edge-sensitive
  GPIO_PORTC_IBE_R &= ~0x20;    	//     PC5 is not both edges
  GPIO_PORTC_IEV_R |= 0x20;   		//     PC5 rising edge event
  GPIO_PORTC_ICR_R = 0x20;     		// (e) clear flag5
  GPIO_PORTC_IM_R |= 0x20;      	// (f) arm interrupt on PC5
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x04;      			// (h) enable interrupt 2 in NVIC
}

void Button2_Init(void){       
  SYSCTL_RCGC2_R |= 0x00000008; 	// (a) activate clock for port D
  GPIO_PORTD_DIR_R &= ~0x40;    	// (c) make PD6 input
  GPIO_PORTD_AFSEL_R &= ~0x40;  	//     disable alt funct on PD6
  GPIO_PORTD_DEN_R |= 0x40;     	//     enable digital I/O on PD6
  GPIO_PORTD_PCTL_R &= ~0x0F000000; //     configure PD6 as GPIO
  GPIO_PORTD_AMSEL_R &= ~0x40;  	//     disable analog functionality on PD6
  GPIO_PORTD_IS_R &= ~0x040;     	// (d) PD6 is edge-sensitive
  GPIO_PORTD_IBE_R &= ~0x40;    	//     PD6 is not both edges
  GPIO_PORTD_IEV_R |= 0x40;    		//     PD6 rising edge event
  GPIO_PORTD_ICR_R = 0x40;      	// (e) clear flag6
  GPIO_PORTD_IM_R |= 0x40;      	// (f) arm interrupt on PD6
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x08;      			// (h) enable interrupt 3 in NVIC
}

void SysTick_Handler(void){
	
	// sample ADC
	ADCMail = ADC_In();

	// set flag
	arrowReady = 1;
	
}

void Input_Init(void){
	ADC_Init();
	SysTick_Init();
	Button1_Init();
	Button2_Init();
}

int getSliderPosition(void){int pos, side;
	pos = ADCMail;
	if (pos < 1024){
		side = 1;
	}
	else if (pos < 2048){
		side = 2;
	}
	else if (pos < 3072){
		side = 3;
	}
	else{
		side = 4;
	}
	return side;
}




