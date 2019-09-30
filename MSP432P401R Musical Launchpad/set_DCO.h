/**
 * set_DCO.h
 * Created by Sharmayne Tanedo, Gerome Cacho
 * 4/10/19
 * Header file that:
 * - Defines frequencies used in set_DCO.C
 *   equivalent as those set in TI provided
 *   header
 */

#ifndef SET_DCO_H_
#define SET_DCO_H_

#define FREQ_1_5_MHZ CS_CTL0_DCORSEL_0
#define FREQ_3_MHZ CS_CTL0_DCORSEL_1
#define FREQ_6_MHZ CS_CTL0_DCORSEL_2
#define FREQ_12_MHZ CS_CTL0_DCORSEL_3
#define FREQ_24_MHZ CS_CTL0_DCORSEL_4
#define FREQ_48_MHZ CS_CTL0_DCORSEL_5
#define ERROR_MAX 20000

void set_DCO(int freq);
void set_48MHZ();
void error(void);

#endif /* SET_DCO_H_ */
