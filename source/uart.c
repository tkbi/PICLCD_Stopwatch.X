/*******************************************************************************
 *
 * File:        uart.c
 * Project:     PICLCD-Stopwatch
 * Author:      Nicolas Pannwitz (https://pic-projekte.de/)
 * Comment:
 * Licence:     Copyrightn (C) 2018 Nicolas Pannwitz
 * 
 *              This program is free software: You can redistribute it and/or 
 *              modify it under the terms of the GNU General Public License as
 *              published by the Free Software Foundation, either version 3 of
 *              the License, or (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 * 
 *              You should have received a copy of the GNU General Public
 *              License along with this program.
 *              If not, see https://www.gnu.org/licenses/
 * 
 ******************************************************************************/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "timer.h"

//*** global variables *********************************************************

// fifo / ring buffer for rx
char inBuf [UART_BUF_MAX];
uint8_t inWr = 0;
uint8_t inRd = 0;

//*** static variables *********************************************************

// fifo / ring buffer for tx
static char outBuf [UART_BUF_MAX];
uint8_t outWr = 0;
uint8_t outRd = 0;

//*** functions ****************************************************************

void uart_init (void)
{
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;

    // set baudrate to 9.6kbs
    BAUDCONbits.BRG16 = 0;
    SPBRG = 25;

    // set interrupt prio to high
    IPR1bits.RC1IP = 1;

    // enable the receive interrupt
    PIE1bits.RC1IE = 1;
}

//..............................................................................

void uart_print (char *pBuf)
{
    if( *pBuf )
    {
        // set "data in uart tx buffer available" flag
        status.iTx = true;
    }

    while( *pBuf )
    {
        outBuf[outWr] = *pBuf;       
        outWr++;
        
        // already on the last index?
        if( outWr == UART_BUF_MAX )
        {
            // yes, start at the beginning
            outWr = 0;
        }

        pBuf++;
    }
}

//..............................................................................

void uart_tx (uint16_t tmo)
{
    // create a new 5ms timeout
    uint8_t timeout = timer0_new_timeout(tmo);
    
    // until there is data in the fifo
    while( outRd != outWr )
    {
        // break if a timeout occurred
        if(timer0_get_timeout(timeout) && tmo)
        {
            break;
        }
        
        // ready to send new data?
        if(PIR1bits.TX1IF)
        {
            /*load the byte into the buffer*/
            TXREG1 = outBuf[outRd];
            outRd++;
            
            // already on the last index?
            if( outRd == UART_BUF_MAX )
            {
                // yes, start at the beginning
                outRd = 0;
            }
        }     
    }
    
    // if the buffer is empty clear "data in uart tx buffer available" flag
    if( outRd == outWr )
    {
        status.iTx = false;
    }
    
    // clear the timeout
    timer0_clear_timeout(timeout);
}

//..............................................................................
