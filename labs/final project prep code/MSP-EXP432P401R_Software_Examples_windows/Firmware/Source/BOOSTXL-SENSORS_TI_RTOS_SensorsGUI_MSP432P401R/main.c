/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// MSP432 TI-RTOS main.c
// Description: Source code for the implementation of the Bosch BoosterPack with
// an MSP432P401R launchpad and TI-RTOS. BoosterPack includes:
// - Inertial Measurement Unit (IMU) sensor with an accelerometer and gyroscope
// - Magnetometer
// - Environmental sensor with pressure, ambient temperature and humidity
// - Ambient light sensor
// - Infrared temperature sensor
//
// Adapted by Michael Arriete
/*
 *  ======== main.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Error.h>


/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateMutexPri.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>

/* Board Header file */
#include "Board.h"
#include "string.h"
#include "stdio.h"
#include "opt3001.h"
#include "tmp007.h"
#include "math.h"
#include "bme280_support.h"
#include "bmi160_support.h"


/* Defines */
#define Board_TMP007_ADDR           (0x40)
#define Board_OPT3001_ADDR			(0x47)

/* Sample time options for updating sensor values */
//Sample times are based off FR6989 running at 16MHz clock
#define SAMPLE_TIME_1				(53)
#define SAMPLE_TIME_2				(26)
#define SAMPLE_TIME_4				(13)
#define SAMPLE_TIME_6				(8)
#define SAMPLE_TIME_8				(6)
#define SAMPLE_TIME_10				(5)
#define NUM_AVGR_SUMS				2 //x^2 frames

/* Global Sample Rates */
const uint8_t timeSamplesBMI [6] = {
		SAMPLE_TIME_1, 		//Sample at 1 time per second
		SAMPLE_TIME_2, 		//Sample at 2 times per second
		SAMPLE_TIME_4, 		//Sample at 4 times per second
		SAMPLE_TIME_6, 		//Sample at 6 times per second
		SAMPLE_TIME_8, 		//Sample at 8 times per second
		SAMPLE_TIME_10, 	//Sample at 10 times per second
};

const uint8_t timeSamplesBMM [6] = {
		SAMPLE_TIME_1, 		//Sample at 1 time per second
		SAMPLE_TIME_2, 		//Sample at 2 times per second
		SAMPLE_TIME_4, 		//Sample at 4 times per second
		SAMPLE_TIME_6, 		//Sample at 6 times per second
		SAMPLE_TIME_8, 		//Sample at 8 times per second
		SAMPLE_TIME_10, 	//Sample at 10 times per second
};

const uint8_t timeSamplesBME [6] = {
		SAMPLE_TIME_1, 		//Sample at 1 time per second
		SAMPLE_TIME_2, 		//Sample at 2 times per second
		SAMPLE_TIME_4, 		//Sample at 4 times per second
		SAMPLE_TIME_6, 		//Sample at 6 times per second
		SAMPLE_TIME_8, 		//Sample at 8 times per second
		SAMPLE_TIME_10, 	//Sample at 10 times per second
};

const uint8_t timeSamplesTMP [6] = {
		SAMPLE_TIME_1, 		//Sample at 1 time per second
		SAMPLE_TIME_2, 		//Sample at 2 times per second
		SAMPLE_TIME_4, 		//Sample at 4 times per second
		SAMPLE_TIME_6, 		//Sample at 6 times per second
		SAMPLE_TIME_8, 		//Sample at 8 times per second
		SAMPLE_TIME_10, 	//Sample at 10 times per second
};

const uint8_t timeSamplesOPT [6] = {
		SAMPLE_TIME_1, 		//Sample at 1 time per second
		SAMPLE_TIME_2, 		//Sample at 2 times per second
		SAMPLE_TIME_4, 		//Sample at 4 times per second
		SAMPLE_TIME_6, 		//Sample at 6 times per second
		SAMPLE_TIME_8, 		//Sample at 8 times per second
		SAMPLE_TIME_10, 	//Sample at 10 times per second
};

