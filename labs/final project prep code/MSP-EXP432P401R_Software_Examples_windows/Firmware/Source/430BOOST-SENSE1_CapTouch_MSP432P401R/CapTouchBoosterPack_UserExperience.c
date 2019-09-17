/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// MSP432 main.c
//
//*****************************************************************************

//*****************************************************************************
//         MSP432P401 - LaunchPad CapTouch BoosterPack User Experience
//
//! \brief This application operates on the LaunchPad platform using the MSP432P401
//!        device and the CapTouch BoosterPack plugin board.
//!
//******************************************************************************

//******************************************************************************
// Includes
//******************************************************************************
#include "msp.h"
#include "driverlib.h"
#include "CTS_Layer.h"

#include "uart.h"

//******************************************************************************
// UART Interface defines
//******************************************************************************

//! \brief Comment out to remove sending status updates via UART
//!
#define UART_ENABLED

//! \brief UART Bytes representing Wake Up event
//!
#define WAKE_UP_UART_CODE       0xBE
#define WAKE_UP_UART_CODE2      0xEF

//! \brief UART Bytes representing Sleep Mode event
//!
#define SLEEP_MODE_UART_CODE    0xDE
#define SLEEP_MODE_UART_CODE2   0xAD

//! \brief UART Byte representing Middle Button
//!
#define MIDDLE_BUTTON_CODE      0x80

//! \brief UART Byte representing Invalid Gesture
//!
#define INVALID_GESTURE         0xFD

//! \brief UART Byte representing Gesture Start event
//!
#define GESTURE_START           0xFC

//! \brief UART Byte representing Gesture Stop event
//!
#define GESTURE_STOP            0xFB

//! \brief UART Byte representing Counter-clockwise gesture direction
//!
#define COUNTER_CLOCKWISE       1

//! \brief UART Byte representing Clockwise gesture direction
//!
#define CLOCKWISE               2

//! \brief UART Byte representing gesutre position offset
//!
#define GESTURE_POSITION_OFFSET 0x20

//! \brief UART Byte representing wheel position offset
//!
#define WHEEL_POSITION_OFFSET   0x30

//******************************************************************************
// User Experience Application defines
//******************************************************************************

//! \brief Delay between re-sendings of touches
//!
#define WHEEL_TOUCH_DELAY		12

//! \brief Delay to put application to sleep after
//!        last recorded user interaction
#define MAX_IDLE_TIME           150

//! \brief Change in count before proximity event is recognized
//!
#ifdef RO_CTIO_TA2_WDTA
#define PROXIMITY_THRESHOLD     60				//Threshold that determines proximity event
#endif
#ifdef RO_CTIO_TA2_TA3
#define PROXIMITY_THRESHOLD     60
#endif
#ifdef fRO_CTIO_TA2_TA3
#define PROXIMITY_THRESHOLD 	20
#endif

//******************************************************************************
// DIO Port and Pin Masks for LEDs defines
//
//! \brief There are 8 LEDs to represent different positions around the wheel. They are
//!        controlled by 5 pins using a muxing scheme. The LEDs are divided
//!        vertically into two groups of 4, in which each LED is paired up [muxed] with
//!        the LED mirrored on the other side of the imaginary center vertical line via
//!        the use of pin P4.1.
//!        Specifically, the pairs are LEDs [0,7], [1,6], [2,5], [3,4].
//!
//******************************************************************************
#define MASK7                   BIT3
#define MASK6                   BIT5
#define MASK5                   BIT7
#define MASK4                   BIT6
#define MASK3                   (BIT1+BIT3+BIT5+BIT7)
#define MASK2                   (BIT1+BIT3+BIT5+BIT6)
#define MASK1                   (BIT1+BIT3+BIT6+BIT7)
#define MASK0                   (BIT1+BIT5+BIT6+BIT7)

#define PORT1_LEDMASK           (BIT5+BIT6+BIT7)
#define PORT4_LEDMASK           (BIT1+BIT3)

//*******************************************************************************
// LED Globals
//*******************************************************************************

//! \brief Masks required to display touch position
//!        at all 16 unique positions that can be displayed
//!        using given LEDs
//!
const uint8_t LedWheelPosition[16] =
{
		MASK0, MASK0, MASK0 & MASK1, MASK1,
		MASK1 & MASK2, MASK2, MASK2 & MASK3, MASK3,
		MASK4, MASK4, MASK4 | MASK5, MASK5,
		MASK5 | MASK6,  MASK6, MASK6 | MASK7, MASK7
};

