#include "parser.h"

static int buffsize = 25;
static char* buff;

static char* makebuffer(char* prevbuffer, int buffsize);

static char* makebuffer(char* prevbuffer, int buffsize)
{
    if (prevbuffer == NULL)
    {
        char* newbuffer = malloc(buffsize); // make initial buffer
        if (newbuffer == NULL) {exit(1);} 
        return newbuffer;
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
        int ch;
        while (len > 0 && buff[len - 1] != '\n' && (ch = fgetc(fileptr)) != EOF)
        {
            buffsize += 5;
            buff = makebuffer(buff, buffsize); 
            ungetc(ch, fileptr);
            if (fgets(buff + len, buffsize - len, fileptr) == NULL)
                break;
            
            len = strlen(buff);
        }

        return true;
    } 

    else 
        return false;
}