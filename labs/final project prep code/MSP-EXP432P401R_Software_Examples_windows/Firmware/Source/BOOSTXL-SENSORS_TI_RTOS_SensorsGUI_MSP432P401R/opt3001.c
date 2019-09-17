/**************************************************************************************************
 *  Filename:       opt3001.c
 *  Revised:        
 *  Revision:       
 *
 *  Description:    Driver for the Texas Instruments OP3001 Optical Sensor
 *
 *  Copyright (C) 2014 - 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "msp432.h"
#include "driverlib.h"
 #include <ti/drivers/I2C.h>
#include "opt3001.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

/* Slave address */
#define OPT3001_I2C_ADDRESS             0x47

/* Register addresses */
#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOW_LIMIT                   0x02
#define REG_HIGH_LIMIT                  0x03

#define REG_MANUFACTURER_ID             0x7E
#define REG_DEVICE_ID                   0x7F

/* Register values */
#define MANUFACTURER_ID                 0x5449  // TI
#define DEVICE_ID                       0x3001  // Opt 3001
#define CONFIG_RESET                    0xC810                   
#define CONFIG_TEST                     0xCC10
#define CONFIG_ENABLE                   0x10C4 // 0xC410   - 100 ms, continuous               
#define CONFIG_DISABLE                  0x10C0 // 0xC010   - 100 ms, shutdown

/* Bit values */
#define DATA_RDY_BIT                    0x0080  // Data ready

/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_LENGTH                     2

/* ------------------------------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------------------------
 */
bool optWriteI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);
bool optReadI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Public functions
 * -------------------------------------------------------------------------------------------------
 */


/**************************************************************************************************
 * @fn          sensorOpt3001Init
 *
 * @brief       Initialize the temperature sensor driver
 *
 * @return      none
 **************************************************************************************************/
bool sensorOpt3001Init(void)
{
	sensorOpt3001Enable(false);

	return (true);
}


/**************************************************************************************************
 * @fn          sensorOpt3001Enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Enable(bool enable)
{
	uint16_t val;

	if (enable)
	{
		val = CONFIG_ENABLE;
	}
	else
	{
		val = CONFIG_DISABLE;
	}

	optWriteI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t*)&val, REGISTER_LENGTH);
}


/**************************************************************************************************
 * @fn          sensorOpt3001Read
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      TRUE if valid data
 **************************************************************************************************/
bool sensorOpt3001Read(uint16_t *rawData)
{
	bool success;
	uint16_t val;

	success = optReadI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)&val,
			REGISTER_LENGTH); 

	if (success)
	{
		success = (val & DATA_RDY_BIT) == DATA_RDY_BIT;
	}

	if (success)
	{
		success = optReadI2C(OPT3001_I2C_ADDRESS, REG_RESULT, (uint8_t *)&val,
				REGISTER_LENGTH); 
	}

	if (success)
	{
		// Swap bytes
		*rawData = (val << 8) | (val>>8 &0xFF);
	} 
	else
	{
		//	  sensorSetErrorData
	}

	return (success);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 **************************************************************************************************/
bool sensorOpt3001Test(void)
{
	uint16_t val;
	
	// Check manufacturer ID
	optReadI2C(OPT3001_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val,
			REGISTER_LENGTH);
	val = (val << 8) | (val>>8 &0xFF);

	if (val != MANUFACTURER_ID)
	{
		return (false);
	}
	// Check device ID
	optReadI2C(OPT3001_I2C_ADDRESS, REG_DEVICE_ID, (uint8_t *)&val,
			REGISTER_LENGTH);
	val = (val << 8) | (val>>8 &0xFF);

	if (val != DEVICE_ID)
	{
		return (false);
	}

	return (true);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawData - raw data from sensor
 *
 * @param       convertedLux - converted value (lux)
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Convert(uint16_t rawData, float *convertedLux)
{
	uint16_t e, m;

	m = rawData & 0x0FFF;
	e = (rawData & 0xF000) >> 12;

	*convertedLux = m * (0.01 * exp2(e));
}

/***********************************************************
  Function:
*/
bool optWriteI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount)
{
	int i = 1;
	bool result = false;
	//Fill TX Buffer with the register address and subsequent data bytes
	txBuffer[0] = ui8Reg;
	for(i=1; i<ui8ByteCount+1;i++)
		txBuffer[i] = Data[i-1];

	//Point TI-RTOS I2C parameters to correct values
	i2cTransaction.slaveAddress = ui8Addr;
	i2cTransaction.writeBuf = txBuffer;
	i2cTransaction.writeCount = ui8ByteCount+1;
	i2cTransaction.readBuf = rxBuffer;
	i2cTransaction.readCount = 0;
	//Perform I2C write
	result = I2C_transfer(i2c, &i2cTransaction);
	return(result);
}

/***********************************************************
  Function:
*/
bool optReadI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount)
{
	bool result = false;

	//Fill TX Buffer with the register address
	txBuffer[0] = ui8Reg;
	//Point I2C parameters to correct values. Set up to read "cnt" number of bytes.
	i2cTransaction.slaveAddress = ui8Addr;
	i2cTransaction.writeBuf = txBuffer;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = Data;
	i2cTransaction.readCount = ui8ByteCount;
	//Perform I2C read
	result = I2C_transfer(i2c, &i2cTransaction);
	return(result);
}

