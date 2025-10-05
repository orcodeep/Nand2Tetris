#include <stdio.h>


int fileopen(char* filename);



// first open the .asm file 
int fileopen(char* filename)
{
    FILE* ptr = fopen(filename, "r");
}
