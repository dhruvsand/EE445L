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
//! \brief User Experience Code for the MSP-EXP432P401R
//!        Timer Functions
//!
//!        Created: Version 1.0:
//!
//*******************************************************************************


//*******************************************************************************
// Includes
//*******************************************************************************
#include "driverlib.h"
#include "Sharp96x96.h"

#include "myTimer.h"
#include "msp432p4xx_EXP.h"


//! \brief  Sleep in LPM0 for input amount of ACLK/8 cycles
//!
//! \param  uint32_t time - The amount of ACLK/8 cycles to wait in LPM0
//!
void TimerA1_sleep(uint32_t time)
{
	Timer_A_UpModeConfig timerA_param = {0};
    timerA_param.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    timerA_param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8;
    timerA_param.timerPeriod = time;
    timerA_param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timerA_param.captureCompareInterruptEnable_CCR0_CCIE =
    		TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    timerA_param.timerClear = TIMER_A_SKIP_CLEAR;

    Timer_A_configureUpMode(TIMER_A1_BASE, &timerA_param);
    Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_UP_MODE);

    __enable_interrupt();
    __sleep();				               // Go to LPM0
}


//! \brief  Turn on Timer A0 for VCOM toggling (for LCD)
//!
void TimerA0_enableVCOMToggle(void)
{
	Timer_A_UpModeConfig timerA_param = {0};
    timerA_param.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    timerA_param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8;
    timerA_param.timerPeriod = 4095;
    timerA_param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timerA_param.captureCompareInterruptEnable_CCR0_CCIE =
    		TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    timerA_param.timerClear = TIMER_A_SKIP_CLEAR;

    Timer_A_configureUpMode(TIMER_A0_BASE, &timerA_param);
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
}

//! \brief  Turn off Timer A1
//!
void TimerA1_turnOff(void)
{
    Timer_A_disableCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_stopTimer(TIMER_A1_BASE);
}

//! \brief  Turn off Timer A0
//!
void TimerA0_turnOff(void)
{
	Timer_A_disableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
	Timer_A_stopTimer(TIMER_A0_BASE);
}

//! \brief  Interrupt Service Routine for Timer A1
//!
#if defined (__TI_COMPILER_VERSION__) || defined (__IAR_SYSTEMS_ICC__) || ( __CC_ARM )
void TA1_0_IRQHandler(void)
{
	if(TA1CCTL0 & CCIFG)
	{
		TA1CCTL0 &= ~CCIFG;
		TimerA1_turnOff();
		__low_power_mode_off_on_exit();
	}
}
#else
#error Compiler not supported!
#endif


//! \brief  Interrupt Service Routine for Timer A0 (for VCom on LCD)
//!
#if defined (__TI_COMPILER_VERSION__) || defined (__IAR_SYSTEMS_ICC__) || ( __CC_ARM )
void TA0_0_IRQHandler(void)
{
	TA0CCTL0 &= ~CCIFG;
    Sharp96x96_SendToggleVCOMCommand();
}
#else
#error Compiler not supported!
#endif
