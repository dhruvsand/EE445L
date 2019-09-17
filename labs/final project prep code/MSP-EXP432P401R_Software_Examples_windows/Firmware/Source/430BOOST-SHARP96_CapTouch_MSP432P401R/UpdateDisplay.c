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
// UpdateDisplay.c
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Display things on the LCD
//!
//!        July 2015
//!        Rev 1.0
//!
//*******************************************************************************

//*******************************************************************************
// Includes
//*******************************************************************************
#include <string.h>

#include "driverlib.h"
#include "grlib.h"
#include "Sharp96x96.h"

#include "UpdateDisplay.h"
#include "Game.h"
#include "msp432p4xx_EXP.h"
#include "Preloaded_Images/images.h"

//*******************************************************************************
// defines
//*******************************************************************************
#define NORMAL_DISPLAY

//! \brief  Turn on LCD
//!
void LCD_turnOn(void)
{
    P6OUT |= BIT0;                          // Turn on LCD (Vcc = HI)
}


//! \brief  Pull LCD DISP pin high
//!
void LCD_display(void)
{
    P4OUT |= BIT1;                            // Enable LCD output (DISP = HI)
}

//! \brief  Turn off LCD
//!
void LCD_turnOff(void)
{
    P4OUT &= ~BIT1;
    int i=0;
    for(;i<100;i++);

    // Turn off LCD (DISP = LO & Vcc = LO)
    P6OUT &= ~BIT0;
}


//! \brief  Write "430BOOST-SHARP96 Out of Box Demo" before
//!         showing main menu.
//!
void LCD_introWrite(void)
{
    GrClearDisplay(&g_sContext);
    GrContextFontSet(&g_sContext, &g_sFontCmss12);

    GrStringDraw(&g_sContext, "430BOOST-", -1, 20, 15, 0);
    GrStringDraw(&g_sContext, "SHARP96", -1, 24, 29, 0);
    GrStringDraw(&g_sContext, "Out of Box Demo", -1, 4, 43, 0);
    GrStringDraw(&g_sContext, UE_VERSION_STRING, -1, 30, 57, 0);
    GrStringDraw(&g_sContext, "For MSP432", -1, 18, 71, 0);

    GrFlush(&g_sContext);
}


//! \brief  Create main menu for UE (highlight correct choice)
//!
void LCD_displayMainMenu(uint8_t choice)
{
    char outString[32];
    uint8_t text_start = 0, i;

    GrContextFontSet(&g_sContext, &g_sFontCmss12);
    GrClearDisplay(&g_sContext);

    // Write "MAIN MENU:"
    GrStringDraw(&g_sContext, "MAIN MENU:", -1, 16, 1, 0);

    // Write menu options
    GrStringDraw(&g_sContext, "Clock", -1, 2, 14, 0);
    GrStringDraw(&g_sContext, "SliderBall Game", -1, 2, 28, 0);
    GrStringDraw(&g_sContext, "Enter", -1, 66, 84, 0);

    // Overwrite chosen option with inverse colors
    switch (choice) {
    case 4:
        text_start = 14;
        strcpy(outString, "Clock");
        break;
    case 12:
        text_start = 28;
        strcpy(outString, "SliderBall Game");
        break;
    default: break;
    }

    //ClrBlack       this affects the highlight color
    GrContextForegroundSet(&g_sContext, ClrBlack);
    for (i = text_start; i < (text_start + 14); i++) {
        GrLineDrawH(&g_sContext, 0, 96, i);
    }

    //ClrWhite      this affects the text color in the highlight
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 2, text_start, 0);

    //black         this affects the text of everything else.
    GrContextForegroundSet(&g_sContext, ClrBlack);

    GrFlush(&g_sContext);
}

