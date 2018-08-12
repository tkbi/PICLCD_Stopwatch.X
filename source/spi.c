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
#include <stdint.h>
#include "main.h"

//*** prototypess **************************************************************

static void __spi_power_on (void);
static void __spi_power_off (void);
static uint8_t __spi_send (uint8_t val);

//*** functions ****************************************************************

void spi_init (void)
{
    // clk = FOSC/64 and clk idle state = high
    SSPCON1 = 0b00010010;
    
    __spi_power_on();
}

void spi_send (uint8_t* pWr, uint8_t* pRd, uint8_t len)
{
    uint8_t i;
    
    for(i=0; i<len; i++)
    {
        if(pRd != NULL)
        {
            pRd[i] = __spi_send( pWr[i] );
        }
        else
        {
            __spi_send( pWr[i] );
        }
    }
}

//*** static functions *********************************************************

static void __spi_power_on (void)
{
    SSPCON1bits.SSPEN = 1;
}

static void __spi_power_off (void)
{
    SSPCON1bits.SSPEN = 0;
}

static uint8_t __spi_send (uint8_t val)
{
    SSPBUF = val;
    while( !SSPSTATbits.BF );

    return SSPBUF;
}
