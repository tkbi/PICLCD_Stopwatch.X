/*******************************************************************************
 *
 * File:        isr.c
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
#include <stdbool.h>

#include "timer.h"
#include "main.h"
#include "func.h"
#include "uart.h"

//*** functions ****************************************************************

void __interrupt() highPrio (void)
{
    // waked up due to INT2?
    if( INTCON3bits.INT2IF )
    {
        // nothing to do here
        INTCON3bits.INT2IF = 0;
    }
    // received data via UART?
    else if(PIR1bits.RCIF)
    {
        status.iRx = true;

        // store the received byte into the fifo (this will also clear RCIF)
        *pInBufWr = RCREG;
        
        // already on the last index?
        if( pInBufWr == &inBuf[UART_BUF_MAX-1] )
        {
            // yes, start at the beginning
            pInBufWr = inBuf;
        }
        else
        {
            // no, go to the next index
            pInBufWr++;
        }
    }
}

//..............................................................................

void __interrupt(low_priority) lowPrio (void)
{
    // 10ms passed --> TMR2?
    if( PIR1bits.TMR2IF )
    {
        // clear the interrupt flag
        PIR1bits.TMR2IF = 0;
        
        // set the 10ms passed flag
        status.iXms = true;
    }
    else if(INTCONbits.T0IF)
    {
        INTCONbits.T0IF = 0;
        
        // decrement all timeout counter (if in use)
        timer0_decrease_timeout();
    }
}

//..............................................................................
