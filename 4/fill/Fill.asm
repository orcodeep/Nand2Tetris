// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

// store the last register's address + 1. which is = RAM[l]
@SCREEN 
D = A 
@l 
M = D 
@8192 
D = A 
@l 
M = M + D

(CHECKKBD) 

    @SCREEN 
    D = A 
    @i 
    M = D

    @KBD 
    D = M 
    @FILL 
    D; JNE 

    @IFCLEAR 
    0; JMP 

(FILL) 
    @i 
    A = M 
    M = -1 

    @i 
    M = M + 1 
    @FLOOP 
    0; JMP 

(FLOOP)
    @i 
    D = M 
    @l 
    D = D - M 
    @FILL 
    D; JLT 

    @CHECKKBD 
    0; JMP 

(IFCLEAR)
    @SCREEN
    D = M 
    @CLEAR 
    D; JNE 

    @CHECKKBD 
    0; JMP 

(CLEAR) 
    @i 
    A = M 
    M = 0 

    @i 
    M = M + 1 
    @CLOOP 
    0; JMP 

(CLOOP) 
    @i 
    D = M 
    @l 
    D = D - M 
    @CLEAR 
    D; JLT 

    @CHECKKBD 
    0; JMP  