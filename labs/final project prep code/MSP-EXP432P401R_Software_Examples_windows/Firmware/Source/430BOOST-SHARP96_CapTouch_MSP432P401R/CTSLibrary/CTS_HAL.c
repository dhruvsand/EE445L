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
 /*!
 *  @file   CTS_HAL.c
 *
 *  @brief  This file contains the source for the different implementations.
 *
 *  @par    Project:
 *             MSP432 Capacitive Touch Library
 *
 *  @par    Developed using:
 *             CCS Version : 6.1.0.00104 , w/support for GCC extensions (--gcc)
 *             IAR Embedded Workbench for ARM Version : 7.40.3.8938
 *
 *  @author  C. Sterzik
 *  @author  T. Hwang
 *  @author  E. Fu
 *  @author  B. Moore
 *
 *  @version     v0.1
 *
 *  @par    Supported Hardware Implementations:
 *              - TI_CTS_RO_CTIO_TA2_WDTA_HAL()
 *              - TI_CTS_RO_CTIO_TA2_TA3_HAL()
 *              - TI_CTS_fRO_CTIO_TA2_TA3_HAL()
 */

/*! 
 *  @defgroup CTS_HAL Capacitive Touch Implementations
 *  @{
 */

#include "CTS_HAL.h"

#ifdef fRO_CTIO_TA2_TA3
/*!
 *  ======== TI_CTS_fRO_CTIO_TA2_TA3_HAL ========
 *  @brief  fRO using Capacitive Touch IO, TimerA2, and TimerA3
 *          
 *  \n      Schematic Description: 
 * 
 *  \n      element-----+->Px.y
 * 
 *  \n      The TimerA2 interval represents the measurement window.  The number
 *          of counts within the TA3R that have accumulated during the
 *          measurement window represents the capacitance of the element.
 * 
 *  @param group  pointer to the sensor to be measured
 *  @param counts pointer to where the measurements are to be written
 *  @return none
 */
void TI_CTS_fRO_CTIO_TA2_TA3_HAL(const struct Sensor *group,uint16_t *counts)
{ 
    uint8_t i;
    /*
     *  Allocate Context Save Variables
     *  TIMERA2: TA2CTL, TA2CCTL0, TA2CCR0
     *  TIMERA3: TA3CTL, TA3CCTL0, TA3CCR0
     *  CTIO: CTIOxCTL
     */
    uint16_t contextSaveTA2CTL,contextSaveTA2CCTL0,contextSaveTA2CCR0;
    uint16_t contextSaveTA3CTL,contextSaveTA3CCTL0,contextSaveTA3CCR0;
    uint8_t contextSaveCtl;
    uint32_t contextSaveNVIC;

    /* Perform context save of registers used. */
    contextSaveTA2CTL = TA2CTL;
    contextSaveTA2CCTL0 = TA2CCTL0;
    contextSaveTA2CCR0 = TA2CCR0;
    contextSaveTA3CTL = TA3CTL;
    contextSaveTA3CCTL0 = TA3CCTL0;
    contextSaveTA3CCR0 = TA3CCR0;
    contextSaveCtl = *(group->inputCaptioctlRegister);
    contextSaveNVIC = contextSaveNVIC = NVIC_GetActive(TA2_0_IRQn);
    /*
     *  TimerA3 is the measurement timer and counts the number of clock cycles
     *  of the source which is connected to measGateSource, typically SMCLK.
     *  TimerA3 is in continuous mode.  TA3CCR0 is configured as a capture
     *  register and will be triggered as a SW capture event.
     */
    TA3CTL = group->measGateSource + MC_2;
    TA3CCTL0 = CM_3+CCIS_2+CAP+SCS;
    /*
     *  TimerA2 is the gate (measurement interval) timer.  The number of
     *  oscillations counted, by TimerA3, within the gate interval represents
     *  the measured capacitance.  The input is TA2CLK.
     */
    TA2CCR0 = (group->accumulationCycles);
    TA2CTL = TASSEL_3+group->sourceScale + TAIE;
    TA2CCTL0 = CCIE;

    // Enable Timer A2 interrupt in NVIC
    NVIC_EnableIRQ(TA2_0_IRQn);

    for (i = 0; i<(group->numElements); i++)
    {
    	/* Enable Capacitive Touch IO oscillation */
	    *(group->inputCaptioctlRegister)
	            = ((group->arrayPtr[i])->inputBits)+CAPTIOEN;
        TA3CTL |= TACLR;                   // Clear TimerA3, measurement timer
        TA3CTL &= ~TAIFG;                  // Clear overflow flag
        TA2CTL |= (TACLR + MC_1);          // Clear and start TimerA2

        /*
		 *  The measGateSource represents the measurement source for timer
		 *  TIMERA3, which can be sourced from TACLK, ACLK, SMCLK, or INCLK.
		 *  The interrupt handler is defined in TIMER2_A0_VECTOR, which simply
		 *  clears the low power mode bits in the Status Register before
		 *  returning from the ISR.
		 */

        __enable_interrupt();	// Enable interrupts
        __sleep();				// Go to low power mode 0

        TA3CCTL0 ^= CCIS0;  // Create SW capture of TA3R into TA3CCR0
        TA2CTL &= ~MC_1;    // Halt Timer
        if(TA3CTL & TAIFG)
        {
        	/*
			 *  If a rollover in the timer has occurred then set counts to
			 *  0.  This will prevent erroneous data from entering the baseline
			 *  tracking algorithm.
			 */
			counts[i] = 0;
        }
        else
        {
            counts[i] = TA3CCR0;  // Save result
        }
    } // End For Loop

    /* Context restore GIE within Status Register and registers used. */
    if(!contextSaveNVIC)
    {
    	NVIC_DisableIRQ(TA2_0_IRQn);
    }
    *(group->inputCaptioctlRegister) = contextSaveCtl;
    TA2CTL = contextSaveTA2CTL;
    TA2CCTL0 = contextSaveTA2CCTL0;
    TA2CCR0 = contextSaveTA2CCR0;
    TA3CTL = contextSaveTA3CTL;
    TA3CCTL0 = contextSaveTA3CCTL0;
    TA3CCR0 = contextSaveTA3CCR0;
}
#endif

