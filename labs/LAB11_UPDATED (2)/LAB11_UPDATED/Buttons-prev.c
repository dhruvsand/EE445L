
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include <stdbool.h>
#include "Buttons.h"




void Buttons_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0); // allow time for clock to start
                                    // 2) no need to unlock PE0-3
  GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x11;      // 4) disable analog function on PE0-3
                                    // 5) no pullup for external switches
  GPIO_PORTE_DIR_R &= ~0x11;        // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x11;      // 6) regular port function
	GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4

  GPIO_PORTE_DEN_R |= 0x11;         // 7) enable digital port

}
// bool debounce[4] = { hours, minutes, arm_alarm, set_alarm };
uint16_t debounce[4];

#define CYCLE_WAIT 5

void Buttons_ReadInput(void) {
	int32_t data = GPIO_PORTE_DATA_R;
	if((data & 0x01) != 0)
		debounce[0] = (data & 0x01)*CYCLE_WAIT;
	if((data & 0x02) != 0)
		debounce[1] = ((data & 0x2) >> 1)*CYCLE_WAIT;
	if((data & 0x04) != 0)
		debounce[2] = ((data & 0x4) >> 2)*CYCLE_WAIT;
	if((data & 0x08) != 0)
		debounce[3] = ((data & 0x8) >> 3)*CYCLE_WAIT;
}

extern bool Play_mode;
void Pause(void);
void Play(void);
void Rewind(void);


void Buttons_Pressed(uint32_t button) {
}
void Buttons_10ms_Handler(void) {
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
}

void Play(void) {
	
}

void Pause(void) {
	
}


void Rewind(void) {
	Pause();

}
