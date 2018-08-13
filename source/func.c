/*******************************************************************************
 *
 * File:        func.c
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
#include "func.h"
#include "lcd.h"

//*** prototypes ***************************************************************

void __func_update_stopwatch (void);
char* __func_time_to_str (sw_t *pSw);

//*** functions ****************************************************************

void func_workload (void)
{
    // check if  another 10ms are passed
    if( status.ixms )
    {
        status.ixms = 0;
        __func_update_stopwatch();       
    }
}

//*** static functions *********************************************************

void __func_update_stopwatch (void)
{
    static sw_t sWatch = {0};

    // increment milli second (+1 => +10ms)
    sWatch.ms += 1;
    
    // 1000ms passed?
    if( sWatch.ms > 99)
    {
        sWatch.ms = 0;
        sWatch.s++;

        // one minute passed?
        if( sWatch.s > 59 )
        {
            sWatch.s = 0;
            sWatch.m++;
            
            // more than 99 minutes passed?
            if( sWatch.m > 99 )
            {
                sWatch.m = 0;
            }
        }
    }
    
    // display the new time
    lcd_return_home();
    lcd_write( __func_time_to_str(&sWatch) );
}

char* __func_time_to_str (sw_t *pSw)
{
    static char buf[8];
    
    buf[0] = (pSw->m  / 10) + '0';
    buf[1] = (pSw->m  % 10) + '0';
    buf[2] = ':';
    buf[3] = (pSw->s  / 10) + '0';
    buf[4] = (pSw->s  % 10) + '0';
    buf[5] = ':';
    buf[6] = (pSw->ms / 10) + '0';
    buf[7] = (pSw->ms % 10) + '0';
    
    return buf;
}