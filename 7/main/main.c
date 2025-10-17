#include "../parser/parser.h"
#include "../codewriter/codewriter.h"

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
        
        /*
        if (ar1 != NULL && ar2 == NULL)
            printf("arg1 = %s\n", ar1);

        else if (ar1 != NULL && ar2 != NULL)
            printf("arg1 = %s, arg2 = %s\n", ar1, ar2);
        */

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