//Defaulted sampling rates for each sensor
volatile uint8_t sampleTimePeriodBMI = 2;
volatile uint8_t sampleTimePeriodBMM = 5;
volatile uint8_t sampleTimePeriodBME = 0;
volatile uint8_t sampleTimePeriodTMP = 0;
volatile uint8_t sampleTimePeriodOPT = 0;


/* General Defines */
//RX and TX Buffers for I2C Protocol
uint8_t         txBuffer[10] = {0};
uint8_t         rxBuffer[30] = {0};
//String buffer for snprintf function
char			stringBuffer[295];

//Receive UART Variables
#define NUM_RX_CHARS 64
char rxMsgData[NUM_RX_CHARS] = "";
int numMsgsRx = 0;
int tempIndex = 5;
int numChars = 0;

//Calibration off-sets
int8_t accel_off_x;
int8_t accel_off_y;
int8_t accel_off_z;
int16_t gyro_off_x;
int16_t gyro_off_y;
int16_t gyro_off_z;

// Task stack size setup
#define TMP_TASK_STACK_SIZE  				512
#define OPT_TASK_STACK_SIZE	 				512
#define BME_TASK_STACK_SIZE	 				576
#define BMI_TASK_STACK_SIZE  				512
#define BMM_TASK_STACK_SIZE  				512
#define INIT_TASK_STACK_SIZE 				600
#define UART_SEND_TASK_STACK_SIZE 			2048
#define UART_RECEIVE_TASK_STACK_SIZE 		1024
#define MAIN_THREAD_TASK_STACK_SIZE			512

static Char TMPTaskStack[TMP_TASK_STACK_SIZE];
static Char OPTTaskStack[OPT_TASK_STACK_SIZE];
static Char BMETaskStack[BME_TASK_STACK_SIZE];
static Char BMITaskStack[BMI_TASK_STACK_SIZE];
static Char BMMTaskStack[BMM_TASK_STACK_SIZE];
static Char INITTaskStack[INIT_TASK_STACK_SIZE];
static Char UartSendTaskStack[UART_SEND_TASK_STACK_SIZE];
static Char UartReceiveTaskStack[UART_RECEIVE_TASK_STACK_SIZE];
static Char MainThreadTaskStack[MAIN_THREAD_TASK_STACK_SIZE];



/* Global Variables */
Task_Handle initSensors, readTMP, readOPT, readBME, readBMI, readBMM;
Task_Handle uartReceive, uartSend, mainThread;
Task_Params taskParams;

Semaphore_Handle BMESem, BMISem, BMMSem, OPTSem, TMPSem, receiveSem, uartSem;

GateMutexPri_Handle i2cGate;
GateMutexPri_Params gateParams;
IArg gatekey;

Error_Block eb;

I2C_Handle      i2c;
I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;

UART_Handle 	uart;
UART_Params 	uartParams;
UART_Callback	uartCallback;

// TMP007
uint16_t rawTemp;
uint16_t rawObjTemp;
float    tObjTemp;
float    tObjAmb;

// OPT3001
uint16_t rawData;
float    convertedLux;

// BME280
s32 			g_s32ActualTemp   = 0;
u32 			g_u32ActualPress  = 0;
u32	 			g_u32ActualHumity = 0;

// BMI160/BMM150
struct bmi160_gyro_t        s_gyroXYZ;
struct bmi160_accel_t       s_accelXYZ;
struct bmi160_mag_xyz_s32_t s_magcompXYZ;

//Timer Counter
uint16_t WDTcount = 0;

