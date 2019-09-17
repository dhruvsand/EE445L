//this function will initialize the ports needed to initialize the EKG
//It will also initialize the sampling rate at which we want to get the data from the EKG

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ACCEL.h" 
void ACCEL_Init(void){
}


//this will just get the values of the electrodes and the overall output values which we will store in the main function
int ACCEL_Get_Data(void){

return(0);
}

//We could hava an optional function which will basically store the time measuremnts and the output values so we can easily output the graphs which we can then print on the kentex display
void ACCEL_Display_Data(void){
}