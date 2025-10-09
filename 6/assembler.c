#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/*
COMMENTS:
On Windows, lines end with \r\n; on Linux and modern macOS, just \n.
To handle this consistently, we ignore \r so all line endings are treated as \n.
This avoids unexpected behavior when printing or processing lines.
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

typedef struct {
    char* sym;
    uint8_t val;
} bitval;

symval initial_symbols[] = {
    {"R0", 0},         {"R1", 1},      {"R2", 2},   {"R3", 3},
    {"R4", 4},         {"R5", 5},      {"R6", 6},   {"R7", 7},
    {"R8", 8},         {"R9", 9},      {"R10", 10}, {"R11", 11},
    {"R12", 12},       {"R13", 13},    {"R14", 14}, {"R15", 15},
    {"SCREEN", 16384}, {"KBD", 24576}, {"SP", 0},   {"LCL", 1}, 
    {"ARG", 2},        {"THIS", 3},    {"THAT", 4},
};

bitval comptable[] = {
    {"0", 0b00101010},   {"1", 0b00111111},   {"-1", 0b00111010}, 
    {"D", 0b00001100},   {"A", 0b00110000},   {"!D", 0b00001101}, 
    {"!A", 0b00110001},  {"-D", 0b00001111},  {"-A", 0b00110011}, 
    {"D+1", 0b00011111}, {"A+1", 0b00110111}, {"D-1", 0b00001110}, 
    {"A-1", 0b00110010}, {"D+A", 0b00000010}, {"D-A", 0b00010011}, 
    {"D&A", 0b00000000}, {"D|A", 0b00010101}, {"M", 0b01110000}, 
    {"!M", 0b01110001},  {"-M", 0b01110011},  {"M+1", 0b01110111}, 
    {"M-1", 0b01110010}, {"D+M", 0b01000010}, {"D-M", 0b01010011}, 
    {"M-D", 0b01000111}, {"D&M", 0b01000000}, {"D|M", 0b01010101}, 
};

bitval jumptable[] = {
    {"JGT", 0b00000001}, {"JEQ", 0b00000010}, {"JGE", 0b00000011}, 
    {"JLT", 0b00000100}, {"JNE", 0b00000101}, {"JLE", 0b00000110}, 
    {"JMP", 0b00000111}, 
};

bitval desttable[]= {
    {"M", 0b00000001}, {"D", 0b00000010},  {"MD", 0b00000011}, 
    {"A", 0b00000100}, {"AM", 0b00000101}, {"AD", 0b00000110}, 
    {"AMD", 0b00000111}, 
};

lineNode* fileopen(int arg_no ,char* filename);
symval* mksymbltabl(symval** tableptr, char* sym, int val, size_t* size);
lineNode* firstpass(lineNode* head, symval** tableptr, size_t* sizeoftabl);
lineNode* secondpass(lineNode* head, symval** tableptr, size_t* sizeoftabl);
void outputandfree(char* filename, lineNode* head, symval* table, size_t* sizeoftabl);

int main(int argc, char* argv[])
{
    // open the file & load all the lines 
    lineNode* lines = fileopen(argc, argv[argc - 1]);

    // initialize the table
    size_t stsize;
    symval* sthead = NULL;
    sthead = mksymbltabl(&sthead, NULL, 0, &stsize);

    // first pass 
    lines = firstpass(lines, &sthead, &stsize); 

    // second pass 
    lines = secondpass(lines, &sthead, &stsize); 
    
    // output the instructions to a file and free the line list and the symbol table
    outputandfree(argv[argc-1], lines, sthead, &stsize);
    
}

