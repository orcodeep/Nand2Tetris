#include <stdio.h>

typedef struct line {
    char* buffer;
}line;

typedef struct lineNode {

    line* line;
    int n; // linenumber
    struct lineNode* next;
} lineNode; 


int fileopen(char* filename);





// first open the .asm file and read and store in linked list then return ptr to 
// this linked list
int fileopen(char* filename)
{
    FILE* fileptr = fopen(filename, "r");
    if(fileptr == NULL)
        return 1;
    
    char* buffer = malloc(2 * sizeof(char));

    int c;
    char ch;
    while((c = fgetc(fileptr)) != EOF)
    {
        ch = (char) c;
    }
}
