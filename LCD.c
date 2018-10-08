#include "LCD.h"

int writeLCD(char * input, int autoWrap) {
    int length = strlen(input);
    int i = 0;
    int line = 1;
    while (i < length && i <= 32 && input[i] != '\0') {
        if (i == 15 && autoWrap) {
            SPI1BUF = 0xC0;
            while (SPI1STATbits.SPITBF);
            SPI1BUF = input[i];
            while (SPI1STATbits.SPITBF);
        } else if (input[i] == '\n') {
            if (line != 1)
                break;
            line = 2;
            SPI1BUF = 0xC0;
            while (SPI1STATbits.SPITBF);
        } else {
            SPI1BUF = input[i];
            while (SPI1STATbits.SPITBF);
        }
        i++;
    }

    return i;
}

void clearLCD() { // Clear LCD
    int i;

    SPI1BUF = 0x80;
    while (SPI1STATbits.SPITBF);

    for (i = 0; i < 16; i++) {
        SPI1BUF = ' ';
        while (SPI1STATbits.SPITBF);
    }
    SPI1BUF = 0xC0;
    while (SPI1STATbits.SPITBF);
    for (i = 0; i < 16; i++) {
        SPI1BUF = ' ';
        while (SPI1STATbits.SPITBF);
    }

    SPI1BUF = 0x80;
    while (SPI1STATbits.SPITBF);
}