void outputandfree(char* filename, lineNode* head, symval* table, size_t* sizeoftabl)
{
    uint16_t len = strlen(filename); // give number of non null characters
    char* ext = strrchr(filename, '.'); // will return pointer to the '.'
    if (len < 5 || ext == NULL || strcmp(ext, ".asm") != 0) // order of '||' matters here else segmentation fault
    {
        printf("Incorrect input file\nUsage:- ./assembler filename.asm");
        exit(1);
    }
    if (ext) // i.e ext != NULL
    {
        *ext = '\0'; // put a '\0' at the place of the '.'
    }
    
    char* outputfile = malloc(strlen(filename) + strlen(".hack") + 1);
    strcpy(outputfile, filename);
    strcat(outputfile, ".hack");

    FILE* output = fopen(outputfile, "w");
    if (output == NULL) {exit(1);}

    lineNode* current = head;
    while(current != NULL)
    {
        for (int i = 15; i >= 0; i--) 
        {
            char bit = (current->instruction >> i) & 1;
            fputc(bit + '0', output); // convert numberic value 0/1 to ascii 
        }
        fputc('\n', output); // On Windows, when you write \n to a file opened in text mode, 
                             // the C runtime automatically converts \n to a CR+LF sequence 
                             // ("\r\n", bytes 13 and 10).
        current = current->next;
    }
    fclose(output); 
    free(outputfile);

    // free the linked list of lines 
    current = head;
    lineNode* tmp;
    while (current != NULL)
    {
        free(current->line);
        tmp = current->next;
        free(current);
        current = tmp;
    }

    // free the symbol table 
    for (size_t i = 0; i < *sizeoftabl; i++) 
    {
        free(table[i].sym);
    }
    free(table);  // free the whole array
}

