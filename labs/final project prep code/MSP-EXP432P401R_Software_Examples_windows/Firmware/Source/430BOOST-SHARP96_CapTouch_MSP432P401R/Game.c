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
// Game.c
//! \brief User Experience Code for the MSP-EXP432P401R
//!
//!        Program Execution: Objective is to keep the ball from passing either paddle
//!        on the sides of the screen by using the CapTouch sliders on each side.
//!        When the ball is hit by the paddle, points are awarded (respective of game
//!        difficulty). User plays until 5 lives are lost.
//!
//!        Directions for use: Upon entry, a menu is displayed for choosing game
//!        difficulty. When select button is pressed, the game begins and user has 5
//!        lives to hit the ball as many times as possible. Game can be ended by
//!        pressing select button again. User will be returned to difficulty menu. If
//!        menu button is pressed, user exits game app entirely.
//!
//!        July 2015
//!
//*******************************************************************************

//*******************************************************************************
// Includes
//*******************************************************************************
#include "grlib.h"
#include "Sharp96x96.h"
#include "CTS_LCDSliders.h"

#include "Game.h"
#include "msp432p4xx_EXP.h"
#include "myTimer.h"
#include "UpdateDisplay.h"

//*******************************************************************************
// Defines
//*******************************************************************************

//! \brief Value to select Easy Mode
//!
#define EASY_MODE    2

//! \brief Value to select Normal Mode
//!
#define NORMAL_MODE  4

//! \brief Value to select Hard Mode
//!
#define HARD_MODE    6

//! \brief Value to select Insane Mode
//!
#define INSANE_MODE  8

//! \brief Value used to select game menu
//!
#define SEL_MENU_MODE  2

//! \brief Value to select game mode
//!
#define SEL_GAME_MODE  4


//*******************************************************************************
// Game Global Variables
//*******************************************************************************

//! \brief High Score for game
//!
uint32_t Game_highScore;

//! \brief Max and min y for ball, leave buffer for size
//!
volatile uint8_t MIN_Y = 14 + 3;

//! \brief  Max Y of ball itself
//!
volatile uint8_t MAX_Y = 82 - 3;

//! \brief Max and min x for ball, leave buffer for size
//!
volatile uint8_t MIN_X = 6 + 3;

//! \brief Maximum x location of ball and paddle
//!
volatile uint8_t MAX_X = 89 - 3;

//! \brief horizontal position of ball
//!
volatile uint8_t x;

//! \brief vertical position of ball
//!
volatile uint8_t y;

//! \brief horiz direction/speed of ball, start positive
//!
volatile uint8_t dx;

//! \brief vert direction/speed of ball, start positive
//!
volatile uint8_t dy;

//! \brief flag to slow down horizontal speed
//!
volatile uint8_t halfstep = 1;

//! \brief Game mode variable
//!
volatile uint8_t gameMode;

//! \brief Speed of ball in x direction
//!
volatile uint8_t gameSpeedX;

//! \brief Speed of ball in y direction
//
volatile uint8_t gameSpeedY;

//! \brief Left paddle location
//!
uint8_t paddle0Loc;

//! \brief Right paddle location
uint8_t paddle1Loc;

//! \brief Number of lives remaining in current game
//!
uint16_t lives;

//! \brief Current game score
//!
uint16_t score;

//! \brief Multiplier used to award points
//!        based on game mode
uint16_t scoreMultiplier;

//! \brief Number of times missed
//!
uint8_t missed;

//*******************************************************************************
// Function Prototypes
//*******************************************************************************

//! \brief Collision detection algorithm
//!
static void SliderBall_collisionDetect(uint8_t position_l, uint8_t position_r);

//! \brief Used to choose game parameters
//!
static uint8_t SliderBall_chooseGameMode(void);

//! \brief Rounding algorithm to try and make paddle positioning smoother
//!
static void SliderBall_roundingAlgorithm(void);



//! \brief  SliderBall Game Application - Push button to start. Every time
//!         the ball hits a paddle, 5*scoreMultiplier points is added. User
//!        gets 5 lives. Every miss costs a life. Once all 5 lives are lost,
//!        game is over, the user's score is displayed, and the user will
//!        return to the game menu, to again choose the difficulty.
//!
void SliderBall_runApp(void)
{
    while (mode == APP_SLIDERBALL) {
        select = SEL_MENU_MODE;
        gameMode = SliderBall_chooseGameMode();
        gameSpeedX = dx;
        gameSpeedY = dy;

        if (select) {                                                   // Only display if main menu button not pressed
            GrClearDisplay(&g_sContext);                                  // init buffer (get ready for game)
            GrContextFontSet(&g_sContext, &g_sFontCmss12);
            GrLineDrawH(&g_sContext, 0, 96, 13);                          // Draw Upper Bound Line
            GrLineDrawH(&g_sContext, 0, 96, 83);                          // Draw Lower Bound Line

            lives = 5; score = 0; dy = 3; missed = 0;                   // Reset game
            y = 44; x = 32;                                             // Initialize ball in the middle of the screen
            LCD_updateLives(lives);
            LCD_updateScore(score);
        }

        while ((select == SEL_GAME_MODE) && (lives != 0)) {
            CTS_getReading();
            SliderBall_roundingAlgorithm();

            LCD_drawSliderBall((uint8_t)paddle0Loc, (uint8_t)paddle1Loc, x, y);

            if (missed) {             // If ball was missed, allow for reset time
                LCD_updateLives(lives);
                TimerA1_sleep(150);
                missed--;
            }

            // For easier levels, sleep between ball movements to slow gameplay
            switch (gameMode) {
            case EASY_MODE:             // Slowest speed
                TimerA1_sleep(125);
                break;
            case NORMAL_MODE:             // Normal Speed
                TimerA1_sleep(50);
                break;
            case HARD_MODE:             // Fast
                TimerA1_sleep(6);
                break;
            case INSANE_MODE:             // Super fast
                __no_operation();
                break;
            default: break;
            }
            SliderBall_collisionDetect((uint8_t)paddle0Loc, (uint8_t)paddle1Loc);
        }

        if (select) {                                   // Do not display game over if main menu button pressed
            if (score > Game_highScore) {                    // Show "NEW HIGH SCORE" if high score is beaten
                LCD_displayNewHighScore(score);
                TimerA1_sleep(8192);                        // 2s
            }
            LCD_displayGameOver(score);
            TimerA1_sleep(8192);             // 2s
        }
    }
}


