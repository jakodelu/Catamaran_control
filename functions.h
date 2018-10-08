#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#include "parser.h"
#include "circularBuff.h"
#include "LCD.h"
#include "timer.h"
#include "utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xc.h>

#define TASKS 6

#define MESSAGE_TASK 0
#define TEMPERATURE_TASK 1
#define FEEDBACK_TASK 2
#define D3BLINK 3
#define D4BLINK 4
#define TIMEOUT_TASK 5

#define NORMAL (0)
#define TIMEOUT (1)
#define SAFE (2)
/*----------------------STRUCTURES---------------------------------------------*/
typedef struct{
    int n; //numero di periodi passati
    int period; //numero di periodi dopo il quale si ripete il task
    int activated;
}heartbeat;
/*-----------------------INTERRUPTS--------------------------------------------*/
void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt(void);
void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt();
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt();
/*-----------------------FUNCTIONS---------------------------------------------*/
void interpreter();
void initialize();
void scheduler();
void generatorPWM(int n1Value, int n2Value);
void temperature();
void feedback();
void timeout();
void State(char switchState);
void D3Blink();
void D4Blink();

/*--------------------GLOBAL VARIABLES-----------------------------------------*/
// parser initialization
extern parser_state pstate;
//scheduler initialization
extern heartbeat task[TASKS];
//circular buffer init
extern volatile CircularBuffer cb;
//NEW_MESSAGE alert
extern int messageReceived;
//min, max allowed
extern int minAllowed;
extern int maxAllowed;
extern int minAllowedNew;
extern int maxAllowedNew;
//RPMs motors
extern int n1Value;
extern int n2Value;
//state variable
extern char state;
//temperature values
extern int ADValue;
extern int x;
//string to transmitt
extern char fbk1[23];
extern char fbk2[12];


#endif	/* FUNCTIONS_H */

