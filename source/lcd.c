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
    spi_send(buf, NULL, 9);
    LCD_CS = 1;
    
    __delay_ms(10);
    
    //lcd_return_home();
}

//..............................................................................

void lcd_return_home (void)
{
    uint8_t buf [2];
 
    // set register selection: command
    LCD_RS = 0;
    
    buf[0] = 0b00110000;    // function set (instruction table 0)
    buf[1] = 0b00000010;    // go home
    
    LCD_CS = 0;
    spi_send(buf, NULL, 2);
    LCD_CS = 1;
}

//..............................................................................

void lcd_write (char *pStr)
{
    // set register selection: data
    LCD_RS = 1;
    LCD_CS = 0;
    
    while(*pStr)
    {
        spi_send((uint8_t*)pStr, NULL, 1);
        
        pStr++;
    }
    
    LCD_CS = 1;
}