//! \brief  Collision detection for the SliderBall game. When the ball
//!         reaches the end of the screen, it checks if the ball has made
//!         contact with the paddle on that side, if so, the ball turns
//!         around and the player is awarded points (amount of points is
//!         determined by game mode). If not, the ball returns to the center
//!         and the player loses a life.
//!
//! \param  position_l and position_r - position variables passed in by the
//!         CapTouch functions
//!
static void SliderBall_collisionDetect(uint8_t position_l, uint8_t position_r)
{
    uint8_t leftPaddle, rightPaddle;

    leftPaddle = (uint8_t)(105 - (52 * position_l / 18));
    rightPaddle = (uint8_t)(105 - (52 * position_r / 18));

    x += dx;
    y += dy;

    // Check for collision with paddles
    if ((x > MAX_X) && (dx == gameSpeedX)) {     // Ball on right side, check right paddle
        if ( ((y > rightPaddle) && ((y - rightPaddle) < 11))
             || ((y < rightPaddle) && ((rightPaddle - y) < 11))
             || (y == rightPaddle)) {
            dx = -dx;             //paddle hit, turn ball around, add points
            x = MAX_X;
            score += 5 * scoreMultiplier;
            LCD_updateScore(score);
        }
        else {           // miss, reset ball, lose life
            x = 32;
            y = 44;
            dy = gameSpeedY;
            missed++;
            lives--;
        }
    }
    else if ((x < MIN_X) && (dx != gameSpeedX)) {      // Ball on left side, check left paddle
        if ( ((y > leftPaddle) && ((y - leftPaddle) < 12))
             || ((y < leftPaddle) && ((leftPaddle - y) < 12))
             || (y == leftPaddle)) {
            dx = -dx;             //paddle hit, turn ball around, add points
            x = MIN_X;
            score += 5 * scoreMultiplier;
            LCD_updateScore(score);
        }
        else {           // miss, reset ball, lose life
            x = 64;
            y = 44;
            dy = gameSpeedY;
            missed++;
            lives--;
        }
    }

    // Check for collision with upper/lower bounds
    if (y >= MAX_Y) {     // hit edge, change direction
        y = MAX_Y;
        dy = -dy;
    }
    else if (y < MIN_Y) {
        y = MIN_Y;
        dy = -dy;
    }

}


//! \brief  Chooses the game mode. Valid choices are EASY_MODE, NORMAL_MODE,
//!         HARD_MODE, and INSANE_MODE.
//!
//! \return unsigned char - speed choice (1, 2, 3, or 4)
//!
static uint8_t SliderBall_chooseGameMode(void)
{
    uint8_t choice = 2;
    uint8_t lastChoice = 0;

    while (select == SEL_MENU_MODE) {

    	//Detect swipe
    	uint8_t dir = CTS_getSwipe(0, 10);
    	//Increment location if swipe up or decrement location if swipe down
    	if(dir == 2)
    	{
    		choice -= 2;
    	}
    	else if(dir == 0)
    	{
    		choice += 2;
    	}
    	//Adjust if at endpoints of list
    	if(choice > INSANE_MODE) choice = INSANE_MODE;
    	else if(choice < EASY_MODE) choice = EASY_MODE;

    	//Update screen if selection changed
        if(choice != lastChoice)
        {
        	LCD_sliderBallMenu(choice);
        }
        lastChoice = choice;
    }

    //Set game parameters based on final selection
    if (choice == INSANE_MODE)
    {
    	scoreMultiplier = 10;
        dx = 4;
        dy = 6;
    }
    else if (choice == HARD_MODE)
    {
        scoreMultiplier = 4;
        dx = 2;
        dy = 3;
    }
    else if (choice == NORMAL_MODE)
    {
        scoreMultiplier = 2;
        dx = 2;
        dy = 3;
    }
    else
    {
    	scoreMultiplier = 1;
        dx = 2;
        dy = 3;
    }
    return choice;
}

//! \brief  Round slider positions so gameplay is easier/better. Top and
//!         bottom of CapTouch sliders are cut off, as they are less responsive
//!         to touch and make gameplay more difficult
//!
static void SliderBall_roundingAlgorithm(void)
{
    // Rounding algorithm to make gameplay better
	if (sliderPos[0] - 30 < 29) {
	      paddle0Loc = 29;
	 }
	 else if (sliderPos[0] - 30 > 69) {
	      paddle0Loc = 69;
	 }
	 else { paddle0Loc = sliderPos[0] - 30; }

	 if (sliderPos[1] - 30 < 29) {
		 paddle1Loc = 29;
	 }
	 else if (sliderPos[1] - 30 > 69) {
	     paddle1Loc = 69;
	 }
	 else { paddle1Loc = sliderPos[1]-30; }

	 paddle0Loc = 11 + (paddle0Loc - 29)*(29-11)/(69-29);
	 paddle1Loc = 11 + (paddle1Loc - 29)*(29-11)/(69-29);
}
