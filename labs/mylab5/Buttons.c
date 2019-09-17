
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include <stdbool.h>
#include "Buttons.h"
#include "Timer1A.h"
#include "Music.h"



int data_instrument=0;

int Get_Instrument(){
	
	return data_instrument;


}

void Buttons_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0); // allow time for clock to start
                                    // 2) no need to unlock PE0-3
  GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 4) disable analog function on PE0-3
                                    // 5) no pullup for external switches
  GPIO_PORTE_DIR_R &= ~0x0F;        // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable digital port

}
// bool debounce[4] = { hours, minutes, arm_alarm, set_alarm };
uint16_t debounce[4];

#define CYCLE_WAIT 5

void Buttons_ReadInput(void) {
	int32_t data = GPIO_PORTE_DATA_R;
	
	if(data==1){
		if(Play_mode)
			Pause();
		else
			Play();
		
		
		return;
	}
	if(data==2){
		Rewind();
		return;
	}
	if(data==4){
				data_instrument++;
		data_instrument%=4;
		return;
	}
	if(data==8){
		
		return;
	}
		
	
}

extern bool Play_mode;
void Pause(void);
void Play(void);
void Rewind(void);


void Buttons_Pressed(uint32_t button) {

}
void Buttons_10ms_Handler(void) {
	Buttons_ReadInput();
}

void Play(void) {
	Play_mode = true;
	NVIC_ST_CTRL_R = 0x07;
	TIMER0_CTL_R = 0x00000001;
	NVIC_ST_RELOAD_R = 500;
}

void Pause(void) {
	Play_mode = false;
	NVIC_ST_CTRL_R = 0;
	TIMER0_CTL_R = 0x00000000;
}

extern const Song song;

void Rewind(void) {
	Pause();
	Song_PlayInit(song);
}
