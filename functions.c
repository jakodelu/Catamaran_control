#include "functions.h"

/*--------------------GLOBAL VARIABLES-----------------------------------------*/

// parser initialization
parser_state pstate;

//scheduler initialization
heartbeat task[TASKS];

//circular buffer init
volatile CircularBuffer cb;

//NEW_MESSAGE alert
int messageReceived;

//min, max allowed
int minAllowed = -8000;
int maxAllowed = 8000;
int minAllowedNew;
int maxAllowedNew;


//RPMs motors
int n1Value;
int n2Value;

//state variable
char state;

//temperature values
int ADValue = 0;
int x;

//string to transmitt
char fbk1[23];
char fbk2[12];

/*--------------------------INTERRUPTS-----------------------------------------*/

void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt(void) {

    //UART Interrupt 
    IFS1bits.U2RXIF = 0; //Clear interrupt flag
    write_buffer(&cb, U2RXREG); //write char from the UART in the buffer    
}

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    //interrupt of button S5
    IFS0bits.INT0IF = 0; // set flag on S5 BUTTON to 0
    IEC0bits.INT0IE = 0; // disable interrupt of S5 BUTTON
    State(SAFE);
    IEC0bits.INT0IE = 1; // Enable interrupt on S5

}

void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    //interrupt of button S6
    IFS1bits.INT1IF = 0; // set flag on S6 BUTTON to 0
    IEC1bits.INT1IE = 0; // disable interrupt of S6 BUTTON
    State(SAFE);
    IEC1bits.INT1IE = 1; // Enable interrupt on S6


}

/*------------------------------SETUP------------------------------------------*/
void initialize() {
    /*--------------------------VARIABLES----------------------------------------*/
    minAllowedNew = -8000;
    maxAllowedNew = 8000;
    cb.writeIndex = 0;
    cb.readIndex = 0;
    /*------------------------------INIT LED------------------------------------*/
    TRISBbits.TRISB0 = 0; //set D3 LED as output
    TRISBbits.TRISB1 = 0; //set D4 LED as output
    LATBbits.LATB0 = 0; //turn off the LED D3
    LATBbits.LATB1 = 0; //turn off the LED D4

    /*----------------------------INIT UART---------------------------------------*/
    IEC1bits.U2RXIE = 1; // enable interrupt RX
    U2STAbits.URXISEL = 0; // 1 char received
    U2BRG = 11; //Baude Rate 9600
    U2MODEbits.UARTEN = 1; //enable UART
    U2STAbits.UTXEN = 1; //enable communication

    /*------------------------------INIT PWM------------------------------------*/
    PTCONbits.PTMOD = 0; //set free running mode
    PTCONbits.PTCKPS = 0; //Prescalar 1
    PTPER = 1842; //1843200/(1000)-1 check 10 ms period
    PWMCON1bits.PEN1H = 1; //Enabling pin RE1
    PWMCON1bits.PEN2H = 1; //Enabling pin RE3
    PTCONbits.PTEN = 1; //PWM on


    /*------------------------------LCD setup---------------------------------------*/
    /*SPI1CONbits.MSTEN = 1; // master mode
    SPI1CONbits.MODE16 = 0; // 8bit mode
    SPI1CONbits.PPRE = 3; // 1:1 primary prescaler
    SPI1CONbits.SPRE = 3; // 5:1 secondary prescaler
    SPI1STATbits.SPIEN = 1; // enable SPI    
    tmr1_setup_ms(1000);
    tmr1_wait_period();*/

    /*---------------------------INIT INTERRUPT--------------------------------*/
    IFS0bits.INT0IF = 0; // set flag on S5 BUTTON to 0
    IEC0bits.INT0IE = 1; // enable interrupt on S5 BUTTON
    IFS1bits.INT1IF = 0; // set flag on S6 BUTTON to 0
    IEC1bits.INT1IE = 1; // enable interrupt on S6 BUTTON

    /*---------------------------INIT ADC-------------------------------------*/


    ADPCFGbits.PCFG3 = 0; // Analog input pin AN3 in Analog mode , port read input disabled, A/D samples pin voltage
    ADCON2bits.VCFG = 0; //voltage references for A/D conversions
    ADCON3bits.ADCS = 63; //tad =32*Tcy
    ADCON2bits.CHPS = 0; //Sample CH0, Convert CH0 
    ADCON1bits.ASAM = 0; // the A/D to manually ally begin sampling a channel
    //whenever a conversion is not active on that channel
    ADCHSbits.CH0SA = 3; //select the analog input  AN3 for the positive input of channel 0
    ADCON1bits.SSRC = 7; // the conversion trigger is under A/D clock control
    ADCON3bits.SAMC = 31; //value specifies the sampling time
    ADCON1bits.FORM = 0; //  output format integer
    ADCON1bits.ADON = 1; //Turn on A/D

    /*------------------------------INIT TASK-----------------------------------*/
    // Starting with counter 0
    int i = 0;
    for (; i < TASKS; i++)
        task[i].n = 0;

    //defining task periods
    task[MESSAGE_TASK].period = 1;
    task[TEMPERATURE_TASK].period = 100;
    task[FEEDBACK_TASK].period = 200;
    task[D3BLINK].period = 50;
    task[D4BLINK].period = 50;
    task[TIMEOUT_TASK].period = 500;
    //init state 
    State(NORMAL);

    /*---------------------------INIT PARSER------------------------------------*/
    pstate.state = STATE_DOLLAR;
    pstate.index_type = 0;
    pstate.index_payload = 0;

}