//gesture recognition
int getGestures = 1;
int dominant = 0;
uint16_t gyroAbsX, gyroAbsY, gyroAbsZ;
uint16_t deltaAccelX, deltaAccelY, deltaAccelZ;
int16_t prevAccelX = 0;
int16_t prevAccelY = 0;
int16_t prevAccelZ = 0;
int16_t prevGyroX = 0;
int16_t prevGyroY = 0;
int16_t prevGyroZ = 0;
int16_t stillCount = 0;
int32_t gyroAvgX = 0.0;
int32_t gyroAvgY = 0.0;
int32_t gyroAvgZ = 0.0;
int32_t accelAvgX = 0.0;
int32_t accelAvgY = 0.0;
int32_t accelAvgZ = 0.0;

//Sensor Status Variables
bool BME_on = true;
bool BMI_on = true;
bool TMP_on = true;
bool OPT_on = true;

//***** Function Prototypes *****
int32_t movingAvg(int prevAvg, int16_t newValue);

/***********************************************************
  Funtion:

   Main thread handler for system. Determines when to update
   sensor values using a counter, what gesture should be displayed
   in the GUI, and when to send over UART.

 ***********************************************************/
void mainThreadHandler(UArg arg0, UArg arg1)
{
	while(1) {

		//TMP update frequency
		if ((WDTcount%timeSamplesTMP[sampleTimePeriodTMP] == 0) && TMP_on)
		{
			// Read/convert tmp007 and opt3001 data
			Semaphore_post(TMPSem);
		}

		//OPT update frequency
		if ((WDTcount%timeSamplesOPT[sampleTimePeriodOPT] == 0) && OPT_on) {
			Semaphore_post(OPTSem);
		}

		//BME update frequency
		if ((WDTcount%timeSamplesBME[sampleTimePeriodBME] == 0) && BME_on)
		{
			//Read BME environmental data
			Semaphore_post(BMESem);
		}

		//Accel and Gyro update frequency
		if ((WDTcount%timeSamplesBMI[sampleTimePeriodBMI] == 0) && BMI_on)
		{
			//Read Accel and Gyro values
			Semaphore_post(BMISem);
		}

		//Mag update frequency
		if ((WDTcount%timeSamplesBMM[sampleTimePeriodBMM] == 0) && BMI_on)
		{
			//Read Mag value (BMM) through BMI
			Semaphore_post(BMMSem);
		}

		if(((WDTcount%timeSamplesBME[sampleTimePeriodBME] == 0) && BME_on) ||
				((WDTcount%timeSamplesBMM[sampleTimePeriodBMM] == 0) && BMI_on) ||
				((WDTcount%timeSamplesBMI[sampleTimePeriodBMI] == 0) && BMI_on) ||
				((WDTcount%timeSamplesTMP[sampleTimePeriodTMP] == 0) && TMP_on) ||
				((WDTcount%timeSamplesOPT[sampleTimePeriodOPT] == 0) && OPT_on))
		{
			if (getGestures)
			{

				gyroAbsX = abs(s_gyroXYZ.x);
				gyroAbsY = abs(s_gyroXYZ.y);
				gyroAbsZ = abs(s_gyroXYZ.z);

				gyroAvgX = movingAvg(gyroAvgX, gyroAbsX );
				gyroAvgY = movingAvg(gyroAvgY, gyroAbsY );
				gyroAvgZ = movingAvg(gyroAvgZ, gyroAbsZ );



				if( gyroAvgX > 1000 || gyroAvgY > 1000 || gyroAvgZ > 1000 )
				{
					stillCount = 0;
					if ( gyroAvgY > gyroAvgX )
					{
						if ( gyroAvgY > gyroAvgZ )
						{
							dominant = 2; //gyro Y
						}
						else
						{
							dominant = 3; //gyro Z
						}

					}
					else if ( gyroAvgZ > gyroAvgX )
					{
						dominant = 3; //gyro Z
					}
					else
					{
						dominant = 1; //gyro X
					}
				}
				else{

					deltaAccelX = abs(s_accelXYZ.x - prevAccelX);
					deltaAccelY = abs(s_accelXYZ.y - prevAccelY);
					deltaAccelZ = abs(s_accelXYZ.z - prevAccelZ);

					accelAvgX = movingAvg(accelAvgX, deltaAccelX);
					accelAvgY = movingAvg(accelAvgY, deltaAccelY);
					accelAvgZ = movingAvg(accelAvgZ, deltaAccelZ);


					if ( accelAvgX > 100 || accelAvgY > 100 || accelAvgZ > 100 )
					{
						stillCount = 0;

						if ( accelAvgY > accelAvgX )
						{
							if ( accelAvgY > accelAvgZ )
							{
								dominant = 5; //accel Y
							}
							else
							{
								dominant = 6; //accel Z
							}

						}
						else if ( accelAvgZ > accelAvgX )
						{
							dominant = 6; //accel Z
						}
						else
						{
							dominant = 4; //accel X
						}

					}
					else{
						stillCount++;
						if (stillCount>3)
						{
							dominant = 0; //still condition
						}

					}

				}

				prevAccelX = s_accelXYZ.x;
				prevAccelY = s_accelXYZ.y;
				prevAccelZ = s_accelXYZ.z;
			}
			Semaphore_post(uartSem);
		}

		//WDTcount increment and reset
		if (WDTcount == 53) {
			//Reset WDT counter after reaching roughly one second
			WDTcount = 0;
		}
		WDTcount++;
		Semaphore_post(receiveSem);

		Task_sleep(20);
	}
}

