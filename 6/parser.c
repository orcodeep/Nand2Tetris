#include <stdio.h>
#include <stdbool.h>


typedef struct lineNode {

    char* line;
    int n; // linenumber
    struct lineNode* next;
} lineNode; 


int fileopen(char* filename);


// open the .asm file and read and store in a linked list & return ptr to this linked list.
int fileopen(char* filename)
{
    FILE* fileptr = fopen(filename, "r");
    if(fileptr == NULL)
        return 1;
    
    int initial_size = 4;
    size_t size = initial_size; // this is the min. ex:- @R10
    char* line = malloc(size*sizeof(char));
    if (line == NULL)
    {
        return 2;
    }
    lineNode* linenode = malloc(sizeof(lineNode));
    if (linenode == NULL)
    {
        free(line);
        return 2;
    }
    linenode->line = line; // point the first linenode to this first line buffer.
    linenode->next = NULL;

    int c;
    char ch;
    int char_number = 1;
    int line_number = 0;
    int buffer_index = 0;
    int lastbi;             // last buffer_index 
    lineNode* current_linenode = linenode;
    bool comment_text = false;
    while((c = fgetc(fileptr)) != EOF)
    {

        // check if line size >= buffer size. >= because if i = size and next character is
        // = '\0' then the last line buffer wont be '\0' terminated.
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
            current_linenode->line = line;
        }
        if (c == '/')
        {
            comment_text = true;
        }
 
        if (c == ' ' || c == '\t' || (c != '\n' && comment_text))
        {
            continue;
        }    
        // check if end of a line end reached i.e '\n' (newline character) encountered 
        // then end of line. now make new node for next line and reset all counters.
        else if (c == '\n')
        {
            comment_text = false;
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
                char* tmp = realloc(line, size*sizeof(char));
                if (tmp == NULL)
                {
                    return 2;
                }
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
                    return 2;
                }
                new_linenode->next = NULL;
                current_linenode->next = new_linenode;
                new_linenode->line = line;
                current_linenode = new_linenode;
                continue;
            }
        } 
        else
        {
            // ch = (char) c; 
            line[buffer_index] = (char) c;
            char_number++;
            buffer_index++;
            lastbi = buffer_index;
        }
    }
    // but what about '\0' with no '\n' before it? we just write it to tht index of EOF.
    // if there was \n before \0 we just rewrite the \0 written by \n in the line.
    line[lastbi] = '\0';


}
