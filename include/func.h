/*******************************************************************************
 *
 * File:        func.h
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

#ifndef FUNC_H
#define FUNC_H

//*** include ******************************************************************

#include <xc.h>
#include <stdint.h>
#include "main.h"

//*** define *******************************************************************

#define USR PORTAbits.RA2
#define PB  PORTAbits.RA4

// key events
#define NO_KEY_EVENT            0
#define KEY_PRESSED             1
#define KEY_RELEASED            2

// key bitfield
#define KEY_PB                  1
#define KEY_USR                 2

// key press & hold time border values [10ms]
#define KEY_HOLD_SAVE           300
#define KEY_HOLD_CLR            500

// states for the stop watch' state machine
#define SW_STATE_IDLE           0
#define SW_STATE_RUN            1
#define SW_STATE_STOP           2
#define SW_STATE_CLR            3   
#define SW_STATE_CLRD           4
#define SW_STATE_SAVED          5

// some time definitions (x*10ms) switch automatically from a to b after ..
#define IDLE_TO_SLEEP_TIME      1500    // idle     -> sleep
#define STOP_TO_IDLE_TIME       1000    // stop     -> idle
#define CLEAR_TO_IDLE_TIME      800     // clear    -> idle
#define CLEARED_TO_IDLE_TIME    300     // cleared  -> idle
#define SAVED_TO_IDLE_TIME      300     // saved    -> idle

//*** typedef ******************************************************************

typedef struct sw_s
{
    uint16_t ms;
    uint8_t s;
    uint8_t m;
    
} sw_t;

//*** extern *******************************************************************

extern uint8_t uartBuf;

//*** prototypes ***************************************************************

/**
 * This function controlls the complete stop watch. The function has to be
 * called at least every 10ms.
 */

void func_workload (void);

/**
 * This function will display the current stop watch value on the LCD.
 */

void func_disp_sw (void);

#endif