// PWMtest.c
// Runs on TM4C123
// Test project for motor board
// ***************************************************
// ************remove R9 and R10**********************
// ***************************************************

// PB7 A+  PWM 100 Hz, right motor, PWM positive logic
// PB6 A-  regular GPIO, right motor, 0 means forward
// PB5 B+  PWM 100 Hz, left motor, PWM negative logic
// PB4 B-  regular GPIO, left motor, 1 means forward
// PD0 is servo A, 20ms period, pulse time 0.5 to 2.5ms
// PD1 is servo B (not used), 20ms period, pulse time 0.5 to 2.5ms
// Servo period = 20ms = 25000 PWM clocks (1.25MHz PWM)
//   min pulse  = 0.5ms = 625 PWM clocks
//   mid pulse  = 1.5ms = 1875 PWM clocks
//   max pulse  = 2.5ms = 3125 PWM clocks
//   because of rack-pinion restrict to 1250 to 2375
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// February 2, 2017

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2015
   Program 8.4, Section 8.3

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "PLL.h"
#include "PWM.h"
#include "tm4c123gh6pm.h"

void WaitForInterrupt(void);  // low power mode
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
// connect PF1 to PB7
void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0x01;      // enable commit for PF0

  GPIO_PORTF_DIR_R &= ~0x11;       // make PF4 input
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x1F;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x1F;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PUR_R |=0x11;          // pullup
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF00000)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off
}
#define SERVOMAX 2375
#define SERVOMID 1875
#define SERVOMIN 1275
#define SERVODELTA 100
// SW2 cycles through 12 positions
// mode=0 1875,...,1275
// mode=1 1870,...,2375
uint32_t Steering;     // 625 to 3125
uint32_t SteeringMode; // 0 for increase, 1 for decrease
#define POWERMIN 400
#define POWERMAX 12400
#define POWERDELTA 2000
uint32_t Power;
void DelayWait10ms(uint32_t n);

void WaitForTouch(void){
  while((PF0==0x01)&&(PF4==0x10)){};  // wait for switch
  DelayWait10ms(2); // debounce
  while((PF0!=0x01)||(PF4!=0x10)){}; // wait for both release
  DelayWait10ms(2); // debounce
}
// SW1 cycles through seven speeds 400,2400,4400,6400,8400,10400,12400
int main(void){
  PLL_Init(Bus80MHz);  // bus clock at 80 MHz
  PortF_Init();
  Power = 5;
  Steering = SERVOMID;  // 20ms period 1.5ms pulse
  SteeringMode = 0;
  Left_Init(12500, Power,0);          // initialize PWM0, 100 Hz
  Right_Init(12500, 12500-Power,1);   // initialize PWM0, 100 Hz
  Servo_Init(25000, Steering);   

//// test code
//  Power = POWERMAX/4; //25 % duty
//  Right_Init(12500, 12500-Power,1); // 100 Hz
//  Right_Duty(12500-Power,1);        // negative logic
//  Left_Init(12500, 12500-Power,1);  // 100 Hz
//  Left_Duty(12500-Power,1);         // negative logic
//  WaitForTouch();
//  //-------------
//  Right_Init(12500, Power,0); // 100 Hz
//  Right_Duty(Power,0);        // positive logic
//  Left_Init(12500, Power,0);  // 100 Hz
//  Left_Duty(Power,0);         // positive logic
//  WaitForTouch();
//  //-------------
//  Right_InitB(12500, 12500-Power,1); // 100 Hz
//  Right_DutyB(12500-Power,1);        // negative logic
//  Left_InitB(12500, 12500-Power,1);  // 100 Hz
//  Left_DutyB(12500-Power,1);         // negative logic
//  WaitForTouch();
//  //-------------
//  Right_InitB(12500, Power,0);  // 100 Hz
//  Right_DutyB(Power,0);         // positive logic
//  Left_InitB(12500, Power,0);   // 100 Hz
//  Left_DutyB(Power,0);          // positive logic
//  WaitForTouch();

//  Right_Init(12500, 12500-Power,1);   // 100 Hz
//  Left_Init(12500, Power,0);          // 100 Hz
//  Power = 5;
// end of test
  Power = POWERMIN;     // PWMclock at 1.25MHz
  while(1){
    while((PF0==0x01)&&(PF4==0x10)){};
    if(PF0==0){
      if(SteeringMode){
        if(Steering >= SERVOMIN+SERVODELTA){
          Steering = Steering - SERVODELTA;
        }else{
          Steering = SERVOMID; // go to center and
          SteeringMode = 0;    // switch direction
        }
      }else{
        Steering = Steering + SERVODELTA;
        if(Steering > SERVOMAX){
          Steering = SERVOMID; // go to center and
          SteeringMode = 1;    // switch direction
        }
      }
      Servo_Duty(Steering);    // SERVOMIN to SERVOMAX
      PF1 ^= 0x02;
    }
    if((PF0==0x01)&&(PF4==0)){
      Power = Power + POWERDELTA;
      if(Power > POWERMAX){
        Power = POWERMIN;      // go back to minimum
      }
      PF2 ^= 0x04;
      Left_Duty(Power,0);       // 400 to 12400 (positive logic)
    //  Right_Duty(Power,0);       // 400 to 12400 (positive logic)
      Right_Duty(12500-Power,1);  // 12400 to 400 (negative logic)
    }
    WaitForTouch();
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
