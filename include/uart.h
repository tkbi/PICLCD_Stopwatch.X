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

#define UART_BUF_MAX    48

//*** extern *******************************************************************

extern char inBuf [UART_BUF_MAX];
extern uint8_t inWr;
extern uint8_t inRd;

//*** prototypes ***************************************************************

/**
 * This function will initialize the UART interface and enables it's high
 * priority interrupt.
 */

void uart_init(void);

/**
 * Please use this function to send messages over the uart interface. The data
 * will be moved into an internal fifo ringbuffer. Afterwards the data will be
 * shifted out if the cpu has some time left.
 * 
 * @param pBuf Pointer to the message (string / char array with trailin zero).
 */

void uart_print (char *pBuf);

/**
 * Don't call this function by your own. It will be called within the
 * func_workload function and automatically sends the tx-ringbuffer if data is
 * available and the cpu has some free time.
 */

void uart_tx (void);

#endif
