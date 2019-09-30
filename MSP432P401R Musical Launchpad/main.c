/*
 * main.c
 *
 *  Created by Sharmayne Tanedo, Gerome Cacho
 *  6/2/19
 *  File that:
 *      - initializes TimerA, MCLK to 48Mhz, SMCLK to 24Mhz, keypad, and SPI
 *      - wait for user input
 *      - depending on what button is pressed, check case statements for outputting note or changing colors/sound
 *      - if a note is pressed
 *          - turn on LED light depending on state you are in
 *          - adjust CCR value
 *      - if last 3 buttons are pressed
 *          - change color of led accordingly
 *
 */
#include "msp.h"
#include "keypad.h"
#include "synth.h"
#include "SPI.h"
#include "set_DCO.h"

void main(void){
    uint8_t c = NOTHING, previous = NOTHING;
    static int row = FIRST_ROW, col = FIRST_COL;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    set_48MHZ();                                    // MCLK - 48Mhz from HFXT, SMCLK - 24Mhz
    timer_enable_init();                            // enable TimerA
    Keypad_init();                                  // Initializes Keypad. See keypad.c
    SPI_init();                                     // Initialize SPI to output Voltage to speaker
    while (1){                                      // Infinite Loop
        c =  Getkey();                              // Consistently pulls key value
        if (previous != c){                         // if the button detects a different value from previous time
            if (previous!= NOTHING){
                P_BLUE->OUT &= ~(LED1_BL_GR<<row);  // turn blue, green, red leds low
                P_RED_GR->OUT &= ~(LED1_BL_GR<<row);
                P_RED_GR->OUT &= ~(LED1_GND_RD<<row);
                P_COL->OUT |= (LED1_GND_RD<<col);   // turn ground led high
             }
             clear_led();                           // clear led - led_arr is set low
        }
        _delay_cycles(DELAY_VAL);
        switch (c){
        case NOTE_C4:                               // first button - C4 case
            row = FIRST_ROW;                        // set row/columns
            col = FIRST_COL;
            if (check_state()== GREEN){             // piano sound
                P_RED_GR->OUT |=(LED1_BL_GR<<row);  // turn led on
                set_ccr0(CCR_C4_G);                 // set ccr
            }
            else if (check_state() == RED){         // perfect sine wave sound
                P_RED_GR->OUT |=(LED1_GND_RD<<row); // turn led on
                set_ccr0(CCR_C4_R);                 // set ccr
            }
            else{                                   // trumpet sound
                P_BLUE->OUT |= (LED1_BL_GR<<row);   // turn led on
                set_ccr0(CCR_C4_B);                 // set ccr
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);                 // ground led set low
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;           // enable IRQ for CCR0
            light_up(c);                                       // light up the led
            break;
        case NOTE_CSHARP4:                           // second button - CSHARP4 case
            row = FIRST_ROW;
            col = SECOND_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_CSHARP4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_CSHARP4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_CSHARP4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;
            light_up(c);
            break;
        case NOTE_D4:                                   // third button - D4 case
            row = FIRST_ROW;
            col = THIRD_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_D4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_D4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_D4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_DSHARP4:                               // fourth button - DSHARP4 case
            row = FIRST_ROW;
            col = FOURTH_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_DSHARP4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_DSHARP4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_DSHARP4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_E4:                                   // fifth button - E4 case
            row = SECOND_ROW;
            col = FIRST_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_E4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_E4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_E4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_F4:                                    // sixth button - F4 case
            row = SECOND_ROW;
            col = SECOND_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_F4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_F4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_F4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_FSHARP4:                              // seventh button - FSHARP4 case
            row = SECOND_ROW;
            col = THIRD_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_FSHARP4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_FSHARP4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_FSHARP4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_G4:                                    // eighth button - G4 case
            row = SECOND_ROW;
            col = FOURTH_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_G4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_G4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_G4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_GSHARP4:                              // ninth button - GSHARP4 case
            row = THIRD_ROW;
            col = FIRST_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_GSHARP4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_GSHARP4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_GSHARP4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_A4:                                   // tenth button - A4 case
            row = THIRD_ROW;
            col = SECOND_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_A4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_A4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_A4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_ASHARP4:                              // eleventh button - ASHARP4 case
            row = THIRD_ROW;
            col = THIRD_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_ASHARP4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_ASHARP4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_ASHARP4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_B4:                                   // Twelfth button - B4 case
            row = THIRD_ROW;
            col = FOURTH_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_B4_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_B4_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_B4_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case NOTE_C5:                                   // thirteenth button - C5 case
            row = FOURTH_ROW;
            col = FIRST_COL;
            if (check_state()== GREEN){
                P_RED_GR->OUT |=(LED1_BL_GR<<row);
                set_ccr0(CCR_C5_G);
            }
            else if (check_state() == RED){
                P_RED_GR->OUT |=(LED1_GND_RD<<row);
                set_ccr0(CCR_C5_R);
            }
            else{
                P_BLUE->OUT |= (LED1_BL_GR<<row);
                set_ccr0(CCR_C5_B);
            }
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable IRQ for CCR0
            light_up(c);
            break;
        case CHANGE_RED:                               // Fourteenth button - change to red/sine-wave
            row = FOURTH_ROW;                          // find rows and columns
            col = SECOND_COL;
            set_state(RED);                            // change state/color
            P_RED_GR->OUT |=(LED1_GND_RD<<row);        // turn on led
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            light_up(c);
            break;
        case CHANGE_GREEN:                             // fifteenth button - change to green/piano-wave
            row = FOURTH_ROW;
            col = THIRD_COL;
            set_state(GREEN);
            P_RED_GR->OUT |=(LED1_BL_GR<<row);
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            light_up(c);
            break;
        case CHANGE_BLUE:                               // sixteenth button - change to blue/trumpet-wave
            row = FOURTH_ROW;
            col = FOURTH_COL;
            set_state(BLUE);
            P_BLUE->OUT |= (LED1_BL_GR<<row);
            P_COL->OUT &= ~(LED1_GND_RD<<col);
            light_up(c);
            break;
        case NOTHING:                                   // no buttons pressed
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;   // disable IRQ for CCR0
            clear_led();                                // clear leds
            break;
        }
        previous = c;                                   // save value for the next iteration
    }
}
