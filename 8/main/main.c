#include "main.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Wrong usage\nuse: ./vmtranslator filename.vm\n");
        return 1;
    }

    FILE* file = parser_construct(argv[1]);
    FILE* outputfile = codewriter_construct(argv[1]);
    bool hasMore;
    while ((hasMore = parser_hasMoreCommands(file))) // each iteration we get the newline
    {
        char* line = parser_advance(hasMore); // although here hasMore always = true
        commandType type = parser_commandType(line);
        char* ar1 = arg1(line, type);
        char* ar2 = arg2(line, type); 

        if (ar1 != NULL && ar2 == NULL && type == C_ARITHMETIC) // if arithmetic vm command
        {
            codewriter_writeArithmetic(outputfile, ar1);
        }

        else if (ar1 != NULL && ar2 != NULL && (type == C_PUSH || type == C_POP))
        {
            codewriter_writePushPop(outputfile, argv[1], type, ar1, ar2);
        }
        
    }
    codewriter_close(outputfile);
    fclose(file);
}

// check if argv[1] is a file or directory if directory then parse .vm files one after one
// it will return the filepath/filename.ext of the file that parser has to parse 

char* checkargv1(char* arg) // arg is argv[1]
{
    struct stat path_stat;
    bool isdir = false;
    
    if (stat(arg, &path_stat)) // stat returns 0 for success
    {
        printf("stat failed");
        exit(1);
    }

    if (S_ISREG(path_stat.st_mode)) 
        isdir = false;

    else if (S_ISDIR(path_stat.st_mode)) 
        isdir = true;
    else
    {
        printf("\n%s is not a valid path to a file or directory\n", arg);
        exit(1);
    }

    if (!isdir)
    {
        checkext(arg); // will stop program if the file is not a ".vm" file (defined in codewriter.h)
        return arg;
    }
    else 
    {
        // now recursively return the .vm filepaths one by one
    }

}


