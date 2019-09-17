

#include <stdint.h>
#include "PWM.h"

void motor_PWM_Init(uint16_t period, uint16_t duty){
PWM0A_Init( period, duty);
}


void motor_Set_Duty(uint16_t duty){
	PWM0A_Duty(duty);

}

void motor_Off(){
PWM0A_Duty(0);
}