//! \brief  This function updates the LCD buffer with the RTC clock contents
//!
//!        NOTE: for regular clock operation, the values given to blink and
//!        value variables do not matter
//!
//! \param  blink - flag for blinking variables during clock setting
//!         value - value to blink while setting clock
//!         choose-1:display set in the right; 0: dispaly ULP
//!
//!
void LCD_updateClock(uint8_t blink, uint32_t value, uint8_t choose)
{
    char outString[32];
    char dataValue[10] = "";
    uint32_t year, month, day, hour, min, sec, weekday;
    uint8_t select_save, dow_spacing;
    char dow[10] = "";

    select_save = select;                   // Save select value in case of interrupt

    year = RTCYEAR; month = RTCMON; day = RTCDAY;
    hour = RTCHOUR; min = RTCMIN; sec = RTCSEC; weekday = RTCDOW;

    switch (select_save) {  // overwrite RTC value for parameter being set
    case  2:                // Clock running        // TODO: Populate symbolic constants for all those
        break;
    case  4:                // Setting hour
        hour = value;
        break;
    case  6: // Setting minute
        min = value;
        break;
    case  8: // Setting weekday
        weekday = value;
        break;
    case 10: // Setting month
        month = value;
        break;
    case 12: // Setting day of month
        day = value;
        break;
    case 14: // Setting year
        year = value;
        break;
    default: break;
    }

    switch (weekday) { // Decode day of week
    case 0:
        strcpy(dow, "SUNDAY");
        dow_spacing = 26;
        break;
    case 1:
        strcpy(dow, "MONDAY");
        dow_spacing = 24;
        break;
    case 2:
        strcpy(dow, "TUESDAY");
        dow_spacing = 21;
        break;
    case 3:
        strcpy(dow, "WEDNESDAY");
        dow_spacing = 13;
        break;
    case 4:
        strcpy(dow, "THURSDAY");
        dow_spacing = 18;
        break;
    case 5:
        strcpy(dow, "FRIDAY");
        dow_spacing = 26;
        break;
    case 6:
        strcpy(dow, "SATURDAY");
        dow_spacing = 17;
        break;
    default: break;
    }

    GrClearDisplay(&g_sContext);
    GrContextForegroundSet(&g_sContext, ClrBlack);

    // Write Day of Week
    if ((select_save != 8) || (blink == 1)) { // if setting DOW, blink it
        GrContextFontSet(&g_sContext, &g_sFontCmss12);
        GrStringDraw(&g_sContext, (int8_t*)dow, -1, dow_spacing, 12, 0);
    }

    // Write time (hour and minutes)
    strcpy(dataValue, "");  // Clear dataValue
    if (hour >= 10) {       // write hour value
        UnsignedInt_To_ASCII(hour, dataValue);
        strcpy(outString, dataValue);
    }
    else {
        strcpy(outString, "");
        dataValue[0] = 0x30;
        dataValue[1] = hour % 10 + 0x30;
        strncat(outString, dataValue, 2);
    }
    strcat(outString, ":");                     // append colon between hour and minute
    if ((select_save != 6) || (blink == 1)) {   // append minute value to time
        if (min >= 10) {                        // only if it is not blinking
            UnsignedInt_To_ASCII(min, dataValue);
            strncat(outString, dataValue, 2);
        }
        else {
            dataValue[0] = 0x30;
            dataValue[1] = min % 10 + 0x30;
            strncat(outString, dataValue, 2);
        }
    }
    GrContextFontSet(&g_sContext, &g_sFontCmss32);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 10, 30, 0); // write time (hr:min)

    // if blinking hour, overwrite it with background color
    if ((select_save == 4) && (blink == 0)) {
        GrContextForegroundSet(&g_sContext, ClrWhite);
        strcpy(dataValue, ""); // Clear dataValue
        if (hour >= 10) {
            UnsignedInt_To_ASCII(hour, dataValue);
            strcpy(outString, dataValue);
        }
        else {
            strcpy(outString, "");
            dataValue[0] = 0x30;
            dataValue[1] = hour % 10 + 0x30;
            strncat(outString, dataValue, 2);
        }
        GrStringDraw(&g_sContext, (int8_t*)outString, -1, 10, 30, 0);
        GrContextForegroundSet(&g_sContext, ClrBlack);
    }

    // Write seconds
    strcpy(dataValue, ""); // Clear dataValue
    if (sec >= 10) {
        UnsignedInt_To_ASCII(sec, dataValue);
        strcpy(outString, dataValue);
    }
    else {
        strcpy(outString, "");
        dataValue[0] = 0x30;
        dataValue[1] = sec % 10 + 0x30;
        strncat(outString, dataValue, 2);
    }
    GrContextFontSet(&g_sContext, &g_sFontCmss12);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 75, 45, 0);

    // Write date (mm/dd/yyyy)
    strcpy(dataValue, "");  // Clear dataValue
    if (month >= 10) {      // first write month
        UnsignedInt_To_ASCII(month, dataValue);
        strcpy(outString, dataValue);
    }
    else {
        strcpy(outString, "");
        dataValue[0] = 0x30;
        dataValue[1] = month % 10 + 0x30;
        strncat(outString, dataValue, 2);
    }
    strcat(outString, "/"); // append "/"
    if (day >= 10) {        // then write day
        UnsignedInt_To_ASCII(day, dataValue);
        strncat(outString, dataValue, 2);
    }
    else {
        dataValue[0] = 0x30;
        dataValue[1] = day % 10 + 0x30;
        strncat(outString, dataValue, 2);
    }
    strcat(outString, "/");                     // append another slash
    if ((select_save != 14) || (blink == 1)) {  // If blinking year, conditionally append it
        UnsignedInt_To_ASCII(year, dataValue);
        strncat(outString, dataValue, 4);
    }
    GrContextFontSet(&g_sContext, &g_sFontCmss12);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 18, 70, 0); // write date (mm/dd/yyyy)

    // If setting the day, overwrite it (and month) with background color
    if ((select_save == 12) && (blink == 0)) {
        GrContextForegroundSet(&g_sContext, ClrWhite);
        strcpy(dataValue, ""); // Clear dataValue
        if (month >= 10) {
            UnsignedInt_To_ASCII(month, dataValue);
            strcpy(outString, dataValue);
        }
        else {
            strcpy(outString, "");
            dataValue[0] = 0x30;
            dataValue[1] = month % 10 + 0x30;
            strncat(outString, dataValue, 2);
        }
        strcat(outString, "/");
        if (day >= 10) {
            UnsignedInt_To_ASCII(day, dataValue);
            strncat(outString, dataValue, 2);
        }
        else {
            dataValue[0] = 0x30;
            dataValue[1] = day % 10 + 0x30;
            strncat(outString, dataValue, 2);
        }
        GrStringDraw(&g_sContext, (int8_t*)outString, -1, 18, 70, 0);
    }

    if (((select_save == 10) || (select_save == 12)) && (blink == 0)) {
        if (select_save == 10) { // setting month, overwrite with background color
            GrContextForegroundSet(&g_sContext, ClrWhite);
        }
        else { // setting day, rewrite the month & append a slash
            GrContextForegroundSet(&g_sContext, ClrBlack);
        }
        strcpy(dataValue, ""); // Clear dataValue
        if (month >= 10) {
            UnsignedInt_To_ASCII(month, dataValue);
            strcpy(outString, dataValue);
        }
        else {
            strcpy(outString, "");
            dataValue[0] = 0x30;
            dataValue[1] = month % 10 + 0x30;
            strncat(outString, dataValue, 2);
        }
        if (select_save == 12) { // Add slash if setting day
            strcat(outString, "/");
        }
        GrStringDraw(&g_sContext, (int8_t*)outString, -1, 18, 70, 0);
        if (select_save == 10) {
            GrContextForegroundSet(&g_sContext, ClrBlack);
        }
    }

    // Write "Exit" in bottom left corner and "Set" in bottom right corner
    GrStringDraw(&g_sContext, "Exit           Set", -1, 2, 84, 0);

    GrFlush(&g_sContext);
}


