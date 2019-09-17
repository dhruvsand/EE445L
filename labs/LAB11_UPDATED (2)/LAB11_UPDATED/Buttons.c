
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include <stdbool.h>
//#include "Buttons.h"
#include "SSD2119.h" 

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

void ST7735_DrawCue(float xcen, float ycen,float xbond, float ybond, int16_t textColor);
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
int previous_press;

void Buttons_Init(void) {
	previous_press = 0;
  SYSCTL_RCGCGPIO_R |= 0x20;     // 1) activate Port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
                                 // 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
  GPIO_PORTF_CR_R |= (SW1|SW2);  // 2b) enable commit for PF4 and PF0
                                 // 3) disable analog functionality on PF4 and PF0
  GPIO_PORTF_AMSEL_R &= ~(SW1|SW2);
                                 // 4) configure PF0 and PF4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;
  GPIO_PORTF_DIR_R &= ~(SW1|SW2);// 5) make PF0 and PF4 in (built-in buttons)
                                 // 6) disable alt funct on PF0 and PF4
  GPIO_PORTF_AFSEL_R &= ~(SW1|SW2);
//  delay = SYSCTL_RCGC2_R;        // put a delay here if you are seeing erroneous NMI
  GPIO_PORTF_PUR_R |= (SW1|SW2); // enable weak pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= (SW1|SW2); // 7) enable digital I/O on PF0 and PF4

}
// bool debounce[4] = { hours, minutes, arm_alarm, set_alarm };
uint16_t debounce[4];

#define CYCLE_WAIT 5

void delay(void){
	int i;
	for(i = 0; i< 10000; i++){
		i += 0;
	}
}

void Buttons_ReadInput(void) {
	int32_t data = GPIO_PORTF_DATA_R;
	if((data & 0x01) != 0)
		debounce[0] = (data & 0x01)*CYCLE_WAIT;
	if((data & 0x02) != 0)
		debounce[1] = ((data & 0x2) >> 1)*CYCLE_WAIT;
	if((data & 0x04) != 0)
		debounce[2] = ((data & 0x4) >> 2)*CYCLE_WAIT;
	if((data & 0x08) != 0)
		debounce[3] = ((data & 0x8) >> 3)*CYCLE_WAIT;
}

extern bool Play_mode;
void Pause(void);
void Play(void);
void Rewind(void);


void Buttons_Pressed(uint32_t button) {
}

//------------Board_Input------------
// Read and return the status of the switches.
// Input: none
// Output: 0x01 if only Switch 1 is pressed
//         0x10 if only Switch 2 is pressed
//         0x00 if both switches are pressed
//         0x11 if no switches are pressed
uint32_t Board_Input(void){
  return SWITCHES;
}

void Buttons_10ms_Handler(void) {
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
}
int findLargest(int Buffer[], int buffer_length){
	int max_value;
	int i = 0;
	for(i = 0; i< buffer_length; i++){
		int local_max = Buffer[i];
		if(local_max < max_value){
			max_value = local_max;
		}
	}
	return(max_value);
	
}

int findSmallest(int Buffer[], int buffer_length){
	int min_value;
	int i = 0;
	for(i = 0; i< buffer_length; i++){
		int local_min = Buffer[i];
		if(local_min > min_value){
			min_value = local_min;
		}
	}
	return(min_value);
	
}


void Zoom(int Buffer[], int buffer_length ) {
	int previous_values_x;
	int previous_values_y;
	int smallest;
	int largest;
	int offset;
	int iterations = 320/buffer_length;
	LCD_ColorFill(0xFFFF);	
	previous_values_x = 0;
	previous_values_y = 0;
	largest = findLargest(Buffer, buffer_length);
	smallest = findSmallest(Buffer, buffer_length);
	offset = largest - smallest; 
	
				
				
				for(int i=0; i< (iterations) ; i++){
					int rise;
					int run;
					int ADCvalue= Buffer[i];
					if(ADCvalue==-1){
						i=i-1;
						continue;
					}
					//Subtract by smallest value, divide by offset between largetand smallest, multiply by 220 
					LCD_SetCursor(0,0);
					LCD_PrintInteger(ADCvalue);
					//int temp = Temperature_Convert(ADCvalue);
					LCD_PrintString("\n");
					LCD_PrintInteger(ADCvalue/100);
					LCD_PrintString(".");
					LCD_PrintInteger(ADCvalue%100);
						unsigned short yval= 220* ADCvalue/ 4095;
					yval = (((yval -smallest) / offset)* 220);
					ST7735_Line(previous_values_x, previous_values_y, i* (320/iterations), yval, 0xFF);
						LCD_DrawPixel(i* (320/iterations),yval, 0x1234 );
					previous_values_x = i* (320/iterations);
					previous_values_y = yval;
						for(int j=0; j<10000;j++){
							int y=0;
							y++;
							}
				}
  }

