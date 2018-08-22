/*******************************************************************************
 *
 * File:        main.h
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


#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

//*** define *******************************************************************

#define _XTAL_FREQ  16000000

// Uncomment the following line to get some debug messages via UART
// #define DEBUG       0

//*** typedef ******************************************************************

// The status struct contains several flags that are important for the work of
// the stop watch and will be checked within the func_workload function.

typedef struct status_s
{
    bool iXms       : 1;    // next 10ms passed
    bool iRx        : 1;    // data inside UART rx buffer available
    bool iTx        : 1;    // data inside UART tx buffer available
    
} status_t;

//*** export *******************************************************************

extern status_t status;

#endif
