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
#include <stdio.h>

#include "main.h"
#include "func.h"
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "eeprom.h"
#include "build.h"

//*** global variables *********************************************************

uint8_t uartBuf;

//*** static variables *********************************************************

// stop watch' state machine
static uint8_t state = SW_STATE_IDLE;

// holds the current measurement
static __pack sw_t sWatch = {0};

// last pressed/released key bitfields
static uint8_t lastPressedKey = 0;
static uint8_t lastReleasedKey = 0;

// press & hold key counter
static uint16_t debCntUSR = 0;
static uint16_t debCntPB = 0;

// state counter
static uint16_t state_cnt = 0;

// this buffer is used for converting numbers to its string representation
static char gBuf[8];

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

/**
 * This function will convert a byte into its hexadecimal representation.
 * 
 * @param val Byte value to be convert
 * @return Pointer to the null terminated hex string of val
 */

//static char* __func_uint8_to_hex (uint8_t val);

/**
 * This function will convert a 16-bit value into its decimal representation.
 * 
 * @param val uint16_t value to be convert
 * @return Pointer to the null terminated decimal string of val
 */

static char* __func_uint16_to_dec (uint16_t val);

/**
 * This function manages the automatical time behaviour of the stop watch. One
 * example is the automatically switch to sleep state after the stop watch was
 * idle for some time. The function must be called every 10ms (system tick)
 * within the function func_workload().
 */

static void __func_auto_time_behaviour (void);

/**
 * You can use this function to the latest stop watch measurement (which is not
 * yet erased due to automatically switching to the idle state) into the
 * external EEPROM.
 * 
 * @param pSw Stop watch measurement to save into the external EEPROM.
 * @return Address/Slot where the measurement was saved.
 */

static uint16_t __func_save (sw_t *pSw);

/**
 * This function will check if a new measurement is a new record. If the new 
 * measurement is below the current record measurement (pRec) the function will
 * return true. Otherwise false (no new record).
 * 
 * @param pSw Pointer to the latest measurement. 
 * @return True if the last measurement is a new record otherwise false.
 */

static bool __func_is_new_record (sw_t *pSw);

/**
 * This function will read the current record.
 * 
 * @param pRec Pointer to provided memory to store the record measurement at.
 * @return 0 if the record was read successfully or 1 if there is no record.
 */

static uint8_t __func_get_record (sw_t *pRec);

/**
 * This function will read the address pointer to the first free address inside
 * the external EEPROM. This address (not a pointer) can be used to store the
 * next data into the external EEPROM.
 * 
 * @return The 16 bit address to the next free slot inside the ext. EEPROM
 */

static uint16_t __func_get_addr_ptr (void);

/**
 * You can update the address value that informs about the next free address
 * slot inside the external EEPROM. This function will be called if new data
 * was written into the external EEPROM.
 * 
 * @param addr_ptr New address to the next free slot inside the ext. EEPROM
 */

static void __func_set_addr_ptr (uint16_t addr_ptr);

/**
 * Call this function to clear all saved stop watch measurements. The function
 * wont override all memory of the external EEPROM with e.g. zeros. No.. it will
 * only set the internal address-pointer back to 0x0004. The data inside the
 * external EEPROM remains unchanged.
 */

static void __func_clear_eeprom (void);

/*
 * This function will handle the remote messages. If the stopwatch gets remote
 * Messages from the LCD-Stopwatch Remote (PC Tool) this messages will be
 * handled within this function.
 * 
 * @return True if the receive buffer is not yet empty otherwise false.
 */

