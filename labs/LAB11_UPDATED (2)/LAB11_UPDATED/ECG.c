//this function will initialize the ports needed to initialize the EKG
//It will also initialize the sampling rate at which we want to get the data from the EKG

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ECG.h" 
#include "fixed.h"
#include "ST7735.h"
#include "SSD2119.h"
#include "ADCT0ATrigger.h"
#include "Buttons.h"

int new_data;

int Buffer[1000];
int Y[1000];
int X[1000];

int buffer_length = 1000;

int max_data;

int32_t local_maximmum_previous; 
int32_t local_maximmum_current;

void ECG_Init(void){
	ADC0_InitTimer0ATriggerSeq3(9, 80000);//10000Hz sampling on PE4
//	int delay; 
//	int i = 0;
//	//SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate clock for Port E
//	for(i= 0; i< 10; i++){
//		i = i + 0; 
//	}

//  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
//	
//	GPIO_PORTE_DIR_R &= ~0x10;  // 3.9) make PE4 input
//      GPIO_PORTE_AFSEL_R |= 0x10; // 4.9) enable alternate function on PE4
//      GPIO_PORTE_DEN_R &= ~0x10;  // 5.9) disable digital I/O on PE4
//      GPIO_PORTE_AMSEL_R |= 0x10; // 6.9) enable analog functionality on PE4

//  //DisableInterrupts();
//  SYSCTL_RCGCADC_R |= 0x01;     // activate ADC0 
//	delay = SYSCTL_RCGCADC_R;
//	GPIO_PORTF_DATA_R ^= 0x02;
//	ADC0_PC_R = 0x01;         // configure for 125K samples/sec
//  ADC0_SSPRI_R = 0x3210;    // sequencer 0 is highest, sequencer 3 is lowest
//  ADC0_ACTSS_R &= ~0x08;    // disable sample sequencer 3
//  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFF0FFF)+0x5000; // timer trigger event

// 
//  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field

//  ADC0_SSMUX3_R += 9;             //    set channel Ain9 (PE4)

//  ADC0_SAC_R = ADC_SAC_AVG_64X;//Sampling that Dhruv Added
//  ADC0_SSCTL3_R = 0x06;          // set flag and end                       
//  ADC0_IM_R |= 0x08;             // enable SS3 interrupts
//  ADC0_ACTSS_R |= 0x08;          // enable sample sequencer 3
//  NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFF00FF)|0x00004000; //priority 2
//  NVIC_EN0_R = 1<<17;
//	//ADC0_PC_R = 0x01;             // 2) configure for 125K samples/sec

}
//Need an output_indexer to determine where we are in placign the output 
//Need an input_indexer to determine where we are at getting the FIFO
int output_indexer = 0;
int input_indexer= 0 ;

//volatile uint32_t ADCvalue; COmmented out by Dhruv as conflict with ADCtrigger
void ADC0Seq3_Handler_temp(void){//Dhruv Changed the name to temp
	int port_data = 0;
	int i = 0;
	int new_data_copy = 0;
	uint32_t ADCvalue;
	port_data = ADC0_SSFIFO3_R;
	Buffer[input_indexer] = ADC0_SSFIFO3_R;
	input_indexer = (input_indexer +1) % buffer_length;
	
		/*while(new_data_copy != 0){
			new_data_copy--;
			ECG_Display_Data(Buffer[new_data_copy]);
		}*/
	
	
	
  ADC0_ISC_R = 0x08;          // acknowledge ADC sequence 3 completion
  ADCvalue = ADC0_SSFIFO3_R;  // 12-bit result
}



//this will just get the values of the electrodes and the overall output values which we will store in the main function
uint32_t ECG_Get_Data(void){
	int port_data = 0;
	GPIO_PORTF_DATA_R &= ~0x02;              // turn off LED
	int new_data_copy = 0;
	new_data_copy = new_data;
	port_data = GPIO_PORTE_DATA_R;
	if (new_data == 0){return 0;}
	else{
		while(new_data_copy != 0){
			new_data_copy--;
			if (input_indexer - new_data_copy < 0){
				ECG_Display_Data(Buffer[(buffer_length + input_indexer - new_data_copy)]);
			}else{
				ECG_Display_Data(Buffer[(input_indexer - new_data_copy)]);
			}
		}
		return(port_data);
	}
}

