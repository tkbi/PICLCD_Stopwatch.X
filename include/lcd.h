/*******************************************************************************
 *
 * File:        LCD.h
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


#ifndef LCD_H
#define LCD_H

//*** include ******************************************************************

#include <stdint.h>

//*** defines ******************************************************************

#define LCD_CS  LATCbits.LC0    // chip select
#define LCD_RS  LATCbits.LC1    // register select (0: Instruction 1: Data)

//*** prototypes ***************************************************************

/**
 * This function will initialize the display.
 */
void lcd_init (void);

/**
 * Return to the origin start address of the display.
 */
//void lcd_return_home (void);

/**
 * You can write a string to the lcd by using this function.
 * 
 * @param pStr  Pointer to a string which shall be written on the display.
 * @param addr  The DDRAM address to write the string to.
 */
void lcd_write (char *pStr, uint8_t addr);

/**
 * This function will shut off the lc-display (in order to reduce the energy 
 * consumption). Please use lcd_init() to power the lcd on again.
 */

void lcd_off (void);

#endif