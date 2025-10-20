#include "parser.h"

static int buffsize = 25;
static char* buff; 

static char* makebuffer(char* prevbuffer, int buffsize);
static char* makecopy(char* line);

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

static char* makecopy(char* line)
{
    size_t len = strlen(line);
    char* copy = malloc(len + 1);
    if (copy == NULL) {exit(1);}
    strcpy(copy, line); 

    return copy;
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
    char* copy = makecopy(line); 
    char* firstWord = strtok(copy, " \t\r\n"); // if empty string strtok will return NULL

    // it will return INVALID for comments and empty lines 
    if (firstWord == NULL || firstWord[0] == '/') 
    {
        free(copy);
        return C_INVALID;
    }

    else 
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
}

char* arg1(char* line, commandType command)
{
    // the function returns NULL for empty or comment lines or if RETURN command
    if (command == C_INVALID || command == C_RETURN)
        return NULL;

    char* copy = makecopy(line);
    char* firstWord = strtok(copy, " \t\r\n");
    if (command == C_ARITHMETIC)
    {
        char* result = strdup(firstWord); // ** cant free(copy) then return firstWord as firstWord will be a dangling pointer
        free(copy);
        return result;
    }

    else 
    {
        // first argument is secondWord in the line
        char* secondWord = strtok(NULL, " \t\r\n"); // u pass NULL to strtok to continue from where it left off in original string
        char* result = NULL;
        result = strdup(secondWord);

        free(copy);
        return result;
    }
}

char* arg2(char* line, commandType command)
{
    // Only process commands that have a third argument (e.g., push, pop, function, call)
    if (command == C_PUSH || command == C_POP || command == C_FUNCTION || command == C_CALL)
    {
        char* copy = makecopy(line); 
        strtok(copy, " \t\r\n");
        strtok(NULL, " \t\r\n");
        char* thirdWord = strtok(NULL, " \t\r\n"); 
        char* result = NULL;
        result = strdup(thirdWord);

        free(copy);
        return result;
    }

    // for any command other than those in the if block
    return NULL;
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
        commandType type = parser_commandType(line);
        char* ar1 = arg1(line, type);
        char* ar2 = arg2(line, type); 

        if (ar1 != NULL && ar2 == NULL)
            printf("arg1 = %s\n", ar1);

        else if (ar2 != NULL && ar2 != NULL)
            printf("arg1 = %s, arg2 = %s\n", ar1, ar2);
    }
    
    fclose(file);
}
*/