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
    
    size_t size = 2 * sizeof(char); 
    char* buffer = malloc(size);

    int c;
    char ch;
    int i = 1;
    int buffer_index;
    while((c = fgetc(fileptr)) != EOF)
    {
        
        if (i > size)
        {
            char* temp = realloc(buffer, i*sizeof(char));
            if (temp == NULL)
            {
                free(buffer);
                return 2;
            }
            buffer = temp;
        }



        buffer_index = i-1;
        ch = (char) c; 
        buffer[buffer_index] = ch;



        i++;
    }
}
