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
 * This function will set the write enable latch of the external EEPROM in order
 * to disable the write protection. Afterwards data can be written.
 */

static void __eeprom_25LC56_set_wel (void);

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
        __eeprom_25LC56_set_wel();

        // check if WEL bit is really set
        while( !(eeprom_25LC56_read_status_reg() & EEPROM_25LC256_SR_WEL) );
        
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
        
        // wait until WIP bit is cleared
        while( eeprom_25LC56_read_status_reg() & EEPROM_25LC256_SR_WIP );
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
    eeprom_25LC256_set_addr_ptr(0x0002);
}

//..............................................................................

uint16_t eeprom_25LC256_get_addr_ptr (void)
{
    uint16_t addr_ptr;
    
    eeprom_25LC256_read(0x0000, (uint8_t*)(&addr_ptr), 2);
    
    return addr_ptr;
}

//..............................................................................

void eeprom_25LC256_set_addr_ptr (uint16_t addr_ptr)
{
    eeprom_25LC256_write(0x0000, (uint8_t*)(&addr_ptr), 2);
}

//*** static functions *********************************************************

static void __eeprom_25LC56_set_wel (void)
{
    uint8_t buf = EEPROM_25LC256_WREN;
    
    EEPROM_CS = 0;
    spi_transfer(&buf, NULL, 1);
    EEPROM_CS = 1;
}

//..............................................................................
