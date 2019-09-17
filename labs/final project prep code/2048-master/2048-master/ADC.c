// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Ryan Norton

#include "../inc/tm4c123gh6pm.h"

// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PD2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |=	0x08;		// activate clock for port D
	delay = SYSCTL_RCGC2_R;			// let clock stabilize
	GPIO_PORTD_DIR_R &= ~0x04;		// make PD2 input
	GPIO_PORTD_AFSEL_R |=	0x04;	// enable alternate function PD2
	GPIO_PORTD_DEN_R &=	~0x04;		// disable digital I/O PD2
	GPIO_PORTD_AMSEL_R |=	0x04;	// enable analog PD2
	
	SYSCTL_RCGC0_R |=	0x10000;	// activate ADC0
	delay = SYSCTL_RCGC2_R;			// let clock stabilize	
	SYSCTL_RCGC0_R &= ~0x0300;		// configure for 125K
	ADC0_SSPRI_R = 0x0123;			// sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;		// disable sample sequencer 3
	ADC0_EMUX_R	&= ~0xF000;			// seq3 is software trigger
	ADC0_SSMUX3_R &= ~0x0F;			// clear SS3 field
	ADC0_SSMUX3_R += 5;				// set channel Ain5 (PD2)
	ADC0_SSCTL3_R = 0x0006;			// no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008;			// disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008;			// enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
int ADC_In(void){ int result;
  ADC0_PSSI_R = 0x0008;					// initiate SS3 
	while ((ADC0_RIS_R & 0x08) == 0) {}	// wait for conversion done
	result = ADC0_SSFIFO3_R & 0x0FFF;	// read 12-bit result	
	ADC0_ISC_R = 0x0008;				// acknowledge completion
	return result;
}
