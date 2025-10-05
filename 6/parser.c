#include <stdio.h>



typedef struct lineNode {

    char* line;
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
    
    int initial_size = 4 * sizeof(char);
    size_t size = initial_size; // this is the min. ex:- @R10
    char* line = malloc(size);
    lineNode* linenode = malloc(sizeof(lineNode));
    linenode->line = line; // point the first linenode to this first line buffer.
    linenode->n = 1;
    linenode->next = NULL;

    int c;
    char ch;
    int i = 1;
    int buffer_index = 0;
    lineNode* current_linenode = linenode;
    while((c = fgetc(fileptr)) != EOF)
    {
        // check if end of a line end reached i.e '\n' (newline character) encountered then end of line
        if (c == '\n')
        {
            line[buffer_index] = '\0'; // make it a c string

            i = 1;
            size = initial_size;
            line = malloc(size);

            lineNode* new_linenode = malloc(sizeof(lineNode));
            current_linenode->next = new_linenode;
            (new_linenode)->line = line;
            new_linenode->n = current_linenode->n + 1;
            new_linenode->next = NULL;
            current_linenode = new_linenode;
            
            buffer_index = 0;
            continue;
        }
        
        // check if line size > buffer size
        if (i*sizeof(char) > size)
        {
            size *= 2;
            char* temp = realloc(line, size); // 2x the size of buffer
            if (temp == NULL)
            {
                free(line);
                return 2;
            }

            line = temp;
        }


        i++;
        buffer_index = i-1;
        ch = (char) c; 
        line[buffer_index] = ch;
    }
}