void Pause(void) {
	uint32_t data = Board_Input();//on-board switch 0 
	if (data != 0 && previous_press == 0){
		while(data != 0){
			data = Board_Input();
			delay();
		}
		previous_press = 1;
	}
	if (data != 0 && previous_press == 1){
		previous_press = 0;
	}
}


void Rewind(void) {
	Pause();

}

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	if((x1>127)||(x2>127)||(y1>159)||(y2>159)){
		return;
	}
	
//	while((x1!=x2)||(y1!=y2)){
//		LCD_DrawPixel(int16_t x1, int16_t y1, uint16_t color);
//		
//	}
	
	ST7735_DrawCue(x1, y1,x2, y2, color);

}

void ST7735_DrawCue(float xcen, float ycen,float xbond, float ybond, int16_t textColor){
 
  float xtemp,ytemp;
	xtemp=xcen;
	ytemp=ycen;
	LCD_DrawPixel(xtemp, ytemp, textColor);
	if(xcen<xbond)
		xtemp++;
	else if (xcen>xbond)
		xtemp--;
	else if (ycen<ybond)
		ytemp++;
	else
		ytemp--;
		      LCD_DrawPixel(xtemp, ytemp, textColor);
	
	
	
	if((xcen<=xbond)&&(ycen>=ybond)){//1st quadrant
	while((xtemp!=xbond)||(ytemp!=ybond)){
		if(ytemp==ybond)
		{while(xtemp!=xbond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			xtemp++;
		}
			return;
		}
		if(xtemp==xbond)
		{while(ytemp!=ybond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			ytemp--;
		}
			return;
		}
		
		if(((-ytemp+ycen)/(xtemp-xcen))>((-ybond+ycen)/(xbond-xcen)))
			xtemp++;
			else 
				ytemp--;
		
 
          LCD_DrawPixel(xtemp, ytemp, textColor);
	}
	LCD_DrawPixel(xbond, ybond, textColor);
	return;
}
	


		if((xcen>=xbond)&&(ycen>=ybond)){//2nd quadrant
	while((xtemp!=xbond)||(ytemp!=ybond)){
		if(ytemp==ybond)
		{while(xtemp!=xbond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			xtemp--;
		}
			return;
		}
		if(xtemp==xbond)
		{while(ytemp!=ybond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			ytemp--;
		}
			return;
		}
		
		if(((-ytemp+ycen)/(xcen-xtemp))>((-ybond+ycen)/(xcen-xbond)))
			xtemp--;
			else 
				ytemp--;
			
		
 
          LCD_DrawPixel(xtemp, ytemp, textColor);
	}
		LCD_DrawPixel(xbond, ybond, textColor);
	return;
}
				if((xcen>=xbond)&&(ycen<=ybond)){//3rd quadrant
	while((xtemp!=xbond)||(ytemp!=ybond)){
		if(ytemp==ybond)
		{while(xtemp!=xbond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			xtemp--;
		}
			return;
		}
		if(xtemp==xbond)
		{while(ytemp!=ybond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			ytemp++;
		}
			return;
		}
		if(((-ycen+ytemp)/(xcen-xtemp))>((-ycen+ybond)/(xcen-xbond)))
			xtemp--;
			else 
				ytemp++;
		
 
          LCD_DrawPixel(xtemp, ytemp, textColor);
	}
		LCD_DrawPixel(xbond, ybond, textColor);
	return;
}
					if((xcen<=xbond)&&(ycen<=ybond)){//4th quadrant
	while((xtemp!=xbond)||(ytemp!=ybond)){
		if(ytemp==ybond)
		{while(xtemp!=xbond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			xtemp++;
		}
			return;
		}
		if(xtemp==xbond)
		{while(ytemp!=ybond){
			LCD_DrawPixel(xtemp, ytemp, textColor);
			ytemp++;
		}
			return;
		}
		
		if(((-ycen+ytemp)/(-xcen+xtemp))>((-ycen+ybond)/(-xcen+xbond)))
			xtemp++;
			else 
				ytemp++;
		
 
          LCD_DrawPixel(xtemp, ytemp, textColor);
	}
		LCD_DrawPixel(xbond, ybond, textColor);
	return;
}
					}