//! \brief  Display different difficulties for SliderBall Game
//! \param uint8_t choice - selected game difficulty
//!
void LCD_sliderBallMenu(uint8_t choice)
{
    char outString[32];
    uint8_t text_start = 0, i;

    GrClearDisplay(&g_sContext);
    GrContextFontSet(&g_sContext, &g_sFontCmss12);
    GrContextForegroundSet(&g_sContext, ClrBlack);

    // Write "MAIN MENU:"
    GrStringDraw(&g_sContext, "Choose Difficulty:", -1, 2, 1, 0);

    // Write menu options
    GrStringDraw(&g_sContext, "Easy", -1, 2, 14, 0);
    GrStringDraw(&g_sContext, "Normal", -1, 2, 28, 0);
    GrStringDraw(&g_sContext, "Hard", -1, 2, 42, 0);
    GrStringDraw(&g_sContext, "INSANE", -1, 2, 56, 0);
    GrStringDraw(&g_sContext, "Exit         Enter", -1, 2, 84, 0);

    // Overwrite chosen option with inverse colors
    switch (choice) {
    case 2:
        text_start = 14;
        strcpy(outString, "Easy");
        break;
    case 4:
        text_start = 28;
        strcpy(outString, "Normal");
        break;
    case 6:
        text_start = 42;
        strcpy(outString, "Hard");
        break;
    case 8:
        text_start = 56;
        strcpy(outString, "INSANE");
        break;
    default: break;
    }

    GrContextForegroundSet(&g_sContext, ClrBlack);
    for (i = text_start; i < (text_start + 14); i++) {
        GrLineDrawH(&g_sContext, 0, 96, i);
    }
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 2, text_start, 0);
    GrContextForegroundSet(&g_sContext, ClrBlack);

    GrFlush(&g_sContext);
}

