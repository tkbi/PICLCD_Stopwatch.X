/*******************************************************************************
 *
 * File:        uart.h
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

#ifndef UART_H
#define UART_H

//*** include ******************************************************************

#include <xc.h>
#include <stdbool.h>

//*** define *******************************************************************

#define UART_BUF_MAX    32

//*** extern *******************************************************************

extern char inBuf [UART_BUF_MAX];
extern char *pInBufWr;
extern char *pInBufRd;

//*** prototypes ***************************************************************

/**
 * This function will initialize the UART interface and enables it's high
 * priority interrupt.
 */

void uart_init(void);

/**
 * 
 * @param pBuf
 */

void uart_write_buf (char *pBuf);

/**
 * 
 */

void uart_tx (void);
 
#endif