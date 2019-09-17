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
// msp432p4xx_EXP.c
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Initialization, main menu, and button control functions
//!
//!        July 2015
//!        Rev 1.0
//!
//*******************************************************************************

//*******************************************************************************
// Includes
//*******************************************************************************
#include "msp.h"
#include "msp432p4xx_EXP.h"

#include "driverlib.h"
#include "Sharp96x96.h"
#include "CTS_LCDSliders.h"

#include "ClockApp.h"
#include "Game.h"
#include "myTimer.h"
#include "UpdateDisplay.h"

//*******************************************************************************
// Globals
//*******************************************************************************

//! \brief Application mode, initialized to Main Menu
//!
volatile uint8_t mode = 2;

//! \brief Select button flag
//!
volatile uint8_t select = 0;

//! \brief Graphics Context used to update LCD display
//!
Graphics_Context g_sContext;

//*******************************************************************************
// Function Prototypes
//*******************************************************************************

//! \brief Initializes system clocks, I/Os, interrupts, etc. to run application
void System_init(void);

//! \brief Draws start-up images, initializes Slider counts and High Score of
//!        SliderBall game, and enters appropriate routine to
//!        run selected application
//!
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

	Game_highScore = 0;

    // Start up normally
    System_init();   // Initialize board

    // Display Launchpad Logo
    LCD_drawRocket();
    TimerA1_sleep(8192);         // 2s

    // Display TI Logo
    LCD_drawTILogo();
    TimerA1_sleep(8192);   		// 2s

    // Write "User Experience" on LCD
    LCD_introWrite();
    TimerA1_sleep(12288);         // 3s

    // Init CapTouch Sliders
    TI_CAPT_Init_Baseline(&slider0);
    TI_CAPT_Init_Baseline(&slider1);

    TI_CAPT_Update_Baseline(&slider0, 10);
    TI_CAPT_Update_Baseline(&slider1, 10);

    while (1) {
        // Choose which app to enter based on mode variable
        switch (mode)
        {
        	case APP_MAIN_MENU:
        		mode = MainMenu();
        		break;
        	case APP_CLOCK:
        		Clock_runApp();
        		break;
        	case APP_SLIDERBALL:
        		SliderBall_runApp();
        		break;
        	default:
        		break;
        }
    }
}

//! \brief  Initializes the System to run LCD application
//!
void System_init(void)
{
    // Set the DCO to 8MHz. Do not change this frequency!
    // It impacts the cap touch scan window.
	CS_setDCOFrequency(CS_8MHZ);

    // Configure clock source and clock dividers. After this the clock configuration will be as follows:
    // ACLK=LFXT1/1=32,768Hz; SMCLK=DCOCLK/1=8MHz; and MCLK=DCOCLK/1=8MHz.
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Set all GPIO to output low to minimize current draw by eliminating floating pins.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    // Configure the left button (S2) connected to P1.1. For this enable the internal pull-up resistor and
    // setup the pin interrupt to trigger on rising edges.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    // Configure the right button (S3) connected to P1.4 For this enable the internal pull-up resistor and
    // setup the pin interrupt to trigger on rising edges.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    // CapSense Setup. GPIO pins 6.4, 6.5, 4.6, 3.6, 5.0, and 5.2 are used for capacitive touch so let's
    // switch them to inputs.
    GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);
    GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN2);

    // Enable LFXT functionality on PJ.0 and PJ.5?. For this we only need to configure PJ.0 to
    // LFXIN and the port module logic takes care of the rest.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);

    // Perform the required LFXT startup procedure now that all of the port pins are configured.
    CS_setExternalClockSourceFrequency(32768, 0);
    CS_startLFXT(CS_LFXT_DRIVE0);

    //Enable Interrupts in NVIC
    NVIC_EnableIRQ(TA0_0_IRQn);
    NVIC_EnableIRQ(TA1_0_IRQn);
    NVIC_EnableIRQ(TA3_0_IRQn);

    NVIC_EnableIRQ(RTC_C_IRQn);
    NVIC_EnableIRQ(PORT1_IRQn);

    // Initialize LCD driver and the context for the LCD
    Sharp96x96_LCDInit();
    TimerA0_enableVCOMToggle();
    GrContextInit(&g_sContext, &g_sharp96x96LCD);
    GrContextForegroundSet(&g_sContext, ClrBlack);
    GrContextBackgroundSet(&g_sContext, ClrWhite);
}


//! \brief  Display Main Menu, choose next option
//!
//! \return mode - choice selected
//!
uint8_t MainMenu(void)
{
    uint8_t appChoice = APP_CLOCK;
    uint8_t lastAppChoice = APP_MAIN_MENU;

    while (!select) {

    	// Detect swipe
    	uint8_t dir = CTS_getSwipe(0, 15);

    	//Update selection based on swipe detection
    	if(dir == 2 && appChoice == APP_SLIDERBALL)
    	{
    		appChoice = APP_CLOCK;
    	}
    	else if(dir == 0 && appChoice == APP_CLOCK)
    	{
    		appChoice = APP_SLIDERBALL;
    	}

    	//Update screen if swipe occurred
        if(appChoice != lastAppChoice)
        {
        	LCD_displayMainMenu(appChoice);
        	LCD_display();
        }
        lastAppChoice = appChoice;

    }

    select = 0;
    return appChoice;
}

