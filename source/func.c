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
#include "timer.h"

//*** static variables *********************************************************

// stop watch' state machine
static uint8_t state = SW_STATE_IDLE;

// holds the current measurement
static sw_t sWatch = {0};

// last pressed/released key bitfields
static uint8_t lastPressedKey = 0;
static uint8_t lastReleasedKey = 0;

// press & hold key counter
static uint16_t debCntUSR = 0;
static uint16_t debCntPB = 0;

// idle/stop state counter
static uint16_t idl_cnt = 0;
static uint16_t stp_cnt = 0;

//*** prototypes ***************************************************************

/**
 * This function will update tthe stop watch and also display the new stop watch
 * value on the lcd (by calling func_disp_sw).
 */

void __func_update_stopwatch (void);

/**
 * This function will convert the stop watch time (milli seconds, seconds and
 * minutes) to an string (char array) of the formar <M><M>:<m><m>:<ms><ms>.
 * 
 * @param pSw Pointer to the stop watch struct containing the stop watch values.
 * @return Pointer to the string (char array).
 */

char* __func_time_to_str (sw_t *pSw);

/**
 * This function will debounce the external switches (PB and USR). When a key
 * was pressed or released the function will return this whith an event code.
 * If no key was neither pressed or released an no key event will be returned.
 * In case of an key released event you can use the additional press & hold 
 * counter variables (see debCntPB and debCntUSR) to check for what time the
 * button was pressed.
 * 
 * @return  NO_KEY_EVENT No key was neither pressed or released.
 *          KEY_PRESSED Some key/keys was/were pressed (see lastPressedKey).
 *          KEY_RELEASED Some key/keys was/were released (see lastReleasedKey 
 *          and the press & hold key variabled (see debCntPB and debCntUSR).
 */

static uint8_t __func_debounce (void);

/**
 * This function can be used to clear the stop watch measurement structure.
 * Normally you would do this after the user pressed (<3s) & released the PB 
 * after a measurement has been already stopped.
 * 
 * @param pSw Pointer to the stop watch measurement struct to clear.
 */

static void __func_clear_sw (sw_t *pSw);

/**
 * This function handles the stop watch state machine and should always be
 * called if the PB was released.
 */

static void __func_sw_state_machine (void);

/**
 * 
 */

static void __func_sleep (void);

//*** functions ****************************************************************

void func_workload (void)
{    
    // check if another 10ms are passed
    if( status.iXms )
    {
        status.iXms = false;

        // update stop watch every 10ms
        if(state == SW_STATE_RUN)
        {
            __func_update_stopwatch();
        }
        
        // check for an key released event
        if( __func_debounce() == KEY_RELEASED )
        {
            if(lastReleasedKey & KEY_PB)
            {
                __func_sw_state_machine();

                // reset the counter
                idl_cnt = 0;
                stp_cnt = 0;
                
                // confirm the released key
                lastReleasedKey &= ~KEY_PB;
                
                // reset the pressed & hold counter
                debCntPB = 0;
            }
            
            if(lastReleasedKey & KEY_USR)
            {
                // confirm the released key
                lastReleasedKey &= ~KEY_USR;
                
                // reset the idle counter
                idl_cnt = 0;
                
                // trigger an event here..
                
                // reset the pressed & hold counter
                debCntUSR = 0;
            }
        }
        
        // in idle or stop state?
        if(state == SW_STATE_IDLE)
        {
            idl_cnt++;
        }
        else if( state == SW_STATE_STOP )
        {
            stp_cnt++;
        }
    }
    
    // time to sleep?
    if(idl_cnt > IDL_TO_SLP_TIME)
    {
        __func_sleep();
    }
    
    // time to go from stop to idle?
    if(stp_cnt > STP_TO_IDL_TIME)
    {
        __func_clear_sw(&sWatch);
        func_disp_sw();
        state = SW_STATE_IDLE;
    }
}

//..............................................................................

void func_disp_sw (void)
{
    // display the new time
    lcd_return_home();
    lcd_write( __func_time_to_str(&sWatch) );
}

//*** static functions *********************************************************

void __func_update_stopwatch (void)
{
    // increment millisecond (+1 => +10ms)
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
    
    // display the new value on the LCD
    func_disp_sw();
}

//..............................................................................

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

//..............................................................................

static uint8_t __func_debounce (void)
{
    uint8_t actKeyDown = 0;
    uint8_t ret = NO_KEY_EVENT;

    // check if PB is pressed right now
    if(PB)
    {
        // set the bit for PB
        actKeyDown |= KEY_PB;
        
        // increment the press&hold count var for PB
        debCntPB++;
    }
    
    // check if USR is pressed right now
    if(USR)
    {
        // set the bit for USR
        actKeyDown |= KEY_USR;
        
        // increment the press&hold count var for USR
        debCntUSR++;
    } 
    
    // is there a new key event (key pressed or released)?
    if(actKeyDown != lastPressedKey)
    {
        // use KEY_PRESSED as default event
        ret = KEY_PRESSED;

        // was is a key released event?
        if(actKeyDown < lastPressedKey)
        {
            lastReleasedKey = actKeyDown ^ lastPressedKey;
            ret = KEY_RELEASED;
        }
        
        // take the new actKeyDown as lastPressedKey
        lastPressedKey = actKeyDown;
    }
    
    return ret;
}

//..............................................................................

static void __func_clear_sw (sw_t *pSw)
{
    pSw->m  = 0;
    pSw->ms = 0;
    pSw->s  = 0;
}

//..............................................................................

static void __func_sw_state_machine (void)
{
    switch(state)
    {
        case SW_STATE_IDLE:
        {
            // check how long the key was pressed
            if(debCntPB > KEY_HOLD_CLR)
            {
                // delete the complete sw-memory
                // to do
            }
            else if(debCntPB > KEY_HOLD_SAVE)
            {
                // save the last sw-value
                // to do
            }
            else
            {
                state = SW_STATE_RUN;
            }

            break;
        }
        
        case SW_STATE_RUN:
        {
            // check how long the key was pressed
            if(debCntPB > KEY_HOLD_CLR)
            {
                // delete the complete sw-memory
                // to do
            }
            else if(debCntPB > KEY_HOLD_SAVE)
            {
                // save the last sw-value
                // to do
            }
            else
            {
                state = SW_STATE_STOP;
            }

            break;
        }
        
        case SW_STATE_STOP:
        {
            // check how long the key was pressed
            if(debCntPB > KEY_HOLD_CLR)
            {
                // delete the complete sw-memory
                // to do
            }
            else if(debCntPB > KEY_HOLD_SAVE)
            {
                // save the last sw-value
                // to do
            }
            else
            {
                __func_clear_sw(&sWatch);
                func_disp_sw();
                state = SW_STATE_IDLE;
            }

            break;
        }
    }
}

//..............................................................................

static void __func_sleep (void)
{
    // shut the timer and lcd off
    timer_stop();
    lcd_off();  
    
    // reset the idle counter
    idl_cnt = 0;
   
    // enable INT2 to wake up the pic  
    INTCON3bits.INT2IE = 1;
    
    // clear a may existing INT2 flag
    INTCON3bits.INT2IF = 0;

    SLEEP();
    NOP();
    
    // disable INT2 after wakeup   
    INTCON3bits.INT2IE = 0;
    
    // turn the lcd on and display 00:00:00
    lcd_init();
    func_disp_sw();   
    
    // wait for PB is released
    __delay_ms(100);
    while(PB);
    
    // turn the timer on (normal functionallity available from now)
    timer_start();
}

//..............................................................................
