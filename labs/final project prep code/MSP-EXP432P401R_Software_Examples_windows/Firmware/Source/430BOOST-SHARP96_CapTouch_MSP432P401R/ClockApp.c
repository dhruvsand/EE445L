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
//*******************************************************************************
//
// ClockApp.c
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Clock Application
//!
//!        Program Execution: Clock is initialized, started, and displayed. Device goes
//!        into LPM0 awaiting a 1s RTC interrupt to update the clock again.
//!
//!        Directions for use: Clock may be used as is, or can be set using the left
//!        CapTouch slider (to increment/decrement parameter) and right select button
//!        (to cycle through parameters). Time is retained on exit.
//!
//!        July 2015
//!
//******************************************************************************

//******************************************************************************
// Includes
//******************************************************************************
#include "driverlib.h"
#include "grlib.h"
#include "Sharp96x96.h"
#include "CTS_LCDSliders.h"

#include "ClockApp.h"
#include "msp432p4xx_EXP.h"
#include "myTimer.h"
#include "UpdateDisplay.h"

//******************************************************************************
// Clock Global Variables
//******************************************************************************

//! \brief Flag indicating clock is set
//!
volatile uint8_t setClock = 1;

//! \brief 1: update LCD; 0: no update to LCD
//!
volatile uint8_t freshFlag = 1;

//! \brief Flag indicating first time select button pressed
//!
volatile uint8_t firstEnter = 1;

//******************************************************************************
// Clock Function Prototypes
//******************************************************************************

//! \brief Initializes clock values, RTC for keeping track of time
//!        and resets slider position values
static void Clock_initApp(void);

//! \brief Function used to set a value displayed by clock (except Year)
//!
static uint8_t Clock_setParameter(uint8_t value, uint8_t max,
                                        uint8_t min);

//! \brief Special function for setting clock year
//!
static uint16_t  Clock_setYear(uint16_t value, uint16_t max,
                                   uint16_t min);

//! \brief Stops RTC and re-enables all Cap Touch I/Os
//!
static void Clock_exitApp(void);


//! \brief  Main clock application
//!
void Clock_runApp(void)
{
    uint8_t current_param;
    uint16_t year_param;                     // treated different (max value = 4095)

    Clock_initApp();
    firstEnter = 1; // update when first enter

    while (mode == APP_CLOCK) {
        switch (select) {
        case SEL_RUN_CLOCK:
        	LCD_updateClock(1, 1, firstEnter);                         // Update LCD buffer with clock
            __enable_interrupt();
            __sleep();							   // Go to LPM0
            break;
        case SEL_SET_HOUR:
            current_param = RTCHOUR;
            RTCCTL0 = RTCKEY;
            RTCHOUR = Clock_setParameter(current_param, 23, 0);
            RTCCTL0 = 0;
            break;
        case SEL_SET_MIN:
            current_param = RTCMIN;
            RTCCTL0 = RTCKEY;
            RTCMIN = Clock_setParameter(current_param, 59, 0);
            RTCCTL0 = 0;
            break;
        case SEL_SET_DOW:
            current_param = RTCDOW;
            RTCCTL0 = RTCKEY;
            RTCDOW = Clock_setParameter(current_param, 6, 0);
            RTCCTL0 = 0;
            break;
        case SEL_SET_MON:
            current_param = RTCMON;
            RTCCTL0 = RTCKEY;
            RTCMON = Clock_setParameter(current_param, 12, 1);
            RTCCTL0 = 0;
            break;
        case SEL_SET_DAY:
            current_param = RTCDAY;
            RTCCTL0 = RTCKEY;
            RTCDAY = Clock_setParameter(current_param, 31, 1);
            RTCCTL0 = 0;
            break;
        case SEL_SET_YEAR:
            year_param = RTCYEAR;
            RTCCTL0 = RTCKEY;
            RTCYEAR = Clock_setYear(year_param, 4095, 0);
            RTCCTL0 = 0;
            break;
        default: break;
        }
    }
    Clock_exitApp();
}

//! \brief  Initialize the clock app (RTC)
//!
static void Clock_initApp(void)
{
	RTC_C_Calendar clockCalendar;

    // Disable CapSense Pins
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN2);
    RTC_C_holdClock();

    if (setClock) {
        clockCalendar.seconds = 0;              // Set clock to generic time
        clockCalendar.minutes = 0;
        clockCalendar.hours = 12;
        clockCalendar.dayOfWeek = 1;
        clockCalendar.dayOfmonth = 1;
        clockCalendar.month = 1;
        clockCalendar.year = 2015;

        RTC_C_initCalendar(&clockCalendar, RTC_C_FORMAT_BINARY);

        setClock = 0;
    }

    oldSliderPos[0] = 20;
    oldSliderPos[1] = 20;
    sliderPos[0] = 20;
    sliderPos[1] = 20;

    select = SEL_SET_HOUR;
}

//! \brief  Set the designated parameter
//!
//! \param  value - previous/current value of parameter
//!         param - parameter index
//!         max   - maximum value for parameter
//!         min   - minimum value for parameter
//!
//! \return value - newly set value for parameter
//!
static uint8_t Clock_setParameter(uint8_t value, uint8_t max,
										uint8_t min)
{
	uint8_t temp0 = 0;
	uint8_t blink = 1;
	uint8_t lastValue = 0;

    temp0 = select;

    while (select == temp0) {

        if (blink) {                                    // only update values when user can see it updating
        	uint16_t dir = CTS_getSwipe(0, 10);
        	if(dir == 2)
        	{
        		value++;
        		if (value > max) {  									// wrap around setting
        			value = min;
        		}
        	}
        	else if(dir == 0)
        	{
        		value--;
        		if ((value < min) || (value > max)) {
        			value = max;
        		}
        	}

        }
        firstEnter = 1;

        if(value != lastValue)
        {
        	LCD_updateClock(blink, value, firstEnter);
        }
        lastValue = value;
        blink ^= 1;
    }
    return value;
}

//! brief  Set the year - handled separately because it has a larger range
//!
//! \param  value - previous/current value of year
//!         max   - maximum value for parameter
//!         min   - minimum value for parameter
//!
//! \return value - newly set value for year
//!
static uint16_t Clock_setYear(uint16_t value, uint16_t max,
                                  uint16_t min)
{
    uint8_t blink = 1;
    uint16_t lastValue = 0;

    while (select == SEL_SET_YEAR) {

        if (blink) {                                    // only update values when user can see it updating
        	uint16_t dir = CTS_getSwipe(0, 10);
        	if(dir == 2)
        	{
        		value++;
        		if (value > max) {  									// wrap around setting
        			value = min;
        		}
        	}
        	else if(dir == 0)
        	{
        		value--;
        		if ((value < min) || (value > max)) {
        			value = max;
        		}
        	}
        }
        firstEnter = 1;
        if(value != lastValue)
        {
        	LCD_updateClock(blink, value, firstEnter);
        }
        lastValue = value;
        blink ^= 1;
    }
    return value;
}


//! \brief  Exit the clock app
//!
static void Clock_exitApp(void)
{
    RTC_C_holdClock();
    RTC_C_disableInterrupt(RTC_C_PRESCALE_TIMER1_INTERRUPT);

    // Re-enable CapSense
    GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);
    GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN2);
}