/***********************************************************
  Funtion:

   Task that posts and pends when receiving a transmission over
   UART. Will change sampling rates or sensor status depending
   on value and identifier received.

 ***********************************************************/
void uart_Receive (UArg arg0, UArg arg1) {
	while(1) {
		Semaphore_pend(receiveSem, BIOS_WAIT_FOREVER);
		if (UART_read(uart,rxMsgData,NUM_RX_CHARS) > 0){
			numMsgsRx++;
			numChars = strlen(rxMsgData);

			if (numChars > 6) {
				switch(rxMsgData[2]){
				case 's':  // e.g. {"s":16} or {"s":8} or {"s":4) to set sampling rates of 16kHz, 8kHz or 4kHz
					switch(rxMsgData[3]){
					case 'i':
						tempIndex = 6;
						if (rxMsgData[tempIndex] == '"'){
							tempIndex = 7;  // this accommodates values that have been converted to strings e.g. from the droplist's selectedValue
						}
						switch (rxMsgData[tempIndex]){
						case '0':
							sampleTimePeriodBMI = 0;
							break;
						case '1':
							sampleTimePeriodBMI = 1;
							break;
						case '2':
							sampleTimePeriodBMI = 2;
							break;
						case '3':
							sampleTimePeriodBMI = 3;
							break;
						case '4':
							sampleTimePeriodBMI = 4;
							break;
						case '5':
							sampleTimePeriodBMI = 5;
							break;
						}
						break;
						case 'm':
							tempIndex = 6;
							if (rxMsgData[tempIndex] == '"'){
								tempIndex = 7;  // this accommodates values that have been converted to strings e.g. from the droplist's selectedValue
							}
							switch (rxMsgData[tempIndex]){
							case '0':
								sampleTimePeriodBMM = 0;
								break;
							case '1':
								sampleTimePeriodBMM = 1;
								break;
							case '2':
								sampleTimePeriodBMM = 2;
								break;
							case '3':
								sampleTimePeriodBMM = 3;
								break;
							case '4':
								sampleTimePeriodBMM = 4;
								break;
							case '5':
								sampleTimePeriodBMM = 5;
								break;
							}
							break;
							case 't':
								tempIndex = 6;
								if (rxMsgData[tempIndex] == '"'){
									tempIndex = 7;  // this accommodates values that have been converted to strings e.g. from the droplist's selectedValue
								}
								switch (rxMsgData[tempIndex]){
								case '0':
									sampleTimePeriodTMP = 0;
									break;
								case '1':
									sampleTimePeriodTMP = 1;
									break;
								case '2':
									sampleTimePeriodTMP = 2;
									break;
								case '3':
									sampleTimePeriodTMP = 3;
									break;
								case '4':
									sampleTimePeriodTMP = 4;
									break;
								case '5':
									sampleTimePeriodTMP = 5;
									break;
								}
								break;
								case 'o':
									tempIndex = 6;
									if (rxMsgData[tempIndex] == '"'){
										tempIndex = 7;  // this accommodates values that have been converted to strings e.g. from the droplist's selectedValue
									}
									switch (rxMsgData[tempIndex]){
									case '0':
										sampleTimePeriodOPT = 0;
										break;
									case '1':
										sampleTimePeriodOPT = 1;
										break;
									case '2':
										sampleTimePeriodOPT = 2;
										break;
									case '3':
										sampleTimePeriodOPT = 3;
										break;
									case '4':
										sampleTimePeriodOPT = 4;
										break;
									case '5':
										sampleTimePeriodOPT = 5;
										break;
									}
									break;
									case 'e':
										tempIndex = 6;
										if (rxMsgData[tempIndex] == '"'){
											tempIndex = 7;  // this accommodates values that have been converted to strings e.g. from the droplist's selectedValue
										}
										switch (rxMsgData[tempIndex]){
										case '0':
											sampleTimePeriodBME = 0;
											break;
										case '1':
											sampleTimePeriodBME = 1;
											break;
										case '2':
											sampleTimePeriodBME = 2;
											break;
										case '3':
											sampleTimePeriodBME = 3;
											break;
										case '4':
											sampleTimePeriodBME = 4;
											break;
										case '5':
											sampleTimePeriodBME = 5;
											break;
										}
										break;
					}
					break;
					case 'i': //e.g.: {"a":true} or {"a":false}
						if (rxMsgData[5] == '1'){
							bmi160_config_running_mode(APPLICATION_NAVIGATION);
							BMI_on = true;
						} else {
							bmi160_config_running_mode(ACCEL_PEDOMETER);
							BMI_on = false;
						}
						break;
					case 't': //e.g.: {"g":0} or {"g":1}
						if (rxMsgData[5] == '1'){
							sensorTmp007Enable(true);
							TMP_on = true;
						} else {
							sensorTmp007Enable(false);
							TMP_on = false;
						}
						break;
					case 'o': //e.g.: {"g":0} or {"g":1}
						if (rxMsgData[5] == '1'){
							sensorOpt3001Enable(true);
							OPT_on = true;
						} else {
							sensorOpt3001Enable(false);
							OPT_on = false;
						}
						break;
					case 'e': //e.g.: {"g":0} or {"g":1}
						if (rxMsgData[5] == '1'){
							bme280_set_power_mode(BME280_NORMAL_MODE);
							BME_on = true;
						} else {
							bme280_set_power_mode(BME280_SLEEP_MODE);
							BME_on = false;
						}
						break;
					case 'r':
						if (rxMsgData[5] == '1'){
							getGestures = 1;
						} else {
							getGestures = 0;
							dominant = 0;
						}
						break;
				}
			}
		}
	}
}

