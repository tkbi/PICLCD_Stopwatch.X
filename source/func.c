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
#include "uart.h"
#include "eeprom.h"

//*** global variables *********************************************************

uint8_t uartBuf;
char hex[3];

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

// state counter
static uint16_t state_cnt = 0;

//*** prototypes ***************************************************************

/**
 * This function will update tthe stop watch and also display the new stop watch
 * value on the lcd (by calling func_disp_sw).
 */

static void __func_update_stopwatch (void);

/**
 * This function will convert the stop watch time (milli seconds, seconds and
 * minutes) to an string (char array) of the formar <M><M>:<m><m>:<ms><ms>.
 * 
 * @param pSw Pointer to the stop watch struct containing the stop watch values.
 * @return Pointer to the string (char array).
 */

static char* __func_time_to_str (sw_t *pSw);

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
 * 
 * @return True if the key event was accepted or false if not.
 */

static bool __func_sw_state_machine (void);

/**
 * The PIC will go sleeping if you call this function. Normally this function
 * wont be called manually. The stop watch will automatically call the function
 * it it remains longer than STP_TO_IDL_TIME * 10ms in the idle state.
 */

static void __func_sleep (void);

///**
// * 
// * @param val
// * @return 
// */
//
//static char* __func_uint8_to_hex (uint8_t val);

/**
 * 
 */

static void __func_auto_time_behaviour (void);

//*** functions ****************************************************************

void func_workload (void)
{    
    static uint8_t keyMem;
    
    // check if another 10ms are passed
    if( status.iXms )
    {
        status.iXms = false;

        // increase the state counter
        state_cnt++;
        
        // update stop watch every 10ms
        if(state == SW_STATE_RUN)
        {
            __func_update_stopwatch();
        }
        
        // check for an key released event
        keyMem |= __func_debounce();

        // changes on PB?
        if(keyMem & KEY_PB)
        {
            // reset the pressed & hold counter if key is accepted
            if( __func_sw_state_machine() )
            {
                debCntPB = 0;
                keyMem &= ~KEY_PB;
                state_cnt = 0;
            }
        }

        if(keyMem & KEY_USR)
        {
            // reset the pressed & hold counter
            debCntUSR = 0;
            keyMem &= ~KEY_USR;
        }
    }
    
    // call the uart tx-function if data is waiting out buffer
    if( status.iTx )
    {
        // the iTX flag will be cleared inside uart_tx (if buffer is empty)
        uart_tx();
    }
    
    // manage the automatically time behaviour of the stop watch
    // (e.g. automatically go sleeping after .. ms in idle state ..)
    __func_auto_time_behaviour();
}

//..............................................................................

void func_disp_sw (void)
{
    // display the new time
    lcd_write( __func_time_to_str(&sWatch), 0x00 );
}

//*** static functions *********************************************************

