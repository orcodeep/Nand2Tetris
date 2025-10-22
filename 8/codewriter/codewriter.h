#ifndef CODEWRITER_H
#define CODEWRITER_H 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.h"

FILE* codewriter_construct(char* inputpath, bool isdir);
void codewriter_writeArithmetic(FILE* file, char* arg1);
void codewriter_writePushPop(FILE* fp, char* file, commandType command, char* arg1, char* arg2);
void codewriter_close(FILE* outputfile);
void codewriter_writeInit(int vmfilecount, FILE* fileptr);
void codewriter_writeLabel(FILE* fp, char* arg1);
void codewriter_writeGoto(FILE* fp, char* arg1);
void codewriter_writeIf(FILE* fp, char* arg1);
void codewriter_writeCall(FILE* fp, char* arg1, char* arg2);
void codewriter_writeFunction(FILE* fp, char* arg1, char* arg2);
void codewriter_writeReturn(FILE* fp);

// used only as a validatin function in main.c
char* checkext(char* name);

#endif