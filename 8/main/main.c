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

// check if argv[1] is a file or directory if directory then return NULL
char* checkargv1(char* arg) // arg is argv[1] on first call in main
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

    if (!isdir) // i.e is filepath
    {
        char* ext = checkext(arg); 
        if (ext == NULL) // will stop program if the file is not a ".vm" file (defined in codewriter.h)
        {
            printf("Incorrect file type\nFilepath doesnt point to a *.vm file");
            exit(1);
        }
        return arg;
    }
    else 
    {
        // its a dir so need to recursively return the .vm filepaths one by one
        return NULL;
    }
}

DIR* opendirectory(char* dirpath)
{
    DIR* dir = opendir(dirpath); // should be closed later with closedir
    if (!dir)
    {
        printf("couldnt open directory %s\n", dirpath);
        exit(1);
    }
    return dir;
}

// get the .vm file names one by one in the directory
char* getvmfn(DIR* dir, char* dirpath)
{
    struct dirent* entry; // get the data about the file or directory 

    // getvmfn can be used in a while loop in main to check if end of directory reached
    while((entry = readdir(dir)) != NULL)
    {
        char* name = entry->d_name;
        // Skip current and parent directory entries
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;

        // Only proceed if it has a valid .vm extension
        if (!checkext(name)) // i.e checkext(name) != NULL
            continue;

        size_t required = strlen(entry->d_name) + strlen(dirpath) + 2; // +2 for a '/' before the filename and a '\0' at the end 
        if (required > filepathBuffSize)
        {
            filepathBuffSize = required;
        }
        filepathBuff = makebuffer(filepathBuff, filepathBuffSize); 

        snprintf(filepathBuff, filepathBuffSize, "%s/%s", dirpath, entry->d_name);
        return filepathBuff;
    }
    
    /*
    // if you want to differentiate between "end of directory" and "error"
    if (errno != 0) {
        perror("readdir");
        // handle error
    }
    */

    // end of directory
    return NULL;
}




