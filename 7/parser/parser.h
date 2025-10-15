#ifndef PARSER_H
#define PARSER_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


FILE* parser_construct(char* filename);
bool parser_hasMoreCommands(FILE* fileptr);



#endif