static bool __func_remote_sm (void);

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
    
    // some data over the UART interface was received
    if( status.iRx )
    {
        state_cnt = 0;
        
        // handle incomming messages
        status.iRx = __func_remote_sm();
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
    gBuf[0] = (pSw->m  / 10) + '0';
    gBuf[1] = (pSw->m  % 10) + '0';
    gBuf[2] = ':';
    gBuf[3] = (pSw->s  / 10) + '0';
    gBuf[4] = (pSw->s  % 10) + '0';
    gBuf[5] = ':';
    gBuf[6] = (pSw->ms / 10) + '0';
    gBuf[7] = (pSw->ms % 10) + '0';
    
    return gBuf;
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
    uint16_t addr;
            
    switch(state)
    {
        case SW_STATE_PRE_IDLE:
        {
            state = SW_STATE_IDLE;
            
            // send the "back in idle cmd"
            uart_print("<7>");

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
                
                // check if the measurement is a new record
                if( __func_is_new_record(&sWatch) )
                {
                    state = SW_STATE_RECORD;

                    #ifdef DEBUG
                        uart_print("state: RUN -> RECORD\n");
                    #endif
                }
                else
                {
                    #ifdef DEBUG
                        uart_print("state: RUN -> STOP\n");
                    #endif
                }
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
                addr = __func_save(&sWatch);

                // display an info message (-> #xxxx)
                lcd_write(__func_uint16_to_dec((addr-2)>>2), 3);
                lcd_write("-> #",0);
                
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
                __func_clear_eeprom();   
                lcd_write("Erased  ",0);

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
                // display again the 00:00:00
                __func_clear_sw(&sWatch);
                func_disp_sw();

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
                __func_clear_sw(&sWatch);
                func_disp_sw();

                state = SW_STATE_PRE_IDLE;

                #ifdef DEBUG
                    uart_print("state: SAVED -> IDLE\n");
                #endif
            }

            break;
        }
        
        case SW_STATE_RECORD:
        {      
            if(!PB)
            {
                lcd_write("Record! ",0);
            }
            else
            {
                // display again the 00:00:00
                __func_clear_sw(&sWatch);
                func_disp_sw();

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

    // enable auto wake up (UART receive)
    BAUDCONbits.WUE = 1;
    
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
//    tmp_buf[0] = val / 16 + '0';
//    tmp_buf[1] = val % 16 + '0';
//    
//    if( tmp_buf[0] > '9' ) tmp_buf[0] += 7;
//    if( tmp_buf[1] > '9' ) tmp_buf[1] += 7;
//    
//    tmp_buf[2] = '\0';
//
//    return tmp_buf;
//}

//..............................................................................

static char* __func_uint16_to_dec (uint16_t val)
{
    gBuf[0] = val/10000 + '0';
    val %= 10000;
    gBuf[1] = val/ 1000 + '0';
    val %=  1000;
    gBuf[2] = val/  100 + '0';
    val %=   100;
    gBuf[3] = val/   10 + '0';
    val %=    10;
    gBuf[4] = val       + '0';
    
    gBuf[5] = '\0';

    return gBuf;
}

//..............................................................................

static void __func_auto_time_behaviour (void)
{
    bool go_idle = false;

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

        case SW_STATE_STOP:
        {
            // time to go from stop to idle?
            if(state_cnt > STOP_TO_IDLE_TIME)
            {
                go_idle = true;
                
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
                go_idle = true;
                
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
                go_idle = true;
                
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
                go_idle = true;
                
                #ifdef DEBUG
                    uart_print("state (auto): SAVED -> IDLE\n");
                #endif
            }
            
            break;
        }
        
        case SW_STATE_RECORD:
        {
            if(state_cnt > RECORD_TO_IDLE_TIME)
            {
                go_idle = true;
                
                #ifdef DEBUG
                    uart_print("state (auto): RECORD -> IDLE\n");
                #endif
            }
            
            break;
        }
        
        default: break;
    }
    
    if(go_idle)
    {
        state_cnt = 0;

        // display the resetted time
        __func_clear_sw(&sWatch);
        func_disp_sw();

        state = SW_STATE_IDLE;
        
        // send the "back in idle cmd"
        uart_print("<7>");
    }
}

//..............................................................................

static uint16_t __func_save (sw_t *pSw)
{
    uint16_t addr = __func_get_addr_ptr();
    
    // take this as record if this is the first one inside the EEPROM
    if(addr == 0x0004)
    {
        eeprom_25LC256_write(0x0002, (uint8_t*)(&addr), 2);
    }
    
    // store the latest measurement
    eeprom_25LC256_write(addr, (uint8_t*)pSw, SIZE_OF_SW);
    
    // update the address of the next free slot
    addr += SIZE_OF_SW;
    
    // update the address pointer (next free slot)
    __func_set_addr_ptr(addr);
    
    return addr;
}

//..............................................................................

static bool __func_is_new_record (sw_t *pSw)
{
    bool new_rec = false;
    uint16_t addr;
    sw_t rec;
    
    // get the latest record (abort if no latest record was found)
    if( __func_get_record(&rec) )
    {
        #ifdef DEBUG
            uart_print("no data in eeprom\n");
        #endif

        return new_rec;
    }
    
    // check if the new measurement is a new record
    if(pSw->m < rec.m)
    {
        new_rec = true;
    }
    else if(pSw->m == rec.m)
    {
        if(pSw->s < rec.s)
        {
            new_rec = true;
        }
        else if(pSw->s == rec.s)
        {
            if(pSw->ms < rec.ms)
            {
                new_rec = true;
            }
        }
    }
    
    // update the record + address pointer if new record
    if( new_rec )
    {
        #ifdef DEBUG
            uart_print("record\n");
        #endif
            
        // get the next free slot
        addr = __func_get_addr_ptr();
        
        // store the new measurement
        eeprom_25LC256_write(addr, (uint8_t*)pSw, SIZE_OF_SW);
        
        // update the record address to this slot
        eeprom_25LC256_write(0x0002, (uint8_t*)(&addr), 2);
        
        // and increment the next free slot address
        addr += SIZE_OF_SW;
        __func_set_addr_ptr(addr);
    }    
    
    return new_rec;
}

//..............................................................................


static uint8_t __func_get_record (sw_t *pRec)
{
    uint16_t addr;

    // read the address pointer of the current record
    eeprom_25LC256_read(0x0002, (uint8_t*)(&addr), 2);
    
    // abort if the record address pointer is 0x0000
    // (this means there is no storred record/data inside the EEPROM)
    if(addr == 0x0000)
    {
        return 1;
    }

    // read the current record (shortest measured time)
    eeprom_25LC256_read(addr, (uint8_t*)pRec, SIZE_OF_SW);
    
    return 0;
}

//..............................................................................

static uint16_t __func_get_addr_ptr (void)
{
    uint16_t addr_ptr;
    
    eeprom_25LC256_read(0x0000, (uint8_t*)(&addr_ptr), 2);
    
    return addr_ptr;
}

//..............................................................................

static void __func_set_addr_ptr (uint16_t addr_ptr)
{
    eeprom_25LC256_write(0x0000, (uint8_t*)(&addr_ptr), 2);
}

//..............................................................................

static void __func_clear_eeprom (void)
{
    uint8_t buf [2] = {0x00, 0x00};
    
    // reset the next free slot to address 0x0004
    __func_set_addr_ptr(0x0004);
    
    // clear the record (this is only a address)
    eeprom_25LC256_write(0x0002, buf, 2);
}

//..............................................................................

static bool __func_remote_sm (void)
{
    static uint8_t remState = REM_STATE_IDLE;
    static int8_t cmd = 0;
    uint16_t addr;
    
    switch(remState)
    {
        // ignore everything else than '<' as first char
        case REM_STATE_IDLE:
        {
            if(*pInBufRd == '<')
            {
                remState = REM_STATE_START;
            }
            break;
        }
        // the message started, now read the command number
        case REM_STATE_START:
        {
            cmd = *pInBufRd;
            remState = REM_STATE_END;
            break;
        }
        // now wait/read for the end char '>'
        case REM_STATE_END:
        {
            if(*pInBufRd == '>')
            {
                // handle the command
                switch(cmd)
                {
                    // ping
                    case '0':
                    {
                        uart_print("<0>");
                        break;
                    }
                    // read system info
                    case '1':
                    {
                        uart_print("<1|");
                        uart_print(__func_uint16_to_dec(BUILD_NR));
                        uart_print("|");
                        uart_print(BUILD_DATE);
                        uart_print(">");
                        break;
                    }
                    // erase memory
                    case '2':
                    {
                        state = SW_STATE_CLRD;
                        __func_clear_eeprom();
                        lcd_write("Erased  ",0);
                        uart_print("<2>");
                        break;
                    }
                    // export data
                    case '3':
                    {
                        break;
                    }
                    // start measurement
                    case '4':
                    {
                        state = SW_STATE_RUN;
                        uart_print("<4>");
                        break;
                    }
                    // stop measurement
                    case '5':
                    {
                        state = SW_STATE_STOP;
                        uart_print("<5|");
                        uart_print(__func_time_to_str(&sWatch));
                        uart_print(">");
                        break;
                    }
                    // save measurement
                    case '6':
                    {
                        state = SW_STATE_SAVED;
                        
                        // save the last sw-value into the EEPROM
                        addr = __func_save(&sWatch);

                        // display an info message (-> #xxxx)
                        lcd_write(__func_uint16_to_dec((addr-2)>>2), 3);
                        lcd_write("-> #",0);  
                        
                        uart_print("<6>");
                        break;
                    }
                    // unknown command
                    default: break;
                }
            }
            
            remState = REM_STATE_IDLE;
            break;
        }
    }
    
    // next time next char
    pInBufRd++;
    
    return (pInBufRd != pInBufWr);
}

//..............................................................................
