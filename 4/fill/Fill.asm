// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

// store the last register's address + 1. which is = RAM[l]
@l 
M = D 
@8192 
D = A 
@l 
M = M + D

(CHECK)

@SCREEN 
D = A 
@i 
M = D 

@KBD 
D = M 
@FILL
D; JNE 
 
@CLEAR 
0; JMP 

(FILL)
    @i 
    D = M 
    @l 
    D = D - M 
    @FLOOP
    D; JLT 
    @CHECK 
    0; JMP 

(FLOOP) 
    @i 
    A = M 
    M = -1
    @i 
    M = M + 1
    @FILL 
    0; JMP 

(CLEAR)
    @i 
    D = M
    @l 
    D = D - M 
    @CLOOP
    D; JLT 
    @END 
    0; JMP 

(CLOOP)
    @i 
    A = M 
    M = -1
    @i 
    M = M +1
    @CLEAR  
    0; JMP 

(END) 
    @END 
    0; JMP 