#ifdef RO_CTIO_TA2_TA3
/*!
 *  ======== TI_CTS_RO_CTIO_TA3_TA2_HAL ========
 *  @brief   RO method using Capacitive Touch IO, TimerA2, and TimerA3
 *
 *  \n      Schematic Description:
 *  \n      element-----+->Px.y
 *
 *  \n      The TimerA3 interval represents the measurement window.  The number
 *          of counts within the TA2R that have accumulated during the
 *          measurement window represents the capacitance of the element.
 *
 *  @param group  pointer to the sensor to be measured
 *  @param counts pointer to where the measurements are to be written
 *  @return        none
 */
void TI_CTS_RO_CTIO_TA2_TA3_HAL(const struct Sensor *group,uint16_t *counts)
{
    uint8_t i;
    /*
     *  Allocate Context Save Variables
     *  TIMERA2: TA2CTL, TA2CCTL0, TA2CCR0
     *  TIMERA3: TA3CTL, TA3CCTL0, TA3CCR0
     *  CTIO: CTIOxCTL
     */
    uint16_t contextSaveTA2CTL,contextSaveTA2CCTL0,contextSaveTA2CCR0;
    uint16_t contextSaveTA3CTL,contextSaveTA3CCTL0,contextSaveTA3CCR0;
    uint8_t contextSaveCtl;
    uint32_t contextSaveNVIC;

    /* Perform context save of registers used. */
    contextSaveTA2CTL = TA2CTL;
    contextSaveTA2CCTL0 = TA2CCTL0;
    contextSaveTA2CCR0 = TA2CCR0;
    contextSaveTA3CTL = TA3CTL;
    contextSaveTA3CCTL0 = TA3CCTL0;
    contextSaveTA3CCR0 = TA3CCR0;
    contextSaveCtl = *(group->inputCaptioctlRegister);
    contextSaveNVIC = NVIC_GetActive(TA3_0_IRQn);
    /*
     *  TimerA2 is the measurement timer and counts the number of relaxation
     *  oscillation cycles of the element which is connected to TA2CLK.
     *  TimerA2 is in continuous mode.  TA2CCR0 is configured as a capture
     *  register and will be triggered as a SW capture event.
     */
    TA2CTL = TASSEL_3+MC_2;
    TA2CCTL0 = CM_3+CCIS_2+CAP+SCS;
    /*
     *  TimerA3 is the gate (measurement interval) timer.  The number of
     *  oscillations counted, by TimerA2, within the gate interval represents
     *  the measured capacitance.
     */
    TA3CCR0 = (group->accumulationCycles);
    TA3CTL = group->measGateSource + group->sourceScale + TAIE;
    TA3CCTL0 = CCIE;

    // Enable Timer A3 Interrupt in NVIC
    NVIC_EnableIRQ(TA3_0_IRQn);

    for (i = 0; i<(group->numElements); i++)
    {
    	/* Enable Capacitive Touch IO oscillation */
	    *(group->inputCaptioctlRegister)
	            = ((group->arrayPtr[i])->inputBits)+CAPTIOEN;
        TA2CTL |= TACLR;                   // Clear TimerA2, measurement timer
        TA2CTL &= ~TAIFG;                  // Clear overflow flag
        TA3CTL |= (TACLR + MC_1);          // Clear and start TimerA3
        /*
		 *  The measGateSource represents the gate source for timer TIMERA3,
		 *  which can be sourced from TACLK, ACLK, SMCLK, or INCLK.  The
		 *  interrupt handler is defined in TIMER3_A0_VECTOR, which simply
		 *  clears the low power mode bits in the Status Register before
		 *  returning from the ISR.
		 */

        __enable_interrupt();  // Enable interrupts
        __sleep();			   // Go to low power mode 0

        TA2CCTL0 ^= CCIS0;  // Create SW capture of TA2R into TA2CCR0
        TA3CTL &= ~MC_1;    // Halt Timer
        if(TA2CTL & TAIFG)
        {
        	/*
			 *  If a rollover in the timer has occurred then set counts to
			 *  0.  This will prevent erroneous data from entering the baseline
			 *  tracking algorithm.
			 */
			counts[i] = 0;
        }
        else
        {
            counts[i] = TA2CCR0;  // Save result
        }
    } // End For Loop

    /* Context restore registers that may have been modified. */
    if(!contextSaveNVIC)
    {
    	NVIC_DisableIRQ(TA3_0_IRQn);
    }
    *(group->inputCaptioctlRegister) = contextSaveCtl;
    TA2CTL = contextSaveTA2CTL;
    TA2CCTL0 = contextSaveTA2CCTL0;
    TA2CCR0 = contextSaveTA2CCR0;
    TA3CTL = contextSaveTA3CTL;
    TA3CCTL0 = contextSaveTA3CCTL0;
    TA3CCR0 = contextSaveTA3CCR0;
}
#endif

