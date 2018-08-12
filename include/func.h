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

//*** typedef ******************************************************************

typedef struct sw_s
{
    uint16_t ms;
    uint8_t s;
    uint8_t m;
    
} sw_t;

//*** prototypes ***************************************************************

void func_workload (void);

#endif