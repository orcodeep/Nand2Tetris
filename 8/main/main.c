#include "main.h"

static int getvmcount(const char* dirpath);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Wrong usage\nuse: ./vmtranslator filename.vm\n");
        return 1;
    }

    char* path = checkargv1(argv[1]);
    bool isdir = false;

    if (path == NULL) // i.e a directory
    {
        isdir = true;
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

            }
            fclose(file);
        }
        codewriter_close(outputfile);
        // also close the dir using closedir() opened with opendirectory function 
        closedir(dir);


    }
    else // i.e argv[1] == filepath not directory
    {
        isdir = false;
        FILE* outputfile = codewriter_construct(path, isdir);
        FILE* file = parser_construct(path);
        bool hasmore;
        while((hasmore = parser_hasMoreCommands(file)))
        {

        }
        codewriter_close(outputfile);
        fclose(file);
    }

    free(filepathBuff);
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


