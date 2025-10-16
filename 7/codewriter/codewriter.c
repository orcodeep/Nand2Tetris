#include "codewriter.h"

size_t arith_label_count = 0;

FILE* codewriter_construct(char* inputname)
{
    uint16_t len = strlen(inputname); // give number of non null characters
    char* ext = strrchr(inputname, '.'); // will return pointer to the '.'
    if (len < 4 || ext == NULL || strcmp(ext, ".vm") != 0) // order of '||' matters here else segmentation fault
    {
        printf("Incorrect input file\nUsage:- ./assembler filename.vm");
        exit(1);
    }
    if (ext) // i.e ext != NULL
        *ext = '\0'; // **put a '\0' at the place of the '.' in the inputname
    
    char* outputfile = malloc(strlen(inputname) + strlen(".asm") + 1);
    strcpy(outputfile, inputname);
    strcat(outputfile, ".asm");

    FILE* output = fopen(outputfile, "w");
    if (output == NULL) {exit(1);}

    return output;
}

void codewriter_writeArithmetic(FILE* file, char* arg1) 
{
    arith_label_count++;

    if (strcmp(arg1, "add") == 0)
    {
        fprintf(file, "// add\n");
        fprintf(file, "@SP\n");
        fprintf(file, "AM = M - 1\n");
        fprintf(file, "D = M\n");
        fprintf(file, "A = A - 1\n");
        fprintf(file, "M = M + D\n");  
    }
    else if (strcmp(arg1, "sub") == 0)
    {
        fprintf(file, "// sub\n");
        fprintf(file, "@SP\n");
        fprintf(file, "AM = M - 1\n");
        fprintf(file, "D = M\n");
        fprintf(file, "A = A - 1\n");
        fprintf(file, "M = M - D\n");
    }
    else if (strcmp(arg1, "neg") == 0)
    {
        fprintf(file, "// neg\n");
        fprintf(file, "@SP\n");
        fprintf(file, "A = M - 1\n");
        fprintf(file, "M = -M\n");
    }
    else if (strcmp(arg1, "eq") == 0)
    {
        fprintf(file, "// eq\n");

    }
}



