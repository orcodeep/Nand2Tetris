#include "main.h"

static int getvmcount(const char* dirpath);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Wrong usage\nuse: ./vmtranslator filename.vm\n");
        return 1;
    }

    bool isdir = checkargv1(argv[1]);

    if (isdir) // i.e a directory
    {
        //**argv[1] is the dirpath then
        FILE* outputfile = codewriter_construct(argv[1], isdir);
        int vmfilecount = getvmcount(argv[1]);
        if (vmfilecount == 0) 
        {
            printf("Directory doesnt contain any .vm files\n");
            return 1;
        }
        codewriter_writeInit(vmfilecount, outputfile);

        DIR* dir = opendirectory(argv[1]);
        char* filepath;
        while((filepath = getvmfn(dir, argv[1])) != NULL)
        {
            FILE* file = parser_construct(filepath);
            bool hasmore;
            while((hasmore = parser_hasMoreCommands(file)))
            {
                char* line = parser_advance(); // here hasMore always = true
                commandType type = parser_commandType(line);

                // dont need setfilename because codewriter functions will take filepath as argument
                char* ar1 = arg1(line, type);
                char* ar2 = arg2(line, type); 

                if (ar1 != NULL && ar2 == NULL && type == C_ARITHMETIC) // if arithmetic vm command
                    codewriter_writeArithmetic(outputfile, ar1);
                else if (ar1 != NULL && ar2 == NULL && type == C_GOTO)
                    codewriter_writeGoto(outputfile, ar1);
                else if (ar1 != NULL && ar2 == NULL && type == C_IF)
                    codewriter_writeIf(outputfile, ar1);
                else if (ar1 != NULL && ar2 == NULL && type == C_LABEL)
                    codewriter_writeLabel(outputfile, ar1);
                else if (ar1 != NULL && ar2 != NULL && (type == C_PUSH || type == C_POP))
                    codewriter_writePushPop(outputfile, filepath, type, ar1, ar2); // here 2nd arg is filepath of this iteration of outer while loop
                else if (ar1 != NULL && ar2 != NULL && type == C_FUNCTION)
                    codewriter_writeFunction(outputfile, ar1, ar2);
                else if (ar1 != NULL && ar2 != NULL && type == C_CALL)
                    codewriter_writeCall(outputfile, ar1, ar2);
                else if (ar1 == NULL && ar2 == NULL && type == C_RETURN)
                    codewriter_writeReturn(outputfile);

                free(ar1);
                free(ar2);
            }
            fclose(file);
        }
        codewriter_close(outputfile);
        // also close the dir using closedir() opened with opendirectory function 
        closedir(dir);
    }
    else // i.e argv[1] == filepath not directory
    {
        FILE* outputfile = codewriter_construct(argv[1], isdir);
        FILE* file = parser_construct(argv[1]);
        bool hasmore;
        while((hasmore = parser_hasMoreCommands(file)))
        {
            char* line = parser_advance(); // here hasMore always = true
            commandType type = parser_commandType(line);
            
            char* ar1 = arg1(line, type);
            char* ar2 = arg2(line, type); 
            if (ar1 != NULL && ar2 == NULL && type == C_ARITHMETIC) // if arithmetic vm command
                codewriter_writeArithmetic(outputfile, ar1);
            else if (ar1 != NULL && ar2 == NULL && type == C_GOTO)
                codewriter_writeGoto(outputfile, ar1);
            else if (ar1 != NULL && ar2 == NULL && type == C_IF)
                codewriter_writeIf(outputfile, ar1);
            else if (ar1 != NULL && ar2 == NULL && type == C_LABEL)
                codewriter_writeLabel(outputfile, ar1);
            else if (ar1 != NULL && ar2 != NULL && (type == C_PUSH || type == C_POP))
                codewriter_writePushPop(outputfile, argv[1], type, ar1, ar2);
            else if (ar1 != NULL && ar2 != NULL && type == C_FUNCTION)
                codewriter_writeFunction(outputfile, ar1, ar2);
            else if (ar1 != NULL && ar2 != NULL && type == C_CALL)
                codewriter_writeCall(outputfile, ar1, ar2);
            else if (ar1 == NULL && ar2 == NULL && type == C_RETURN)
                codewriter_writeReturn(outputfile);

            free(ar1);
            free(ar2);
        }
        codewriter_close(outputfile);
        fclose(file);
    }

    free(filepathBuff);
}

// check if argv[1] is a file or directory if directory then return NULL
bool checkargv1(char* arg) // arg is argv[1] on first call in main
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

    if (isdir) 
    {
        // its a dir so need to recursively return the .vm filepaths one by one
        return true;
    }
    char* ext = checkext(arg); 
    if (ext == NULL) // will stop program if the file is not a ".vm" file (defined in codewriter.h)
    {
        printf("Incorrect file type\nFilepath doesnt point to a *.vm file\n");
        exit(1);
    }
    return false;
}

DIR* opendirectory(const char* dirpath)
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
        if (checkext(name) == NULL) // i.e checkext(name) == NULL
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

static int getvmcount(const char* dirpath)
{
    DIR* dir = opendirectory(dirpath);
    struct dirent* entry;
    int count = 0;
    while((entry = readdir(dir)) != NULL)
    {
        // Only proceed if it has a valid .vm extension
        if (checkext(entry->d_name) != NULL) // i.e checkext(name) != NULL i.e a .vm file
            count++;
    }
    closedir(dir);
    return count;
}


