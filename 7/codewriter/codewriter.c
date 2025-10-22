#include "codewriter.h"

size_t arith_label_count = 0;

static char* getfilename(char* file)
{
    // ** acually *ext = '\0' works as long as filename is a mutable character array
    // so to be safe make a duplicate array. as it wont work if file is a 'string literal' like:-
    // getfilename("file.vm");
    char* dup = strdup(file); // strdup'ed string must also be freed later
    uint16_t len = strlen(dup); // give number of non null characters
    char* ext = strrchr(dup, '.'); // will return pointer to the '.'
    if (len < 4 || ext == NULL || strcmp(ext, ".vm") != 0) // order of '||' matters here else segmentation fault
    {
        printf("Incorrect input file\nUsage:- ./assembler filename.vm");
        exit(1);
    }
    if (ext) // i.e ext != NULL
        *ext = '\0';

    return dup;
}

FILE* codewriter_construct(char* file)
{
    char* inputname = getfilename(file);
    
    char* outputfile = malloc(strlen(inputname) + strlen(".asm") + 1);
    strcpy(outputfile, inputname);
    strcat(outputfile, ".asm");

    FILE* output = fopen(outputfile, "w");
    if (output == NULL) {exit(1);}

    // Clean up
    free(inputname);
    free(outputfile);

    return output;
}

void codewriter_writeArithmetic(FILE* file, char* arg1) 
{
    arith_label_count++;

    if (strcmp(arg1, "add") == 0 || strcmp(arg1, "sub") == 0)
    {
        if (strcmp(arg1, "add") == 0)
            fprintf(file, "// add\n");
        else
            fprintf(file, "// sub\n");
        fprintf(file, "@SP\n");
        fprintf(file, "AM=M-1\n");
        fprintf(file, "D=M\n");
        fprintf(file, "A=A-1\n");
        if (strcmp(arg1, "add") == 0)
            fprintf(file, "M=M+D\n");
        else
            fprintf(file, "M=M-D\n");
    }
    else if (strcmp(arg1, "neg") == 0 || strcmp(arg1, "not") == 0)
    {
        if (strcmp(arg1, "neg") == 0)
            fprintf(file, "// neg\n");
        else
            fprintf(file, "// not\n");
        fprintf(file, "@SP\n");
        fprintf(file, "A=M-1\n");
        if (strcmp(arg1, "neg") == 0)
            fprintf(file, "M=-M\n");
        else
            fprintf(file, "M=!M\n");
    }
    else if (strcmp(arg1, "eq") == 0 || strcmp(arg1, "gt") == 0 || strcmp(arg1, "lt") == 0)
    {
        if (strcmp(arg1, "eq") == 0)
            fprintf(file, "// eq\n");
        else if (strcmp(arg1, "gt") == 0)
            fprintf(file, "// gt\n");
        else
            fprintf(file, "// lt\n");

        fprintf(file, "@SP\n");
        fprintf(file, "AM=M-1\n");
        fprintf(file, "D=M\n");
        fprintf(file, "A=A-1\n");
        fprintf(file, "D=M-D\n");
        fprintf(file, "@EQ_TRUE_%zu\n", arith_label_count);
        
        if (strcmp(arg1, "eq") == 0)
            fprintf(file, "D;JEQ\n");
        else if (strcmp(arg1, "gt") == 0)
            fprintf(file, "D;JGT\n");
        else
            fprintf(file, "D;JLT\n");

        fprintf(file, "@SP\n");
        fprintf(file, "A=M-1\n");
        fprintf(file, "M=0\n");
        fprintf(file, "@END_%zu\n", arith_label_count);
        fprintf(file, "0;JMP\n");
        fprintf(file, "(EQ_TRUE_%zu)\n", arith_label_count);
        fprintf(file, "@SP\n");
        fprintf(file, "A=M-1\n");
        fprintf(file, "M=-1\n");
        fprintf(file, "(END_%zu)\n", arith_label_count);
    }
    else if (strcmp(arg1, "and") == 0 || strcmp(arg1, "or") == 0)
    {
        if (strcmp(arg1, "and") == 0)
            fprintf(file, "// and\n");
        else
            fprintf(file, "// or\n");

        fprintf(file, "@SP\n");
        fprintf(file, "AM=M-1\n");
        fprintf(file, "D=M\n");
        fprintf(file, "A=A-1\n");

        if (strcmp(arg1, "and") == 0)
            fprintf(file, "M=M&D\n");
        else
            fprintf(file, "M=M|D\n"); 
    }
    else
    {
        printf("incorrect arithmetic vm command\n");
        exit(1);
    }
}

