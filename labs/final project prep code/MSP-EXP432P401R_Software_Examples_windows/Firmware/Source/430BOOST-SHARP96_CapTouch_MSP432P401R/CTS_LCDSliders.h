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
// CTS_LCDSliders.h
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Contains SHARP-96 BoosterPack specific variables and functions
//!        enabling slider operation
//!
//!        Created: Version 1.0:
//!
//*******************************************************************************

#ifndef CTS_LCD_SLIDERS
#define CTS_LCD_SLIDERS

#include "CTS_Layer.h"

//*******************************************************************************
// Defines
//*******************************************************************************

//! \brief Indicates No touch detected
//!
#define NO_DETECT         0

//! \brief Indicates Upper touch
//!
#define UPPER             2

//! \brief Indicates Upper-Middle touch
//!
#define UPPER_MIDDLE      4

//! \brief Indicates Lower-Middle touch
//!
#define LOWER_MIDDLE      6

//! \brief Indicates Lower touch
//!
#define LOWER             8

//! \brief Size of arrays storing slider position information
//!        (1 for each slider: left and right)
#define ARRAY_SIZE        2

//*******************************************************************************
// Globals
//*******************************************************************************

//! \brief Current slider touch positions
//!
extern volatile uint16_t sliderPos[2];

//! \brief Previous slider touch positions
//!
extern volatile uint16_t oldSliderPos[2];

//! \brief Flag indicating touch on slider
//!
extern volatile uint16_t touchFlag[2];

//! \brief Flag indicating direction of movement along slider
//!
extern volatile uint16_t dirFlag[2];

//*******************************************************************************
// Function Prototypes
//*******************************************************************************

//! \brief Update CapTouch Values for sliders
//!
void CTS_getReading(void);

//! \brief Update CapTouch Values for sliders
//!        and detect swipe up/down gestures
uint8_t CTS_getSwipe(uint8_t sliderIndex, uint8_t samples);

#endif