//! \brief LED light-up sequence to signal wake-up event
//!
const uint8_t startSequence[8] =
{
		MASK0,
		MASK1,
		MASK2,
		MASK3,
		MASK4,
		MASK5,
		MASK6,
		MASK7
};


//*******************************************************************************
// Sensor Globals
//*******************************************************************************

//! \brief Used to store current and last detected wheel position
//!
uint16_t wheel_position=ILLEGAL_SLIDER_WHEEL_POSITION, last_wheel_position=ILLEGAL_SLIDER_WHEEL_POSITION;

//! \brief Stores single count value due to change in capacitance
//!
uint16_t deltaCnts[1];

//! \brief Stores raw counts from proximity sensor
//!
uint16_t prox_raw_Cnts;

//*******************************************************************************
// Function Prototypes
//*******************************************************************************

//! \brief Used to configure Digital I/Os that control
//!        LEDs on the BoosterPack
//!
void System_setupDIOPorts(void);

//! \brief Routine used to put device into LPM0 and periodically
//!        measure Proximity Sensor for wake-up event
//!
void CTBP_waitForProxEvent(void);

//! \brief Initializes the baseline counts for all capacitive touch elements
//!        used in application
//!
void CTBP_measureCapBaseLine(void);

//! \brief Displays start-up LED sequence on proximity event
//!
void CTBP_runLedStartUpSequence(void);

//! \brief Determines touch gesture based on current and previous
//!        wheel position
//!
uint8_t CTBP_getGesture(uint8_t position);

//! \brief Updates system state based on user touch/gesture
//!
void CTBP_processInput(void);

#ifdef UART_ENABLED
void UART_sendTouchByte(uint8_t touch);
#endif


//! \brief Main application that configures device, then cycles
//!        between Idle Mode and Active Mode forever
//!
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    System_setupDIOPorts();

    /* Setup Clocks: DCO to 1MHz and SMCLK to DCO/8 = 125kHz, MCLK = DCO, ACLK = REFO */
    /* MAP_function will use ROM version of function if available                     */
    MAP_FPU_enableModule();				// Enable FPU for DCO frequency calculation
    MAP_CS_setDCOFrequency(CS_1MHZ); 	// Set DCO Frequency to 1MHz

    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);		//Source DCO/8 for SMCLK
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source DCO for MCLK, no divider
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source REFO for ACLK

    NVIC_EnableIRQ(TA0_0_IRQn); //Enable TA0 interrupt in NVIC

    TI_CAPT_Init_Baseline(&proximity_sensor);

    while(1)
    {
    	CTBP_waitForProxEvent();
    	CTBP_measureCapBaseLine();
    	CTBP_runLedStartUpSequence();
    	CTBP_processInput();
    }
}

//! \brief Configures digital IOs to control the LEDs on the 430BOOST-SENSE1
//!        BoosterPack using defined masks.
//!
void System_setupDIOPorts(void)
{
	// DIO Port init
	P1OUT &= ~(PORT1_LEDMASK);
	P4OUT &= ~(PORT4_LEDMASK);
	P6OUT &= ~(BIT0);

	P1DIR |= PORT1_LEDMASK;
	P4DIR |= PORT4_LEDMASK;
	P6DIR |= BIT0;
}


//! \brief Device stays in LPM0 'sleep' mode, only Proximity Sensor
//!        is used to detect any movement triggering device wake up
//!
void CTBP_waitForProxEvent(void)
{

#ifdef UART_ENABLED
	/* Send status via UART: 'sleep' = [0xDE, 0xAD] */
	UART_sendTouchByte(SLEEP_MODE_UART_CODE);
	UART_sendTouchByte(SLEEP_MODE_UART_CODE2);
#endif

	/* Set DCO to 1MHz */
	/* Set SMCLK to 1MHz / 8 = 125kHz */
	MAP_CS_setDCOFrequency(CS_1MHZ); 	// Set DCO Frequency to 1MHz
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);		//Source DCO/8 for SMCLK
	MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source DCO for MCLK, no divider
	MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source REFO for ACLK

	TI_CAPT_Update_Baseline(&proximity_sensor,5);

	P6OUT |= BIT0;			// Turn on center LED
	deltaCnts[0] = 0;       // Reset delta count

	/* Sleeping in LPM0 with ACLK/100 = 32.768Khz/273 = 120 Hz wake up interval */
	/* Measure proximity sensor count upon wake up */
	/* Wake up if proximity deltaCnts > THRESHOLD */
	do
	{
		TA0CCR0 = 273;
		TA0CTL = TASSEL_1 + MC_1;
		TA0CCTL0 &= ~CCIFG;
		TA0CCTL0 |= CCIE;

		__enable_interrupt();
		__sleep();

		TA0CCTL0 &= ~CCIE;
		TI_CAPT_Custom(&proximity_sensor,(uint16_t *)deltaCnts);
	}
	while (deltaCnts[0] <= PROXIMITY_THRESHOLD);

	P6OUT &= ~BIT0;			// Turn off center LED
}

