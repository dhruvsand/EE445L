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
//!        Author: a0224638, 9/10/2015
//!
//**************************************************************

#include "structure.h"

//! \brief Structure definition for Wheel - Volume down
//!        element (connected to P6.4)
//!
const struct Element volume_down = {

		// Select pin P6.4
	    .inputBits = CAPTIOPOSEL_6 + CAPTIOPISEL_4,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 650+650,
        .threshold = 650
#endif
#ifdef RO_CTIO_TA2_TA3
        .maxResponse = 100+450,
        .threshold = 100
#endif
#ifdef fRO_CTIO_TA2_TA3
        .maxResponse = 50+250,
        .threshold = 50
#endif
};

//! \brief Structure definition for  Wheel - Forward right
//!        element (connected to P3.6)
//!
const struct Element right = {

		// Select Pin 3.6
		.inputBits = CAPTIOPOSEL_3 + CAPTIOPISEL_6,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 200+600,
        .threshold = 200
#endif
#ifdef RO_CTIO_TA2_TA3
		.maxResponse = 80+320,
		.threshold = 80
#endif
#ifdef fRO_CTIO_TA2_TA3
		.maxResponse = 50+250,
		.threshold = 50
#endif
};

//! \brief Structure definition for Wheel - Volume up
//!        element (connected to P5.2)
//!
const struct Element volume_up = {

		// Select P5.2
		.inputBits = CAPTIOPOSEL_5 + CAPTIOPISEL_2,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 200+600,
        .threshold = 200
#endif
#ifdef RO_CTIO_TA2_TA3
		.maxResponse = 80+340,
		.threshold = 80
#endif
#ifdef fRO_CTIO_TA2_TA3
		.maxResponse = 50+250,
		.threshold = 50
#endif
};

//! \brief Structure definition for Wheel - Reverse left
//!        element (connected to P6.5)
//!
const struct Element left = {

		// Select P6.5
		.inputBits = CAPTIOPOSEL_6 + CAPTIOPISEL_5,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 350+655,
        .threshold = 350
#endif
#ifdef RO_CTIO_TA2_TA3
		.maxResponse = 100+400,
		.threshold = 100
#endif
#ifdef fRO_CTIO_TA2_TA3
		.maxResponse = 50+250,
		.threshold = 50
#endif
};

//! \brief Structure definition for Wheel - Middle button
//!        element (connected to P5.0)
//!
const struct Element middle_element = {

        // Select P5.0
		.inputBits = CAPTIOPOSEL_5 + CAPTIOPISEL_0,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 300+655,
        .threshold = 300
#endif
#ifdef RO_CTIO_TA2_TA3
		.maxResponse = 250+300,
		.threshold = 250
#endif
#ifdef fRO_CTIO_TA2_TA3
		.maxResponse = 250+250,
		.threshold = 250
#endif
};

//! \brief Structure definition for Proximity element
//!        (connected to P4.6)
//!
const struct Element proximity_element = {

        //Select P4.6
		.inputBits = CAPTIOPOSEL_4 + CAPTIOPISEL_6,

		// Set .maxResponse and .threshold according
		// to selected measurement configuration
#ifdef RO_CTIO_TA2_WDTA
        .maxResponse = 150,
        .threshold = 60
#endif
#ifdef RO_CTIO_TA2_TA3
        .maxResponse = 150,
        .threshold = 60
#endif
#ifdef fRO_CTIO_TA2_TA3
        .maxResponse = 150,
        .threshold = 60
#endif
};