#ifdef RO_CTIO_TA2_WDTA // ********************************************* ADDED for MSP432 ************
/*!
 *  ======== TI_CTS_RO_CTIO_TA2_WDTA_HAL ========
 *  @brief  RO method measurement using Capacitive Touch IO, TimerA2, and WDTA
 *
 *  \n      Schematic Description:
 *
 *  \n      element-----+->Px.y
 *
 *  \n      The WDTA interval represents the measurement window.  The number of
 *          counts within the TA2R that have accumulated during the measurement
 *          window represents the capacitance of the element.
 *
 *  @param group  pointer to the sensor to be measured
 *  @param counts pointer to where the measurements are to be written
 *  @return  none
 */
void TI_CTS_RO_CTIO_TA2_WDTA_HAL(const struct Sensor *group, uint16_t *counts)
{
    uint8_t i;
    /*
     *  Allocate Context Save Variables
     *  WDT: WDTCTL
     *  TIMERA2: TA2CTL, TA2CCTL0, TA2CCR0
     *  CTIO: CTIOxCTL
     */
    uint16_t contextSaveWDTCTL;
    uint16_t contextSaveTA2CTL,contextSaveTA2CCTL0,contextSaveTA2CCR0;
    uint8_t contextSaveCtl;
    uint32_t contextSaveNVIC;

    /* Perform context save of registers used. */
    contextSaveWDTCTL = WDTCTL;
    contextSaveWDTCTL &= 0x00FF;
    contextSaveWDTCTL |= WDTPW;
    contextSaveTA2CTL = TA2CTL;
    contextSaveTA2CCTL0 = TA2CCTL0;
    contextSaveTA2CCR0 = TA2CCR0;
    contextSaveCtl = *(group->inputCaptioctlRegister);
    contextSaveNVIC = NVIC_GetActive(WDT_A_IRQn);

    /*
     *  TimerA2 is the measurement timer and counts the number of relaxation
     *  oscillation cycles of the element which is connected to TA2CLK.
     *  TimerA2 is in continuous mode.  TA2CCR0 is configured as a capture
     *  register and will be triggered as a SW capture event.
     */
    TA2CTL = TASSEL_3+MC_2;
    TA2CCTL0 = CM_3+CCIS_2+CAP+SCS;

    /*
     *  The WDTA is the gate (measurement interval) timer.  The number of
     *  oscillations counted, by TimerA2, within the gate interval represents
     *  the measured capacitance.
     */

    // Enable WDTA interrupt in NVIC
    NVIC_EnableIRQ(WDT_A_IRQn);

    for (i = 0; i<(group->numElements); i++)
    {
	    /* Enable Capacitive Touch IO oscillation */
	    *(group->inputCaptioctlRegister)
	            = ((group->arrayPtr[i])->inputBits)+CAPTIOEN;
	    TA2CTL |= TACLR;        // Clear Timer_A2 measurement timer
	    TA2CTL &= ~TAIFG;       // Clear the overflow flag
        /*
         *  The measGateSource represents the gate source for the WDTA, which
         *  can be sourced from ACLK, SMCLK, VLOCLK or X_CLK. The
         *  accumulationCycles represents the watchdog timer interval select.
         */
        WDTCTL = WDTPW+WDTTMSEL+WDTCNTCL+ group->measGateSource
                + group->accumulationCycles;
        /*
         *  The interrupt handler is defined in WDT_VECTOR, which simply clears
         *  the low power mode bits in the Status Register before returning
         *  from the ISR.
         */

        __enable_interrupt();  // Enable interrupts
        __sleep();             // Go to low power mode 0

	    TA2CCTL0 ^= CCIS0;  // Create SW capture of TA2R into TA2CCR0
	    WDTCTL = WDTPW + WDTHOLD;  // Halt watchdog timer
        if(TA2CTL & TAIFG)
        {
        	/*
			 *  If a rollover in the timer has occurred then set counts to
			 *  0.  This will prevent erroneous data from entering the baseline
			 *  tracking algorithm.
			 */
			counts[i] = 0;
        }
        else
        {
            counts[i] = TA2CCR0;  // Save result
        }
    }  // End For Loop

    /* Context restore registers that were modified. */
    if(!contextSaveNVIC)
    {
    	NVIC_DisableIRQ(WDT_A_IRQn);
    }
    *(group->inputCaptioctlRegister) = contextSaveCtl;
    WDTCTL = contextSaveWDTCTL;
    TA2CTL = contextSaveTA2CTL;
    TA2CCTL0 = contextSaveTA2CCTL0;
    TA2CCR0 = contextSaveTA2CCR0;
}
#endif

