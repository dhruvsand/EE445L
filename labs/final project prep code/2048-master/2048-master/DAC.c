#include "tm4c123gh6pm.h"

// ----------------DAC_Init------------------
// Initialize 6-bit DAC, called once, PE0-PE5
// Input: none
// Output: none
void DAC_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;				// activate port E
	delay = SYSCTL_RCGC2_R;				// let clock stabilize
	GPIO_PORTE_AMSEL_R &= ~0x3F;		// disable analog
	GPIO_PORTE_PCTL_R &= ~0x0FFFFFF;	// regular function
	GPIO_PORTE_DIR_R |= 0x3F;			// make PE0-PE5 output
	GPIO_PORTE_AFSEL_R &= ~0x3F;		// disable alt function
	GPIO_PORTE_DEN_R |= 0x3F;			// enable digital I/O
	GPIO_PORTE_DR8R_R |= 0x3F;
}

// -----------------DAC_Out------------------
// Output to DAC
// Input: 6-bit data, 0 to 63 
// Output: none
void DAC_Out(unsigned long data){
	GPIO_PORTE_DATA_R = data;			// output data to DAC
}