//! \brief  This defines the grouping of elements in, the method to measure change in
//!         capacitance for, and the function of the Wheel sensor group
//!
const struct Sensor wheel =	{

		//Select appropriate .halDefinition
#ifdef RO_CTIO_TA2_WDTA
		.halDefinition = RO_CTIO_TA2_WDTA,
#endif
#ifdef RO_CTIO_TA2_TA3
		.halDefinition = RO_CTIO_TA2_TA3,
#endif
#ifdef fRO_CTIO_TA2_TA3
		.halDefinition = fRO_CTIO_TA2_TA3,
#endif
		//Set CAPTIO0CTL register for input CAPTIO config
		// based on chosen timers
		.inputCaptioctlRegister = (uint16_t *)&CAPTIO0CTL,
        .numElements = 4,
        .points = 64,
        .sensorThreshold = 75,
        .baseOffset = 0,

        // Pointer to elements
        .arrayPtr[0] = &volume_up,  // point to first element
        .arrayPtr[1] = &right,
		.arrayPtr[2] = &volume_down,
        .arrayPtr[3] = &left,

        // Timer Information
#ifdef RO_CTIO_TA2_WDTA
      	.measGateSource = GATE_WDTA_VLO,     //  Gate Clock Source
		.accumulationCycles = WDTA_GATE_64
#endif
#ifdef RO_CTIO_TA2_TA3
		.measGateSource = TIMER_ACLK,     // Clock Source for the Gate Timer A3
		.sourceScale = TIMER_SOURCE_DIV_3,
		.accumulationCycles = 15        //CCRx for Timer A3*/
#endif
#ifdef fRO_CTIO_TA2_TA3
		.measGateSource = TIMER_SMCLK,     // Clock source for Timer A3
		.sourceScale = TIMER_SOURCE_DIV_0,
		.accumulationCycles = 10000
#endif
};

//! \brief  This defines the grouping of elements in, the method to measure change in
//!         capacitance for, and the function of the Middle Button sensor group
//!
const struct Sensor middle_button =	{

		//Select appropriate .halDefinition
#ifdef RO_CTIO_TA2_WDTA
		.halDefinition = RO_CTIO_TA2_WDTA,
#endif
#ifdef RO_CTIO_TA2_TA3
		.halDefinition = RO_CTIO_TA2_TA3,
#endif
#ifdef fRO_CTIO_TA2_TA3
		.halDefinition = fRO_CTIO_TA2_TA3,
#endif
		//Set CAPTIO0CTL register for input CAPTIO config
		// based on chosen timers
		.inputCaptioctlRegister = (uint16_t *)&CAPTIO0CTL,
        .numElements = 1,
        .baseOffset = 4,

        // Pointer to elements
        .arrayPtr[0] = &middle_element,  // point to first element

        // Timer Information
#ifdef RO_CTIO_TA2_WDTA
      	.measGateSource = GATE_WDTA_VLO,     //  Gate Clock Source
		.accumulationCycles = WDTA_GATE_64
#endif
#ifdef RO_CTIO_TA2_TA3
		.measGateSource = TIMER_ACLK,     // Clock Source for the Gate Timer A3
		.sourceScale = TIMER_SOURCE_DIV_3,
		.accumulationCycles = 15        //CCRx for Timer A3*/
#endif
#ifdef fRO_CTIO_TA2_TA3
		.measGateSource = TIMER_SMCLK,     // Clock source for Timer A3
		.sourceScale = TIMER_SOURCE_DIV_0,
		.accumulationCycles = 10000
#endif
};

//! \brief  This defines the grouping of elements in, the method to measure change in
//!         capacitance for, and the function of the Proximity sensor group
//!
const struct Sensor proximity_sensor =	{

		//Select appropriate .halDefinition
#ifdef RO_CTIO_TA2_WDTA
		.halDefinition = RO_CTIO_TA2_WDTA,
#endif
#ifdef RO_CTIO_TA2_TA3
		.halDefinition = RO_CTIO_TA2_TA3,
#endif
#ifdef fRO_CTIO_TA2_TA3
		.halDefinition = fRO_CTIO_TA2_TA3,
#endif
		//Set CAPTIO0CTL register for input CAPTIO config
		// based on chosen timers
		.inputCaptioctlRegister = (uint16_t *)&CAPTIO0CTL,
        .numElements = 1,
        .baseOffset = 5,

        // Pointer to elements
        .arrayPtr[0] = &proximity_element,  // point to first element

        // Timer Information
#ifdef RO_CTIO_TA2_WDTA
      	.measGateSource = GATE_WDTA_VLO,     //  Gate Clock Source
		.accumulationCycles = WDTA_GATE_512  //WDTA_GATE_64
#endif
#ifdef RO_CTIO_TA2_TA3
		.measGateSource = TIMER_ACLK,     // Clock Source for the Gate Timer A3
		.sourceScale = TIMER_SOURCE_DIV_3,
		.accumulationCycles = 200        //CCRx for Timer A3*/
#endif
#ifdef fRO_CTIO_TA2_TA3
		.measGateSource = TIMER_SMCLK,     // Clock source for Timer A3
		.sourceScale = TIMER_SOURCE_DIV_0,
		.accumulationCycles = 60000
#endif
};
