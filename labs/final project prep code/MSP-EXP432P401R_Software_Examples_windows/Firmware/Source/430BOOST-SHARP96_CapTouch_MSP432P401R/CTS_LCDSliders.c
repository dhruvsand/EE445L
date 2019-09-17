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
// CTS_LCDSliders.c
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Contains SHARP-96 BoosterPack specific variables and functions
//!        enabling slider operation
//!
//!        This file contains all the important function calls for returning a CapTouch
//!        reading to the main application
//!
//*******************************************************************************

//*******************************************************************************
// Includes
//*******************************************************************************
#include "CTS_LCDSliders.h"
#include "structure.h"

//*******************************************************************************
// Defines
//*******************************************************************************

//! \brief Used to adjust for split element in slider sensor
//!
#define BREAKPOINT ( (sliderPtr[i]->points) - ((sliderPtr[i]->points) / (sliderPtr[i]->numElements) / 2) )
#define SLOP            2

//! \brief Constant used to determine if swipe occurs
//!
#define SWIPE_THRESHOLD 0.60

//! \brief Delay between samples when detecting swipe
//!
#define SWIPE_SAMPLE_DELAY 2500

//*******************************************************************************
// Cap Touch Global Variables
//*******************************************************************************

//! \brief Current touch position on sliders
//!
volatile uint16_t sliderPos[2] = { 0, 0 };

//! \brief Previous touch position on sliders
//!
volatile uint16_t oldSliderPos[2] = { 0, 0 };

//! \brief Flags indicating touch on sliders
//!
volatile uint16_t touchFlag[2] = { 0, 0 };

//! \brief Flags indicating direction of swipe
//!        on sliders
volatile uint16_t dirFlag[2] = { 1, 1 };

//! \brief Structure containing pointers to
//!        slider sensors defined in structure.c
//!
const struct Sensor* sliderPtr[2] =
{
    &slider0,
    &slider1
};


//! \brief  CTS_getReading - this function will get a CapTouch reading from
//!         both sliders. Final positions are stored in sliderPos[x].
//!
//!         The variables sliderPos and oldSliderPos indicate the detection
//!         status and the current location.  In the event that sliderPos
//!         equals ILLEGAL_SLIDER_WHEEL_POSITION, then the previous location
//!         information is found in oldSliderPos.
//!
void CTS_getReading(void)
{
    volatile uint8_t i, j;

    for (i = 0; i < 2; i++)
    {
        // Determine Threshold crossing
        sliderPos[i] = TI_CAPT_Wheel(sliderPtr[i]);

        if (sliderPos[i] != ILLEGAL_SLIDER_WHEEL_POSITION) {

            // Check for slop zone
           if ((sliderPos[i] >= BREAKPOINT - SLOP) && (sliderPos[i] <= BREAKPOINT + SLOP)) {
                if (oldSliderPos[i] < ((sliderPtr[i]->points) / 2)) {
                    sliderPos[i] = 0;
                }
                else {
                    sliderPos[i] = sliderPtr[i]->points;
                }
            }

            // Translate wrap around of wheel to slider
            else if (sliderPos[i] < (sliderPtr[i]->points) - ((sliderPtr[i]->points) / (sliderPtr[i]->numElements) / 2)) {
                sliderPos[i] += ((sliderPtr[i]->points) / (sliderPtr[i]->numElements) / 2);
            }
            else {
                sliderPos[i] -= (sliderPtr[i]->points) - ((sliderPtr[i]->points) / (sliderPtr[i]->numElements) / 2);
            }

           //Prevent wrap-around at top and bottom edge of slider
           if(touchFlag[i] && (oldSliderPos[i] > (sliderPtr[i]->points - 2)) && sliderPos[i]<((sliderPtr[i]->points) / (sliderPtr[i]->numElements)))
           {
        	   sliderPos[i] += sliderPtr[i]->points;
           }
           else if((oldSliderPos[i] == (sliderPtr[i]->points + ((sliderPtr[i]->points) / (sliderPtr[i]->numElements))-1)) && !touchFlag[i] && (dirFlag[i] == 2) && sliderPos[i]<((sliderPtr[i]->points) / (sliderPtr[i]->numElements)))
           {
        	   sliderPos[i] = sliderPtr[i]->points + ((sliderPtr[i]->points) / (sliderPtr[i]->numElements))-2;
           }

           if(oldSliderPos[i] < sliderPos[i])
           {
        	   dirFlag[i] = 2;
           }
           else if(oldSliderPos[i] > sliderPos[i])
           {
        	   dirFlag[i] = 0;
           }

           oldSliderPos[i] = sliderPos[i];
           touchFlag[i] = 1;
        }
        else {
        	//Ignore update if illegal position is returned
            sliderPos[i] = oldSliderPos[i];
            touchFlag[i] = 0;
        }
    }     // end for loop
    return;
}


//! \brief  CTS_getSwipe - this function will look for a swipe gesture along
//!			a specified slider by keeping track of the direction of change
//! 		of the slider position over multiple samples. The final gesture
//!			determination is stored in the gesture variable.
//!
//!         The variables lastVal and currentVal indicate the value associated
//!         with the previous slider position and the current slider position,
//!         respectively. The variable gesture indicates the direction of
//!         the recorded swipe. The variable changesum indicates the sum of the
//!         position changes.
//!
//! \param  uint8_t sliderIndex - specifies which slider to read position
//! 									information on.
//! 		uint8_t samples - the number of samples to take before determining
//! 		   						direction of swipe.
//!
//! \return unint16_t - the general direction of the swipe.
//! 						gesture = 1 for no swipe
//! 						gesture = 2 for up swipe
//! 						gesture = 0 for down swipe
//!
uint8_t CTS_getSwipe(uint8_t sliderIndex, uint8_t samples)
{
	//mInitialize variables
	uint16_t lastVal = 0;
	uint16_t currentVal = 0;
	uint8_t gesture = 1;
	int16_t changeSum = 0;

	// Get current location
	CTS_getReading();
	lastVal = sliderPos[sliderIndex];

	// Take specified number of samples
	uint32_t i;
	for(i=0;i<samples;i++)
	{
		// Delay between swipes
		uint32_t j;
		for(j=0;j<SWIPE_SAMPLE_DELAY;j++);

		CTS_getReading();
		currentVal = sliderPos[sliderIndex];

		// Keep track of direction of change between samples
		// except when ILLEGAL_SLIDER_WHEEL_POSITION is detected
		if(lastVal != ILLEGAL_SLIDER_WHEEL_POSITION && currentVal != ILLEGAL_SLIDER_WHEEL_POSITION)
		{
			if(currentVal > lastVal) { changeSum++; }
			else if (currentVal < lastVal) { changeSum--; }
		}

		lastVal = currentVal;
	}

	//If number of changes detected above threshold return 2 to represent a move up.
	//If number of changes detected below threshold return 0 to represent a move down.
	//Else return default 1 for no change.
	if(((float)changeSum/(float)samples) >= SWIPE_THRESHOLD)
	{
		gesture = 2;
	}
	else if(((float)changeSum/(float)samples) <= -SWIPE_THRESHOLD)
	{
		gesture = 0;
	}
	return gesture;
}


