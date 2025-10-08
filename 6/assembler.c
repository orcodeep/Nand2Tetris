#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>


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
    uint16_t instruction;
    struct lineNode* next;
} lineNode; 

typedef struct {
    char* sym;
    size_t val;
} symval;

symval initial_symbols[] = {
    {"R0", 0},
    {"R1", 1},
    {"R2", 2},
    {"R3", 3},
    {"R4", 4},
    {"R5", 5},
    {"R6", 6},
    {"R7", 7},
    {"R8", 8},
    {"R9", 9},
    {"R10", 10},
    {"R11", 11},
    {"R12", 12},
    {"R13", 13},
    {"R14", 14},
    {"R15", 15},
    {"SCREEN", 16384},
    {"KBD", 24576},
    {"SP", 0},
    {"LCL", 1},
    {"ARG", 2},
    {"THIS", 3},
    {"THAT", 4},
};


int total_instructions = 0;

lineNode* fileopen(int arg_no ,char* filename);
symval* mksymbltabl(symval** tableptr, char* sym, int val, size_t* size);
lineNode* firstpass(lineNode* head, symval** tableptr, size_t* sizeoftabl);
int secondpass(lineNode* head, symval** tableptr, size_t* sizeoftabl);

int main(int argc, char* argv[])
{
    // open the file & load all the lines 
    lineNode* lines = fileopen(argc, argv[argc - 1]);

    // initialize the table
    size_t stsize;
    symval* sttable = NULL;
    sttable = mksymbltabl(&sttable, NULL, 0, &stsize);

    // first pass 
    lines = firstpass(lines, &sttable, &stsize); 

    // second pass 
    // lines = secondpass(lines, &sttable, &stsize);
    
}

int secondpass(lineNode* head, symval** tableptr, size_t* sizeoftabl)
{
    uint8_t dest = 0;
    uint8_t comp = 0;
    uint8_t jmp  = 0;

    // go to each linenode and check if its an "a" instruction or "c" instruction
    // if its "a" check if its value exists in symbol table if yes just translate 
    // its decimal value to binary, if no then put its value 16 onwards in the
    // symbol table and then translate. 

    // if its a c instruction then translate according to tokens.
}






lineNode* firstpass(lineNode* head, symval** tableptr, size_t* sizeoftabl)
{
    lineNode* current_linenode = head;
    lineNode* tmp = current_linenode;
    while(current_linenode != NULL)
    {
        if (current_linenode->line[0] != '(')
        {
            tmp = current_linenode;
            current_linenode = current_linenode->next;
            continue;
        }

        // if label detected
        int i = 1;
        while(current_linenode->line[i] != ')')
            i++;

        char* label = malloc(i*sizeof(char));
        for(int j = 0; j < i-1; j++)
            label[j] = current_linenode->line[j+1];

        label[i-1] = '\0'; // now label contains the label symbol 

        // now enter this symbol in the symbol table 
        *tableptr = mksymbltabl(tableptr, label, current_linenode->n, sizeoftabl);
        free(label);

        // free this node 
        if (current_linenode == head)
        {
            head = current_linenode->next;
            tmp = current_linenode;
            current_linenode = head;
            free(tmp);
            tmp = current_linenode;
        }
        else
        {
            tmp->next = current_linenode->next;
            free(current_linenode);
            current_linenode = tmp->next;
        }
    }
    return head;
}

symval* mksymbltabl(symval** tableptr, char* sym, int val, size_t* size)
{
    symval* table;
    if (*tableptr == NULL)
    {
        // initialise symbol table
        *size = sizeof(initial_symbols) / sizeof(initial_symbols[0]);
        table = malloc((*size)*sizeof(symval));
        if (table == NULL)
            exit(1);
            
        for (size_t i = 0; i < *size; i++)
        {
            table[i].sym = strdup(initial_symbols[i].sym);
            if (table[i].sym == NULL) { exit(1); }
            table[i].val = initial_symbols[i].val;
        }
        *tableptr = table;
    } 
    table = *tableptr;
    
    // now we are sure that a table exists so put in the new symbols if they given 
    if (sym != NULL)
    {
        *size += 1;
        symval* tmp = realloc(table, (*size)*sizeof(symval)); // realloc takes care of freeing the prev chunk
        if (tmp == NULL) { exit(1); }
        table = tmp;
        *tableptr = table;
        table[*size - 1].sym = strdup(sym);
        table[*size - 1].val = val;
    }

    return table;
}

// open the .asm file and read and store in a linked list & return ptr to this linked list.
lineNode* fileopen(int arg_no, char*filename)
{
    if (arg_no != 2)
    {
        printf("Wrong usage\nuse: ./assembler filename.c\n");
        exit(1);
    }

    FILE* fileptr = fopen(filename, "r");
    if(fileptr == NULL)
    {
        printf("File Not Found\n");
        exit(1);
    }
        
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
    int buffer_index = 0;
    int line_number = 0;
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
                current_linenode->instruction = 0; // for now the actual binary instruction = 0

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

                lineNode* new_linenode = malloc(sizeof(linenode));
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
    total_instructions = line_number;
    return linenode; // return head of list
}
