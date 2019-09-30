/*
 * synth.c
 *
 *  Created by Sharmayne Tanedo, Gerome Cacho
 *  6/2/19
 *  File that:
 *      - defines the piano, trumpet, and perfect sine array that the ISR outputs to the speaker
 *      - set_ccr0() changes the CCR depending on what note you want to play - based on frequency of a note
 *      - set_state() changes the color and the sound
 *      - timer_enable_init() initializes CCR0 for TimerA
 *      - check_state() returns to the main what color to output
 *
 */

#include "msp.h"
#include "synth.h"
#include "SPI.h"

//piano, trumpet, and sine waves
int piano_arr[MAX_DATA_PTS] = {2158, 2652,3111,3505,3818,4044,4192,4276,4313,4317,4297,4258,4204,4136,4061,3988,3926,3885,3870,3881,3911,3949,3983,4005,4009,3996,3971,3940,3905,3868,3824,3768,3694,3603,3497,3385,3277,3183,3108,3051,3007,2968,2923,2869,2805,2738,2674,2620,2583,2559,2542,2526,2500,2462,2413,2358,2306,2268,2246,2242,2250,2262,2271,2273,2266,2256,2248,2249,2261,2281,2304,2322,2328,2318,2293,2260,2226,2197,2176,2163,2153,2140,2119,2090,2056,2023,1999,1989,1994,2012,2035,2055,2067,2068,2060,2050,2043,2045,2054,2067,2074,2070,2048,2009,1958,1903,1854,1816,1790,1774,1757,1733,1695,1642,1578,1510,1447,1393,1348,1309,1265,1208,1133,1039,931,819,713,621,548,492,448,411,376,345,320,307,311,333,367,405,435,446,431,390,328,254,179,112,58,19,0,3,40,124,273,499,812,1207,1666};
int trumpet_arr[MAX_DATA_PTS] = {5541,7346,8934,10136,10855,11083,10894,10410,9773,9110,8507,8005,7604,7278,6990,6712,6428,6135,5844,5569,5326,5129,4988,4908,4889,4925,5002,5103,5207,5293,5350,5375,5378,5373,5377,5403,5452,5519,5589,5645,5677,5681,5663,5635,5611,5600,5606,5621,5635,5636,5613,5564,5491,5405,5318,5244,5193,5174,5189,5238,5314,5410,5513,5612,5696,5759,5800,5825,5846,5877,5929,6005,6099,6194,6266,6289,6243,6118,5921,5674,5408,5161,4964,4840,4794,4817,4889,4984,5078,5154,5206,5237,5258,5284,5325,5387,5471,5570,5673,5769,5845,5894,5909,5890,5839,5764,5677,5591,5519,5470,5447,5448,5462,5477,5482,5472,5448,5420,5402,5406,5438,5495,5564,5631,5681,5706,5710,5705,5708,5733,5790,5877,5980,6081,6159,6194,6175,6095,5953,5756,5513,5238,4946,4654,4369,4092,3804,3477,3076,2573,1970,1307,671,188,0,230,951,2155,3745};
int sine_arr[MAX_DATA_PTS] = {2048,2128,2209,2290,2370,2449,2528,2607,2684,2760,2836,2910,2982,3053,3123,3191,3257,3322,3384,3444,3502,3558,3611,3662,3711,3757,3800,3840,3878,3913,3945,3974,4000,4023,4043,4059,4073,4083,4090,4095,4095,4093,4088,4079,4067,4052,4034,4013,3988,3961,3931,3897,3861,3822,3780,3736,3689,3639,3587,3533,3476,3417,3356,3292,3227,3160,3091,3021,2949,2876,2801,2725,2648,2571,2492,2413,2333,2253,2172,2091,2010,1929,1849,1769,1689,1609,1531,1453,1376,1300,1225,1152,1080,1009,940,873,808,745,683,624,567,512,460,410,363,318,276,237,200,167,136,109,84,63,44,29,17,8,2,0,0,4,11,21,35,51,71,93,119,148,179,214,251,292,335,380,429,479,533,588,646,706,768,832,898,966,1036,1107,1180,1253,1329,1405,1482,1560,1639,1719,1799,1879,1960};

int ccr0;                                            // ccr0 value dependent on the note's frequency - how often you iterate through array data points
int state = RED;                                     // what color the led board displays and what noise it plays

void set_ccr0(uint16_t i){
    ccr0 = i;                                        // set ccr0 value for whatever key you are pressing
}

void set_state(int i){
    state = i;                                       // set state to either red, green, or blue
}

void timer_enable_init(){
    TIMER_A0->CTL= TIMER_A_CTL_TASSEL_2
                    | TIMER_A_CTL_MC_2;              // run off SMCLK (24MHz) & cont mode
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;        // disable IRQ for CCR0 - sound - wait until button gets pressed to enable
    NVIC -> ISER[0] = 1 << (TA0_0_IRQn & NVIC_VAL);  // NVIC for Ta0_0
    __enable_irq();
}

int check_state(){
    return state;                                   // check what state youre on and return it to the main
}

void TA0_0_IRQHandler(){                            // ISR that samples whole period one by one
    static int index = 0;
    if (TIMER_A0->CCTL[0]
                       & TIMER_A_CCTLN_CCIFG){      // flag is on
        TIMER_A0 ->CCR[0]+= ccr0;
        TIMER_A0-> CCTL[0] &=~ TIMER_A_CCTLN_CCIFG;
        if (state == RED){                          // output perfect sine wave if outputting red leds
            set_outputV(sine_arr[index]);
        }
        else if (state == GREEN){                   // output piano wave if outputting green leds
            set_outputV(piano_arr[index]);
        }
        else                                        // output trumpet wave if outputting blue leds
        {
            set_outputV(trumpet_arr[index]);
        }
        index++;
        if (index == MAX_DATA_PTS){                 // reset index if reached the end of the array
            index = 0;
        }
    }
}
