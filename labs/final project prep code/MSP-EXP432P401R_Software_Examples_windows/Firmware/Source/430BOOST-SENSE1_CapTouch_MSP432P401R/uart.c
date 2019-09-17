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
//  uart.c
//
//! \brief File for UART communication using MSP432 eUSCI A peripheral
//!
//*******************************************************************************

//*******************************************************************************
// Includes
//*******************************************************************************
#include "uart.h"
#include "driverlib.h"

//*******************************************************************************
// UART Globals
//*******************************************************************************

//! \brief Stores data for transmission
//!
uint8_t UART_txData;


//! \brief Function configures Timer_A for full-duplex UART operation
//!
void UART_init(void)
{
	//Set DCO to 1MHz, SMCLK to 1MHz, MCLK to 1MHz, and source REFO for ACLK
	MAP_CS_setDCOFrequency(CS_1MHZ);

	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
	MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
	MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	// Configure 2-UART pins by selecting second function
	P1SEL0 |= UART_RXD | UART_TXD;

	// Enable eUSCIA0 interrupt in NVIC module
	NVIC_EnableIRQ(EUSCIA0_IRQn);

	// Configure UART
	// Put eUSCI in reset
	UCA0CTLW0 |= UCSWRST;
	// Select SMCLK as eUSCI clock input
	UCA0CTLW0 |= UCSSEL__SMCLK;

	// Baud Rate calculation
	UCA0BR0 = 6;                                    // 1000000/9600/16
	UCA0BR1 = 0x00;
	UCA0MCTLW = 0x2000 | UCOS16 | 0x0080;

	// Initialize eUSCI
	UCA0CTLW0 &= ~UCSWRST;

}


//! \brief Function unconfigures Timer_A for full-duplex UART operation
//!
void UART_shutdown(void)
{
	P1SEL0 &= ~(UART_RXD | UART_TXD);                         // Reset 2-UART pin as second function
	P1DIR &= ~UART_TXD;

	__disable_interrupt();
	NVIC_DisableIRQ(EUSCIA0_IRQn);  // Disable eUSCIA0 interrupt in NVIC module

	UCA0IE &= ~UCTXIE;                                // Disable USCI_A0 TX interrupt
}

//! \brief Outputs one byte using the Timer_A UART
//! \param uint8_t byte - The byte to be transmitted
//!
void UART_tx(uint8_t byte)
{
    UART_txData = byte;           // Load global variable
    UCA0IE |= UCTXIE;             // Enable USCI_A0 TX interrupt

    // Go to LPM0 until Tx ready
    __enable_interrupt();
    __sleep();
}

//! \brief Prints a string over using the Timer_A UART
//! \param uint8_t * string - pointer to string to be printed
//!
void UART_print(uint8_t *string)
{
    while (*string)
    {
        UART_tx(*string++);
    }
}

//! \brief UART interrupt service routine
//!
void EUSCIA0_IRQHandler(void)
{
	if (UCA0IFG & UCTXIFG)
	{
    	UCA0TXBUF = UART_txData;
    	UCA0IE &= ~UCTXIE;               // Disable USCI_A0 TX interrupt until ready to send again
    	while(!(UCA0IFG & UCTXCPTIFG));
    }
}
   