//! \brief  This function draws the paddles and ball used in the SliderBall
//!         game
//!
//! \param  position_l - position of left paddle
//!        position_r - position of right paddle
//!         x,y - ball's horizontal/vertical (respectively) position
//!
void LCD_drawSliderBall(uint8_t position_l, uint8_t position_r,
                        uint8_t x, uint8_t y)
{
    uint8_t LCD_line_l, LCD_line_r; //, i;

    // Take CT position and turn it into the line on the LCD it represents
    LCD_line_l = (uint8_t)(105 - (52 * position_l / 18));
    LCD_line_r = (uint8_t)(105 - (52 * position_r / 18));

    // Clear part of buffer used for gameplay
    uint8_t i = 0, j = 0;
    for (i = 14; i < 83; i++) {
        for (j = 0; j < 12; j++) {
            DisplayBuffer[i][j] = 0xFF;
        }
    }

    // Draw Paddles and Ball
    GrLineDrawV(&g_sContext, 5, LCD_line_l - 8, LCD_line_l + 8);  // Draw Left Paddle
    GrLineDrawV(&g_sContext, 90, LCD_line_r - 8, LCD_line_r + 8); // Draw Right Paddle
    GrCircleDraw(&g_sContext, x, y, 3);                           // Draw Ball

    // Send Buffer to LCD
    GrFlush(&g_sContext);
}


//! \brief  This function displays how many lives the user has left
//!
//! \param  lives - how many lives should be showing
//!
void LCD_updateLives(uint8_t lives)
{
    char outString[32];
    char dataValue[10] = "";

    // Clear part of buffer used for Lives
    uint8_t i = 0, j = 0;

    for (i = 0; i < 13; i++) {
        for (j = 0; j < 12; j++) {
            DisplayBuffer[i][j] = 0xFF;
        }
    }

    // Write Lives at top of screen
    strcpy(outString, "Lives: ");
    UnsignedInt_To_ASCII(lives, dataValue);
    strncat(outString, dataValue, 1);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 2, 0, 0);

    // Send Buffer to LCD
    GrFlush(&g_sContext);
}

//! \brief  This function displays the user's score
//! \param  score - the user's score to be displayed
//!
void LCD_updateScore(uint32_t score)
{
    char outString[32];
    char dataValue[10] = "";
    int32_t digits;

    // Clear part of buffer used for scoring
    uint8_t i = 0, j = 0;

    for (i = 84; i < 94; i++) {
        for (j = 4; j < 12; j++) {
            DisplayBuffer[i][j] = 0xFF;
        }
    }

    // Write Score
    strcpy(outString, "Score: ");
    digits = UnsignedInt_To_ASCII(score, dataValue);
    strncat(outString, dataValue, digits);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 2, 84, 0);

    // Send Buffer to LCD
    GrFlush(&g_sContext);
}

