// DAC.h
// Runs on TM4C123

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer2A.h"
#include <math.h>
#include <stdio.h>
#include "Music.h"


extern int time;

/**
 * Initialize the DAC, setting up any necessary ports and pins.
 */
void DAC_Init(uint16_t initialData) {
	SYSCTL_RCGCSSI_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x01;
	while((SYSCTL_PRGPIO_R & 0x01) == 0){};
	GPIO_PORTA_AFSEL_R |= 0x2C;
	GPIO_PORTA_DEN_R |= 0x2C;
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0F00FF) + 0x00202200;
	GPIO_PORTA_AMSEL_R = 0;
	SSI0_CR1_R = 0x0;
	SSI0_CPSR_R = 0x02;
	SSI0_CR0_R &= ~(0x0000FFF0);
	SSI0_CR0_R |= 0x0F;
	SSI0_DR_R = initialData;
	SSI0_CR1_R |= 0x02;
}

//double value=0;

/**
 * Set the DAC output voltage to the specified value.
 * The output is limited to 12 bits so `outputVoltage` will be truncated to 12 bits before setting the output.
 */
void DAC_Output(uint16_t outputVoltage) {
	Note* note= Song_CurrentNote();
	
	int j= note->duration;
	j=j/2;
	
 double	 value  = exp(-(1+j) * time);
	int value_int = outputVoltage *value;
	while((SSI0_SR_R & 0x02) == 0){};
	if (value_int >= 0){
		SSI0_DR_R =  value_int;
}
	//	SSI0_DR_R =  outputVoltage;
		
	}

