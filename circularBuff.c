#include "circularBuff.h"


void write_buffer(volatile CircularBuffer *cb, char value) {
    cb->buffer[cb->writeIndex] = value;
    cb->writeIndex++;
    if (cb->writeIndex == BUFFER_SIZE)
        cb->writeIndex = 0;
}

int read_buffer(volatile CircularBuffer *cb, char *value) {
    if (cb->readIndex == cb->writeIndex)
        return 0;
    *value = cb->buffer[cb->readIndex];
    cb->readIndex++;
    if (cb->readIndex == BUFFER_SIZE)
        cb->readIndex = 0;
    return 1;
}

int avl_in_buffer(volatile CircularBuffer *cb) {
    IEC1bits.U2RXIE = 0;
    int wri = cb->writeIndex;
    int rdi = cb->readIndex;
    IEC1bits.U2RXIE = 1;

    if (wri >= rdi) {
        int a = wri - rdi;
        return a;
    } else {
        return wri - rdi + BUFFER_SIZE;
    }
}



