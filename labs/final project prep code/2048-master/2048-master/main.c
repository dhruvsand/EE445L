// EE 319K Intro Embedded Systems Game Design Project
// 2048 Game
// main.c
// Runs on LM4F120 or TM4C123
// Ryan Norton

// Graphic display on Kentec EB-LM4F120-L35
// Sound output to DAC
// Analog Input connected to PD2=ADC1 (1-D joystick),
// 	
// Switch input

#include "../inc/tm4c123gh6pm.h"
#include "SSD2119.h"
#include "PLL.h"
#include "sounds.h"     	// audio waveforms
#include "gameEngine.h"		// 2048 game implementation details
#include "random.h"
#include "Input.h"
#include "DAC.h"
#include "sounds.h"

void delay(int instructions);
void EnableInterrupts(void);
void Timer2_Init(unsigned long period);
void Timer1_Init(unsigned long period);
unsigned long Button1;
unsigned long Button2;
unsigned int score;
unsigned int highscore = 0;
int pauseMode = 0;
int elapsedTime = 0;
int timeReady = 0;
int playSound = 0;
int soundIndex = 0;
int gameOver = 0;

int main(void) {int addScore, position, pauseSel;
	
	// Initializations
	PLL_Init();  			// Clock set at 80 MHz
  	LCD_Init(); 
	Board_Init();
	Input_Init();
	DAC_Init();
	Random_Init(NVIC_ST_CURRENT_R); 
	Timer2_Init(80000000); 	// time interrupt
	Timer1_Init(2000);		// sound interrupt
	EnableInterrupts();
	generateRandomTile();
	drawAllTiles();
	writeScore(0);
	writeHighscore(0);
	writeTime(0);
	displayHighestTile();
	
	while(1){ 
		
		// draw arrow if ready
		if (arrowReady == 1){
			// acknowledge flag
			arrowReady = 0;
			// draw arrow
			drawArrow();
		}
		
		// write time if ready
		if (timeReady == 1){
			// acknowledge flag
			timeReady = 0;
			// write time
			writeTime(elapsedTime);
		}
		
		// Play mode and button1 is pushed
		if(Button1 && !pauseMode && !gameOver){		
			// Play sound
			playSound = 1;
			// shift and merge tiles towards arrow
			position = getSliderPosition();
			if (position == 1){
				shiftLeft();
				addScore = mergeLeft();
				shiftLeft();
			}
			else if (position == 2){
				shiftUp();
				addScore = mergeUp();
				shiftUp();	
			}
			else if (position == 3){
				shiftRight();
				addScore = mergeRight();
				shiftRight();
			}
			else {
				shiftDown();
				addScore = mergeDown();
				shiftDown();
			}
			
			eraseBoard();
			drawAllTiles();
			
			// update score
			score += addScore;
			addScore = 0;
			writeScore(score);
			displayHighestTile();
			
			// delay before adding new tile
			delay(200);
			
			// create new tile
			if (countEmptyTiles() != 0){
				generateRandomTile();
			}
			drawAllTiles();
			
			// update highest tile image
			displayHighestTile();

			// check if game over
			if (checkGameOver() == 1){
				gameOver = 1;
			}
			
			// unset flag
			Button1 = 0;
    }
		
		// button 2 is pause
		else if (Button2 && !gameOver && !pauseMode){
			pauseMode = 1;
			pauseSel = 0;
			
			// disable arrow and timer
			NVIC_ST_CTRL_R = 0;
			TIMER2_CTL_R = 0x00000000;
			LCD_DrawFilledRect(prevX,prevY,20,20,BLACK);
			
			// draw pause mode screen
			drawPauseMode();
			
			// acknowledge button 
			Button2 = 0;
			
			// wait until button is pushed
			while (pauseMode){
			
				// Button 1 selects current pause selection button
				if (Button1){ 
					
					// acknowledge button
					Button1 = 0;
					Button2 = 0;
					pauseMode = 0;
					
					// if pause selection = "continue" (pauseSel = 0), continue with game
					if (pauseSel == 0){
						
						// redraw screen
						eraseBoard();
						drawAllTiles();
						
						// enable gameplay
						NVIC_ST_CTRL_R = 0x07;
						TIMER2_CTL_R = 0x00000001;
					}
				
					// if pause selection = "restart" (pauseSel = 0), end game
					else if (pauseSel == 1){
						if (score > highscore){
							writeHighscore(score);
						}
						score = 0;
						eraseScore();
						writeScore(0);
						clearBoard();
						eraseBoard();
						pauseMode = 0;
						generateRandomTile();
						drawAllTiles();
						elapsedTime = 0;
						eraseTime();
						writeTime(0);
						displayHighestTile();
						NVIC_ST_CTRL_R = 0x07;
						TIMER2_CTL_R = 0x00000001;
					}
				}
			
				// Button 2 changes pause selection
				else if (Button2){
					Button2 = 0;
					if (pauseSel == 0){
						pauseSel = 1;
						LCD_DrawRect(144,112,58,16,BLACK);
						LCD_DrawRect(222,112,51,16,WHITE);
					}
					else if (pauseSel == 1){
						pauseSel = 0;
						LCD_DrawRect(222,112,51,16,BLACK);
						LCD_DrawRect(144,112,58,16,WHITE);						
					}
				}
			}
		}
		
		// game over
		if (gameOver == 1){
			NVIC_ST_CTRL_R = 0;
			TIMER2_CTL_R = 0x00000000;
			LCD_DrawFilledRect(156,38,100,20,BLACK);
			LCD_SetTextColor(255,255,240);	
			LCD_Goto(30,5);
			printf("GAME OVER");
			while (Button1 == 0 && Button2 == 0) {}
			Button1 = 0;
			Button2 = 0;
			LCD_DrawFilledRect(prevX,prevY,20,20,BLACK);
			drawGameOver(score, elapsedTime);
			if (score > highscore){
				writeHighscore(score);
				gameOverHighscore(score);
			}
			// wait til button is pushed
			while (Button1 == 0 && Button2 == 0){}
			// acknowledge buttons
			Button1 = 0;
			Button2 = 0;
			// start new game
			score = 0;
			eraseScore();
			writeScore(0);
			clearBoard();
			eraseBoard();
			pauseMode = 0;
			generateRandomTile();
			drawAllTiles();
			elapsedTime = 0;
			eraseTime();
			writeTime(0);
			displayHighestTile();
			NVIC_ST_CTRL_R = 0x07;
			TIMER2_CTL_R = 0x00000001;
			
			// finish game over mode
			gameOver = 0;
		}
  }	
}
void delay(int ms){unsigned int i;
	for (i=ms*8000; i>0; i--){}
}
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  NVIC_EN0_R |= 1<<23;          // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer1_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate timer1
  delay = SYSCTL_RCGCTIMER_R;
  TIMER1_CTL_R = 0x00000000;    // 1) disable timer1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear timer3A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  NVIC_EN0_R |= 1<<21;          // 9) enable IRQ 35 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable timer1A
}

void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   	// acknowledge timer2A timeout
  elapsedTime ++;
	timeReady = 1;
}

void Timer1A_Handler(void){ 
  TIMER1_ICR_R = 0x00000001;   	// acknowledge timer1A timeout
	if (playSound == 1){
		if (soundIndex < 982){
			DAC_Out(fastinvader1[soundIndex]);
			soundIndex ++;
		}
		else{
			soundIndex = 0;
			playSound = 0;
		}
	}
}

void GPIOPortC_Handler(void){
	GPIO_PORTC_ICR_R = 0x20;   // acknowledge flag 5
	delay(100);
	Button1 = 1; // trigger
}

void GPIOPortD_Handler(void){
	GPIO_PORTD_ICR_R = 0x40;   // acknowledge flag 3
	delay(100);
	Button2 = 1; // trigger
}

