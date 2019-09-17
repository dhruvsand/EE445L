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
// ClockApp.h
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Clock app header file
//!
//!        Created: Version 1.0:
//!
//*******************************************************************************

#ifndef CLOCK_APP
#define CLOCK_APP

//*******************************************************************************
// Clock App Defines
//*******************************************************************************

//! \brief Defines state number for running clock
//!
#define SEL_RUN_CLOCK  2

//! \brief Defines state number for setting Hour value
//!
#define SEL_SET_HOUR   4

//! \brief Defines state number for setting Minutes value
//!
#define SEL_SET_MIN    6

//! \brief Defines state number for setting Day-of-week value
//!
#define SEL_SET_DOW    8

//! \brief Defines state number for setting Month value
//!
#define SEL_SET_MON   10

//! \brief Defines state number for setting Day value
//!
#define SEL_SET_DAY   12

//! \brief Defines state number for setting Year value
//!
#define SEL_SET_YEAR  14

//*******************************************************************************
// Clock App Globals
//*******************************************************************************

//! \brief Flag indicating clock is set
//!
extern volatile uint8_t setClock;

//! \brief Flag indicating an update is needed on LCD
//
extern volatile uint8_t freshFlag;

//! \brief Ultra-low-power flag
//!
extern volatile uint8_t ULP;

//! \brief First time select button pressed flag
//!
extern volatile uint8_t firstEnter;

//*******************************************************************************
// Clock App Function Prototypes
//*******************************************************************************

//! \brief Runs clock application code
//!
extern void Clock_runApp(void);

#endif