/*!
 *  @defgroup ISR_GROUP ISR Definitions
 *  @ingroup CTS_HAL
 */
 
#ifdef WDT_GATE
/*!
 *  ======== watchdog_timer ========
 *  @ingroup ISR_GROUP
 *  @brief  WDT_ISR
 *
 *          This ISR wakes the CPU from LPM
 * 
 *  @param none
 *  @return none
 */
void WDT_A_IRQHandler(void)
{
	// EXIT LPM3 on RETI
	__low_power_mode_off_on_exit();
}
#endif

#ifdef TIMERA2_0GATE
/*!
 *  ======== TIMER2_A0_ISR ========
 *  @ingroup ISR_GROUP
 *  @brief  TIMER2_A0_ISR
 *
 *          This ISR wakes the CPU from LPM
 * 
 *  @param none
 *  @return none
 */
void TA2_0_IRQHandler(void)
{
	// EXIT LPM3 on RETI
	TA2CCTL0 &= ~CCIFG;
	__low_power_mode_off_on_exit();
}
#endif

#ifdef TIMERA3_0GATE
/*!
 *  ======== TIMER3_A0_ISR ========
 *  @ingroup ISR_GROUP
 *  @brief  TIMER3_A0_ISR
 *
 *          This ISR wakes the CPU from LPM
 * 
 *  @param none
 *  @return none
 */
void TA3_0_IRQHandler(void)
{
	// EXIT LPM3 on RETI
	TA3CCTL0 &= ~CCIFG;
	__low_power_mode_off_on_exit();
}
#endif
