#include "timer.h"


void choose_prescaler(int ms, int*tckps, int* pr) {
    long ticks = 2000L * ms;
    if (ticks <= 65535) {
        *tckps = 0;
        *pr = ticks;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        *tckps = 1;
        *pr = ticks;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        *tckps = 2;
        *pr = ticks;
        return;
    }
    ticks = ticks / 4;
    *tckps = 3;
    *pr = ticks;
    return;
}

void tmr1_setup_ms(int ms) {
    T1CONbits.TON = 0;
    TMR1 = 0; // timer is initialized and starts 
    int tckps, pr;
    choose_prescaler(ms, &tckps, &pr);
    T1CONbits.TCKPS = tckps; // controls the value of the prescalar
    PR1 = pr; //16 bits register where we write the value the timer should reach
    T1CONbits.TON = 1; // sets the timer
    return;
}

void tmr1_wait_period() {
    while (!IFS0bits.T1IF);
    IFS0bits.T1IF = 0;

}

