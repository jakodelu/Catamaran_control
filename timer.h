#ifndef TIMER_H
#define	TIMER_H

#include <xc.h>


void tmr1_setup_ms(int ms);
void tmr1_wait_period();
void choose_prescaler(int ms, int*tckps, int* pr);

#endif	/* TIMER_H */

