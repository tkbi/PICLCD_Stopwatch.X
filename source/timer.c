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

//*** functions ****************************************************************

void timer_init (void)
{
    // post- and pre-scale = 1/16
    T2CON = 0b01111010;
    
    // low interrupt priority
    IPR1bits.TMR2IP = 0;
    
    // enable interrupt
    PIE1bits.TMR2IE = 1;
    
    // compare value
    PR2 = 156;
}

void timer_on (void)
{
    T2CONbits.TMR2ON = 1;
}

void timer_off (void)
{
    T2CONbits.TMR2ON = 0;
}