//! \brief Re-measure the baseline capacitance of the wheel elements & the center
//!        button. To be called after each wake up event.
//!
void CTBP_measureCapBaseLine(void)
{
	P1OUT &= ~PORT1_LEDMASK;
	P4OUT &= ~PORT4_LEDMASK;
	P6OUT |= BIT0;

	/* Set DCO to 8MHz */
	/* SMCLK = 8MHz/8 = 1MHz */
	MAP_CS_setDCOFrequency(CS_8MHZ); 	// Set DCO Frequency to 8MHz
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);		//Source DCO/8 for SMCLK
	MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source DCO for MCLK, no divider
	MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source REFO for ACLK

	TI_CAPT_Init_Baseline(&wheel);
	TI_CAPT_Update_Baseline(&wheel,2);
	TI_CAPT_Init_Baseline(&middle_button);
	TI_CAPT_Update_Baseline(&middle_button,2);
}



//! \brief Display an LED lighting sequence to indicate the wake up event
//!
void CTBP_runLedStartUpSequence(void)
{
	uint8_t i;
	TA0CCTL0 &= ~CCIFG;
	TA0CCTL0 = CCIE;                           // CCR0 interrupt enabled
	TA0CTL |= TACLR;
	TA0CCR0 = TA0R + 500;                       // ?ms
	TA0CTL = TASSEL_1 + MC_1;                  // ACLK, upmode

	/* Slow clockwise sequence */
	for(i=0; i<8; i++)
	{
		P1OUT = startSequence[i]&PORT1_LEDMASK;
		P4OUT = startSequence[i]&PORT4_LEDMASK;

		__enable_interrupt();
		__sleep();

		uint16_t d;					//Replace with TA0 and sleep?
		for(d=0;d<50000;d++);       //??

		TA0CCR0 = TA0R + 500;   // 50ms
	}

	P1OUT &= ~PORT1_LEDMASK;
	P4OUT &= ~PORT4_LEDMASK;
	P6OUT |= BIT0;

	/* Fast counter-clockwise sequence */
	while(i)
	{
		i--;
		P1OUT = startSequence[i]&PORT1_LEDMASK;
		P4OUT = startSequence[i]&PORT4_LEDMASK;

		__enable_interrupt();
		__sleep();

		TA0CCR0 = TA0R + 500;   // ?ms
	}

	TA0CCTL0 &= ~CCIE;                         // CCR0 interrupt disabled
	P1OUT &= ~PORT1_LEDMASK;
	P4OUT &= ~PORT4_LEDMASK;
	P6OUT &= ~BIT0;                                // Turn off all LEDs
}


