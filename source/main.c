/*******************************************************************************
 *
 * File:        main.c
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
#include "main.h"
#include "lcd.h"
#include "timer.h"
#include "func.h"
#include "uart.h"
#include "eeprom.h"

//*** configuration ************************************************************

#pragma config FOSC     = IRC   // internal rc oscillator
#pragma config PLLEN    = OFF   // pll off
#pragma config FCMEN    = ON    // fail-safe clock monitor on
#pragma config PWRTEN   = OFF   // power-up timer off
#pragma config BOREN    = OFF   // brown-out reset off
#pragma config WDTEN    = OFF   // watchdog timer off
#pragma config MCLRE    = ON    // MCLR enabled (RA3 input pin disabled)
#pragma config LVP      = OFF   // low voltage programming off
#pragma config XINST    = OFF   // extended instruction set off

//*** globals ******************************************************************

status_t status = {0};

//*** prototypes ***************************************************************

static void __main_init_pic (void);

//*** functions ****************************************************************

void main (void)
{
    // init pic and uart
    __main_init_pic();
    uart_init();
    
    // init the lcd and display 00:00:00
    lcd_init();
    func_disp_sw();

    // init and start the timer 0 and 2
    timer0_init();
    timer2_init();
    timer2_start();
    
    // go and do your job
    while(1)
    {
        func_workload();
    }
}

//*** static functions *********************************************************

static void __main_init_pic (void)
{
    // select the internal rc oscillator frequency to 16MHz
    OSCCONbits.SCS = 0b10;
    OSCCONbits.IRCF = 0b111;
    
    // set input/output direction
    TRISA = 0b00011100;
    TRISB = 0b00110000;
    TRISC = 0b00000000;
    
    // enable pull up on RB4 (MISO)
    INTCON2bits.RABPU = 0;
    WPUBbits.WPUB4 = 1;
    
    // analog and digital selection
    ANSEL  = 0x00;
    ANSELH = 0x00;
    
    // set priority to high for INT2 (USR input)
    INTCON3bits.INT2IP = 1;
    
    // enable interrupt priotiries
    RCONbits.IPEN = 1;
    
    // enable interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    // some further initial GPIO settings
    EEPROM_CS = 1;
    // set LB6 default high (SCL)
    LATBbits.LB6 = 1;
}

//..............................................................................
