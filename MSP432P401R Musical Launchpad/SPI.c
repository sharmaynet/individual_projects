
/**
 * SPI.c
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/30/19
 *
 * Sub functions that:
 * - Initializes SPI Protocol
 * - Output Data by waiting for transmition flag to go high
 * - Outputs a voltage in the 0-3.3V range (4096 values within range)
 * - Outputs:   - 1.5 to SPI Clk
 *              - 1.6 SIMO (Controls & Data Output)
 *              - 5.7 Chip Select
 */

#include "msp.h"
#include "SPI.h"

void SPI_init(){
    //set_DCO(FREQ_24_MHZ);
    //1. set UCSWRTST
    EUSCI_B0-> CTLW0 |= EUSCI_B_CTLW0_SWRST;        // Reset serial state machine
    //2. initialize all eUSCI reg with UCSWRST
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST|          // Stay in reset
                      EUSCI_B_CTLW0_MST|            // Master mode
                      EUSCI_B_CTLW0_SYNC|           // Enable synchronous
                      EUSCI_B_CTLW0_UCSSEL_2|       // Select SMCLK
                      EUSCI_B_CTLW0_CKPL|           // Set clock inactive high
                      EUSCI_B_CTLW0_MSB;            // MSB is received/transmitted first
    //3. configure ports
    EUSCI_B0->BRW = 0x01;                           // Run @ full smclk speed
    P1->DIR |= BIT7;                                // Chip Select = 1.7
    P1->SEL0 |= (BIT5 | BIT6);                      // 1.5 = clk, 1.6 = simo
    P1->SEL1 &= ~(BIT5 | BIT6);
    //4. clear UCSWRST
    EUSCI_B0->CTLW0 &= ~ EUSCI_B_CTLW0_SWRST;
    P1->OUT |= BIT7;                                // Set chip select high
}

void set_outputV(uint16_t data){
    uint8_t loByte = data & 0xFF;                   // Lower 8 Byte
    uint8_t hiByte = (data >> 8) & 0x0F;            // Higher 8 Byte
    hiByte |= (BIT4 | BIT5);                        // Control Bits of the data signal
    P1->OUT &= ~BIT7;                               // Chip select low to start
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = hiByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = loByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));    // Wait for transition to end
    P1->OUT |= BIT7;                                // Set CS to high
}

