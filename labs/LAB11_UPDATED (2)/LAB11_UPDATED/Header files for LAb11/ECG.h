/**************************************************************************
 *                                                                         *
 * ADXL345 Driver for Arduino                                              *
 *                                                                         *
 ***************************************************************************
 *                                                                         * 
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU License.                                  *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU License V2 for more details.                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef ECG_h
#define ECG_h

//this function will initialize the ports needed to initialize the EKG
//It will also initialize the sampling rate at which we want to get the data from the EKG
void ECG_Init(void);


//this will just get the values of the electrodes and the overall output values which we will store in the main function
uint32_t ECG_Get_Data(void);

//We could hava an optional function which will basically store the time measuremnts and the output values so we can easily output the graphs which we can then print on the kentex display
void ECG_Display_Data(void);

#endif