/***********************************************************
  Funtion:

   Task handling the reading of the TMP007 sensor.

 ***********************************************************/
void taskTMP(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(TMPSem, BIOS_WAIT_FOREVER);

		gatekey = GateMutexPri_enter(i2cGate);

		// Read/convert tmp007
		if (sensorTmp007Read(&rawTemp, &rawObjTemp)) {
			GateMutexPri_leave(i2cGate, gatekey);

			sensorTmp007Convert(rawTemp, rawObjTemp, &tObjTemp, &tObjAmb);

			System_printf("Sample TMP: %d (C)\n", tObjTemp);
		}
		else {
			System_printf("I2C Bus fault\n");
			GateMutexPri_leave(i2cGate, gatekey);
		}

		System_flush();
	}
}
/***********************************************************
  Funtion:

   Task handling the reading of the OPT3001 sensor.

 ***********************************************************/
void taskOPT(UArg arg0, UArg arg1) {


	while(1) {
		Semaphore_pend(OPTSem, BIOS_WAIT_FOREVER);

		gatekey = GateMutexPri_enter(i2cGate);

		//Read and convert OPT values
		if (sensorOpt3001Read(&rawData)) {
			GateMutexPri_leave(i2cGate, gatekey);
			sensorOpt3001Convert(rawData, &convertedLux);

			System_printf("Sample OPT: %d \n", convertedLux);
		}
		else {
			System_printf("I2C Bus fault\n");
			GateMutexPri_leave(i2cGate, gatekey);
		}
		System_flush();
	}
}

