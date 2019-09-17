// Lab1.c
// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// January 17, 2018
// Possible main program to test the lab
// Feel free to edit this to match your specifications

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "../inc/tm4c123gh6pm.h"
void DelayWait10ms(uint32_t n);
void PortF_Init(void);
// const will place these structures in ROM

struct outTestCase1{    // used to test routines
  int32_t InNumber;     // test input number
  char OutBuffer[12];   // Output String  
};
typedef const struct outTestCase1 outTestCaseType1;
outTestCaseType1 outTests1[13]={ 
{     0,  " =   0.00?\r" }, //     0/100 = 0.00  
{     4,  " =   0.04?\r" }, //     4/100 = 0.04  
{    -5,  " =  -0.05?\r" }, //    -5/100 = -0.05
{    78,  " =   0.78?\r" }, //    78/100 = 0.78
{  -254,  " =  -2.54?\r" }, //  -254/100 = -2.54
{   999,  " =   9.99?\r" }, //   999/100 = 9.99
{ -1000,  " = -10.00?\r" }, // -1000/100 = -10.00
{  1234,  " =  12.34?\r" }, //  1234/100 = 12.34
{ -5678,  " = -56.78?\r" }, // -5678/100 = -56.78
{ -9999,  " = -99.99?\r" }, // -9999/100 = -99.99
{  9999,  " =  99.99?\r" }, //  9999/100 = 99.99
{ 10000,  " =  **.**?\r" }, // positive error
{-10000,  " = -**.**?\r" }  // negative error
};

// const will place these structures in ROM
struct outTestCase2{   // used to test routines
  uint32_t InNumber;   // test input number
  char OutBuffer[12];  // Output String  
};
typedef const struct outTestCase2 outTestCaseType2;

outTestCaseType2 outTests2[14]={ 
{     0,  " =   0.00?\r" }, //      0/64 = 0.00  
{     1,  " =   0.02?\r" }, //      4/64 = 0.02  
{    10,  " =   0.16?\r" }, //     10/64 = 0.16
{   200,  " =   3.13?\r" }, //    200/64 = 3.13
{   254,  " =   3.97?\r" }, //    254/64 = 3.97
{   505,  " =   7.89?\r" }, //    505/64 = 7.89
{  1070,  " =  16.72?\r" }, //   1070/64 = 10.72
{  5120,  " =  80.00?\r" }, //   5120/64 = 80.00
{ 12184,  " = 190.38?\r" }, //  12184/64 = 190.38
{ 26000,  " = 406.25?\r" }, //  26000/64 = 406.25
{ 32767,  " = 511.98?\r" }, //  32767/64 = 511.98
{ 34567,  " = 540.11?\r" }, //  34567/64 = 540.11
{ 63999,  " = 999.98?\r" }, //  63999/64 = 999.98
{ 64000,  " = ***.**?\r" }  // error
};
#define PF2   (*((volatile uint32_t *)0x40025010))
#define PF3   (*((volatile uint32_t *)0x40025020))
#define PF4   (*((volatile uint32_t *)0x40025040))

void Pause(void){
  while(PF4==0x00){ 
    DelayWait10ms(10);
  }
  while(PF4==0x10){
    DelayWait10ms(10);
  }
}
// 180 points on a circle of radius 2.000
const int32_t CircleXbuf[180] = { 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
};
const int32_t CircleYbuf[180] = {0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70
};
// 50 points of a start
const int32_t StarXbuf[50] = {0, -6, -12, -18, -24, -30, -35, -41, -47, -53, 59, 53, 47, 41, 35, 30, 24, 18, 12, 6, 95, 76, 57, 38, 19, 0, -19, -38, -57, -76, -59, -44, -28, -13, 3, 18, 33, 49, 64, 80, -95, -80, -64, -49, -33, -18, -3, 13, 28, 44
};
const int32_t StarYbuf[50] = {190, 172, 154, 136, 118, 100, 81, 63, 45, 27, 9, 27, 45, 63, 81, 100, 118, 136, 154, 172, 121, 121, 121, 121, 121, 121, 121, 121, 121, 121, 9, 20, 31, 43, 54, 65, 76, 87, 99, 110, 121, 110, 99, 87, 76, 65, 54, 43, 31, 20
};

int main(void){
	uint32_t i=0;

  PLL_Init(Bus80MHz); 
  PortF_Init();
  ST7735_InitR(INITR_REDTAB);
//	while(1)
//	ST7735_FillScreen(0);
  while(1){
    ST7735_FillScreen(ST7735_BLACK); 
    ST7735_SetCursor(0,0);
 //   printf("Lab 1\rST7735_sDecOut2\r");
    for(i=0; i<13; i++){
      ST7735_sDecOut2(outTests1[i].InNumber);  // your solution
      ST7735_OutString((char*)outTests1[i].OutBuffer); // expected solution
    }
    Pause();
  
    ST7735_FillScreen(0);  // set screen to black
    ST7735_SetCursor(0,0);
 //   printf("ST7735_uBinOut6\r");
    for(i=0; i<14; i++){
      ST7735_uBinOut6(outTests2[i].InNumber);  // your solution
      ST7735_OutString((char*)outTests2[i].OutBuffer); // expected solution
    }
    Pause();
    
    ST7735_XYplotInit("Circle",-2500, 2500, -2500, 2500);
    ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf);
    Pause();
    
    ST7735_XYplotInit("Star- upper right",-450, 150, -400, 200);
    ST7735_XYplot(50,(int32_t *)StarXbuf,(int32_t *)StarYbuf);
    Pause(); 
  } 
} 

// PF4 is input
// Make PF2 an output, enable digital I/O, ensure alt. functions off
void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}


int main4(void){ 
  PortF_Init();
  while(1){
    DelayWait10ms(1);
    PF2 ^= 0x04;
  }
}
int main5(void){ 
  PortF_Init();
  while(1){
    DelayWait10ms(100);
    PF2 ^= 0x04;
  }
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
