/*******************************************************************************
 *
 * File:        eeprom.h
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

#ifndef EEPROM_H
#define EEPROM_H

//*** include ******************************************************************

#include <xc.h>

//*** define *******************************************************************

#define EEPROM_CS               LATCbits.LATC2

// 25LC256 instruction set

#define EEPROM_25LC256_READ     0b00000011  // Read data from memory array 
                                            // beginning at selected address
#define EEPROM_25LC256_WRITE    0b00000010  // Write data to memory array 
                                            // beginning at selected address
#define EEPROM_25LC256_WRDI     0b00000100  // Reset the write enable latch 
                                            // (disable write operations)
#define EEPROM_25LC256_WREN     0b00000110  // Set the write enable latch 
                                            // (enable write operations)
#define EEPROM_25LC256_RDSR     0b00000101  // Read STATUS register
#define EEPROM_25LC256_WRSR     0b00000001  // Write STATUS register

//*** prototypes ***************************************************************

/**
 * 
 * @param addr
 * @param pBuf
 * @param len
 */

void eeprom_25LC256_read (uint16_t addr, uint8_t *pBuf, uint8_t len);

/**
 * 
 * @param addr
 * @param pBuf
 * @param len
 */

void eeprom_25LC256_write (uint16_t addr, uint8_t *pBuf, uint8_t len);

/**
 * Call this function to clear all saved stop watch measurements. The function
 * wont override all memory of the external EEPROM with e.g. zeros. No.. it will
 * only set the internal address-pointer back to 0. The data inside the external
 * EEPROM remains unchanged.
 */

void eeprom_25LC256_clear (void);

uint8_t eeprom_25LC56_read_status_reg (void);

#endif