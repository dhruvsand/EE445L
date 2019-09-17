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
//**************************************************************
// structure.c
//
//! \brief Contians definitions for element and sensor structures
//!        used in associated application.
//!
//!        TWO SLIDERS, IMPLEMENTED AS WHEELS (4 physical electrodes, 3 keys)
//!        UNRWRAPPED BACK TO SLIDER IN CTS_LCDSliders layer
//!
//!        Revised by a0224638 on 9/10/2015
//*************************************************************


#include "structure.h"
#include "msp.h"

//! \brief Structure containing definitions for Upper Left
//!        element of slider0 (connected to P4.6)
//!
const struct Element upperLeft = {

    .inputBits        = CAPTIOPOSEL_4 + CAPTIOPISEL_6,
    .maxResponse    = 128 + 42,
    .threshold        = 10
};
//! \brief Structure containing definitions for Lower Left
//!        element of slider0 (connected to P6.5)
//!
const struct Element lowerLeft = {

    .inputBits        = CAPTIOPOSEL_6 + CAPTIOPISEL_5,
    .maxResponse    = 120 + 40,
    .threshold        = 10

};
//! \brief Structure containing definitions for Split Left
//!        element of slider0 (connected to P6.4)
//!
const struct Element splitLeft = {

    .inputBits        = CAPTIOPOSEL_6 + CAPTIOPISEL_4,
    .maxResponse    = 75 + 15,
    .threshold        = 10
};
//! \brief Structure containing definitions for Upper Right
//!        element of slider1 (connected to P5.0)
//!
const struct Element upperRight = {

    .inputBits        = CAPTIOPOSEL_5 + CAPTIOPISEL_0,
    .maxResponse    = 95 + 25,
    .threshold        = 25
};
//! \brief Structure containing definitions for Lower Right
//!        element of slider1 (connected to P5.2)
//!
const struct Element lowerRight = {
    .inputBits        = CAPTIOPOSEL_5 + CAPTIOPISEL_2,
    .maxResponse    = 95 + 25,
    .threshold        = 25
};
//! \brief Structure containing definitions for Split Right
//!        element of slider1 (connected to P3.6)
//!
const struct Element splitRight = {

    .inputBits        = CAPTIOPOSEL_3 + CAPTIOPISEL_6,
    .maxResponse    = 75 + 15,
    .threshold        = 15
};

//**************************************************************
// Sensor Definitions
//**************************************************************

//! \brief Structure containing definitions for Left Slider sensor
//!
const struct Sensor slider0 = {
    .halDefinition            = RO_CTIO_TA2_WDTA,
    .inputCaptioctlRegister = (uint16_t *) &CAPTIO0CTL,
    .numElements            = 3,
    .baseOffset                = 0,
    .points                    = 100,
    .sensorThreshold        = 5,

    // Pointer to elements
    .arrayPtr[0] = &splitLeft,
    .arrayPtr[1] = &lowerLeft,
    .arrayPtr[2] = &upperLeft,
    .measGateSource            = GATE_WDTA_SMCLK,
    .accumulationCycles        = WDTA_GATE_512
};

//! \brief Structure containing definitions for Right Slider sensor
//!
const struct Sensor slider1 = {
    .halDefinition            = RO_CTIO_TA2_WDTA,
    .inputCaptioctlRegister = (uint16_t *) &CAPTIO0CTL,
    .numElements            = 3,
    .baseOffset                = 3,
    .points                    = 100,
    .sensorThreshold        = 5,

    // Pointer to elements
    .arrayPtr[0] = &splitRight,
    .arrayPtr[1] = &lowerRight,
    .arrayPtr[2] = &upperRight,
    .measGateSource            = GATE_WDTA_SMCLK,
    .accumulationCycles        = WDTA_GATE_512
};
