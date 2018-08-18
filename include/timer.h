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

#ifndef TIMER_H
#define TIMER_H

//*** include ******************************************************************

#include <xc.h>
#include <stdint.h>
#include "main.h"

//*** struct *******************************************************************

struct tOut_s
{
    bool inUse;
    uint16_t cnt;
};

//*** define *******************************************************************

#define MAX_TOUT    4

//*** prototypes ***************************************************************

/**
 * This function will be called to initialize the TIMER0. Furthermore the timer
 * overflow interrupt will be enabled (1ms tick).
 */

void timer0_init (void);

/**
 * Call this function if you need an timeout counter. You are able to define a
 * desired timeout value with an resolution of 1ms (max 65536ms). You will
 * receive an timeout index that can be used to call timer0_get_timeout. If no
 * internal memory is free to create a new timeout counter you will receive a
 * -1 as return value.
 * 
 * @param t Timeout value in [ms]. The range is 1..65536ms.
 * @return Timeout counter index or -1 if no timeout counter slot is free atm.
 */

int8_t timer0_new_timeout (uint16_t ms);

/**
 * This function will automatically be called from the TIMER0 overflow 
 * inerrupt. Please don't call this function by your own. Otherwise your desired
 * timeout value will be differ!
 * 
 * A once created timeout counter needs to be cleared with timer0_clear_timeout
 * if it is no longer needed.
 */

void timer0_decrease_timeout (void);

/**
 * Call this function to check if your timeout occured.
 * 
 * @param i Timeout index (received by timer0_new_timeout).
 * @return True if your timeout occurred otherwise false.
 */

bool timer0_get_timeout (uint8_t i);

/**
 * A once created timeout counter needs to be cleared with timer0_clear_timeout
 * if it is no longer needed. You can clear a timeout counter with this
 * function and the timeout index (received by timer0_new_timeout).
 * 
 * @param i Timeout index value (this timeout shall be cleared).
 */

void timer0_clear_timeout (uint8_t i);

/**
 * This function will initialize the TIMER2 which will be used to generate the
 * 10ms timebase for the stop watch. Within this function the overflow interrupt
 * will be activated. You have to call this function on boot.
 */

void timer2_init (void);

/**
 * Start the TIMER2 after it was stopped (e.g. before gooing to sleep).
 */

void timer2_start (void);

/**
 * Stop the TIMER2 (e.g. to reduce current consumption during sleep-state).
 */

void timer2_stop (void);

#endif