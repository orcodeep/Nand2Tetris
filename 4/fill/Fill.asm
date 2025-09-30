// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

@SCREEN 
D = A 
// store the last register's address + 1. which is = RAM[l]
@l 
M = D 
@8192 
D = A 
@l 
M = M + D

(START)
@SCREEN 
D = A 
@i 
A = D 

(CHECK)
@KBD 
D = M 
@FILL
D; JNE 

@SCREEN 
D = A 
@i 
A = D 
@CLEAR 
0; JMP 

(FILL)
    @i 
    D = A
    @l 
    D = D - M 
    @FLOOP
    D; JLT 
    @START 
    0; JMP 

(FLOOP) 
    @i 
    M = -1
    A = A + 1
    @CHECK 
    0; JMP 

(CLEAR)
    @i 
    D = A
    @l 
    D = D - M 
    @CLOOP
    D; JLT 
    @START 
    0; JMP 

(CLOOP)
    @i 
    M = 0
    A = A + 1
    @CLEAR  
    0; JMP 

(END) 
    @END 
    0; JMP 