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
    
    int initial_size = 4;
    size_t size = initial_size; // this is the min. ex:- @R10
    char* line = malloc(size*sizeof(char));
    lineNode* linenode = malloc(sizeof(lineNode));
    linenode->line = line; // point the first linenode to this first line buffer.
    linenode->next = NULL;

    int c;
    char ch;
    int char_number = 1;
    int line_number = 0;
    int buffer_index = 0;
    lineNode* current_linenode = linenode;
    while((c = fgetc(fileptr)) != EOF)
    {

        // check if line size >= buffer size. >= because 
        // if i = size and next character = '\0' then
        // the last line buffer wont be '\0' terminated.
        if (char_number >= size)
        {
            size += 20;
            char* temp = realloc(line, size*sizeof(char)); // +20 the size of buffer
            if (temp == NULL)
            {
                free(line);
                return 2;
            }

            line = temp;
        }


        // check if end of a line end reached i.e '\n' (newline character) encountered 
        // then end of line. now make new node for next line and reset all counters.
        if (c == '\n')
        {
            // if no character before it 
            if (buffer_index == 0)
            {
                continue;
            }
            else
            {
                line[buffer_index] = '\0'; // make it a c string
                current_linenode->n = line_number;
                if (!(line[buffer_index - 1] == ')'))
                    line_number++;
                char_number = 1;
                buffer_index = 0;
                size = initial_size;
                line = malloc(size);
    
                lineNode* new_linenode = malloc(sizeof(lineNode));
                current_linenode->next = new_linenode;
                new_linenode->line = line;
                new_linenode->next = NULL;
                current_linenode = new_linenode;
                continue;
            }
        } 
        else if (c == ' ' || c == '\t')
        {
            continue;
        }    
        else if (c == '/')
        {

        }
        
        else
        {
            char_number++;
            buffer_index++;
            // ch = (char) c; 
            line[buffer_index] = (char) c;
        }
    }

    // but what about '\0' and no '\n' before it?deosnt matter 
    // if a string ends with two '\0' so set the last character 
    // of the last line buffer = '\0' for every asm file.


}
