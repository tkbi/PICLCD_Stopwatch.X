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

//*** functions ****************************************************************

void timer_init (void)
{
    // timer off, 8 bit, 1/64
    T0CON = 0b01000100;
    
    // interrupt enable
    INTCONbits.T0IE = 1;
}

void timer_on (void)
{
    T0CONbits.TMR0ON = 1;
}

void timer_off (void)
{
    T0CONbits.TMR0ON = 0;
}