lineNode* secondpass(lineNode* head, symval** tableptr, size_t* sizeoftabl)
{
    symval* table = *tableptr;
    size_t var_n = 16; // variable number
    lineNode* current_linenode = head;
    while (current_linenode != NULL)
    {
        
        // lets do the "a" instruction part first 
        if (current_linenode->line[0] == '@')
        {
            int i = 1;
            while(current_linenode->line[i] != '\0')
                i++;

            char* sym = malloc(i*sizeof(char));
            for(int j = 0; j < i; j++)
                sym[j] = current_linenode->line[j+1];

            // now sym contains the variable or label or predef symbol name 

            // check if all digits or no  
            bool isdigit = true;
            size_t len = strlen(sym);
            for (int i = 0; i < len; i++)
            {
                if (sym[i] >= '0' && sym[i] <= '9')
                    continue;
                else
                {
                    isdigit = false;
                    break;
                }
            }

            uint16_t binary;
            bool intable = false;
            // now translate and put in symbol table according to isdigit 

            if (isdigit) // if its decimal value just translate and set instruction for this node and move to next linenode
            {
                binary = atoi(sym);
                current_linenode->instruction = binary & 0x7FFF; // & 0111 1111 1111 1111 to mask the bit15

                current_linenode = current_linenode->next;
                free(sym);
                continue;
            }
            else // check if already in symbol table if not put it in and translate and set instruction & move to next linenode
            {
                // if in table then tranaslate 
                for (int i = 0; i < *sizeoftabl; i++)
                {
                    if (strcmp(table[i].sym, sym) == 0)
                    {
                        binary = table[i].val;
                        intable = true;
                        break;
                    }
                }

                if (!intable)
                {
                    // put it in table 
                    *tableptr = mksymbltabl(tableptr, sym, var_n, sizeoftabl);
                    table = *tableptr;
                    // translate 
                    binary = var_n;
                    var_n++;
                } 

                // SET INSTRUCTION AND MOVE TO NEXT LINENODE:
                current_linenode->instruction = binary & 0x7FFF; 
                current_linenode = current_linenode->next;
            }
            continue;
        }

        // now do "c" instruction 
        else
        {
            uint8_t dest = 0;
            uint8_t comp = 0;
            uint8_t jump  = 0;
            current_linenode->instruction |= 0b111 << 13; // 0000000000000000 | 1110000000000000

            uint8_t size = strlen(current_linenode->line) + 1;
            char* sym = malloc(size * sizeof(char));
            if (sym == NULL) {exit(1);}
            char* D = malloc(4*sizeof(char));
            if (D == NULL) {exit(1);}
            memset(D, '0', 3);
            D[3] = '\0';
            char* C = malloc(4*sizeof(char));
            if (C == NULL) {exit(1);}
            memset(C, '0', 3);
            C[3] = '\0';
            char* J = malloc(4*sizeof(char));
            if (J == NULL) {exit(1);}
            memset(J, '0', 3);
            J[3] = '\0';
            bool ifdest = false;
            bool ifjump = false;

            // TOKENISE:
            for (uint8_t i = 0; i < size; i++)
            {
                sym[i] = current_linenode->line[i];

                if (current_linenode->line[i] == '=')
                {
                    memcpy(D, sym, i); // memcpy doesnt copy ith byte
                    D[i] = '\0'; // because D might contain less than 3 characters.
                    // ready sym for the comp field 
                    memset(sym, 'd', i+1); // we will ignore all 'd' as they represent dest field
                    ifdest = true;
                }
                else if (current_linenode->line[i] == ';' && ifdest)
                {
                    uint8_t j = 0;
                    while(sym[j] == 'd')
                        j++;

                    memcpy(C, sym + j, i-j);
                    if ((i - j) < 4) C[i - j] = '\0';
                    // ready sym for jump field 
                    memset(sym + j, 'c', i-(j-1));
                    ifjump = true;
                }
                else if (current_linenode->line[i] == '\0' && ifdest && ifjump)
                {
                    uint8_t j = 0;
                    while(sym[j] == 'd' || sym[j] == 'c')
                        j++;

                    memcpy(J, sym + j, i-j); 
                    // **No need to null terminate is jump field is always 3bytes still good practice to write
                    if ((i - j) < 4) J[i - j] = '\0';
                    // and then loop will automatically stop 
                }
                else if (current_linenode->line[i] == '\0' && !(ifdest) && ifjump)
                {
                    uint8_t j = 0;
                    while(sym[j] == 'c')
                        j++;

                    memcpy(J, sym + j, i-j);
                    if ((i - j) < 4) J[i - j] = '\0';
                }
                else if (current_linenode->line[i] == '\0' && ifdest && !(ifjump))
                {
                    uint8_t j = 0;
                    while(sym[j] == 'd')
                        j++;

                    memcpy(C, sym + j, i-j);
                    if ((i - j) < 4) C[i - j] = '\0';
                }
                else if (current_linenode->line[i] == ';' && !(ifdest))
                {
                    memcpy(C, sym, i);
                    if (i < 4) C[i] = '\0';
                    // ready sym for jump field
                    memset(sym, 'c', i+1);
                    ifjump = true;
                }
                else if (current_linenode->line[i] == '\0' && !(ifdest) && !(ifjump))
                {
                    memcpy(C, sym, i);
                    if (i < 4) C[i] = '\0';
                }
            }
            free(sym);
            // SET INSTRUCTION FOR THIS NODE:
            uint8_t desttable_n = sizeof(desttable) / sizeof(desttable[0]);
            uint8_t comptable_n = sizeof(comptable) / sizeof(comptable[0]);
            uint8_t jumptable_n = sizeof(jumptable) / sizeof(jumptable[0]);

            // translate the tokens
            for (uint8_t i = 0; i < desttable_n; i++)
            {
                if (strcmp(desttable[i].sym, D) == 0)
                    dest = desttable[i].val;
            }
            for (uint8_t i = 0; i < comptable_n; i++)
            {
                if (strcmp(comptable[i].sym, C) == 0)
                    comp = comptable[i].val;
            }
            for (uint8_t i = 0; i < jumptable_n; i++)
            {
                if (strcmp(jumptable[i].sym, J) == 0)
                    jump = jumptable[i].val;
            }

            free(D);
            free(C);
            free(J);

            // set the instruction
            current_linenode->instruction |= comp << 6;
            current_linenode->instruction |= dest << 3;
            current_linenode->instruction |= jump;

            // move to next linenode
            current_linenode = current_linenode->next;
        }
    }
    return head;
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
            table[i].sym = strdup(initial_symbols[i].sym); // strdup does memory allocation & string duplication
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
    return linenode; // return head of list
}
