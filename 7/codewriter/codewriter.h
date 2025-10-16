#ifndef CODEWRITER_H
#define CODEWRITER_H 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern size_t arith_label_count;

FILE* codewriter_construct(char* inputname);
void codewriter_writeArithmetic(FILE* file, char* arg1);



#endif