/*******************************************************************************
 *
 * File:        lcd.c
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
#include "lcd.h"
#include "spi.h"
#include "main.h"

//*** static functions *********************************************************

/**
 * Use this function to direct the DDRAM of the LC-Display to a desired address.
 * 
 * @param addr New address for the DDRAM.
 */

static void __lcd_goto (uint8_t addr);

//*** functions ****************************************************************

void lcd_init (void)
{
    uint8_t buf [9];
    
    LCD_CS = 1;
    spi_init();
    
    // set register selection: command
    LCD_RS = 0;
    
    buf[0] = 0b00110001;    // function set
    buf[1] = 0b00010100;    // bias set
    buf[2] = 0b01010101;    // power control
    buf[3] = 0b01101101;    // follower control
    buf[4] = 0b01110001;    // contrast set
    buf[5] = 0b00110000;    // function set
    buf[6] = 0b00001100;    // display on/off
    buf[7] = 0b00000001;    // clear display
    buf[8] = 0b00000110;    // entry mode set
    
    LCD_CS = 0;
    spi_transfer(buf, NULL, 9);
    LCD_CS = 1;
    
    __delay_ms(10);
}

//..............................................................................

//void lcd_return_home (void)
//{
//    uint8_t buf [2];
// 
//    // set register selection: command
//    LCD_RS = 0;
//    
//    buf[0] = 0b00110000;    // function set (instruction table 0)
//    buf[1] = 0b00000010;    // go home
//    
//    LCD_CS = 0;
//    spi_transfer(buf, NULL, 2);
//    LCD_CS = 1;
//}

//..............................................................................

void lcd_write (char *pStr, uint8_t addr)
{
    // set the start address first
    __lcd_goto(addr);

    // set register selection: data
    LCD_RS = 1;
    LCD_CS = 0;
    
    while(*pStr)
    {
        spi_transfer((uint8_t*)pStr, NULL, 1);
        pStr++;
    }
    
    LCD_CS = 1;
}

//..............................................................................

void lcd_off (void)
{
    uint8_t buf;

    // set register selection: command
    LCD_RS = 0;
    
    // display off (bit #2 = 0)
    buf = 0b00001000;
    
    LCD_CS = 0;
    spi_transfer(&buf, NULL, 1);
    LCD_CS = 1;
}

//*** static functions *********************************************************

static void __lcd_goto (uint8_t addr)
{
    uint8_t buf[2];

    buf[0] = 0b00110000;    // function set (instruction table 0)
    buf[1] = 0x80 | addr;   // set DDRAM cmd + address
    
    // set register selection: command
    LCD_RS = 0;
    LCD_CS = 0;

    spi_transfer(buf, NULL, 2);
    
    LCD_CS = 1;    
}

//..............................................................................