//! \brief Determine immediate gesture based on current & previous wheel position
//! \param position is the given wheel position
//! \return gesture is the byte representing the identified gesture
//!
uint8_t CTBP_getGesture(uint8_t position)
{
	uint8_t gesture = INVALID_GESTURE, direction, ccw_check, cw_check;
	// ******************************************************************************
	// gesturing
	// determine if a direction/swipe is occuring
	// the difference between the initial position and
	// the current wheel position should not exceed 8
	// 0-1-2-3-4-5-6-7-8-9-A-B-C-D-E-F-0...
	//
	// E-F-0-1-2:  cw, 4
	// 2-1-0-F-E: ccw, 4
	// A-B-C-D-E-F

	if(last_wheel_position != ILLEGAL_SLIDER_WHEEL_POSITION)
	{
		if(last_wheel_position  > position)
		{
			// E-D-C-B-A:  ccw, 4
			// counter clockwise: 0 < (init_wheel_position - wheel_position) < 8
			//                    gesture = init_wheel_position - wheel_position
			//
			// E-F-0-1-2:  cw, 4
			// clockwise:        0 < (init_wheel_position+wheel_position)-16 <8
			//
			ccw_check = last_wheel_position  - position;
			if(ccw_check < 8)
			{
				gesture = ccw_check;
				direction = COUNTER_CLOCKWISE;
			}
			else
			{
				// E-F-0-1-2:  cw, 4
				// 16 - 14 + 2 = 4
				cw_check = 16 - last_wheel_position  + position ;
				if(cw_check < 8)
				{
					gesture = cw_check;
					direction = CLOCKWISE;
				}
			}
		}
		else
		{
			// initial_wheel_position <= wheel_position
			//
			// 2-1-0-F-E: ccw, 4
			// counter clockwise:
			//                    0 < (init_wheel_position+wheel_position)-16 <8
			//                    gesture = init_wheel_position - wheel_position
			//
			// 0-1-2-3-4:  cw, 4
			// clockwise:        0 < (wheel_position - init_wheel_position) < 8
			//
			cw_check = position - last_wheel_position ;
			if(cw_check < 8)
			{
				gesture = cw_check;
				direction = CLOCKWISE;
			}
			else
			{
				// 2-1-0-F-E: ccw, 4
				// 16 + 2 - 14 = 4
				ccw_check = 16 + last_wheel_position  - position ;
				if(ccw_check < 8)
				{
					gesture = ccw_check;
					direction = COUNTER_CLOCKWISE;
				}
			}
		}
	}
	if (gesture == INVALID_GESTURE)
		return gesture;
	if (direction == COUNTER_CLOCKWISE)
		return (gesture + 16);
	else
		return gesture;
}