void codewriter_writePushPop(FILE* fp, char* file, commandType command, char* arg1, char* arg2)
{
    char* pathname = getfilename(file); // will be used in static segment. also this must be freed
    char* lastbackslash = strrchr(pathname, '/');
    char* filename;
    if (lastbackslash != NULL)
        filename = strdup(lastbackslash + 1);
    else
    {
        // No slash found, copy whole pathname
        filename = strdup(pathname);
    }
    free(pathname);
    
    if (command == C_PUSH)
    {
        if(strcmp(arg1, "local") == 0)
        {
            fprintf(fp, "// push local %s\n", arg2);
            fprintf(fp, "@LCL\n");
        }
        else if (strcmp(arg1, "argument") == 0)
        {
            fprintf(fp, "// push argument %s\n", arg2);
            fprintf(fp, "@ARG\n");
        }
        else if (strcmp(arg1, "this") == 0)
        {
            fprintf(fp, "// push this %s\n", arg2);
            fprintf(fp, "@THIS\n");
        }
        else if (strcmp(arg1, "that") == 0)
        {
            fprintf(fp, "// push that %s\n", arg2);
            fprintf(fp, "@THAT\n");
        }
        else if (strcmp(arg1, "constant") == 0)
        {
            fprintf(fp, "// push constant %s\n", arg2);
            fprintf(fp, "@%s\n", arg2);
        }
        else if (strcmp(arg1, "temp") == 0)
        {
            fprintf(fp, "// push temp %s\n", arg2);
            int tempAddr = 5 + atoi(arg2);
            fprintf(fp, "@%d\n", tempAddr);
            fprintf(fp, "D=M\n");

            // Then jump directly to push to stack
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M\n");
            fprintf(fp, "M=D\n");
            fprintf(fp, "@SP\n");
            fprintf(fp, "M=M+1\n");

            free(filename);
            return;
        }
        else if (strcmp(arg1, "pointer") == 0)
        {
            fprintf(fp, "// push pointer %s\n", arg2);
            if (strcmp(arg2, "0") == 0)
                fprintf(fp, "@THIS\n");
            else if (strcmp(arg2, "1") == 0)
                fprintf(fp, "@THAT\n");
        }
        else if (strcmp(arg1, "static") == 0)
        {
            fprintf(fp, "// push static %s\n", arg2); 
            fprintf(fp, "@%s.%s\n", filename, arg2);

        } 

        if (strcmp(arg1, "constant") == 0)
            fprintf(fp, "D=A\n");
        else
            fprintf(fp, "D=M\n");

        if (strcmp(arg1, "constant") == 0 || strcmp(arg1, "pointer") == 0 || strcmp(arg1, "static") == 0)
        {
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M\n");
            fprintf(fp, "M=D\n");
            fprintf(fp, "@SP\n");
            fprintf(fp, "M=M+1\n");
            return;
        }

        // For local, argument, this, that, temp: add offset and push
        fprintf(fp, "@%s\n", arg2);
        fprintf(fp, "A=D+A\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "M=M+1\n");
    }

    else // i.e if C_POP
    {
        if(strcmp(arg1, "local") == 0)
        {
            fprintf(fp, "// pop local %s\n", arg2);
            fprintf(fp, "@LCL\n");
        }
        else if (strcmp(arg1, "argument") == 0)
        {
            fprintf(fp, "// pop argument %s\n", arg2);
            fprintf(fp, "@ARG\n");
        }
        else if (strcmp(arg1, "this") == 0)
        {
            fprintf(fp, "// pop this %s\n", arg2);
            fprintf(fp, "@THIS\n");
        }
        else if (strcmp(arg1, "that") == 0)
        {
            fprintf(fp, "// pop that %s\n", arg2);
            fprintf(fp, "@THAT\n");
        }
        else if (strcmp(arg1, "temp") == 0)
        {
            fprintf(fp, "// pop temp %s\n", arg2);
            fprintf(fp, "@SP\n");
            fprintf(fp, "AM=M-1\n");
            fprintf(fp, "D=M\n");
            int t = 5;
            int offset = atoi(arg2);
            t += offset;
            fprintf(fp, "@%i\n", t);
            fprintf(fp, "M=D\n");
            return;
        }
        else if (strcmp(arg1, "static") == 0)
        {
            fprintf(fp, "// pop static %s\n", arg2);
            fprintf(fp, "@SP\n");
            fprintf(fp, "AM=M-1\n");
            fprintf(fp, "D=M\n");
            fprintf(fp, "@%s.%s\n", filename, arg2);
            fprintf(fp, "M=D\n");
            return;
        }
        else if (strcmp(arg1, "pointer") == 0)
        {
            fprintf(fp, "// pop pointer %s\n", arg2);
            fprintf(fp, "@SP\n");
            fprintf(fp, "AM=M-1\n");
            fprintf(fp, "D=M\n");
            if (strcmp(arg2, "0") == 0)
                fprintf(fp, "@THIS\n");
            else if (strcmp(arg2, "1") == 0)
                fprintf(fp, "@THAT\n");
            fprintf(fp, "M=D\n");
            return;
        }

        fprintf(fp, "D=M\n");
        fprintf(fp, "@%s\n", arg2);
        fprintf(fp, "D=D+A\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    }

    free(filename);
}

void codewriter_close(FILE* outputfile)
{
    // Optional: write infinite loop (halt)
    fprintf(outputfile, "(END)\n@END\n0;JMP\n");

    fclose(outputfile);
}

