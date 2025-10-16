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
    return false;
}

char* parser_advance(bool hasMore)
{
    if (hasMore)
        return buff; 

    return NULL;
}

commandType parser_commandType(char* line)
{
    // **make a copy on whcih strtok will operate because strtok modifies input string
    size_t len = strlen(line);
    char* copy = malloc(len + 1);
    if (copy == NULL) {exit(1);}
    strcpy(copy, line); 

    char* firstWord = strtok(copy, " \t\n"); // if empty string strtok will return NULL
    if (firstWord != NULL && firstWord[0] != '/') 
    {
        commandType type;
        if(strcmp(firstWord, "push") == 0)
            type = C_PUSH;
        else if (strcmp(firstWord, "pop") == 0)
            type = C_POP;
        else if (strcmp(firstWord, "label") == 0)
            type = C_LABEL;
        else if (strcmp(firstWord, "goto") == 0)
            type = C_GOTO;
        else if (strcmp(firstWord, "if-goto") == 0)
            type = C_IF;
        else if (strcmp(firstWord, "function") == 0)
            type = C_FUNCTION;
        else if (strcmp(firstWord, "return") == 0)
            type = C_RETURN;
        else if (strcmp(firstWord, "call") == 0)
            type = C_CALL;
        else
            type = C_ARITHMETIC;

        free(copy);
        return type;
    }
    // it will return INVALID for comments and empty lines 
    free(copy);
    return C_INVALID;
}

/*
TEST:-

int main(int argc, char* argv[])
{
    FILE* file = parser_construct(argv[1]);
    bool hasMore;
    while ((hasMore = parser_hasMoreCommands(file))) // each iteration we get the newline
    {
        char* line = parser_advance(hasMore); // although here hasMore always = true
        printf("%s", line);
    }
    
    fclose(file);
}
*/