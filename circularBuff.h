#ifndef CIRCULARBUFF_H
#define	CIRCULARBUFF_H

#include <xc.h>


#define BUFFER_SIZE 50

typedef struct {
char buffer[BUFFER_SIZE];
int readIndex;
int writeIndex;
} CircularBuffer;


void write_buffer(volatile CircularBuffer *cb, char value);
int read_buffer(volatile CircularBuffer *cb, char *value);
int avl_in_buffer( volatile CircularBuffer *cb);


#endif	/* CIRCULARBUFF */

