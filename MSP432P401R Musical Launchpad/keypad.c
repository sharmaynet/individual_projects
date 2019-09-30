/**
 * Keypad.c
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/17/19
 * File that:
 * - Has 2 keypad related functions to initialize and operate keypad
 * - Inputs:    - P2.4 - 2.7 are row buttons
 *              - P5.4 - 5.7 are column buttons
 *              - P6.4 - 6.7 are blue led buttons
 *              - P4.0 - 4.3 are red led buttons
 *              - P4.4 - 4.7 are green led buttons
 * - GetKey() find specific button you are on
 * - led_arr is an array of 16. a specific button should light up whenever its index is set high
 */

#include "msp.h"
#include "keypad.h"
#include <stdint.h>
#include <string.h>


static int led_arr[MAX_LEDS];

void Keypad_init(){
    P_ROW -> DIR |= (BTN1 | BTN2 | BTN3 | BTN4);                                // row buttons and leds are outputs
    P_BLUE -> DIR |= (LED1_BL_GR | LED2_BL_GR | LED3_BL_GR | LED4_BL_GR);       // blue led pins are outputs
    P_BLUE -> OUT &=~ (LED1_BL_GR | LED2_BL_GR | LED3_BL_GR | LED4_BL_GR);      // blue leds are turned off
    P_RED_GR -> DIR |= (LED1_BL_GR | LED2_BL_GR | LED3_BL_GR | LED4_BL_GR | LED1_GND_RD | LED2_GND_RD | LED3_GND_RD | LED4_GND_RD);     // red and green led pins are outputs
    P_RED_GR -> OUT &=~ (LED1_BL_GR | LED2_BL_GR | LED3_BL_GR | LED4_BL_GR| LED1_GND_RD | LED2_GND_RD | LED3_GND_RD | LED4_GND_RD);     // red and green led pins are turned off
    P_COL -> DIR |= (LED1_GND_RD | LED2_GND_RD | LED3_GND_RD | LED4_GND_RD);    // column ground leds are outputs
    P_COL -> OUT |= (LED1_GND_RD | LED2_GND_RD | LED3_GND_RD | LED4_GND_RD);    // column ground leds are set high
    P_COL -> DIR &= ~(BTN1 | BTN2 | BTN3 | BTN4);                               // column buttons are inputs
    P_COL -> REN |= (BTN1 | BTN2 | BTN3 | BTN4);                                // column buttons are set as pull down
    P_COL -> OUT &= ~(BTN1 | BTN2 | BTN3 | BTN4);
}

uint8_t Getkey(){
    uint8_t row, col, key;
    P_ROW->OUT |= (BTN1 | BTN2 | BTN3 | BTN4);                // Powers all row buttons on
    _delay_cycles(DELAY_KEY);                                 // account for button bounce
    col = (P_COL->IN & (BTN1 | BTN2 | BTN3 | BTN4));          // check if you get a column input
    if (col == 0)
        return NOTHING;                                       // No button pressed, return specific value
    for (row = 0; row < MAX_ROW; row++){                      // iterate through rows
        P_ROW->OUT &= ~(BTN1 | BTN2 | BTN3 | BTN4);           // Turns off row outputs
        P_ROW->OUT |= (BTN1 << row);                          // Turns on 1 row at a time
        _delay_cycles(DELAY_KEY);                             // account for button bounce
        col = P_COL->IN & (BTN1 | BTN2 | BTN3 | BTN4);        // check if you get a column input from specific row
        if (col != 0)                                         // column input exists, so exit for loop
            break;
    }
    P_ROW->OUT &= ~(BTN1 | BTN2 | BTN3 | BTN4);               // Turns of all rows
    if (row == MAX_ROW)                                       // Failsafe, no keys pressed
        return NOTHING;                                       // Return specific value when no button pressed
    col = col >> SHIFT_COL;                                   // Algorithm to find where we are on the keyboard
    if (col == THIRD_ROW_WRONG){                              // change col 4 to col 3
        col = THIRD_ROW_RIGHT;
    }
    else if (col == FOURTH_ROW_WRONG){                        // change col 8 to col 4
        col = FOURTH_ROW_RIGHT;
    }
    col -= ADJUST_ROW;                                        // Set up rows for algorithm to find where we are on keyboard
    key = col + (row*MAX_ROW);                                // keyboard algorithm
    return key;                                               // return button pressed
}
void light_up(int c){
    led_arr[c] = TURN_ON;                                     // turn on LED for specific button
}
void clear_led(){                                             // turn off all LEDs in the array
    int i = 0;
    for (i = 0; i<MAX_LEDS; i++){
       led_arr[i] = 0;
    }
}