//! \brief  This function writes the game over message and shows the user's
//!         final score when either a button has been pushed or the user
//!         has run out of lives in the game
//!
//! \param  score - final score of user
//!
void LCD_displayGameOver(uint32_t score)
{
    char outString[32];
    char dataValue[10] = "";
    int digits;

    GrClearDisplay(&g_sContext);

    GrStringDraw(&g_sContext, "GAME OVER!", -1, 14, 5, 0);

    // Write Final Score
    GrStringDraw(&g_sContext, "Final Score: ", -1, 20, 25, 0);
    strcpy(outString, "");
    digits = UnsignedInt_To_ASCII(score, dataValue);
    strncat(outString, dataValue, digits);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 52 - (4 * digits), 40, 0);

    // Write High Score
    GrStringDraw(&g_sContext, "High Score: ", -1, 20, 55, 0);
    strcpy(outString, "");
    digits = UnsignedInt_To_ASCII(Game_highScore, dataValue);
    strncat(outString, dataValue, digits);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 52 - (4 * digits), 70, 0);

    // Send Buffer to LCD
    GrFlush(&g_sContext);
}

//! \brief  This function lets the user know they set a new high score
//!         and overwrites the highscore in FRAM with the user's score.
//!
//! \param  score - final score of user
//!
void LCD_displayNewHighScore(uint32_t score)
{
    char outString[32];
    char dataValue[10] = "";
    int32_t digits;

    Game_highScore = score;                      // set new high score

    GrClearDisplay(&g_sContext);

    GrStringDraw(&g_sContext, "Congratulations!", -1, 10, 15, 0);
    GrStringDraw(&g_sContext, "NEW HIGH SCORE:", -1, 2, 40, 0);

    // Write Final Score
    strcpy(outString, "");
    digits = UnsignedInt_To_ASCII(score, dataValue);
    strncat(outString, dataValue, digits);
    GrStringDraw(&g_sContext, (int8_t*)outString, -1, 52 - (4 * digits), 55, 0);

    // Send Buffer to LCD
    GrFlush(&g_sContext);
}

//! \brief  This function takes a pre-loaded image of a rocket and displays
//!         it on the LCD.
//!
void LCD_drawTILogo(void)
{
	Graphics_clearDisplay(&g_sContext);
	Graphics_drawImage(&g_sContext, &TI_Logo_69x64_1BPP_UNCOMP, 15, 15);
	Graphics_flushBuffer(&g_sContext);
	return;
}

//! \brief  This function takes a pre-loaded image of a rocket and displays
//!         it on the LCD.
//!
void LCD_drawRocket(void)
{
	Graphics_drawImage(&g_sContext, &LPRocket_96x37_1BPP_UNCOMP, 3, 28);
	Graphics_flushBuffer(&g_sContext);
	return;
}

//! \brief  UnsignedInt_To_ASCII
//!
//! \param  unsigned int hex - hexadecimal value of int to be converted
//! \param  char * ASCII - pointer to where the result was stored
//!
//! return i - number of digits that the Unsigned Int is in ASCII
//!
int UnsignedInt_To_ASCII(unsigned int hex, char *ASCII)
{
    int32_t i, j;
    char flipped[5] = { 0, 0, 0, 0, 0 };

    // Convert from hex to ASCII
    for (i = 1; i < 6; i++) {
        flipped[i] = hex % 10 + 0x30;
        hex /= 10;
        if (hex == 0) {
            break;
        }
    }

    // Flip data values to correct position in array
    for (j = 0; j < 5; j++) {
        ASCII[j] = flipped[i - j];
    }

    return i;
}


//! \brief  UnsignedLong_To_ASCII
//!
//! \param  unsigned long hex - hexadecimal value of long to be converted
//! \param  char * ASCII - pointer to where the result was stored
//!
//! \return i - number of digits that the Unsigned Long is
//!
int UnsignedLong_To_ASCII(unsigned long hex, char *ASCII)
{
    int32_t i, j;
    char flipped[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Convert from hex to ASCII
    for (i = 1; i < 10; i++) {
        flipped[i] = hex % 10 + 0x30;
        hex /= 10;
        if (hex == 0) {
            break;
        }
    }

    // Flip data values to correct position in array
    for (j = 0; j < i; j++) {
        ASCII[j] = flipped[i - j];
    }

    return i;
}
