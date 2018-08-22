/*******************************************************************************
 *
 * File:        timer.c
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
#include "timer.h"

//*** golabl variables *********************************************************

static struct tOut_s tOut[MAX_TOUT];

//*** functions ****************************************************************

void timer0_init (void)
{
    // 8 bit, 1/16 -> T0IF every ~1ms
    T0CON = 0b01000011;
    
    // set its interrupt prio to low
    INTCON2bits.TMR0IP = 0;
    
    // enable T0IF, prio low
    INTCONbits.T0IE = 1;
    
    // start the timer
    T0CONbits.TMR0ON = 1;
}

//..............................................................................

int8_t timer0_new_timeout (uint16_t ms)
{
    int8_t i = 0;

    // find a free timeout counter
    while( tOut[i].inUse && (i < MAX_TOUT) )
    {
        i++;
    }
    
    // mark the tOut as "in use"
    if(i < MAX_TOUT)
    {
        tOut[i].inUse = true;
        tOut[i].cnt = ms;
    }
    else
    {
        // no free timeout slot found
        i = -1;
    }

    return i;
}

//..............................................................................

void timer0_decrease_timeout (void)
{
    uint8_t i = 0;

    while(i < MAX_TOUT)
    {
        if(tOut[i].inUse && tOut[i].cnt)
        {
            tOut[i].cnt--;
        }

        // next timeout
        i++;
    }
}

//..............................................................................

bool timer0_get_timeout (uint8_t i)
{
    return (tOut[i].cnt == 0 && tOut[i].inUse);
}

//..............................................................................

void timer0_clear_timeout (uint8_t i)
{
    tOut[i].inUse = false;
    tOut[i].cnt = 0;
}

//..............................................................................

void timer2_init (void)
{
    // post- and pre-scale = 1/16
    T2CON = 0b01111010;
    
    // low interrupt priority
    IPR1bits.TMR2IP = 0;
    
    // enable interrupt
    PIE1bits.TMR2IE = 1;
    
    // compare value (see calculation below)
    PR2 = 156;
    
    /* Calculation of PR2:
     * 
     * Internal RC-Oscillator:  16 MHz
     * Timer2 frequency:        16 Mhz / 4 = 4 MHz
     * Necessary time base:     10 ms
     * Prescaler of TIMER2:     1/16
     * Postscaler of TIMER2:    1/16
     * Tick count of TIMER2:    1/(4 MHz / (16*16)) = 64us
     * Ticks for time base:     10ms / 64us = 156,25 
     * Nominal PR2-Value:       156
     * Error:                   0,16% (9,984ms instead of 10ms)
     * Error-Direction:         faster
     */
}

//..............................................................................

void timer2_start (void)
{
    T2CONbits.TMR2ON = 1;
}

//..............................................................................

void timer2_stop (void)
{
    T2CONbits.TMR2ON = 0;
    
    // reset the timer value
    TMR2 = 0;
}

//..............................................................................