static void __func_update_stopwatch (void)
{
    // to do: only update the changed chars instead of every time re-writing
    // the hole display
    
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

static char* __func_time_to_str (sw_t *pSw)
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
    uint8_t ret = 0;

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
        // check which keys changed
        ret = actKeyDown ^ lastPressedKey;
        
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

static bool __func_sw_state_machine (void)
{
    bool keyAccepted = true;
            
    switch(state)
    {
        case SW_STATE_PRE_IDLE:
        {
            state = SW_STATE_IDLE;

            #ifdef DEBUG
                uart_print("state: PRE IDLE -> IDLE\n");
            #endif

            break;
        }

        case SW_STATE_IDLE:
        {
            // check how long the key was pressed
            if(debCntPB > KEY_HOLD_CLR && PB)
            {
                // display some status info
                lcd_write("Erase?  ",0);
                state = SW_STATE_CLR;      
                
                #ifdef DEBUG
                    uart_print("state: IDLE -> CLEAR\n");
                #endif
            }
            // is the key already released?
            else if(!PB)
            {
                // start a new measurement
                state = SW_STATE_RUN;
                
                #ifdef DEBUG
                    uart_print("state: IDLE -> RUN\n");
                #endif
            }
            else
            {
                // key was NOT accepted
                keyAccepted = false;
            }

            break;
        }

        case SW_STATE_RUN:
        {
            if(PB)
            {
                state = SW_STATE_PRE_STOP;

                #ifdef DEBUG
                    uart_print("state: RUN -> STOP\n");
                #endif
            }

            break;
        }

        case SW_STATE_PRE_STOP:
        {   
            state = SW_STATE_STOP;
            
            #ifdef DEBUG
                uart_print("state: PRE STOP -> STOP\n");
            #endif

            break;
        }

        case SW_STATE_STOP:
        {
            // check how long the key was pressed
            if(debCntPB > KEY_HOLD_SAVE && PB)
            {
                // save the last sw-value into the EEPROM
                // ...
                
                // clear the last measurement
                __func_clear_sw(&sWatch);
                
                // display an info message
                lcd_write("Saved   ",0);
                
                // and switch to the saved state (just show the "Saved" message
                // for some seconds)
                state = SW_STATE_SAVED;
               
                #ifdef DEBUG
                    uart_print("state: STOP -> SAVED\n");
                #endif
            }
            else if(!PB)
            {
                __func_clear_sw(&sWatch);
                func_disp_sw();
                state = SW_STATE_IDLE;
                
                #ifdef DEBUG
                    uart_print("state: STOP -> IDLE\n");
                #endif
            }
            else
            {
                // key was NOT accepted
                keyAccepted = false;
            }

            break;
        }

        case SW_STATE_CLR:
        {
            if(PB)
            {
                // if the program counter got here the user confirmed to "clear"
                // the complete stop watch memory
                eeprom_25LC256_clear();   

                lcd_write("Erased  ",0);

                // and go into the cleared state (just wait some time here to 
                // display the message and go back to idle)
                state = SW_STATE_CLRD;

                #ifdef DEBUG
                    uart_print("state: CLEAR -> CLEARED\n");
                #endif
            }
            
            break;
        }

        case SW_STATE_CLRD:
        {
            if(PB)
            {
                // if the program counter got here the user pushed the button again
                // to faster switch from state cleared to idle
                __func_clear_sw(&sWatch);

                // display again the 00:00:00
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_PRE_IDLE;

                #ifdef DEBUG
                    uart_print("state: CLEARED -> IDLE\n");
                #endif
            }

            break;
        }
        
        case SW_STATE_SAVED:
        {      
            if(PB)
            {
                // display again the 00:00:00
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_PRE_IDLE;

                #ifdef DEBUG
                    uart_print("state: SAVED -> IDLE\n");
                #endif
            }

            break;
        }
    }
    
    return keyAccepted;
}

//..............................................................................

static void __func_sleep (void)
{
    // shut the timer and lcd off
    timer2_stop();
    lcd_off();  
    
    // reset the state counter
    state_cnt = 0;
   
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
    timer2_start();
}

//..............................................................................

//static char* __func_uint8_to_hex (uint8_t val)
//{
//    hex[0] = val / 16 + '0';
//    hex[1] = val % 16 + '0';
//    
//    if( hex[0] > '9' ) hex[0] += 7;
//    if( hex[1] > '9' ) hex[1] += 7;
//    
//    hex[2] = '\0';
//
//    return hex;
//}

//..............................................................................

static void __func_auto_time_behaviour (void)
{
    switch(state)
    {
        case SW_STATE_IDLE:
        {
            // time to sleep?
            if(state_cnt > IDLE_TO_SLEEP_TIME)
            {
                __func_sleep();
            }
            
            break;
        }

        case SW_STATE_RUN:
        {
            break;
        }

        case SW_STATE_STOP:
        {
            // time to go from stop to idle?
            if(state_cnt > STOP_TO_IDLE_TIME)
            {
                state_cnt = 0;

                // clear the stop watch
                __func_clear_sw(&sWatch);

                // display the resetted time
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_IDLE;
                
                #ifdef DEBUG
                    uart_print("new state (auto): STOP -> IDLE\n");
                #endif
            }
            
            break;
        }

        case SW_STATE_CLR:
        {
            // time to leave "Clear?" state (because of no user confirmation)
            if(state_cnt > CLEAR_TO_IDLE_TIME)
            {                
                state_cnt = 0;

                // display the resetted time
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_IDLE;
                
                #ifdef DEBUG
                    uart_print("state (auto): CLEAR -> IDLE\n");
                #endif
            }

            break;
        }

        case SW_STATE_CLRD:
        {
            // time to leave "Cleard!" state
            if(state_cnt > CLEARED_TO_IDLE_TIME)
            {
                state_cnt = 0;

                // display the resetted time
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_IDLE;
                
                #ifdef DEBUG
                    uart_print("state (auto): CLEARED -> IDLE\n");
                #endif
            }
            
            break;
        }
        
        case SW_STATE_SAVED:
        {
            if(state_cnt > SAVED_TO_IDLE_TIME)
            {
                state_cnt = 0;

                // display the resetted time
                func_disp_sw();

                // and switch to the idle state
                state = SW_STATE_IDLE;
                
                #ifdef DEBUG
                    uart_print("state (auto): SAVED -> IDLE\n");
                #endif
            }
            
            break;
        }
    }
}

//..............................................................................
