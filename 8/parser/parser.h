#ifndef PARSER_H
#define PARSER_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_INVALID
} commandType;

FILE* parser_construct(char* filename);
bool parser_hasMoreCommands(FILE* fileptr);
char* parser_advance(bool hasMore);
commandType parser_commandType(char* line);
char* arg1(char* file, commandType command);
char* arg2(char* line, commandType command);


// use in main.c too to make a buffer to hold filepath
char* makebuffer(char* prevbuffer, int buffsize);

#endif