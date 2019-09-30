
/**
 * Keypad.h
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/17/19
 * Header file that:
 * - Defines macros used for Keypad.c file
 * - includes GPIOs for LED
 */


#ifndef KEYPAD_H_
#define KEYPAD_H_
//GPIO
#define P_COL P5
#define P_ROW P2
#define P_BLUE P6
#define P_RED_GR P4
#define BTN1 BIT4
#define BTN2 BIT5
#define BTN3 BIT6
#define BTN4 BIT7
#define LED1_GND_RD BIT0
#define LED2_GND_RD BIT1
#define LED3_GND_RD BIT2
#define LED4_GND_RD BIT3
#define LED1_BL_GR BIT4
#define LED2_BL_GR BIT5
#define LED3_BL_GR BIT6
#define LED4_BL_GR BIT7

#define NOTHING 0xFF
#define DELAY_WAIT 300000
#define TOP_COUNT 16
#define BOTTOM_COUNT 32
#define MAX_COL 3
#define MAX_ROW 4
#define SHIFT_COL 4
#define ADJUST_ROW 1
#define DELAY_KEY 500
#define DIVIDE_ROW 16
#define THIRD_ROW_WRONG 4
#define THIRD_ROW_RIGHT 3
#define FOURTH_ROW_WRONG 8
#define FOURTH_ROW_RIGHT 4
#define TURN_ON 1
#define MAX_LEDS 16

//functions
void Keypad_init();
uint8_t Getkey();
void light_up(int c);
void clear_led();
#endif /* KEYPAD_H_ */