/*--------------------------FUNCTIONS------------------------------------------*/
void State(char switchState) {
    switch (switchState) {
        case NORMAL:
            state = NORMAL;
            LATBbits.LATB1 = 0;
            task[MESSAGE_TASK].activated = 1;
            task[TEMPERATURE_TASK].activated = 1;
            task[FEEDBACK_TASK].activated = 1;
            task[D3BLINK].activated = 1;
            task[D4BLINK].activated = 0;
            task[TIMEOUT_TASK].activated = 1;
            break;

        case TIMEOUT:
            state = TIMEOUT;
            n1Value = 0;
            n2Value = 0;
            generatorPWM(n1Value, n2Value);
            task[MESSAGE_TASK].activated = 1;
            task[TEMPERATURE_TASK].activated = 1;
            task[FEEDBACK_TASK].activated = 1;
            task[D3BLINK].activated = 1;
            task[D4BLINK].activated = 1;
            task[TIMEOUT_TASK].activated = 0;
            break;

        case SAFE:
            state = SAFE;
            n1Value = 0;
            n2Value = 0;
            LATBbits.LATB1 = 0;
            generatorPWM(n1Value, n2Value);
            task[MESSAGE_TASK].activated = 1;
            task[TEMPERATURE_TASK].activated = 1;
            task[FEEDBACK_TASK].activated = 1;
            task[D3BLINK].activated = 1;
            task[D4BLINK].activated = 0;
            task[TIMEOUT_TASK].activated = 0;
            break;

    }
}

void interpreter() {
    char readValue;
    // while there are new chars. . .
    while (read_buffer(&cb, &readValue)) {

        //parse each char
        messageReceived = parse_byte(&pstate, readValue);


        if (messageReceived == NEW_MESSAGE) {
            messageReceived = NO_MESSAGE;

            //RPMs for the motors
            if (strcmp(pstate.msg_type, "HLREF") == 0 && state != SAFE) {

                char *n1;
                char *n2;

                //get the first word
                n1 = strtok(pstate.msg_payload, ",");
                //get the second word
                n2 = strtok(NULL, ",");

                //constrain the values and extract integer from char
                n1Value = ValueConstrained(extract_integer(n1), minAllowedNew, maxAllowedNew);
                n2Value = ValueConstrained(extract_integer(n2), minAllowedNew, maxAllowedNew);

                //generate PWM
                generatorPWM(n1Value, n2Value);

                //set the timeout counter period to zero
                task[TIMEOUT_TASK].n = 0;
                State(NORMAL);

            }//Saturate min max RPMs allowed
            else if (strcmp(pstate.msg_type, "HLSAT") == 0) {

                char *min;
                char *max;
                //get the first word
                min = strtok(pstate.msg_payload, ",");
                //get the second word
                max = strtok(NULL, ",");
                //convert char to integer
                minAllowedNew = extract_integer(min);
                maxAllowedNew = extract_integer(max);
                //constrain the max and min allowed
                minAllowedNew = ValueConstrained(minAllowedNew, minAllowed, maxAllowed);
                maxAllowedNew = ValueConstrained(maxAllowedNew, minAllowed, maxAllowed);
                //constrain the values
                n1Value = ValueConstrained(n1Value, minAllowedNew, maxAllowedNew);
                n2Value = ValueConstrained(n2Value, minAllowedNew, maxAllowedNew);
                //generate PWM
                generatorPWM(n1Value, n2Value);

            }//Enable the firmware to send references to the motors
            else if (strcmp(pstate.msg_type, "HLENA") == 0 && state == SAFE) {
                State(NORMAL);
            }
        }
    }
}

void generatorPWM(int n1Value, int n2Value) {
    PDC1 = (int) n1Value * (PTPER / 10000.0f) + PTPER; //tutta l'operazione diventa tra float e poi cast ad int
    PDC2 = (int) n2Value * (PTPER / 10000.0f) + PTPER;
}

void temperature() {
    ADCON1bits.SAMP = 1;
    while (!ADCON1bits.DONE); //wait for the conversion
    if (ADValue != ADCBUF0) {
        ADValue = ADCBUF0;
        x = (ADValue - 102) / 2;

    }
}

void feedback() {

    int i;

    sprintf(fbk1, "$MCFBK,%d,%d,%d*\n", n1Value, n2Value, state);
    for (i = 0; i < strlen(fbk1); i++) {
        while (!U2STAbits.TRMT);
        U2TXREG = fbk1[i];
    }
    sprintf(fbk2, "$MCTEM,%d*\n", x);
    for (i = 0; i < strlen(fbk2); i++) {
        while (!U2STAbits.TRMT);
        U2TXREG = fbk2[i];
    }
}

void D3Blink() {
    LATBbits.LATB0 = !LATBbits.LATB0;
}

void D4Blink() {
    LATBbits.LATB1 = !LATBbits.LATB1;

}

void timeout() {
    if (state == NORMAL) { //After 5s Timeout mode
        State(TIMEOUT);
    }
}

void scheduler() {

    int i;
    for (i = 0; i < TASKS; i++) {
        task[i].n++;
        if (task[i].n == task[i].period) {
            task[i].n = 0;
            if (task[i].activated) {
                switch (i) {
                    case 0:
                        interpreter(); //each period 10ms
                        break;
                    case 1:
                        temperature(); //100 period 1s
                        break;
                    case 2:
                        feedback(); //200 period 2s
                        break;
                    case 3:
                        D3Blink(); //50 period 500ms
                        break;
                    case 4:
                        D4Blink(); //50 period 500ms
                        break;
                    case 5:
                        timeout(); 
                }
            }
        }
    }
}