/***********************************************************
  Funtion:

   Task handling the reading of the BME280 sensor.

 ***********************************************************/
void taskBME(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(BMESem, BIOS_WAIT_FOREVER);
		gatekey = GateMutexPri_enter(i2cGate);
		bme280_read_pressure_temperature_humidity(
				&g_u32ActualPress, &g_s32ActualTemp, &g_u32ActualHumity);
		GateMutexPri_leave(i2cGate, gatekey);

		System_printf("Sample BME: %d (P), %d (T), %d (H)\n", g_u32ActualPress, g_s32ActualTemp, g_u32ActualHumity);
		System_flush();
	}
}

/***********************************************************
  Funtion:

   Task handling the reading of the BMI160 sensor.

 ***********************************************************/
void taskBMI(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(BMISem, BIOS_WAIT_FOREVER);
		//Read Accel and Gyro values
		gatekey = GateMutexPri_enter(i2cGate);
		bmi160_read_accel_xyz(&s_accelXYZ);
		bmi160_read_gyro_xyz(&s_gyroXYZ);
		GateMutexPri_leave(i2cGate, gatekey);

		System_printf("Sample BMI Accel: %d (X), %d (Y), %d (Z)\n"
				"Sample BMI Gyro: %d (X), %d (Y), %d (Z)\n",
				s_accelXYZ.x, s_accelXYZ.y, s_accelXYZ.z,
				s_gyroXYZ.x, s_gyroXYZ.y, s_gyroXYZ.z);
		System_flush();
	}
}

/***********************************************************
  Funtion:

   Task handling the reading of the BMM150 sensor.

 ***********************************************************/
void taskBMM(UArg arg0, UArg arg1)
{
	while(1) {
		Semaphore_pend(BMMSem, BIOS_WAIT_FOREVER);
		//Read Magnetometer values
		gatekey = GateMutexPri_enter(i2cGate);
		bmi160_bmm150_mag_compensate_xyz(&s_magcompXYZ);
		GateMutexPri_leave(i2cGate, gatekey);

		System_printf("Sample BMI Mag: %d (X), %d (Y), %d (Z)\n",
				s_magcompXYZ.x, s_magcompXYZ.y, s_magcompXYZ.z);
		System_flush();
	}
}

/***********************************************************
  Funtion:

   Task handling the initializations of I2C and UART peripherals,
   activating sensors, and changing all task priorities.


 ***********************************************************/
