#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
IMP COMMENTS

On windows to end a line "\r\n" is used (carriage return and linefeed)
On linux and modern macs end of line is denoted by "\n" (just linefeed)
To handle this difference we ignore \r altogether so that all line ends
are denoted by "\n". so when u go to print the lines and other data along
with it doesnt show weird behaviour.
*/

typedef struct lineNode {
    
    char* line;
    int n; // linenumber
    struct lineNode* next;
} lineNode; 


lineNode* fileopen(char* filename);

// open the .asm file and read and store in a linked list & return ptr to this linked list.
lineNode* fileopen(char* filename)
{
    FILE* fileptr = fopen(filename, "r");
    if(fileptr == NULL)
        return NULL;
    
    int initial_size = 4;
    size_t size = initial_size;
    char* line = malloc(size*sizeof(char));
    if (line == NULL)
        return NULL;
    lineNode* linenode = malloc(sizeof(lineNode));
    if (linenode == NULL)
    {
        free(line);
        return NULL;
    }
    linenode->line = line; // point the first linenode to this first line buffer.
    linenode->next = NULL;

    int c;
    int char_number = 1;
    int line_number = 0;
    int buffer_index = 0;
    lineNode* current_linenode = linenode;
    bool comment = false;
    while((c = fgetc(fileptr)) != EOF)
    {

        // check if line size >= buffer size. >= because if i = size and next character is
        // = '\0' then the last line buffer wont be '\0' terminated.
        if (char_number >= size)
        {
            size += 10;
            char* temp = realloc(line, size*sizeof(char)); // +10 the size of buffer
            if (temp == NULL)
            {
                free(line);
                return NULL;
            }

            line = temp;
            current_linenode->line = line;
        }

        if (c == '/')
        {
            comment = true;
            continue;
        }

        if (c == '\r' || c == ' ' || c == '\t' || (c != '\n' && comment))
            continue;  

        // check if end of a line end reached i.e '\n' (newline character) encountered 
        // then end of line. now make new node for next line and reset all counters.
        else if (c == '\n')
        {

            comment = false;

            // if no character before it 
            if (buffer_index == 0)
                continue; 

            else
            {
                line[buffer_index] = '\0'; // make it a c string
                current_linenode->n = line_number;

                if (!(line[buffer_index - 1] == ')'))
                    line_number++;
                    
                // if next char if eof font make new node. Break
                int next = fgetc(fileptr);
                if (next == EOF)
                    break;
                else
                    ungetc(next, fileptr); // return the next char if its not EOF
                                           // ungetc cant return EOF to fileptr
                char_number = 1;
                buffer_index = 0;
                size = initial_size;
                
                char* tmp = malloc(size*sizeof(char));
                if (tmp == NULL)
                    return NULL;

                line = tmp;

                lineNode* new_linenode = malloc(sizeof(lineNode));
                if (new_linenode == NULL)
                {
                    free(line);
                    lineNode* freeptr = linenode;
                    while(freeptr != NULL)
                    {
                        linenode = freeptr->next;
                        free(freeptr->line);
                        free(freeptr);
                        freeptr = linenode;
                    }
                    return NULL;
                }
                new_linenode->line = line;
                new_linenode->next = NULL;
                current_linenode->next = new_linenode;
                current_linenode = new_linenode;
                continue;
            }
        } 

        else
        {
            line[buffer_index] = c;
            char_number++;
            buffer_index++;
        }
    }
    fclose(fileptr);
    return linenode; // return head of list
}
