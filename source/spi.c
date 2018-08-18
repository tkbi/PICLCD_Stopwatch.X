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

//*** prototypes ***************************************************************

/**
 * This function will send and receive bytes over the serial peripheral 
 * interface (SPI).
 * 
 * @param val Byte to send
 * @return Received byte
 */

static uint8_t __spi_rxtx (uint8_t val);

//*** functions ****************************************************************

void spi_init (void)
{
    // clk = FOSC/64 and clk idle state = low (Bit #4)
    SSPCON1 = 0b00110010;
}

//..............................................................................

void spi_transfer (uint8_t* pWr, uint8_t* pRd, uint8_t len)
{
    uint8_t i, dummy = 0;
    
    // abort if read and write pointer are NULL
    if(pWr == NULL && pRd == NULL)
    {
        return;
    }
    
    // start reading and/or writing
    for(i=0; i<len; i++)
    {
        if(pWr && pRd)
        {
            // write and read
            pRd[i] = __spi_rxtx( pWr[i] );
        }
        else if(pWr == NULL)
        {
            // read only (will send dummy byte 0x00)
            pRd[i] = __spi_rxtx(dummy);
        }
        else
        {
            // write only (return value discarded)
            __spi_rxtx( pWr[i] );
        }
    }
}

//*** static functions *********************************************************

static uint8_t __spi_rxtx (uint8_t val)
{
    SSPBUF = val;
    while( !SSPSTATbits.BF );

    return SSPBUF;
}

//..............................................................................
