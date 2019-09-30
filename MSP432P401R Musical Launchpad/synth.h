
/**
 * synth.h
 *
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/17/19
 * Header file that:
 * - Defines macros used for synth.c and main.c
 * - includes
 *      - case statements for what note is being pressed
 *      - defines ccr values depending on frequency of each note
 *          - each sound (sine, piano, trumpet) has a different ccr value from each other
 *      - set row and column gpio
 *      - set color options
 */
#ifndef SYNTH_H_
#define SYNTH_H_

//case statements
#define NOTE_C4 0
#define NOTE_CSHARP4 1
#define NOTE_D4 2
#define NOTE_DSHARP4 3
#define NOTE_E4 4
#define NOTE_F4 5
#define NOTE_FSHARP4 6
#define NOTE_G4 7
#define NOTE_GSHARP4 8
#define NOTE_A4 9
#define NOTE_ASHARP4 10
#define NOTE_B4 11
#define NOTE_C5 12
#define CHANGE_RED 13
#define CHANGE_GREEN 14
#define CHANGE_BLUE 15

//ccr values for the frequencies of a note
#define CCR_C4_G 578
#define CCR_C4_R 577
#define CCR_C4_B 585
#define CCR_CSHARP4_G 546
#define CCR_CSHARP4_R 545
#define CCR_CSHARP4_B 549
#define CCR_D4_G 511
#define CCR_D4_R 513
#define CCR_D4_B 512
#define CCR_DSHARP4_G 483
#define CCR_DSHARP4_R 483
#define CCR_DSHARP4_B 486
#define CCR_E4_G 458
#define CCR_E4_R 458
#define CCR_E4_B 458
#define CCR_F4_G 432
#define CCR_F4_R 432
#define CCR_F4_B 432
#define CCR_FSHARP4_G 408
#define CCR_FSHARP4_R 406
#define CCR_FSHARP4_B 408
#define CCR_G4_G 385
#define CCR_G4_R 381
#define CCR_G4_B 386
#define CCR_GSHARP4_G 363
#define CCR_GSHARP4_R 364
#define CCR_GSHARP4_B 363
#define CCR_A4_G 344
#define CCR_A4_R 345
#define CCR_A4_B 345
#define CCR_ASHARP4_G 323
#define CCR_ASHARP4_R 321
#define CCR_ASHARP4_B 317
#define CCR_B4_G 304
#define CCR_B4_R 305
#define CCR_B4_B 305
#define CCR_C5_G 291
#define CCR_C5_R 290
#define CCR_C5_B 288

//rows and columns
#define DELAY_VAL 50
#define FIRST_ROW 0
#define SECOND_ROW 1
#define THIRD_ROW 2
#define FOURTH_ROW 3
#define FIRST_COL 0
#define SECOND_COL 1
#define THIRD_COL 2
#define FOURTH_COL 3

//color options
#define RED 0
#define GREEN 1
#define BLUE 2


//subfunction macros
#define MAX_DATA_PTS 159
#define NVIC_VAL 31
//functions
void timer_enable_init();
void set_ccr0(uint16_t i);
void set_state(int i);
int check_state();

#endif /* SYNTH_H_ */
