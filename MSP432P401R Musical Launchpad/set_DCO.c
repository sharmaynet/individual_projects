/**
 * set_DCO.c
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/10/19
 * Sub function that:
 * - takes nominal frequency from TI provided header
 * - Unlocks Clock System (CS) register by inputing
 *   special value
 * - Sets the Digital Controller Oscillator (DCO) to
 *   nominal frequency
 */

#include "msp.h"
#include "set_DCO.h"

void set_DCO(int freq)                      //Input is nominal frequency
{                                           //(see TI header for details)
    if (freq == FREQ_48_MHZ){               //Special Case for 48 MHz Clk
        while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
            ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
            ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
    }
    CS->KEY = CS_KEY_VAL;                                 //Unlocks CS Register
    CS->CTL0 = freq;                                      //Sets nominal value of DCO frequency
    CS->CTL1 = CS_CTL1_SELM__DCOCLK
                    |CS_CTL1_SELS__DCOCLK   // SMCLK and HSMCLK to DCO
                    |CS_CTL1_DIVS_1;         // SMCLK/2 = 24Mhz
    CS->KEY = 0;                            //Lock CS Register
}




void set_48MHZ(){                       // HFXT code (Obtained from example code CS 9)

    uint32_t currentPowerState;
    currentPowerState = PCM->CTL0 & PCM_CTL0_CPM_MASK;

    if (currentPowerState != PCM_CTL0_CPM_0)
       error();

    while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
    PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
    while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
    if (PCM->IFG & PCM_IFG_AM_INVALID_TR_IFG)
       error();                            // Error if transition was not successful
    if ((PCM->CTL0 & PCM_CTL0_CPM_MASK) != PCM_CTL0_CPM_1)
       error();                            // Error if device is not in AM1_LDO mode

    /* Step 2: Configure Flash wait-state to 1 for both banks 0 & 1 */
    FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
           FLCTL_BANK0_RDCTL_WAIT_1;
    FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) |
           FLCTL_BANK1_RDCTL_WAIT_1 ;

    /* Step 3: Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*/


    PJ->SEL0 |= BIT2 | BIT3;                // Configure PJ.2/3 for HFXT function
    PJ->SEL1 &= ~(BIT2 | BIT3);

    CS->KEY = CS_KEY_VAL ;                  // Unlock CS module for register access
    CS->CTL2 |= CS_CTL2_HFXT_EN | CS_CTL2_HFXTFREQ_6 | CS_CTL2_HFXTDRIVE;
    while(CS->IFG & CS_IFG_HFXTIFG)
       CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
    CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK | CS_CTL1_SELS_MASK | CS_CTL1_DIVHS_MASK) |
           CS_CTL1_SELM__HFXTCLK | CS_CTL1_SELS__HFXTCLK;
    CS->CTL1 |= CS_CTL1_SELS_5             // Sets HSM and SM Clk to HXFT
                | CS_CTL1_DIVS_1;           // 48M/2 = 24Mhz for SM Clk
    CS->KEY = 0;                            // Lock CS module from unintended accesses
   }

   void error(void)
   // This function is from the above TI sample code
   {
       volatile uint32_t i;

       while (1)
       {
           for(i = ERROR_MAX; i> 0; i--);          // Blink LED forever
       }
   }
