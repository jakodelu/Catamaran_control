#include "utilities.h"

int extract_integer(const char* str) {
    int i = 0, number = 0, sign = 1;
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        sign = 1;
        i++;
    }
    while (str[i] != ',' && str[i] != '\0') {
        number *= 10; // multiply the current number by 10;
        number += str[i] - '0'; // converting character to decimalnumber
        i++;
    }
    return sign*number;
}

int ValueConstrained(int value, int min, int max) {
    if (value < min)
        value = min;
    if (value > max)
        value = max;

    return value;
}