void taskInit(UArg arg0, UArg arg1)
{

	while(1) {
		/* Initialize I2C */
		I2C_Params_init(&i2cParams);
		i2cParams.bitRate = I2C_400kHz;
		i2cParams.transferMode = I2C_MODE_BLOCKING;
		i2cParams.transferCallbackFxn = NULL;
		i2c = I2C_open(Board_I2C0, &i2cParams);
		if (i2c == NULL) {
			System_abort("Error Initializing I2C\n");
		}
		else {
			System_printf("I2C Initialized!\n");
		}
		System_flush();

		/* Initialize UART */
		UART_Params_init(&uartParams);
		uartParams.writeDataMode = UART_DATA_BINARY;
		uartParams.readMode = UART_MODE_BLOCKING;
		uartParams.readDataMode = UART_DATA_TEXT;
		uartParams.readReturnMode = UART_RETURN_NEWLINE;
		uartParams.readEcho = UART_ECHO_OFF;
		uartParams.baudRate = 115200;
		uart = UART_open(Board_UART0, &uartParams);
		if(uart == NULL) {
			System_abort("UART open failed...\n");
		}
		else {
			System_printf("UART Initialized!\n");
		}
		System_flush();

		/* Initialize the TMP Sensor */
		sensorTmp007Init();
		sensorTmp007Enable(true);

		/* Initialize the OPT Sensor */
		sensorOpt3001Init();
		sensorOpt3001Enable(true);

		/* Initialize the BME Sensor */
		bme280_data_readout_template();
		bme280_set_power_mode(BME280_NORMAL_MODE);

		/* Initialize the BMI Sensor */
		bmi160_initialize_sensor();
		bmi160_config_running_mode(APPLICATION_NAVIGATION);
		bmi160_accel_foc_trigger_xyz(0x03, 0x03, 0x01, &accel_off_x, &accel_off_y, &accel_off_z);
		bmi160_set_foc_gyro_enable(0x01, &gyro_off_x, &gyro_off_y, &gyro_off_z);

		Task_setPri(readTMP, 2);
		Task_setPri(readOPT, 2);
		Task_setPri(readBME, 2);
		Task_setPri(readBMI, 2);
		Task_setPri(readBMM, 2);
		Task_setPri(uartSend, 2);
		Task_setPri(uartReceive, 1);
		Task_setPri(mainThread, 1);
		Task_setPri(initSensors, -1);
	}
}

/***********************************************************
  Funtion:

   Task handling the formatting of JSON string to be sent over
   UART to the GUI.

 ***********************************************************/
Void uartFxn(UArg arg0, UArg arg1)
{
	while (1) {
		Semaphore_pend(uartSem, BIOS_WAIT_FOREVER);
		snprintf(stringBuffer, 295,
				"{\"gyro\":{\"x\":%d,\"y\":%d,\"z\":%d},"
				"\"accel\":{\"x\":%d,\"y\":%d,\"z\":%d},"
				"\"mag\":{\"x\":%d,\"y\":%d,\"z\":%d},"
				"\"enviro\":{\"humid\":%d,\"press\":%d,\"amb_temp\":%d},"
				"\"ir_temp\":{\"obj_temp\":%5.2f},"
				"\"light\":{\"lux\":%5.2f},"
				"\"dominant\":{\"code\":%d}}\n",
				s_gyroXYZ.x, s_gyroXYZ.y, s_gyroXYZ.z,
				s_accelXYZ.x, s_accelXYZ.y, s_accelXYZ.z,
				s_magcompXYZ.x, s_magcompXYZ.y, s_magcompXYZ.z,
				g_u32ActualHumity, g_u32ActualPress, g_s32ActualTemp,
				tObjTemp, convertedLux, dominant);
		gatekey = GateMutexPri_enter(i2cGate);
		UART_write(uart, stringBuffer, strlen(stringBuffer));
		GateMutexPri_leave(i2cGate, gatekey);
	}
}

/***********************************************************
  Funtion:

   Works as a simple moving averager. Used for gesture recognition.

 ***********************************************************/
int32_t movingAvg(int prevAvg, int16_t newValue)
{
	return (((prevAvg << NUM_AVGR_SUMS) + newValue - prevAvg) >> NUM_AVGR_SUMS);
}

/***********************************************************
  Funtion:

   Function called before BIOS_Start() to dynamically initialize
   and allocate the proper stack sizes for each task.

 ***********************************************************/
