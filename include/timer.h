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
 * 
 */

void timer0_init (void);

/**
 * 
 * @param t
 * @return 
 */

int8_t timer0_new_timeout (uint16_t ms);

/**
 * 
 */

void timer0_decrease_timeout (void);

/**
 * 
 * @param i
 * @return 
 */

bool timer0_get_timeout (uint8_t i);

/**
 * 
 * @param i
 */

void timer0_clear_timeout (uint8_t i);

/**
 * 
 */

void timer2_init (void);

/**
 * 
 */

void timer2_start (void);

/**
 * 
 */

void timer2_stop (void);

#endif