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
// UpdateDisplay.h
//! \brief User Experience Code for the MSP-EXP432P401R
//!         main LCD/graphics header file
//!
//!         Created: Version 1.0:
//!
//!******************************************************************************

#ifndef UPDATE_DISPLAY
#define UPDATE_DISPLAY

//! \brief Turn on LCD
//!
extern void LCD_turnOn(void);

//! \brief Pull DISP high
//!
extern void LCD_display(void);

//! \brief Turn LCD off
//!
extern void LCD_turnOff(void);

//! \brief Write "430BOOST-SHARP96 Out of Box Demo for MSP432" as an intro
//!
extern void LCD_introWrite(void);

//! \brief Display the main menu
//!
extern void LCD_displayMainMenu(uint8_t choice);

//! \brief Update the clock contents in the LCD buffer
//!
extern void LCD_updateClock(uint8_t blink, uint32_t value, uint8_t choose);

//! \brief Display the menu choices for the SliderBall game
//!
extern void LCD_sliderBallMenu(uint8_t choice);

//! \brief Draw ball at new location in SliderBall game
//!
extern void LCD_drawSliderBall(uint8_t position_l, uint8_t position_r,
		                          uint8_t x, uint8_t y);

//! \brief Update how many lives the user has left
//!
extern void LCD_updateLives(uint8_t lives);

//! \brief Update User's score in SliderBall game
//!
extern void LCD_updateScore(uint32_t score);

//! \brief User lost game, show game over and score
//!
extern void LCD_displayGameOver(uint32_t score);

//! \brief User beat high score, show new high score
//!
extern void LCD_displayNewHighScore(uint32_t score);

//! \brief Draw TI Logo on SHARP-96 LCD
//!
extern void LCD_drawTILogo(void);

//! \brief Draw LaunchPad Rocket Logo on SHARP-96 LCD
//!
extern void LCD_drawRocket(void);

//! \brief Convert an unsigned int to ASCII
//!
extern int UnsignedInt_To_ASCII(unsigned int hex, char *ASCII);

//! \brief Convert an unsigned long to ASCII
//!
extern int UnsignedLong_To_ASCII(unsigned long hex, char *ASCII);

#endif