//! \brief  Disables Left Switch
//!
void DisableLeftSwitch(void)
{
    GPIO_disableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
}


//! \brief  Enables Left Switch
//!
void EnableLeftSwitch(void)
{
	GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
}

//! \brief  Disables Right Switch
//!
void DisableRightSwitch(void)
{
    GPIO_disableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
}

//! \brief  Enables Right Switch
//!
void EnableRightSwitch(void)
{
	GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
}


//! \brief  Starts Debounce Timer
//!
void StartDebounceTimer(void)
{
	// Create and initializes parameter structure
	Timer_A_UpModeConfig param = {0};
	// Select ACLK source
    param.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;

    // Set debounce period
    param.timerPeriod = 18000;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;

    // Enable interrupt to signal debounce complete
    param.captureCompareInterruptEnable_CCR0_CCIE =
    		TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_SKIP_CLEAR;

    // Configure timer based on paramater structure
    Timer_A_configureUpMode(TIMER_A3_BASE, &param);
    Timer_A_startCounter(TIMER_A3_BASE,TIMER_A_UP_MODE);
}

//*******************************************************************************
// Interrupt Service Routines
//*******************************************************************************

//! \brief  Port 1 ISR for Left and Right Switch (S2 & S3) - Option Select Button
//!
#if defined (__TI_COMPILER_VERSION__) || defined (__IAR_SYSTEMS_ICC__) || ( __CC_ARM )
void PORT1_IRQHandler(void)
{
    char wakeup = 0;

    switch (P1IV) {
    case P1IV__P1IFG0: break;                    // P1.0 Interrupt
    case P1IV__P1IFG1:                           // Main Menu Button Interrupt
    	DisableLeftSwitch();
    	DisableRightSwitch();
    	StartDebounceTimer();
    	switch (mode) {
    	case APP_MAIN_MENU:
    		wakeup = 0;
    		break;
    	case APP_CLOCK:
    	    wakeup = 1;
    	    break;
    	case APP_SLIDERBALL:
    	    wakeup = 1;
    	    break;
    	default: break;
    	}
    	select = 0;                             // Reset button pushes
    	mode = APP_MAIN_MENU;                   // Reset mode
        break;
    case P1IV__P1IFG2: break;                    // P1.2 Interrupt
    case P1IV__P1IFG3: break;                    // P1.3 Interrupt
    case P1IV__P1IFG4: 							 // Option Select Interrupt
    	DisableLeftSwitch();
    	DisableRightSwitch();
    	StartDebounceTimer();
    	switch (mode) {                         // take action based on what mode the UE was in
    	case APP_MAIN_MENU:
    		select = 1;
    	    wakeup = 1;
    	    break;
    	case APP_CLOCK:
    	    if (firstEnter) {
    	    	if (select == SEL_RUN_CLOCK) {                 // stop clock and start setting parameters
    	    		RTC_C_holdClock();
    	            RTCPS1CTL = 0;
    	    	}
    	        select += 2;
    	        if (select > SEL_SET_YEAR) {                 // last parameter set, start clock
    	        	select = SEL_RUN_CLOCK;
    	        	firstEnter = 0;
    	            RTCSEC = 0;                     // Reset seconds
    	            RTC_C_definePrescaleEvent(RTC_C_PRESCALE_1, RTC_C_PSEVENTDIVIDER_128);
    	            RTC_C_enableInterrupt(RTC_C_PRESCALE_TIMER1_INTERRUPT);
    	            RTC_C_startClock();
    	        }
    	    }
    	    else {
    	    	select = SEL_RUN_CLOCK;
    	        freshFlag = 1;
    	    }
    	    wakeup = 1;
    	    break;
    	case APP_SLIDERBALL:
    	    select += 2;
    	    wakeup = 1;
    	    break;
    	default: break;
    	}
    	break;
    case P1IV__P1IFG5: break;                    // P1.5 Interrupt
    case P1IV__P1IFG6: break;                    // P1.6 Interrupt
    case P1IV__P1IFG7: break;                    // P1.7 Interrupt
    default: break;
    }

    if (wakeup) {
    	__low_power_mode_off_on_exit();
    }
}
#else
#error Compiler not supported!
#endif


//! \brief  RTC_ISR - Interrupt Service Routine for RTC
//!
#if defined (__TI_COMPILER_VERSION__) || defined (__IAR_SYSTEMS_ICC__) || ( __CC_ARM )
void RTC_C_IRQHandler(void)
{
    if(RTCPS1CTL & RT1PSIFG)
    {
    	RTC_C_clearInterruptFlag(RTC_C_PRESCALE_TIMER1_INTERRUPT);
    	__low_power_mode_off_on_exit();
    }
}
#else
#error Compiler not supported!
#endif

//! \brief  TA3_ISR - Interrupt Service Routine for debounce timer (TA3)
//!
#if defined (__TI_COMPILER_VERSION__) || defined (__IAR_SYSTEMS_ICC__) || ( __CC_ARM )
void TA3_0_IRQHandler(void)
{
	TA3CCTL0 &= ~(CCIFG);
    Timer_A_disableCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_6);
    Timer_A_stopTimer(TIMER_A3_BASE);

    // Re-enable switches since debounce period has
    // passed
    EnableRightSwitch();
    EnableLeftSwitch();
}
#else
#error Compiler not supported!
#endif
