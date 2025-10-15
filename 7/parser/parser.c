#include "parser.h"

static int buffsize = 25;
static char* buff;

static char* makebuffer(char* prevbuffer, int buffsize);

static char* makebuffer(char* prevbuffer, int buffsize)
{
    if (prevbuffer == NULL)
    {
        char* buff = malloc(buffsize); // make initial buffer
        if (buff == NULL) {exit(1);} 
        return buff;
    }
    else 
    {
        char* newbuffer = realloc(prevbuffer, buffsize);
        if (newbuffer == NULL)
        {
            free(prevbuffer);
            exit(1);
        }

        return newbuffer;
    }
}

FILE* parser_construct(char* filename)
{
    FILE* fileptr = fopen(filename, "r");
    if (fileptr == NULL) 
    {
        printf("File not found\n");
        exit(1);
    }

    return fileptr;
}

bool parser_hasMoreCommands(FILE* fileptr)
{
    long pos = ftell(fileptr);
    if (pos == 0)
    {
        buffsize = 25;
        buff = makebuffer(NULL, buffsize); // initial buffer 
    }

    if (fgets(buff, buffsize, fileptr) != NULL) // parameters: buffer to store to, available size to store to im buffer, FILE*
    {
        int len = strlen(buff); 
        while (buff[len - 1] != '\n' && !feof(fileptr))
        {
            buffsize += 5;
            buff = makebuffer(buff, buffsize); 
            if (fgets(buff + len, buffsize - len, fileptr) == NULL) // will also copy the rest of the line into the regrown buffer. 
            //if fgets fails (returns NULL), it just means no new data was read
                break;
            // if fgets had read up to the last char of the last line and didn't encounter the EOF because it stopped at the last char
            // (also no '\n' on this line) when this time we call fgets, if it encounters the EOF('\0') break and buff contains the prev data
            
            len = strlen(buff);
        }

        return true;
    } 

    else 
        return false;
}