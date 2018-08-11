/*******************************************************************************
 *
 * File:        main.c
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
#include "main.h"

//*** configuration ************************************************************

#pragma config FOSC = IRC       // internal rc oscillator
#pragma config PLLEN = OFF      // pll off
#pragma config FCMEN = ON       // fail-safe clock monitor on
#pragma config PWRTEN = ON      // power-up timer on
#pragma config BOREN = OFF      // brown-out reset off
#pragma config WDTEN = OFF      // watchdog timer off
#pragma config MCLRE = ON       // MCLR enabled (RA3 input pin disabled)
#pragma config LVP = OFF        // low voltage programming off
#pragma config XINST = OFF      // extended instruction set off

//*** prototypes ***************************************************************

static void __main_init_pic (void);

//*** functions ****************************************************************

void main (void)
{
    __main_init_pic();

    while(1);
}

//*** static functions *********************************************************

static void __main_init_pic (void)
{
    // select the internal rc oscillator frequency to 16MHz
    OSCCONbits.IRCF = 0b111;
}