/*******************************************************************************
 *
 * File:        eeprom.c
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

#include <xc.h>
#include <stdint.h>
#include "eeprom.h"
#include "spi.h"

//*** prototypes ***************************************************************

/**
 * This function will read the address pointer to the first free address inside
 * the external EEPROM. This address (not a pointer) can be used to store the
 * next data into the external EEPROM.
 * 
 * @return The 16 bit address to the next free slot inside the ext. EEPROM
 */

static uint16_t __eeprom_25LC256_get_addr_ptr (void);

/**
 * You can update the address value that informs about the next free address
 * slot inside the external EEPROM. This function will be called if new data
 * was written into the external EEPROM.
 * 
 * @param addr_ptr New address to the next free slot inside the ext. EEPROM
 */

static void __eeprom_25LC256_set_addr_ptr (uint16_t addr_ptr);

//*** functions ****************************************************************

void eeprom_25LC256_read (uint16_t addr, uint8_t *pBuf, uint8_t len)
{
    uint8_t buf [3];
    uint8_t *p = pBuf;
    uint8_t next_len;
    
    // copy READ instruction and read address into buffer
    buf[0] = EEPROM_25LC256_READ;
    
    while(len)
    {
        // check how many bytes can be written within the next command
        next_len = 64 - (addr % 64);
        
        // if less bytes shall be send, take this as next length
        if(next_len > len) next_len = len;
        
        // copy the next address into the buffer
        buf[1] = (uint8_t)((addr >> 8) & 0xFF);
        buf[2] = (uint8_t)(addr & 0xFF);
        
        EEPROM_CS = 0;
        spi_transfer(buf, NULL, 3);
        spi_transfer(NULL, p, next_len);
        EEPROM_CS = 1;
        
        // update the remaining length and the address
        len -= next_len;
        addr += next_len;
    }
}

//..............................................................................

void eeprom_25LC256_write (uint16_t addr, uint8_t *pBuf, uint8_t len)
{
    uint8_t wren = EEPROM_25LC256_WREN;
    uint8_t buf [3];
    uint8_t *p = pBuf;
    uint8_t next_len;
 
    // set the write instruction
    buf[0] = EEPROM_25LC256_WRITE;
    
    // write until all data has been written
    while(len)
    {
        // check how many bytes can be written within the next command
        next_len = 64 - (addr % 64);
        
        // if less bytes shall be send, take this as next length
        if(next_len > len) next_len = len;
        
        // enable the write latch by sending WREN
        EEPROM_CS = 0;
        spi_transfer(&wren, NULL, 1);
        EEPROM_CS = 1; 
        
        // copy the (next) address to the buffer
        buf[1] = (uint8_t)((addr >> 8) & 0xFF);
        buf[2] = (uint8_t)(addr & 0xFF);
        
        // send the command and address
        EEPROM_CS = 0;
        spi_transfer(buf, NULL, 3);
        
        // continue by sending the data
        spi_transfer(p, NULL, next_len);
        EEPROM_CS = 1;
        
        // update the remaining length and the address
        len -= next_len;
        addr += next_len;
    }
}

//..............................................................................

uint8_t eeprom_25LC56_read_status_reg (void)
{
    uint8_t buf;
    
    buf = EEPROM_25LC256_RDSR;
    
    EEPROM_CS = 0;
    spi_transfer(&buf, NULL, 1);
    spi_transfer(NULL, &buf, 1);
    EEPROM_CS = 1;
    
    return buf;
}

//..............................................................................

void eeprom_25LC256_clear (void)
{
    __eeprom_25LC256_set_addr_ptr(0x0002);
}

//..............................................................................

void eeprom_25LC56_wren (void)
{
    uint8_t buf = EEPROM_25LC256_WREN;
    
    EEPROM_CS = 0;
    spi_transfer(&buf, NULL, 1);
    EEPROM_CS = 1;
}

//*** static functions *********************************************************

static uint16_t __eeprom_25LC256_get_addr_ptr (void)
{
    uint16_t addr_ptr;
    
    eeprom_25LC256_read(0x0000, (uint8_t*)(&addr_ptr), 2);
    
    return addr_ptr;
}

//..............................................................................

static void __eeprom_25LC256_set_addr_ptr (uint16_t addr_ptr)
{
    eeprom_25LC256_write(0x0000, (uint8_t*)(&addr_ptr), 2);
}

//..............................................................................