void AP_createTask(void)
{
	// Configure tasks
	Task_Params_init(&taskParams);
	taskParams.stack = TMPTaskStack;
	taskParams.stackSize = TMP_TASK_STACK_SIZE;
	taskParams.priority = -1;
	readTMP = Task_create(taskTMP, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = OPTTaskStack;
	taskParams.stackSize = OPT_TASK_STACK_SIZE;
	taskParams.priority = -1;
	readOPT = Task_create(taskOPT, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = BMETaskStack;
	taskParams.stackSize = BME_TASK_STACK_SIZE;
	taskParams.priority = -1;
	readBME = Task_create(taskBME, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = BMITaskStack;
	taskParams.stackSize = BMI_TASK_STACK_SIZE;
	taskParams.priority = -1;
	readBMI = Task_create(taskBMI, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = BMMTaskStack;
	taskParams.stackSize = BMM_TASK_STACK_SIZE;
	taskParams.priority = -1;
	readBMM = Task_create(taskBMM, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = UartSendTaskStack;
	taskParams.stackSize = UART_SEND_TASK_STACK_SIZE;
	taskParams.priority = -1;
	uartSend = Task_create(uartFxn, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = UartReceiveTaskStack;
	taskParams.stackSize = UART_RECEIVE_TASK_STACK_SIZE;
	taskParams.priority = -1;
	uartReceive = Task_create(uart_Receive, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = MainThreadTaskStack;
	taskParams.stackSize = MAIN_THREAD_TASK_STACK_SIZE;
	taskParams.priority = -1;
	mainThread = Task_create(mainThreadHandler, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = INITTaskStack;
	taskParams.stackSize = INIT_TASK_STACK_SIZE;
	taskParams.priority = 1;
	initSensors = Task_create(taskInit, &taskParams, NULL);
}

/***********************************************************
  Funtion:

   Function called before BIOS_Start() to dynamically initialize
   each semaphore.

 ***********************************************************/
void AP_createSemaphore(void) {
	BMESem = Semaphore_create(0, NULL, &eb);
	if(BMESem == NULL) {
		System_abort("BME semaphore create failed");
	}

	BMISem = Semaphore_create(0, NULL, &eb);
	if(BMISem == NULL) {
		System_abort("BMI semaphore create failed");
	}

	BMMSem = Semaphore_create(0, NULL, &eb);
	if(BMMSem == NULL) {
		System_abort("BMM semaphore create failed");
	}

	OPTSem = Semaphore_create(0, NULL, &eb);
	if(OPTSem == NULL) {
		System_abort("OPT semaphore create failed");
	}

	TMPSem = Semaphore_create(0, NULL, &eb);
	if(TMPSem == NULL) {
		System_abort("TMP semaphore create failed");
	}

	receiveSem = Semaphore_create(0, NULL, &eb);
	if(receiveSem == NULL) {
		System_abort("Receive semaphore create failed");
	}

	uartSem = Semaphore_create(0, NULL, &eb);
	if(uartSem == NULL) {
		System_abort("UART semaphore create failed");
	}

}

/***********************************************************
  Funtion:

   Function called before BIOS_start() to create the Gate
   Mutex used for the isolation of the I2C and UART peripherals.

 ***********************************************************/
void AP_createGateMutex(void) {
	GateMutexPri_Params_init(&gateParams);

	i2cGate = GateMutexPri_create(&gateParams, &eb);
	if(i2cGate == NULL) {
		System_abort("Gate create failed");
	}
}

/*
 *  ======== main ========
 */
int main(void)
{
	/* Call board init functions */
	Board_initGeneral();
	Board_initGPIO();
	Board_initI2C();
	Board_initUART();

	/* Turn on user LED */
	GPIO_write(Board_LED0, Board_LED_ON);

	System_printf("Starting the example\nSystem provider is set to SysMin. "
			"Halt the target to view any SysMin contents in ROV.\n");
	/* SysMin will only print to the console when you call flush or exit */
	System_flush();

	Error_init(&eb);

	AP_createTask();
	AP_createSemaphore();
	AP_createGateMutex();

	System_flush();
	/* Start BIOS */
	BIOS_start();

	return (0);
}