//We could hava an optional function which will basically store the time measuremnts and the output values so we can easily output the graphs which we can then print on the kentex display
void ECG_Display_Data(int ECG_input_value){

				int totat_time=1;//increment each time i is increased so we know how many samples have taken place
				int times_decreased=0; // will count the time the value decreased
				int beats_counted=0;//will keep track of all the beats counted
				uint32_t samples_to_deacrease=4;// how many samles should deacrease
				uint32_t min_difference_to_detect_beat=7;//seflexplanatory
				int first_value_captured=0;// this will be what the last value is compared against
	


				
				uint32_t arr_size=20;
				uint32_t arr[20];
				uint32_t arr_start=0;
				uint32_t arr_end=0;
				uint32_t next_arr_end=0;
				uint32_t next_arr_start=0;
				uint32_t cur_size=0;
				
				Buttons_Init();
			
				
				while(1){				
				
				LCD_ColorFill(0xFFFF);	
					
				
				
				unsigned short old_yval=0;
				for(int i=0; i< 320 ; i++){
					int ADCvalue= fifo_pop();
					if(ADCvalue==-1){
						i=i-1;
						continue;
					}
					
					uint32_t yval= 220* ADCvalue/ 4095;
					

					
					
					
					next_arr_end = (arr_end+1)%arr_size;
					next_arr_start = (arr_start+1)%arr_size;
					
					if(cur_size<arr_size){//array is not full
						arr[arr_end]= (uint32_t)yval;
						arr_end= next_arr_end;
						cur_size+=1;
					}
					else{//array is full
						
						uint32_t temp_min=arr[0];
						uint32_t temp_max=arr[0];
						
						for(int j=0; j<arr_size;j++){
								if(arr[j]>temp_max)
									temp_max= arr[j];
								if(arr[j]<temp_min)
									temp_min= arr[j];
						}
						
						if((temp_max-temp_min)>min_difference_to_detect_beat){
								//detected a beat
								beats_counted+=1;
								arr_start=0;
								arr_end=0;//flush the array
								cur_size=0;
								//Zoom(arr,arr_size);
								uint32_t input= Board_Input(); 
								if(input!=0x11){
									input = Board_Input();
									while(input!=0x11){
									input = Board_Input();
									}
									
									while(input!=0x01){
									input = Board_Input();
									}
									while(input!=0x11){
									input = Board_Input();
									}
									
									
									
								}
								
						}
						else{//beat not detected in these 20 values
							arr[arr_end]= (uint32_t)yval;
							arr_end = next_arr_end;
							arr_start= next_arr_start;
						
						}
					}
					
					
					
					
					
					
					
					
					
					
					LCD_SetCursor(0,0);
					LCD_PrintInteger(ADCvalue);
					LCD_PrintString("\n");
					LCD_PrintInteger(beats_counted*25/totat_time);
					LCD_PrintString("\n");
					LCD_PrintInteger(beats_counted);
				//LCD_PrintString("The average BPM:");

						
					
						if(i==0){
							LCD_DrawPixel(i,yval, 0x1234 );
							
						}
						else{
							unsigned short old_i = i-1;
							LCD_DrawLine(old_i,old_yval,i,yval,0x1234);
						}
						
						
//				if(old_yval<yval){
//						if(times_decreased==0){
//							times_decreased=1;
//							first_value_captured=old_yval;
//						}
//						else
//							times_decreased+=1;
//				}
//				else if(old_yval>yval){
//						if(times_decreased>samples_to_deacrease){
//							if((yval-old_yval)>min_difference_to_detect_beat)
//								beats_counted+=1;
//						}
//						times_decreased=0;
//				}
//				
				
				
						
						
				
				old_yval = yval;
						
						
						for(int j=0; j<10000;j++){
							int y=0;
							y++;
							}
				}
				totat_time+=1;
				
			}
				
				
				
				
				
	
}
/*
Since we store EKG points in a buffer, It would be beneficial to as we store them, store the local maximmum. 
Call this variable local max. 
After X milliseconds if a new local max has not been established, store this value as local_max_current, then run BPM_FINDER
ONCE BPM_FINDER IS FINISHED STORE Current into previous. 
*/

/*int32_t ECG_MAX_FIND(void){
	
	return(0);
}*/

/*
This function takes the time index we set as the local index of the previously found local minimmum and uses it by subtracting the current maximmum 

This time interval will then divide the 60 *1000 milliseconds that make up a minute. Thats how we will get BPM. 
Our precision will thus be in milli
*/

uint32_t ECG_GET_BPM(void){
	//Next local max variables shoudl be globals 
	//int32_t local_maximmum_previous; 
	//int32_t local_maximmum_current;
	int32_t BPM;
	
	if(local_maximmum_current >= local_maximmum_previous){
		BPM = (60 * 1000) /(local_maximmum_current - local_maximmum_previous);
	}else{
		BPM = (60 * 1000) /(local_maximmum_previous - local_maximmum_current);
	}
	
	local_maximmum_previous = local_maximmum_current;
	return BPM;
	
	
}