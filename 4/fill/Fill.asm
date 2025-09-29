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
@i 
A = D 

// store the last register's address + 1. which is = RAM[l]
@l 
M = D 
@8192 
D = A 
@l 
M = M + D

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
    @SCREEN 
    D = A 
    @i 
    A = D

(FLOOP) 
    @i 
    M = 1
    @16 
    D = A 
    @i 
    A = D + A 
    @FILL 
    0; JMP 

(CLEAR)
    @i 
    D = A
    @l 
    D = D - M 
    @CLOOP
    D; JLT 
    @END 
    0; JMP 

(CLOOP)
    @i 
    M = 1
    @16 
    D = A 
    @i 
    A = D + A 
    @CLEAR  
    0; JMP 

(END) 
    @END 
    0; JMP 