//! \brief Updates system and BoosterPack state based on gesture
//!
void CTBP_processInput(void)
{
	uint8_t idleCounter, activeCounter;
	uint8_t gesture, gestureDetected;
	uint8_t centerButtonTouched = 0;
	uint16_t wheelTouchCounter = WHEEL_TOUCH_DELAY - 1;

	gesture = INVALID_GESTURE;            // Wipes out gesture history

#ifdef UART_ENABLED
	/* Send status via UART: 'wake up' = [0xBE, 0xEF] */
	UART_sendTouchByte(WAKE_UP_UART_CODE);
	UART_sendTouchByte(WAKE_UP_UART_CODE2);
#endif

	idleCounter = 0;
	activeCounter = 0;
	gestureDetected = 0;

	while (idleCounter++ < MAX_IDLE_TIME)
	{
		/* Set DCO to 8MHz */
		/* SMCLK = 8MHz/8 = 1MHz */
		MAP_CS_setDCOFrequency(CS_8MHZ); 	// Set DCO Frequency to 8MHz
		MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);		//Source DCO/8 for SMCLK
		MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source DCO for MCLK, no divider
		MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);		//Source REFO for ACLK

		TA0CCTL0 &= ~CCIE;

		wheel_position = ILLEGAL_SLIDER_WHEEL_POSITION;
		wheel_position = TI_CAPT_Wheel(&wheel);

		/* Process wheel touch/position/gesture  if a wheel touch is registered*/
		/* Wheel processing has higher priority than center button*/

		if(wheel_position != ILLEGAL_SLIDER_WHEEL_POSITION)
		{
			centerButtonTouched = 0;

			/* Adjust wheel position based: rotate CCW by 2 positions */
			if (wheel_position < 0x08)
			{
				wheel_position += 0x40 - 0x08;
			}
			else
			{
				wheel_position -= 0x08;
				/* Adjust wheel position based: rotate CCW by 2 positions */
			}

			wheel_position = wheel_position >>2;  // divide by four
			gesture = CTBP_getGesture(wheel_position);

			/* Add hysteresis to reduce toggling between wheel positions if no gesture
			 * has been TRULY detected. */
			if ( (gestureDetected==0) && ((gesture<=1) || (gesture==0x11) || (gesture==0x10)))
			{
				if (last_wheel_position != ILLEGAL_SLIDER_WHEEL_POSITION)
					wheel_position = last_wheel_position;
				gesture = 0;
			}

			/* Turn on corresponding LED(s) */
			//P1OUT = (P1OUT & BIT0) | LedWheelPosition[wheel_position];
			//P6OUT |= BIT0;
			P1OUT = LedWheelPosition[wheel_position] & PORT1_LEDMASK;
			P4OUT = LedWheelPosition[wheel_position] & PORT4_LEDMASK;

			if ((gesture != 0) && (gesture != 16) && (gesture != INVALID_GESTURE))
			{
				/* A gesture has been detected */
				if (gestureDetected ==0)
				{
					/* Starting of a new gesture sequence */
					gestureDetected = 1;

#ifdef UART_ENABLED
					/* Transmit gesture start status update & position via UART to PC */
					UART_sendTouchByte(GESTURE_START);
					UART_sendTouchByte(last_wheel_position + GESTURE_POSITION_OFFSET);
#endif
				}
				/* Transmit gesture & position via UART to PC */
#ifdef UART_ENABLED
				UART_sendTouchByte(gesture);
				UART_sendTouchByte(wheel_position + GESTURE_POSITION_OFFSET);
#endif
			}
			else
				if (gestureDetected==0)
				{
					/* If no gesture was detected, this is constituted as a touch/tap */
					if (++wheelTouchCounter >= WHEEL_TOUCH_DELAY)
					{
						/* Transmit wheel position [twice] via UART to PC */
						wheelTouchCounter = 0;
#ifdef UART_ENABLED
						UART_sendTouchByte(wheel_position + WHEEL_POSITION_OFFSET );
						UART_sendTouchByte(wheel_position + WHEEL_POSITION_OFFSET );
#endif
					}
				}
				else
					wheelTouchCounter = WHEEL_TOUCH_DELAY - 1;

			idleCounter = 0;                      // Reset idle counter
			activeCounter++;
			last_wheel_position = wheel_position;
		}
		else
		{
			/* no wheel position was detected */
			if(TI_CAPT_Button(&middle_button))
			{
				/* Middle button was touched */
				if (centerButtonTouched==0)
				{
#ifdef UART_ENABLED
					/* Transmit center button code [twice] via UART to PC */
					UART_sendTouchByte(MIDDLE_BUTTON_CODE);
					UART_sendTouchByte(MIDDLE_BUTTON_CODE);
#endif

					centerButtonTouched = 1;

					P6OUT = (P6OUT & BIT0) ^ BIT0;	// Toggle Center LED
				}
				idleCounter = 0;
			}
			else
			{
				/* No touch was registered at all [Not wheel or center button */
				centerButtonTouched = 0;
				//P6OUT &= ~BIT0;

				if ( (gesture == INVALID_GESTURE) || (gestureDetected ==0))
				{
					/* No gesture was registered previously */
					if (last_wheel_position  != ILLEGAL_SLIDER_WHEEL_POSITION)
					{
#ifdef UART_ENABLED
						/* Transmit last wheel position [twice] via UART to PC */
						UART_sendTouchByte(last_wheel_position  + WHEEL_POSITION_OFFSET );
						UART_sendTouchByte(last_wheel_position  + WHEEL_POSITION_OFFSET );
#endif
						wheelTouchCounter = WHEEL_TOUCH_DELAY - 1;
					}
				}
				if (gestureDetected == 1)
				{
#ifdef UART_ENABLED
					/* A gesture was registered previously */
					/* Transmit status update: stop gesture tracking [twice] via UART to PC */
					UART_sendTouchByte(GESTURE_STOP);
					UART_sendTouchByte(GESTURE_STOP);
#endif
				}
			}
			// Reset all touch conditions, turn off LEDs,
			last_wheel_position= ILLEGAL_SLIDER_WHEEL_POSITION;
			gesture = INVALID_GESTURE;
			gestureDetected = 0;
			P1OUT &= ~PORT1_LEDMASK;
			P4OUT &= ~PORT4_LEDMASK;
		}
  /* ------------------------------------------------------------------------
   * Option:
   * Add delay/sleep cycle here to reduce active duty cycle. This lowers power
   * consumption but sacrifices wheel responsiveness. Additional timing
   * refinement must be taken into consideration when interfacing with PC
   * applications GUI to retain proper communication protocol.
   * -----------------------------------------------------------------------*/
	}
}

#ifdef UART_ENABLED

//! \brief Sends a byte of data indicating touch type/gesture via UART
//! \param touch contains the byte corresponding to the touch or gesture
//!
void UART_sendTouchByte(uint8_t touch)
{
	UART_init();
	UART_tx(touch);
	UART_shutdown();
}
#endif

//! \brief Interrupt handler for Timer A0. Used to implement delays.
//!
void TA0_0_IRQHandler(void)
{
	TA0CCTL0 &= ~CCIFG;
	__low_power_mode_off_on_exit();
}


