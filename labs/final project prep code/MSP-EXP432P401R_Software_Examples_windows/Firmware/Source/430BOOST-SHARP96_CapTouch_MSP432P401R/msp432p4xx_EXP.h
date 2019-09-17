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
// msp432p4xx_EXP.h
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Experience Header File
//!
//!
//*******************************************************************************

#ifndef P4XX_EXP_UE_CODE
#define P4XX_EXP_UE_CODE

//*******************************************************************************
// Includes
//*******************************************************************************
#include "grlib/grlib/grlib.h"

//*******************************************************************************
// Defines
//*******************************************************************************
//! \brief The project's version string as it will get output to the LCD during startup.
//!
#define UE_VERSION_STRING           "V1.0.0"

//! \brief The system's configured MCLK frequency.
//!
#define UE_MCLK_FREQUENCY           8000000

//! \brief Helper definition that can be used for SW delays etc.. Note that the pre-processor constant is
//!        defined as a floating point constant to make sure not to loose precision in case the calculation
//!        would yield a non-integer result. This won't affect application runtime as this definition is
//!        resolved at compile time.
//!
#define MCLK_CYCLES_PER_US          (UE_MCLK_FREQUENCY / 1000000)

//! \brief Display Main Menu
//!
#define APP_MAIN_MENU       2

//! \brief RTC clock/watch mode
//!
#define APP_CLOCK           4

//! \brief SliderBall game
//!
#define APP_SLIDERBALL      12

//*******************************************************************************
// External globals
//*******************************************************************************

//! \brief The context associated with LCD display
//!
extern tContext g_sContext;

//! \brief Application mode
//!
extern volatile uint8_t mode;

//! \brief Select button flag
//!
extern volatile uint8_t select;

//*******************************************************************************
// Function Prototypes
//*******************************************************************************

//! \brief Application code for Main Menu
//!
extern uint8_t MainMenu(void);

//! \brief Used to disable use of left button
//!
extern void DisableLeftSwitch(void);

//! \brief Used to enable use of left button
//!
extern void EnableLeftSwitch(void);

//! \brief Used to disable use of right button
//!
extern void DisableRightSwitch(void);

//! \brief Used to enable use of right switch
//!
extern void EnableRightSwitch(void);

//! \brief Starts a timer to prevent bouncing on
//!        button clicks
//!
extern void StartDebounceTimer(void);

//! \brief Turns on LED
extern void onLED(void);

//! \brief Turns off LED
extern void offLED(void);

#endif /* P4XX_EXP_UE_CODE */
