/******************************************************************************
 *  Filename:       sensor_tmp007.c
 *  Revised:
 *  Revision:
 *
 *  Description:    Driver for the TI TMP06 infrared thermophile sensor.
 *
 *  Copyright (C) 2014 - 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
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
 *
 ******************************************************************************/

/* -----------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "msp432.h"
#include "driverlib.h"
#include <ti/drivers/I2C.h>
#include "tmp007.h"
#include "Board.h"


/* -----------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------
 */

/* Slave address */
#define TMP007_I2C_ADDRESS              0x40

/* TMP006 register addresses */
#define TMP007_REG_ADDR_VOLTAGE         0x00
#define TMP007_REG_ADDR_LOCAL_TEMP      0x01
#define TMP007_REG_ADDR_CONFIG          0x02
#define TMP007_REG_ADDR_OBJ_TEMP        0x03
#define TMP007_REG_ADDR_STATUS          0x04
#define TMP007_REG_ADDR_STATUS_ENABLE   0x05
#define TMP007_REG_ADDR_TC0_COEFFICIENT 0x11
#define TMP007_REG_ADDR_TC1_COEFFICIENT 0x12
#define TMP007_REG_PROD_ID              0x1F

/* TMP006 register values */
#define TMP007_VAL_CONFIG_ON            0x1040  // Sensor on state (1 sec) with Output Filtering On
#define TMP007_VAL_CONFIG_OFF           0x0000  // Sensor off state
#define TMP007_VAL_CONFIG_RESET         0x8000
#define TMP007_VAL_PROD_ID              0x0078  // Product ID
#define TMP007_VAL_TC0					0x0000  //TC0 Coefficient Register
#define TMP007_VAL_TC1					0x0000  //TC1 Coefficient Register

#define TMP007_VAL_STATUS_MASK_ALERT    0x8000  // Enable Alert Pin
#define TMP007_VAL_STATUS_MASK_TEMP     0x4000  // Enable Temperature Conversion Ready Enable

/* Bit values */
#define CONV_RDY_BIT                    0x4000  // Conversion ready 

/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_SIZE                       4

/* Byte swap of 16-bit register value */
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define SWAP(v) ( (LO_UINT16(v) << 8) | HI_UINT16(v) )

/* -----------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------
 */
bool tmpWriteI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);
bool tmpReadI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);

/* -----------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------
 */
static uint8_t buf[DATA_SIZE];
static uint16_t val;
/* -----------------------------------------------------------------------------
 *                                           Public functions
 * ------------------------------------------------------------------------------
 */


/*******************************************************************************
 * @fn          sensorTmp007Init
 *
 * @brief       Initialize the temperature sensor driver
 *
 * @return      none
 ******************************************************************************/
bool sensorTmp007Init(void)
{
	// Configure sensor 
	return (sensorTmp007Enable(false));
}


/*******************************************************************************
 * @fn          sensorTmp007Enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 ******************************************************************************/
bool sensorTmp007Enable(bool enable)
{
	bool success;
//	uint8_t         txBuffer[4];
//	uint8_t         rxBuffer[0];

	if (enable)
	{
		val = TMP007_VAL_CONFIG_ON;
	}
	else
	{
		val = TMP007_VAL_CONFIG_OFF;
	}

	val = SWAP(val);
	uint16_t TC0 = TMP007_VAL_TC0;
	uint16_t TC1 = TMP007_VAL_TC1;
	success = tmpWriteI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_CONFIG, (uint8_t*)&val, REGISTER_LENGTH);
	success = tmpWriteI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_TC0_COEFFICIENT, (uint8_t*)&TC0, REGISTER_LENGTH);
	success = tmpWriteI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_TC1_COEFFICIENT, (uint8_t*)&TC1, REGISTER_LENGTH);

	return (success);
}

/*******************************************************************************
 * @fn          sensorTmp007EnableInterruptConversion
 *
 * @brief       Enable the alert pin to signal the interrupt conversion
 *
 * @return      none
 ******************************************************************************/
bool sensorTmp007EnableInterruptConversion(bool enable)
{
	bool success;

	if (enable)
	{
		val = TMP007_VAL_STATUS_MASK_ALERT | TMP007_VAL_STATUS_MASK_TEMP;
	}
	else
	{
		val = TMP007_VAL_CONFIG_OFF;
	}

	val = SWAP(val);
	success = tmpWriteI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_STATUS_ENABLE, (uint8_t*)&val, REGISTER_LENGTH);

	return (success);
}

/*******************************************************************************
 * @fn          sensorTmp007Read
 *
 * @brief       Read the sensor voltage and sensor temperature registers
 *
 * @param       rawtTemp - temperature in 16 bit format
 *
 * @param       rawtObjTemp - object temperature in 16 bit format
 *
 * @return      TRUE if valid data
 ******************************************************************************/
bool sensorTmp007Read(uint16_t *rawTemp, uint16_t *rawObjTemp)
{
	bool success;

	success = tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_STATUS, (uint8_t *)&val, REGISTER_LENGTH);

	if (success)
	{
		val = SWAP(val);
		success = val & CONV_RDY_BIT;
	}

	if (success)
	{
		// Read the sensor registers
		success = tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_LOCAL_TEMP, &buf[0],
				REGISTER_LENGTH);

		if (success)
		{
			success = tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_OBJ_TEMP, &buf[2],
					REGISTER_LENGTH);
		}
	}

	if (!success)
	{
		//	  sensorSetErrorData
	}

	// Swap bytes
	*rawTemp = (buf[0]<<6) | (buf[1]>>2);
	*rawObjTemp = (buf[2]<<6) | (buf[3]>>2);

	return (success);
}

/*******************************************************************************
 * @fn          sensorTmp007Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 ******************************************************************************/
bool sensorTmp007Test(void)
{
	// Check product ID
	tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_PROD_ID, (uint8_t *)&val,
			REGISTER_LENGTH);
	val = SWAP(val);
	
	if (val != TMP007_VAL_PROD_ID)
	{
		return (false);
	}
	// Turn sensor on
	sensorTmp007Enable(true);

	// Check config register (on)
	tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_CONFIG, (uint8_t *)&val,
			REGISTER_LENGTH);
	val = SWAP(val);

	if (val != TMP007_VAL_CONFIG_ON)
	{
		return (false);
	}

	// Turn sensor off
	sensorTmp007Enable(false);

	// Check config register (off)
	tmpReadI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_CONFIG, (uint8_t *)&val,
			REGISTER_LENGTH);
	val = SWAP(val);

	if (val != TMP007_VAL_CONFIG_OFF)
	{
		return (false);
	}

	return (true);
}

/*******************************************************************************
 * @fn          sensorTmp007Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawTemp - raw temperature from sensor
 *
 * @param       rawObjTemp - raw temperature from sensor
 *
 * @param       tObj - converted object temperature
 *
 * @param       tAmb - converted ambience temperature
 *
 * @return      none
 ******************************************************************************/
void sensorTmp007Convert(uint16_t rawTemp, uint16_t rawObjTemp, float *tObj, 
		float *tAmb)
{
	const float SCALE_LSB = 0.03125;

	*tObj = ((float)(rawObjTemp)*SCALE_LSB);

	*tAmb = ((float)(rawTemp)*SCALE_LSB);
}

/***********************************************************
  Function:
*/
bool tmpWriteI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount)
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
bool tmpReadI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount)
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
