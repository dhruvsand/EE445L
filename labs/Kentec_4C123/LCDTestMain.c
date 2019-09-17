#include "../inc/tm4c123gh6pm.h"
#include "SSD2119.h"
#include "PLL.h"
#include <stdint.h>

#include "random.h"
#include "LCDTests.h"


void ADC0_InitSWTriggerSeq3_Ch9(void){ 
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
                                  // 1) activate clock for Port E
  SYSCTL_RCGCGPIO_R |= 0x10;
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};
  GPIO_PORTE_DIR_R &= ~0x10;      // 2) make PE4 input
  GPIO_PORTE_AFSEL_R |= 0x10;     // 3) enable alternate function on PE4
  GPIO_PORTE_DEN_R &= ~0x10;      // 4) disable digital I/O on PE4
  GPIO_PORTE_AMSEL_R |= 0x10;     // 5) enable analog functionality on PE4
    
//  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};    // good code, but not yet implemented in simulator


  ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 9;             //    set channel
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
}


//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC0_InSeq3(void){  uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}







int main(void){unsigned long i=0;
  // Set the clocking to run at 80MHz from the PLL.
  PLL_Init(Bus80MHz);

    // Initialize LCD
  LCD_Init();
    
 
    // Initialize RNG
    Random_Init(121213);

    // Initialize touchscreen GPIO
    
	
	
	Touch_Init();

	
	//ADC0_InitSWTriggerSeq3_Ch9();
	
      
    for(;;) {
        
        // TO PLAY WITH THESE, EMPTY THE SYSTICK OF TIMED
        // STUFF AND UNCOMMENT ONE TEST AT A TIME
        // - SOME MAY BE BROKEN NOW
        
        //This file was modified to make a short animated
        //demonstration of the LCD driver.  Each test is
        //shown at least briefly, then the best footage
        //will be selected in post-production.
        //NOTE: To return this file to its previous
        //condition, un-comment the following lines in the
        //SysTick_Handler:
        //touchDebug();
        //LCD_DrawBMP(testbmp, 64,64);
        //Beginning of animated demonstration
		//	while(1){
			//	touchDebug();
		//	LCD_SetCursor(0,0);
				//LCD_PrintInteger((long)ADC0_InSeq3());
				
			//	for(int i=0; i< 320 ; i++){
				
			//	unsigned short yval= 320* ADC0_InSeq3()/ 4095;
					
					
		//			LCD_DrawPixel(i,yval, 0x1234 );
					
	//				for(int j=0; j<100000;j++){
	//				int y=0;
	//				}
					
				
				
				
		//		}
				
			//	LCD_ColorFill(0xFFFF);
				
				
				
				
				
				
		//	}
			
			
			
			
        for(i=0; i<1000; i=i+1){
          RandomRectangles();
        }
        LCD_ColorFill(convertColor(0, 0, 255));
        for(i=0; i<1250; i=i+1){
          MovingColorBars();
        }
        for(i=0; i<15; i=i+1){
          LineSpin();
        }
        LCD_ColorFill(convertColor(0, 0, 0));
        for(i=0; i<2000; i=i+1){
          printfTest();
        }
        for(i=0; i<250; i=i+1){
          RandomCircle();
        }
        for(i=0; i<6500; i=i+1){
          Random4BPPTestSprite();
        }
        //End of animated demonstration
        //PrintAsciiChars();
        //BlastChars();
        //TestStringOutput();
        //RandomRectangles();
        //MovingColorBars();
        //RandomColorFill();
        //BWWaitFill();
        //LineSpin();
        //printfTest();
        //charTest();
        //RandomCircle();
        //Random4BPPTestSprite();

    }
    
}


