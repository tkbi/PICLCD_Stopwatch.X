/*******************************************************************************
 *
 * File:        spi.h
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

#ifndef SPI_H
#define SPI_H

//*** include ******************************************************************

#include <xc.h>
#include <stdint.h>
#include "main.h"

//*** prototypes ***************************************************************

/**
 * This function will initialize the serial peripheral interface (SPI).
 */
void spi_init (void);

/**
 * Use this function to send/receive data via the serial peripheral interface
 * (SPI). You have to define the length within the parameters as well as the
 * read and write pointers.
 * 
 * @param pWr Pointer to the write buffer.
 * @param pRd Pointer to the read buffer.
 * @param len Length of data to send/receive.
 */

void spi_transfer (uint8_t* pWr, uint8_t* pRd, uint8_t len);

#endif